#14
SELECT Pokemon.name
FROM Pokemon, Evolution
WHERE id = before_id
  AND type = 'Grass'
