#37 
SELECT s.name, MAX(s.slevel) 
FROM (SELECT name, SUM(level) AS slevel
      FROM CatchedPokemon, Trainer
      WHERE Trainer.id = owner_id
      GROUP BY name) AS s
