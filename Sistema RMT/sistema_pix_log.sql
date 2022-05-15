-- phpMyAdmin SQL Dump
-- version 4.8.5
-- https://www.phpmyadmin.net/
--
-- Host: localhost
-- Generation Time: 15-Maio-2022 às 04:29
-- Versão do servidor: 5.7.25
-- versão do PHP: 7.1.26

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `everragnew`
--

-- --------------------------------------------------------

--
-- Estrutura da tabela `sistema_pix_log`
--

CREATE TABLE `sistema_pix_log` (
  `id` int(11) UNSIGNED NOT NULL,
  `account_id` int(11) UNSIGNED NOT NULL DEFAULT '0',
  `char_id` int(11) UNSIGNED NOT NULL DEFAULT '0',
  `char_nome` varchar(30) CHARACTER SET latin1 NOT NULL,
  `dia` int(11) UNSIGNED NOT NULL DEFAULT '0',
  `mes` int(11) UNSIGNED NOT NULL DEFAULT '0',
  `ano` int(11) UNSIGNED NOT NULL DEFAULT '0',
  `valor_sacado` int(11) UNSIGNED NOT NULL DEFAULT '0',
  `valor_depositado` int(11) UNSIGNED NOT NULL DEFAULT '0',
  `moeda` varchar(30) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `sistema_pix_log`
--
ALTER TABLE `sistema_pix_log`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `sistema_pix_log`
--
ALTER TABLE `sistema_pix_log`
  MODIFY `id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
