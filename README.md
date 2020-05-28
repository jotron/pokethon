# Pokethon - Predicting Pokemon Battles

Given a dataset of pokemon battles and an adversary team, find the best pokemons to beat the enemy team. Hackathon Submission.

### Run Locally

Running Notebooks:

```bash
# Install dependencies
cd pokethon
pipenv install # Alternatively install entries in Pipfile manually
# Start jupyter
jupyter lab
```

Generate Submission from raw data:

```bash
# Generate Predictions for Pokemon Battles:
# This trains the model and and generates the intermediate output.
# The file is mostly identical to the notebook.
# Only major difference to the notebook is that it uses the entire data.
python src/main.py
# Compute Submission Pokemon Team
make -C team_creation
./team_creation/create
```