#21
SELECT name, COUNT(CatchedPokemon.id)
FROM Gym, Trainer, CatchedPokemon
WHERE Trainer.id = leader_id
  AND Trainer.id = owner_id
GROUP BY name
ORDER BY name
