#34
SELECT Pokemon.name, level, nickname
FROM Gym, Trainer, CatchedPokemon, Pokemon
WHERE leader_id = Trainer.id
AND Trainer.id = CatchedPokemon.owner_id
AND pid = Pokemon.id
AND nickname LIKE 'A%'
ORDER BY Pokemon.name DESC
