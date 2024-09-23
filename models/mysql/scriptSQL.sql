CREATE DATABASE airePPMdb;
-- usar
USE airePPMdb;

-- crear la tabla movies
CREATE TABLE registros (
 id INT AUTO_INCREMENT PRIMARY KEY,
 ppm INT NOT NULL,
 fecha DATE NOT NULL,
 tiempo TIME NOT NULL
);

INSERT INTO registros(ppm, fecha, tiempo) VALUES (255, '2024-09-16', '10:12:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (261, '2024-09-16', '10:22:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (267, '2024-09-16', '10:32:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (278, '2024-09-16', '10:42:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (285, '2024-09-16', '10:52:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (294, '2024-09-16', '11:02:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (302, '2024-09-16', '11:12:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (311, '2024-09-16', '11:22:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (319, '2024-09-16', '11:32:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (328, '2024-09-16', '11:42:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (335, '2024-09-16', '11:52:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (344, '2024-09-16', '12:02:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (351, '2024-09-16', '12:12:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (360, '2024-09-16', '12:22:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (368, '2024-09-16', '12:32:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (376, '2024-09-16', '12:42:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (384, '2024-09-16', '12:52:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (391, '2024-09-16', '13:02:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (398, '2024-09-16', '13:12:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (251, '2024-09-16', '13:22:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (259, '2024-09-16', '13:32:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (266, '2024-09-16', '13:42:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (272, '2024-09-16', '13:52:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (280, '2024-09-16', '14:02:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (287, '2024-09-16', '14:12:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (293, '2024-09-16', '14:22:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (299, '2024-09-16', '14:32:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (307, '2024-09-16', '14:42:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (315, '2024-09-16', '14:52:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (321, '2024-09-16', '15:02:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (329, '2024-09-16', '15:12:34');

INSERT INTO registros(ppm, fecha, tiempo) VALUES (255, '2024-09-17', '09:12:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (261, '2024-09-17', '09:22:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (267, '2024-09-17', '09:32:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (278, '2024-09-17', '09:42:34');
INSERT INTO registros(ppm, fecha, tiempo) VALUES (285, '2024-09-17', '09:52:34');
										
SELECT id, ppm, fecha, tiempo FROM registros            
WHERE fecha LIKE '2024-09-17%';  

SELECT * FROM registros;

SELECT id, ppm, fecha, tiempo FROM registros            
WHERE fecha LIKE '2024-09-17%'
ORDER BY fecha DESC, tiempo DESC;

SET SQL_SAFE_UPDATES = 0;
SET SQL_SAFE_UPDATES = 1;
