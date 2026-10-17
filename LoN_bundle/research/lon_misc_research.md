# Legends of Norrath -- four small research items (2026-10-06)

Read-only research. Nothing in any repo, DB or Ghidra project was changed. VAs are Ghidra VAs of
`LegendsOfNorrath.dll` at image base 0x10000000 (RVA = VA - 0x10000000). The DLL has no
DYNAMIC_BASE flag (DllCharacteristics = 0), but always patch relative to the HMODULE anyway.
Wire notation follows `research/eqlib_LoN/LoN_messages.txt`: varints, strings = varint len + bytes + NUL,
one (classId, version) pair per class level (our `Writer::Header(cls, ver, levels)` writes the
same pair `levels` times).

---

## A. Custom avatars (ArchCommand 0x1C0 / 0x1C1 / 0x1C2 / 0x1C4)

### A.1 The flow, step by step (all client code quoted from Ghidra)

1. **Dialog opens** -- `EQAvatarCreationDialog` setup **0x102159C0**:
   disables `backButton` and `nextButton`, sets `questionLabel` to
   *"Waiting for information from server..."* (PUID `eqavatarcreationdialog.cpp.0`), then, if
   logged in, builds `ArchCommand_GetArchetypeID` (ctor 0x101DCC30), sets AccountID
   (0x101DBC70) and sends it (0x10068380). ArchID and ErrorCode are left 0.
2. **Server reply: `ArchCommand_GetArchetypeID` 0x1C0.** Its execute **0x101DCCE0** posts UI
   event **0xAA** with `[ArchID (+0x08), ErrorCode (+0x0C)]`.
3. **Event 0xAA** -- dialog eventFilter **0x1021BBB0**:
   - `ErrorCode == 0x9C` (156) -> box *"You have too many custom avatars. You must delete some
     before you can create more."* (`failedArchIDDialog`) and the dialog closes.
   - otherwise `FUN_1021B040(ArchID)` **0x1021B040**:
     - `ArchID == 0` -> `QWidget::close` (dialog silently closes);
     - `ArchID != 0` -> creates a fresh `EQArchetype` (ctor 0x10214230) at dialog+0x48, gives it
       that ArchID, fills default properties (keys 0x2A1, 0x4C, 0x4A, 0x1050, 0x49, 0x115FD,
       0x11600, 0x115FF, 0x115FE, 0x12C, 0x11638, 0x11601; literal values seen include
       100004290 (0x5F5F1C2), the string "100004762", 3, 8, 0x111C8) and enables the wizard.
4. **Wizard** -- five pages (`FUN_10215170` 0x10215170, step at dialog+0x50): profession,
   methods/strategies, trade skill, "what else", name. Race/gender/gametext pickers fill the
   archetype's properties. All local, no traffic.
5. **Name page, Finish** -- the client sends **`ArchCommand_ValidateUsername` 0x1C4**
   (sender 0x10216BE0): AccountID, ErrorCode 0, `username`, `ArchetypeID`.
6. **Server reply: `ArchCommand_ValidateUsername` 0x1C4.** Execute **0x101DDAB0** posts event
   **0xB0** `[ArchetypeID (+0x28), ErrorCode (+0x24)]`. The handler (same eventFilter,
   0x1021BBB0) acts only if `ArchetypeID == the dialog's archetype id` AND step == 5:
   - `ErrorCode 0` -> `FUN_1039ED30` (save, below);
   - `0x4D` "This name is already taken.", `0x4E` "This name is not valid. It contains illegal
     words or phrases.", `0x4F` "An unknown error has occurred." -> back to the name page.
7. **Save** -- `WAAvatarCreationDialog` save **0x1039ED30**: adds the new archetype to the local
   archetype DB and collection (0x10070510 / 0x1006FE10 / 0x10072930), builds
   **`ArchCommand_StoreArchetype` 0x1C1** (ctor 0x101DD4B0), sets AccountID (0x101DBC70) and the
   archetype object (0x101DD5C0 with dialog+0x48), sends it and **closes the dialog at once**.
   It does not wait for anything.
8. **StoreArchetype reply** -- execute **0x101DD5B0 is `return 1`**: a reply is accepted and
   ignored. No reply needed.
9. **Delete** (Collection, sender **0x103DD480**, wadbcollectioncontroller.cpp): sends
   **`ArchCommand_DeleteArchetype` 0x1C2** {AccountID, ErrorCode 0, ArchID} and removes the
   avatar from the local collection itself. Execute **0x101DC920 is `return 1`**. No reply needed.

### A.2 What happens with no reply (today)

GetArchetypeID unanswered: the dialog stays on *"Waiting for information from server..."* with
Back and Next disabled forever. No crash, no timeout; the player can only close the window.
ValidateUsername unanswered: the name page stays waiting the same way. StoreArchetype /
DeleteArchetype need no reply.

### A.3 Exact replies the server must send

Field order is wire order from LoN_messages.txt (base ArchCommand first).

```
0x1C0 ArchCommand_GetArchetypeID v1     (2 class levels)
  Header(0x1C0, 1, 2)          varints C0 03 01 C0 03 01
  U AccountID                  (the session's account)
  U ErrorCode                  0 = ok, 0x9C = too many avatars
  U ArchID                     the NEW archetype id (0 closes the dialog)

0x1C4 ArchCommand_ValidateUsername v1   (2 class levels)
  Header(0x1C4, 1, 2)
  U AccountID
  U ErrorCode                  0 ok / 0x4D taken / 0x4E illegal / 0x4F unknown
  S username                   echo what the client sent
  U ArchetypeID                MUST echo the client's ArchetypeID (the handler compares it)
```

(If the live captures ever show a different header convention for ArchCommand, follow
`HandleRedeemItemGrant`'s `Header(kRedeemItemGrant, 3, 2)` pattern, which is the same shape:
one derived class over a base with AccountID.)

**Incoming 0x1C1 StoreArchetype**: `AccountID, ErrorCode, object(nullable)` -- the object is
`varint flag (0 = present) + varint classId (0x11195 EQArchetype) + body` (body = its own
class-level headers + fields, base `Archetype` 0x0003 v7: ArchetypeID, DefiningArchetypeID,
CardDisplayStyle, FormatSpecificData(object -> PropertySet with name/race/gender/stats/gametext),
NoFoil, OnlyFoil). **Store those object bytes verbatim** -- see A.4 for why.

### A.4 The avatar must come back on every login: `ArchCommand_AddArchetypesToDB` 0x1C3 v2

The client's archetype DB is the shipped `data/archetypes/*.dat`; a custom avatar is not in it.
The server pushes it with **0x1C3** (read **0x101DC630**, execute **0x101DBFE0**):

- v >= 2 body: **vector<string>** (read 0x1000A340: varint count, then that many strings via
  0x101772F0).
- Execute: each string is opened as a byte stream (0x100977A0) and read with the generic
  nullable-object reader **0x1018BED0** (`varint flag; if 0: varint classId; factory; vt+0x24
  read`), then installed in the archetype DB (0x100449D0 / 0x10043A00 / 0x10044A40).
- So each string = exactly the `object(nullable)` bytes the client sent in StoreArchetype.

Send order at login: `AddArchetypesToDB` (all of the account's custom avatars) **before**
`SynchronizationCommand_StartDelivery` / the collection, so the instances and any deck using the
avatar resolve. Header: `Header(0x1C3, 2, 2).U(account).U(count)` then `S(blob)` per avatar
(check that our `S()` writes binary-safe -- the blob can contain NUL bytes; it must write the
length prefix from the vector size, not strlen).

### A.5 ArchID allocation

- Card/avatar archetypes are >= 99M (README). The client treats any id whose **hundred-thousands
  digit is 3 or 4** as the foil printing of `id - 300000` (isFoil 0x100457B0). Custom ids must
  avoid that digit. Suggestion: `200000000 + seq`, seq < 100000 (digit is 0), i.e. an
  AUTO_INCREMENT starting at 200000001.
- Also give the account one collection copy (`lon_card` row, source `avatar:custom`) so it shows
  under Avatars and can be put in a deck.

### A.6 DB design (schema, for the record -- not applied)

```sql
CREATE TABLE lon_custom_avatar (
  archetype_id  INT UNSIGNED NOT NULL AUTO_INCREMENT,   -- starts 200000001 (see A.5)
  account_id    INT UNSIGNED NOT NULL,
  name          VARCHAR(64)  NOT NULL,                  -- from ValidateUsername
  blob          MEDIUMBLOB   NULL,                      -- StoreArchetype object bytes, verbatim; NULL = id reserved, not saved yet
  created_at    DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
  deleted_at    DATETIME     NULL,
  PRIMARY KEY (archetype_id),
  UNIQUE KEY uq_name (name),
  KEY ix_account (account_id, deleted_at)
) ENGINE=InnoDB AUTO_INCREMENT=200000001;
```

Handlers:
- **0x1C0**: count the account's live avatars (`blob IS NOT NULL AND deleted_at IS NULL`); over
  the cap (a configurable number) -> ErrorCode 0x9C, ArchID 0. Else INSERT a reserved row (blob NULL,
  placeholder name like `pending:<id>`) and reply with its id. Reap reserved rows older than a day.
- **0x1C4**: name rules (length, letters, profanity list) -> 0x4E; `name` already used by another
  row -> 0x4D; else set `name` on the reserved row (archetype must belong to this account) and
  reply 0.
- **0x1C1**: archetype id inside the blob must be a reserved row of this account; store `blob`,
  insert the `lon_card` copy. No reply.
- **0x1C2**: set `deleted_at`, delete the `lon_card` copies (and decks using it, or leave them
  invalid -- design call). No reply.
- Login: 0x1C3 with every live blob of the account.

Open: whether the stored PropertySet also needs server-side validation (stats are client-chosen
from the wizard's fixed choices; a modified client could send any numbers). Cheapest guard:
parse the blob with our existing archetype parser and reject property values outside the
wizard's option set.

---

## B. VictoryType for achievements 75 (Inquisitor) / 76 (Oathbound)

### B.1 The value is the LOSER's "loss type"

`EQGame::gameWonByPlayer` **0x10269AC0** (eqgame.cpp; "GAME WON BY PLAYER %d") walks the
loss-type map at EQGame+0x288 (`param_1+0xA2`), takes the value of the player who is not on the
winner's team, and, if the local player won, records it as stat 0x14:

```c
// 0x10269AC0, for every losing player with no entry in the map:
      *map_0x288[loserId] = 0x11177;          // default loss type
...
    if (param_1[0x4a] == param_2) {           // the local player is the winner
      (**(code **)(*param_1 + 0x20c))(0x14, lossTypeOfOpponent, 0);   // stat 20 = "VictoryType"
```

Stat 0x14 is registered as `"VictoryType"` with aggregation 0 (set) in the EQGameData ctor
0x10275B80. The map is written by `EQGame::gameLostByPlayer(player, lossType)` **0x1026F8B0**
(EQGame vtable slot +0xAC; `*map[playerId] = param_3`).

### B.2 The loss-type values and where each is set

| Value | Meaning | Set at | Evidence |
|---|---|---|---|
| **0x11175 (69973)** | **avatar killed** (player's health hit 0) | `EQActionPlayedState::checkForDeadPlayers` **0x101E9D10** | "Player %d is dead!" ... `FUN_100862f0` (new CommandObject_LoseGame) + `FUN_100863d0(0x11175)`; simultaneous-death path calls `game->vt+0xAC(id, 0x11175)` |
| 0x11176 (69974) | ran out of cards | `EQDrawCardState::nextState` **0x1025B630** | "Player %d has run out of cards and lost!" -> `FUN_100863d0(0x11176)` |
| **0x11177 (69975)** | **default: the loser never "lost", the winner WON** -- i.e. a quest/scenario win via the script's "win game" node (`CommandObject_WinGame` 0x100924D0 -> game vt+0xA8) | `gameWonByPlayer` 0x10269AC0 | `*puVar7 = 0x11177;` for losers with no map entry |
| 1 | concede | `Game::concede` **0x100F8C50** | "Player %d has conceded!" -> `vt+0xAC(player, 1)` |
| 2 | turn/no-action timeout | **0x100F3A30**, **0x100F3E90** | "No action time up" / "#Time up for player" -> RemovePlayer loss type 2, `vt+0xAC(player, 2)` |
| 4 | concede sub-game | **0x100F8D20** | "Player %d has conceded sub game %d!" -> `vt+0xAC(player, 4)` |
| any int | card/scenario script "lose game" node (ActionNode key 0x54) | **0x10028060** case 0x54 | `FUN_100863d0(value from the script)` -- scenario data can choose its own |

The end-of-game screen agrees: **0x102A8CD0** (and 0x1041CDA9) pick the banner from the same
value -- `0x11175 -> "victory"/"defeat"`, `0x11177 -> "victory_quest"/"defeat_quest"`, anything
else -> "victory_generic"; end type 0 = game, 2 = tie ("victory_game_tie"), 3 = "defeat_player",
4 = force end ("Game Ended").

### B.3 Mapping for lon_service.cpp

- **75 Inquisitor** ("Win a single player game by killing the opposing avatar"): `VictoryType == 0x11175` (69973).
- **76 Oathbound** ("Win a single player game by questing"): `VictoryType == 0x11177` (69975).
- Ignore every other value (1/2/4 = opponent conceded/timed out; 0x11176 = opponent decked out;
  script-specific values).

The client sends RequestGameplayAchievement whenever `value >= threshold` (`research/lon_achievements.md`
section 4), so with threshold 1 **both** 75 and 76 are requested on every win; the server must
decide by the value in +0x24. Only sent when the local player is the winner. Caveat: a
scenario whose script fires "lose game" on the AI with its own value would not count as either;
scenario "win" objectives (the common case) produce 0x11177.

---

## C. The 60 unmapped EverQuest loot cards

### C.1 How the service maps them

- Card data: the client's `LegendsOfNorrath/data/archetypes/index.dat + storage.dat`, copied to
  the service's data root and parsed by `LoadData` (lon_service.cpp ~line 507). A card is an
  EQ loot card when its subtype is `"EverQuest Loot Card"`; those go to `loot_by_name`.
- Redemption (`HandleRedeemItemGrant`, ~line 1495): `SELECT item_id FROM lon_loot_item WHERE
  card_archetype = N` (table PK = card_archetype, one item per card). No row -> "not available".
  The item goes to /claim as `5000000 + item_id` when a `veteran_reward_templates` row exists,
  else a parcel.
- The name match was done offline (`research/lon_loot_item_match.json`, seed
  `lon_loot_seed.sql`, 332 rows in peq now). The 60 names it missed are
  `research/lon_loot_unmapped.json`; card reward text comes from `research/lon_catalog.json`.

### C.2 Proposed matches (H = high confidence, M = medium, - = none)

54 of 60 names map (63 card archetypes, because Ping's packs exist in several sets). None of the
proposed item ids is used by an existing lon_loot_item row (checked).

| Card | Card archetype(s) | peq id | peq item | Conf | Notes |
|---|---|---|---|---|---|
| Axe of Bones | 100013995 | 47706 | Axe of Bones Ornament | H | aug type 8. Card also grants Frost Covered Gem 47738 (pack 47730 is a 2-slot bag that arrives EMPTY) |
| Balebri's Horse | 100023788 | 67889 | Balebri's Horse Saddle | H | mount, click 32880 |
| Bow of the Emerald Dragon | 100021628 | 54991 | Bow of the Emerald Dragon Ornament | H | aug type 12 |
| Bridle of the Guardian Wrulon | 100006063 | 40644 | Bridle of the Wrulon Guardian | H | word order; mount click 12893 |
| Bulwark of the Emerald Dragon | 100021626 | 54990 | Bulwark of the Emerald Dragon Ornament | H | aug type 8 (55895 '...Ornamentation' is a dead type-11 item) |
| Clawed Skull | 100013997 | 47710 | Clawed Skull Ornament | H | as Axe of Bones (pack 47734) |
| Crown of Fire | 100022636 | 60946 | Crown of Fire Nimbus | H | click 9848 |
| Crown of Lightning | 100020716 | - | (none) | - | NO MATCH. Spell 24053 'Crown of Lightning' exists; its item (id 54916, the gap in the 54912-54918 LoN run) is not in peq |
| Crushing Emerald Dragon | 100021618 | 54987 | Crushing Emerald Dragon Ornament | H | aug type 8 |
| Dagger of Skulls | 100013999 | 47708 | Dagger of Skulls Ornament | H | as Axe of Bones (pack 47732) |
| Dallyn's Horse | 100023790 | 67890 | Dallyn's Horse Saddle | H | mount |
| Death and Decay | 100021614 | 54984 | Death and Decay Nimbus | H | click 17851 |
| Discordian Blade Ornament | 100008574 | 40755 | Discordant Blade Ornament | H | Discordian->Discordant; aug 8 (40770 '...Ornamentation' = dead type-11) |
| Discordian Dagger Ornament | 100008576 | 40757 | Discordant Dagger Ornament | H | aug 8 |
| Discordian Great Blade Ornament | 100008618 | 40756 | Discordant Great Blade Ornament | H | aug 8 |
| Discordian Great Mace Ornament | 100008614 | 40754 | Discordant Great Mace Ornament | H | aug 8 |
| Discordian Spear Ornament | 100008578 | 40758 | Discordant Spear Ornament | H | aug 8 |
| Discordian Spikefist Ornament | 100008628 | 40759 | Discordant Spikefist Ornament | H | aug 8 |
| Discordian War Hammer Ornament | 100008616 | 40753 | Discordant War Hammer Ornament | H | aug 8 |
| Dragon Marked Amulet | 100023379 | - | (none) | - | NO MATCH. Set-13 LoN run 64555-64575 has empty ids 64558/64568/64570. Weak stand-in: 71139 Dragorn Crested Amulet (different set, NO_DROP) |
| Estarim Family Crest Banner | 100023810 | 67905 | Estarim Family Crest | H | lore 'A banner bearing the Estarim crest' |
| EverQuest 1-Month Subscription | 100005351 | - | (none) | - | NO MATCH - not an item. Leave unmapped or make a custom reward (design call) |
| Fist of the Emerald Dragon | 100021624 | 54989 | Fist of the Emerald Dragon Ornament | H | aug type 8 |
| Gargoyle Mercenary | 100023828 | 67904 | Gargoyle Contract | H | click 119 |
| Great Blade of Bones | 100014006 | 47707 | Great Blade of Bones Ornament | H | as Axe of Bones (pack 47731) |
| Hammer of Bones | 100014008 | 47704 | Hammer of Bones Ornament | H | as Axe of Bones (pack 47728) |
| Iksar Skeleton Illusion | 100023822 | 67900 | Strange Bone Mask | H | click 32878 = 'Illusion: Iksar Skeleton' (57340 is a later spell scroll, not the LoN item) |
| Meleen's Horse | 100023792 | 67891 | Meleen's Horse Saddle | H | mount |
| Merc Contract - Werewolf | 100024269 | 72271 | Werewolf Contract | H | click 119 |
| Nature Touched Wurm Saddle | 100023362 | 64561 | Nature-Touched Wurm Saddle | H | hyphen |
| Nimbus of Dust Storms | 100019532 | 50858 | Nimbus of Dust Storm | H | singular in peq |
| Painting: Estarim Family | 100023385 | - | (none) | - | NO MATCH (67906-67908 are the single portraits, each its own card already) |
| Painting: Fancy Dress Party | 100024219 | - | (none) | - | NO MATCH; ids 67910-67912 in the same run are empty |
| Painting: Ice Dragon | 100021636 | 54994 | Painting: Ice Dragon Avatar | H | part of the Hermit's Hideaway set 54999 |
| Painting: Santug Suit | 100024361 | - | (none) | - | NO MATCH; Frostfell paintings 67712-67717 exist, 67718 (gap) is the likely id |
| Painting: Strength of Ganak | 100022630 | 60955 | Strength of Ganak Painting | H | word order |
| Painting: Tahka | 100023814 | 67907 | Painting: Tahka Estarim | H |  |
| Painting: Terris Thule | 100022638 | 60956 | Terris-Thule Painting | H | 61064 'Painting: Adiene's Terris-Thule' is a different (player art) painting |
| Path of Fire | 100021616 | 54985 | Path of Fire Nimbus | H | click 17852 |
| Path of Ice | 100022640 | 60947 | Path of Ice Nimbus | H | click 9849 |
| Piercing Emerald Dragon | 100021622 | 54988 | Piercing Emerald Dragon Ornament | H | aug type 8 |
| Ping's Peaceful Potion Package | 100006915 | 40707 | Ping's Potion Package | H | lore names it; 5-slot bag (bagtype 7) - arrives EMPTY |
| Ping's Pellucid Potion Pack | 100006089, 100008598, 100014020 | 40673 | Ping's Potion Package | H | lore names it; empty bag |
| Ping's Petite Potion Pack | 100006087, 100008604, 100012367, 100014022 | 40674 | Ping's Potion Package | H | lore names it; empty bag |
| Ping's Pinning Potion Package | 100006917, 100010837, 100019560, 100022614 | 40708 | Ping's Potion Package | H | lore names it; empty bag |
| Ping's Punctuality Potion Package | 100006919, 100010818 | 40709 | Ping's Potion Package | H | lore names it; empty bag |
| Prismatic Dragon Statue Trophy | 100023818 | 67909 | Idol of the Prismatic | M | lore 'An idol for the worship of Kerafyrm'; same 679xx set, same pattern as Idol of Severilous/Talendor/the Fallen (all matched by name) |
| Rallosian Spear Ornamentation | 100005337 | 40626 | Rallosian Greatsword Ornamentation | M | card text: 'Enchants your 2-Handed SLASHING weapon'; 40626 is the only Rallosian piece no card maps to. Every Rallosian *Ornamentation row is itemtype 11 / augtype 0 (not an equippable augment) - same as the 5 already mapped |
| Razor's Edge | 100014036 | 47717 | Razors Edge | H | apostrophe; click 21946 'Familiar: Razors Edge' (not the discipline) |
| Rhyan's Horse | 100023794 | 67892 | Rhyan's Horse Saddle | H | mount |
| Riftseeker Glave Ornament | 100007515 | 40726 | Riftseeker Glaive Ornament | H | Glave->Glaive; aug 8 |
| Sarnak Illusion | 100023824 | 67901 | Scaley Horned Mask | H | click 32879 = 'Illusion: Sarnak' |
| Shissar Mercenary | 100023826 | 67903 | Shissar Contract | H | click 119 merc contract |
| Slashing Emerald Dragon | 100021620 | 54986 | Slashing Emerald Dragon Ornament | H | aug type 8 |
| Spear of Bones | 100014014 | 47709 | Spear of Bones Ornament | H | as Axe of Bones (pack 47733) |
| Staff of Bones | 100014016 | 47705 | Staff of Bones Ornament | H | as Axe of Bones (pack 47729) |
| Tahka's Horse | 100023796 | 67893 | Tahka's Horse Saddle | H | mount |
| Venemous Raptor Familiar | 100022624 | 60939 | Venomous Raptor Familiar | H | spelling |
| Venemous Raptor Saddle | 100022626 | 60944 | Venomous Raptor Saddle | H | spelling |
| Visage of King Gelaqua | 100005311 | 40613 | Visage of the King Gelaqua | H | 'the' |

**No plausible peq item (6):** Crown of Lightning, Dragon Marked Amulet, Painting: Estarim
Family, Painting: Fancy Dress Party, Painting: Santug Suit, EverQuest 1-Month Subscription.
The five real items are absent from peq (gaps in the LoN id runs); they would need new item rows
(design call) or stay unredeemable.

Proposed seed (63 rows; needs a schema migration + `veteran_reward_templates` rows
`5000000 + item_id` for /claim, as was done for the first batch):

```sql
INSERT INTO lon_loot_item (card_archetype, item_id) VALUES
(100013995,47706),(100023788,67889),(100021628,54991),(100006063,40644),(100021626,54990),
(100013997,47710),(100022636,60946),(100021618,54987),(100013999,47708),(100023790,67890),
(100021614,54984),(100008574,40755),(100008576,40757),(100008618,40756),(100008614,40754),
(100008578,40758),(100008628,40759),(100008616,40753),(100023810,67905),(100021624,54989),
(100023828,67904),(100014006,47707),(100014008,47704),(100023822,67900),(100023792,67891),
(100024269,72271),(100023362,64561),(100019532,50858),(100021636,54994),(100022630,60955),
(100023814,67907),(100022638,60956),(100021616,54985),(100022640,60947),(100021622,54988),
(100006915,40707),(100006089,40673),(100008598,40673),(100014020,40673),(100006087,40674),
(100008604,40674),(100012367,40674),(100014022,40674),(100006917,40708),(100010837,40708),
(100019560,40708),(100022614,40708),(100006919,40709),(100010818,40709),(100023818,67909),
(100005337,40626),(100014036,47717),(100023794,67892),(100007515,40726),(100023824,67901),
(100023826,67903),(100021620,54986),(100014014,47709),(100014016,47705),(100023796,67893),
(100022624,60939),(100022626,60944),(100005311,40613);
```

### C.3 Side findings in the 332 rows ALREADY mapped (not part of the 60)

1. **Wrong item: card 100023381 "Idol of the Fallen" -> 32322**, which is a *shield*
   (itemtype 8) of the same name. The LoN trophy is **64571** "Idol of the Fallen" (itemtype 11,
   lore "An idol to honor Vishimtar", in the set-13 LoN run). Exact-name match picked the
   lower id.
2. **20 rows map to dead "Ornamentation" placeholders** (itemtype 11, augtype 0, icon IT63, no
   proc): Rallosian x5 (40623-40627, 40639), Draconic x7 (40661-40667), Tunarian x7
   (40692-40698), Magi Shield 43994. Compare the Discordant pair: 40770 "Ornamentation" (dead)
   vs 40755 "Ornament" (itemtype 54, augtype 8, proc 20150). Only **Magi Shield has a real
   augment: 44992 "Magi Shield Ornament"** (itemtype 54) -- remap 100012447 to it. Rallosian /
   Draconic / Tunarian have no augment version in peq; the players get an unusable item.
3. **Empty bags**: every Ping's potion pack/package (40633-40637, 40672, 40675, 40710 and the
   five above) and the Bones "Ornament Pack" items are bags (5/10 or 2 slots). A /claim or
   parcel delivers the bag only, no potions/ornament/gem. If the contents matter, the
   redemption needs a per-card item LIST (lon_loot_item is PK on card_archetype today) or the
   pack contents granted instead of the bag. Bones cards: live gave the ornament AND the Frost
   Covered Gem 47738 (card text) -- one item per card cannot express that.

---

## D. Two EQ clients on one PC (LoN gateway port)

### D.1 Where LoN gets its gateway host and port

LoN `Initialize` **0x101E2BE0** parses its command line (`--realm=`, `--host=`, `--lang=`,
`--post-login=`; strings 0x1062B924 / 0x1062B8C0 / 0x1062B9D4 / 0x1062B8C8). For realm
"production" (or no realm arg; value 5) it runs:

```
101e38a2  68 d8 b8 62 10        PUSH 0x1062b8d8            ; "lvsccg-02-01.station.sony.com"
101e38a7  b9 a8 c3 78 10        MOV  ECX, 0x1078c3a8       ; NetworkService::sGatewayHost
101e38ac  ff 15 90 f0 5e 10     CALL std::string::assign
101e38b2  68 d8 b8 62 10        PUSH 0x1062b8d8            ; same literal
101e38b7  b9 8c c3 78 10        MOV  ECX, 0x1078c38c       ; NetworkService::sDefaultHost
101e38bc  c7 05 68 c3 78 10 a6 3d 00 00   MOV [0x1078c368], 0x3da6   ; sGatewayPort = 15782
101e38c6  ff 15 90 f0 5e 10     CALL std::string::assign
101e38cc  c7 05 64 c3 78 10 a7 3d 00 00   MOV [0x1078c364], 0x3da7   ; sDefaultPort = 15783
```

- **Ports are 32-bit immediates in code**: sGatewayPort imm at **VA 0x101E38C2 (RVA 0x1E38C2)**,
  sDefaultPort imm at **VA 0x101E38D2 (RVA 0x1E38D2)**. Not relocated (immediates), so they can
  be patched at LoadLibrary time exactly like the host literal (RVA 0x62B8D8) is today. Our
  realm is "production" (we pass no `--realm=`), so this block runs. Other realms
  (development/plattest/stage) skip it.
- Names come from the client's own asserts in `NetworkService` ctor **0x101739A0**:
  `NetworkService::sDefaultHost/sGatewayHost/sDefaultPort/sGatewayPort`.
- **Two hops.** Gateway connection (NetworkService+0x4D set) connects to sGatewayHost:sGatewayPort
  (**0x101751B0**: `this->host = sGatewayHost; this->port(+0x3C) = sGatewayPort`). The gateway's
  `GatewayCommand_GetConnectionServer` reply execute **0x101176E0** then OVERWRITES
  sDefaultHost/sDefaultPort with the reply's host (+0x3C) and port (+0x58):
  ```
  10117734  ...  MOV ECX,0x1078c38c ; CALL string::operator=   ; sDefaultHost = reply host
  10117741  8b 47 58             MOV EAX,[EDI+0x58]
  10117744  a3 64 c3 78 10       MOV [0x1078c364],EAX          ; sDefaultPort = reply port
  ```
  and reconnects (0x10175490) to sDefaultHost:sDefaultPort. Our world answers that with
  `"127.0.0.1", 15782` (lon_service.cpp line ~1773), so today both hops hit the same relay.
- **Config file (shared, not per-process):** at every connect, **0x10173B40** reads
  **`host.svr`** from the current directory (`fopen("host.svr","r")`), lines `host X`, `port N`,
  `ghost X` (sets sGatewayHost), `gport N` (sets sGatewayPort). Both EQ processes share the
  client folder, so this cannot give each process its own port. Trap: a stray `host.svr` in the
  RoF2 folder overrides our patches, because it is read after Initialize.
- No `--port=` style argument exists (the argument strings near 0x1062B8xx are only realm, host
  game, lang, post-login, sessionID, username, challenge).

### D.2 Recommended fix (client side, dinput8 only; no server change)

Per process, in `nms_lon_link.cpp`:

1. In `Install` (or the first `LoadLibraryExA_Wrap` hit), create the listening socket bound to
   `127.0.0.1:0` (ephemeral), `listen()`, read the port back with `getsockname`. Keep
   `SO_EXCLUSIVEADDRUSE`. Start `ListenThread` on that socket right away; `PumpThread` already
   reads `g_host/g_port` lazily at accept time, so the relay can exist before OP_NMSLoNSession
   (if the session has not arrived yet, ConnectUpstream fails and the card game shows its normal
   connection error -- same as today).
2. In `PatchGatewayHost` (LoadLibrary of LegendsOfNorrath.dll, before Initialize runs), next to
   the host-literal patch, write the port as a DWORD into **both** immediates:
   `*(DWORD*)(lon + 0x1E38C2) = port;  *(DWORD*)(lon + 0x1E38D2) = port;`
   Guard: check the 10 bytes at RVA 0x1E38BC are `C7 05 ?? ?? ?? ?? A6 3D 00 00` and at
   0x1E38CC `C7 05 ?? ?? ?? ?? A7 3D 00 00` (skip the 4 relocated address bytes), same
   "unknown build -> leave alone" rule as the host check. Code page -> VirtualProtect RWX.
3. **NOP the reply-port store** at **RVA 0x117744** (5 bytes `A3 xx xx xx xx` -> `90 x5`; guard
   on `8B 47 58 A3` at RVA 0x117741) so sDefaultPort keeps our patched port no matter what the
   server sends. The host store just before it stays (server keeps sending "127.0.0.1").
   Do this after LoadLibrary returns (relocations already applied).

Alternative to step 3 (no extra code patch): leave 0x117744 alone and have the world echo the
right port -- but world cannot know the client's ephemeral port unless the relay tells it (e.g.
a tiny custom first frame on each upstream connection carrying the local port, consumed by
lon_service before normal parsing). More moving parts; step 3 is simpler.

Result: each eqgame.exe has its own relay port, LoN in that process connects only to it, and
the second client no longer fails with "port 15782 is taken". The server's `U(15782)` in
GetConnectionServer becomes irrelevant (can stay).

---

## Open questions

1. A: the exact header convention for ArchCommand replies (2 levels assumed, like
   RedeemItemGrant) and whether our `Writer::S()` is binary-safe for the AddArchetypesToDB blob
   (embedded NULs). Verify both with the client trace on the first test.
2. A: avatar cap (0x9C) and name rules are design calls; also whether the PropertySet from a
   custom avatar should be range-checked server side.
3. A: whether 0x1C3 must precede the collection delivery or can follow it -- assumed before.
4. B: the 4th caller of the loss-type setter, `FUN_10231F10` (call at 0x10237161), was not
   read; likely another "player dead" path (expected 0x11175). Scenario scripts can pass any
   value to "lose game" (key 0x54); a handful of scenarios might report something other than
   0x11177 for a quest win -- the server log line at lon_service.cpp ~1232 will show it.
5. C: 6 cards have no item; 20 existing rows point at dead Ornamentation placeholders; bags
   arrive empty; Idol of the Fallen maps to a shield. All need design decisions.
6. D: confirm LoN `Initialize` is not re-run with a `--realm=` from eqgame (it is not passed
   today), and that no `host.svr` ships in the patcher MASTER.

## C2. The 6 "no item" loot cards, researched against the LIVE client + ZAM (10-06 evening)

Tool: a PFS reader that lists every .mod and the textures it uses.

| Card | Live item id | Model in OUR RoF2 client | Notes |
|---|---|---|---|
| Crown of Lightning | 54916 (gap in 54912-54918 run) | n/a (IT63, click item) | ZAM: No Trade/Heirloom, AMMO slot, any-slot click 2.0 s, unlimited, wt 0.5, SMALL, lore "Crown your head in lightning", icon 3363. Spell 24053 (nimbus 608) is in peq AND in our shipped spells_us.txt (our client and patcher master; stock RoF2 lacks it). Clone 54915 with click 24053. |
| Painting: Estarim Family | 64570 (eqresource) | IT23346 in furniture11.eqg (texture estarim_family.dds) - present in RoF2 | ZAM: No Trade/Heirloom, wt 5.0, GIANT, lore "Painting of the Estarim Family". Clone 67906 row shape. |
| Painting: Santug Suit | unknown on live (67718 gap guess) | IT60000 in lon15.eqg (only model using Santugs_suit_c.dds) - present in RoF2 | Clone 67717 row shape. |
| Painting: Fancy Dress Party | none - only EQII had this item | NO model in RoF2 or live | Needs a borrowed painting model or a new model (card art). |
| Dragon Marked Amulet | 64558 or 64568 (gaps) | n/a (neck jewelry) | Live: level 85 req, AC 77, ~30-37 stats, 1275 HP/1290 mana/end. Out of era - stats are a design call. |
| EverQuest 1-Month Subscription | not an item | n/a | Needs a custom reward (design call). |

Also found: 54912-54918 click spells 24049-24055 are missing from STOCK RoF2 spells_us.txt but present in ours - fine for players on the patcher.
