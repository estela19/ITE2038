#5
SELECT DISTINCT name
FROM Trainer
WHERE id <> ALL (SELECT leader_id
                    FROM Gym)
ORDER BY name
