# Legends of Norrath (LoN) card game -- install bundle

This bundle revives the Legends of Norrath card game that ships inside the RoF2 client
(`LegendsOfNorrath\LegendsOfNorrath.dll`). It contains the server side (a card service in
`world`, a session hand-off in `zone`, database tables), the client side (the `dinput8.dll`
code that points the card game at your server and adds PvP hosting), one quest script, the
card data files and the research notes needed to maintain it.

It was written for an EQEmu fork with the same layout as ours: the same `world` / `zone` /
`common` tree, the same item tiers (+1000000 Enchanted, +2000000 Legendary), the same `/claim`
(`veteran_reward_templates` / `account_rewards`) system and the same client `dinput8.dll`
project (`eqgame_dll`, MQ2-style detours). Everything here is **only** the card game plus one
small `/claim` fix it depends on.

**What is NOT in this bundle.** We use a custom marketplace for buying booster packs and starter
decks; you will need to create your own way to sell/grant products (the card service grants a
product to an account by inserting a row into `lon_delivery`: `account_id`, `archetype`,
`quantity`, `source`). Some loot-card prizes (house pets / placeable items) need player housing
or another workaround; without it they are not placeable.

Read this whole file before applying anything. Section 3 is the install order.

---

## Contents

1. Overview
2. Client setup first
3. Install checklist (in order)
4. Reference tables (message classes, rules, tables, client patches, session packet)
5. The /claim lore fix (walkthrough)
6. Card prizes (loot cards)
7. Gameplay rules and design
8. Known crash traps
9. Test plan
10. Files in this bundle

---

## 1. Overview

```
 EQ client (eqgame.exe + your dinput8.dll)                       Server
 ------------------------------------------                      ------
 LegendsOfNorrath.dll  --TCP-->  127.0.0.1:15782  (relay thread in dinput8)
                                       |
                                       +----TCP---->  world.exe : Custom:LoNPort (15792)
                                                      world/lon_service.cpp (the card service)
 dinput8 <--- OP_NMSLoNSession 0x141C (token, host, port, link buttons) --- zone.exe
                                                      zone/nms_lon_session.cpp
```

- **The card service** (`world/lon_service.cpp`, ~5,700 lines) runs inside `world.exe` on TCP
  port `Custom:LoNPort` (default 15792). It speaks the card game's own binary protocol (4-byte
  big-endian frame length; LoN varints; strings carry their NUL; each class level writes
  `(classId, version)` before its fields). It answers login, collection sync, decks, pack
  opening, loot-card redemption, single-player rewards, Loremaster achievements, preferences,
  posted trades, the Trade Lobby with live trading, custom avatars, Casual Games (PvP),
  Tournaments, the home screen panels and the leaderboard box. It is off unless
  `Custom:LoNEnabled` is true. It loads the card data from `Custom:LoNDataPath` (default
  `lon_data`, relative to the server root) at start.
- **The server owns everything.** Every card copy is a `lon_card` row; the client only builds,
  sends and displays. Packs are rolled server-side (live's 11 common / 3 uncommon / 1 rare
  layout, loot and ultra-rare chances by rule). Rewards are checked against the begin/end the
  client reported (`lon_game`) with minimum turns/seconds. Trades are validated and swapped in
  guarded transactions; posted-trade cards are held in escrow (moved to account 0).
- **Session token.** When the client's dinput8 reports in (`OP_NMSClientInfo`, which the DLL
  already sends), zone creates a random 32-hex-character token in `lon_session` (12 hours; a live
  one is re-sent on every zone-in) and sends `OP_NMSLoNSession` (0x141C) with the token, the host
  to relay to (`Custom:LoNPublicHost`, else the world address the client already uses, else
  127.0.0.1), the port (`Custom:LoNPort`) and the link-button switches/addresses. The token is the
  only identity the card service accepts; any other session string is refused.
- **Client.** dinput8 (a) wraps eqgame's `LoadLibraryExA` import so that, the moment
  `LegendsOfNorrath.dll` loads, its hard-coded gateway literal
  (`lvsccg-02-01.station.sony.com`, RVA 0x62B8D8) becomes `127.0.0.1`; (b) runs a relay on
  `127.0.0.1:15782` to the host/port from 0x141C; (c) writes the token into eqgame's
  `--sessionID=` buffer so the card game logs in by itself (no username/password box); (d)
  closes the "Session Login -- Entering main lobby..." box after a successful login; (e) patches
  the card game for PvP hosting (P1-P7) and greys the navigator link items that are off (P8); (f)
  drops every URL the card game asks to open except the configured link-button pages (the
  card game's Buy/store button therefore does nothing -- hook your own shop there if you have one;
  see `OpenUrlPost` in `nms_lon_link.cpp`).
- **PvP model.** World never runs card rules. For a PvP game it launches an ordinary network game
  on both clients; the host's `SetupGame` carries the marker `'NMSH'` (0x4E4D5348) in its Server ID
  for seat 1, on which the host's dinput8 turns that client's game into the authority. World only
  relays game commands between the two clients and records the result both clients report.

---

## 2. Client setup first

### Unpacking Legends of Norrath into the client

Nothing to unpack: the stock RoF2 client already contains the `LegendsOfNorrath\` folder. Check
it is present and complete (list below); if a client copy is missing it, copy the folder from a
stock RoF2 client. Your patcher must ship it if your master client copy does not have it.

The client folder must contain `LegendsOfNorrath\` with at least:

- `LegendsOfNorrath.dll` (the build these patches were written against; every patch checks its
  bytes first and does nothing on a different build)
- Qt 4 DLLs: `QtCore4.dll`, `QtGui4.dll`, `QtNetwork4.dll`, `QtScript4.dll`, `QtSvg4.dll`,
  `QtXml4.dll` (plus `OpenAL32.dll`, `libogg.dll`, `libvorbis.dll`, `libvorbisfile.dll`,
  `libtheora.dll`, `dbghelp.dll`)
- `cards.rcc`, `effects.rcc`, `foil.rcc`, `resources.rcc`, `sounds.rcc`
- `browser\`, `data\` (archetypes, decks, campaign.dat, ...), `locale\`
- `Microsoft.VC80.CRT\` and `Microsoft.VC80.CRT.manifest`
- (`client.de.qm`, `client.fr.qm`, `qt.de.qm`, `qt.fr.qm` are optional translations)

Nothing in that folder is modified on disk. dinput8 patches the card game **in memory** each time
it loads (it unloads when the player closes it, and the next open loads a fresh copy):

- the gateway literal at LoN RVA **0x62B8D8** (`lvsccg-02-01.station.sony.com` -> `127.0.0.1`),
  written from the `LoadLibraryExA` wrapper before the card game's `Initialize` copies it;
- eqgame's URL callback table (0xF8034C slots 0/1), the session id buffer (0xF800D0) and the
  login-result handler -- see section 4.4 for every address.

---

## 3. Install checklist (in order)

Paths are relative to the bundle root. Each server diff is a plain unified diff of only the LoN
lines with NO context lines; apply each with `git apply --unidiff-zero <file>` (verified with
`git apply --check --unidiff-zero` against our tree just before LoN).
Where a diff fails on your tree, section 4 and the per-file notes below say where each hunk goes.

### 3.1 Server code

1. **New files** -- copy as is:
   - `server/new/world/lon_service.cpp`, `server/new/world/lon_service.h` -> `world/`
   - `server/new/zone/nms_lon_session.cpp` -> `zone/`
2. **Diffs** (`git apply --unidiff-zero server/diffs/NN_*.diff`; if a hunk does not apply, paste it
   by hand as described):
   - `01_world_main.cpp.diff`: `#include "world/lon_service.h"` with the other world includes;
     `LoNService::Process();` inside the main loop lambda next to `dynamic_zone_manager.Process();`;
     `LoNService::Start();` once, after the loop lambda is defined and before
     `EQ::Timer process_timer(loop_fn);`.
   - `02_world_CMakeLists.txt.diff`: `lon_service.cpp` in `world_sources`, `lon_service.h` in
     `world_headers`.
   - `03_zone_CMakeLists.txt.diff`: `nms_lon_session.cpp` in `zone_sources`.
   - `04_zone_client.h.diff`: in `class Client` (public), declare `void SendLoNSession();`.
   - `05_zone_client_packet.cpp.diff`: at the end of `Client::Handle_OP_NMSClientInfo`, call
     `SendLoNSession();`. If your tree has no `OP_NMSClientInfo` handler, call it anywhere the
     zone knows the player's dinput8 is present (it must not go to clients without the DLL --
     0x141C is unknown to a stock RoF2 client; the DLL swallows it).
   - `06_common_emu_oplist.h.diff`: add `N(OP_NMSLoNSession),` to the emu op list.
   - `07_utils_patches_patch_RoF2.conf.diff`: add `OP_NMSLoNSession=0x141C`. **Every**
     `patch_RoF2.conf` your servers load needs this line (the source copy in `utils/patches/` and
     any deployed copy, e.g. the server root's `assets/patches/`). Check first that 0x141C is not
     already used by one of your own opcodes; if it is, pick a free one and change it in the conf
     AND in the DLL (`case 0x141C:` in `eqgame.cpp`).
   - `08_common_eq_packet_structs.h.diff`: `NMSLoNSession_Struct` (172 bytes, static_assert) and
     the `NMS_LONLINK_*` defines. Put it anywhere among the other custom structs.
   - `09_common_ruletypes.h.diff`: every `Custom:LoN*` rule (31 lines). Put them **inside**
     `RULE_CATEGORY(Custom)` ... `RULE_CATEGORY_END()`. If you have no `Custom` category, create one
     or rename the category in the code (`RuleB(Custom, ...)` etc. in the three new files).
   - `10_zone_client.cpp.claim_lore_fix.diff`: `Client::TryReward` -- see section 5.
3. **Dependencies the code assumes you already have** (all present in our fork's common base):
   `EQ::Net::TCPServer` (`common/net/tcp_server.h`), `common/compression.h` (zlib Deflate/Inflate),
   `ZSList::Instance()->SendEmoteMessageRaw` (world -> a character's chat), `character_parcels`
   (stock parcels; fallback delivery for an item without a `/claim` template),
   `veteran_reward_templates` + `account_rewards` (stock `/claim`), `ZoneConfig::get()->WorldAddress`.
4. **One card game per PC: the card game is limited to the active looter. You will need to add the
   card game to your own active-looter check -- implement `IsActiveLooter()` in
   `lon_service.cpp` for your system (it is a stub that allows everyone until you do).**
5. Build world and zone. The code compiles under MSVC and Linux GCC/clang.

### 3.2 Database

Apply `server/sql/01_*.sql` ... `10_*.sql` **in file-name order**. Each file's header has its
original manifest version, its guard query and the condition (also in the matching
`*.guard.sql`). Renumber them into your own manifest (put the SQL text in your manifest entry
and the guard in `.check` / `.condition`), or run them by hand once each. All statements are
plain MySQL 8 / MariaDB 10.6 SQL (`IF NOT EXISTS`, `INSERT IGNORE`).

| Step | Original | What |
| --- | --- | --- |
| 01 | v431 | card tables + loot-card -> EQ item map (332 cards) |
| 02 | v433 | Loremaster achievements + saved preferences |
| 03 | v435 | `/claim` templates for every loot item (claim id 5000000 + item id) |
| 04 | v436 | clears a stored `Custom:LoNMonthlyPacks` (no-op on a fresh install) |
| 05 | v437 | posted trades + `Custom:LoNPostedTradeDays`, `Custom:LoNActiveLooterOnly` rows |
| 06 | v438 | custom avatars + `Custom:LoNMaxCustomAvatars` |
| 07 | v439 | loot-card fixes (new ornament augs, models, title clicks, familiars, card map, filled packs) -- **housing parts removed**, see the file header |
| 08 | v440 | `Custom:LoNPvPEnabled` row (seeded false) |
| 09 | v442 | PvP results, ratings, tournaments + 12 rules |
| 10 | v443 | `lon_news` (home screen) + link-button / MOTD rules |

Not included on purpose: our v432 / v434 (marketplace rows and prices) and v441 (unrelated spell
text). After step 07, **run shared_memory** (items, npcs) and restart zones (`/claim` templates
load at zone boot). `server/sql/health_check_reference.sql` is an optional read-only check of
all ten steps; its last line is the verdict.

Step 07 depends on rows that exist in stock PEQ plus your item tiers (listed in its header). It
also needs a quest handler for click spell **36878** that grants the title whose `title_set` equals
the clicked item id (ours: `global/global_player.pl`, `EVENT_ITEM_CLICK_CAST`:
`if ($spell_id == 36878) { plugin::AddTitleFlag($item_id, $client) unless plugin::HasTitle($item_id) }`).
If you do not have one, add one or the ten title cards do nothing.

### 3.3 Rules

All rules are in section 4.2. The minimum:

- `Custom:LoNEnabled` = `true`
- `Custom:LoNPublicHost` = the public host name or IP players reach world on (empty = the world
  address the client already uses, which is right for most servers; set it when world sits behind
  a different address).
- `Custom:LoNPvPEnabled` = `true` when you want Casual Games and Tournaments.

Set rules from a zone that uses your default ruleset (or in the database), then restart world
(the port is read once at start) and let players zone once (0x141C is sent on zone-in).

### 3.4 Card data

Copy the bundle's `lon_data/` folder to the **server root** (next to `bin/`, so the default
`Custom:LoNDataPath` = `lon_data` finds it). It holds `archetypes/` (8 files), `decks/` (53
`.eqd` starter/tournament decks) and `campaign.dat`. These are byte-identical copies of the stock
RoF2 client's `LegendsOfNorrath\data\` files (verified file by file); you can copy them from any
stock client instead. World logs an error and the service stays off if they are missing.

### 3.5 Network

Open / publish **TCP 15792** (or your `Custom:LoNPort`) on the world host: firewall, Docker port
mapping, NAT/port forward, cloud security group. The client-side port 15782 is loopback only and
needs nothing. If world runs in a container, publish the port on the container too.

### 3.6 Client DLL

1. Copy `client/new/eqgame_dll/nms_lon_link.cpp` and `nms_lon_link.h` into `eqgame_dll/`.
2. Apply `client/diffs/01_eqgame_dll_eqgame.cpp.diff`, `02_eqgame_dll_MQ2Pulse.cpp.diff`,
   `03_eqgame_dll_eqgame_dll.vcxproj.diff` with `git apply --unidiff-zero` (verified against our
   pre-LoN tree). By hand, the hunks are:
   - `eqgame.cpp`: `#include "nms_lon_link.h"`;
     `LoNSetSessionID_Detour` (+ `DETOUR_TRAMPOLINE_EMPTY` trampoline) -- **replaces** eqgame's
     session-id setter at 0x7A1460 (never call the trampoline: the original's copy loops jump back
     into the first 5 bytes, which crashed eqgame at startup), writing
     `--sessionID=<token>` (or the placeholder `NMS`, which the service refuses) into 0xF800D0;
     `LoNLoginResult_Detour` + `NMSLoNPulse()` -- closes the session login box after a successful
     login, re-hooked on every card-game load with `DetourFunction` (fresh trampoline);
     `case 0x141C:` in `HandleWorldMessage_Detour` -> `NMSLoN::OnSession(buf, size); return 1;`;
     in `InitHooks()`: `EzDetour(0x7A1460 ...)` and `NMSLoN::Install(baseAddress);`.
   - `MQ2Pulse.cpp`: in `Heartbeat()`, `{ extern void NMSLoNPulse(); NMSLoNPulse(); }`.
   - `eqgame_dll.vcxproj`: `nms_lon_link.cpp` / `nms_lon_link.h`.
3. `nms_lon_link.cpp` uses `nms_paths.h` (`NMSPaths::File(NMSPaths::kLogs, "LoNPvP.log")`) for its
   PvP log; if you lack it, write the log path another way. It is deliberately standalone
   (winsock2 must not meet the `<winsock.h>` that `MQ2Main.h` pulls in) -- do not include MQ headers
   in it.
4. Build `eqgame_dll` Release|Win32 and ship the new `dinput8.dll`.
5. **PvP host marker.** `kHostMarker` in `world/lon_service.cpp` and `kPvPHostMarker` in
   `nms_lon_link.cpp` must be the same value: **0x4E4D5348** (`'NMSH'`: the int value spells N M S H
   from the most significant byte down; in memory, little-endian, the bytes are 48 53 4D 4E; on
   the LoN wire, as a varint, `88 34 D5 E4 04`). Change both together if you pick your own.

### 3.7 Quest script

`quests/global/items/script_8884.pl` -> your quests `global/items/`. It is the click for the loot
card prize Chalandria's Fang (item 43987, whose `scriptfileid` must be 8884 -- check it in your
`items` table). Rogues get 3 Concentrated Chalandria's Bite, others 1 Chalandria's Bite, best tier
the character can use, once per 20 hours (own data-bucket lock + the item's recast). It uses
`HasClass("Rogue")`; on a server without a class bitmask use `GetClass() == 9`.

---

## 4. Reference tables

### 4.1 Message classes the service speaks

Frame: 4-byte big-endian length, then (client -> world) a 12-byte preamble world strips, then the
body. Body = per header level `(classId, version)` varints, leaf first, then fields. "Levels" =
header levels. Full field layouts: `research/eqlib_LoN/LoN_messages.txt`. S->C = world sends,
C->S = client sends.

**Login / session / home screen**

| Class | Name | Ver | Levels | Dir | Purpose |
| --- | --- | --- | --- | --- | --- |
| 0x19E | LoginCommand_GetConnectionServer | 1 | 3 | S->C | gateway answer: connect to 127.0.0.1:15782 (the relay) |
| 0x19A | LoginCommand_SendPassword | - | - | C->S | refused (no passwords) |
| 0x19B | LoginCommand_SendSessionID | 7 | - | C->S | the token from 0x141C |
| 0x051 | LoginCommand_Results | 6 | 3 | S->C | Succeeded, Explanation, AccountID, UseExplanation, ServerID, SessionID (`"NMS"`), zlib empty restrictions vector |
| 0x072 | IntroduceAccount | 1 | 2 | S->C | account properties (Loremaster 0x1082-0x1087, PvP 0x1055/0xFC8/0xFC9, flags 0xFC5, saved prefs) |
| 0x135 | LoginCommand_DisplayMOTD | 1 | 3 | S->C | home screen Message of the Day (`Custom:LoNMOTD`) |
| 0x1C9 | NetworkCommand_News | 1 | 1 | S->C | home screen Getting Started (`lon_news`) |
| 0x1CA | LeaderBoardData | 1 | 1 | S->C | home screen Leaderboards box (top 10 from `lon_pvp_stats`) |
| 0x070 | Ping | - | - | both | echoed |
| 0x116 | Disconnect | - | - | C->S | session closed |
| 0x076 | AccountCommand_SetPreferences | - | - | C->S | stored in `lon_account_pref`, sent back at login |

**Collection, decks, packs, loot, rewards**

| Class | Name | Ver | Levels | Dir | Purpose |
| --- | --- | --- | --- | --- | --- |
| 0x053 | SyncCommand_Start | 3 | 3 | S->C | start of sync |
| 0x057 | SyncCommand_Update | - | - | C->S | starts the collection sync |
| 0x054 / 0x055 / 0x056 | StartInstances / SendInstances / Complete | 2 | 3 | S->C | the collection, in chunks |
| 0x139 | ProcessBulkFulfillment | - | - | C->S | open owned products (packs/decks) |
| 0x08F | SyncCommand_RemoveInstances | 1 | 3 | S->C | the opened product leaves |
| 0x08D | SyncCommand_StartDelivery | 6 | 3 | S->C | the new cards arrive |
| 0x169 / 0x16A | DeckCommand_AddOnlineDeck / DeleteOnlineDeck | - | - | C->S | `lon_deck` |
| 0x16B | DeckCommand_Populate | 1 | 3 | S->C | saved decks at login |
| 0x1B9 | ECommCommand_RedeemItemGrant | 3 | 2 | both | loot card -> EQ item (/claim); "Choose a Pack" card -> booster |
| 0x1E7 | AccountCommand_RecordSinglePlayerGame | - | - | C->S | begin/end of a single-player game (`lon_game`) |
| 0x19F | SetCampaignStatus | - | - | C->S | scenario result (hero archetype in CustomInt, one status per difficulty) |
| 0x1E0 / 0x1E1 / 0x1E2 | Achievement / NotifyAchievement / SendAchievementList | 1 | 1/2/2 | S->C | Journal + Loremaster |
| 0x1E3 | RequestGameplay | - | - | C->S | achievement list request |
| 0x15E / 0x017 / 0x016 | InstanceID / PropertySet / ValueData | 1/1/4 | - | - | embedded objects |

**Posted trades, Trade Lobby, live trades, avatars**

| Class | Name | Ver | Levels | Dir |
| --- | --- | --- | --- | --- |
| 0x18D | TradeCommand_AcceptPostedTrade | - | - | C->S |
| 0x18E / 0x18F | AddPostedTrade / CancelPostedTrade | 1 | 3 | both |
| 0x190 / 0x192 | EndPostedTrades / StartPostedTrades | 1 | 3 | S->C |
| 0x191 | RequestPostedTrades | - | - | C->S |
| 0x193 | TransferPostedTrade | 2 | 3 | S->C |
| 0x194 | PostedTrade (object) | 3 | 1 | S->C |
| 0x195 | PostedTradeError | 1 | 3 | S->C |
| 0x1AF | SetListenStatus | - | - | C->S |
| 0x1CB | GetAccountsByName | 2 | 2 | both |
| 0x1CD | GetGuildsByName (always answers none) | 1 | 3 | both |
| 0x5C / 0x5D | LobbyCommand_Join / Leave | 4 / 3 | 3 | both |
| 0x5E / 0x5F | LobbyCommand_AddGroups / RemoveGroups | 1 | 3 | S->C |
| 0x67 | Lobby (object) | 3 | 1 | S->C |
| 0x61 | LobbyCommand_SendText | 4 | 3 | both |
| 0x6A | LobbyCommand_BulkJoin (zlib block) | 3 | 3 | S->C |
| 0x89 | LobbyCommand_ChangeStatus | 1 | 3 | both |
| 0x19C | LobbyStats | 3 | 3 | S->C |
| 0x93-0x9D, 0x110, 0x111 | TradeCommand_IssueTradeRequest ... ClearAcceptState, StartTradeSync, LockTrade | 1-2 | 3 | both |
| 0x6B | ErrorMsg | 2 | 2 | S->C |
| 0x1C0 | ArchCommand_GetArchetypeID | 1 | 2 | both |
| 0x1C1 / 0x1C2 | StoreArchetype / DeleteArchetype | - | - | C->S (no reply) |
| 0x1C3 | AddArchetypesToDB | 2 | 2 | S->C |
| 0x1C4 | ValidateUsername | 1 | 2 | both |

**Casual Games, PvP, Tournaments**

| Class | Name | Ver | Levels | Dir |
| --- | --- | --- | --- | --- |
| 0x68 | Match (object in AddGroups) | 3 | 2 | S->C |
| 0x63 | MatchCommand_SetReady | 1 | 4 | both |
| 0x65 | MatchCommand_SelectDeck (never echoed) | 2 | 4 | C->S |
| 0x7C | MatchCommand_SetState | 1 | 4 | S->C |
| 0x11178 | EQMatchCommand_LaunchGame | 1 | 5 | S->C |
| 0x11177 | EQGameCommand_SetupGame (Server ID = marker on the host's seat 1) | 12 | 4 | S->C |
| 0x3A | GameCommand_SelectDeckForPlayer (host only) | 2 | 3 | S->C |
| 0x8B | LostPlayer | 2 | 3 | S->C |
| 0x10B | RemovePlayer (loss type 1 only) | 4 | 3 | S->C |
| 0x40, 0x75, 0x84, 0xA0, 0x107, 0x108, 0x17F, 0x1D7 | OutOfSync, ReadyForStart, FatalError, ConcedeGame, Accept/OfferAdjourn, GameOverReached, EndGame | - | - | relayed / watched |
| 0x11172 | EQDeck (object) | - | 2 | inside SelectDeck |
| 0x12A | LobbyCommand_Concede (tournament Drop button) | 1 | 3 | C->S |
| 0x124 | DisplayStats | 2 | 3 | S->C |
| 0x125 | UpdateTournament | 1 | 3 | S->C |
| 0x12B | SetTournament (tid, round, timer, group) | 2 | 3 | S->C |
| 0x1CC | StartTournament | 1 | 3 | S->C |
| **0x7F0** | **PvP result report (ours; no client reads it)** | 1 | 1 | C->S (dinput8 only) |

The 0x7F0 report fields (all varints): MatchID, the reporter's seat, kind (1 lost / 2 won), the seat
it names, the loss type, the game's turn number. The dinput8 relay inserts it into the card game's
upstream stream only at a frame boundary.

Lobby groups: 2 = Casual Games, 3 = Trade Lobby, 5 = Tournaments (group 1 is never created: it
would make the client auto-join the main lobby). PvP / tournament game ids start at 2000000; trade
lobby ids are 1000000 + n.

### 4.2 Rules (`Custom:` category)

| Rule | Type | Default | Meaning |
| --- | --- | --- | --- |
| LoNEnabled | bool | false | Run the card service in world. Off = the card game cannot log in. |
| LoNPort | int | 15792 | TCP port world listens on. Restart world after a change. |
| LoNPublicHost | string | "" | Host/IP the DLL relays to. Empty = the world address the client uses. |
| LoNDataPath | string | lon_data | Folder (relative to the server root) with archetypes/, decks/, campaign.dat. |
| LoNLootPermille | int | 125 | Per-booster chance (per mille) the rare slot is an EverQuest loot card. |
| LoNUltraRarePermille | int | 125 | Per-booster chance (sets that have them) the rare slot is an Ultra Rare. |
| LoNMonthlyPacks | int | 0 | Free "Choose a Pack" cards per account per month. 0 = none (design default). |
| LoNMinScenarioSeconds | int | 120 | A scenario win pays only if its begin was reported at least this long before the end. |
| LoNMinScenarioTurns | int | 3 | A scenario win pays only after this many turns. |
| LoNActiveLooterOnly | bool | true | One card game per PC via `IsActiveLooter()` (a stub that allows everyone until you implement it). Checked at login and every 10 s. |
| LoNMaxCustomAvatars | int | 5 | Custom avatars an account may keep. |
| LoNPostedTradeDays | int | 7 | Posted trades expire after this many days; cards go back to the poster. |
| LoNPvPEnabled | bool | false | Casual Games and Tournaments. Off = create/join/queue refused with a line in EQ chat. |
| LoNBonusLootPermillePerLevel | int | 5 | Loremaster bonus loot per level (per mille). 0 = off. |
| LoNPvPMinSeconds | int | 180 | A PvP win by the rules pays the Journal / moves rating only after this many seconds (and LoNMinScenarioTurns turns). |
| LoNRatingStart | int | 1500 | Starting tournament rating. |
| LoNRatingK | int | 32 | Elo K factor. |
| LoNRatingPairDailyGames | int | 3 | Counted games per 24 h against the same opponent, per mode (anti-farm). |
| LoNConcedeLosesRating | bool | true | Tournament concede / disconnect / missing report is rated. |
| LoNRankedMinGames | int | 5 | Rated tournament games before a player is "ranked". |
| LoNRankedMinOpponents | int | 3 | Different opponents before a player is "ranked". |
| LoNTournamentPlayers | int | 4 | Players the open queue waits for (2-16). |
| LoNTournamentTitle | string | Open | Queue / tournament title. |
| LoNTournamentRewardText | string | Journal + ranking | Reward Structure column text. |
| LoNTournamentOneEntryPerIP | bool | true | Refuse a queue entry from an IP already in the queue. |
| LoNTournamentNoShowSeconds | int | 180 | Wait for a busy player before a forfeit. |
| LoNLinkButtons | int | 0 | Navigator items that work, as a sum: 1 Help, 2 Review EULA, 4 Forums, 8 Leaderboards (+ View More...), 16 Website. Sent on zone-in. |
| LoNHelpUrl / LoNForumsUrl / LoNLeaderboardUrl / LoNWebsiteUrl | string | "" | http(s) page per button (max 255 chars). Empty = that item stays grey. |
| LoNMOTD | string | (welcome text) | Home screen Message of the Day (plain text). |

The SQL steps seed `rule_values` rows (ruleset 1) for most of these; the compiled defaults apply
where there is no row.

### 4.3 Tables

| Table | Key columns | Purpose |
| --- | --- | --- |
| lon_account | account_id | one row per EQ account that opened the card game; `starter_granted`, `last_monthly` |
| lon_card | id (instance), account_id, archetype, series, tradable, source | every card copy; account 0 = held in escrow |
| lon_deck | id, account_id, name (unique per account), archetype, deck (EQDeck bytes) | saved decks |
| lon_session | token (32 hex), account_id, character_id, expires_at | one-time login tokens (12 h) |
| lon_game | (account_id, game_id), script, began_at, ended_at, result, turns | single-player games reported by the client (reward checks) |
| lon_reward_claim | (account_id, script, difficulty, variant), archetype | rewards already paid; difficulty -1 = first-win loot card |
| **lon_delivery** | id, account_id, archetype, quantity, source, delivered_at | **products granted from outside** (your shop, GM, NPC): INSERT a row with `delivered_at` NULL; world delivers it at login, or within seconds if that account is in the card game |
| lon_loot_redemption | id, account_id, character_id, card_archetype, item_id | log of redeemed loot cards |
| lon_loot_item | card_archetype -> item_id | which EQ item each EverQuest loot card gives |
| lon_achievement | (account_id, achievement_id), xp | Journal achievements completed (Loremaster XP) |
| lon_account_pref | (account_id, prop_key), value (raw ValueData) | the card game's saved preferences |
| lon_posted_trade | id, account_id, poster_name, recipient_*, state, accepted_by | posted trades (state 0 open, 1 done, 2 cancelled, 3 expired, 4 being accepted) |
| lon_posted_trade_line | (trade_id, side, archetype), quantity | offered (0) / wanted (1) cards |
| lon_posted_trade_escrow | card_id, trade_id, account_id | copies held for an open trade |
| lon_custom_avatar | archetype_id (AUTO_INCREMENT from 200000001), account_id, name (unique), blob, deleted_at | custom avatars |
| lon_pvp_stats | account_id, ratings, wins/losses, tournaments, games, opponents, ranked, hidden | PvP record + tournament rating (leaderboard source; `hidden` = GM flag) |
| lon_pvp_match | id, game_id, mode (1 casual / 2 tournament), seats, winner/loser, end_type, counted, reason, rating_change | every PvP game |
| lon_tournament / lon_tournament_entry | id, title, players, rounds, winner / (tournament_id, account_id), place, wins, losses, dropped | tournaments |
| lon_news | id, sort_key, title, body, enabled | home screen Getting Started items |

**Products for `lon_delivery.archetype`** (the card game's own product ids; the matching deck
files are in `lon_data/decks/`): set *N* booster = `N*1000000 + 900001`; set *N* starter decks =
`+900002` Fighter, `+900003` Mage, `+900004` Priest, `+900005` Scout (sets 1-4 starter decks, set 5
tournament decks; sets 6-15 boosters only). Sets: 1 Oathbound, 2 Forsworn, 3 Inquisitor,
4 Oathbreaker, 5 Ethernauts, 6 Against the Void, 7 Storm Break, 8 Travelers, 9 Vengeful Gods,
10 Doom of the Ancient Ones, 11 Dragonbrood - The Anarchs, 12 Legacies, 13 Priestess of the
Anarchs, 14 Fall of the Estarim, 15 Debt of the Ratonga. Example (one Oathbound booster):

```sql
INSERT INTO lon_delivery (account_id, archetype, quantity, source) VALUES (12345, 1900001, 1, 'shop');
```

Decks never contain loot cards; boosters roll them per `Custom:LoNLootPermille`.

### 4.4 Client patches (dinput8)

Addresses: eqgame VAs are for image base 0x400000 (the code rebases them); LoN RVAs are relative
to `LegendsOfNorrath.dll`'s load address. Every patch checks the listed bytes first; a mismatch
means a different build and that patch group is skipped (logged to `Logs\LoNPvP.log` for P1-P8).

**Link / session (eqgame + LoN)**

| What | Address | Checked | Change |
| --- | --- | --- | --- |
| LoadLibraryExA import | eqgame 0x9C0274 (called at 0x7A1785) | - | pointer -> `LoadLibraryExA_Wrap` |
| Gateway literal | LoN RVA 0x62B8D8 | `lvsccg-02-01.station.sony.com\0` | -> `127.0.0.1` (rest zeroed) |
| URL callbacks | eqgame 0xF8034C slots 0 (open URL) / 1 (open URL + POST) | - | -> `OpenUrl` / `OpenUrlPost`: configured link pages only, everything else dropped |
| Session id setter | eqgame 0x7A1460 (`--username=` setter 0x7A14B0; argv at 0x7A192E) | - | replaced (detour, trampoline never called) |
| Session id buffer | eqgame 0xF800D0, 0x40 bytes | - | `--sessionID=<token>` |
| Login result handler | LoN RVA 0x4A6E50 (`WASessionLoginDialog`) | `6A FF 68` | detour: after success (reason != 0x4C) `QTimer::singleShot(0, box, "1close()")` |
| Event field getter | LoN RVA 0xACAA0 | `56 8B F1` | called (field +8 type, +4 value) |
| Relay | 127.0.0.1:15782 | - | listen thread; one pump thread per connection; upstream = 0x141C host:port |

**PvP host (P1-P7, one all-or-none group) and link items (P8, its own group)**

| Patch | Slot / code (LoN RVA) | Original (RVA) + checked bytes | Replacement | Purpose |
| --- | --- | --- | --- | --- |
| P1 | 0x6416A0 = SetupGame vtable 0x641664 + 0x3C | 0x2737E0 `64 A1 00 00 00 00 6A FF` | `PvPSetupExec` | if Server ID (+0x28) == 0x4E4D5348 and PlayerID (+4) == 1: set Game+0x139 (isServer) directly (never via vtable+0x1B0) |
| P2 | 0x64142C = EQGame vtable 0x64139C + 0x90 | 0x0F4180 `55 8B EC 6A FF 68` | `PvPHostFlush` | Game::flush for the host game: receive, send seat 2's queue, clear queues |
| P3 | 0x64139C = EQGame vtable + 0 | 0x272BD0 `56 8B F1 E8` | `PvPHostDtor` | set +0x304 on the dying host game so the singleton is cleared (no reuse of freed memory) |
| P4 | code 0x0E2058 (9 bytes) | `80 BE 39 01 00 00 00 75 23` | last 2 bytes -> `90 90` | host keeps its own display actions (else black board) |
| P5 | code 0x0E233E (19), 0x0E24E9 (19), 0x0E262E (23), 0x0E278F (19) | context bytes ending in a `75 xx` jne | jne -> `90 90` | host does not show seat 2's game text ("You draw ...") |
| P6 | 0x641444 = EQGame vtable + 0xA8 | 0x269AC0 `64 A1 00 00 00 00 6A FF 68 8B E7 5A` | `PvPGameWon` | report a win (class 0x7F0) |
| P7 | 0x641448 = EQGame vtable + 0xAC | 0x26F8B0 `64 A1 00 00 00 00 6A FF 68 34 EC 5A` | `PvPGameLost` | report a loss (loss type) |
| P8 | 0x649848 = EQNavigator vtable 0x649714 + 0x134 | 0x4712C0 `83 EC 08 53 55 56 57 8B F1 E8`; helpers 0x471220 / 0x471270 `56 8B 74 24 08 6A 00/01 8B CE FF 15`, tail at +0x3F `5F 5E C2 04 00` | `NavUpdateActions` | grey Help (+0x90), EULA (+0x8C), Forums (+0x7C), Leaderboards (+0x80), Website (+0x84) while their switch is off |

P6/P7 report only for games whose MatchID (+0x60) >= 2000000, started (+0x108), not over (+0x121),
no fatal error (+0x1C4). Code pages are written with `PAGE_EXECUTE_READWRITE` and
`FlushInstructionCache`. Skirmish, scenario and tutorial games use EQMainController's own vtable
and are untouched by every patch.

### 4.5 OP_NMSLoNSession (0x141C, server -> client)

| Offset | Size | Field |
| --- | --- | --- |
| 0 | 36 | `token` -- NUL-terminated, 32 hex characters |
| 36 | 128 | `host` -- NUL-terminated host or IP to relay to |
| 164 | 4 | `port` -- `Custom:LoNPort` |
| 168 | 4 | `link_buttons` -- bits 1 Help, 2 EULA, 4 Forums, 8 Leaderboards, 16 Website (0 = all grey) |
| 172 | 4 | tail magic `0x55534D4E` ('NMSU') |
| 176 | var | four entries, in order help, forums, leaderboards, website: `uint16 len` + `len` bytes (no NUL, max 255; an over-long address is sent with len 0) |

The DLL accepts size >= 172 (an older server without the tail = all buttons grey) and ignores a
malformed tail. It only opens an address that is http(s), printable ASCII, no spaces/quotes.

---

## 5. The /claim lore fix (`10_zone_client.cpp.claim_lore_fix.diff`)

Loot cards are delivered to the account's `/claim` list (template 5000000 + item id), so the
player picks the character. Stock `Client::TryReward(claim_id)` did this, in order:

1. found the template, 2. **spent the claim** (DELETE the `account_rewards` row when amount is 1,
else amount - 1), 3. created the item -- if that failed it saved and returned **true**, 4. checked
lore -- on a conflict it deleted the item and returned **true**.

So claiming a LORE item the character already owned (or a template whose item could not be
created) consumed the claim and gave nothing; the player could not claim it on another character.
The fix reorders it:

1. create the item; if that fails, return **false** (claim untouched);
2. check lore; on a conflict send the stock duplicate-lore message (`DuplicateLoreMessage`), delete
   the item and return **false** (claim untouched, still in the list for another character);
3. fill the container slots as before;
4. only now spend the claim (same DELETE / UPDATE as stock), then put the item in the inventory.

Nothing else in `TryReward` changes. It matters for every `/claim` reward, not just LoN.

---

## 6. Card prizes (loot cards)

- **Redemption.** In the card game the player redeems an EverQuest loot card
  (`ECommCommand_RedeemItemGrant`). World looks up `lon_loot_item` (card archetype -> EQ item),
  takes the card copy, and adds `account_rewards(account_id, 5000000 + item_id, 1)` (amount
  incremented if present). The item then appears in `/claim` for any character of the account.
  If the item has no template, world falls back to a parcel for the character that opened the
  card game (`character_parcels`, from "Legends of Norrath"). Every redemption is logged in
  `lon_loot_redemption`. "Choose a Pack" cards turn into the chosen booster instead.
- **Ids.** The map uses base item ids (as PEQ). Items with tiers keep them (+1000000 Enchanted,
  +2000000 Legendary rows exist for e.g. Crown of Lightning 54916 / 1054916 / 2054916; the card
  gives the base id).
- **Ornaments.** Ornament cards give the live "... Ornament(ation)" augment itself (19 created at
  live ids 152662-152680); 35 ornament items (and their tier rows) got their weapon model.
- **Titles.** Ten title cards give an item that clicks spell 36878; your quest handler grants the
  `titles` row whose `title_set` = item id (section 3.2).
- **Familiars / swarm pets.** Eight new `pets` rows + npcs 999700-999707 so their clicks work.
- **Packs.** Ping's potion packs (13) and the 9 "Bones" packs arrive filled: their contents are
  template rows 2-7 of the bag's claim.
- **Need player housing** (without it they are ordinary items and cannot be placed): Baby Red Dragon
  (54998) and Flame Wurmling (60958) -- house pets on our server; Estarim Family Crest banner
  (67905); the paintings (Painting: Estarim Family 64570, Painting: Santug Suit 67718 and the other
  mapped paintings); idol trophies (54997, 60957, 67909, 64571); building blocks (64593-64597);
  Hermit's Hideaway (54999, a house exterior model). Details: `research/lon_loot_card_audit.md`
  section 7.
- **Left as they were on purpose:** Dragon-Marked Amulet, Painting: Fancy Dress Party, 1-Month
  Subscription, every mercenary contract card, Legendary Norrathian Standard, Dragon Brood Crypt Key.
- **Chalandria's Fang** (43987) needs `quests/global/items/script_8884.pl`.

---

## 7. Gameplay rules and design

- **One card game per PC: the card game is limited to the active looter. You will need to add the
  card game to your own active-looter check -- implement `IsActiveLooter()` in `lon_service.cpp`
  for your system (it is a stub that allows everyone until you do).**
- **No free packs.** `Custom:LoNMonthlyPacks` = 0: free packs per account invite throwaway accounts
  made to farm loot cards. A new account gets one random starter deck (fixed cards, never a loot
  card). Boosters come from `lon_delivery` (your shop) or rewards.
- **Single-player rewards** follow live's scenario rules: one reward card per scenario x difficulty
  (every lower difficulty too; foil on Hard), the scenario's first-win EverQuest loot card once per
  account, campaign completion. Only paid when the client reported the game's begin
  `LoNMinScenarioSeconds` earlier and it lasted `LoNMinScenarioTurns` turns (anti-farm).
- **Loremaster.** Journal achievements pay XP (our values; live never published them; level 4 needs
  111 XP), XP makes the level, each level adds `LoNBonusLootPermillePerLevel` to a booster's chance
  of an extra Loremaster loot card.
- **Posted trades.** Offered copies are held in escrow (`lon_card.account_id` = 0 +
  `lon_posted_trade_escrow`) while the trade is up, so nothing else can touch them; accepting swaps
  both sides in one guarded transaction after a conditional claim (state 0 -> 4), so a trade is
  accepted once. 20 public posts per account, 50 cards per side, expiry `LoNPostedTradeDays`.
  Guild-only trades are not offered.
- **Live trades** run from the Trade Lobby (group 3): roster, status, chat, right-click Trade. Every
  card is checked for ownership and tradability on every step.
- **Custom avatars**: ids from 200000001 (never an id the client reads as foil); cap
  `LoNMaxCustomAvatars`; the avatar copy is not tradable.
- **PvP host model.** Casual Games (group 2): Create / Join a match, both Ready starts it, the
  creator hosts. World sends guest first (LaunchGame, SetupGame seat 2), then host (LaunchGame,
  SetupGame seat 1 + marker, SelectDeckForPlayer seat 1 then seat 2). World relays every game command
  unchanged (+4 zero bytes), never runs a rule, drops authority-only classes from the guest. Ends:
  concede / game over / end game seen (60 s trailing relay); leaving or disconnecting first is a
  loss; 10 idle minutes = no result.
- **Results** come from both clients' dinput8 (class 0x7F0). A result needs both reports agreeing,
  or what world saw itself (concede, disconnect), or one report naming a side that disconnected; in
  a tournament one report stands alone (the silent side stopped responding).
- **Ratings are tournament-only.** Casual Games are unrated (as live: "non-rated games"); they pay
  the Journal (72 winner / 73 loser) when won by the rules after the minimum length. Tournament games
  move the Elo rating (both sides by the same step) and pay Journal 74. Same-opponent games beyond
  `LoNRatingPairDailyGames` per 24 h count on the record only; games between two accounts on the
  same address move no rating and pay no Journal (anti-farm).
- **Tournaments** (group 5): free, constructed, single elimination, best of one,
  `LoNTournamentPlayers` players, one open queue that starts when full and is replaced at once, one
  entry per IP (`LoNTournamentOneEntryPerIP`), no-show forfeit after `LoNTournamentNoShowSeconds`.
- **No item prizes from PvP or tournaments, ever** (a second PC would farm them): the Profile record,
  the rating and the Journal only.

---

## 8. Known crash traps

Each of these crashed or hung the client during development. Keep them if you change the code.

1. **Never whisper a player as the card game** (a card-game whisper whose sender is the receiver
   leaves the `[*listener*]` slot empty and the card game aborts). All service lines go to EQ chat
   (`TellCharacter` / `PvPTell`).
2. **RemovePlayer loss type is 1 (concede), never 2.** Type 2 posts display action 0x45, which
   writes the turn-timer text that only exists when the game has a Duration; ours is 0 (match
   property 0x273), so the client writes through NULL.
3. **Display gate (P4).** Without P4 the host never gets display action 0x60 and shows a black
   board. Without P5 the host shows seat 2's private text.
4. **Never send `NetworkCommand_EULA` 0x1C8.** It opens the mode-1 EULA dialog whose Decline closes
   the card game.
5. **Equal list lengths.** News 0x1C9 / LeaderBoardData 0x1CA (one header level) index their second
   and third lists by the first list's count with no bounds check: build each group in one loop. The
   Trade Lobby BulkJoin lists run in parallel too.
6. **LobbyCommand handlers read the AccountID first** (every LobbyCommand starts with it); a missed
   field desyncs every later read.
7. **IntroduceAccount must carry the Loremaster keys 0x1082-0x1087** (the Profile window crashes
   without them) and 0x1055 must be ValueData type 13 (int-int map) only.
8. **A nameless deck crashes Select Deck** (DeckDB.cpp:194) -- starter decks get names.
9. **A zlib block that does not inflate asserts the client.**
10. **Lobby rules:** never create group 1 (auto-join of the main lobby); a non-match child group
    crashes the right-click menu; a Match must reach the client before any Join echo for it; one
    AddGroups per client per match; MatchCommand_* have four header levels; SelectDeck is never
    echoed; property 0xFA5 is only an int list; Duration 0, structure 1, 2 players, game type 1.
11. **Tournaments:** a lobby must reach the client in AddGroups before SetTournament/standings;
    never send the fee keys 0xFD7/0xFD9, the rating gate 0xFDE, 0xFDB/0xFDC/0xFA5/0x1069 in a queue;
    everyone ends with SetTournament timer -1 (else the client cannot leave group 5); launch a
    round's games only once both players are off the last game screen.
12. **PvP:** GameID is never 0; teams {1:1, 2:2}; decks only after SetupGame and only to the host;
    world-built game commands carry a TimeStamp >= 2^28; P1 writes isServer directly, never through
    vtable+0x1B0 (its base setter clears the client-side game singleton).
13. **Client DLL:** never call the session-setter trampoline (crash at startup); re-hook the login
    box on every LoN load (the DLL unloads on close); use `DetourFunction` (no MQ detour list) so a
    hook whose DLL unloaded is forgotten rather than written into freed memory at EQ exit.

Details and addresses for each: the specs in `research/`.

---

## 9. Test plan

**One PC**

1. World log at start: the service loads the data and listens on 15792 (no "data missing" error).
2. Zone in with the new dinput8. Open the card game (LoN button): it logs in by itself, the session
   box closes, the home screen shows the MOTD and Getting Started.
3. New account: one starter deck in the collection. Insert a `lon_delivery` booster (4.3); it opens
   to 15 cards.
4. Play a tutorial / scenario to the end (>= 3 turns, >= 120 s): the reward card arrives; the Journal
   updates; a second win on the same difficulty pays nothing.
5. Redeem a loot card: the item appears in `/claim`; claim it on a character that already has the
   lore item -> refused, the claim stays; claim on another character -> works.
6. Close and reopen the card game twice: it logs in each time (re-hook works).
7. Custom avatar: Deck Builder > Create Avatar; it survives a relog.
8. Post a trade, cancel it: the cards come back. `LoNLinkButtons` = 31 with URLs set + zone: the
   navigator items are enabled and open the pages; 0 = greyed.

**Two PCs / two players (different accounts, different addresses)**

9. Trade Lobby: both see each other; chat; a live trade swaps cards both ways.
10. Posted trade accepted by the other account.
11. `LoNPvPEnabled` = true: Casual Games, create + join + both Ready: the game starts on both
    clients, the host's board is not black, the host does not see the guest's draw text. Play to a
    win; `lon_pvp_match` gets a row with both reports agreeing; Profile record updates; no rating
    change (casual). Concede and disconnect cases = a loss for the leaver.
12. Tournament with `LoNTournamentPlayers` = 2: queue both, the pairing launches, the winner gets
    Journal 74 and a rating change; both clients return to the lobby and can leave group 5.
13. `NMS\Logs\LoNPvP.log` (or your log path) on the host shows "P1-P7 installed", "P1 host role
    ON", "P2 first send", "P3 host game destroyed" and the result report.

---

## 10. Files in this bundle

```
README.md                         this file
MANIFEST.txt                      every file with its sha256
server/new/world/lon_service.cpp  the card service (new file)
server/new/world/lon_service.h
server/new/zone/nms_lon_session.cpp   zone: sends OP_NMSLoNSession (new file)
server/diffs/01..10_*.diff        changes to existing server files (LoN hunks only)
server/sql/01..10_*.sql           database steps in apply order (+ *.guard.sql each)
server/sql/health_check_reference.sql   optional read-only content check
client/new/eqgame_dll/nms_lon_link.cpp  relay, gateway patch, URL callbacks, P1-P8 (new file)
client/new/eqgame_dll/nms_lon_link.h
client/diffs/01..03_*.diff        eqgame.cpp, MQ2Pulse.cpp, eqgame_dll.vcxproj
quests/global/items/script_8884.pl     Chalandria's Fang loot-card prize
lon_data/                         card data for the server (copy of the client's LegendsOfNorrath\data subset)
research/*.md                     specs: trade lobby, live trade, posted trades, casual games, PvP build,
                                  PvP host black screen, tournaments, home screen + links, leaderboard button,
                                  achievements, misc (avatars, loot mapping), loot-card audit (+ verify)
research/eqlib_LoN/               protocol reference: LoN.h (offsets), LoN_README.md, LoN_service.md,
                                  LoN_messages.txt (every message, field by field), LoN_opcodes.txt,
                                  LoN_errors.txt, LoN_events.txt, LoN_eventflow.txt, LoN_senders.txt,
                                  LoN_campaigns.txt
```

The research notes describe the development history and sometimes name manifest versions (v431 ...
v443) and our own decisions; map those versions with the table in 3.2.
