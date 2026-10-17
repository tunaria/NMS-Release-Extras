-- Legends of Norrath -- step 8 of 10. Originally manifest v440 (2026_10_06_legends_of_norrath_pvp_rule).
-- Renumber it into your own manifest. Apply in file-name order.
--
-- GUARD (manifest .check, condition "empty"): run the SQL below only when this query returns
-- NO rows.
--   SELECT `rule_name` FROM `rule_values` WHERE `rule_name` = 'Custom:LoNPvPEnabled' AND `ruleset_id` = 1
--
-- The PvP switch (Custom:LoNPvPEnabled), seeded OFF. Casual Games + Tournaments: one client hosts
-- the game (dinput8 makes it the authority) and world relays the game commands. No item prizes.

INSERT IGNORE INTO `rule_values` (`ruleset_id`, `rule_name`, `rule_value`, `notes`) VALUES
(1, 'Custom:LoNPvPEnabled', 'false', 'Legends of Norrath: card-game PvP -- Casual Games (Create / Join a match) and Tournaments. Off = creating, joining and queueing are refused with a line in EQ chat.');
