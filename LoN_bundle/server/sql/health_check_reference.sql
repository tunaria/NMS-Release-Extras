-- Legends of Norrath -- OPTIONAL read-only content check (reference).
-- One section per SQL step (the vNNN numbers are the original manifest versions; the step number is
-- the bundle file). Runs on any MySQL/MariaDB client; the last line is the verdict.
SET @total = 0, @fail = 0;

-- ==== step 01 (v431): 2026_10_06_legends_of_norrath_card_tables ====
-- The nine Legends of Norrath card tables exist and the loot-card -> EQ item map is seeded (332 cards).
SET @v = (SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = DATABASE() AND table_name IN ('lon_account','lon_card','lon_deck','lon_session','lon_game','lon_reward_claim','lon_delivery','lon_loot_redemption','lon_loot_item')) * 1000 + (SELECT COUNT(*) FROM `lon_loot_item`);
SELECT 'v431 Legends of Norrath card tables + loot map' AS what, IF(@v >= 9332, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect>=9332')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v >= 9332, 0, 1);

-- ==== step 02 (v433): 2026_10_06_legends_of_norrath_loremaster_and_preferences ====
-- The Loremaster achievement table and the saved card-game preferences table exist.
SET @v = (SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = DATABASE() AND table_name IN ('lon_achievement','lon_account_pref'));
SELECT 'v433 Legends of Norrath Loremaster + preferences tables' AS what, IF(@v = 2, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=2')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 2, 0, 1);

-- ==== step 03 (v435): 2026_10_06_legends_of_norrath_loot_claim_templates ====
-- Every Legends of Norrath loot item has its /claim template (claim id 5000000 + item id).
SET @v = (SELECT COUNT(DISTINCT l.`item_id`) FROM `lon_loot_item` l JOIN `items` i ON i.`id` = l.`item_id`
           LEFT JOIN `veteran_reward_templates` t ON t.`claim_id` = 5000000 + l.`item_id` AND t.`item_id` = l.`item_id`
          WHERE t.`claim_id` IS NULL);
SELECT 'v435 Legends of Norrath loot items all have a /claim template' AS what, IF(@v = 0, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=0')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 0, 0, 1);

-- ==== step 04 (v436): 2026_10_06_legends_of_norrath_no_monthly_packs ====
-- No ruleset hands out free monthly booster packs (a missing row = the compiled default, 0).
SET @v = (SELECT COUNT(*) FROM `rule_values` WHERE `rule_name` = 'Custom:LoNMonthlyPacks' AND `rule_value` <> '0');
SELECT 'v436 Legends of Norrath monthly free packs off' AS what, IF(@v = 0, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=0')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 0, 0, 1);

-- ==== step 05 (v437): 2026_10_06_legends_of_norrath_posted_trades ====
-- The three posted-trade tables exist and both rules are seeded (tables x 10 + rule rows).
SET @v = (SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = DATABASE() AND table_name IN ('lon_posted_trade','lon_posted_trade_line','lon_posted_trade_escrow')) * 10
       + (SELECT COUNT(*) FROM `rule_values` WHERE `rule_name` IN ('Custom:LoNPostedTradeDays','Custom:LoNActiveLooterOnly') AND `ruleset_id` = 1);
SELECT 'v437 Legends of Norrath posted trades tables + expiry and one-card-game-per-PC rules' AS what, IF(@v = 32, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=32')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 32, 0, 1);

-- ==== step 06 (v438): 2026_10_06_legends_of_norrath_custom_avatars ====
-- The custom avatar table exists, its ids start at 200000001 or later, and the cap rule is seeded.
SET @v = (SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = DATABASE() AND table_name = 'lon_custom_avatar' AND IFNULL(auto_increment, 0) >= 200000001) * 10
       + (SELECT COUNT(*) FROM `rule_values` WHERE `rule_name` = 'Custom:LoNMaxCustomAvatars' AND `ruleset_id` = 1);
SELECT 'v438 Legends of Norrath custom avatar table + cap rule' AS what, IF(@v = 11, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=11')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 11, 0, 1);

-- ==== step 07 (v439): 2026_10_06_legends_of_norrath_loot_card_fixes ====
-- (a) Every loot card's item exists and its /claim template (5000000 + item, slot 1) gives that item.
SET @v = (SELECT COUNT(*) FROM `lon_loot_item` l LEFT JOIN `items` i ON i.`id` = l.`item_id`
           LEFT JOIN `veteran_reward_templates` t ON t.`claim_id` = 5000000 + l.`item_id` AND t.`reward_slot` = 1 AND t.`item_id` = l.`item_id`
          WHERE i.`id` IS NULL OR t.`claim_id` IS NULL);
SELECT 'v439 Legends of Norrath every loot card item exists and has its /claim template' AS what, IF(@v = 0, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=0')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 0, 0, 1);
-- (b) No loot-card ornament (itemtype 54 '... Ornament(ation)', or the Tunarian Fist mold) or its Enchanted/Legendary row has no model (IT63/IT64/IT0).
SET @v = (SELECT COUNT(*) FROM `items` x JOIN `lon_loot_item` l ON x.`id` IN (l.`item_id`, l.`item_id` + 1000000, l.`item_id` + 2000000)
          JOIN `items` b ON b.`id` = l.`item_id`
          WHERE b.`Name` LIKE '%Ornament%' AND (b.`itemtype` = 54 OR b.`id` = 40698) AND x.`idfile` IN ('IT63', 'IT64', 'IT0', ''));
SELECT 'v439 Legends of Norrath loot-card ornaments (and their tiers) all carry a weapon model' AS what, IF(@v = 0, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=0')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 0, 0, 1);
-- (c) Every loot-card familiar / swarm-pet click finds its pets row and npc; every loot-card title click (36878) has its titles row.
SET @v = (SELECT COUNT(*) FROM `lon_loot_item` l JOIN `items` i ON i.`id` = l.`item_id` JOIN `spells_new` s ON s.`id` = i.`clickeffect`
           LEFT JOIN `pets` p ON p.`type` = s.`teleport_zone` LEFT JOIN `npc_types` n ON n.`id` = p.`npcID`
          WHERE s.`effectid1` IN (108, 152) AND n.`id` IS NULL)
       + (SELECT COUNT(*) FROM `lon_loot_item` l JOIN `items` i ON i.`id` = l.`item_id` LEFT JOIN `titles` t ON t.`title_set` = i.`id`
          WHERE i.`clickeffect` = 36878 AND t.`id` IS NULL);
SELECT 'v439 Legends of Norrath loot-card pets and titles all resolve' AS what, IF(@v = 0, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=0')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 0, 0, 1);
-- (d) Content tally: 24 new items x1000 + 10 titles x100 + 8 npcs x10 + 8 pets + pack contents (19 Ping + 18 Bones) x100000
--     + the 83 cards this entry maps x10000000.
SET @v = (SELECT COUNT(*) FROM `items` WHERE `id` IN (152662,152663,152664,152665,152666,152667,152668,152669,152670,152671,152672,152673,152674,152675,152676,152677,152678,152679,152680,54916,1054916,2054916,64570,67718)) * 1000
       + (SELECT COUNT(*) FROM `titles` WHERE `title_set` IN (40734,40766,40705,40706,40733,40765,40795,40796,43991,43990)) * 100
       + (SELECT COUNT(*) FROM `npc_types` WHERE `id` IN (999700,999701,999702,999703,999704,999705,999706,999707)) * 10
       + (SELECT COUNT(*) FROM `pets` WHERE `type` IN ('FamiliarGildedDragon','FamiliarPureNature','FamiliarDCSporali','FamiliarSpringworkSpider','FamiliarVenomousRaptor','SwarmPetLon8Slash','SwarmPetLon8Pierce','SwarmPetLon8Blunt'))
       + (SELECT COUNT(*) FROM `veteran_reward_templates` WHERE `claim_id` IN (5040633,5040634,5040635,5040636,5040637,5040672,5040673,5040674,5040675,5040707,5040708,5040709,5040710) AND `reward_slot` BETWEEN 2 AND 7) * 100000
       + (SELECT COUNT(*) FROM `veteran_reward_templates` WHERE `claim_id` BETWEEN 5047728 AND 5047736 AND `reward_slot` IN (2, 3)) * 100000
       + (SELECT COUNT(*) FROM `lon_loot_item` WHERE `card_archetype` IN (100023381,100005331,100005333,100005335,100005337,100005339,100006020,100006097,100006093,100006095,100006091,100006099,100006103,100006101,100006935,100007007,100006937,100007009,100006939,100006941,100012447,100020716,100023385,100024361,100013995,100023788,100021628,100006063,100021626,100013997,100022636,100021618,100013999,100023790,100021614,100008574,100008576,100008618,100008614,100008578,100008628,100008616,100023810,100021624,100014006,100014008,100023822,100023792,100023362,100019532,100021636,100022630,100023814,100022638,100021616,100022640,100021622,100006915,100006089,100008598,100014020,100006087,100008604,100012367,100014022,100006917,100010837,100019560,100022614,100006919,100010818,100023818,100014036,100023794,100007515,100023824,100021620,100014014,100014016,100023796,100022624,100022626,100005311)) * 10000000;
SELECT 'v439 Legends of Norrath card-fix rows (items, titles, pets, pack contents, card map)' AS what, IF(@v = 833725088, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=833725088')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 833725088, 0, 1);

-- ==== step 08 (v440): 2026_10_06_legends_of_norrath_pvp_rule ====
-- The PvP switch is seeded in the default ruleset (its value is the operator's choice; the seed is 'false').
SET @v = (SELECT COUNT(*) FROM `rule_values` WHERE `rule_name` = 'Custom:LoNPvPEnabled' AND `ruleset_id` = 1);
SELECT 'v440 Legends of Norrath PvP rule seeded' AS what, IF(@v = 1, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=1')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 1, 0, 1);

-- ==== step 09 (v442): 2026_10_06_legends_of_norrath_pvp_results_and_tournaments ====
-- The four PvP result / tournament tables exist (x 100), lon_pvp_stats has its leaderboard columns
-- (x 1000: hidden, ranked, both ratings) and the twelve rules are seeded.
SET @v = (SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = DATABASE() AND table_name IN ('lon_pvp_stats','lon_pvp_match','lon_tournament','lon_tournament_entry')) * 100
       + (SELECT COUNT(*) FROM information_schema.columns WHERE table_schema = DATABASE() AND table_name = 'lon_pvp_stats' AND column_name IN ('hidden','ranked','rating_casual','rating_tournament')) * 1000
       + (SELECT COUNT(*) FROM `rule_values` WHERE `ruleset_id` = 1 AND `rule_name` IN ('Custom:LoNPvPMinSeconds','Custom:LoNRatingStart','Custom:LoNRatingK','Custom:LoNRatingPairDailyGames','Custom:LoNConcedeLosesRating','Custom:LoNRankedMinGames','Custom:LoNRankedMinOpponents','Custom:LoNTournamentPlayers','Custom:LoNTournamentTitle','Custom:LoNTournamentRewardText','Custom:LoNTournamentOneEntryPerIP','Custom:LoNTournamentNoShowSeconds'));
SELECT 'v442 Legends of Norrath PvP results, ratings and tournaments (tables + rules)' AS what, IF(@v = 4412, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=4412')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 4412, 0, 1);

-- ==== step 10 (v443): 2026_10_06_legends_of_norrath_home_screen_and_link_buttons ====
-- lon_news has its five columns (x 1000), the four seeded Getting Started items are there (x 100)
-- and the six home screen / link button rules are seeded.
SET @v = (SELECT COUNT(*) FROM information_schema.columns WHERE table_schema = DATABASE() AND table_name = 'lon_news' AND column_name IN ('id','sort_key','title','body','enabled')) * 1000
       + (SELECT COUNT(*) FROM `lon_news` WHERE `id` IN (1,2,3,4)) * 100
       + (SELECT COUNT(*) FROM `rule_values` WHERE `ruleset_id` = 1 AND `rule_name` IN ('Custom:LoNLinkButtons','Custom:LoNHelpUrl','Custom:LoNForumsUrl','Custom:LoNLeaderboardUrl','Custom:LoNWebsiteUrl','Custom:LoNMOTD'));
SELECT 'v443 Legends of Norrath home screen news table + link button / MOTD rules' AS what, IF(@v = 5406, 'OK', CONCAT('*** MISSING *** found=', IFNULL(@v,'NULL'), ' expect=5406')) AS status;
SET @total = @total + 1, @fail = @fail + IF(@v = 5406, 0, 1);

-- ============================================================================
-- Verdict
-- ============================================================================
SELECT IF(@fail = 0,
          CONCAT('SUMMARY: ALL LEGENDS OF NORRATH CHECKS PASSED (', @total, ' checks)'),
          CONCAT('SUMMARY: ', @fail, ' OF ', @total, ' CHECKS FAILED -- see *** MISSING *** lines above')) AS verdict;
