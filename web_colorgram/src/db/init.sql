
USE colorgram_db;

CREATE TABLE `users` (
  `name` varchar(255) NOT NULL,
  `email` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `description` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO users (name,email,password,description) values ('admin', 'admin@gmail.com', 'srdnlen{N1ce_J0B_s0rry_f0r_th1s_X55_4nd_h77p0nly_bypass}', 'I would like to visit sardinia!');

