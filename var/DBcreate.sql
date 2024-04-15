-- phpMyAdmin SQL Dump
-- version 5.0.1
-- https://www.phpmyadmin.net/
--
-- Host: localhost
-- Erstellungszeit: 18. Dez 2023 um 08:19
-- Server-Version: 10.11.6-MariaDB-1
-- PHP-Version: 7.4.3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";

SET @dbname:='energie';
SET @tabname_power='leistung';
SET @tabname_proto='proto';
SET @tabname_harvest='ertrag';
SET @tabname_temperature='temperature';
SET @tabname_data='data';
SET @tabname_mdata='mdaten';


--
-- Datenbank: `energie`
--
CREATE DATABASE IF NOT EXISTS `@dbname` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `@dbname`;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `data`
--

CREATE TABLE `@tabname_data` (
  `data_id` bigint(20) UNSIGNED NOT NULL,
  `data_ts` int(11) NOT NULL,
  `data_type` varchar(255) NOT NULL,
  `data_raw` mediumtext NOT NULL
) ENGINE=Aria DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `proto`
--


CREATE TABLE `@tabname_proto` (
  `proto_id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `proto_ts` datetime NOT NULL,
  `proto_device` varchar(255) NOT NULL,
  `proto_type` varchar(255) NOT NULL,
  `proto_text` mediumtext NOT NULL,
  UNIQUE KEY `idx_proto_id` (`proto_id`)
) ENGINE=Aria DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

ALTER TABLE `@tabname_proto`
  ADD KEY `idx_proto_device` (`proto_device`) USING BTREE,
  ADD KEY `idx_proto_ts` (`proto_ts`) USING BTREE;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `ertrag`
--

CREATE TABLE `@tabname_harvest` (
  `edevice` varchar(255) NOT NULL,
  `edatum` date NOT NULL,
  `thour` tinyint(4) NOT NULL DEFAULT -1,
  `dcharge` double NOT NULL DEFAULT 0,
  `ddischarge` double NOT NULL DEFAULT 0,
  `dgridcharge` double NOT NULL DEFAULT 0,
  `dload` double NOT NULL DEFAULT 0,
  `dpv` double NOT NULL DEFAULT 0,
  `tpv` double NOT NULL DEFAULT 0,
  `tfromgrid` double DEFAULT 0,
  `tgrid` double DEFAULT 0,
  `tdischarge` double DEFAULT 0,
  `tcharge` double DEFAULT 0,
  `tload` double DEFAULT 0,
  `tgridcharge` double DEFAULT 0,
  `dgrid` double DEFAULT 0,
  `dfromgrid` double NOT NULL DEFAULT 0,
  `dprognose` double NOT NULL DEFAULT 0,
  `dprognextd` double NOT NULL DEFAULT 0,
  `dmore` mediumtext DEFAULT NULL
) ENGINE=Aria DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `leistung`
--

CREATE TABLE `@tabname_power` (
  `pid` bigint(20) UNSIGNED NOT NULL,
  `pzeit` datetime NOT NULL,
  `pdevice` varchar(255) NOT NULL,
  `pturn` int(11) NOT NULL DEFAULT 0,
  `ppv` double DEFAULT 0,
  `ppv1` double DEFAULT 0,
  `ppv2` double DEFAULT 0,
  `ppv3` double DEFAULT 0,
  `ppv4` double DEFAULT 0,
  `puser` double DEFAULT 0,
  `pload` double DEFAULT 0,
  `pgrid` double DEFAULT 0,
  `pcharge` double NOT NULL DEFAULT 0,
  `pdischarge` double NOT NULL DEFAULT 0,
  `paccharge` double NOT NULL DEFAULT 0,
  `soc` double DEFAULT 0,
  `ubat` double DEFAULT 0,
  `ugrid` double DEFAULT 0,
  `fgrid` double DEFAULT 0,
  `more` mediumtext DEFAULT NULL
) ENGINE=Aria DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `mdaten`
--

CREATE TABLE `@tabname_mdata` (
  `md_id` bigint(20) UNSIGNED NOT NULL,
  `md_date` datetime NOT NULL DEFAULT current_timestamp(),
  `md_type` varchar(64) NOT NULL,
  `md_dev` varchar(255) DEFAULT NULL,
  `md_dwert` double DEFAULT NULL,
  `md_twert` mediumblob DEFAULT NULL
) ENGINE=Aria DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `temperature`
--

CREATE TABLE `@tabname_temperature` (
  `tid` bigint(20) UNSIGNED NOT NULL,
  `tzeit` datetime NOT NULL,
  `tdevice` varchar(255) NOT NULL,
  `tturn` int(11) NOT NULL,
  `tdata` mediumtext DEFAULT NULL
) ENGINE=Aria DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Indizes der exportierten Tabellen
--

--
-- Indizes für die Tabelle `data`
--
ALTER TABLE `@tabname_data`
  ADD UNIQUE KEY `x_data_id` (`data_id`) USING BTREE,
  ADD UNIQUE KEY `x_type` (`data_type`) USING BTREE;

--
-- Indizes für die Tabelle `ertrag`
--
ALTER TABLE `@tabname_harvest`
  ADD PRIMARY KEY (`edatum`,`edevice`,`thour`) USING BTREE,
  ADD UNIQUE KEY `x_edevice_thour,edatum` (`edevice`,`thour`,`edatum`) USING BTREE,
  ADD KEY `x_edevice` (`edevice`) USING BTREE;

--
-- Indizes für die Tabelle `leistung`
--
ALTER TABLE `@tabname_power`
  ADD PRIMARY KEY (`pzeit`,`pdevice`) USING BTREE,
  ADD UNIQUE KEY `x_pid` (`pid`) USING BTREE,
  ADD KEY `x_pdevice` (`pdevice`) USING BTREE;

--
-- Indizes für die Tabelle `mdaten`
--
ALTER TABLE `@tabname_mdata`
  ADD UNIQUE KEY `x_md_id` (`md_id`) USING BTREE,
  ADD KEY `x_md_date` (`md_date`) USING BTREE,
  ADD KEY `x_md_type_md_dev` (`md_type`,`md_dev`) USING BTREE;

--
-- Indizes für die Tabelle `temperature`
--
ALTER TABLE `@tabname_temperature`
  ADD PRIMARY KEY (`tzeit`,`tdevice`) USING BTREE,
  ADD UNIQUE KEY `x_tid` (`tid`) USING BTREE,
  ADD KEY `x_tdevice` (`tdevice`);

--
-- AUTO_INCREMENT für exportierte Tabellen
--

--
-- AUTO_INCREMENT für Tabelle `data`
--
ALTER TABLE `@tabname_data`
  MODIFY `data_id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT für Tabelle `leistung`
--
ALTER TABLE `@tabname_power`
  MODIFY `pid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT für Tabelle `mdaten`
--
ALTER TABLE `@tabname_mdata`
  MODIFY `md_id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT für Tabelle `temperature`
--
ALTER TABLE `temperature`
  MODIFY `tid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT;
