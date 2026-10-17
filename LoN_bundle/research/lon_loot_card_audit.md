# LoN EverQuest loot cards - full item audit (2026-10-06)

Research only: no database row, client file or process was touched. Every SQL below is a
**proposal**; apply it through your own migration path. Edit the BASE item only. On a server that keeps
Enchanted (+1000000) and Legendary (+2000000) tier copies of items, copy idfile changes to those rows too
(see the verification report, W1).

Scope: 401 EverQuest loot card ids = 355 unique names (card catalog decoded from the client's `data/archetypes`; (internal tool, not included)).
332 ids are mapped in `peq.lon_loot_item` today, 63 more are proposed in `lon_misc_research.md` C.2,
6 have no item. Every mapped or proposed item was checked against the **player client**
(the client folder as shipped to players), not the raw RoF2 folder.

## 1. Summary

| Verdict | Card names | Card ids |
|---|---|---|
| PASS (proven by static evidence) | 238 | 247 |
| FIX (exact change below) | 90 | 126 |
| TEST (in-game check, list in section 9) | 3 | 3 |
| DECISION (data is right, the feature behind it is off) | 21 | 22 |
| OUT (design decision: stays unmapped) | 3 | 3 |
| **Total** | **355** | **401** |

FIX by category (card names):

| Category | Names | What |
|---|---|---|
| Wrong item | 1 | Idol of the Fallen -> shield 32322 instead of trophy 64571 |
| Ornamentation molds -> aug ornaments | 21 | 19 new aug rows to create (Rallosian 6, Draconic 7, Tunarian 6) + Magi Shield remap to 44992 (+idfile) + Tunarian Fist keeps the mold (no live aug) with a real idfile |
| Aug ornaments with no model (IT63 / IT0) | 33 | Void 7, Riftseeker 7, Combine 6, Vermillion 6, Fallen 6, Brell's Hammer 1 (IT0) |
| Containers that arrive empty | 13 | all Ping's potion packs/packages |
| Familiar / swarm pet with no pets row | 8 | click fails with "Unable to find data for pet" |
| Title items that do nothing | 10 | click 205/7653 placeholders, no titles rows |
| Player-housing flags | 1 | Estarim Family Crest banner not placeable (needs player housing or another workaround) |
| Items not in peq (create) | 3 | Crown of Lightning 54916, Painting: Estarim Family 64570, Painting: Santug Suit (custom) |

Headline facts:

- **No DLL, GlobalLoad or OnDemand change is needed for any card.** Every model the fixes use is
  already indexed in the player client's `Resources\OnDemandResources.txt` (or a GlobalLoad /
  GlobalLoad_chr archive), the archive is present, and it is byte-identical to stock RoF2 (md5 of
  lon02-lon16, sofequip, wallet14, furniture11/15, tssequip, tbsequip, wallet06, phexterior1a all SAME).
  The `.mod` files are EQGM v1 or v3; v3 is what stock RoF2 itself ships in lon05/lon10 (the already
  working Discordant/Avanish ornaments are v3), so v3 is proven to load.
- **All 201 click/proc/focus/worn spells** of the mapped items exist in `spells_new` AND in the
  shipped `spells_us.txt` (root and `Resources\` copies are identical, md5 7c8e8673), and every
  column matches peq except `deleteable` (cosmetic) and `targettype` on the two pet-illusion totems.
- **All 38 mount spells PASS**: horses row present, race <= 732 (stock, tag from eqlib Races.h),
  `<tag>.eqg` OnDemand-indexed and present, `.mds` is EQGS v1 with the `MOUNT01` rider dag, and the
  texture variant (`c_<tag>_*_sNN`) exists for every `texture` used.
- **All illusions PASS**: every SPA 58 race resolves to a model the player client loads (OnDemand,
  GlobalLoad, or `Resources\GlobalLoad_chr.txt` for kgo/ise/tac = Mask of Nurga, Mastruq, Turepta).
- **All icons PASS**: highest icon used is 7334; the client has dragitem1..222 = icons 500..8491.
  Note: icons are laid out **column-major** in each sheet (icon k: x = k//6, y = k%6), which matters
  for anyone rendering them.

## 2. Wrong item

| Card id | Card | Column | Old | New | Evidence |
|---|---|---|---|---|---|
| 100023381 | Idol of the Fallen | `lon_loot_item.item_id` | 32322 (itemtype 8 shield, IT67) | **64571** | 64571 "Idol of the Fallen", itemtype 11, lore "An idol to honor Vishimtar", placeable 1, idfile IT12089 (furniture11.eqg, OnDemand, EQGM v3). Lucy lists both ids; only 64571 is the LoN trophy. |

```sql
UPDATE lon_loot_item SET item_id = 64571 WHERE card_archetype = 100023381;
```

All other 354 names were compared (name similarity + item type); the remaining name differences are
spelling/word order only (Discordian -> Discordant, Glave -> Glaive, Venemous -> Venomous, etc.) and
are the same live item.

## 3. Ornamentation cards -> aug ornaments (correction applied)

On live a LoN ornament card gave the **aug-style ornament** (itemtype 54), not the transformation
mold. Live has an aug version for every Rallosian/Draconic/Tunarian piece **except Tunarian Fist**
(Lucy 152681 is empty, ZAM 83002 shows only the 40698 container). peq has none of 152662-152680.

### 3.1 How the models were identified (texture + mesh shape + live cross-check)

Mesh extents come from a small EQGM parser (internal tool, not included); top-down silhouettes
((internal tool, not included)) were compared with the item icons rendered from
the client's own dragitem sheets. Live idfiles from Lucy raw data, where Lucy has them, agree with
every assignment (marked LIVE).

| Set | Archive / textures | Model -> shape (x length, notes) |
|---|---|---|
| Rallosian | sofequip.eqg, `tc_slashing` / `tc_bashing` (Plane of Tactics = Rallos Zek) - the only unused weapon group of the right shape before lon02; no item in peq uses 11129-11131/11133-11135 | 11129 1H sword 4.57 (crossguard, icon 2228), 11130 flanged mace 4.73 (icon 2229), 11131 rapier w/ guard 4.68 (icon 2230), 11132 2H sword 5.67 (icon 2231), 11133 hammer-staff 5.42 (icon 2232), 11135 claw 0.93 (icon 2582). 11134 is a centred 6.23 spear with no Rallosian item. |
| Draconic | lon02.eqg `dragon_*` | 11274 `hammer_1H` 3.05, 11275 `hammer_2H` 3.91 double head, 11276 sword 4.14 (1H), 11277 sword 5.08 (2H), 11278 curved dagger 3.31, 11279 spear 6.46, 11280 claw 0.77 |
| Tunarian | lon03.eqg `tunarian_*` | 11281 staff 5.16 (LIVE: Tunarian Staff = IT11281), 11282 longer staff 5.56, 11283 shortsword 4.11, 11284 axe 4.73 (axe head), 11285 dagger 2.33, 11286 spear 5.35, 11287 stone fist 1.03 |
| Riftseeker | lon04.eqg `*Pyrilen*` (warm) / `*PyrilenICY*` (blue, mean RGB ~ 60,65,85) - same 7 meshes twice | icons alternate fire/ice: Hammer 2626 ice, Great Hammer 2620 fire, Blade 2621 fire (LIVE IT11290), Great Blade 2629 ice, Short Spear 2623 fire (LIVE IT11292), Glaive 2631 ice (LIVE IT11300), Claw 2632 ice |
| Void | lon06.eqg `LON06_void_weapons_*` | 11342 mace 3.73, 11343 hammer-staff 5.22 (LIVE), 11344 sword 3.75, 11345 great axe 5.41, 11346 dagger 2.85 (LIVE), 11347 pike 8.40 (LIVE), 11348 knuckle 1.13 |
| Combine | lon07.eqg `EQ_Combine_Weapons*` | 11386 1H sword 3.34, 11387 2H sword 5.70 (LIVE), 11388 dirk 2.17, 11389 lance 7.72 (LIVE), 11390 katar 1.44 (LIVE), 11391 shield (LIVE); 11384/11385 are axes with no card |
| Magi Shield | lon07.eqg `magi_shield_001` | 11530 shield 3.46 x 2.78 (LIVE: 152743 Magi Shield Ornamentation = IT11530) |
| Vermillion | lon12.eqg `firedragon_weap` | 11990 curved blade 1H, 11991 2H blade, 11992 1H axe, 11993 2H double axe, 11994 1H thin spear, 11995 2H spear 8.28, 11996 fist, 11997 shield, 11998 bow (LIVE) |
| Fallen | lon13.eqg `death_dragon_sheet` | 12077 spiked sword 1H, 12078 2H, 12079 1H hammer, 12080 2H hammer, 12081 curved dagger, 12082 2H scythe-spear, 12083/12084 mirrored claws, 12085 shield, 12086 bow (LIVE) |
| Brell's Hammer | wallet14.eqg | 11616 1H hammer 2.49 - same model peq already uses on the mold 48086 |

Card slashing/crushing/piercing ornaments (Emerald, Vermillion, Fallen) fit both 1H and 2H weapons;
one aug has one idfile, so the **1H model** is used, as peq already does for the Emerald set
(54986 = IT11894; the 2H Emerald pierce is a separate item 55890 = IT11899).

### 3.2 Card remaps (mold -> aug)

```sql
UPDATE lon_loot_item SET item_id = CASE card_archetype
  WHEN 100005331 THEN 152662  WHEN 100005333 THEN 152663  WHEN 100005335 THEN 152664
  WHEN 100005337 THEN 152665  WHEN 100005339 THEN 152666  WHEN 100006020 THEN 152667
  WHEN 100006097 THEN 152668  WHEN 100006093 THEN 152669  WHEN 100006095 THEN 152670
  WHEN 100006091 THEN 152671  WHEN 100006099 THEN 152672  WHEN 100006103 THEN 152673
  WHEN 100006101 THEN 152674  WHEN 100006935 THEN 152675  WHEN 100007007 THEN 152676
  WHEN 100006937 THEN 152677  WHEN 100007009 THEN 152678  WHEN 100006939 THEN 152679
  WHEN 100006941 THEN 152680  WHEN 100012447 THEN 44992
  END
WHERE card_archetype IN (100005331,100005333,100005335,100005337,100005339,100006020,
  100006097,100006093,100006095,100006091,100006099,100006103,100006101,
  100006935,100007007,100006937,100007009,100006939,100006941,100012447);
-- Tunarian Fist (100007011) stays on mold 40698 - live has no aug version.
```

(100005337 "Rallosian Spear Ornamentation" -> Greatsword: the card text says "2-Handed Slashing";
live has no Rallosian spear item. If loot is delivered through /claim, remember the
`veteran_reward_templates` rows `5000000 + item_id`.)

### 3.3 New aug rows to create (live data from Lucy raw, ids are the live ids)

Template = **72264 Lightforged Longsword Ornament** (a working itemtype 54, augtype 524288 ornament:
ornament slot type 20 exists on 8,445 peq weapons; `IsOrnamentationAugment()` accepts augtype
524288). Common live values for all 19: itemtype 54, augtype 524288, nodrop 0 (No Trade), norent 1,
heirloom 1, magic 0, weight 8, size 0, classes 65535, races 65535, proceffect -1, charmfile per row.

| New id | Name | augrestrict | slots | icon | idfile | lore (live) |
|---|---|---|---|---|---|---|
| 152662 | Rallosian Blade Ornamentation | 5 | 24576 | 2228 | IT11129 | Enchants one-handed slash weapons with designs of Rallos Zek |
| 152663 | Rallosian Club Ornamentation | 6 | 24576 | 2229 | IT11130 | Enchants one-handed blunt weapons with designs of Rallos Zek |
| 152664 | Rallosian Dagger Ornamentation | 7 | 24576 | 2230 | IT11131 | Enchants one-handed pierce weapon with designs of Rallos Zek |
| 152665 | Rallosian Greatsword Ornamentation | 9 | 8192 | 2231 | IT11132 | Enchants two-handed slash weapons with designs of Rallos Zek |
| 152666 | Rallosian Staff Ornamentation | 10 | 8192 | 2232 | IT11133 | Enchants two-handed blunt weapons with designs of Rallos Zek |
| 152667 | Rallosian Claw Ornamentation | 8 | 24576 | 2582 | IT11135 | Enchants hand-to-hand weapons with designs of Rallos Zek |
| 152668 | Draconic Hammer Ornamentation | 6 | 24576 | 2584 | IT11274 | Enchants one-handed blunt weapons with Draconic designs |
| 152669 | Draconic Great Hammer Ornamentation | 10 | 8192 | 2585 | IT11275 | Enchants two-handed blunt weapons with Draconic designs |
| 152670 | Draconic Blade Ornamentation | 5 | 24576 | 2586 | IT11276 | Enchants one-handed slash weapons with Draconic designs |
| 152671 | Draconic Great Blade Ornamentation | 9 | 8192 | 2587 | IT11277 | Enchants two-handed slash weapons with Draconic designs |
| 152672 | Draconic Dagger Ornamentation | 7 | 24576 | 2588 | IT11278 | Enchants one-handed pierce weapons with Draconic designs |
| 152673 | Draconic Spear Ornamentation | 11 | 8192 | 2589 | IT11279 | Enchants two-handed pierce weapons with Draconic designs |
| 152674 | Draconic Fist Ornamentation | 8 | 24576 | 2590 | IT11280 | Enchants hand-to-hand weapons with Draconic designs |
| 152675 | Tunarian Staff Ornamentation | 6 | 24576 | 2605 | IT11281 | Enchants one-handed blunt weapons with Tunarian designs |
| 152676 | Tunarian Great Staff Ornamentation | 10 | 8192 | 2606 | IT11282 | Enchants two-handed blunt weapons with Tunarian designs |
| 152677 | Tunarian Blade Ornamentation | 5 | 24576 | 2607 | IT11283 | Enchants one-handed slash weapons with Tunarian designs |
| 152678 | Tunarian Great Axe Ornamentation | 9 | 8192 | 2608 | IT11284 | Enchants two-handed slash weapons with Tunarian designs |
| 152679 | Tunarian Dagger Ornamentation | 7 | 24576 | 2609 | IT11285 | Enchants one-handed pierce weapons with Tunarian designs |
| 152680 | Tunarian Spear Ornamentation | 11 | 8192 | 2610 | IT11286 | Enchants two-handed pierce weapons with Tunarian designs |

augrestrict values are EQEmu `AugRestriction` (5 1H slash, 6 1H blunt, 7 piercing, 8 hand-to-hand,
9 2H slash, 10 2H blunt, 11 2H pierce). Live charmfiles on Lucy are scrambled for this run (e.g. the
Draconic Blade says ITEMTransAugHTH), so set charmfile from the type: ITEMTransAug1HS / 1HB / 1HP /
2HS / 2HB / 2HP / HTH (charmfile does not gate aug insertion). Guessed: nothing else.

```sql
-- one block per row; shown for 152662, repeat with the table values
CREATE TEMPORARY TABLE t_aug AS SELECT * FROM items WHERE id = 72264;
UPDATE t_aug SET id=152662, Name='Rallosian Blade Ornamentation', augtype=524288, augrestrict=5,
  slots=24576, icon=2228, idfile='IT11129', lore='Enchants one-handed slash weapons with designs of Rallos Zek',
  charmfile='ITEMTransAug1HS', nodrop=0, norent=1, heirloom=1, magic=0, weight=8, size=0, proceffect=-1;
INSERT INTO items SELECT * FROM t_aug; DROP TEMPORARY TABLE t_aug;
```

### 3.4 Existing rows: idfile fixes

| Item | Name | idfile old -> new | Model check (player client) |
|---|---|---|---|
| 44992 | Magi Shield Ornament | IT63 -> **IT11530** | lon07 OnDemand, EQGM v3, LIVE |
| 40698 | Tunarian Fist Ornamentation (mold, kept) | IT63 -> **IT11287** | lon03 OnDemand, v3. Mold path: tradeskills.cpp sets ornament idfile = int(IDFile+2) = 11287, icon = 2611; charmfile ITEMTransfigHTH -> CanTransform accepts itemtype 45 only - correct |

For the other 20 molds (kept in peq for other uses) the same idfiles as section 3.3 apply if anyone
ever hands them out; their charmfiles (ITEMTransfig1HS/1HB/1HP/2HS/2HB/2HP/HTH/SHIELD) map to the
right CanTransform weapon types (0,3,2,1,4,35,45,8).

## 4. Aug ornaments that never change the weapon's look (idfile IT63 / IT0)

`ItemInstance::IsOrnamentationAugment()` treats an aug as an ornament when augtype is 524288/1048576
**or idfile is not IT63/IT64**. These 33 have IT63 (so the aug is a plain proc aug - no look change)
or IT0 (Brell's: counted as an ornament with model 0 - the weapon would vanish).

| Item | Name | old -> new idfile | Evidence |
|---|---|---|---|
| 40786 | Hammer Ornament of the Void | IT63 -> IT11342 | lon06 mace 3.73 |
| 40787 | Staff Ornament of the Void | IT63 -> IT11343 | LIVE 152697 = IT11343 |
| 40788 | Sword Ornament of the Void | IT63 -> IT11344 | 1H blade 3.75 |
| 40789 | Great Axe Ornament of the Void | IT63 -> IT11345 | 2H axe head 5.41 |
| 40790 | Dagger Ornament of the Void | IT63 -> IT11346 | LIVE 152700 = IT11346 |
| 40791 | Pike Ornament of the Void | IT63 -> IT11347 | LIVE 152701 = IT11347 |
| 40792 | Knuckle Ornament of the Void | IT63 -> IT11348 | fist 1.13 |
| 40721 | Riftseeker Hammer Ornament | IT63 -> IT11295 | 1H hammer, ICY texture = blue icon 2626 |
| 40722 | Riftseeker Great Hammer Ornament | IT63 -> IT11289 | 2H hammer, fire = orange icon 2620 |
| 40723 | Riftseeker Blade Ornament | IT63 -> IT11290 | LIVE 152684 = IT11290 |
| 40724 | Riftseeker Great Blade Ornament | IT63 -> IT11298 | 2H blade, ICY = blue icon 2629 |
| 40725 | Riftseeker Short Spear Ornament | IT63 -> IT11292 | LIVE 40725 = 11292 |
| 40726 | Riftseeker Glaive Ornament | IT63 -> IT11300 | LIVE 152687 = IT11300 |
| 40727 | Riftseeker Claw Ornament | IT63 -> IT11301 | fist, ICY = dark-blue icon 2632 |
| 44986 | Combine Arming Sword Ornament | IT63 -> IT11386 | 1H sword 3.34 |
| 44987 | Combine Claymore Ornament | IT63 -> IT11387 | LIVE 44987 = 11387 |
| 44988 | Combine Dirk Ornament | IT63 -> IT11388 | dagger 2.17 |
| 44989 | Combine Lance Ornament | IT63 -> IT11389 | LIVE 152740 = IT11389 |
| 44990 | Combine Katar Ornament | IT63 -> IT11390 | LIVE 152741 = IT11390 |
| 44991 | Combine Shield Ornament | IT63 -> IT11391 | LIVE 152742 = IT11391 |
| 60948 | Slashing Vermillion Dragon Ornament | IT63 -> IT11990 | 1H curved blade |
| 60949 | Crushing Vermillion Dragon Ornament | IT63 -> IT11992 | 1H axe (icon 3800 is an axe) |
| 60950 | Piercing Vermillion Dragon Ornament | IT63 -> IT11994 | 1H thin spear |
| 60951 | Tekko of the Vermillion Dragon Ornament | IT63 -> IT11996 | fist |
| 60952 | Guard of the Vermillion Dragon Ornament | IT63 -> IT11997 | shield |
| 60953 | Bow of the Vermillion Dragon Ornament | IT63 -> IT11998 | LIVE 60953 = 11998 |
| 64562 | Slashing Fallen Dragon Ornament | IT63 -> IT12077 | spiked 1H sword (icon 3920) |
| 64563 | Crushing Fallen Dragon Ornament | IT63 -> IT12079 | 1H hammer |
| 64564 | Piercing Fallen Dragon Ornament | IT63 -> IT12081 | curved dagger (icon 3924) |
| 64565 | Claw of the Fallen Dragon Ornament | IT63 -> IT12083 | claw (12084 is its mirror) |
| 64566 | Guard of the Fallen Dragon Ornament | IT63 -> IT12085 | shield |
| 64567 | Bow of the Fallen Dragon Ornament | IT63 -> IT12086 | LIVE 64567 = 12086 |
| 50873 | Brell's Hammer Ornament | **IT0** -> IT11616 | wallet14 1H hammer, same model as mold 48086 |

```sql
UPDATE items SET idfile = CASE id
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
WHERE id IN (40786,40787,40788,40789,40790,40791,40792,40721,40722,40723,40724,40725,40726,40727,
  44986,44987,44988,44989,44990,44991,60948,60949,60950,60951,60952,60953,
  64562,64563,64564,64565,64566,64567,50873,44992,40698);
```

Side note (no fix proposed): live later widened these augs to augtype 136 (slot types 4 and 8) and
loosened some augrestricts (19 / 4). peq keeps the original augtype 8; weapons with a type-4 slot
number 8,477, so the augs fit.

## 5. Containers that arrive empty - pack contents

/claim and parcels deliver the bag only. The card text is the spec for the contents; every potion
exists in peq (stack 20, click spells present in spells_us). `lon_loot_item` holds one item per card,
so either grant the contents instead of the bag, or add a bundle table (proposal:
`lon_loot_item_bundle(card_archetype, item_id, qty)` via a manifest entry) and have
`HandleRedeemItemGrant` hand out every row.

| Bag (current item) | Card ids | Contents (qty x peq id name) |
|---|---|---|
| 40633 Ping's Poignant Potion Pack | 100005293, 100007479, 100008596, 100022616 | 5 x 40605 Potion of Adventure (12918, SPA 337 exp) |
| 40634 Ping's Palliative Potion Pack | 100005295, 100007475, 100009235, 100014018, 100019566, 100020446 | 5 x 40606 Potion of Harvest (mana/end) + 5 x 40607 Potion of Healing |
| 40635 Ping's Protective Potion Pack | 100005297, 100007485, 100012371, 100014026, 100019564 | 5 x 40608 Potion of Divinity (12921 Divine Glory = invulnerability) |
| 40636 Ping's Practical Potion Pack | 100005299, 100007481, 100012369, 100014024, 100019562, 100020456, 100021648, 100022628, 100024358 | 5 x 40605 + 10 x 40606 + 10 x 40607 |
| 40637 Ping's Prodigious Potion Pack | 100005301, 100007483 | 5 x 40608 + 5 x 40605 + 10 x 40606 + 10 x 40607 |
| 40672 Ping's Penetrating Potion Pack | 100006105, 100008600, 100012429 | 5 x 40648 Potion of Barbs |
| 40673 Ping's Potion Package (Pellucid) | 100006089, 100008598, 100014020 | 5 x 40649 Potion of Translucence (invis) |
| 40674 Ping's Potion Package (Petite) | 100006087, 100008604, 100012367, 100014022 | 5 x 40650 Potion of Diminution (shrink) |
| 40675 Ping's Peregrinating Potion Pack | 100006107, 100008602, 100014028 | 5 x 40651 Potion of Translocation (gate) |
| 40707 Ping's Potion Package (Peaceful) | 100006915 | 5 x 40680 Potion of Deadishness (IVU) |
| 40708 Ping's Potion Package (Pinning) | 100006917, 100010837, 100019560, 100022614 | 5 x 40681 Potion of Binding |
| 40709 Ping's Potion Package (Punctuality) | 100006919, 100010818 | 5 x 40682 Potion of Speed |
| 40710 Ping's Propagating Potion Package | 100006921, 100010816, 100021650 | 5 x 40683 Potion of Stature |

Other packs (not FIX, listed for completeness):

| Card | Mapped item (works) | Live delivered | Missing part |
|---|---|---|---|
| Axe/Hammer/Staff/Great Blade/Spear of Bones, Dagger of Skulls, Clawed Skull | ornament aug 47704-47710 (IT11582-11588, PASS) | "... Ornament Pack" 47728-47734 (2-slot bag) | 1 x 47738 Frost Covered Gem (part 3 of Frozen Skull of the Cursed 47711) |
| Path of Skulls | 47703 nimbus (PASS) | 47736 Path of Skulls Pack | 1 x 47737 Empty Glowing Skull (card text); the pack lore says "a skull fragment and a trapped soul gem" - live data disagrees with itself |
| Plagued Skull Fragment | 47715 familiar (PASS) | 47735 Plagued Skull Fragment Pack | 1 x 47739 Trapped Soul Gem (card text) |
| Nature Touched Wurm Saddle | 64561 saddle (PASS) | 64575 Wurm Saddle Package ("A package containing the Nature-Touched Wurm Saddle") | nothing - mapping the saddle directly is the working equivalent |

The three skull parts only feed "Frozen Skull of the Cursed", a mercenary item; peq has **no recipe**
for it and mercenaries are off (`Mercs:AllowMercs=false`). Design decision whether to grant the parts.
Same for the "Part x of 3" combines (Fiery Dragonhide Pack 60959 from Venomous Raptor Saddle + Idol
of Talendor + Flame Wurmling; Hermit's Hideaway from Painting: Ice Dragon + Baby Red Dragon): **no
tradeskill recipe exists in peq**, so the "combine" text on those cards cannot be done.

## 6. Clicks that fail or do nothing

### 6.1 Familiar / swarm pets with no `pets` row (FIX)

`MakeFamiliar` (pets.cpp:182) and `TypesTemporaryPets` (aa.cpp:73) look the spell's `teleport_zone`
up in `pets`; these 8 have no row and no npc_types row, so the click prints "Unable to find data for
pet ...". Proposed rows (clone the named working sibling; everything marked * is a guess):

| Item | Card | Spell | pets.type to add | Clone npc_types | Overrides |
|---|---|---|---|---|---|
| 40700 | Familiar of Severilous | 12882 | FamiliarGildedDragon | 690 FamiliarFrostDragon | name, texture 2* (drg.eqg has bd_s00..s05; 0,1,3,4,5 are used by the other dragon familiars) |
| 54977 | Drake of Pure Nature | 17846 | FamiliarPureNature | 690 | race 432 DKE* (drake, dke.eqg GlobalLoad) |
| 54931 | Spore Curer Bait | 24056 | FamiliarDCSporali | 678 SporaliFamiliar | name |
| 47091 | Springwork Spider | 21933 | FamiliarSpringworkSpider | 718 FamiliarClickingBeetle | race 457 CWG* or 559 BTL* (clockwork) |
| 60939 | Venomous Raptor Familiar | 9696 | FamiliarVenomousRaptor | 718 | race 609 RPT (rpt.eqg GlobalLoad), size 2* |
| 47713 | Dancing Blade of Fire | 21943 | SwarmPetLon8Slash | 651 SwarmPet809 (level 60 swarm) | race 127 (invisible man, global_chr), d_melee_texture1 11113 (the item's own idfile, tbsequip OD) |
| 47712 | Dancing Dagger of Thunder | 21942 | SwarmPetLon8Pierce | 651 | race 127, d_melee_texture1 11396 (wallet06 OD) |
| 47714 | Dancing Shard of Ice | 21944 | SwarmPetLon8Blunt | 651 | race 127, d_melee_texture1 10959 (tssequip OD) |

```sql
-- pattern (one per row): npc first, then the pets row pointing at it
INSERT INTO npc_types (...) SELECT <new id>, 'FamiliarGildedDragon', ... FROM npc_types WHERE id = 690;
INSERT INTO pets (type, petpower, npcID, temp, petcontrol, petnaming, monsterflag, equipmentset)
  SELECT 'FamiliarGildedDragon', petpower, <new id>, temp, petcontrol, petnaming, monsterflag, equipmentset
  FROM pets WHERE type = 'FamiliarFrostDragon';
```

Swarm-pet damage/level is a gameplay value - tunable; decide it before setting it.

### 6.2 Title items (FIX)

Live used click 205/7653 placeholders plus a server script (`scriptfileid` 9838/13024/8886...). We
have no such scripts; our working title items are THJ "title certificates" (click **36878**, handled
in `global_player.pl` -> `plugin::AddTitleFlag`, title text from a `titles` row with
`title_set = item id`, e.g. 300001). Proposal for each: `UPDATE items SET clickeffect=36878, clicktype=<as 300001>, casttime=<as 300001>, maxcharges=<as 300001>` + one `titles` row:

| Item | Card | titles row (prefix/suffix = design choice) |
|---|---|---|
| 40734 | Title of the Champion of Norrath | "Champion of Norrath" |
| 40766 | Title of the Legend of Norrath | "Legend of Norrath" |
| 40705 | Title of the Hero of the Inquisition | "Hero of the Inquisition" |
| 40706 | Title of the Hero of the Oathbound | "Hero of the Oathbound" |
| 40733 | Title of the Scoundrel of the Oathbreakers | "Scoundrel of the Oathbreakers" |
| 40765 | Title of the Void Seeker | "the Void Seeker" |
| 40795 | Title of the Voidwalker | "The Voidwalker" |
| 40796 | Title of the Guardian of the Grove | "The Grove Guardian" |
| 43991 | Writ of the Eagle | "The Eagle Eye" |
| 43990 | Writ of the Truthkeeper | "The Truthkeeper" |

### 6.3 Design decisions (data matches live; the feature behind it is off)

| Item(s) | Card(s) | Why it does nothing |
|---|---|---|
| 54929, 50869, 54928, 60942, 54980, 50868, 54930, 72270, 54981, 60943 + proposed 67904, 72271, 67903 | 13 mercenary contracts (Blue Pirate, Dark Arachnids, Crushbone, Dragorn, Gnome Pirate, Goblinoid, Mystical, Nihil, Rotting, Terrifying, Gargoyle, Werewolf, Shissar) | click 119 placeholder + script 13964; `Mercs:AllowMercs=false` |
| 40760 | Lightweaver's Enchanting Wand | click 119 placeholder; live fired fireworks from a script. Option: point it at an existing fireworks click spell |
| 43987 | Chalandria's Fang | click 7651 placeholder + script 8884 (summon poison) |
| 40622 | Legendary Norrathian Standard | click 205 placeholder + script 9386 (guild banner) |
| 47747 | Unwritten Glyph | click 205 placeholder + script 13471 (AA glyph choice) |
| 47720 | Dragon Brood Crypt Key | key for the level-85 LoN Dragon Crypt mission |
| 72263 | Heavy Throwing Dagger | spell 37654 is SPA 10 only - no knockdown, on live data either |
| 54998, 60958 | Baby Red Dragon, Flame Wurmling | "house pets": idfile IT63, placeable 0, Lucy has no model id - nothing to place; needs player housing (house pet) or another workaround |
| 54997, 60957, 67909, 64571 | Idol trophies | the "increases statistics / Essence of Kerafyrm" benefit has no server system; the item itself is placeable (needs player housing) |

## 7. Items that need player housing

These items only do something on a server with player housing (or another workaround). Reference
flag set for a wall painting: placeable 1, nodrop 1, norent 1, fvnodrop 0, UNK226 1 (e.g. 72277 Painting:
Steed of War).

| Item | Status |
|---|---|
| All 18 mapped paintings (54993-54996, 60955, 60956, 64569, 67906-67908, 72277-72283) | PASS as items - wall-painting flags, models OnDemand-indexed; needs player housing |
| Idols 54997, 60957, 67909, 64571; building blocks 64593-64597 | PASS as items - placeable 1, models OnDemand-indexed; needs player housing |
| 67905 Estarim Family Crest (banner) | **FIX** `placeable 0->1, fvnodrop 1->0, UNK226 0->1` (wall banner; UNK226 is an inference from it being a hanging banner); needs player housing |
| 54999 Hermit's Hideaway | **TEST** - placeable 1, model IT20022 phexterior1a (a house exterior); needs player housing that can place it |

## 8. Items not in peq (create)

| New id | Name | Clone | Overrides (live values from Lucy) | Guessed |
|---|---|---|---|---|
| 54916 | Crown of Lightning | 54915 Nimbus of Fire and Ice | clickeffect 24053 (nimbus 608, in shipped spells_us), icon 3363, lore 'Crown your head in lightning', slots 4194304, weight 5, size 1, magic 1 | none |
| 64570 | Painting: Estarim Family | 67906 Painting: Meleen Estarim | idfile IT23346 (furniture11, texture estarim_family.dds, OnDemand), icon 3824, lore 'Painting of the Estarim Family', weight 50, size 4; use the wall-painting flags (placeable 1, nodrop 1, norent 1, fvnodrop 0, UNK226 1) | none |
| (pick a free id, e.g. 67718) | Painting: Santug Suit | 67717 Painting: Santug's Stockings | idfile IT60000 (lon15, only model using Santugs_suit_c.dds, OnDemand), lore 'Painting: Santug Suit' | **id and icon** (no live item exists; 87561 "Santug Suit" is chest armour) |
| 152662-152680 | 19 aug ornaments | 72264 | section 3.3 | none |

Card rows to add once created: `(100020716, 54916)`, `(100023385, 64570)`, `(100024361, <id>)`, plus
`veteran_reward_templates` 5000000 + id if loot is delivered through /claim.

## 9. TEST-NEEDED (the only things static evidence cannot settle)

1. **Hermit's Hideaway (54999)** - does a player-housing system place a house-exterior model (IT20022)?
2. **Metamorph Totem: Albino Rat / Turepta (72268/72269)** - peq targettype 38 vs spells_us 6; click
   with a pet out and confirm the pet changes.
3. **One new aug ornament** after creation (e.g. Draconic Blade 152670 in a 1H slash weapon) - proves
   the 524288 ornament path with an lon02 model. If it shows, the other 18 use the same path.
4. **One fixed IT63 aug** (e.g. Riftseeker Hammer 40721 -> IT11295) - proves the augtype-8 path with
   a lon04 model.
5. **One nimbus whose spell is not in stock RoF2's spell file** (e.g. Nimbus of the Void 40783,
   index 472). Stock RoF2 ships nimbus indices 446-463 and 654-655, so 464-642 sit inside the known
   range and the client data has matching emitter names (e.g. `Nimbus_CrystalShield`,
   `TarotCardNimbus`, `Nimbus dustStorm`), but the index table itself was not decoded.
6. **One swarm pet** (Dancing Blade of Fire) after the pets/npc rows are added.

## Out of era (live reqlevel; peq has reqlevel 0 on all of them - listed only, no change proposed)

| Item | Live reqlevel / reclevel |
|---|---|
| 43995 Shield of Scraps | 70 / 75 |
| 54944 Earring of the Ancient One | 80 / 85 (focus "Detrimental Duration 26 L85") |
| 54992 Emblem of Druushk | 80 / 85 |
| 60954 Dragonflame Ring | 85 / 90 |
| 67902 Dragon Embossed Belt | 85 / 90 |
| 40735, 40767, 40775, 47719, 50871 | 65 / 80-85 (at the cap) |
| 47720 Dragon Brood Crypt Key | level-85 content |

Dragon Marked Amulet (85), 1-Month Subscription and Fancy Dress Party stay out (design decision).

## Method / tools (internal tools, not included)

A card -> item -> peq row table; a scanner for every IT model in an install; an idfile resolver that
works the way the player client does (OnDemand index, GlobalLoad archives incl. custom ones by raw entry
scan, archive present, EQGM version); mesh-extent and top-down silhouette tools; a column-major dragitem
icon cropper; a Lucy raw item field reader; a spell compare against both spells_us.txt files; and the
generator for the master table below.

## 10. Master table - one row per card name (355 names / 401 card ids)

Verdicts: PASS = static evidence proves it loads and works; FIX = a change below is required; TEST = static evidence cannot settle it (in the short test list); DECISION = works as data but the feature behind it is off/missing - design decision; OUT = decided to leave unmapped.

| Card | Card id(s) | Item | Verdict | Evidence / action |
|---|---|---|---|---|
| Archaeologist's Runic Belt | 100014032 | 47719 Archaeologist's Runic Belt | **PASS** | click 3631 in spells_us |
| Aspect of the Bloodstone | 100012379 | 43971 Aspect of the Bloodstone | **PASS** | illusion race 464 (GGY) loadable |
| Aspect of the Brood | 100012351 | 43973 Aspect of the Brood | **PASS** | illusion race 530 (DRG) loadable |
| Aspect of the Eagle | 100012341 | 43972 Aspect of the Eagle | **PASS** | illusion race 558 (AVK) loadable |
| Avanish Guard Ornament | 100020424 | 54926 Avanish Guard Ornament | **PASS** | model IT11661 OD lon10.eqg EQGM v3; ornament aug, model OK |
| Axe of Bones | 100013995 | 47706 Axe of Bones Ornament | **PASS** | model IT11584 OD lon08.eqg EQGM v3; ornament aug, model OK; live also gave a skull-merc combine part (not granted) |
| Baby Red Dragon | 100021644 | 54998 Baby Red Dragon | **DECISION** | house pet: idfile IT63, placeable 0, live has no model either; cannot be placed (needs player housing / house pet or another workaround) |
| Balebri's Club Ornament | 100023798 | 67894 Balebri's Club Ornament | **PASS** | model IT12349 OD lon14.eqg EQGM v3; ornament aug, model OK |
| Balebri's Horse | 100023788 | 67889 Balebri's Horse Saddle | **PASS** | mount SumUNMm9Run2: horses row + archive OK |
| Bear Saddle | 100020426 | 54933 Bear Saddle | **PASS** | mount SumBear1Fast: horses row + archive OK |
| Bladestorm Nimbus | 100019546 | 50856 Bladestorm Nimbus | **PASS** | nimbus 567, spell matches spells_us |
| Blunt Force | 100014040 | 47718 Blunt Force | **PASS** | model IT10934 OD tssequip.eqg EQGM v1; pet FamiliarBluntForce row OK |
| Bow of Avanish Ornament | 100020428 | 54927 Bow of Avanish Ornament | **PASS** | model IT11662 OD lon10.eqg EQGM v3; ornament aug, model OK |
| Bow of the Emerald Dragon | 100021628 | 54991 Bow of the Emerald Dragon Ornament | **PASS** | model IT11902 OD lon11.eqg EQGM v3; ornament aug, model OK |
| Bow of the Fallen Dragon Ornament | 100023377 | 64567 Bow of the Fallen Dragon Ornament | **FIX** | aug idfile IT63 -> IT12086 (lon13.eqg EQGM v3); today the weapon look never changes |
| Bow of the Vermillion Dragon Ornament | 100022595 | 60953 Bow of the Vermillion Dragon Ornament | **FIX** | aug idfile IT63 -> IT11998 (lon12.eqg EQGM v3); today the weapon look never changes |
| Bracer of the Centaur Archer | 100010853 | 40779 Bracer of the Centaur Archer | **PASS** | illusion race 521 (CNT) loadable |
| Brell's Hammer Ornament | 100019524 | 50873 Brell's Hammer Ornament | **FIX** | aug idfile IT0 -> IT11616 (wallet14.eqg EQGM v3); today the weapon look never changes |
| Bridle of King Kalakor | 100010791 | 40776 Bridle of King Kalakor | **PASS** | mount SumLionWhiteFast: horses row + archive OK |
| Bridle of the Bloodied Sokokar | 100008570 | 40744 Bridle of the Bloodied Sokokar | **PASS** | mount SumSokokar2Fast: horses row + archive OK |
| Bridle of the Commanded Feran | 100007461 | 40711 Bridle of the Commanded Feran | **PASS** | mount SumFeran1Fast: horses row + archive OK |
| Bridle of the Crimson Cragslither | 100006061 | 40646 Bridle of the Crimson Cragslither | **PASS** | mount SumCragslither1Fast: horses row + archive OK |
| Bridle of the Cursed Kirin | 100006913 | 40679 Bridle of the Cursed Kirin | **PASS** | mount SumKirin2Fast: horses row + archive OK |
| Bridle of the Emerald Sokokar | 100008572 | 40743 Bridle of the Emerald Sokokar | **PASS** | mount SumSokokar1Fast: horses row + archive OK |
| Bridle of the Ferocious Feran | 100007463 | 40712 Bridle of the Ferocious Feran | **PASS** | mount SumFeran2Fast: horses row + archive OK |
| Bridle of the Guardian Wrulon | 100006063 | 40644 Bridle of the Wrulon Guardian | **PASS** | mount SumWrulon1Fast: horses row + archive OK |
| Bridle of the Highland Lion | 100010793 | 40777 Bridle of the Highland Lion | **PASS** | mount SumLionBrownRun2: horses row + archive OK |
| Bridle of the Mystical Kirin | 100005288 | 40603 Bridle of the Mystical Kirin | **PASS** | mount SumKirin0Fast: horses row + archive OK |
| Bridle of the Plagued Cragslither | 100006911 | 40678 Bridle of the Plagued Cragslither | **PASS** | mount SumCragslither3Fast: horses row + archive OK |
| Bridle of the Seething Chimera | 100005290 | 40604 Bridle of the Seething Chimera | **PASS** | mount SumChimeraFast: horses row + archive OK |
| Bridle of the Shadow Panther | 100005286 | 40602 Bridle of the Shadow Panther | **PASS** | mount SumPuma1Fast: horses row + archive OK |
| Bridle of the Snow Leopard | 100005284 | 40601 Bridle of the Snow Leopard | **PASS** | mount SumPuma3Fast: horses row + archive OK |
| Bridle of the Viridian Cragslither | 100006067 | 40647 Bridle of the Viridian Cragslither | **PASS** | mount SumCragslither2Fast: horses row + archive OK |
| Bridle of the Wrulon Protector | 100006065 | 40645 Bridle of the Wrulon Protector | **PASS** | mount SumWrulon2Fast: horses row + archive OK |
| Bridle of the Wrulon Warder | 100006909 | 40677 Bridle of the Wrulon Warder | **PASS** | mount SumWrulon3Fast: horses row + archive OK |
| Building Block - Column - Draconic | 100023395 | 64597 Building Block - Column - Draconic | **PASS** | model IT12094 OD furniture11.eqg EQGM v3 |
| Building Block - Cube - Draconic | 100023393 | 64596 Building Block - Cube - Draconic | **PASS** | model IT12093 OD furniture11.eqg EQGM v3 |
| Building Block - Flat Rectangle - Draconic | 100023391 | 64595 Building Block - Flat Rectangle - Draconic | **PASS** | model IT12092 OD furniture11.eqg EQGM v3 |
| Building Block - Flat Square - Draconic | 100023387 | 64593 Building Block - Flat Square - Draconic | **PASS** | model IT12090 OD furniture11.eqg EQGM v3 |
| Building Block - Long Wall - Draconic | 100023389 | 64594 Building Block - Long Wall - Draconic | **PASS** | model IT12091 OD furniture11.eqg EQGM v3 |
| Bulwark of the Emerald Dragon | 100021626 | 54990 Bulwark of the Emerald Dragon Ornament | **PASS** | model IT11901 OD lon11.eqg EQGM v3; ornament aug, model OK |
| Chalandria's Fang | 100012377 | 43987 Chalandria's Fang | **DECISION** | click 7651 placeholder + scriptfileid 8884, no script: no poison is summoned |
| Claw of the Fallen Dragon Ornament | 100023370 | 64565 Claw of the Fallen Dragon Ornament | **FIX** | aug idfile IT63 -> IT12083 (lon13.eqg EQGM v3); today the weapon look never changes |
| Clawed Skull | 100013997 | 47710 Clawed Skull Ornament | **PASS** | model IT11588 OD lon08.eqg EQGM v3; ornament aug, model OK; live also gave a skull-merc combine part (not granted) |
| Clicking Beetle | 100019552 | 47089 Clicking Beetle | **PASS** | pet FamiliarClickingBeetle row OK |
| Combine Arming Sword Ornament | 100012381 | 44986 Combine Arming Sword Ornament | **FIX** | aug idfile IT63 -> IT11386 (lon07.eqg EQGM v3); today the weapon look never changes |
| Combine Claymore Ornament | 100012343 | 44987 Combine Claymore Ornament | **FIX** | aug idfile IT63 -> IT11387 (lon07.eqg EQGM v3); today the weapon look never changes |
| Combine Dirk Ornament | 100012345 | 44988 Combine Dirk Ornament | **FIX** | aug idfile IT63 -> IT11388 (lon07.eqg EQGM v3); today the weapon look never changes |
| Combine Katar Ornament | 100012347 | 44990 Combine Katar Ornament | **FIX** | aug idfile IT63 -> IT11390 (lon07.eqg EQGM v3); today the weapon look never changes |
| Combine Lance Ornament | 100012349 | 44989 Combine Lance Ornament | **FIX** | aug idfile IT63 -> IT11389 (lon07.eqg EQGM v3); today the weapon look never changes |
| Combine Shield Ornament | 100012375 | 44991 Combine Shield Ornament | **FIX** | aug idfile IT63 -> IT11391 (lon07.eqg EQGM v3); today the weapon look never changes |
| Contract of the Blue Pirate | 100020430 | 54929 Contract of the Blue Pirate | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Contract of the Dark Arachnids | 100019526 | 50869 Contract of the Dark Arachnids | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Crown of Fire | 100022636 | 60946 Crown of Fire Nimbus | **PASS** | nimbus 654, spell matches spells_us |
| Crown of Lightning | 100020716 | - | **FIX** | NEW item 54916 (clone 54915; click 24053, nimbus 608, spell in shipped spells_us) |
| Crushbone Contract | 100020460 | 54928 Crushbone Contract | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Crushing Emerald Dragon | 100021618 | 54987 Crushing Emerald Dragon Ornament | **PASS** | model IT11896 OD lon11.eqg EQGM v3; ornament aug, model OK |
| Crushing Fallen Dragon Ornament | 100023366 | 64563 Crushing Fallen Dragon Ornament | **FIX** | aug idfile IT63 -> IT12079 (lon13.eqg EQGM v3); today the weapon look never changes |
| Crushing Vermillion Dragon Ornament | 100022597 | 60949 Crushing Vermillion Dragon Ornament | **FIX** | aug idfile IT63 -> IT11992 (lon12.eqg EQGM v3); today the weapon look never changes |
| Crystalline Shield Nimbus | 100020462 | 54918 Crystalline Shield Nimbus | **PASS** | nimbus 610, spell matches spells_us |
| Dagger Ornament of the Void | 100010795 | 40790 Dagger Ornament of the Void | **FIX** | aug idfile IT63 -> IT11346 (lon06.eqg EQGM v3); today the weapon look never changes |
| Dagger of Avanish Ornament | 100020432 | 54923 Dagger of Avanish Ornament | **PASS** | model IT11658 OD lon10.eqg EQGM v3; ornament aug, model OK |
| Dagger of Skulls | 100013999 | 47708 Dagger of Skulls Ornament | **PASS** | model IT11586 OD lon08.eqg EQGM v3; ornament aug, model OK; live also gave a skull-merc combine part (not granted) |
| Dallyn's Horse | 100023790 | 67890 Dallyn's Horse Saddle | **PASS** | mount SumUNMm10Run2: horses row + archive OK |
| Dallyn's Taiko Stick Ornament | 100023800 | 67895 Dallyn's Taiko Stick Ornament | **PASS** | model IT12350 OD lon14.eqg EQGM v3; ornament aug, model OK |
| Dancing Blade of Fire | 100014001 | 47713 Dancing Blade of Fire | **FIX** | pet type SwarmPetLon8Slash has no pets row and no npc_types row: click says "Unable to find data for pet" |
| Dancing Dagger of Thunder | 100014044 | 47712 Dancing Dagger of Thunder | **FIX** | pet type SwarmPetLon8Pierce has no pets row and no npc_types row: click says "Unable to find data for pet" |
| Dancing Shard of Ice | 100014042 | 47714 Dancing Shard of Ice | **FIX** | pet type SwarmPetLon8Blunt has no pets row and no npc_types row: click says "Unable to find data for pet" |
| Death and Decay | 100021614 | 54984 Death and Decay Nimbus | **PASS** | nimbus 642, spell matches spells_us |
| Discordian Blade Ornament | 100008574 | 40755 Discordant Blade Ornament | **PASS** | model IT11304 OD lon05.eqg EQGM v3; ornament aug, model OK |
| Discordian Dagger Ornament | 100008576 | 40757 Discordant Dagger Ornament | **PASS** | model IT11306 OD lon05.eqg EQGM v3; ornament aug, model OK |
| Discordian Great Blade Ornament | 100008618 | 40756 Discordant Great Blade Ornament | **PASS** | model IT11305 OD lon05.eqg EQGM v3; ornament aug, model OK |
| Discordian Great Mace Ornament | 100008614 | 40754 Discordant Great Mace Ornament | **PASS** | model IT11303 OD lon05.eqg EQGM v3; ornament aug, model OK |
| Discordian Spear Ornament | 100008578 | 40758 Discordant Spear Ornament | **PASS** | model IT11307 OD lon05.eqg EQGM v3; ornament aug, model OK |
| Discordian Spikefist Ornament | 100008628 | 40759 Discordant Spikefist Ornament | **PASS** | model IT11308 OD lon05.eqg EQGM v3; ornament aug, model OK |
| Discordian War Hammer Ornament | 100008616 | 40753 Discordant War Hammer Ornament | **PASS** | model IT11302 OD lon05.eqg EQGM v3; ornament aug, model OK |
| Doll's Mask | 100023358 | 64557 Doll's Mask | **PASS** | illusion race 638 (BEL) loadable |
| Draconic Blade Ornamentation | 100006095 | 40663 Draconic Blade Ornamentation | **FIX** | remap mold 40663 -> NEW aug 152670, idfile IT11276 (lon02.eqg EQGM v1) |
| Draconic Dagger Ornamentation | 100006099 | 40665 Draconic Dagger Ornamentation | **FIX** | remap mold 40665 -> NEW aug 152672, idfile IT11278 (lon02.eqg EQGM v1) |
| Draconic Fist Ornamentation | 100006101 | 40667 Draconic Fist Ornamentation | **FIX** | remap mold 40667 -> NEW aug 152674, idfile IT11280 (lon02.eqg EQGM v3) |
| Draconic Great Blade Ornamentation | 100006091 | 40664 Draconic Great Blade Ornamentation | **FIX** | remap mold 40664 -> NEW aug 152671, idfile IT11277 (lon02.eqg EQGM v1) |
| Draconic Great Hammer Ornamentation | 100006093 | 40662 Draconic Great Hammer Ornamentation | **FIX** | remap mold 40662 -> NEW aug 152669, idfile IT11275 (lon02.eqg EQGM v1) |
| Draconic Hammer Ornamentation | 100006097 | 40661 Draconic Hammer Ornamentation | **FIX** | remap mold 40661 -> NEW aug 152668, idfile IT11274 (lon02.eqg EQGM v3) |
| Draconic Spear Ornamentation | 100006103 | 40666 Draconic Spear Ornamentation | **FIX** | remap mold 40666 -> NEW aug 152673, idfile IT11279 (lon02.eqg EQGM v1) |
| Dragon Brood Crypt Key | 100014003 | 47720 Dragon Brood Crypt Key | **DECISION** | key for the level-85 LoN Dragon Crypt mission (charm ITEMLON8MissionKey); no such content here |
| Dragon Embossed Belt | 100023820 | 67902 Dragon Embossed Belt | **PASS** | click 21916 in spells_us; live reqlevel 85, peq 0 (out-of-era list) |
| Dragon Marked Amulet | 100023379 | - | **OUT** | design decision: stays unmapped |
| Dragonflame Ring | 100022632 | 60954 Dragonflame Ring | **PASS** | live reqlevel 85, peq 0 (out-of-era list) |
| Dragorn Contract | 100022601 | 60942 Dragorn Contract | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Dragorn War Mask | 100022599 | 60940 Dragorn War Mask | **PASS** | illusion race 413 (DDM) loadable |
| Drake of Pure Nature | 100021600 | 54977 Drake of Pure Nature | **FIX** | pet type FamiliarPureNature has no pets row and no npc_types row: click says "Unable to find data for pet" |
| Earring of the Ancient One | 100020472 | 54944 Earring of the Ancient One | **PASS** | click 6771 in spells_us; live reqlevel 80, peq 0 (out-of-era list) |
| Ember Wurm Saddle | 100021610 | 54982 Ember Wurm Saddle | **PASS** | mount SumMWMm1Run2: horses row + archive OK |
| Emblem of Druushk | 100021631 | 54992 Emblem of Druushk | **PASS** | live reqlevel 80, peq 0 (out-of-era list) |
| Essence of the Banshee | 100010857 | 40780 Essence of the Banshee | **PASS** | illusion race 487 (BSE) loadable |
| Estarim Family Crest Banner | 100023810 | 67905 Estarim Family Crest | **FIX** | banner not placeable: placeable 0->1, fvnodrop 1->0, UNK226 0->1; model IT23638 furniture15 OK |
| EverQuest 1-Month Subscription | 100005351 | - | **OUT** | design decision: stays unmapped |
| Familiar of Hulcror | 100005343 | 40629 Familiar of Hulcror | **PASS** | pet FamiliarBazu row OK |
| Familiar of King Gelaqua | 100007519 | 40731 Familiar of King Gelaqua | **PASS** | pet FamiliarIcyGelidran row OK |
| Familiar of Lady Vox | 100006069 | 40671 Familiar of Lady Vox | **PASS** | pet FamiliarFrostDragon row OK |
| Familiar of Lightningcaller Vhalek | 100005347 | 40631 Familiar of Lightningcaller Vhalek | **PASS** | pet FamiliarLightningWarrior row OK |
| Familiar of Lord Nagafen | 100005341 | 40628 Familiar of Lord Nagafen | **PASS** | pet FamiliarDragon row OK |
| Familiar of Nexona | 100007465 | 40729 Familiar of Nexona | **PASS** | pet FamiliarDragonSeer row OK |
| Familiar of Phara Dar | 100006119 | 40643 Familiar of Phara Dar | **PASS** | pet FamiliarAzureDragon row OK |
| Familiar of Queen Eletyl | 100006073 | 40668 Familiar of Queen Eletyl | **PASS** | pet FamiliarQueenHarpy row OK |
| Familiar of Queen Pyrilonis | 100007521 | 40732 Familiar of Queen Pyrilonis | **PASS** | pet FamiliarFieryPyrelin row OK |
| Familiar of Severilous | 100007013 | 40700 Familiar of Severilous | **FIX** | pet type FamiliarGildedDragon has no pets row and no npc_types row: click says "Unable to find data for pet" |
| Familiar of The Girplan Guardian | 100005345, 100019556 | 40630 Familiar of the Girplan Guardian | **PASS** | pet FamiliarGirplan row OK |
| Familiar of The Stone Hive | 100005349 | 40632 Familiar of the Stone Hive | **PASS** | pet FamiliarBixie row OK |
| Familiar of Trakanon | 100008584 | 40762 Familiar of Trakanon | **PASS** | pet FamiliarEmeraldDragon row OK |
| Familiar of Valik the Cruel | 100006115, 100009231 | 40670 Familiar of Valik the Cruel | **PASS** | pet FamiliarStoneGargoyle row OK |
| Familiar of Warden Hanvar | 100007517 | 40730 Familiar of Warden Hanvar | **PASS** | pet FamiliarMinotaur row OK |
| Familiar of Zi-Thuuli, the Granite Claw | 100006071 | 40669 Familiar of Zi-Thuuli, the Granite Claw | **PASS** | pet FamiliarRunicGargoyle row OK |
| Familiar of the Armored Worg | 100008580 | 40764 Familiar of the Armored Worg | **PASS** | pet FamiliarSpikedWorg row OK |
| Familiar of the Bitter Brownie | 100006943 | 40699 Familiar of the Bitter Brownie | **PASS** | pet FamiliarBitterBrownie row OK |
| Familiar of the Gorged Eye | 100006947 | 40704 Familiar of the Gorged Eye | **PASS** | pet FamiliarGorgedEye row OK |
| Familiar of the Hooded Scrykin | 100007001 | 40703 Familiar of the Hooded Scrykin | **PASS** | pet FamiliarHoodedScrykin row OK |
| Familiar of the Kobold King | 100006999 | 40701 Familiar of the Kobold King | **PASS** | pet FamiliarKoboldKing row OK |
| Familiar of the Phased Scrykin | 100006945 | 40702 Familiar of the Phased Scrykin | **PASS** | pet FamiliarPhasedScrykin row OK |
| Familiar of the Runebound Goblin | 100008626 | 40761 Familiar of the Runebound Goblin | **PASS** | pet FamiliarRuneboundGoblin row OK |
| Familiar of the Spiroc Roostmaster | 100008582 | 40763 Familiar of the Spiroc Roostmaster | **PASS** | pet FamiliarCrimsonSpiroc row OK |
| Familiar of the Zombie Doorman | 100007467, 100012433 | 40728 Familiar of the Zombie Doorman | **PASS** | pet FamiliarZombieHostess row OK |
| Firescale Wrulon Saddle | 100022603 | 60945 Firescale Wrulon Saddle | **PASS** | mount SumMWRm1Run2: horses row + archive OK |
| Fist of Avanish Ornament | 100020470 | 54925 Fist of Avanish Ornament | **PASS** | model IT11660 OD lon10.eqg EQGM v3; ornament aug, model OK |
| Fist of the Emerald Dragon | 100021624 | 54989 Fist of the Emerald Dragon Ornament | **PASS** | model IT11900 OD lon11.eqg EQGM v3; ornament aug, model OK |
| Flame Wurmling | 100022606 | 60958 Flame Wurmling | **DECISION** | house pet: idfile IT63, placeable 0, live has no model either; cannot be placed (needs player housing / house pet or another workaround) |
| Form of Crystalline Heads | 100020464 | 54912 Form of Crystalline Heads | **PASS** | illusion race 615 (HYC) loadable |
| Form of Crystalline Skittering | 100020468 | 54914 Form of Crystalline Skittering | **PASS** | illusion race 647 (CSE) loadable |
| Form of the Recluse | 100019528 | 50853 Form of the Recluse | **PASS** | illusion race 440 (TAR) loadable |
| Frost Wurm Saddle | 100021612 | 54983 Frost Wurm Saddle | **PASS** | mount SumMWMm4Run2: horses row + archive OK |
| Gargoyle Mercenary | 100023828 | 67904 Gargoyle Contract | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Gnome Pirate Contract | 100021606 | 54980 Gnome Pirate Contract | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Goblinoid Contract | 100019530 | 50868 Goblinoid Contract | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Great Axe Ornament of the Void | 100010847 | 40789 Great Axe Ornament of the Void | **FIX** | aug idfile IT63 -> IT11345 (lon06.eqg EQGM v3); today the weapon look never changes |
| Great Blade of Bones | 100014006 | 47707 Great Blade of Bones Ornament | **PASS** | model IT11585 OD lon08.eqg EQGM v3; ornament aug, model OK; live also gave a skull-merc combine part (not granted) |
| Great Sword of Avanish Ornament | 100020434 | 54922 Great Sword of Avanish Ornament | **PASS** | model IT11655 OD lon10.eqg EQGM v3; ornament aug, model OK |
| Guard of the Fallen Dragon Ornament | 100023375 | 64566 Guard of the Fallen Dragon Ornament | **FIX** | aug idfile IT63 -> IT12085 (lon13.eqg EQGM v3); today the weapon look never changes |
| Guard of the Vermillion Dragon Ornament | 100022608 | 60952 Guard of the Vermillion Dragon Ornament | **FIX** | aug idfile IT63 -> IT11997 (lon12.eqg EQGM v3); today the weapon look never changes |
| Guise of the Predator | 100021602 | 54978 Guise of the Predator | **PASS** | illusion race 609 (RPT) loadable |
| Hammer Ornament of the Void | 100010797 | 40786 Hammer Ornament of the Void | **FIX** | aug idfile IT63 -> IT11342 (lon06.eqg EQGM v3); today the weapon look never changes |
| Hammer of Bones | 100014008 | 47704 Hammer of Bones Ornament | **PASS** | model IT11582 OD lon08.eqg EQGM v3; ornament aug, model OK; live also gave a skull-merc combine part (not granted) |
| Harness of the Flaming Beast | 100020436 | 54932 Harness of the Flaming Beast | **PASS** | pet FamiliarFPBolrog row OK |
| Heavy Throwing Dagger | 100024251 | 72263 Heavy Throwing Dagger | **DECISION** | spell 37654 = SPA 10 only (no knockdown); same as live data, works as a no-op |
| Hermit's Hideaway | 100021646 | 54999 Hermit's Hideaway | **TEST** | house exterior IT20022 (phexterior1a, OnDemand); placeable=1 - needs player housing; confirm it accepts a house exterior |
| Highland Cragslither Saddle | 100024233 | 72275 Highland Cragslither Saddle | **PASS** | mount SumMCSm3Run2: horses row + archive OK |
| Idol of Severilous | 100021642 | 54997 Idol of Severilous | **PASS** | model IT11907 OD lon11.eqg EQGM v3; trophy "benefit" text is cosmetic only |
| Idol of Talendor | 100022610 | 60957 Idol of Talendor | **PASS** | model IT12001 OD lon12.eqg EQGM v3; trophy "benefit" text is cosmetic only |
| Idol of the Fallen | 100023381 | 32322 Idol of the Fallen | **FIX** | WRONG ITEM 32322 (a shield) -> 64571 (trophy, IT12089 furniture11, OnDemand) |
| Iksar Skeleton Illusion | 100023822 | 67900 Strange Bone Mask | **PASS** | illusion race 606 (SKI) loadable |
| Jade Hydra Saddle | 100012353 | 43969 Jade Hydra Saddle | **PASS** | mount SumHydraGrnFast: horses row + archive OK |
| Jaled Dar's First Shielder's Pendant | 100010799 | 40775 Jaled Dar's First Shielder's Pendant | **PASS** | item + flags OK |
| Kiss of Erollisi Marr | 100005305 | 40610 Kiss of Erollisi Marr | **PASS** | click 12923 in spells_us |
| Knuckle Ornament of the Void | 100010855 | 40792 Knuckle Ornament of the Void | **FIX** | aug idfile IT63 -> IT11348 (lon06.eqg EQGM v3); today the weapon look never changes |
| Kurn's Blood Drop Earring | 100008586 | 40767 Kurn's Blood Drop Earring | **PASS** | item + flags OK |
| Legendary Norrathian Standard | 100005329 | 40622 Legendary Norrathian Standard | **DECISION** | click 205 placeholder + scriptfileid 9386, no script: no guild banner |
| Lightforged Claw Ornament | 100024243 | 72266 Lightforged Claw Ornament | **PASS** | model IT12514 OD lon15.eqg EQGM v3; ornament aug, model OK |
| Lightforged Dagger Ornament | 100024247 | 72265 Lightforged Dagger Ornament | **PASS** | model IT12513 OD lon15.eqg EQGM v3; ornament aug, model OK |
| Lightforged Longsword Ornament | 100024245 | 72264 Lightforged Longsword Ornament | **PASS** | model IT12512 OD lon15.eqg EQGM v3; ornament aug, model OK |
| Lightforged Shield Ornament | 100024241 | 72267 Lightforged Shield Ornament | **PASS** | model IT12516 OD lon15.eqg EQGM v3; ornament aug, model OK |
| Lightweaver's Enchanting Wand | 100008588 | 40760 Lightweaver's Enchanting Wand | **DECISION** | click 119 placeholder (live: server script); fireworks never fire |
| Mace of Avanish Ornament | 100020438 | 54919 Mace of Avanish Ornament | **PASS** | model IT11656 OD lon10.eqg EQGM v3; ornament aug, model OK |
| Magi Shield Ornamentation | 100012447 | 43994 Magi Shield Ornamentation | **FIX** | remap mold 43994 -> aug 44992; 44992 idfile IT63 -> IT11530 (lon07, OnDemand) |
| Mask of Nurga | 100010849 | 40778 Mask of Nurga | **PASS** | illusion race 137 (KGO) loadable |
| Mask of Scaled Cunning | 100022642 | 60941 Mask of Scaled Cunning | **PASS** | illusion race 581 (WYR) loadable |
| Mastruq Bone Mask | 100024231 | 72272 Mastruq Bone Mask | **PASS** | illusion race 402 (ISE) loadable |
| Mechamatic Remote Control | 100012330 | 43988 Mechamatic Remote Control | **PASS** | pet FamiliarClockworkToy row OK |
| Meleen's Dagger Ornament | 100023802 | 67896 Meleen's Dagger Ornament | **PASS** | model IT12351 OD lon14.eqg EQGM v3; ornament aug, model OK |
| Meleen's Horse | 100023792 | 67891 Meleen's Horse Saddle | **PASS** | mount SumUNMm11Run2: horses row + archive OK |
| Merc Contract - Werewolf | 100024269 | 72271 Werewolf Contract | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Metamorph Totem: Albino Rat | 100024249 | 72268 Metamorph Totem: Albino Rat | **TEST** | pet illusion: peq targettype 38 vs spells_us 6; model OK (GlobalLoad_chr) |
| Metamorph Totem: Turepta | 100024267 | 72269 Metamorph Totem: Turepta | **TEST** | pet illusion: peq targettype 38 vs spells_us 6; model OK (GlobalLoad_chr) |
| Miniature Eyepatch | 100021604 | 54979 Miniature Eyepatch | **PASS** | illusion race 338 (GPM) loadable |
| Minor Stone of Summoning: Air | 100010801 | 40793 Minor Stone of Summoning: Air | **PASS** | pet FamiliarLesserAir row OK |
| Minor Stone of Summoning: Earth | 100010803 | 40794 Minor Stone of Summoning: Earth | **PASS** | pet FamiliarLesserEarth row OK |
| Moonfang's Cub | 100012373 | 43989 Moonfang's Cub | **PASS** | pet FamiliarPantherCub row OK |
| Mystical Contract | 100020440 | 54930 Mystical Contract | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Nature Touched Wurm Saddle | 100023362 | 64561 Nature-Touched Wurm Saddle | **PASS** | mount SumMWMm3Run2: horses row + archive OK |
| Nihil Contract | 100024239 | 72270 Nihil Contract | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Nimbus of Burning Steps | 100010805 | 40784 Nimbus of Burning Steps | **PASS** | nimbus 473, spell matches spells_us |
| Nimbus of Daylight | 100012355 | 43977 Nimbus of Daylight | **PASS** | nimbus 511, spell matches spells_us |
| Nimbus of Discordant Chains | 100008590 | 40749 Nimbus of Discordant Chains | **PASS** | nimbus 465, spell matches spells_us |
| Nimbus of Discordant Energy | 100008592 | 40748 Nimbus of Discordant Energy | **PASS** | nimbus 464, spell matches spells_us |
| Nimbus of Discordant Souls | 100008624 | 40752 Nimbus of Discordant Souls | **PASS** | nimbus 468, spell matches spells_us |
| Nimbus of Discordant Spines | 100008594 | 40750 Nimbus of Discordant Spines | **PASS** | nimbus 466, spell matches spells_us |
| Nimbus of Dust Storms | 100019532 | 50858 Nimbus of Dust Storm | **PASS** | nimbus 572, spell matches spells_us |
| Nimbus of Fire and Ice | 100020442 | 54915 Nimbus of Fire and Ice | **PASS** | nimbus 607, spell matches spells_us |
| Nimbus of Growth | 100006083 | 40658 Nimbus of Growth | **PASS** | nimbus 453, spell matches spells_us |
| Nimbus of Mata Muram | 100008622 | 40751 Nimbus of Mata Muram | **PASS** | nimbus 467, spell matches spells_us |
| Nimbus of Midnight | 100012357 | 43978 Nimbus of Midnight | **PASS** | nimbus 512, spell matches spells_us |
| Nimbus of Runes | 100019534 | 50855 Nimbus of Runes | **PASS** | nimbus 570, spell matches spells_us |
| Nimbus of Rushing Water | 100010807 | 40785 Nimbus of Rushing Water | **PASS** | nimbus 474, spell matches spells_us |
| Nimbus of the Air Queen | 100007005 | 40690 Nimbus of the Air Queen | **PASS** | nimbus 457, spell matches spells_us |
| Nimbus of the Burning Prince | 100005325, 100019558 | 40620 Nimbus of the Burning Prince | **PASS** | nimbus 447, spell matches spells_us |
| Nimbus of the Burning Pyrilen | 100007509 | 40719 Nimbus of the Burning Pyrilen | **PASS** | nimbus 462, spell matches spells_us |
| Nimbus of the Crystal Gelidran | 100007469 | 40716 Nimbus of the Crystal Gelidran | **PASS** | nimbus 459, spell matches spells_us |
| Nimbus of the Dead | 100012332 | 43976 Nimbus of the Dead | **PASS** | nimbus 513, spell matches spells_us |
| Nimbus of the Faceless | 100006085 | 40659 Nimbus of the Faceless | **PASS** | nimbus 451, spell matches spells_us |
| Nimbus of the Frozen Gelidran | 100007507, 100010839 | 40717 Nimbus of the Frozen Gelidran | **PASS** | nimbus 461, spell matches spells_us |
| Nimbus of the Gambler | 100019536 | 50857 Nimbus of the Gambler | **PASS** | nimbus 571, spell matches spells_us |
| Nimbus of the Howling Wind | 100010809 | 40782 Nimbus of the Howling Wind | **PASS** | nimbus 471, spell matches spells_us |
| Nimbus of the Inquisition | 100012383 | 43974 Nimbus of the Inquisition | **PASS** | nimbus 509, spell matches spells_us |
| Nimbus of the Jester | 100007003 | 40689 Nimbus of the Jester | **PASS** | nimbus 456, spell matches spells_us |
| Nimbus of the Lover | 100006113 | 40660 Nimbus of the Lover | **PASS** | nimbus 450, spell matches spells_us |
| Nimbus of the Molten Pyrilen | 100007471 | 40718 Nimbus of the Molten Pyrilen | **PASS** | nimbus 460, spell matches spells_us |
| Nimbus of the Ocean Lord | 100006933 | 40691 Nimbus of the Ocean Lord | **PASS** | nimbus 458, spell matches spells_us |
| Nimbus of the Plaguebringer | 100005323 | 40619 Nimbus of the Plaguebringer | **PASS** | nimbus 446, spell matches spells_us |
| Nimbus of the Rain Keeper | 100006081 | 40657 Nimbus of the Rain Keeper | **PASS** | nimbus 452, spell matches spells_us |
| Nimbus of the Riftseeker | 100007473 | 40720 Nimbus of the Riftseeker | **PASS** | nimbus 463, spell matches spells_us |
| Nimbus of the Rumbling Earth | 100010812 | 40781 Nimbus of the Rumbling Earth | **PASS** | nimbus 470, spell matches spells_us |
| Nimbus of the Storm Lord | 100005327 | 40621 Nimbus of the Storm Lord | **PASS** | nimbus 448, spell matches spells_us |
| Nimbus of the Tranquil | 100006931 | 40688 Nimbus of the Tranquil | **PASS** | nimbus 455, spell matches spells_us |
| Nimbus of the Void | 100010814 | 40783 Nimbus of the Void | **PASS** | nimbus 472, spell matches spells_us |
| Nimbus of the Wisp | 100006929 | 40687 Nimbus of the Wisp | **PASS** | nimbus 454, spell matches spells_us |
| Noble Dojorn's Lamp | 100005307 | 40611 Noble Dojorn's Lamp | **PASS** | click 12924 in spells_us |
| Onyx Hydra Saddle | 100012359 | 43970 Onyx Hydra Saddle | **PASS** | mount SumHydraBlkFast: horses row + archive OK |
| Ornate Flying Carpet | 100024229 | 72276 Ornate Flying Carpet | **PASS** | mount SumMCPm0Run2: horses row + archive OK |
| Painting: Balebri Estarim | 100023816 | 67908 Painting: Balebri Estarim | **PASS** | model IT23641 OD furniture15.eqg EQGM v3 |
| Painting: Belt of Purity | 100024263 | 72282 Painting: Belt of Purity | **PASS** | model IT23648 OD lon15.eqg EQGM v3 |
| Painting: Dagda Icefury | 100024257 | 72279 Painting: Dagda Icefury | **PASS** | model IT23645 OD lon15.eqg EQGM v3 |
| Painting: Estarim Family | 100023385 | - | **FIX** | NEW item 64570 (clone 67906; model IT23346 furniture11.eqg, OnDemand-indexed) |
| Painting: Faithful Odyssey | 100024265 | 72283 Painting: Faithful Odyssey | **PASS** | model IT23649 OD lon15.eqg EQGM v3 |
| Painting: Fancy Dress Party | 100024219 | - | **OUT** | design decision: stays unmapped |
| Painting: Fit for Kings | 100021634 | 54993 Painting: Fit For Kings | **PASS** | model IT11903 OD lon11.eqg EQGM v3 |
| Painting: Form of the Fighter | 100024255 | 72278 Painting: Form of the Fighter | **PASS** | model IT23644 OD lon15.eqg EQGM v3 |
| Painting: Fortress of Fervor | 100024261 | 72281 Painting: Fortress of Fervor | **PASS** | model IT23647 OD lon15.eqg EQGM v3 |
| Painting: Ice Dragon | 100021636 | 54994 Painting: Ice Dragon Avatar | **PASS** | model IT11904 OD lon11.eqg EQGM v3 |
| Painting: Lady Vox | 100021638 | 54995 Painting: Lady Vox | **PASS** | model IT11905 OD lon11.eqg EQGM v3 |
| Painting: Lord Nagafen | 100021640 | 54996 Painting: Lord Nagafen | **PASS** | model IT11906 OD lon11.eqg EQGM v3 |
| Painting: Meleen Estarim | 100023812 | 67906 Painting: Meleen Estarim | **PASS** | model IT23639 OD furniture15.eqg EQGM v3 |
| Painting: Paladin's Crusade | 100024259 | 72280 Painting: Paladin's Crusade | **PASS** | model IT23646 OD lon15.eqg EQGM v3 |
| Painting: Santug Suit | 100024361 | - | **FIX** | NEW custom row (no live item found); model IT60000 lon15.eqg OnDemand-indexed; icon guessed |
| Painting: Steed of War | 100024253 | 72277 Painting: Steed of War | **PASS** | model IT23643 OD lon15.eqg EQGM v3 |
| Painting: Strength of Ganak | 100022630 | 60955 Strength of Ganak Painting | **PASS** | model IT11999 OD lon12.eqg EQGM v3 |
| Painting: Tahka | 100023814 | 67907 Painting: Tahka Estarim | **PASS** | model IT23640 OD furniture15.eqg EQGM v3 |
| Painting: Terris Thule | 100022638 | 60956 Terris-Thule Painting | **PASS** | model IT12000 OD lon12.eqg EQGM v3 |
| Painting: Unrestrained Fury | 100023383 | 64569 Painting: Unrestrained Fury | **PASS** | model IT23345 OD furniture11.eqg EQGM v3 |
| Path of Decay | 100020444 | 54917 Path of Decay | **PASS** | nimbus 609, spell matches spells_us |
| Path of Fire | 100021616 | 54985 Path of Fire Nimbus | **PASS** | nimbus 641, spell matches spells_us |
| Path of Ice | 100022640 | 60947 Path of Ice Nimbus | **PASS** | nimbus 655, spell matches spells_us |
| Path of Skulls | 100014010 | 47703 Path of Skulls | **PASS** | nimbus 545, spell matches spells_us; live also gave a skull-merc combine part (not granted) |
| Piercing Emerald Dragon | 100021622 | 54988 Piercing Emerald Dragon Ornament | **PASS** | model IT11898 OD lon11.eqg EQGM v3; ornament aug, model OK |
| Piercing Fallen Dragon Ornament | 100023368 | 64564 Piercing Fallen Dragon Ornament | **FIX** | aug idfile IT63 -> IT12081 (lon13.eqg EQGM v3); today the weapon look never changes |
| Piercing Vermillion Dragon Ornament | 100022612 | 60950 Piercing Vermillion Dragon Ornament | **FIX** | aug idfile IT63 -> IT11994 (lon12.eqg EQGM v3); today the weapon look never changes |
| Pike Ornament of the Void | 100010851 | 40791 Pike Ornament of the Void | **FIX** | aug idfile IT63 -> IT11347 (lon06.eqg EQGM v3); today the weapon look never changes |
| Pike of Avanish Ornament | 100020458 | 54924 Pike of Avanish Ornament | **PASS** | model IT11659 OD lon10.eqg EQGM v3; ornament aug, model OK |
| Ping's Palliative Potion Pack | 100005295, 100007475, 100009235, 100014018, 100019566, 100020446 | 40634 Ping's Palliative Potion Pack | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Peaceful Potion Package | 100006915 | 40707 Ping's Potion Package | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Pellucid Potion Pack | 100006089, 100008598, 100014020 | 40673 Ping's Potion Package | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Penetrating Potion Pack | 100006105, 100008600, 100012429 | 40672 Ping's Penetrating Potion Pack | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Peregrinating Potion Pack | 100006107, 100008602, 100014028 | 40675 Ping's Peregrinating Potion Pack | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Petite Potion Pack | 100006087, 100008604, 100012367, 100014022 | 40674 Ping's Potion Package | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Pinning Potion Package | 100006917, 100010837, 100019560, 100022614 | 40708 Ping's Potion Package | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Poignant Potion Pack | 100005293, 100007479, 100008596, 100022616 | 40633 Ping's Poignant Potion Pack | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Practical Potion Pack | 100005299, 100007481, 100012369, 100014024, 100019562, 100020456, 100021648, 100022628, 100024358 | 40636 Ping's Practical Potion Pack | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Prodigious Potion Pack | 100005301, 100007483 | 40637 Ping's Prodigious Potion Pack | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Propagating Potion Package | 100006921, 100010816, 100021650 | 40710 Ping's Propagating Potion Package | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Protective Potion Pack | 100005297, 100007485, 100012371, 100014026, 100019564 | 40635 Ping's Protective Potion Pack | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Ping's Punctuality Potion Package | 100006919, 100010818 | 40709 Ping's Potion Package | **FIX** | potion bag arrives EMPTY: grant the potions (pack table) |
| Plagued Skull Fragment | 100014012 | 47715 Plagued Skull Fragment | **PASS** | pet FamiliarFloatingSkull row OK; live also gave a skull-merc combine part (not granted) |
| Prismatic Dragon Statue Trophy | 100023818 | 67909 Idol of the Prismatic | **PASS** | model IT23642 OD furniture15.eqg EQGM v3; trophy "benefit" text is cosmetic only |
| Protective Mimic | 100019538 | 50870 Protective Mimic | **PASS** | pet FamiliarProtectiveMimic row OK |
| Rallosian Blade Ornamentation | 100005331 | 40623 Rallosian Blade Ornamentation | **FIX** | remap mold 40623 -> NEW aug 152662, idfile IT11129 (sofequip.eqg EQGM v1) |
| Rallosian Claw Ornamentation | 100006020 | 40639 Rallosian Claw Ornamentation | **FIX** | remap mold 40639 -> NEW aug 152667, idfile IT11135 (sofequip.eqg EQGM v1) |
| Rallosian Club Ornamentation | 100005333 | 40624 Rallosian Club Ornamentation | **FIX** | remap mold 40624 -> NEW aug 152663, idfile IT11130 (sofequip.eqg EQGM v1) |
| Rallosian Dagger Ornamentation | 100005335 | 40625 Rallosian Dagger Ornamentation | **FIX** | remap mold 40625 -> NEW aug 152664, idfile IT11131 (sofequip.eqg EQGM v1) |
| Rallosian Spear Ornamentation | 100005337 | 40626 Rallosian Greatsword Ornamentation | **FIX** | remap mold 40626 -> NEW aug 152665, idfile IT11132 (sofequip.eqg EQGM v1) |
| Rallosian Staff Ornamentation | 100005339 | 40627 Rallosian Staff Ornamentation | **FIX** | remap mold 40627 -> NEW aug 152666, idfile IT11133 (sofequip.eqg EQGM v1) |
| Razor's Edge | 100014036 | 47717 Razors Edge | **PASS** | model IT10869 OD tssequip.eqg EQGM v1; pet FamiliarRazorsEdge row OK |
| Rhyan's Horse | 100023794 | 67892 Rhyan's Horse Saddle | **PASS** | mount SumUNMm12Run2: horses row + archive OK |
| Rhyan's Javelin Ornament | 100023804 | 67897 Rhyan's Javelin Ornament | **PASS** | model IT12352 OD lon14.eqg EQGM v3; ornament aug, model OK |
| Riftseeker Blade Ornament | 100007487 | 40723 Riftseeker Blade Ornament | **FIX** | aug idfile IT63 -> IT11290 (lon04.eqg EQGM v3); today the weapon look never changes |
| Riftseeker Claw Ornament | 100007491 | 40727 Riftseeker Claw Ornament | **FIX** | aug idfile IT63 -> IT11301 (lon04.eqg EQGM v3); today the weapon look never changes |
| Riftseeker Glave Ornament | 100007515 | 40726 Riftseeker Glaive Ornament | **FIX** | aug idfile IT63 -> IT11300 (lon04.eqg EQGM v3); today the weapon look never changes |
| Riftseeker Great Blade Ornament | 100007513 | 40724 Riftseeker Great Blade Ornament | **FIX** | aug idfile IT63 -> IT11298 (lon04.eqg EQGM v3); today the weapon look never changes |
| Riftseeker Great Hammer Ornament | 100007511 | 40722 Riftseeker Great Hammer Ornament | **FIX** | aug idfile IT63 -> IT11289 (lon04.eqg EQGM v3); today the weapon look never changes |
| Riftseeker Hammer Ornament | 100007493 | 40721 Riftseeker Hammer Ornament | **FIX** | aug idfile IT63 -> IT11295 (lon04.eqg EQGM v3); today the weapon look never changes |
| Riftseeker Short Spear Ornament | 100007489 | 40725 Riftseeker Short Spear Ornament | **FIX** | aug idfile IT63 -> IT11292 (lon04.eqg EQGM v3); today the weapon look never changes |
| Roadyle's Mask | 100012361 | 43992 Roadyle's Mask | **PASS** | item + flags OK |
| Rotting Contract | 100021608 | 54981 Rotting Contract | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Sarnak Illusion | 100023824 | 67901 Scaley Horned Mask | **PASS** | illusion race 610 (SRK) loadable |
| Scale of Rodcet Nife | 100005303 | 40609 Scale of Rodcet Nife | **PASS** | click 12922 in spells_us |
| Shield of Scraps | 100012449 | 43995 Shield of Scraps | **PASS** | model IT11531 OD lon07.eqg EQGM v3; live reqlevel 70, peq 0 (out-of-era list) |
| Shissar Mercenary | 100023826 | 67903 Shissar Contract | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Signet of Anashti Sul | 100019548 | 50871 Signet of Anashti Sul | **PASS** | item + flags OK |
| Skittering Mount Saddle | 100019540 | 50851 Skittering Mount Saddle | **PASS** | mount SumSpider1Fast: horses row + archive OK |
| Slashing Emerald Dragon | 100021620 | 54986 Slashing Emerald Dragon Ornament | **PASS** | model IT11894 OD lon11.eqg EQGM v3; ornament aug, model OK |
| Slashing Fallen Dragon Ornament | 100023364 | 64562 Slashing Fallen Dragon Ornament | **FIX** | aug idfile IT63 -> IT12077 (lon13.eqg EQGM v3); today the weapon look never changes |
| Slashing Vermillion Dragon Ornament | 100022618 | 60948 Slashing Vermillion Dragon Ornament | **FIX** | aug idfile IT63 -> IT11990 (lon12.eqg EQGM v3); today the weapon look never changes |
| Spear of Bones | 100014014 | 47709 Spear of Bones Ornament | **PASS** | model IT11587 OD lon08.eqg EQGM v3; ornament aug, model OK; live also gave a skull-merc combine part (not granted) |
| Spore Curer Bait | 100020448 | 54931 Spore Curer Bait | **FIX** | pet type FamiliarDCSporali has no pets row and no npc_types row: click says "Unable to find data for pet" |
| Springwork Spider | 100019554 | 47091 Springwork Spider | **FIX** | pet type FamiliarSpringworkSpider has no pets row and no npc_types row: click says "Unable to find data for pet" |
| Staff Ornament of the Void | 100010843 | 40787 Staff Ornament of the Void | **FIX** | aug idfile IT63 -> IT11343 (lon06.eqg EQGM v3); today the weapon look never changes |
| Staff of Bones | 100014016 | 47705 Staff of Bones Ornament | **PASS** | model IT11583 OD lon08.eqg EQGM v3; ornament aug, model OK; live also gave a skull-merc combine part (not granted) |
| Steed of War Saddle | 100024237 | 72273 Steed of War Saddle | **PASS** | mount SumHRSm9Run2: horses row + archive OK |
| Sword Ornament of the Void | 100010832 | 40788 Sword Ornament of the Void | **FIX** | aug idfile IT63 -> IT11344 (lon06.eqg EQGM v3); today the weapon look never changes |
| Sword of Avanish Ornament | 100020450 | 54921 Sword of Avanish Ornament | **PASS** | model IT11654 OD lon10.eqg EQGM v3; ornament aug, model OK |
| Tahka's Horse | 100023796 | 67893 Tahka's Horse Saddle | **PASS** | mount SumUNMm13Run2: horses row + archive OK |
| Tahka's Shield Ornament | 100023808 | 67899 Tahka's Shield Ornament | **PASS** | model IT12354 OD lon14.eqg EQGM v3; ornament aug, model OK |
| Tahka's Spear Ornament | 100023806 | 67898 Tahka's Spear Ornament | **PASS** | model IT12353 OD lon14.eqg EQGM v3; ornament aug, model OK |
| Tarn's Band of Ambition | 100007495 | 40735 Tarn's Band of Ambition | **PASS** | item + flags OK |
| Tear of Tranquility | 100005321, 100014030 | 40618 Tear of Tranquility | **PASS** | click 12931 in spells_us |
| Tekko of the Vermillion Dragon Ornament | 100022620 | 60951 Tekko of the Vermillion Dragon Ornament | **FIX** | aug idfile IT63 -> IT11996 (lon12.eqg EQGM v3); today the weapon look never changes |
| Terrifying Contract | 100022622 | 60943 Terrifying Contract | **DECISION** | merc contract (click 119 + script 13964); Mercs:AllowMercs=false, so it does nothing |
| Tiger Raptor Saddle | 100023360 | 64560 Tiger Raptor Saddle | **PASS** | mount SumMRPm1Run2: horses row + archive OK |
| Title of the Champion of Norrath | 100007497 | 40734 Title of the Champion of Norrath | **FIX** | click 205/7653 placeholder, no titles row: nothing happens. Convert to title certificate (click 36878 + titles row) |
| Title of the Guardian of the Grove | 100010834 | 40796 Title of The Guardian of the Grove | **FIX** | click 205/7653 placeholder, no titles row: nothing happens. Convert to title certificate (click 36878 + titles row) |
| Title of the Hero of the Inquisition | 100006949 | 40705 Title of the Hero of the Inquisition | **FIX** | click 205/7653 placeholder, no titles row: nothing happens. Convert to title certificate (click 36878 + titles row) |
| Title of the Hero of the Oathbound | 100006951 | 40706 Title of the Hero of the Oathbound | **FIX** | click 205/7653 placeholder, no titles row: nothing happens. Convert to title certificate (click 36878 + titles row) |
| Title of the Legend of Norrath | 100008606 | 40766 Title of the Legend of Norrath | **FIX** | click 205/7653 placeholder, no titles row: nothing happens. Convert to title certificate (click 36878 + titles row) |
| Title of the Scoundrel of the Oathbreakers | 100007499 | 40733 Title of the Scoundrel of the Oathbreakers | **FIX** | click 205/7653 placeholder, no titles row: nothing happens. Convert to title certificate (click 36878 + titles row) |
| Title of the Void Seeker | 100008608 | 40765 Title of the Void Seeker | **FIX** | click 205/7653 placeholder, no titles row: nothing happens. Convert to title certificate (click 36878 + titles row) |
| Title of the Voidwalker | 100010845 | 40795 Title of The Voidwalker | **FIX** | click 205/7653 placeholder, no titles row: nothing happens. Convert to title certificate (click 36878 + titles row) |
| Tunarian Blade Ornamentation | 100006937 | 40694 Tunarian Blade Ornamentation | **FIX** | remap mold 40694 -> NEW aug 152677, idfile IT11283 (lon03.eqg EQGM v3) |
| Tunarian Dagger Ornamentation | 100006939 | 40696 Tunarian Dagger Ornamentation | **FIX** | remap mold 40696 -> NEW aug 152679, idfile IT11285 (lon03.eqg EQGM v3) |
| Tunarian Fist Ornamentation | 100007011 | 40698 Tunarian Fist Ornamentation | **FIX** | live has NO aug version; keep mold 40698, set idfile IT63 -> IT11287 (lon03, OnDemand) |
| Tunarian Great Axe Ornamentation | 100007009 | 40695 Tunarian Great Axe Ornamentation | **FIX** | remap mold 40695 -> NEW aug 152678, idfile IT11284 (lon03.eqg EQGM v3) |
| Tunarian Great Staff Ornamentation | 100007007 | 40693 Tunarian Great Staff Ornamentation | **FIX** | remap mold 40693 -> NEW aug 152676, idfile IT11282 (lon03.eqg EQGM v3) |
| Tunarian Spear Ornamentation | 100006941 | 40697 Tunarian Spear Ornamentation | **FIX** | remap mold 40697 -> NEW aug 152680, idfile IT11286 (lon03.eqg EQGM v3) |
| Tunarian Staff Ornamentation | 100006935 | 40692 Tunarian Staff Ornamentation | **FIX** | remap mold 40692 -> NEW aug 152675, idfile IT11281 (lon03.eqg EQGM v3) |
| Umbral Selyrah Saddle | 100024235 | 72274 Umbral Selyrah Saddle | **PASS** | mount SumMSLm2Run2: horses row + archive OK |
| Unwritten Glyph | 100014034, 100022634 | 47747 Unwritten Glyph | **DECISION** | click 205 placeholder + scriptfileid 13471, no script: no glyph granted |
| Vanishing Point | 100014038 | 47716 Vanishing Point | **PASS** | model IT11589 OD lon08.eqg EQGM v1; pet FamiliarVanishingPoint row OK |
| Venemous Raptor Familiar | 100022624 | 60939 Venomous Raptor Familiar | **FIX** | pet type FamiliarVenomousRaptor has no pets row and no npc_types row: click says "Unable to find data for pet" |
| Venemous Raptor Saddle | 100022626 | 60944 Venomous Raptor Saddle | **PASS** | mount SumMRPm0Run2: horses row + archive OK |
| Visage of Emperor Draygun | 100005319 | 40617 Visage of Emperor Draygun | **PASS** | illusion race 467 (SHL) loadable |
| Visage of King Gelaqua | 100005311 | 40613 Visage of the King Gelaqua | **PASS** | illusion race 417 (FRD) loadable |
| Visage of King Gragnar | 100006075 | 40652 Visage of King Gragnar | **PASS** | illusion race 455 (KBD) loadable |
| Visage of Nillipus | 100006079 | 40655 Visage of Nillipus | **PASS** | illusion race 568 (BNM) loadable |
| Visage of Queen Pelzia | 100006077 | 40653 Visage of Queen Pelzia | **PASS** | illusion race 520 (BXI) loadable |
| Visage of Queen Pyrilonis | 100005309 | 40612 Visage of Queen Pyrilonis | **PASS** | illusion race 411 (SCU) loadable |
| Visage of the Aviak Rook | 100007501 | 40713 Visage of the Aviak Rook | **PASS** | illusion race 558 (AVK) loadable |
| Visage of the Bixie Drone | 100006109, 100010841 | 40654 Visage of the Bixie Drone | **PASS** | illusion race 520 (BXI) loadable |
| Visage of the Brownie Noble | 100006111 | 40656 Visage of the Brownie Noble | **PASS** | illusion race 568 (BNM) loadable |
| Visage of the Crystal Golem | 100005315 | 40615 Visage of the Crystal Golem | **PASS** | illusion race 374 (GLM) loadable |
| Visage of the Dark Arachnids | 100019542 | 50854 Visage of the Dark Arachnids | **PASS** | SPA 374 triggers 32821 Illusion: Drachnid (race 461 DCM/DCF, OnDemand); both spells in spells_us |
| Visage of the Embattled Minotaur | 100008610 | 40745 Visage of the Embattled Minotaur | **PASS** | illusion race 574 (MTR) loadable |
| Visage of the Frozen Goblin | 100007503 | 40715 Visage of the Frozen Goblin | **PASS** | illusion race 433 (GBN) loadable |
| Visage of the Girplan | 100023764 | 67065 Visage of the Girplan | **PASS** | illusion race 419 (DDV) loadable |
| Visage of the Hideous Harpy | 100006925, 100012431 | 40685 Visage of the Hideous Harpy | **PASS** | illusion race 527 (HRP) loadable |
| Visage of the Hooded Scrykin | 100006117 | 40642 Visage of the Hooded Scrykin | **PASS** | illusion race 495 (SRN) loadable |
| Visage of the Ice Golem | 100006036, 100009940 | 40638 Visage of the Ice Golem | **PASS** | illusion race 374 (GLM) loadable |
| Visage of the Kobold Serf | 100006923 | 40684 Visage of the Kobold Serf | **PASS** | illusion race 455 (KBD) loadable |
| Visage of the Phased Scrykin | 100005317 | 40616 Visage of the Phased Scrykin | **PASS** | illusion race 495 (SRN) loadable |
| Visage of the Primal Kerran | 100008620 | 40747 Visage of the Primal Kerran | **PASS** | illusion race 562 (KRM) loadable |
| Visage of the Screaming Banshee | 100005313 | 40614 Visage of the Screaming Banshee | **PASS** | illusion race 488 (BSG) loadable |
| Visage of the Siren Enticer | 100007505 | 40714 Visage of the Siren Enticer | **PASS** | illusion race 564 (SIN) loadable |
| Visage of the Spirited Satyr | 100008612 | 40746 Visage of the Spirited Satyr | **PASS** | illusion race 529 (SAT) loadable |
| Visage of the Stone Gargoyle | 100006927 | 40686 Visage of the Stone Gargoyle | **PASS** | illusion race 464 (GGY) loadable |
| Visage of the Vitrik | 100020466 | 54913 Visage of the Vitrik | **PASS** | illusion race 620 (DVL) loadable |
| Visage of the War Pirate | 100019550 | 50872 Visage of the War Pirate | **PASS** | illusion race 340 (OPM) loadable |
| War Bear Saddle | 100020452 | 54934 War Bear Saddle | **PASS** | mount SumBear2Fast: horses row + archive OK |
| War Hammer of Avanish Ornament | 100020454 | 54920 War Hammer of Avanish Ornament | **PASS** | model IT11657 OD lon10.eqg EQGM v3; ornament aug, model OK |
| Wiring Mount Saddle | 100019544 | 50852 Wiring Mount Saddle | **PASS** | mount SumSpider2Fast: horses row + archive OK |
| Writ of the Eagle | 100012363 | 43991 Writ of the Eagle | **FIX** | click 205/7653 placeholder, no titles row: nothing happens. Convert to title certificate (click 36878 + titles row) |
| Writ of the Truthkeeper | 100012365 | 43990 Writ of the Truthkeeper | **FIX** | click 205/7653 placeholder, no titles row: nothing happens. Convert to title certificate (click 36878 + titles row) |
