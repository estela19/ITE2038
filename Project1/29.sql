#29
SELECT COUNT(CatchedPokemon.id)
FROM CatchedPokemon, Pokemon
WHERE pid = Pokemon.id
GROUP BY type
ORDER BY type
