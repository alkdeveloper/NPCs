-- phpMyAdmin SQL Dump
-- version 4.8.5
-- https://www.phpmyadmin.net/
--
-- Host: localhost
-- Generation Time: 26-Maio-2022 às 22:40
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
-- Database: `bor`
--

-- --------------------------------------------------------

--
-- Estrutura da tabela `sistema_missao`
--

CREATE TABLE `sistema_missao` (
  `id` int(11) UNSIGNED NOT NULL,
  `account_id` int(11) UNSIGNED NOT NULL,
  `char_id` int(11) UNSIGNED NOT NULL,
  `func` int(3) UNSIGNED NOT NULL,
  `tipo_missao` int(3) UNSIGNED NOT NULL,
  `id_mission_a` int(11) UNSIGNED NOT NULL,
  `id_mission_c` int(11) UNSIGNED NOT NULL,
  `id_mission_i` int(11) UNSIGNED NOT NULL,
  `pontos` int(11) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Estrutura da tabela `sistema_missao_assasin`
--

CREATE TABLE `sistema_missao_assasin` (
  `id` int(11) UNSIGNED NOT NULL,
  `nome` varchar(21) CHARACTER SET latin1 NOT NULL,
  `texto` varchar(100) CHARACTER SET latin1 NOT NULL,
  `pontos` int(11) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Extraindo dados da tabela `sistema_missao_assasin`
--

INSERT INTO `sistema_missao_assasin` (`id`, `nome`, `texto`, `pontos`) VALUES
(1, 'TESTE', '[Central de Missões]: Você deve matar [%s] para ganhar %d pontos. %s', 0);

-- --------------------------------------------------------

--
-- Estrutura da tabela `sistema_missao_caça`
--

CREATE TABLE `sistema_missao_caça` (
  `id` int(11) UNSIGNED NOT NULL,
  `mobid` int(11) UNSIGNED NOT NULL,
  `texto` varchar(100) CHARACTER SET latin1 NOT NULL,
  `pontos` int(11) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Estrutura da tabela `sistema_missao_item`
--

CREATE TABLE `sistema_missao_item` (
  `id` int(11) UNSIGNED NOT NULL,
  `itemid` int(11) UNSIGNED NOT NULL,
  `texto` varchar(150) CHARACTER SET latin1 NOT NULL,
  `pontos` int(11) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `sistema_missao`
--
ALTER TABLE `sistema_missao`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `sistema_missao_assasin`
--
ALTER TABLE `sistema_missao_assasin`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `sistema_missao_caça`
--
ALTER TABLE `sistema_missao_caça`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `sistema_missao_item`
--
ALTER TABLE `sistema_missao_item`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `sistema_missao`
--
ALTER TABLE `sistema_missao`
  MODIFY `id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `sistema_missao_assasin`
--
ALTER TABLE `sistema_missao_assasin`
  MODIFY `id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT for table `sistema_missao_caça`
--
ALTER TABLE `sistema_missao_caça`
  MODIFY `id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `sistema_missao_item`
--
ALTER TABLE `sistema_missao_item`
  MODIFY `id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
