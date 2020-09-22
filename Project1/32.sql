#32
SELECT name
FROM Pokemon
WHERE id <> ALL(
  SELECT pid
  FROM CatchedPokemon)
ORDER BY name
