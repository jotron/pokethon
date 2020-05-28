#!/usr/bin/env python
# coding: utf-8

# # Neural Network Model

# Comments:
#    - We solve the problem with a neural network. Other approaches like Random Forests were considered but were to hard to train.
#    - We manually experimented with various hyperparameters, and this is the best performing setup.
#    - The selection of the actual team happens in a separate file.

# In[12]:


import tensorflow as tf
from tensorflow import keras
import tensorflow.keras.layers as layers
from tensorflow.keras.utils import plot_model

import sys
import numpy as np
import os
import matplotlib.pyplot as plt
import pandas as pd

# For reproducability across runs
np.random.seed(42)
tf.random.set_seed(42)

import sklearn
from sklearn.model_selection import train_test_split
from sklearn.compose import ColumnTransformer
from sklearn.pipeline import Pipeline
from sklearn.base import BaseEstimator, TransformerMixin
from sklearn.preprocessing import StandardScaler, OneHotEncoder, OrdinalEncoder
from sklearn.pipeline import make_pipeline, Pipeline

# Relativ Path to folder with data structure (CHANGE FROM NOTEBOOK !!!)
data_path = 'data/'


# ### Load Data

# In[13]:

print ("Load Data...")
Submission = pd.read_csv(data_path+'01_raw/Submission.csv', delimiter='|')
AvailablePokemons = pd.read_csv(data_path+'01_raw/AvailablePokemons.csv', delimiter='|')
BattleResults = pd.read_csv(data_path+'01_raw/Battle_Results.csv', delimiter='|')
Weakness_Pokemon = pd.read_csv(data_path+'01_raw/Weakness_Pokemon.csv', delimiter='|')
AllPokemons = pd.read_csv(data_path+'01_raw/All_Pokemons.csv', delimiter='|')


# ### Data Pipeline Auxilliary Functions

# In[14]:


def RelativeResult(df):
    """Return array with Battle Outcomes scaled to [-1, 1]"""
    HP = df.apply(lambda row: row["HP_1"] if row["BattleResult"] > 0 else row["HP_2"], axis=1).values
    relativized = df["BattleResult"].values / HP
    return relativized


# In[15]:


class LevelScaler(BaseEstimator, TransformerMixin):
    """Return Level scaled to range [0,1]"""
    def __init__(self): 
        pass
    def fit(self, X, y=None):
        return self  
    def transform(self, X):
        return X / 99


# In[16]:


class AddPokemonTypes(BaseEstimator, TransformerMixin):
    """Add Pokemon Types to the Dataframe, e.g. 'Fire'"""
    def __init__(self): 
        # Column names. Where Type2_1 is the second type of the first pokemon
        self.type_attributes = ['Type1_1', 'Type2_1', 'Type1_2', 'Type2_2']
        # Dictionnary for fast access
        self.NametoIndex = {sys.intern(name): AllPokemons.loc[AllPokemons["Name"]==name].iloc[0].ID-1 for name in AllPokemons.Name}
        
    def fit(self, X, y=None):
        return self  
    
    def transform(self, df):
        # Copy Dataframe
        df = df.copy()
        # Directly work on underlying data
        values = df.values
        # Create array of shape (n, 2) describing pokemon types for convenience
        AllPokemonTypes = AllPokemons.loc[:,['Type_1', 'Type_2']]
        AllPokemonTypes.Type_2 = AllPokemons.Type_2.fillna(AllPokemons.Type_1)
        AllPokemonTypes = AllPokemonTypes.values
        
        # Old Column indexes
        name1_idx = df.columns.get_loc("Name_1")
        name2_idx = df.columns.get_loc("Name_2")

        # New Columns
        Types = np.empty((len(df),4), dtype='U15')

        # Go through all rows
        for i in range(len(df)):
            idx_1 = self.NametoIndex[values[i,name1_idx]]
            idx_2 = self.NametoIndex[values[i,name2_idx]]

            # Types
            Types[i, 0] = AllPokemonTypes[idx_1, 0]
            Types[i, 1] = AllPokemonTypes[idx_1, 1]
            Types[i, 2] = AllPokemonTypes[idx_2, 0]
            Types[i, 3] = AllPokemonTypes[idx_2, 1]

        
        # Add columns
        df[self.type_attributes] = pd.DataFrame(Types, index=df.index)
        return df


# In[17]:


def DuplicateData(df, y):
    """
    Add inverse data to the dataframe for data augmentation. For every row 
        (pokemon1_stats, pokemon2_stats, result)
    add the row
        (pokemon2_stats, pokemon1_stats, (-1) * result).
    """
    df_inverse = df.copy()
    y_inverse = y.copy()
    
    # Inverse y
    y_inverse = (-1) * y_inverse
    
    # Inverse df
    swap_attributes = ['Name', 'Level', 'HP', 'Attack', 'Defense', 'Sp_Atk', 'Speed', 'Legendary', 'Price']
    for attr in swap_attributes:
        df_inverse.loc[:, [f'{attr}_1', f'{attr}_2']] = df.loc[:, [f'{attr}_2', f'{attr}_1']].values
    df_inverse.loc[:, 'BattleResult'] = - df.loc[:, 'BattleResult'] # just for consistency
    
    # Merge and Shuffle
    df_duplicated = df.append(df_inverse)
    y_duplicated = np.concatenate([y, y_inverse], axis=0)
    df_duplicated, y_duplicated = sklearn.utils.shuffle(df_duplicated, y_duplicated, random_state=42)
    
    return df_duplicated, y_duplicated


# ### Data Pipeline

# In[18]:


# Scale labels
print ("Scale y...")
y = RelativeResult(BattleResults)


# In[19]:


# Augment training data
print ("Augment data...")
df_augm_x, augm_y = DuplicateData(BattleResults, y)


# In[22]:


# Define preprocessing pipeline and fit sklearn scalers to data
numerical_attributes = ['HP_1', 'Attack_1', 'Defense_1', 'Sp_Atk_1', 'Sp_Def_1', 'Speed_1', 
                          'HP_2', 'Attack_2', 'Defense_2', 'Sp_Atk_2', 'Sp_Def_2', 'Speed_2']
type_attributes = ['Type1_1', 'Type2_1', 'Type1_2', 'Type2_2']
TypeEncoder = OrdinalEncoder()

full_pipeline = Pipeline([
    ("Add Types", AddPokemonTypes()),
    ("Individual Feature Preprocessing", ColumnTransformer([
        ("Drop", "drop", ['Name_1', 'Name_2', 'Price_1', 'Price_2', 'BattleResult']),
        ("Numerical Attributes", StandardScaler(), numerical_attributes), # Faulty when not using augmented data
        ("Boolean", "passthrough", ['Legendary_1', 'Legendary_2']),
        ("Level" , LevelScaler(), ['Level_1', 'Level_2']),
        ("Weather", OrdinalEncoder(), ['WeatherAndTime']),
        ("Types", TypeEncoder, type_attributes)
    ])),
])
full_pipeline.fit(df_augm_x);


# In[23]:


# Apply transformation (CHANGE FROM NOTEBOOK !!!)
print ("Transform data...")
augm_x = full_pipeline.transform(df_augm_x)


# ### Functional Model

# In[24]:


# Functional Model Architecture
# Categorical Data is embedded. Same embedding is used for Pokemon First Nature and Second Nature (Type).
""" Input columns are as follows:
- input[0:16] are numerical attributes
- input[16] is Ordinal WeatherAndTime
- input[17] is Ordinal Pokemon_1 First Nature
- input[18] is Ordinal Pokemon_1 Second Nature
- input[19] is Ordinal Pokemon_2 First Nature
- input[20] is Ordinal Pokemon_2 Second Nature
"""
model_input = layers.Input(shape=(21,), name='ModelInput')

# Embeddings
type_emb = layers.Embedding(input_dim=18, output_dim=8, name="Pokemon1Type")
type1_emb_1 = type_emb(model_input[:,17])
type2_emb_1 = type_emb(model_input[:,18])
type1_emb_2 = type_emb(model_input[:,19])
type2_emb_2 = type_emb(model_input[:,20])
weather_emb = layers.Embedding(input_dim=5, output_dim=3, name="WeatherEmbedding")(model_input[:, 16])

# Linear Network
x = layers.Concatenate()([model_input[:, 0:16], weather_emb, type1_emb_1, type2_emb_1, type1_emb_2, type2_emb_2])
x = layers.Dense(200, activation='relu')(x)
x = layers.Dense(200, activation='relu')(x)
x = layers.Dense(200, activation='relu')(x)
model_output = layers.Dense(1, activation='tanh')(x)

model = keras.Model(inputs=model_input, outputs=model_output)

# plot_model(model, show_shapes=True)


# In[29]:


def train_model(learning_rate, batch_size, epochs):
    """Train model with specific learning rate and batch_size"""
    model.compile(loss='mse',
                  optimizer=keras.optimizers.RMSprop(learning_rate=learning_rate),
                  metrics=['RootMeanSquaredError'])
    
    history = model.fit(augm_x, augm_y, epochs=epochs,
                        batch_size=batch_size,
                        verbose=2)
    return history


# ### Train

# In[30]:


train_model(3e-3, 1024, 12)


# In[31]:


train_model(1e-5, 64, 2)


# ### Load/Save Model

# In[ ]:


savemodel = True
loadmodel = False
if (savemodel): model.save(data_path+'02_models/final_model', overwrite=False)
if (loadmodel): model = keras.models.load_model(data_path+'02_models/final_model')


# ### Generate Output

# In[33]:


def create_inference_data():
    """
    Generate Inference data to predict all battles between available pokemons and adversary pokemons.
    Return the same Format as BattleResults with additional PokemonID column.
    """
    # Repeat every row 6 times (there are 6 enemy pokemons)
    merged = pd.DataFrame(np.repeat(AvailablePokemons.values,6,axis=0), columns=AvailablePokemons.columns)
    # Drop empty PokemonID from Submission
    Submission_dropped = Submission.drop(columns=['SelectedPokemonID'])
    # Concat with Submission
    merged = merged.apply(lambda row: pd.concat([row, Submission_dropped.iloc[row.name%6]]), axis=1)
    return merged

def export(predictions):
    """
    Generate comprehensive table describing the outcome of the inference battles.
    """
    predictions = predictions.reshape(-1)
    table = np.empty((len(AvailablePokemons), 7))
    for i in range(len(AvailablePokemons)):
        table[i, 0] = i+1
        table[i, 1:] = predictions[i*6: (i+1)*6]
    df = pd.DataFrame(data=table, columns=['SelectedPokemonID']+list(Submission.Name_2.values))
    return df.astype({'SelectedPokemonID': int})


# In[34]:


# Create Inference Data
inference_data = create_inference_data()


# In[35]:


# Transform Data, Predict outcomes, Export to csv
save_inference = True

x_inference = full_pipeline.transform(inference_data)
inference_predictions = model.predict(x_inference)
output = export(inference_predictions)

if (save_inference): output.to_csv(path_or_buf=data_path+'03_model_output/final_model/inference.csv')


# ### Evaluate on test set

# In[36]:

print(model.evaluate(augm_x, augm_y))

