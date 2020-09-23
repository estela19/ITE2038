#36
SELECT Trainer.name
FROM Trainer, CatchedPokemon, Pokemon, Evolution
WHERE Trainer.id = owner_id
  AND pid = Pokemon.id
  AND Pokemon.id = after_id
  AND Pokemon.id NOT IN (SELECT before_id
                         FROM Evolution)
  AND Pokemon.id IN (SELECT after_id
                     FROM Evolution)
ORDER BY Trainer.name
