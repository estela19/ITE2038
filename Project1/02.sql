#2
SELECT name
FROM Pokemon
WHERE type IN ( 
    SELECT type FROM Pokemon 
    GROUP BY type
    HAVING COUNT(*) >= 
      (SELECT MAX(bcnt)
       FROM (SELECT COUNT(*) AS bcnt FROM Pokemon GROUP BY type) AS B
       WHERE bcnt < (SELECT MAX(cnt) FROM (SELECT COUNT(*) AS cnt FROM Pokemon GROUP BY type)AS A)))
ORDER BY name
