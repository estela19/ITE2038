#40
SELECT hometown, nickname
FROM Trainer, CatchedPokemon
WHERE Trainer.id = owner_id
  AND level IN (SELECT MAX(level)
                 FROM Trainer, CatchedPokemon
                 WHERE Trainer.id = owner_id
                 GROUP BY hometown)
GROUP BY hometown
ORDER BY hometown
               
