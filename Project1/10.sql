SELECT nickname
FROM CatchedPokemon
WHERE 50 <= level
    AND 6 <= owner_id
ORDER BY nickname
