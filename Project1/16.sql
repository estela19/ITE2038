#16
SELECT COUNT(type)
FROM Pokemon
WHERE type = 'Water'
  OR type = 'Electric'
  OR type = 'Psychic'
