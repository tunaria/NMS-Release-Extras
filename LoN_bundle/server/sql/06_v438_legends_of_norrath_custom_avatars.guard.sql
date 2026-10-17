-- Guard for 06_v438_legends_of_norrath_custom_avatars.sql: apply that file only when this returns NO rows.
SELECT 1 FROM information_schema.TABLES WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'lon_custom_avatar' AND EXISTS (SELECT 1 FROM `rule_values` WHERE `rule_name` = 'Custom:LoNMaxCustomAvatars');
