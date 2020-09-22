SELECT AVG(level)
FROM Trainer, CatchedPokemon, Pokemon
WHERE Trainer.id = owner_id AND pid = Pokemon.id
    AND hometown='Sangnok City'
    AND type='Electric'
