-- Legends of Norrath -- step 10 of 10. Originally manifest v443 (2026_10_06_legends_of_norrath_home_screen_and_link_buttons).
-- Renumber it into your own manifest. Apply in file-name order.
--
-- GUARD (manifest .check, condition "empty"): run the SQL below only when this query returns
-- NO rows.
--   SELECT 1 FROM DUAL WHERE (SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'lon_news' AND COLUMN_NAME IN ('id','sort_key','title','body','enabled')) = 5 AND (SELECT COUNT(*) FROM `rule_values` WHERE `ruleset_id` = 1 AND `rule_name` IN ('Custom:LoNLinkButtons','Custom:LoNHelpUrl','Custom:LoNForumsUrl','Custom:LoNLeaderboardUrl','Custom:LoNWebsiteUrl','Custom:LoNMOTD')) = 6
--
-- Home screen and link buttons. The home screen's Getting Started box is filled from lon_news (world
-- sends it at card-game login; the client shows the highest sort_key first; title and body are plain
-- text, world escapes them), its Message of the Day from Custom:LoNMOTD, its Leaderboards box from
-- lon_pvp_stats (top 10 tournament, ranked, not hidden). The navigator's Help / Review EULA / Forums /
-- Leaderboards / Website items stay greyed out unless their bit is set in Custom:LoNLinkButtons (and,
-- for the four page buttons, their Custom:LoN*Url is an http(s) address). Seeds: four Getting Started
-- items, every switch OFF, every address empty. The guard reads only information_schema and
-- rule_values; the rules are inserted last, so all six present = the seeds ran.

CREATE TABLE IF NOT EXISTS `lon_news` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `sort_key` int unsigned NOT NULL DEFAULT 0,
  `title` varchar(128) NOT NULL DEFAULT '',
  `body` varchar(2000) NOT NULL DEFAULT '',
  `enabled` tinyint unsigned NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT IGNORE INTO `lon_news` (`id`, `sort_key`, `title`, `body`, `enabled`) VALUES
(1, 40, 'Learn to play', 'Start with Tutorials / Scenarios to learn the rules against the AI.', 1),
(2, 30, 'Your cards', 'Open your packs in Collection, then build a deck in the Deck Builder.', 1),
(3, 20, 'Trade', 'Trade cards with other players in the Trade lobby, or post a trade for anyone to accept.', 1),
(4, 10, 'Play others', 'Play Casual Games for fun and Journal entries, and Tournaments to earn a rating.', 1);

INSERT IGNORE INTO `rule_values` (`ruleset_id`, `rule_name`, `rule_value`, `notes`) VALUES
(1, 'Custom:LoNLinkButtons', '0', 'Legends of Norrath: which navigator menu items work, as a sum: 1 Help, 2 Review EULA, 4 Forums, 8 Leaderboards (and the home screen''s View More...), 16 Website. 0 = all five greyed out. Help, Forums, Leaderboards and Website also need their Custom:LoN*Url. Players get it on their next zone-in.'),
(1, 'Custom:LoNHelpUrl', '', 'Legends of Norrath: the page the Help menu item opens (http:// or https://, at most 255 characters). Empty = Help stays greyed out. Needs bit 1 in Custom:LoNLinkButtons.'),
(1, 'Custom:LoNForumsUrl', '', 'Legends of Norrath: the page the Forums menu item opens (http:// or https://, at most 255 characters). Empty = Forums stays greyed out. Needs bit 4 in Custom:LoNLinkButtons.'),
(1, 'Custom:LoNLeaderboardUrl', '', 'Legends of Norrath: the page the Leaderboards menu item and the home screen''s View More... open (http:// or https://, at most 255 characters). Empty = greyed out. Needs bit 8 in Custom:LoNLinkButtons.'),
(1, 'Custom:LoNWebsiteUrl', '', 'Legends of Norrath: the page the Website menu item opens (http:// or https://, at most 255 characters). Empty = Website stays greyed out. Needs bit 16 in Custom:LoNLinkButtons.'),
(1, 'Custom:LoNMOTD', 'Welcome to Legends of Norrath! Learn the game in Tutorials, build your decks, trade with other players, and good luck at the table.', 'Legends of Norrath: the home screen''s Message of the Day, sent at card-game login. Plain text (HTML is shown as text). Empty = the box stays blank.');
