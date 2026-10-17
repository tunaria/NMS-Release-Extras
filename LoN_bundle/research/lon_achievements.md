# Legends of Norrath - achievements / Loremaster (client RE)

Source: LegendsOfNorrath.dll (our client copy). Addresses are Ghidra VAs at image base 0x10000000.
Read-only research, 2026-10-06. Anything not proven from code is marked **UNPROVEN**.

## 1. Achievement (class 0x1E0 v1): every field

The object is 0x80 bytes. The constructor at 10018910 sets +4 = -1, +8 = -1, +0x70 = -1 and everything else to 0.
Each field has a getter (10018620..100187f0, `mov eax,[ecx+off]`) and a setter (10018610..100187e0).
vt+0x38 = Clone (10018a90). vt+0x3C = CopyTo(dst) (10018c80), which copies every field, including +0x7C.

| Off | Wire | Meaning | Evidence |
|---|---|---|---|
| +0x04 | int | **Achievement id** (0..92 in this client). It is the key of the account map and the key of the client's title/description tables. | map key in 100091b0 (10018620 -> 10005450 on acct+0x70); title lookup 1039adb0(id) in history row 1039b470, journal item 10421130 and popup 1039af90 |
| +0x08 | int | **Type**. The client only tests for one value: **5 = gameplay achievement**, tracked during a game (see section 4). It is also sent as arg 2 of UI event 0xBE / achievementUnlocked(int,int,int), but the handler ignores it. Other values are UNPROVEN. | 10275740 (`+8 == 5`), 1000fa4d |
| +0x0C | string | **Icon base name**, e.g. `LM_icon_gameplay_`. The image is `:/images/achievements/<+0xC><complete or incomplete>.png`. It is also the first Journal sort key, so it acts as the category. | 1042131f..1042137c (`%1%2` = +0xC, then "complete"/"incomplete"); comparator 104c60e0 |
| +0x28 | int | **Base XP** shown in the Journal | 10421838, 10421919 |
| +0x2C | int | **Bonus XP**. When non-zero the Journal uses the "bonus_" entry art and the text `<+0x28 + +0x2C>XP (<round(+0x2C*100/+0x28)>% Bonus)` | 10421216 (flag item+0x5A), 10421823..1042194x, constant at 1062d780 = 100.0 |
| +0x30 | int | **Required (max) completion count**. If 0, the Journal shows only `<count>`; otherwise it shows `<count>/<+0x30>`. The date label reads "Completed on" when count == +0x30 and "Last Completed on" otherwise. | 10421539..104215c5, 10421b51..10421b5a |
| +0x34 | int | **Visible in the Journal while incomplete**. The Journal lists an entry only if +0x34 != 0 OR its completion count (+0x7C) != 0. | 104ca8b0, 104ca8bb |
| +0x38 | int | **Hidden (secret)**. While not completed it shows the icon `LM_icon_hidden_`, the title "Hidden" and the description "???". | 1042131f, 104213f9, 104214a2 |
| +0x3C | vector<int> | **Reward archetype ids**. The Journal uses element [0] for the reward icon (LM_reward.png) and the hover card preview ("Moz"): WAUserInfoDialog::handleMozRequested 104c81e0 -> archetype cache 10044eb0, which takes an archetype id (3xxxxx/4xxxxx foil ids included). | 10421d8e..10421dd8; 10420e60 emits requestMoz(item+0x54, x, y, false) |
| +0x4C | int | **UNPROVEN**. Its getter 10018730 has no callers and nothing in the client reads it (maybe reward quantity or type). | call scan |
| +0x50 | int | **Has reward** flag. The reward icon and hover show only when +0x50 != 0 and +0x3C is not empty. | 10421d8e, 104ca942 |
| +0x54 | string | **Gameplay stat name** for type 5. It is one of the EQGameData stat names, e.g. `QuestsCompleted`. Second Journal sort key. | 10275740/102757fd, 104c6192 |
| +0x70 | int | **Gameplay threshold** for type 5: a stat value >= +0x70 fires RequestGameplayAchievement. -1 = none. Third Journal sort key. | 10275a34..10275a44, 104c6201 |
| +0x74 | int | **XP awarded**: History column "XP", popup "%1XP", arg 3 of UI event 0xBE. | 1039b4e3, 1000fa5d, 10395f60 |
| +0x78 | int | **Completion time as a unix time_t; 0 = not completed**. Used for the History "Completed" column, the Journal date and every "is it completed" test. | 1039b5c5 (QDateTime::setTime_t), 10421c05, 100091c5 |
| +0x7C | (not on the wire) | **Completion count, kept by the client** and maintained by the account merge (section 2). The Journal draws "complete" art when +0x7C != 0. | 10421207, 100091b0 |

Journal sort order (104c60e0, applied by 104ca3f0): completed entries (+0x78 != 0) first, then +0xC ascending, then +0x54 ascending, then +0x70 ascending.

Icons in resources.rcc under /images/achievements/:
- `LM_icon_{attend,avatar,collection,deck,game,gameplay,hidden,pack,scenario,social,trade,tutorial}_{complete,incomplete}.png`
- `LM_jounal_entry_[bonus_]{complete,incomplete}.png`
- `LM_reward[_over].png`, `achievement_box.png`, `level_up_box.png`, `LM_next_level.png`, `xp_bar_{base,fill,top}.png`

So the valid +0xC values are `LM_icon_attend_` ... `LM_icon_tutorial_`, each with a trailing underscore.

## 2. Where the list lives and how it is updated

The account object comes from AccountService (FUN_100184e0) -> getAccount (FUN_10017df0(id)). Relevant members:
- +0x04: account id. +0x34: mPropertySet (asserts in Account.cpp at 10008bdb).
- +0x60: `vector<Achievement*>`, the **history**. Every received entry with +0x78 > 0 is cloned into it (push_back = FUN_10008660). Accessor 10001380. It feeds the History tree (104ca4c0 -> 1039b470).
- +0x70: `map<int id, Achievement*>`, the **journal**, with one entry per id. Accessor 10001390. It feeds the Journal and the gameplay tracker.

Each received entry is merged by Account::AddAchievement (100091b0); 10008ff0 runs the same loop:
1. If new.+0x78 > 0, a clone is pushed to the history (+0x60).
2. If the id is not in the map: map[id] = clone, and if new.+0x78 > 0 then map[id].+0x7C = 1.
3. If the id is in the map: cnt = old.+0x7C. If new.+0x78 > old.+0x78, new is copied over old. Then old.+0x7C = cnt + 1. **This increment happens even when new.+0x78 == 0.**

So for each id the server should send either ONE incomplete entry (+0x78 = 0), or N completed entries (one per completion, each with its own time and XP).

- **SendAchievementList 0x1E2**: wire = AccountCommand header + AccountID, then a `vector<object*>` of Achievement (reader 10013330 -> 10013150). Execute 10012A20 -> 10008ff0, which FIRST CLEARS both containers (10006fb0 deletes the history and the map) and then merges each entry. It posts UI event 0xBF(accountId) to the WAAchievementManager slot (10396800 -> 104d4ca0); nothing visible happens.
- **NotifyAchievement 0x1E1**: one nullable Achievement at +8. Execute 1000F9B0 -> AddAchievement (100091b0), then UI event 0xBE with [accountId, id (+4), type (+8), xp (+0x74)].
  - WAAchievementManager (ctor 1039ace0, global 1078de48, eventFilter 10396a00) passes it to 10396760, which acts only for MY account, -> achievementUnlocked -> 1039af90.
  - Result: a toast (`achievement_box.png`) with "<title>" and "%1XP" (+0x74), sound 0x17B, shown for 5 s.
- Neither message refreshes an open profile. The Journal and History redraw only on UI event 0x10 (IntroduceAccount) or when the profile is opened (getInfo 104c6050).

**The client does NOT compute the Loremaster level.** It only displays these account properties:

| Key | Type | Use | Readers |
|---|---|---|---|
| 0x1082 | int (2) | Loremaster level ("levelText", "otherLevelText"); also used to detect a level-up | 10008c36/51/77/91 (MergeProperties), 104c6fec, 104c7372 |
| 0x1083 | **string (3)** | Current bonus-loot % -> "(%1% Chance of Bonus Loot)" ("modText"). A non-string value triggers a wa_assert. | 104c7433 |
| 0x1084 | **string (3)** | NEXT level's bonus-loot % -> "Next: %1%" ("nextText"). A non-string value triggers a wa_assert. | 104c757d |
| 0x1085 | int | cur = total XP | 104c76ef |
| 0x1086 | int | floor = XP at the start of the current level | 104c7709 |
| 0x1087 | int | next = XP needed for the next level (0 or missing -> treated as cur) | 104c771b |

XP math (104c7725..104c78xx):
- gained = cur - floor, and span = next - floor.
- `xpBar = gained/span` (float, FUN_104d2230).
- "xpText" = `"%1XP / %2XP"`, with %1 = gained and %2 = span.
- "xpRemainingText" = `"%1XP Remaining"`, with span - gained (= next - cur).
- If next == floor the bar divides by zero, so avoid that.
- The XP header is drawn only for your own account; other accounts show only 0x1082.

Other 0x108x immediates, none of which are account keys:
- 0x1084 at 100cf5ec is an assert line number (Game.cpp).
- 0x1088..0x108B at 1018227e..101823fe are PlayElement (in-game card) property getters with no callers.
- 0x1081 at 102c399d is a game-setup property case.

MergeProperties also checks key 0x1013 after the level check (10008d25); that part was not analysed.

**Level-up popup.** Account::MergeProperties (10008bc0) reads the old 0x1082, merges, then reads the new 0x1082. If the old value was an int AND new > old, it posts UI event 0xE3 [accountId, newLevel]. WAAchievementManager 10396430 (my account only) then shows `level_up_box.png` with "You are now level %1", sound 0x184, for 5 s. There is no popup when the old set had no 0x1082.

## 3. Pushing new XP/level live

`AccountCommand_IntroduceAccount` 0x72 (execute 1000EEA0) can be re-sent at any time for an existing account. It:
1. finds or creates the account (10018430) and sets name and gender;
2. calls `MergeProperties(props)` (10008bc0 -> 10192bd0), which does a per-key SetProperty (10192900); keys that are not sent are kept;
3. posts UI event 0x10 [accountId, name, PropertySet].

WAUserInfoDialog::eventFilter (104c67b0) handles 0x10 for my account: it copies the set (10192ae0) into the dialog and re-runs the header (vt+0x104 = 104c6dd0) and the Journal/History (vt+0x114 = 104ca4c0). So a partial PropertySet with 0x1082..0x1087 updates the profile live and fires the level-up toast.

Other callers of the merge: BulkIntroduceAccount 0x115 (1000bf1d), LobbyCommand_Join 0x5C (1014540f), LobbyCommand_BulkJoin 0x6A (1013d877).

`CommandObject_SetProperty` 0x159 is an in-game command object, not an account path; no account use was found.

Recommended server order when awarding: NotifyAchievement first (toast + map update), then IntroduceAccount with the changed 0x1082..0x1087 (profile refresh + level-up toast).

## 4. RequestGameplayAchievement 0x1E3: the only sender

There is one call site: 10275a75 in EQGameData::checkAchievements (10275830). EQGameData's stat recorder (10276010) calls it after every stat update. On the client, execute 10011390 just returns true.

How the pending list is built:
- The EQGameData constructor (10275b80) registers stat id -> name, with an aggregation type (1 = sum, 2 = max, 0 = set). It then calls 10275740, which builds `map<string, vector<Achievement*>>` (+0x28) from the account map.
- Only entries with **+8 == 5, +0x78 == 0, +0x54 != "" and +0x70 != -1** are taken, bucketed by +0x54. 10261940 also rebuilds this list.
- So SendAchievementList must arrive before the game starts.

When stat S is updated:
- name = names[S]. If that bucket has any achievement with `value(S) >= +0x70`, the client sends RGA with **+8 string = stat name** and **+0x24 int = current stat value** (AccountID = my account).
- The bucket is never pruned, so the client can send the same request again on later updates of that stat. The server must dedupe (no pruning code was found).

Stat names (id: name):
- 4 QuestsCompleted, 5 DamageTotal, 6 DamageDealt, 7 DamageReceived, 8 DamageReduced
- 9 DamageBonusMax (max), 10 HealingTotal, 11 HealingAvatar, 12 HealingUnits
- 13 DestroyedUnits, 14 DestroyedItems, 15 PlayedUnits, 16 PlayedItems, 17 PlayedAbilities, 18 PlayedTactics
- 19 MaxCombatDifferential (max), 20 VictoryType (set)

Derived stats in 10276010 (card property 0x1C = card type):
- Stat 2 (played): Unit 0x111B0 -> PlayedUnits, Item 0x111AD -> PlayedItems, Ability 0x111AE -> PlayedAbilities, Tactic 0x111B1 -> PlayedTactics.
- Stat 3 (destroyed): Unit -> DestroyedUnits, Item -> DestroyedItems.
- Stat 10 (healing): Unit -> HealingUnits, Avatar 0x111B2 -> HealingAvatar.

UNPROVEN: which game modes create EQGameData (single player only?), and the VictoryType values for "kill avatar" vs "questing".

Likely type-5 rows (stat and threshold read from the descriptions, not from code):

| id | stat | threshold |
|---|---|---|
| 20 | QuestsCompleted | 10 |
| 21 | PlayedUnits | 10 |
| 22 | DamageBonusMax | 10 |
| 23 | DamageDealt | 20 |
| 24 | HealingAvatar | 10 |
| 25 | HealingUnits | 10 |
| 26 | HealingTotal | 20 |
| 27 | DestroyedUnits | 10 |
| 28 | MaxCombatDifferential | 10 |
| 29 | PlayedItems | 5 |
| 30 | PlayedAbilities | 5 |
| 31 | PlayedTactics | 5 |
| 32 | DestroyedItems | 5 |
| 33 | DamageReduced | 5 |
| 75 | VictoryType | UNPROVEN |
| 76 | VictoryType | UNPROVEN |
| 85 | QuestsCompleted | 5 |
| 86 | PlayedUnits | 5 |
| 87 | DamageBonusMax | 5 |
| 88 | DestroyedUnits | 5 |
| 89 | MaxCombatDifferential | 5 |
| 90 | DamageReduced | 10 |
| 91 | PlayedTactics | 10 |
| 92 | PlayedAbilities | 10 |

## 5. Client title/description table (FUN_10396ef0, WAAchievementManager)

Titles are a QMap<int,QString> at manager+8 (lookup 1039adb0); descriptions are at +0xC (lookup 1039aea0).

The key is the achievement id, which equals the N in `PUID:ui.waachievementmanager.cpp.titleN` / `descriptionN`. Verified at 10397bed (key 0x2B = title43), 10398c93 (key 0 = description0) and 1039ab91 (key 0x5C = description92).

An unknown id is displayed as the number. Titles and descriptions exist only on the client; the wire Achievement carries no text.

| id | Title | Description |
|---|---|---|
| 0 | Secret Surprise! | Receive a bonus loot card as a Loremaster! |
| 1 | Ooh, Shiny! | Find a foil card in a booster pack. |
| 2 | Precious Find | Find an ultra rare card in a booster pack. |
| 3 | Open a Booster Pack | Open a Legends of Norrath booster pack. |
| 4 | Open a Starter Deck | Open a Legends of Norrath starter deck. |
| 5 | Open a Promo Pack | Open a Legends of Norrath promo pack. |
| 6 | Win a Scenario on Hard | Win a scenario game on hard difficulty. |
| 7 | Win a Scenario on Medium | Win a scenario game on medium difficulty. |
| 8 | Win a Scenario on Easy | Win a scenario game on easy difficulty. |
| 9 | Win a Scenario with Fighter | Win a scenario game with a Fighter deck. |
| 10 | Win a Scenario with Generic | Win a scenario game with a Generic deck. |
| 11 | Win a Scenario with Mage | Win a scenario game with a Mage deck. |
| 12 | Win a Scenario with Priest | Win a scenario game with a Priest deck. |
| 13 | Win a Scenario with Scout | Win a scenario game with a Scout deck. |
| 14 | Complete Oathbound Block Scenarios | Complete the Oathbound, Forsworn, Inquisitor and Oathbreaker Campaigns. |
| 15 | Complete Ethernauts Block Scenarios | Complete the Ethernauts, Against the Void and Storm Break Campaigns. |
| 16 | Complete Travelers Block Scenarios | Complete the Travelers, Vengeful Gods and Doom of the Ancient Ones Campaigns. |
| 17 | Complete Dragonbrood Block Scenarios | Complete the Dragonbrood - The Anarchs, Priestess of the Anarchs and Fall of the Estarim Campaigns. |
| 18 | Complete Legacies Campaign | Complete the Legacies Campaign. |
| 19 | Complete Debt of the Ratonga Campaign | Complete the Debt of the Ratonga Campaign. |
| 20 | Ethernaut | Complete 10 quests in a single player game. |
| 21 | Watch Commander | Play 10 units in a single player game. |
| 22 | Ashen Order | Have a damage bonus of 10 or more in a single combat round. |
| 23 | Chosen of Innoruuk | Deal 20 damage in a single player game. |
| 24 | Guardian of Growth | Heal 10 avatar health in a single player game. |
| 25 | Prime Healer | Heal 10 unit health in a single player game. |
| 26 | Chosen of Tunare | Heal 20 total health in a single player game. |
| 27 | Council of Justice | Destroy 10 enemy units in a single player game. |
| 28 | Champion of Rallos Zek | Win a combat round by 10 or more in a single player game. |
| 29 | Storm Guard Smith | Play 5 items in a single player game. |
| 30 | Adventurer | Play 5 Abilities in a single player game. |
| 31 | Trickster | Play 5 Tactics in a single player game. |
| 32 | Sunderer | Destroy 5 enemy items in a single player game. |
| 33 | Bladestopper | Reduce 5 damage with runes in a single player game. |
| 34 | Win a 4-player Raid with Fighter | Win a 4-player Raid game with a Fighter deck. |
| 35 | Win a 4-player Raid with Generic | Win a 4-player Raid game with a Generic deck. |
| 36 | Win a 4-player Raid with Mage | Win a 4-player Raid game with a Mage deck. |
| 37 | Win a 4-player Raid with Priest | Win a 4-player Raid game with a Priest deck. |
| 38 | Win a 4-player Raid with Scout | Win a 4-player Raid game with a Scout deck. |
| 39 | Win a 4-player Raid | Win a 4-player Raid game. |
| 40 | Win a 2-player Raid with Fighter | Win a 2-player Raid game with a Fighter deck. |
| 41 | Win a 2-player Raid with Generic | Win a 2-player Raid game with a Generic deck. |
| 42 | Win a 2-player Raid with Mage | Win a 2-player Raid game with a Mage deck. |
| 43 | Win a 2-player Raid with Priest | Win a 2-player Raid game with a Priest deck. |
| 44 | Win a 2-player Raid with Scout | Win a 2-player Raid game with a Scout deck. |
| 45 | Win a 2-player Raid | Win a 2-player Raid game. |
| 46 | Complete Goblin Dojo Raid | Complete Goblin Dojo Raid. |
| 47 | Complete Trakanon Raid | Complete Trakanon Raid. |
| 48 | Complete Chardok Raid | Complete Chardok Raid. |
| 49 | Complete Cheldrak Raid | Complete Cheldrak Raid. |
| 50 | Complete Ralkor Raid | Complete Ralkor Raid. |
| 51 | Complete Imzok Raid | Complete Imzok Raid. |
| 52 | Complete Bloody Kithicor Raid | Complete Bloody Kithicor Raid. |
| 53 | Complete Void Palace Raid | Complete Void Palace Raid. |
| 54 | Complete Khati Sha Raid | Complete Khati Sha Raid. |
| 55 | Master of Lessons | Complete all 11 Legends of Norrath tutorials. |
| 56 | Treasure Hunter | Find a loot card in a booster pack. |
| 57 | Drakkinshard Release Day | Log into LON on release day for Set 16, Drakkinshard! |
| 58 | Attend the Gathering | Attend the LON Panel at SOE Live 2013! |
| 59 | Build a Fighter Deck | Build a Fighter deck through the deck builder. |
| 60 | Build a Generic Deck | Build a Generic deck through the deck builder. |
| 61 | Build a Mage Deck | Build a Mage deck through the deck builder. |
| 62 | Build a Priest Deck | Build a Priest deck through the deck builder. |
| 63 | Build a Scout Deck | Build a Scout deck through the deck builder. |
| 64 | Builder of Decks | Complete all five deck building tasks. |
| 65 | Create a Fighter Avatar | Build a Fighter avatar through the deck builder. |
| 66 | Create a Mage Avatar | Build a Mage avatar through the deck builder. |
| 67 | Create a Priest Avatar | Build a Priest avatar through the deck builder. |
| 68 | Create a Scout Avatar | Build a Scout avatar through the deck builder. |
| 69 | Creator of Avatars | Complete all four avatar creation tasks. |
| 70 | Hello Friend! | Add a friend in Legends of Norrath. |
| 71 | Forsworn | Create or Join a Guild in Legends of Norrath. |
| 72 | Friendly Combat | Win a casual game by questing, avatar defeat or decking. |
| 73 | Honorable Defeat | Lose a casual game by questing, avatar defeat or decking. |
| 74 | Win a Tournament Game | Win a tournament game by questing, avatar defeat or decking. |
| 75 | Inquisitor | Win a single player game by killing the opposing avatar. |
| 76 | Oathbound | Win a single player game by questing. |
| 77 | Highest Competition | Play in the 2013 LON Championship event at SOE Live 2013! |
| 78 | Best in the World | Take 1st place in the LON Championship event at SOE Live 2013! |
| 79 | Second Best in the World | Take 2nd place in the LON Championship event at SOE Live 2013! |
| 80 | Third Best in the World | Take 3rd place in the LON Championship event at SOE Live 2013! |
| 81 | Didn't Embarrass Myself | Place 4th - 8th place in the LON Championship event at SOE Live 2013! |
| 82 | Complete a Posted Trade as Buyer | Complete a posted trade as a buyer. |
| 83 | Complete a Posted Trade as Seller | Complete a posted trade as a seller. |
| 84 | Post a Trade | Post a trade. |
| 85 | Oathbreaker | Complete 5 quests in a single player game. |
| 86 | Taskmaster | Play 5 units in a single player game. |
| 87 | Ashen Disciple | Have a damage bonus of 5 or more in a single combat round. |
| 88 | Tribunal's Servant | Destroy 5 enemy units in a single player game. |
| 89 | Initiate of Rallos Zek | Win a combat round by 5 or more in a single player game. |
| 90 | Soldier of Tunare | Reduce 10 damage with runes in a single player game. |
| 91 | Swashbuckler | Play 10 Tactics in a single player game. |
| 92 | Seeker of Destiny | Play 10 Abilities in a single player game. |

Groups implied by the ids:

| ids | group |
|---|---|
| 0 | bonus loot (Loremaster) |
| 1-2, 56 | pack finds |
| 3-5 | opening products |
| 6-13 | scenario difficulty and class |
| 14-19 | campaigns |
| 20-33, 85-92 | single-player gameplay stats (type 5) |
| 34-54 | raids |
| 55 | tutorials |
| 57-58, 77-81 | live events |
| 59-64 | deck builder |
| 65-69 | avatar creation |
| 70-71 | social |
| 72-74 | casual and tournament |
| 75-76 | single-player victory type |
| 82-84 | trade |

Other achievement strings in the client:
- Profile tabs: "Journal" and "History" (104c6540).
- History headers: XP, Title, Completed (104ca4c0).
- Journal texts: "Hidden", "???", "Bonus", " (%1% ", "Completed on", "Last Completed on", "%1/%2", "XP".

There are no category-name strings; the icon names are the categories.

## 6. ValueData / PropertySet wire format

**PropertySet** (0x17 v1, writer 10191E30 -> 10191700): `[0x17][1][count]`, then for each entry a `key varint` followed directly by the ValueData. There is no null flag (10176f70 calls vt+0x28).

**ValueData** (0x16 v4, writer 101D5DD0, reader 101D7ED0): `[0x16][4][TypeID][mOwningReference][value]`.

TypeID values:
- 1 bool (varint), 2 int (varint), 3 string
- 5, 9, 10 nullable object
- 6 vector<int>, 7 vector<string>, 8 vector<object*>
- 13 map<int,int>, 14 map<int,object*>

Card data uses owning = 1. Examples:
- int: `16 04 02 01 <varint>`
- string: `16 04 03 01 <len><bytes>00`

## 7. Bonus loot / Loremaster cards

**Collection filter** (eqfilterpanel builder 1025da60):
- The rarity menu filters on key 0x4C: Common 70035, Uncommon 70036, Rare 70037, Ultra Rare 70049, Promo 70038, Fixed 70039, Raid 70046, EQ 70042, EQII 70043, EQP 70044, EQIIP 70045.
- Foils / No Foils use the pseudo-values 0x3E9 / 0x3E8.
- **Loremaster uses the pseudo-value 0x803.** The query builder (1040a764, 1040a9f1..1040aa41) turns it into **card property 0x803 (2051) == int 1**. It is not a rarity value.

**Archetype data** (parsed from the client's archetype files): exactly **10 cards have 2051 = 1**, and all are loot cards. Rarities: 70042 x5, 70043 x3, 70045 x2.

| Archetype | Name | Rarity |
|---|---|---|
| 100005251 | Stein of Recollection | EQII |
| 100005286 | Bridle of the Shadow Panther | EQ |
| 100005305 | Kiss of Erollisi Marr | EQ |
| 100005341 | Familiar of Lord Nagafen | EQ |
| 100007505 | Visage of the Siren Enticer | EQ |
| 100010909 | Vhalen's Cover Band | EQII |
| 100010911 | Wings of Shadow | EQII |
| 100012351 | Aspect of the Brood | EQ |
| 100012425 | Painting: Arabesque | EQIIP |
| 100021556 | Helm of the Wyrmslayer | EQIIP |

Nothing else in the client marks a bonus (extra) pack card; no 16th-card or "bonus loot" logic was found. The bonus-loot chance appears only as display strings (0x1083/0x1084) and in achievement 0 "Secret Surprise!". Bonus-loot rolling is server side (UNPROVEN that live added it as an extra card).
