-- Active: 1700837581046@@sql3.freemysqlhosting.net@3306@sql3664762
CREATE DATABASE  IF NOT EXISTS `sql3664762`;
USE `sql3664762`;

DROP TABLE IF EXISTS `dht_sensor_data`;

CREATE TABLE `dht_sensor_data` (
  `id` int NOT NULL AUTO_INCREMENT,
  `gas` float DEFAULT NULL,
  `temperature` float DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3;
