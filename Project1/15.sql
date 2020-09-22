#15
SELECT Trainer.id, Count(CatchedPokemon.id)
FROM Trainer, CatchedPokemon
WHERE Trainer.id = owner_id
GROUP BY  Trainer.id
HAVING COUNT(CatchedPokemon.id) >= ALL (SELECT COUNT(CatchedPokemon.id)
                                        FROM Trainer, CatchedPokemon
                                        WHERE Trainer.id = owner_id
                                        GROUP BY Trainer.id)
ORDER BY Trainer.id
