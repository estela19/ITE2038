#20
SELECT name, COUNT(pid)
FROM Trainer, CatchedPokemon
WHERE hometown = 'Sangnok city'
  AND Trainer.id = owner_id
GROUP BY name
ORDER BY COUNT(pid)
