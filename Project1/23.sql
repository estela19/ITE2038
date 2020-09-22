#23
SELECT DISTINCT name
FROM CatchedPokemon, Trainer
WHERE Trainer.id = owner_id
  AND level <= 10
ORDER BY name
