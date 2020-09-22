#33
SELECT SUM(level)
FROM Trainer, CatchedPokemon
WHERE name = 'Matis'
  AND Trainer.id = owner_id
