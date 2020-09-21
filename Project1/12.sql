SELECT DISTINCT name, type
FROM Pokemon, CatchedPokemon
WHERE pid = Pokemon.id
  AND level >= 30
ORDER BY name
