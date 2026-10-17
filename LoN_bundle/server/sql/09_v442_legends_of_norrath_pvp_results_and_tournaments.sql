-- Legends of Norrath -- step 9 of 10. Originally manifest v442 (2026_10_06_legends_of_norrath_pvp_results_and_tournaments).
-- Renumber it into your own manifest. Apply in file-name order.
--
-- GUARD (manifest .check, condition "empty"): run the SQL below only when this query returns
-- NO rows.
--   SELECT 1 FROM DUAL WHERE (SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME IN ('lon_pvp_stats','lon_pvp_match','lon_tournament','lon_tournament_entry')) = 4 AND (SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'lon_pvp_stats' AND COLUMN_NAME IN ('hidden','ranked','rating_casual','rating_tournament')) = 4 AND (SELECT COUNT(*) FROM `rule_values` WHERE `ruleset_id` = 1 AND `rule_name` IN ('Custom:LoNPvPMinSeconds','Custom:LoNRatingStart','Custom:LoNRatingK','Custom:LoNRatingPairDailyGames','Custom:LoNConcedeLosesRating','Custom:LoNRankedMinGames','Custom:LoNRankedMinOpponents','Custom:LoNTournamentPlayers','Custom:LoNTournamentTitle','Custom:LoNTournamentRewardText','Custom:LoNTournamentOneEntryPerIP','Custom:LoNTournamentNoShowSeconds')) = 12
--
-- PvP results, ratings and tournaments. No item prizes anywhere (a second PC would farm them): a PvP
-- game pays the Journal (72/73 casual, 74 tournament) when won by questing, avatar defeat or decking,
-- between two accounts on two addresses; only TOURNAMENT games move the Elo rating -- Casual Games
-- are unrated, as live (research/lon_casual_games_spec.md 9). The tables can feed a website
-- leaderboard; world writes them, nothing reads them back but the Profile record and ratings.
--   lon_pvp_stats         one row per account: character name shown, tournament rating (rating_casual
--                         is unused), casual / tournament wins / losses, tournaments played / won, rated
--                         tournament games and distinct opponents, ranked (games >= Custom:LoNRankedMinGames
--                         and opponents >= Custom:LoNRankedMinOpponents), hidden (a GM flag: keep off the
--                         leaderboard), last played
--   lon_pvp_match         one row per game: mode (1 casual, 2 tournament), seats, winner / loser, end type
--                         (avatar, decked, quests, concede, disconnect, noshow, timeout, error, disputed,
--                         unconfirmed, noreport), loss type, turns, seconds, counted + reason, rating change
--   lon_tournament        one row per tournament (id = the number players see); lon_tournament_entry = its
--                         players with place, wins, losses and whether they dropped

CREATE TABLE IF NOT EXISTS `lon_pvp_stats` (
  `account_id` int unsigned NOT NULL,
  `char_name` varchar(64) NOT NULL DEFAULT '',
  `rating_casual` int NOT NULL DEFAULT 1500,
  `rating_tournament` int NOT NULL DEFAULT 1500,
  `casual_wins` int unsigned NOT NULL DEFAULT 0,
  `casual_losses` int unsigned NOT NULL DEFAULT 0,
  `tourn_wins` int unsigned NOT NULL DEFAULT 0,
  `tourn_losses` int unsigned NOT NULL DEFAULT 0,
  `tournaments_played` int unsigned NOT NULL DEFAULT 0,
  `tournaments_won` int unsigned NOT NULL DEFAULT 0,
  `games` int unsigned NOT NULL DEFAULT 0,
  `opponents` int unsigned NOT NULL DEFAULT 0,
  `ranked` tinyint unsigned NOT NULL DEFAULT 0,
  `hidden` tinyint unsigned NOT NULL DEFAULT 0,
  `last_played` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`account_id`),
  KEY `board_casual` (`hidden`,`ranked`,`rating_casual`),
  KEY `board_tournament` (`hidden`,`ranked`,`rating_tournament`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `lon_pvp_match` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `game_id` int unsigned NOT NULL DEFAULT 0,
  `mode` tinyint unsigned NOT NULL DEFAULT 1,
  `tournament_id` int unsigned NOT NULL DEFAULT 0,
  `host_account` int unsigned NOT NULL DEFAULT 0,
  `guest_account` int unsigned NOT NULL DEFAULT 0,
  `host_name` varchar(64) NOT NULL DEFAULT '',
  `guest_name` varchar(64) NOT NULL DEFAULT '',
  `winner_account` int unsigned NOT NULL DEFAULT 0,
  `loser_account` int unsigned NOT NULL DEFAULT 0,
  `end_type` varchar(16) NOT NULL DEFAULT '',
  `loss_type` int unsigned NOT NULL DEFAULT 0,
  `turns` int unsigned NOT NULL DEFAULT 0,
  `seconds` int unsigned NOT NULL DEFAULT 0,
  `started_at` datetime NULL DEFAULT NULL,
  `ended_at` datetime NULL DEFAULT NULL,
  `counted` tinyint unsigned NOT NULL DEFAULT 0,
  `reason` varchar(64) NOT NULL DEFAULT '',
  `rating_change` int NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`),
  KEY `host_ended` (`host_account`,`ended_at`),
  KEY `guest_ended` (`guest_account`,`ended_at`),
  KEY `winner` (`winner_account`),
  KEY `tournament` (`tournament_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `lon_tournament` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(64) NOT NULL DEFAULT '',
  `players` int unsigned NOT NULL DEFAULT 0,
  `rounds` int unsigned NOT NULL DEFAULT 0,
  `winner_account` int unsigned NOT NULL DEFAULT 0,
  `started_at` datetime NULL DEFAULT NULL,
  `ended_at` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `lon_tournament_entry` (
  `tournament_id` int unsigned NOT NULL,
  `account_id` int unsigned NOT NULL,
  `char_name` varchar(64) NOT NULL DEFAULT '',
  `place` int unsigned NOT NULL DEFAULT 0,
  `wins` int unsigned NOT NULL DEFAULT 0,
  `losses` int unsigned NOT NULL DEFAULT 0,
  `dropped` tinyint unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`tournament_id`,`account_id`),
  KEY `account` (`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT IGNORE INTO `rule_values` (`ruleset_id`, `rule_name`, `rule_value`, `notes`) VALUES
(1, 'Custom:LoNPvPMinSeconds', '180', 'Legends of Norrath PvP: a game won by the rules (quests, avatar, decking) moves the tournament rating or pays the Journal only if it lasted at least this many seconds (and Custom:LoNMinScenarioTurns turns).'),
(1, 'Custom:LoNRatingStart', '1500', 'Legends of Norrath PvP: the tournament rating a new player starts with. Casual Games are unrated (as live).'),
(1, 'Custom:LoNRatingK', '32', 'Legends of Norrath PvP: Elo K factor -- the most the tournament rating moves in one game.'),
(1, 'Custom:LoNRatingPairDailyGames', '3', 'Legends of Norrath PvP: counted games per 24 hours against the SAME opponent, per mode (tournament: rated, casual: Journal); games beyond this still count on the Profile record but move no rating and pay no Journal (anti-farm).'),
(1, 'Custom:LoNConcedeLosesRating', 'true', 'Legends of Norrath tournaments: a concede, a disconnect or a missing result report is rated -- the loser loses tournament rating and the winner gains it as for any win (different addresses and the daily limit still apply). Off = they count on the record only. Casual Games are never rated.'),
(1, 'Custom:LoNRankedMinGames', '5', 'Legends of Norrath leaderboard: rated tournament games a player needs before being listed as ranked (lon_pvp_stats.ranked).'),
(1, 'Custom:LoNRankedMinOpponents', '3', 'Legends of Norrath leaderboard: different opponents a player needs in rated tournament games before being listed as ranked.'),
(1, 'Custom:LoNTournamentPlayers', '4', 'Legends of Norrath tournaments: players the open queue waits for; the tournament starts when it fills (single elimination, best of one, free, constructed, no item prizes). 2-16.'),
(1, 'Custom:LoNTournamentTitle', 'Open', 'Legends of Norrath tournaments: the title the queue and the tournament show.'),
(1, 'Custom:LoNTournamentRewardText', 'Journal + ranking', 'Legends of Norrath tournaments: the Reward Structure column text. There are no item prizes.'),
(1, 'Custom:LoNTournamentOneEntryPerIP', 'true', 'Legends of Norrath tournaments: refuse a queue entry from an IP that already has a player in the queue.'),
(1, 'Custom:LoNTournamentNoShowSeconds', '180', 'Legends of Norrath tournaments: how long a round waits for a player still in a game screen, a match room or a trade; after that the player forfeits the pairing.');
