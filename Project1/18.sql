#18
SELECT AVG(level)
FROM Trainer, Gym, CatchedPokemon
WHERE leader_id = Trainer.id
  AND Trainer.id = owner_id
  
