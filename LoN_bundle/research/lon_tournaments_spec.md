# Legends of Norrath Tournaments (navigator group 5): wire spec, checked against the client

Researched 2026-10-06. Static RE only: LegendsOfNorrath.dll in Ghidra (image base 0x10000000), plus a pefile/capstone
read of the same file for vtables and jump tables. Read-only: nothing was renamed, built, run or copied.

Encoding primitives, `Header(cls, ver, levels)`, nullable vs plain objects, PropertySet/ValueData, Join/BulkJoin/
AddGroups/UpdateLobbyStats layouts: all as in `lon_trade_lobby_spec.md` sections 0-5 (verified there). This file
only adds what group 5 needs. Every field below comes from the client's **read** function (vtable slot +0x24) and
the effect from its **execute** (slot +0x3C), both quoted by VA. Anything not traced to code is marked
**UNVERIFIED**.

Vtable slot map used for every LobbyCommand below (checked on UpdateLobbyStats 0x10617674, whose read 0x1014FCE0
and execute 0x1014FB40 the trade spec proved): +0x20 version, +0x24 read, +0x28 write, +0x3C execute.

Shared with the Casual Games research (`lon_casual_games_spec.md`, other agent): the child-lobby list model
(`WALobbyGroupModel`), `RemoveGroups` 0x5F, the join dialog base (`WAJoinMatchDialog`: Join + `MatchCommand_SelectDeck`),
the deck validator, the play-format property 0xFA9, and the Match class 0x68. They are referenced here, not re-derived.

---------------------------------------------------------------------------------------------------------------

## 0. Short answer

- Clicking **Tournaments** (`0x10473910`: `PUSH 0; PUSH 5; CALL 0x10471A50`) runs the same navigator code as the
  Trade Lobby. With our existing UpdateLobbyStats default max 1000 it passes the full check and sends
  `Join{group 5, role 1}` (`0x1046FB20`). World refuses it today (`lon_service.cpp:2395`).
- To open the screen, world needs exactly what group 3 got: **AddGroups(Lobby 5, type 1, Containing 0)**, the
  **Join echo** with AccountID = self, and a **BulkJoin** roster. The echo routes to `0x1045FE90(3, 5, 1)` →
  `0x1045DE50` case 3 → `param_3 == 5` → **`WATournamentScreen`** (`new 0x38`, ctor `0x104B6010`,
  `tournamentlobby.ui`). An empty screen needs nothing else.
- The screen lists **child lobbies of group 5** in two tables: **type 5 = tournament queues**
  (`tournamentQueueTableView`), **types 6/7/8 = started tournaments** (`startedTournamentTableView`). Both are plain
  `Lobby` 0x67 objects sent by AddGroups with ContainingGroupID = 5. Their columns are read from the lobby's
  **general PropertySet** (first of the five nullable sets).
- Types: **5 queue, 6 constructed tournament, 7 booster draft, 8 sealed** (section 2.3). Draft and sealed need
  server-generated card pools pushed mid-tournament (SetDraft instance lists, a temporary "limitedCollection"); leave
  them out.
- Paid entry is **entirely property-driven**: only when a queue carries 0xFD7 (int list → `WAEntryFeeModel`) or 0xFD9
  does the join dialog show fee widgets, and staking used `SynchronizationCommand_LockTournament/UnlockTournament`
  0x1DC/0x1DD. Never send those keys or classes and there is no fee or Station Cash path at all.
- A round's game can use **our existing PvP launch** (LaunchGame 0x11178 + EQ SetupGame with the host marker); the
  client does not need a Match object to start a game (`lon_pvp_build_spec.md` 1.2.1). The tournament context is
  carried by `SetTournament` 0x12B (controller +0x2C = tournament id), and standings/pairings by
  `UpdateTournament` 0x125 and `DisplayStats` 0x124 (results dialog with a "Bye"/"Dropped" aware standings table).
- **The hard part is the result, not the protocol**: `GameOverReached` 0x17F carries no winner (read `0x10106210` =
  base GameCommand fields only; execute `0x10106100` returns 1). World can see concedes, disconnects and timeouts in
  the relay, but a normal win needs a winner report from our dinput8 (section 5).

Recommended scope: **scheduled or fill-to-start, free, single-elimination (or Swiss) constructed tournaments, 4-8
players, best-of-1, card-pack prizes through the existing StartDelivery/GiveCards code.** Section 7 has the build
plan.

---------------------------------------------------------------------------------------------------------------

## 1. Clicking Tournaments

### 1.1 Navigator (verified)

`0x10471A50(group, chooser)` (decompiled):
- `chooser` is 0 for Tournaments, so the lobby-chooser branch is skipped.
- `max = FUN_1006AC30(5)` (ctl+0x90 map, falls back to ctl+0x8C), `cur = FUN_1006ACB0(5)` (ctl+0x68 map).
- If `max <= cur`: allowed only for "Staff"/"StaffLogin" account flags **or, for group 5 only, when ctl+0x2C (the
  current tournament id, section 3.1) is non-zero** (`if (iVar5 == 5) { FUN_100682D0() != 0 → join }`). Otherwise
  the "Lobby Full" dialog (`ui.wanavigator.cpp.39/40`).
- Then `0x1046FB20(5)`: `Join` v4 ctor `0x101457F0`, AccountID = self, GroupID = 5 (`FUN_10145190`),
  RoleType = 1 (`FUN_101451B0(1)`), sent by `FUN_10068380`. Same bytes as the trade spec's navigator Join with
  `03` replaced by `05`.

The other navigator thunks, for reference: `0x104738F0` = group 8 (chooser 0), `0x10473900` = group 2 (chooser 1,
Casual Games), `0x10473920` = group 3 (Trade).

### 1.2 What world must send (all layouts are the trade spec's, GroupID 5)

1. **AddGroups 0x5E v1**, once per connection (it can be the same AddGroups as group 3 with count 2, or a second
   message). Lobby 0x67 v3, **no flag**:
   ```
   Header(0x5E,1,3) U(self) U(1)
     Header(0x67,3,1)
     5 x [U(0) Header(0x17,1,1) U(0)]   // present, empty sets
     U(0) U(0) U(0) U(0) U(0)           // role lists
     U(0)   // Containing 0
     U(5)   // GroupID 5
     U(1)   // eLobbyTypeID 1
     U(1)   // deck: null
     U(0) U(0)   // tournamentRound, subState
   ```
   Event-3 handler `0x10452E00` auto-joins only group 1 (trade spec), so this does not join by itself.
2. **UpdateLobbyStats 0x19C v3**: add key 5 to map 1 (population) and map 4 (max), or rely on the default max
   (ctl+0x8C = 1000) we already send. Keep population accurate so the navigator never says "full".
3. On **Join{5}**: echo to the joiner (AccountID = self, name empty, empty PropertySet) → BulkJoin(5) roster →
   Join announcement to the other group-5 members → UpdateLobbyStats. Identical to group 3.

### 1.3 What the echo does (verified)

`0x10453170` (event 5, self branch): group 5 is not 1; type 1 is not 4/9; `FUN_10153990(5)` (Match cast) is null;
type is not 5/7/8/6 → `FUN_1045FE90(3, 5, 1)`. `0x1045DE50` case 3:
- `FUN_10156B60(5)` (enter lobby). For a type-1 lobby it **sends Leave for every other joined lobby** except group 1,
  groups 5/containing-5, and (because param is 5) lobbies of type 6/7/8 (`0x10156B60` decompile). So a player who
  was in the Trade Lobby sends `Leave(3)`; world already handles that.
- `FUN_10150B60(2)` → the client sends `ChangeStatus 2`.
- `param_3 == 5` → `new WATournamentScreen` (`0x104B6010`), `vtable+0xE4(5)` (setGroup), `+0xCC` (show).

Screen setup `0x104B6860` (WATournamentScreen vtable `0x106A2834` slot +0xF8):
- Two `WALobbyGroupModel` (`0x10452BD0`, vtable `0x1068C544`): +0x60 mode = 1 (queue table) and 2 (started table),
  +0x5C group = 5. Each connects to `addLobbyGroup(int,int,int)` / `removeLobbyGroup(int,int)` (`0x10452670`).
- Account flags "ShowShadowOdyssey", "ShowHoliday", "ShowLivingLegacyButton", "ShowComicConButton",
  "ShowFrostfell", "ShowRingOfScale" add scenario buttons. Do not send them.
- `if (ctl+0x2C == 0) FUN_104B6100(0)` hides `dropButtonPanel`.

---------------------------------------------------------------------------------------------------------------

## 2. What the screen lists

### 2.1 Row filter (verified)

`handleAddLobbyGroup` = `0x10452430(containing, group, type)` and repopulate `0x104529E0`:
- `containing` must equal the model's group (5).
- mode 1 accepts **type 5**; mode 2 accepts **types 6, 7, 8**; mode 0 (casual) accepts type 2.
- `FUN_103A5490(group) == -1` (not already listed).
`removeLobbyGroup` drops the row (RemoveGroups 0x5F; layout in the casual spec / catalog: base + int list).

So a queue = `AddGroups` of `Lobby{Containing 5, GroupID Q, type 5, general set = properties below}`; a started
tournament = the same with type 6. GroupIDs must be unique and must not collide with 1/2/3/5/8 or PvP match ids
(we use 2000000+n for those; suggest 3000000+n for queues and 3100000+n for tournaments).

### 2.2 Columns and where the values come from (verified)

Header setup `0x10451DE0`, tournament branch (+0x60 ≠ 0). Value lookup: model data `0x10452740` →
`0x104523F0` (child must have Containing == 5) → `Lobby::GetProperty` = Lobby vtable `0x106128BC` slot +0x30 =
**`0x10131BB0`**, which **returns nothing when the general set (full+0x34) is null** and otherwise maps some keys:

| Col | Header | Key | Source (0x10131BB0 / translator 0x10459B70) | Type to send |
|---|---|---|---|---|
| 0 | "#" | 0xFB2 | type 5: property **0xFD5** if present; types 9/10: 0xFE0; else the GroupID. Started table (mode 2) shows 0xFD5 via translator | int (ValueData type 2) |
| 1 | "Title" | 1 | translator key 1: **0xFA6** string + " " + format name of **0xFA9** + " " + **0xFA7** (disasm `0x10459BF5/0x10459C7A/0x10459CDF`) | 0xFA6 string (type 3) |
| 2 | "Status" | 0xFBC | type 5: raw; types 7/8 with draft state < 5: draft state names; otherwise a round string from Lobby round (full+0x70) | int |
| 3 | "Cur #" | 0xFB5 | queues: **count of role-2 (player) members** of that lobby (`FUN_10131910(.,2)`); started: count of standings entries whose second value ≠ -3 (translator case 0xFB5) | (computed) |
| 4 | "Max #" | 0xFB6 | → **0xFA8** | int |
| 5 | "Rounds" | 0xFD2 | raw | int |
| 6 | "Time" | 0xFB9 | → **0x273**, else **0xFB1** | UNVERIFIED type (likely string or int start time) |
| 7 | "K" | 0xFCF | raw (rating K-factor) | leave out |
| 8 | "Pack Pool" | 0xFD0 | raw | leave out (draft/sealed) |
| 9 | "Reward Structure" | 0xFD1 | raw | string, e.g. "1st: 3 packs" (UNVERIFIED display of a string) |
| 10 | "Structure" | 0xFD3 | string **"swiss"** or **"single"** → translated; anything else shown as is | string (type 3) |

Missing keys show an empty cell (`FUN_103A5210` default path); no assert on a missing key was found in the model or
translator. **The format name for 0xFA9 and 0xFA7 is decoded by the translator's subclass** (WALobbyTranslator slot
+0x68 is pure virtual, `0x1058317C`), shared with the casual spec. Until that is decoded, send 0xFA9 = the
constructed format value the casual spec settles on (quick-match uses 0x11174, `lon_pvp.md` section 3) and omit
0xFA7.

The queue's player role list in the Lobby object (role key 2) makes "Cur #" right on first sight; keep it current
with Join/Leave relays to group-5 members, or by re-sending the lobby (RemoveGroups + AddGroups).

### 2.3 Lobby type meanings (verified from the self-join and screen code)

| type | evidence | meaning |
|---|---|---|
| 5 | `0x10453170`: opens `WATournamentDialog` "tournamentDialog"; `0x10132290` requires "RegisteredUser" for type 5 | tournament **queue** |
| 6 | self-join: only `FUN_10156B60` (enter lobby), no screen change; screen case 3 returns early for 6/7/8 | started **constructed** tournament |
| 7 | self-join: draft state (full+0x48) < 5 → screen 8 (`DraftScreen`, ctor `0x103F0170`), == 5 → screen 9 (deck builder) | **booster draft** |
| 8 | self-join: draft state ≤ 4 → screen 9 (deck builder) | **sealed** |
| 10 | `0x10156950`: leaving a type-10 lobby clears ctl+0x54; `0x10156B60` type-10 branch | other (league?) UNVERIFIED, do not use |

Full+0x48 (getter `0x1012D730`, setter `0x1012D720`) is written only by `SetDraft` execute (`0x1014C2C7`).

### 2.4 Joining a queue (verified)

Row action → `WATournamentViewController` `0x104B8710(queue)`:
1. If the queue lacks property **0x1057**, the account restriction flags `FUN_100052E0(2/3/4)` show "ineligible
   territory / state / blocked" dialogs. Our accounts do not carry those flags (UNVERIFIED which account key backs
   `FUN_100052E0`; we have never sent one). Sending 0x1057 (any value, it is a presence test via Lobby vtable+0x40)
   skips the check entirely — optional belt and braces.
2. **`Lobby::CanJoin` `0x10132290`** must return true. It fails when:
   - 0xFA5 (int list) contains 1 (friends only) or 4 (guild only, 0x1013 = guild id);
   - 0xFDC (string list) is present and the account has none of those flags ("restricted to certain account classes");
   - 0xFDB (int list) is present and the account is not in it ("invited users");
   - **type 5 and the account lacks "RegisteredUser"** in 0xFC5 (we already send it at login);
   - type 5 and 0x1069 == 1: Gen Con check (`fopen("GenCon.Tourney")`);
   - **0xFDE > 0: rating gate** "This tournament is restricted to users with a [*ratingType*] rating
     [*comparisonText*] [*ratingLimit*]": 0xFDD = rating key (0xFC8 overall, 0xFC9, 0xFCA multiplayer, 0xFCB),
     0xFDF = comparison set (3 '>', 6 '>=', 5 '<=', else '<'), compared with the account's rating property.
   **Send none of 0xFA5/0xFDC/0xFDB/0x1069/0xFDE.** (Guild-only is the 0xFA5 = {4} + 0x1013 path.)
3. Opens `WAJoinTournamentDialog` (`0x10450AB0`, base `WAJoinMatchDialog` `0x1044F8F0`, `jointournament.ui`).
   Setup `0x10450C40`: shows the fee tree only if **0xFD7 is an int list (type 6)**, the entry-choice dropdown only if
   **0xFD9 is type 8**; otherwise both are hidden. If the queue's `FUN_1012EAC0()` is 2 or 3 the dialog needs no deck
   (+0x84 = 1; sealed/draft, UNVERIFIED which property backs it). Summary label: 0xFA6 title and 0xFA9 "Format: %1".
4. OK = `WAJoinMatchDialog::accept` `0x104500F0` (shared with casual):
   - "You must select a deck." unless a deck is chosen; the `deckValidator` must pass ("make sure your deck validates
     for the Tournament's format", format = 0xFA9);
   - sends **`Join` v4 {GroupID = queue, RoleType = 2 (player), or 3 if 0xFAC (observers) is true, Password from
     0xFAD if present, int list = entry choice if 0xFD9}**;
   - then **`MatchCommand_SelectDeck` 0x65 v2 {AccountID, MatchID = queue id, DeckID string, DeckIDOnly, nullable
     EQDeck}** (ctor `0x10160540`, `FUN_1015F700(queue)`, `FUN_10160670(deck)`). Read layout: casual spec.
5. World answers with the **Join echo for the queue** (role 2). Self-join type 5 (`0x10453170`):
   `WATournamentDialog` (`0x104B2E60`, "tournamentDialog", `0x104B2EF0` stores the queue id at +0x48),
   `ChangeStatus 4` (`FUN_10150B60(4)`), enter lobby. The dialog text (`0x104B3150`): "You are currently in
   tournament queue %1, %2. By closing this window you will be removed from the tournament" with %1 = **0xFD5**
   (int) and %2 = the title translation. It shows the queue's member list, so send **BulkJoin(queue)** and Join
   announcements to the queue's members exactly as for group 3.
6. Closing/destroying that dialog (`0x104B2FE0`, `0x104B2F30` on removeParticipant(self, queue), `0x104B2F80` on
   removeLobbyGroup(queue)) calls `FUN_10156950(queue)` → **Leave(queue)** + `ChangeStatus 2`. World must
   therefore accept a Leave for a queue that it already removed (ignore it).

Server-side validation at step 4: check the SelectDeck deck against the DB collection (ownership) and the format;
on failure, remove the player from the queue (Leave to the others, RemoveGroups is not needed) and tell them via
`GameServiceError` 0x8A or a tell. A failed deck can also appear in standings as opponent value **-2 "Invalid Deck"**
(section 3.3).

---------------------------------------------------------------------------------------------------------------

## 3. Tournament messages the client reads (all reads verified)

### 3.1 LobbyCommand_SetTournament 0x12B v2 (3 levels)

Read `0x1014EB30`: header, base (AccountID), **tournamentID (+8), round (+0x10), timer (+0xC)**, `v>1` **groupID
(+0x14)**. Note the wire order: id, round, timer.
```
Header(0x12B,2,3) U(self) U(tid) U(round) U(timer) U(T)
```
Execute `0x1014E9E0` → controller `0x1006B560(tid, round, timer, group)`:
- ctl+0x34 = timer; if timer > 0, ctl+0x38/+0x3C = now (countdown start);
- ctl+0x4C = group; ctl+0x50 = that lobby's type (0 if unknown — **send AddGroups(T) first**);
- **ctl+0x2C = tid**, ctl+0x30 = round; if tid == 0 the per-tournament list at ctl+0xB4 is cleared;
- posts event 0x4C(tid, group, round, timer).
`WATournamentManager` event filter `0x104B4980`, case 0x4C: if timer == **-1** it calls `0x1006B560(0,0,0,0)`
(leave tournament mode); if tid == 0 it clears the lobby manager's lists (`0x10453100`); then emits
`updateTournamentTimer(tid, group, round, timer)` → `WATournamentScreen::handleUpdateTournamentTimer`
(`0x104B63B0`): tid ≠ 0 shows the **Drop** panel. `WATournamentStatus` shows "TID: %1 Rnd: %2 Time: %3".

What ctl+0x2C changes elsewhere (verified readers of `0x100682D0`):
- Navigator: group 5 is never "full" (1.1).
- `0x10156950` (leave): with ctl+0x2C ≠ 0 and ctl+0x58 == 0, the client **refuses to send Leave** for group 5, for
  ctl+0x4C (the tournament lobby), and for any Match whose 0xFD5 equals the tid ("tried to leave ... without
  confirm"). The drop button is the only exit.
- `0x10453700`: no observing matches while in a tournament.
- In-game menus `0x10414763`, `0x1041B3E9`: different end-of-game/leave buttons in tournament games (UI only).
- Deck builder `0x1023E435`: for tournament lobby type 7/8 it locks the play format to the lobby's 0xFA9.
- `0x10240D44`: the deck builder's Ready sends `SelectTournamentDeck` with TournamentID = ctl+0x2C, GroupID =
  ctl+0x4C (sealed/draft only).

**Clear it at the end**: `SetTournament{tid, round, timer = -1, T}` (or tid 0). The client's own Drop also clears it
locally (`0x104B5F90`: `0x1006B560(0,0,-1,0)`).

### 3.2 LobbyCommand_StartTournamentMsg 0x1CC v1

Read `0x1014F5A0`: base, int (+8), int (+0xC). Execute `0x1014F3E0` posts event 0xB3(+8, +0xC). WAMainWindow
(jump table `0x10460408`, handler `0x10460521`) opens a message box "StartTournamentMessage", title "Tournament",
text = message enum 0x1E **"Tournament %1 has begun. Please wait while the tournament matches players against each
other. You will be automatically joined to your match..."** with **%1 = the second int** (`0x10460600:
FUN_100ACAA0(1)`). The catalog's labels "GameID/MatchID" are wrong. Send both = tid.
```
Header(0x1CC,1,3) U(self) U(tid) U(tid)
```

### 3.3 LobbyCommand_UpdateTournament 0x125 v1 (standings into the lobby)

Read `0x10150750`: base, **GroupID (+0x1C), TournamentID (+8), standings map (+0xC), Round (+0x20), SubState
(+0x24)**. The map reader `0x101438B0`: `U(count)` + count × `FUN_101771C0` = **three ints: U(account) U(points)
U(opponent)**.
```
Header(0x125,1,3) U(self) U(T) U(tid) U(n) [U(acct) U(pts) U(opp)]*n U(round) U(substate)
```
Execute `0x101507E0`: event 0x48(group, tid, round, 3 int lists, substate); if lobby T exists:
`FUN_10134620(map)` (standings stored in the lobby, feeds the started table's "Cur #"), `0x1012E860(substate)`
(full+0x74), `0x1012E7F0(round)` (full+0x70); both setters emit a UI refresh (`0x104D2670`). No jump table handles
event 0x48 (scan of all `0x100AC800` switch tables), so its visible effect is through the lobby object.

Opponent value meaning, from `WATournamentUserModel::data` `0x104B7F20` (pair lookup `0x104B7D90`):
column 0 = **points**, column 1 = account name, column 2 = opponent: **> 0 account name, -1 "Bye", -2 "Invalid
Deck", -3 "Dropped"**, else empty. Names come from the client's account cache, so every account in the map must be
known (BulkJoin/Join announcements of T, or DisplayStats v2's name map).

### 3.4 LobbyCommand_DisplayStats 0x124 v2 (pairings / standings dialog)

Read `0x10143B10`: base, **groupID (+0x24), TournamentID (+8), Round (+0x1C), standings map (+0xC, same triple
format), Message (+0x20), Duration (+0x28), MessageArg string (+0x2C)**, `v>1` **map<int,string> (+0x48)**
(`0x10143320` → `0x101770F0` = `U(key) S(value)`).
```
Header(0x124,2,3) U(self) U(T) U(tid) U(round)
  U(n) [U(acct) U(pts) U(opp)]*n
  U(msg) U(duration) S(arg)
  U(m) [U(acct) S(name)]*m
```
Execute `0x10143E60`: event 0x47(group, tid, round, msg, 3 int lists, [arg if non-empty]); for every name-map
account that is not cached, **creates it** (`FUN_10018430`) and sets the name (no assert); then
**`0x1006B560(tid, round, duration, group)`** (so DisplayStats also sets the tournament context and timer) and stores
the standings and round in lobby T.

Event 0x47 → `WATournamentManager` `0x104B44A0`: builds the message text from the enum `msg` through
`0x1046C340` (+ `%1` = MessageArg if present) and opens **`WATournamentResultsDialog`** (`0x104B5DE0`,
`tournamentresults.ui`, player table `WATournamentUserModel`). Useful enums (all in `0x1046C340`):

| msg | text |
|---|---|
| 0x0F / 0x10 | "%1 has dropped from tournament: %2" / "You have dropped from tournament: %1" |
| 0x11 | "Tournament begins now." |
| 0x12 | "Next round starts in 2 minutes." |
| **0x13** | **"Matchups for round: %1"** |
| **0x14** | **"Final Standings."** |
| 0x15 / 0x16 | draft pack opening / sealed deck building (do not use) |
| 0x17 | "%1 has conceded the match." |
| 0x1E | "Tournament %1 has begun..." |
| 0x20 / 0x21 | idle warnings (auto-concede in 2 minutes) |

Indices 4-6 of event 0x47 must be int lists (`0x104B44A0` asserts "mTypeID == kIntegerListTypeID" at
ValueData.h:0x146 otherwise). The execute always adds the three lists, even when empty, so this assert cannot fire
from the wire.

### 3.5 LobbyCommand_StartTournamentRound 0x12C v1

Read `0x1014F980`: base, tournamentID (+8), round (+0xC). Execute `0x1014F7C0`: event 0x4E(tid, round). **No UI
handler for 0x4E was found** in any event switch table (scan above) — harmless but useless. Optional.

### 3.6 LobbyCommand_Concede 0x12A v1 = the client's Drop (C→S)

Drop button → confirm (`0x104B63D0`, "You are about to drop from the %1 tournament (event number %2)") →
`dropFromTournament` `0x104B5F90`: sends **`LobbyCommand_Concede` (ctor `0x10140C80`: +8 = 0)** and clears its own
tournament context (`0x1006B560(0,0,-1,0)`). Read `0x10140E50`: base, int (+8). Execute `0x10140D20` returns 1
(no client effect). World: treat it as "drop the sender from its current tournament"; if they are in a game, it is a
loss (opponent -3 "Dropped" in the next standings).

### 3.7 Messages not to use

- `SynchronizationCommand_LockTournament` 0x1DC / `UnlockTournament` 0x1DD (base `ChangeFlags` 0x1DB): staked entry
  instances. Not needed without fees.
- `SetDraft` 0x12F, `SelectCard` 0x130, `SetDraftReady` 0x131, `SetupDraft` 0x132, `SetDraftedCards` 0x1CF,
  `SelectTournamentDeck` 0x133: draft/sealed only (section 6).
- `InfoEnumMessage` 0x128 (read `0x10145020`: Message + two `FUN_1010C3F0` objects; execute `0x10144970` posts
  event 0x61 only when the client has a current lobby). It can carry the same enums, but the argument objects were
  not traced. Prefer DisplayStats / StartTournamentMsg / tells.

---------------------------------------------------------------------------------------------------------------

## 4. How a round launches a game and how results flow back

### 4.1 Live's model (partly UNVERIFIED)

Evidence that live used **Match** objects inside tournaments: `0x10156B60` (enter lobby) for a type-2 lobby reads
its 0xFD5 (type 2) and, when it equals ctl+0x2C, clears ctl+0x58; `0x10156950` refuses to leave such a match; a
Match whose 0xFBC ≥ 4 is flagged started in AddGroups (`0x10139CB0` → `0x1015F2D0`). So live created one Match per
pairing with 0xFD5 = tid, joined both players ("You will be automatically joined to your match") and launched it.
The exact live order was not recovered.

### 4.2 Recommended: reuse our PvP launch, no Match objects

`lon_pvp_build_spec.md` 1.2.1 shows `EQMatchCommand_LaunchGame` → event 0x11 → screen 0xB with **no Match/group
lookup**. So a round needs no Match object: for each pairing, run the exact `lon_service.cpp` PvP start (guest
LaunchGame + SetupGame seat 2; host LaunchGame + SetupGame seat 1 with marker + SelectDeckForPlayer ×2; relay; ends
on concede/game over/disconnect). The decks are the ones submitted with the queue's SelectDeck (stored and validated
at join). Host = random of the pair (anti-farm rule from `lon_pvp.md` 6).

Avoiding Match objects also avoids the casual-games risks (Match property keys, the user-list Match cast, match
dialog). If the testers report that the post-game screen does not return to the tournament screen, the fallback is
sending the joiner a Join echo for group 5 again (it rebuilds WATournamentScreen; ctl+0x2C survives). **The
post-game return path under ctl+0x2C ≠ 0 was not traced: UNVERIFIED, test item T5.**

A player in a game still belongs to lobby T (they never left it; the leave guard blocks it), so world can send
UpdateTournament/DisplayStats any time; DisplayStats pops a dialog, so send it between rounds, not mid-game.

### 4.3 Results

world knows the end of every game from the relay (`lon_service.cpp` EndPvP): concede (`ConcedeGame` 0xA0 from seat
N → N loses), disconnect/leave (that side loses), idle (no winner), and `GameOverReached`/`EndGame` from the host
(**no winner field**, see section 0). Section 5 covers how to get the winner of a normal game. After each game:
1. update points (e.g. win 3, loss 0, bye 3; single elim: loser's opp = -3 next round);
2. `UpdateTournament` to all members of T;
3. when every game of the round is done: `DisplayStats{msg 0x13 "Matchups for round: %1", arg "<round>"}` with the
   new pairings (opponent = account / -1 bye), then start the games;
4. after the last round: `DisplayStats{msg 0x14 "Final Standings."}`, prizes (4.4), `SetTournament{..., timer -1}`,
   `RemoveGroups(T)` to group 5, and remove the players from T.

### 4.4 Prizes

The client has no prize table (`lon_pvp.md` 7). Pay from world with the existing delivery code
(`GiveCards` + `StartDelivery`, `lon_service.cpp:822-853`; source string e.g. "tournament", source_id = tid) and show
the reward text in column 9 (0xFD1). One card game per PC is gated by `IsActiveLooter()`, a stub you implement:
refuse queue joins from a second account on the same PC (the same check the rest of LoN uses).

---------------------------------------------------------------------------------------------------------------

## 5. Getting the winner (the open design item)

- `GameOverReached` 0x17F: read `0x10106210` = header + GameCommand base only; execute `0x10106100` = `return 1`.
- `ConcedeGame` 0xA0 identifies the loser by PlayerID (already used by EndPvP).
- A normal win (quest, avatar defeat, decking) is decided inside the host's master Game ("WinMap" in the Game dump,
  `lon_pvp.md` 6); nothing in the relayed stream was shown to name the winner. UNVERIFIED whether a state command in
  the stream (e.g. an element-map update) carries it.
Options, best first:
1. **dinput8 result report** on both clients at game over: read the Game's win state and send it upstream (via the
   EQ connection the DLL already uses, or a LoN message world can recognise). Pay only when both reports agree
   (`lon_pvp.md` 6 rule 1). Needs one more RE task: locate WinMap in the Game object (Game dump "WinMap", `0x100D…`
   accessors) — not done here.
2. **Concede-only results**: the loser must concede; a game that ends without a concede is a draw/replay. Simple,
   but players will not like it.
3. **Player report**: both players type the result (tell to a GM-like bot). Error-prone.

---------------------------------------------------------------------------------------------------------------

## 6. Draft and sealed (why to leave them out)

They need server-generated pools mid-tournament (verified in `SetDraft` execute `0x1014B870`):
- `SetDraft` 0x12F v2 read `0x1014C750`: base, tournamentID (+8), round (+0xC), timer (+0x10), **vector of inline
  card-instance objects (+0x18, `FUN_10077790`, 16-byte elements whose +4 is the archetype)**, groupID (+0x14), `v>1`
  bool **SendDeck** (+0x28).
- Execute: builds a client-side collection **"limitedCollection-<tid>"** (assert `FUN_10070510() != 0` at
  LobbyCommandSetDraft.cpp:0x74), adds the instances, sets the draft state (lobby full+0x48 = round), event 0x52
  (round, "clockwise"/"counterClockwise"), event 0x36 (collection). At round 5 (deck building) it merges the limited
  collection into the account's collection view (`0x1014C2CC` path) — a source of fake cards in the UI.
- SendDeck = 1 makes the client read `data\decks\tournament\t<acct>-<tid>.<ext>` and send `SelectTournamentDeck`
  0x133 (read `0x10149F60`: base, DeckID string, nullable deck, TournamentID, GroupID, bool Overwrite).
- Also `SetupDraft` 0x132, `SelectCard` 0x130, `SetDraftReady` 0x131, `SetDraftedCards` 0x1CF (reads not traced here;
  catalog fields only, UNVERIFIED).
Pools would need server-side instance ids that never enter the real collection, pick timers and pass logic. Weeks of
work and test time for little gain. Constructed only.

---------------------------------------------------------------------------------------------------------------

## 7. Asserts, aborts and stalls on these paths

Asserts (EverQuest exits):
- `LobbyCommandAddGroups.cpp:0x8C` "account": optional keys 0xFBD/0xFC1 in a lobby's set. Never send them.
- `ValueData.h:0x146` "mTypeID == kIntegerListTypeID": event 0x47 lists (cannot happen via DisplayStats; also
  `AddGroups` 0xFBD and `Lobby::CanJoin` paths if 0xFDB/0xFA5 are sent with the wrong type — send none).
- `ValueData.h:0x158` "mTypeID == kStringListTypeID": `Lobby::CanJoin` 0xFDC with a wrong type (not sent).
- `ValueData.h:0x11E` "mTypeID == kStringTypeID": `WAJoinTournamentDialog` summary 0xFA6 is guarded by a type-3 check
  first, so a wrong type only skips it. Still: **0xFA6 must be a string**.
- `wajoinmatchdialog.cpp:0xC2/0xF9`: the dialog's own deck (0xFB0) cannot be loaded — client-side deck problem, not a
  wire field.
- `LobbyCommandSetDraft.cpp:0x74`: collection manager missing (draft only).
- Join with a name and a null PropertySet (Join.cpp:0x5E), BulkJoin null set / bad zlib (:0x4F, :0x119), AddMember
  role 0 (Lobby.cpp:0x255): as in the trade spec. Queue joins use role 2; echo role 2.
- `"mGame"` asserts on GameID 0 and the PvP stream rules (`lon_pvp_build_spec.md`): unchanged.
Stalls / silent failures:
- SetTournament/DisplayStats for a group the client does not know: ctl+0x50 = 0, standings not stored. Send
  AddGroups(T) first.
- Standings entries for uncached accounts show blank names: include them in DisplayStats v2's name map or in T's
  BulkJoin.
- A lobby with a null general set shows empty cells (Lobby::GetProperty returns 0).
- A read failure (wrong class id or short field) jams the LoN stream (`lon_pvp_build_spec.md` 4.1). Treat as fatal.
- Leave for group 5 / T / tournament matches is refused by the client while ctl+0x2C ≠ 0 (by design); world must not
  wait for it.

---------------------------------------------------------------------------------------------------------------

## 8. Recommended scope and build plan

### 8.1 Minimal working feature

- **Free** entry, **constructed** decks (format = the one the casual work uses), **single elimination** (Structure
  "single") with byes, or Swiss for 3 rounds if preferred ("swiss" is also translated). Best of one.
- Two ways to start: a **queue that starts when full** (4 or 8), and/or a **scheduled** queue that starts at a time
  with whoever joined (min 2; byes fill the bracket). Admin-created via a GM command or rule values.
- Prizes: card packs by placement through the existing delivery code, plus Journal achievement 74 "Win a Tournament
  Game" per win (existing achievement code).
- Left out: entry fees, Lock/Unlock, rating gates, guild-only, invited lists, draft, sealed, observers, Match objects,
  K/rating updates.

### 8.2 Ordered build plan

**Phase A — screen and queues (no games). Testable on one PC.**
1. Group 5: AddGroups(5) next to group 3 at login; include 5 in UpdateLobbyStats; accept `Join{5}` with the same
   echo/BulkJoin/announce code as group 3 (refactor the group-3 code to take a group id). Handle Leave(5) and
   ChangeStatus. Test T1: Tournaments opens an empty WATournamentScreen; Trade Lobby still works after it.
2. Queue objects: `AddGroups{Lobby Containing 5, GroupID Q, type 5, general set {0xFA6 "Weekly Constructed"
   (string), 0xFD5 tid (int), 0xFA8 max (int), 0xFD2 rounds (int), 0xFD3 "single" (string), 0xFA9 format (int),
   0xFD1 "1st: 3 packs..." (string)}, role-2 list = queued accounts}` to everyone in group 5 (and at Join(5)).
   RemoveGroups(Q) when it closes. T2: the row shows in the queue table with the right columns.
3. Queue join: accept `Join{Q, role 2}` + `MatchCommand_SelectDeck{MatchID Q}` (validate ownership/format; store the
   deck), echo role 2, BulkJoin(Q), announce to Q's members, refresh the row (Cur #). Accept Leave(Q) (also after Q is
   gone). One card game per PC rule. T3: join with a valid deck shows the queue dialog with the member list; close
   leaves; an invalid deck is refused.

**Phase B — running a tournament (needs 2+ clients on a test server; a GM test like PvP's "ghost" mode can run 1 PC).**
4. Start: StartTournamentMsg(tid, tid) to the entrants; AddGroups(T: Containing 5, type 6, same properties, role-2
   list = entrants) to group 5; RemoveGroups(Q); Join echo for T to each entrant (role 2; no screen change) +
   BulkJoin(T); SetTournament{tid, round 1, timer, T}. T4: queue dialog closes, started table shows T, drop button
   appears.
5. Round: pairings (random seeding, byes = -1), `UpdateTournament` + `DisplayStats{0x13}` to T, then the PvP launch
   per pair. T5: both see the Matchups dialog, the game starts, and after the game they land back on a usable screen
   (verify the return path; fallback: re-echo Join(5)).
6. Results: EndPvP hook → per-pair result (section 5 option chosen) → points/elimination → next round
   when all pairs are done; idle and disconnect = loss; Concede 0x12A = drop. T6: a 4-player bracket finishes.
7. End: `DisplayStats{0x14 Final Standings}`, prize delivery, `SetTournament{..., -1}`, RemoveGroups(T), DB rows.

**Phase C — winner report (if option 1).** RE task for the Game win state, dinput8 report on both clients, world
agreement check. Until then run Phase B with concede/disconnect results only and no prizes for disputed games.

### 8.3 Byte templates (U() varints; 0x12B → `2B 12`, 0x125 → `25 12`, 0x124 → `24 12`, 0x1CC → `2C 1C`,
0x12A → `2A 12`, 0xFD5 → `25 FD`, 0xFA6 → `26 FA`, 0xFA8 → `28 FA`, 0xFA9 → `29 FA`, 0xFD2 → `22 FD`,
0xFD3 → `23 FD`, 0xFD1 → `21 FD`, -1 → `11`, -2 → `12`, -3 → `13`)

Queue lobby inside AddGroups (general set with N properties; ValueData int = `Header(0x16,4,1) U(2) U(1) U(v)`,
string = `Header(0x16,4,1) U(3) U(1) S(s)`; the `U(1)` is mOwningReference as in `SendAccount`; confirm against
`SendAccount`'s int/string writer before shipping):
```
Header(0x67,3,1)
  U(0) Header(0x17,1,1) U(N) [U(key) ValueData]*N      // general set
  4 x [U(0) Header(0x17,1,1) U(0)]                    // player/observer/moderator/staff sets
  U(0)                                                 // role 1 list
  U(k) U(acct_1)..U(acct_k)                            // role 2 list = queued players
  U(0) U(0) U(0)                                       // roles 3..5
  U(5) U(Q) U(5)                                       // Containing 5, GroupID Q, type 5
  U(1) U(0) U(0)                                       // deck null, round 0, substate 0
```
SetTournament: `2B 12 02 2B 12 02 2B 12 02 <U(self)> <U(tid)> <U(round)> <U(timer)> <U(T)>`.

---------------------------------------------------------------------------------------------------------------

## 9. Risks

1. **Winner detection** (section 5). Without it, prizes must stay off or concede-only.
2. **Client-hosted authority + prizes**: the host can forge results (`lon_pvp.md` 6). Keep prizes small, pick the host at
   random, refuse same-IP pairs, both-sides agreement.
3. **Post-game return path** with ctl+0x2C set (UNVERIFIED, T5).
4. **Format value 0xFA9 and deck validator**: depends on the casual spec's decode.
5. **Leave guard**: a player stuck in tournament mode (ctl+0x2C ≠ 0) cannot leave group 5/T except via Drop; world
   must always end with `SetTournament{.., -1}` or tid 0, including on server-side aborts and world restarts (a
   reconnect starts clean because the client reloads).
6. **Multi-client testing**: Phase B needs 2-8 clients on a test server. A GM "ghost" mode (one real
   client, fake opponents that concede) can exercise the flow on one PC.
7. Account restriction flags behind `FUN_100052E0(2/3/4)` were not mapped; we never send them, but if a future
   account key collides, joins fail with the US-territory dialogs. Sending 0x1057 on queues avoids the check.

## 10. Effort estimate

- Phase A: about 1-1.5 days of world code (mostly generalising the group-3 lobby code) + one-PC test.
- Phase B: about 2-3 days (tournament state machine, pairings, timers, drop/idle/disconnect, DB tables
  `lon_tournament`, `lon_tournament_entry`, prize delivery) + a multi-client test session.
- Phase C: 1-2 days RE + dinput8 + world agreement logic.
- Draft/sealed: not estimated (multi-week); not recommended.

## 11. Open design questions

1. Single elimination or Swiss? Size (4 / 8)? Queue-fills-to-start, scheduled start times, or both?
2. Prizes: how many packs for 1st/2nd/top 4, and per-win achievement only? Any prize for a bye?
3. Results: OK to add a dinput8 winner report (Phase C), or run concede-only results at first?
4. Who may create tournaments (GM command / fixed weekly schedule)?
5. Same-PC rule: reuse the one-card-game-per-PC gate (`IsActiveLooter()`, a stub you implement) for queue joins.
