#19
SELECT COUNT(DISTINCT type)
FROM Gym, Trainer, CatchedPokemon, Pokemon
WHERE city = 'Sangnok City'
  AND leader_id = Trainer.id
  AND Trainer.id = CatchedPokemon.owner_id
  AND pid = Pokemon.id
  
