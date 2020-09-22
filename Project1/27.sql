#27
SELECT Trainer.name, MAX(level)
FROM Trainer, CatchedPokemon
WHERE Trainer.id = owner_id
GROUP BY name
HAVING COUNT(nickname) >= 4 AND MAX(level)
ORDER BY name
