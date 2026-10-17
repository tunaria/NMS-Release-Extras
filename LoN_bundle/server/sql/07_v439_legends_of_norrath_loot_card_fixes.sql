-- Legends of Norrath -- step 7 of 10. Originally manifest v439 (2026_10_06_legends_of_norrath_loot_card_fixes).
-- Renumber it into your own manifest. Apply in file-name order.
--
-- GUARD (manifest .check, condition "not_empty"): run the SQL below only when this query returns
-- ONE OR MORE rows.
--   SELECT 1 FROM DUAL WHERE (SELECT COUNT(*) FROM `items` WHERE `id` IN (152662,152663,152664,152665,152666,152667,152668,152669,152670,152671,152672,152673,152674,152675,152676,152677,152678,152679,152680,54916,1054916,2054916,64570,67718)) < 24 UNION ALL SELECT 1 FROM `items` WHERE `id` IN (40786,40787,40788,40789,40790,40791,40792,40721,40722,40723,40724,40725,40726,40727,44986,44987,44988,44989,44990,44991,60948,60949,60950,60951,60952,60953,64562,64563,64564,64565,64566,64567,50873,44992,40698) AND `idfile` IN ('IT63','IT0') UNION ALL SELECT 1 FROM `items` t JOIN `items` b ON t.`id` IN (b.`id` + 1000000, b.`id` + 2000000) WHERE b.`id` IN (40786,40787,40788,40789,40790,40791,40792,40721,40722,40723,40724,40725,40726,40727,44986,44987,44988,44989,44990,44991,60948,60949,60950,60951,60952,60953,64562,64563,64564,64565,64566,64567,50873,44992) AND t.`idfile` <> b.`idfile` UNION ALL SELECT 1 FROM `items` WHERE `id` IN (40734,40766,40705,40706,40733,40765,40795,40796,43991,43990) AND `clickeffect` <> 36878 UNION ALL SELECT 1 FROM DUAL WHERE (SELECT COUNT(DISTINCT `title_set`) FROM `titles` WHERE `title_set` IN (40734,40766,40705,40706,40733,40765,40795,40796,43991,43990)) < 10 UNION ALL SELECT 1 FROM DUAL WHERE (SELECT COUNT(*) FROM `npc_types` WHERE `id` IN (999700,999701,999702,999703,999704,999705,999706,999707)) < 8 UNION ALL SELECT 1 FROM DUAL WHERE (SELECT COUNT(DISTINCT `type`) FROM `pets` WHERE `type` IN ('FamiliarGildedDragon','FamiliarPureNature','FamiliarDCSporali','FamiliarSpringworkSpider','FamiliarVenomousRaptor','SwarmPetLon8Slash','SwarmPetLon8Pierce','SwarmPetLon8Blunt')) < 8 UNION ALL SELECT 1 FROM DUAL WHERE (SELECT COUNT(*) FROM `lon_loot_item` WHERE (`card_archetype`, `item_id`) IN ((100023381,64571),(100005337,152665),(100006941,152680),(100012447,44992),(100024361,67718),(100005311,40613))) < 6 UNION ALL SELECT 1 FROM DUAL WHERE (SELECT COUNT(*) FROM `lon_loot_item` WHERE `item_id` IN (152662,152663,152664,152665,152666,152667,152668,152669,152670,152671,152672,152673,152674,152675,152676,152677,152678,152679,152680,64571,44992,54916,64570,67718)) < 24 UNION ALL SELECT 1 FROM DUAL WHERE (SELECT COUNT(*) FROM `lon_loot_item` WHERE `card_archetype` IN (100013995,100023788,100021628,100006063,100021626,100013997,100022636,100021618,100013999,100023790,100021614,100008574,100008576,100008618,100008614,100008578,100008628,100008616,100023810,100021624,100014006,100014008,100023822,100023792,100023362,100019532,100021636,100022630,100023814,100022638,100021616,100022640,100021622,100006915,100006089,100008598,100014020,100006087,100008604,100012367,100014022,100006917,100010837,100019560,100022614,100006919,100010818,100023818,100014036,100023794,100007515,100023824,100021620,100014014,100014016,100023796,100022624,100022626,100005311)) < 59 UNION ALL SELECT 1 FROM `lon_loot_item` l JOIN `items` i ON i.`id` = l.`item_id` LEFT JOIN `veteran_reward_templates` t ON t.`claim_id` = 5000000 + l.`item_id` AND t.`reward_slot` = 1 AND t.`item_id` = l.`item_id` WHERE t.`claim_id` IS NULL UNION ALL SELECT 1 FROM DUAL WHERE (SELECT COUNT(*) FROM `veteran_reward_templates` WHERE `claim_id` IN (5040633,5040634,5040635,5040636,5040637,5040672,5040673,5040674,5040675,5040707,5040708,5040709,5040710) AND `reward_slot` BETWEEN 2 AND 7) < 19 UNION ALL SELECT 1 FROM DUAL WHERE (SELECT COUNT(*) FROM `veteran_reward_templates` WHERE `claim_id` BETWEEN 5047728 AND 5047736 AND `reward_slot` IN (2, 3)) < 18
--
-- Every EverQuest loot card redeems (/claim) to a working item, as live. Design decisions: ornament
-- cards give the '... Ornament(ation)' AUG itself; left out on purpose: Dragon-Marked Amulet,
-- Painting: Fancy Dress Party, 1-Month Subscription, every mercenary contract card, Legendary
-- Norrathian Standard, Dragon Brood Crypt Key (they keep their existing map/state).
--   1. New items, cloned from a working row: 19 ornament augs at their live ids 152662-152680 (from
--      72264, a working augtype-524288 ornament; no Enchanted/Legendary rows), Crown of Lightning
--      54916 + its Enchanted/Legendary rows 1054916 / 2054916 (from 54915 and its tiers), Painting:
--      Estarim Family 64570 (from 64569, same mesh family) and Painting: Santug Suit 67718 (from
--      72277; id and icon are ours - live has no such item). Tunarian Fist has no live aug: its card
--      keeps the mold 40698.
--   2. Weapon models: 35 ornaments that carried IT63/IT0 get their model, and so do their Enchanted
--      and Legendary rows (+1000000 / +2000000). Copies already socketed keep the old look until
--      re-socketed.
--   3. Ten title items click spell 36878 with a `titles` suffix row, title_set = item id, live dbstr
--      wording. REQUIRES a quest handler for spell 36878 (global_player.pl EVENT_ITEM_CLICK_CAST:
--      if spell 36878 -> plugin::AddTitleFlag($item_id)). Check you have it; otherwise add one.
--   4. Familiars / swarm pets whose click said "Unable to find data for pet": pets rows + npcs
--      999700-999707, cloned from the working sibling (Frost Dragon, Sporali, Clicking Beetle
--      familiars; SwarmPet809).
--   5. Card map: Idol of the Fallen -> the trophy 64571 (was the shield 32322), 20 ornament cards ->
--      the augs (Magi Shield -> 44992), 62 cards mapped for the first time.
--   6. /claim templates (claim 5000000 + item, as step 3) for every mapped item, and the 13 Ping's
--      potion packs arrive FILLED: their contents are template rows 2-7 of the bag's claim (TryReward
--      fills a container).
--   7. The 9 'Bones' cards give their live 2-slot pack, filled: the ornament (or skull) plus its
--      combine part (the parts feed Frozen Skull of the Cursed, which needs mercenaries; kept as live).
-- REMOVED FROM THIS EXPORT (player housing -- ours only): the original also (a) set placeable / UNK226
-- on 64570, 67718, 67905 (Estarim Family Crest banner) and placeable on 54998 / 60958, (b) cloned two
-- house-pet NPCs for Baby Red Dragon (54998) and Flame Wurmling (60958) and (c) registered them in our
-- housing table. Without player housing those items are ordinary items and cannot be placed. The
-- paintings keep whatever placeable flag their source rows (64569 / 72277) have. The guard below was
-- reduced to match (housing conditions and the two house-pet NPC ids removed).
-- DEPENDS ON these existing rows (stock PEQ + item tiers): items 72264, 54915, 1054915, 2054915,
-- 64569, 72277, 64571, 44992, 40698 and the 34 ornament item ids in the UPDATEs; npc_types 690, 678,
-- 718, 651; pets types FamiliarFrostDragon, SporaliFamiliar, FamiliarClickingBeetle, SwarmPet809.
-- Items, npcs and spells live in shared memory: run shared_memory, then restart zones (templates
-- load at boot). Guard condition is not_empty = "something is still missing".

CREATE TEMPORARY TABLE `nms_tmp_lon_item` AS SELECT * FROM `items` WHERE `id` = 72264 LIMIT 0;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152662, `Name` = 'Rallosian Blade Ornamentation', `augrestrict` = 5, `slots` = 24576, `icon` = 2228, `idfile` = 'IT11129', `lore` = 'Enchants one-handed slash weapons with designs of Rallos Zek', `charmfile` = 'ITEMTransAug1HS' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152663, `Name` = 'Rallosian Club Ornamentation', `augrestrict` = 6, `slots` = 24576, `icon` = 2229, `idfile` = 'IT11130', `lore` = 'Enchants one-handed blunt weapons with designs of Rallos Zek', `charmfile` = 'ITEMTransAug1HB' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152664, `Name` = 'Rallosian Dagger Ornamentation', `augrestrict` = 7, `slots` = 24576, `icon` = 2230, `idfile` = 'IT11131', `lore` = 'Enchants one-handed pierce weapon with designs of Rallos Zek', `charmfile` = 'ITEMTransAug1HP' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152665, `Name` = 'Rallosian Greatsword Ornamentation', `augrestrict` = 9, `slots` = 8192, `icon` = 2231, `idfile` = 'IT11132', `lore` = 'Enchants two-handed slash weapons with designs of Rallos Zek', `charmfile` = 'ITEMTransAug2HS' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152666, `Name` = 'Rallosian Staff Ornamentation', `augrestrict` = 10, `slots` = 8192, `icon` = 2232, `idfile` = 'IT11133', `lore` = 'Enchants two-handed blunt weapons with designs of Rallos Zek', `charmfile` = 'ITEMTransAug2HB' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152667, `Name` = 'Rallosian Claw Ornamentation', `augrestrict` = 8, `slots` = 24576, `icon` = 2582, `idfile` = 'IT11135', `lore` = 'Enchants hand-to-hand weapons with designs of Rallos Zek', `charmfile` = 'ITEMTransAugHTH' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152668, `Name` = 'Draconic Hammer Ornamentation', `augrestrict` = 6, `slots` = 24576, `icon` = 2584, `idfile` = 'IT11274', `lore` = 'Enchants one-handed blunt weapons with Draconic designs', `charmfile` = 'ITEMTransAug1HB' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152669, `Name` = 'Draconic Great Hammer Ornamentation', `augrestrict` = 10, `slots` = 8192, `icon` = 2585, `idfile` = 'IT11275', `lore` = 'Enchants two-handed blunt weapons with Draconic designs', `charmfile` = 'ITEMTransAug2HB' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152670, `Name` = 'Draconic Blade Ornamentation', `augrestrict` = 5, `slots` = 24576, `icon` = 2586, `idfile` = 'IT11276', `lore` = 'Enchants one-handed slash weapons with Draconic designs', `charmfile` = 'ITEMTransAug1HS' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152671, `Name` = 'Draconic Great Blade Ornamentation', `augrestrict` = 9, `slots` = 8192, `icon` = 2587, `idfile` = 'IT11277', `lore` = 'Enchants two-handed slash weapons with Draconic designs', `charmfile` = 'ITEMTransAug2HS' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152672, `Name` = 'Draconic Dagger Ornamentation', `augrestrict` = 7, `slots` = 24576, `icon` = 2588, `idfile` = 'IT11278', `lore` = 'Enchants one-handed pierce weapons with Draconic designs', `charmfile` = 'ITEMTransAug1HP' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152673, `Name` = 'Draconic Spear Ornamentation', `augrestrict` = 11, `slots` = 8192, `icon` = 2589, `idfile` = 'IT11279', `lore` = 'Enchants two-handed pierce weapons with Draconic designs', `charmfile` = 'ITEMTransAug2HP' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152674, `Name` = 'Draconic Fist Ornamentation', `augrestrict` = 8, `slots` = 24576, `icon` = 2590, `idfile` = 'IT11280', `lore` = 'Enchants hand-to-hand weapons with Draconic designs', `charmfile` = 'ITEMTransAugHTH' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152675, `Name` = 'Tunarian Staff Ornamentation', `augrestrict` = 6, `slots` = 24576, `icon` = 2605, `idfile` = 'IT11281', `lore` = 'Enchants one-handed blunt weapons with Tunarian designs', `charmfile` = 'ITEMTransAug1HB' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152676, `Name` = 'Tunarian Great Staff Ornamentation', `augrestrict` = 10, `slots` = 8192, `icon` = 2606, `idfile` = 'IT11282', `lore` = 'Enchants two-handed blunt weapons with Tunarian designs', `charmfile` = 'ITEMTransAug2HB' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152677, `Name` = 'Tunarian Blade Ornamentation', `augrestrict` = 5, `slots` = 24576, `icon` = 2607, `idfile` = 'IT11283', `lore` = 'Enchants one-handed slash weapons with Tunarian designs', `charmfile` = 'ITEMTransAug1HS' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152678, `Name` = 'Tunarian Great Axe Ornamentation', `augrestrict` = 9, `slots` = 8192, `icon` = 2608, `idfile` = 'IT11284', `lore` = 'Enchants two-handed slash weapons with Tunarian designs', `charmfile` = 'ITEMTransAug2HS' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152679, `Name` = 'Tunarian Dagger Ornamentation', `augrestrict` = 7, `slots` = 24576, `icon` = 2609, `idfile` = 'IT11285', `lore` = 'Enchants one-handed pierce weapons with Tunarian designs', `charmfile` = 'ITEMTransAug1HP' WHERE `id` = 72264;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `id` = 152680, `Name` = 'Tunarian Spear Ornamentation', `augrestrict` = 11, `slots` = 8192, `icon` = 2610, `idfile` = 'IT11286', `lore` = 'Enchants two-handed pierce weapons with Tunarian designs', `charmfile` = 'ITEMTransAug2HP' WHERE `id` = 72264;
UPDATE `nms_tmp_lon_item` SET `itemtype` = 54, `augtype` = 524288, `nodrop` = 0, `norent` = 1, `heirloom` = 1, `magic` = 0, `weight` = 8, `size` = 0, `classes` = 65535, `races` = 65535, `proceffect` = -1 WHERE `id` BETWEEN 152662 AND 152680;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 54915;
UPDATE `nms_tmp_lon_item` SET `id` = 54916, `Name` = 'Crown of Lightning' WHERE `id` = 54915;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 1054915;
UPDATE `nms_tmp_lon_item` SET `id` = 1054916, `Name` = 'Crown of Lightning (Enchanted)' WHERE `id` = 1054915;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 2054915;
UPDATE `nms_tmp_lon_item` SET `id` = 2054916, `Name` = 'Crown of Lightning (Legendary)' WHERE `id` = 2054915;
UPDATE `nms_tmp_lon_item` SET `clickeffect` = 24053, `icon` = 3363, `lore` = 'Crown your head in lightning', `slots` = 4194304, `weight` = 5, `size` = 1, `magic` = 1, `nodrop` = 0, `heirloom` = 1 WHERE `id` IN (54916, 1054916, 2054916);
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 64569;
UPDATE `nms_tmp_lon_item` SET `id` = 64570, `Name` = 'Painting: Estarim Family', `idfile` = 'IT23346', `icon` = 3824, `lore` = 'Painting of the Estarim Family', `weight` = 50, `size` = 4 WHERE `id` = 64569;
INSERT INTO `nms_tmp_lon_item` SELECT * FROM `items` WHERE `id` = 72277;
UPDATE `nms_tmp_lon_item` SET `id` = 67718, `Name` = 'Painting: Santug Suit', `idfile` = 'IT60000', `icon` = 7334, `lore` = 'Painting: Santug Suit' WHERE `id` = 72277;
UPDATE `nms_tmp_lon_item` SET `nodrop` = 1, `norent` = 1, `fvnodrop` = 0 WHERE `id` IN (64570, 67718);
INSERT IGNORE INTO `items` SELECT * FROM `nms_tmp_lon_item`;
DROP TEMPORARY TABLE `nms_tmp_lon_item`;
UPDATE `items` SET `idfile` = CASE `id`
  WHEN 40786 THEN 'IT11342' WHEN 40787 THEN 'IT11343' WHEN 40788 THEN 'IT11344' WHEN 40789 THEN 'IT11345'
  WHEN 40790 THEN 'IT11346' WHEN 40791 THEN 'IT11347' WHEN 40792 THEN 'IT11348'
  WHEN 40721 THEN 'IT11295' WHEN 40722 THEN 'IT11289' WHEN 40723 THEN 'IT11290' WHEN 40724 THEN 'IT11298'
  WHEN 40725 THEN 'IT11292' WHEN 40726 THEN 'IT11300' WHEN 40727 THEN 'IT11301'
  WHEN 44986 THEN 'IT11386' WHEN 44987 THEN 'IT11387' WHEN 44988 THEN 'IT11388' WHEN 44989 THEN 'IT11389'
  WHEN 44990 THEN 'IT11390' WHEN 44991 THEN 'IT11391'
  WHEN 60948 THEN 'IT11990' WHEN 60949 THEN 'IT11992' WHEN 60950 THEN 'IT11994' WHEN 60951 THEN 'IT11996'
  WHEN 60952 THEN 'IT11997' WHEN 60953 THEN 'IT11998'
  WHEN 64562 THEN 'IT12077' WHEN 64563 THEN 'IT12079' WHEN 64564 THEN 'IT12081' WHEN 64565 THEN 'IT12083'
  WHEN 64566 THEN 'IT12085' WHEN 64567 THEN 'IT12086'
  WHEN 50873 THEN 'IT11616' WHEN 44992 THEN 'IT11530' WHEN 40698 THEN 'IT11287'
  END
WHERE `id` IN (40786,40787,40788,40789,40790,40791,40792,40721,40722,40723,40724,40725,40726,40727,
  44986,44987,44988,44989,44990,44991,60948,60949,60950,60951,60952,60953,
  64562,64563,64564,64565,64566,64567,50873,44992,40698);
UPDATE `items` t JOIN `items` b ON t.`id` IN (b.`id` + 1000000, b.`id` + 2000000)
SET t.`idfile` = b.`idfile`
WHERE b.`id` IN (40786,40787,40788,40789,40790,40791,40792,40721,40722,40723,40724,40725,40726,40727,
  44986,44987,44988,44989,44990,44991,60948,60949,60950,60951,60952,60953,
  64562,64563,64564,64565,64566,64567,50873,44992);
UPDATE `items` SET `clickeffect` = 36878, `clicktype` = 3, `casttime` = 0, `maxcharges` = 1, `scriptfileid` = 0
WHERE `id` IN (40734,40766,40705,40706,40733,40765,40795,40796,43991,43990);
INSERT INTO `titles` (`prefix`, `suffix`, `title_set`)
SELECT '', v.`suffix`, v.`ts` FROM (
  SELECT 'Champion of Norrath' AS `suffix`, 40734 AS `ts` UNION ALL
  SELECT 'Legend of Norrath', 40766 UNION ALL
  SELECT 'Hero of the Inquisition', 40705 UNION ALL
  SELECT 'Hero of the Oathbound', 40706 UNION ALL
  SELECT 'Scoundrel of the Oathbreakers', 40733 UNION ALL
  SELECT 'the Void Seeker', 40765 UNION ALL
  SELECT 'The Voidwalker', 40795 UNION ALL
  SELECT 'The Grove Guardian', 40796 UNION ALL
  SELECT 'the Eagle Eye', 43991 UNION ALL
  SELECT 'the Truthkeeper', 43990) v
WHERE NOT EXISTS (SELECT 1 FROM `titles` t WHERE t.`title_set` = v.`ts`);
CREATE TEMPORARY TABLE `nms_tmp_lon_npc` AS SELECT * FROM `npc_types` WHERE `id` = 690 LIMIT 0;
INSERT INTO `nms_tmp_lon_npc` SELECT * FROM `npc_types` WHERE `id` = 690;
UPDATE `nms_tmp_lon_npc` SET `id` = 999700, `name` = 'FamiliarGildedDragon', `texture` = 2, `helmtexture` = 2 WHERE `id` = 690;
INSERT INTO `nms_tmp_lon_npc` SELECT * FROM `npc_types` WHERE `id` = 690;
UPDATE `nms_tmp_lon_npc` SET `id` = 999701, `name` = 'FamiliarPureNature', `race` = 432, `texture` = 0, `helmtexture` = 0 WHERE `id` = 690;
INSERT INTO `nms_tmp_lon_npc` SELECT * FROM `npc_types` WHERE `id` = 678;
UPDATE `nms_tmp_lon_npc` SET `id` = 999702, `name` = 'FamiliarDCSporali' WHERE `id` = 678;
INSERT INTO `nms_tmp_lon_npc` SELECT * FROM `npc_types` WHERE `id` = 718;
UPDATE `nms_tmp_lon_npc` SET `id` = 999703, `name` = 'FamiliarSpringworkSpider', `race` = 457, `texture` = 0, `helmtexture` = 0 WHERE `id` = 718;
INSERT INTO `nms_tmp_lon_npc` SELECT * FROM `npc_types` WHERE `id` = 718;
UPDATE `nms_tmp_lon_npc` SET `id` = 999704, `name` = 'FamiliarVenomousRaptor', `race` = 609, `texture` = 0, `helmtexture` = 0, `size` = 2 WHERE `id` = 718;
INSERT INTO `nms_tmp_lon_npc` SELECT * FROM `npc_types` WHERE `id` = 651;
UPDATE `nms_tmp_lon_npc` SET `id` = 999705, `name` = 'Dancing_Blade_of_Fire', `race` = 127, `gender` = 0, `texture` = 0, `helmtexture` = 0, `d_melee_texture1` = 11113, `d_melee_texture2` = 0 WHERE `id` = 651;
INSERT INTO `nms_tmp_lon_npc` SELECT * FROM `npc_types` WHERE `id` = 651;
UPDATE `nms_tmp_lon_npc` SET `id` = 999706, `name` = 'Dancing_Dagger_of_Thunder', `race` = 127, `gender` = 0, `texture` = 0, `helmtexture` = 0, `d_melee_texture1` = 11396, `d_melee_texture2` = 0 WHERE `id` = 651;
INSERT INTO `nms_tmp_lon_npc` SELECT * FROM `npc_types` WHERE `id` = 651;
UPDATE `nms_tmp_lon_npc` SET `id` = 999707, `name` = 'Dancing_Hammer_of_Ice', `race` = 127, `gender` = 0, `texture` = 0, `helmtexture` = 0, `d_melee_texture1` = 10959, `d_melee_texture2` = 0 WHERE `id` = 651;
INSERT IGNORE INTO `npc_types` SELECT * FROM `nms_tmp_lon_npc`;
DROP TEMPORARY TABLE `nms_tmp_lon_npc`;
INSERT INTO `pets` (`type`, `petpower`, `npcID`, `temp`, `petcontrol`, `petnaming`, `monsterflag`, `equipmentset`)
SELECT v.`new_type`, p.`petpower`, v.`npc`, p.`temp`, p.`petcontrol`, p.`petnaming`, p.`monsterflag`, p.`equipmentset`
FROM (
  SELECT 'FamiliarGildedDragon' AS `new_type`, 'FamiliarFrostDragon' AS `src_type`, 999700 AS `npc` UNION ALL
  SELECT 'FamiliarPureNature', 'FamiliarFrostDragon', 999701 UNION ALL
  SELECT 'FamiliarDCSporali', 'SporaliFamiliar', 999702 UNION ALL
  SELECT 'FamiliarSpringworkSpider', 'FamiliarClickingBeetle', 999703 UNION ALL
  SELECT 'FamiliarVenomousRaptor', 'FamiliarClickingBeetle', 999704 UNION ALL
  SELECT 'SwarmPetLon8Slash', 'SwarmPet809', 999705 UNION ALL
  SELECT 'SwarmPetLon8Pierce', 'SwarmPet809', 999706 UNION ALL
  SELECT 'SwarmPetLon8Blunt', 'SwarmPet809', 999707) v
JOIN `pets` p ON p.`type` = v.`src_type` AND p.`npcID` IN (690, 678, 718, 651)
WHERE NOT EXISTS (SELECT 1 FROM `pets` x WHERE x.`type` = v.`new_type`);
INSERT INTO `lon_loot_item` (`card_archetype`, `item_id`) VALUES
(100023381,64571),
(100005331,152662),(100005333,152663),(100005335,152664),(100005337,152665),(100005339,152666),(100006020,152667),
(100006097,152668),(100006093,152669),(100006095,152670),(100006091,152671),(100006099,152672),(100006103,152673),(100006101,152674),
(100006935,152675),(100007007,152676),(100006937,152677),(100007009,152678),(100006939,152679),(100006941,152680),
(100012447,44992),
(100020716,54916),(100023385,64570),(100024361,67718),
(100013995,47706),(100023788,67889),(100021628,54991),(100006063,40644),(100021626,54990),
(100013997,47710),(100022636,60946),(100021618,54987),(100013999,47708),(100023790,67890),
(100021614,54984),(100008574,40755),(100008576,40757),(100008618,40756),(100008614,40754),
(100008578,40758),(100008628,40759),(100008616,40753),(100023810,67905),(100021624,54989),
(100014006,47707),(100014008,47704),(100023822,67900),(100023792,67891),
(100023362,64561),(100019532,50858),(100021636,54994),(100022630,60955),
(100023814,67907),(100022638,60956),(100021616,54985),(100022640,60947),(100021622,54988),
(100006915,40707),(100006089,40673),(100008598,40673),(100014020,40673),(100006087,40674),
(100008604,40674),(100012367,40674),(100014022,40674),(100006917,40708),(100010837,40708),
(100019560,40708),(100022614,40708),(100006919,40709),(100010818,40709),(100023818,67909),
(100014036,47717),(100023794,67892),(100007515,40726),(100023824,67901),
(100021620,54986),(100014014,47709),(100014016,47705),(100023796,67893),
(100022624,60939),(100022626,60944),(100005311,40613)
ON DUPLICATE KEY UPDATE `item_id` = VALUES(`item_id`);
INSERT IGNORE INTO `veteran_reward_templates` (`claim_id`, `name`, `item_id`, `charges`, `reward_slot`)
SELECT DISTINCT 5000000 + l.`item_id`, LEFT(i.`Name`, 64), l.`item_id`, IF(i.`maxcharges` > 0, i.`maxcharges`, 1), 1
  FROM `lon_loot_item` l JOIN `items` i ON i.`id` = l.`item_id`;
INSERT IGNORE INTO `veteran_reward_templates` (`claim_id`, `name`, `item_id`, `charges`, `reward_slot`)
SELECT 5000000 + v.`bag`, LEFT(i.`Name`, 64), v.`item`, v.`qty`, v.`slot` FROM (
  SELECT 40633 AS `bag`, 40605 AS `item`, 5 AS `qty`, 2 AS `slot` UNION ALL
  SELECT 40634, 40606, 5, 2 UNION ALL SELECT 40634, 40607, 5, 3 UNION ALL
  SELECT 40635, 40608, 5, 2 UNION ALL
  SELECT 40636, 40605, 5, 2 UNION ALL SELECT 40636, 40606, 10, 3 UNION ALL SELECT 40636, 40607, 10, 4 UNION ALL
  SELECT 40637, 40608, 5, 2 UNION ALL SELECT 40637, 40605, 5, 3 UNION ALL SELECT 40637, 40606, 10, 4 UNION ALL SELECT 40637, 40607, 10, 5 UNION ALL
  SELECT 40672, 40648, 5, 2 UNION ALL
  SELECT 40673, 40649, 5, 2 UNION ALL
  SELECT 40674, 40650, 5, 2 UNION ALL
  SELECT 40675, 40651, 5, 2 UNION ALL
  SELECT 40707, 40680, 5, 2 UNION ALL
  SELECT 40708, 40681, 5, 2 UNION ALL
  SELECT 40709, 40682, 5, 2 UNION ALL
  SELECT 40710, 40683, 5, 2) v
JOIN `items` i ON i.`id` = v.`item`;
INSERT INTO `lon_loot_item` (`card_archetype`, `item_id`) VALUES
(100014008,47728),(100014016,47729),(100013995,47730),(100014006,47731),(100013999,47732),
(100014014,47733),(100013997,47734),(100014010,47736),(100014012,47735)
ON DUPLICATE KEY UPDATE `item_id` = VALUES(`item_id`);
INSERT IGNORE INTO `veteran_reward_templates` (`claim_id`, `name`, `item_id`, `charges`, `reward_slot`)
SELECT 5000000 + v.`bag`, LEFT(i.`Name`, 64), v.`item`, v.`qty`, v.`slot` FROM (
  SELECT 47728 AS `bag`, 47728 AS `item`, 1 AS `qty`, 1 AS `slot` UNION ALL SELECT 47728, 47704, 1, 2 UNION ALL SELECT 47728, 47738, 1, 3 UNION ALL
  SELECT 47729, 47729, 1, 1 UNION ALL SELECT 47729, 47705, 1, 2 UNION ALL SELECT 47729, 47738, 1, 3 UNION ALL
  SELECT 47730, 47730, 1, 1 UNION ALL SELECT 47730, 47706, 1, 2 UNION ALL SELECT 47730, 47738, 1, 3 UNION ALL
  SELECT 47731, 47731, 1, 1 UNION ALL SELECT 47731, 47707, 1, 2 UNION ALL SELECT 47731, 47738, 1, 3 UNION ALL
  SELECT 47732, 47732, 1, 1 UNION ALL SELECT 47732, 47708, 1, 2 UNION ALL SELECT 47732, 47738, 1, 3 UNION ALL
  SELECT 47733, 47733, 1, 1 UNION ALL SELECT 47733, 47709, 1, 2 UNION ALL SELECT 47733, 47738, 1, 3 UNION ALL
  SELECT 47734, 47734, 1, 1 UNION ALL SELECT 47734, 47710, 1, 2 UNION ALL SELECT 47734, 47738, 1, 3 UNION ALL
  SELECT 47736, 47736, 1, 1 UNION ALL SELECT 47736, 47703, 1, 2 UNION ALL SELECT 47736, 47737, 1, 3 UNION ALL
  SELECT 47735, 47735, 1, 1 UNION ALL SELECT 47735, 47715, 1, 2 UNION ALL SELECT 47735, 47739, 1, 3) v
JOIN `items` i ON i.`id` = v.`item`;
