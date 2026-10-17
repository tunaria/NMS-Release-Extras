-- Legends of Norrath -- step 6 of 10. Originally manifest v438 (2026_10_06_legends_of_norrath_custom_avatars).
-- Renumber it into your own manifest. Apply in file-name order.
--
-- GUARD (manifest .check, condition "empty"): run the SQL below only when this query returns
-- NO rows.
--   SELECT 1 FROM information_schema.TABLES WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'lon_custom_avatar' AND EXISTS (SELECT 1 FROM `rule_values` WHERE `rule_name` = 'Custom:LoNMaxCustomAvatars')
--
-- Custom avatars (Deck Builder > Create Avatar; Custom:LoNMaxCustomAvatars, default 5).
-- The client builds the avatar; world reserves its archetype id (from 200000001), checks the
-- name, stores the object bytes the client sends (`blob`) and sends them back at every login.
-- A reserved row has no blob yet; deleted avatars keep their row (deleted_at) so names and ids
-- are never reused.

CREATE TABLE IF NOT EXISTS `lon_custom_avatar` (
  `archetype_id` int unsigned NOT NULL AUTO_INCREMENT,
  `account_id` int unsigned NOT NULL,
  `name` varchar(64) NULL DEFAULT NULL,
  `blob` mediumblob NULL,
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `deleted_at` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`archetype_id`),
  UNIQUE KEY `name` (`name`),
  KEY `account_deleted` (`account_id`,`deleted_at`)
) ENGINE=InnoDB AUTO_INCREMENT=200000001 DEFAULT CHARSET=utf8mb4;

INSERT IGNORE INTO `rule_values` (`ruleset_id`, `rule_name`, `rule_value`, `notes`) VALUES
(1, 'Custom:LoNMaxCustomAvatars', '5', 'Legends of Norrath: custom avatars an account may keep (default 5). Deleting one frees a slot.');
