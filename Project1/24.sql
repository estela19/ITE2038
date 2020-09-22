#24
SELECT City.name, AVG(level)
FROM City, Trainer, CatchedPokemon
WHERE City.name = hometown
  AND Trainer.id = owner_id
GROUP BY City.name
ORDER BY AVG(level)
