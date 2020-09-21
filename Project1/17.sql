#17
SELECT COUNT(DISTINCT pid)
FROM CatchedPokemon, Trainer
WHERE hometown = 'Sangnok City'
  AND Trainer.id = owner_id
