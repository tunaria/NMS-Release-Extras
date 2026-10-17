-- Legends of Norrath -- step 4 of 10. Originally manifest v436 (2026_10_06_legends_of_norrath_no_monthly_packs).
-- Renumber it into your own manifest. Apply in file-name order.
--
-- GUARD (manifest .check, condition "not_empty"): run the SQL below only when this query returns
-- ONE OR MORE rows.
--   SELECT `rule_name` FROM `rule_values` WHERE `rule_name` = 'Custom:LoNMonthlyPacks' AND `rule_value` <> '0'
--
-- No free booster packs: free packs per new account or per month invite throwaway accounts made
-- to farm loot cards. The compiled default of Custom:LoNMonthlyPacks is 0 too; this only clears a
-- stored non-zero value, so on a fresh install it is a no-op (safe to skip).

UPDATE `rule_values` SET `rule_value` = '0' WHERE `rule_name` = 'Custom:LoNMonthlyPacks';
