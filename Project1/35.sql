#35
SELECT name
FROM Pokemon, Evolution
WHERE id = before_id
  AND after_id < before_id
ORDER BY name
