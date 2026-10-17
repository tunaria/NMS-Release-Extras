-- Legends of Norrath -- step 2 of 10. Originally manifest v433 (2026_10_06_legends_of_norrath_loremaster_and_preferences).
-- Renumber it into your own manifest. Apply in file-name order.
--
-- GUARD (manifest .check, condition "empty"): run the SQL below only when this query returns
-- NO rows.
--   SHOW TABLES LIKE 'lon_achievement'
--
-- Loremaster + saved preferences.
--   lon_achievement   Journal achievements an account completed, with the XP each paid.
--                     Total XP = Loremaster level = bonus-loot chance (world/lon_service.cpp).
--   lon_account_pref  the card game's own account properties (AccountCommand_SetPreferences),
--                     one row per key: the raw ValueData bytes, sent back at login.

CREATE TABLE IF NOT EXISTS `lon_achievement` (
  `account_id` int unsigned NOT NULL,
  `achievement_id` int unsigned NOT NULL,
  `xp` int unsigned NOT NULL DEFAULT 0,
  `completed_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`account_id`,`achievement_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `lon_account_pref` (
  `account_id` int unsigned NOT NULL,
  `prop_key` int unsigned NOT NULL,
  `value` varbinary(4096) NOT NULL,
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`account_id`,`prop_key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
