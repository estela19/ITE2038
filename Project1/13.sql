SELECT Pokemon.name, Pokemon.id
FROM Trainer, CatchedPokemon, Pokemon
WHERE Trainer.id = owner_id
    AND pid = Pokemon.id
    AND hometown = 'Sangnok City'
ORDER BY Pokemon.id
  
