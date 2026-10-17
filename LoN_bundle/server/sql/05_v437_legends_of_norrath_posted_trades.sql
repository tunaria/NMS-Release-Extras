-- Legends of Norrath -- step 5 of 10. Originally manifest v437 (2026_10_06_legends_of_norrath_posted_trades).
-- Renumber it into your own manifest. Apply in file-name order.
--
-- GUARD (manifest .check, condition "empty"): run the SQL below only when this query returns
-- NO rows.
--   SELECT 1 FROM information_schema.TABLES WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'lon_posted_trade_escrow' AND EXISTS (SELECT 1 FROM `rule_values` WHERE `rule_name` = 'Custom:LoNPostedTradeDays') AND EXISTS (SELECT 1 FROM `rule_values` WHERE `rule_name` = 'Custom:LoNActiveLooterOnly')
--
-- Posted trades (offered cards are held in escrow while the trade is up, trades expire after
-- Custom:LoNPostedTradeDays) and the one-card-game-per-PC switch (Custom:LoNActiveLooterOnly; the
-- bundled IsActiveLooter() is a stub that allows everyone until you implement it).
--   lon_posted_trade         one row per posted trade; state 0 open, 1 completed, 2 cancelled,
--                            3 expired, 4 being accepted (world resets a leftover 4 to 0 at boot)
--   lon_posted_trade_line    the offered (side 0) and wanted (side 1) cards, by archetype
--   lon_posted_trade_escrow  the poster's copies held for an open trade: their lon_card rows
--                            move to account 0 until the trade completes, is cancelled or expires

CREATE TABLE IF NOT EXISTS `lon_posted_trade` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `account_id` int unsigned NOT NULL,
  `poster_name` varchar(64) NOT NULL DEFAULT '',
  `recipient_account_id` int unsigned NOT NULL DEFAULT 0,
  `recipient_name` varchar(64) NOT NULL DEFAULT '',
  `state` tinyint unsigned NOT NULL DEFAULT 0,
  `accepted_by` int unsigned NOT NULL DEFAULT 0,
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `closed_at` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `state_account` (`state`,`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `lon_posted_trade_line` (
  `trade_id` int unsigned NOT NULL,
  `side` tinyint unsigned NOT NULL,
  `archetype` int unsigned NOT NULL,
  `quantity` int unsigned NOT NULL,
  PRIMARY KEY (`trade_id`,`side`,`archetype`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `lon_posted_trade_escrow` (
  `card_id` int unsigned NOT NULL,
  `trade_id` int unsigned NOT NULL,
  `account_id` int unsigned NOT NULL,
  PRIMARY KEY (`card_id`),
  KEY `trade_id` (`trade_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT IGNORE INTO `rule_values` (`ruleset_id`, `rule_name`, `rule_value`, `notes`) VALUES
(1, 'Custom:LoNPostedTradeDays', '7', 'Legends of Norrath: a posted trade nobody accepts expires after this many days and the poster gets the offered cards back.'),
(1, 'Custom:LoNActiveLooterOnly', 'true', 'Legends of Norrath: one card game per PC -- only a character IsActiveLooter() (world/lon_service.cpp) allows may play. The bundled IsActiveLooter() is a stub that allows everyone.');
