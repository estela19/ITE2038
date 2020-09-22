#1
SELECT Trainer.name
FROM Trainer, CatchedPokemon
WHERE Trainer.id = owner_id
GROUP BY name
HAVING COUNT(CatchedPokemon.id) >= 3
ORDER BY COUNT(CatchedPokemon.id) DESC
