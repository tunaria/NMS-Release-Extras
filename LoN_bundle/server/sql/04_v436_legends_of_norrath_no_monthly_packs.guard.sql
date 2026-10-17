-- Guard for 04_v436_legends_of_norrath_no_monthly_packs.sql: apply that file only when this returns ONE OR MORE rows.
SELECT `rule_name` FROM `rule_values` WHERE `rule_name` = 'Custom:LoNMonthlyPacks' AND `rule_value` <> '0';
