SELECT AVG(level)
FROM CatchedPokemon, Trainer
WHERE owner_id = Trainer.id
  AND name = 'Red'
