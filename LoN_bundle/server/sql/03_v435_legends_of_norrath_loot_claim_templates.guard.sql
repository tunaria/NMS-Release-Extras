-- Guard for 03_v435_legends_of_norrath_loot_claim_templates.sql: apply that file only when this returns NO rows.
SELECT `claim_id` FROM `veteran_reward_templates` WHERE `claim_id` BETWEEN 5000000 AND 5999999 LIMIT 1;
