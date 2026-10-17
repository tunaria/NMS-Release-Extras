-- Legends of Norrath -- step 3 of 10. Originally manifest v435 (2026_10_06_legends_of_norrath_loot_claim_templates).
-- Renumber it into your own manifest. Apply in file-name order.
--
-- GUARD (manifest .check, condition "empty"): run the SQL below only when this query returns
-- NO rows.
--   SELECT `claim_id` FROM `veteran_reward_templates` WHERE `claim_id` BETWEEN 5000000 AND 5999999 LIMIT 1
--
-- Loot cards are claimed in /claim (account-wide, so the player picks the character). One
-- veteran reward template per loot item, claim id = 5000000 + item id; world adds account_rewards
-- rows when a card is redeemed. Zones load the templates at boot (restart zones after this).
-- A loot item mapped later (lon_loot_item) needs its template too -- world falls back to a
-- parcel for an item without one.

INSERT IGNORE INTO `veteran_reward_templates` (`claim_id`, `name`, `item_id`, `charges`, `reward_slot`)
SELECT DISTINCT 5000000 + l.`item_id`, LEFT(i.`Name`, 64), l.`item_id`, IF(i.`maxcharges` > 0, i.`maxcharges`, 1), 1
  FROM `lon_loot_item` l JOIN `items` i ON i.`id` = l.`item_id`;
