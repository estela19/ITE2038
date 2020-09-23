#30
SELECT p.id, p.name, (SELECT name
                  FROM Pokemon
                  WHERE id = e.after_id) AS 2name, 
                  (SELECT name
                   FROM Pokemon
                   WHERE id = (SELECT after_id
                                 FROM Evolution a
                                 WHERE e.after_id = a.before_id)) AS 3name                  
FROM Pokemon AS p, Evolution AS e
WHERE p.id = e.before_id
  AND e.after_id IN (SELECT distinct before_id FROM Evolution)
  AND (SELECT after_id FROM Evolution a WHERE e.after_id = a.before_id) NOT IN (SELECT distinct before_id FROM Evolution)
  AND p.id NOT IN (SELECT distinct after_id FROM Evolution)
ORDER BY p.id
