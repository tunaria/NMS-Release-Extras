-- Guard for 08_v440_legends_of_norrath_pvp_rule.sql: apply that file only when this returns NO rows.
SELECT `rule_name` FROM `rule_values` WHERE `rule_name` = 'Custom:LoNPvPEnabled' AND `ruleset_id` = 1;
