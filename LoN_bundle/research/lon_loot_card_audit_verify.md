# LoN loot card audit - adversarial verification (2026-10-06)

This is a read-only check of `lon_loot_card_audit.md`. No database row, client file or process was
changed. The scratch scripts and evidence (an independent EQGM/OnDemand resolver, mount / race / material /
silhouette / icon / Lucy-raw checkers, the Lucy raw pages and the PNGs) are internal tools, not included.

## Verdict counts

| | Findings |
|---|---|
| CONFIRMED | 24 |
| WRONG (correction below) | 6 |
| UNVERIFIABLE (in-game test) | 4 |
| NEW (the audit missed these) | 3 |

## WRONG findings and their corrections

| # | Audit claim | What the evidence shows | Correction |
|---|---|---|---|
| W1 | Section 4 SQL plus the header: "edit the BASE item only; tier rows follow the base". | Nothing copies `idfile` to the tier rows. Our item-tier generator skips ornament augs and only touches stats. 34 of the 35 ids have Enchanted and Legendary rows that still say IT63 (Brell's says IT0): 1040786/2040786 ... 1064567/2064567, 1044992/2044992, 1050873/2050873. Only 40698 has none. | Apply the same CASE to `id + 1000000` and `id + 2000000`, for example `UPDATE items b JOIN items t ON t.id IN (b.id+1000000, b.id+2000000) SET t.idfile = b.idfile WHERE b.id IN (...)` after the base update. Do the same in any health check that guards these rows. |
| W2 | Brell's Hammer Ornament IT0 "counted as an ornament with model 0 - the weapon would vanish". | `IsOrnamentationAugment` is true ("IT0" is not IT63/IT64), but `UpdateOrnamentationInfo` stores ornament idfile 0. rof2.cpp:6669 then sends no ornament string, and mob_appearance.cpp:225/281 falls back to the weapon's own idfile when the value is 0. The model never vanishes. Only the ornament icon changes (to icon 3289). | The fix value IT11616 is still right. Only the stated symptom is wrong. |
| W3 | Section 5: "/claim delivers the bag only, so add a `lon_loot_item_bundle` table and change `HandleRedeemItemGrant`". | `Client::TryReward` (client.cpp:7771-7782) already puts template rows 1..7 inside the claim item when it is a container (ItemClass 1). `Zone::LoadVeteranRewards` (zone.cpp:2940) loads rows `reward_slot < 8` in order. | No new table and no code change. Add `veteran_reward_templates` rows to the existing claim id `5000000+bag` with `reward_slot` 2.., `charges` = quantity (CreateItem charges are the stack count; potions stack to 20). Because the existing template row puts the bag at `reward_slot` 1, at most **6** contents fit (rows 2-7). Moving the bag row to slot 0 allows 7. The largest pack needs 4 distinct contents, so every Ping pack and Bones pack fits. Zones load templates at boot, so the zones must be restarted. Bag slots are enough: the five Ping packs hold 10, the others 5, and the Bones packs 2. |
| W4 | Section 7 note about two non-LoN paintings. | Server-specific data note, not applicable outside our server. | The note was dropped. |
| W5 | Santug Suit: "clone 67717 Painting: Santug's Stockings, override idfile + lore". | 67717 has placeable 0 in our data, UNK228/229 0 and a different mesh family (furniture24, flat in Y). The clone would arrive unplaceable. | Clone **72277 Painting: Steed of War**. Its IT23643 is the same 80-vertex lon15 painting mesh as IT60000, flat in Z. Its flags are placeable 1, nodrop 1, norent 1, fvnodrop 0, UNK226 1. Override idfile IT60000 and the name/lore. I checked the faces of IT60000: they use material `Santugs_suit_c.dds`, so the model choice is right. The id (67718 is free) and the icon are still design picks. |
| W6 | Brell's/Riftseeker etc. "today the weapon look never changes": implied this is the only effect. | For the IT63 augs it is the only effect (not an ornament at all). For Brell's IT0 see W2: the icon does change. | Wording only. |

## NEW findings (the audit missed these)

| # | Finding | Evidence | What to do |
|---|---|---|---|
| N1 | The Vermillion and Fallen slashing/crushing/piercing augs carry augrestrict **16/17/18** (60948-60950, 64562-64564; live Lucy agrees). EQEmu names these `AugRestrictionUnknown1-3`, and `Client::IsAugmentRestricted` returns "restricted" for them (inventory.cpp:6134-6138). | The server does not block them today because `Inventory:EnforceAugmentRestriction=false` (rule_values ruleset 1). The RoF2 client runs its own check at 0x7B2410; our DLL hook only changes case 1, and I could not decompile 0x7B2410 (Ghidra has no eqgame loaded). | In-game test T1. If the client refuses, set those six to 5/6/7 (1H only) or 0 (a gameplay decision). |
| N2 | Copies that are already socketed keep the old look after the idfile fix. | Character inventory load (shareddb.cpp:839-841) restores the stored `ornament_idfile`/icon and only recomputes when both are 0. The rof2 serializer sends the stored value. mob_appearance re-derives from the aug, so other players see the new model, but the holder's item data does not change until the aug is re-socketed. | Testers should socket a fresh copy. Players with old copies re-socket. |
| N3 | Path of Skulls Pack (47736) and Plagued Skull Fragment Pack (47735) have each other's lore. | 47735 lore says "a skull and a empty glowing skull", but its card grants the Trapped Soul Gem. 47736 says "skull fragment and a trapped soul gem", but its card grants the Empty Glowing Skull. | Only matters if the skull parts are granted. Follow the card text. |

## Section-by-section

### 1. PASS sample (127 rows checked, no PASS broken)

- **Mounts (24/24).** Checked 40601-40604, 40644, 40646, 40679, 40711, 40743, 40776, 43969, 50851/2, 54933, 54982, 60944/5, 64560/1, 67889, 72273-72276. For each: the horses row is present, the race resolves to a tag in eqlib Races.h, `<tag>.eqg` is OnDemand-indexed (EQGS/EQGA/EQGL lines) and present, the `.mds` is EQGS v1 with the `MOUNT01` dag, and the texture variant exists.
  - One caveat on the audit's method: HRS texture 9 (Steed of War) has **no** `c_hrs_*_s09` file. `hrs.lay` defines `C_HRS_S09_M01`, and `hrs.mds` carries the `barding_hrs09`/`saddle_hrs09` materials, so the variant is defined. The PASS verdict holds, but the criterion the audit stated is not what proves it. hrs.eqg md5 matches stock RoF2.
- **Illusions (34/34).** All SPA 58 races resolve. Most are OnDemand `<tag>.eqg`; KGO/ISE/BXI come from `GlobalLoad_chr.txt`; KBD, OPM/OPF, RPT, GPM, DKE and FNG come from `GlobalLoad.txt`.
- **Spells (44 rows).** These are click, pet, mount, illusion, potion and title spells. Every column of `spells_new` matches the shipped `spells_us.txt` (root and `Resources\` both md5 7c8e8673) except `deleteable`.
- **Ornament/furniture models (25).** 40755/6/9, 47706, 47716-18, 54922, 54926, 54986, 54991, 54993, 54997, 60955, 60957, 64593, 64597, 67894, 67899, 67908, 67909, 72264, 72267, 72282, 72283 are all OnDemand-indexed, the archive is present and the EQGM version reads. The paintings and idols have placeable 1, and the wall paintings have UNK226 1.
- **Archives.** lon02/03/04/06/07/12/13/15, sofequip, wallet14, furniture11/15 and phexterior1a are md5-identical to the RoF2 client. No DLL/GlobalLoad/OnDemand change is needed: CONFIRMED.
- **Icons.** There are 222 dragitem sheets (icons 500-8491), and the highest mapped icon is 4651. CONFIRMED.
- One minor discrepancy: the audit's slot counts (8,445 type-20 / 8,477 type-4 weapons) do not reproduce. I get 7,297 / 7,305 for weapon itemtypes. It does not matter: both slot types are common.

### 2. New item rows

- **19 aug ornaments 152662-152680: CONFIRMED.**
  - The Lucy raw pages (game ids) give itemtype 54, augvalue 524288, the augres listed, slots 24576/8192, nodrop 0, norent 1, heirloom 1, weight 8, size 0 and the lore text the audit quotes.
  - ZAM 132835 (Draconic Blade) and 132826 (Tunarian Staff) confirm "Augmentation, type 20, PRIMARY SECONDARY, No Trade Heirloom, 1h Slash Only / 1h Blunt Only". Their charm text names the wrong weapon type, which confirms the charmfile scramble.
  - 152681 does not exist on Lucy, and ZAM 83002 is the 1-slot container. Tunarian Fist has no aug: CONFIRMED.
  - No id collides. 152662-152680, +1000000 and +2000000, 54916/64570/67718 and their tiers are all free.
- **Models.** I measured the extents myself and they match the audit. The icons I rendered match the meshes (2228-2232/2582, 2584-2590, 2605-2611).
  - **Draconic (lon02 `dragon_*`) and Tunarian (lon03 `tunarian_*`): CONFIRMED.** Tunarian Staff = IT11281 is live.
  - **Rallosian (sofequip 11129-11135): CONFIRMED as the best available inference.** No live idfile exists: the live molds and augs have a blank idfile, and there is no lon01.eqg in either client. The evidence is circumstantial but strong: IT11128 in the same archive, just before 11129, is the genie lamp (`di_genie_lamp`), which is LoN set 1's Noble Dojorn's Lamp; the `tc_slashing`/`tc_bashing` textures fit Rallos Zek; and the six shapes match the six icons.
- **Template 72264: CONFIRMED sensible.** augtype 524288 is slot type 20, which `IsOrnamentationAugment` accepts. Two notes:
  - The clone inherits `placeable 1`. That is harmless and other real-model ornaments are placeable too.
  - nodrop 0 (No Trade) matches live and 72264, but peq's other LoN ornaments (40755, 54986 ...) are nodrop 1 (tradeable). Tradeability is a design choice.
- **54916 Crown of Lightning: CONFIRMED** (Lucy: icon 3363, slots 4194304, weight 5, size 1, magic 1, spell 24053, which is in spells_us). Two notes:
  - Live is No Trade. The 54915 clone is tradeable in peq, which matches peq's convention for 54915.
  - The card says "small amount of magic resist", but spell 24053 has no resist effect, on live either.
- **64570 Painting: Estarim Family: CONFIRMED** (Lucy: icon 3824, weight 50, size 4, lore). IT23346 uses `estarim_family.dds` and is the same mesh as 64569's IT23345, so cloning 64569 would be even closer than 67906. Either works.
- **Santug Suit:** see W5.

### 3. FIX rows

| Fix | Verdict |
|---|---|
| Idol of the Fallen 32322 -> 64571 | CONFIRMED. 32322 is a shield (itemtype 8, IT67). 64571 is the itemtype 11 trophy (IT12089 furniture11, placeable 1, lore "An idol to honor Vishimtar"). If loot is delivered through /claim it needs template row 5064571. |
| 20 card remaps (section 3.2) | CONFIRMED. The card ids match lon_loot_item. Card 100005337 is named "Spear", but its text says "2-Handed Slashing" and live has no Rallosian spear, so Greatsword is right. If loot is delivered through /claim, every new item needs its own `veteran_reward_templates` row. |
| Magi Shield remap -> 44992 + IT11530 | CONFIRMED (live 152743 = IT11530; lon07 v3). Add the tier rows (W1). |
| Tunarian Fist mold 40698 idfile IT11287 | CONFIRMED. tradeskills.cpp:494 sets ornament idfile = `IDFile+2` and icon = the mold's icon. `ITEMTransfigHTH` maps to itemtype 45 in CanTransform. Today it would set ornament idfile 63. |
| 33 IT63 -> model fixes | CONFIRMED mechanism (item_instance.cpp:553-559). Models: 5 are live (40725 = 11292, 44987 = 11387, 60951 = 11996, 60953 = 11998, 64567 = 12086); 6 more are live through the 1526xx aug twins (11343, 11346, 11347, 11290, 11300, 11389-11391, 11530). The rest follow shape and texture. Riftseeker fire/ice was checked against the icon colours (2626/2629/2631/2632 blue, 2620/2621/2623 orange) and the paired textures (`*icy` = 11295-11301). Stock items already render two of them the same way: Hraxtt 38553 (1H blunt) uses IT11295 and Thulian Lifeclotter 146633 (1H pierce) uses IT12081. One cosmetic caveat: **Crushing Fallen -> IT12079** is the shorter of two long-hafted hammers (5.60, grip centred, 2.6 units of haft behind the hand), and its icon 3923 may correspond to 12080. No live idfile settles it. SQL incomplete: see W1. |
| 8 familiar/swarm pets | CONFIRMED. SPA 108 -> MakeFamiliar (pets.cpp:182) and SPA 152 -> TypesTemporaryPets (aa.cpp:73) both look up `teleport_zone` in `pets`. None of the 8 types has a row. `GetPoweredPetEntry` matches `petpower <= max(power,0)`, so cloning petpower 0/-1 rows works. Dragon familiar textures 0, 1, 3, 4, 5 are taken, so 2 is free. Race 127 with `d_melee_texture1` is the stock animated-weapon pattern (npc 500-504 "Animation1-5"). Races 432/457/559/609 resolve to loadable models. |
| 10 title items | CONFIRMED. global_player.pl:681 calls `AddTitleFlag($item_id)` on spell 36878, and `EnableTitles` runs `quest::enabletitle(id)`, so the `titles.title_set` must equal the item id (like 300001). 36878 is in spells_us and 131 items already use it. No `titles` rows exist for the 10 ids. Copy clicktype 3, casttime 0 and maxcharges 1 from 300001. The title text is a design choice. |
| 67905 banner flags | CONFIRMED (needs player housing). UNK226 is the client's WALL placement mode, and peq's wall banners (City of Bronze 66843-66846, Banner - Nature 66861 ...) have placeable 1 and UNK226 1. The `fvnodrop` change has no gameplay effect. Consider copying UNK228/229 (scale limits) from 66843 too. |

### 4. Ping's packs and Bones packs

- **Contents: CONFIRMED.** I checked them against the card text and ZAM: Practical = 5 Adventure + 10 Harvest + 10 Healing; Palliative = 5 mana + 5 healing; the Potion Packages hold 5 of one potion. All potions exist in peq (stack 20).
- **Delivery:** see W3. There is no 7-limit problem, and no other design is needed.
- **Bones packs** (2-slot bags 47728-47734): /claim can deliver them as bag + ornament + gem rows. Granting the parts stays a design decision (no recipe, mercenaries off): CONFIRMED.

### 5. Ornament rendering

CONFIRMED except W2. `UpdateOrnamentationInfo` (item_instance.cpp:599-630) takes the aug's icon, its HerosForgeModel and `IDFile[2..]`. rof2.cpp:6666-6687 serialises `IT<n>` for both hands, and mob_appearance.cpp:214-227 / 271-282 uses the aug's idfile for the worn model. A weapon shows only the first ornament aug it holds. Note N2 for copies that are already socketed.

### 6. Pets and titles

CONFIRMED (see section 3).

## UNVERIFIABLE from static evidence: final in-game test list

1. **Slashing Vermillion Dragon Ornament 60948, after the IT11990 fix, into a 1H slash weapon.** Can the client insert augrestrict 16 (N1)? Does the look change? This one test also proves the IT63-fix path, so the audit's test 4 can go.
2. **Draconic Blade Ornamentation 152670 (after creation) into a 1H slash weapon.** Checks the cloned row and a v1 lon02 model.
3. **Hermit's Hideaway 54999.** Does a player-housing system place IT20022, a 65 x 53 x 51 house exterior?
4. **Metamorph Totem: Albino Rat 72268 with a pet out.** peq targettype 38 vs spells_us 6.

Optional, if time allows:

- Claim one Ping's Practical pack after adding the template rows. TryReward already proves the bag fill statically.
- Nimbus of the Void 40783. The nimbus index table was not decoded.
- Dancing Blade of Fire, after the rows are added. The mechanism is proven; this test only checks the look and balance values.
