#38
SELECT DISTINCT Pokemon.name
FROM Pokemon, Evolution
WHERE Pokemon.id NOT IN (SELECT before_id
                         FROM Evolution)
  AND Pokemon.id IN (SELECT after_id
                     FROM Evolution)
ORDER BY Pokemon.name
