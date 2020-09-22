#28
SELECT Trainer.name, AVG(level)
FROM Pokemon, CatchedPokemon, Trainer
WHERE Trainer.id = owner_id
AND pid = pokemon.id
AND (type = 'Normal' OR type = 'Electric') 
GROUP BY Trainer.name
ORDER BY AVG(level)
