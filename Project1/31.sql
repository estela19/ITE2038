#31
SELECT type
FROM Pokemon, Evolution
WHERE id = before_id
GROUP BY type
HAVING COUNT(Pokemon.id) >= 3
ORDER BY type DESC
