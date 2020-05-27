# Pokethon - Predicting Pokemon Battles

Given a dataset of pokemon battles and an adversary team, find the best pokemons to beat the enemy team. Hackathon Submission.

### Run Locally

Running Notebooks:

```bash
pip install -r requirements.txt
jupyter lab
```

Generate Submission from raw data:

```bash
# Generate Predictions for Pokemon Battles using the final model
python src/main.py
# Compute Submission Pokemon Team
g++ team_creation/optimalchooser.cpp -o team_creation/main
./team_creation/main -p 'data/04_model_output/ensemble'
```

