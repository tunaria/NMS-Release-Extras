# Legends of Norrath Casual Games lobby (group 2): wire spec for Create / Quick Join / match room / start

Researched 2026-10-06. Static RE only: LegendsOfNorrath.dll in Ghidra (base 0x10000000) plus a capstone/pefile
pass of the same file. Read-only: nothing renamed, built, run or copied. Same conventions as
`lon_trade_lobby_spec.md` (U = varint, S = string, `Header(cls, ver, levels)`, nullable object = `U(0)` + object,
null = `U(1)`, plain object in a vector = no flag). Every claim carries its VA. UNVERIFIED = not proven statically.

Section 9 (results, achievements, profile stats) was added on the coordinator's request; its RE was done by a
helper pass and is summarised with its VAs.

---------------------------------------------------------------------------------------------------------------

## 0. TL;DR

- **Casual Games is just a second type-1 lobby (GroupID 2) built exactly like the Trade Lobby.** Add it to the
  login AddGroups, put it in UpdateLobbyStats, and answer `Join(2)` with echo + BulkJoin + announcements. The
  client then opens the same `WALobbyScreen`, which switches to the casual look for groups 2 and 9..19
  (`0x10457560`: help "casuallobby", Create, Quick Join, practice button, match list).
- **A casual game is a `Match` object (class 0x68, version 3, 2 header levels) that is a child of group 2**
  (ContainingGroupID 2, eLobbyTypeID 2). Its options live in the general PropertySet. The list columns, the
  grid tiles, Quick Join, the join dialog and the room all read the same keys (table in section 2).
- **The client creates a match by sending `LobbyCommand_AddGroups` with one Match** (Create dialog
  `0x103D71D0`, practice/quick buttons `0x10455410`, `0x10455CC0`). The creator's **full EQDeck is inside
  the Match** (deck slot, written by `0x10137A00` from key 0xFB0). World assigns the GroupID, broadcasts a
  cleaned Match, then sends the creator a **Join echo for the match**. That echo opens the match room
  (`WAMatchDialog`, `0x10453170` → factory +0x88 → `0x10466450`).
- **Joining a listed match**: the join dialog (`0x104500F0`) sends `Join{M, role 2, password}` and then
  `MatchCommand_SelectDeck{M, EQDeck}`. World answers with a Join echo to the joiner and a Join announcement
  to the others.
- **Room**: each player toggles Ready → `MatchCommand_SetReady{M, bool}`; world relays it. When both are ready
  and both decks are known, world calls the **existing StartPvP with M as the match/game id**. LaunchGame's UI
  event 0x11 closes the room without sending Leave (`0x10454210` case 0x11 → `0x10464950(0)`).
- **Trade Lobby right-click "Challenge" cannot be made to work by a server**: the item is force-disabled
  (`0x104CB800`) **and** has no handler in `handleContextMenuTriggered` (`0x104CC130`). The casual route makes
  it unnecessary. "Join hosted game" on the same menu already works in the casual lobby once matches exist.
- **Results**: a normal win sends nothing that names the winner (section 9). Phase 1 can only record concede
  and disconnect. Credit needs the phase-2 two-sided DLL report.

---------------------------------------------------------------------------------------------------------------

## 1. Lobby group 2 (Casual Games): what the navigator needs

### 1.1 Navigator click

- "Casual Games" = `0x10473900` → `0x10471A50(2, 1)` (also `0x10461690`).
- `0x10471A50` with param_3 = 1:
  - It counts entries of the population map (ctl+0x68, `0x10068280`) whose key passes `0x10068260`
    (key == 2 or 9 ≤ key ≤ 19).
  - **If more than one** such lobby exists, it opens `lobbyChooserDialog` (sub-lobby picker).
  - Otherwise it falls through to the "Lobby Full" check (max ≤ current → dialog) and then `0x1046FB20(2)`,
    which sends `Join{2, role 1}` (same writer as the Trade Lobby).
- So: **never put groups 9..19 in UpdateLobbyStats**. Group 2 alone → no chooser, direct Join(2).

### 1.2 Server messages (exact bytes)

**AddGroups at login** (extend `SendLobbyGroups`). One AddGroups with a vector of 2 lobbies is legal
(`0x1013A510`: count + plain objects). Note: a count > 1 makes the execute post events 0x39/0x3A around the
batch (`0x101399D0`), harmless. Sending two separate AddGroups is equally fine.
```
Header(0x5E,1,3) U(self) U(2)
  Header(0x67,3,1) [5 x U(0) Header(0x17,1,1) U(0)] U(0)x5  U(0) U(3) U(1) U(1) U(0) U(0)   // Trade Lobby (unchanged)
  Header(0x67,3,1) [5 x U(0) Header(0x17,1,1) U(0)] U(0)x5  U(0) U(2) U(1) U(1) U(0) U(0)   // Casual Games
```
Group 2 bytes: `27 06 03 [00 27 01 01 00]x5 00 00 00 00 00 00 02 01 01 00 00`.
- Type **1** (as group 3): Join auto-sets status 2 (`Lobby::AddMember` `0x101376B0`). The Join echo goes
  through `0x10453170` → type 1 → `0x1045FE90(3, 2, 1)` → screen case 3 → `WALobbyScreen` + `setGroup(2)`.
- Containing 0 (screen case 3 special-cases Containing 8).
- Do not add group 1 (it auto-sends Join(1), `0x10452E00`).

**UpdateLobbyStats** (extend `SendLobbyStats`): map 1 and map 4 get two entries.
```
Header(0x19C,3,3) U(self)  U(2) U(2) U(casualPop) U(3) U(tradePop)  U(0) U(0)  U(1000)  U(2) U(2) U(1000) U(3) U(1000)
```
Maps are written as count + (key, value) pairs; std::map, so order does not matter, but keep keys ascending.

**Join(2) / BulkJoin / announce / Leave / ChangeStatus / SendText**: byte-identical to the Trade Lobby
messages in `lon_trade_lobby_spec.md` sections 3-5 with GroupID 2 instead of 3. Generalise the existing
handlers by group (one roster per group).

### 1.3 Lobby switching (important, new)

`0x10156B60` (enter lobby), type-1 branch: on entering a type-1 lobby the client **sends `Leave(g)` for every
other joined lobby** except group 1, group 8 (special), its own children, and itself.
- Trade → Casual: the client sends `Join(2)`, gets the echo, then sends **`Leave(3)`**.
- Casual → Trade: `Join(3)` then **`Leave(2)`**.
- Entering a lobby also Leaves any joined Match that is not a child of the new lobby (so a player still
  sitting in a match room who clicks Trade Lobby sends `Leave(M)`).

`HandleLobbyLeave` today handles group 3 and the PvP M only. It must handle group 2 and match groups (sections
3.5, 4.4). Track lobby membership per session per group (a session is in at most one of 2/3 at a time, plus
at most one match).

### 1.4 What the casual screen shows (`0x10457560`, groups 2/9..19)

- "Casual Lobby Help Files" (`casuallobby`), posted-trades bar hidden.
- `WALobbyGroupModel` (`0x10452BD0`, vtable `0x1068C544`) over the **children of group 2** (refresh
  `0x10452670`: rows = registry groups whose ContainingGroupID == 2, row getter `0x104523F0`); live updates
  through `addLobbyGroup(int,int,int)` / `removeLobbyGroup(int,int)` signals (AddGroups event 3, RemoveGroups
  event 4).
- A table view and a graphics view in `matchArea` (the graphics view is made current: tiles
  `WAMatchGraphicsItem`, `0x102C6470`). Sort pref `prefs/lobby/matchsort`, "Match ID" column, "Matches: %3".
- `quickJoinButton` → `handleQuickJoinButtonClicked()`, `createButton` → `handleCreateButtonClicked()`.
- Unless the account flags (0xFC5) contain "ShowShadowOdyssey": a `practiceButton`
  ("Create a practice match with another player using a randomly chosen Fighter or Mage starter deck.") →
  `handlePracticeButtonClicked()`. With the flag: scenario buttons instead (single-player paths).

---------------------------------------------------------------------------------------------------------------

## 2. The Match object (class 0x68)

### 2.1 Wire layout (read `0x1015F390`, write `0x1015F3E0`)

- Vtables: serializer `0x10619294` (full object +8), primary `0x106192D4`, +0xC `0x10619280`.
  GetClassID `0x1015F200` = 0x68, version `0x1015F250` = **3**.
- Read: `vtable+0x10` header, then the **Lobby read `0x10137830`**, which reads the header a second time
  (**2 levels**), then `U(GameID)` into serializer +0x70 (= full +0x78), then vtable+0x14 (end, no data).
- The write (`0x1015F3E0` → `0x10137A00`) is the mirror image. The text labels ("Starting Match", ...) are
  no-ops in binary streams.

```
Header(0x68,3,2)
  5 x nullable PropertySet (role 1 general, 2 player, 3 observer, 4 moderator, 5 staff)
  5 x int list (members per role, keys 1..5)
  U(ContainingGroupID)   = 2
  U(GroupID)             = M
  U(eLobbyTypeID)        = 2      (Match ctor 0x1015F1C0 sets +0x24 = 2)
  nullable EQDeck        (v>1)  -> world always sends U(1) (null)
  U(tournamentRound) U(subState) (v>2) = 0 0
  U(GameID)              = 0 (or M; not read by any casual UI path found)
```
Class bytes: 0x68 → `28 06`, so the header is `28 06 03 28 06 03`.

Full-object fields not on the wire: +0x7C "started" byte (`0x1015F300` getter, `0x1015F2D0` setter: true →
also sets state 4 if 0), +0x80 match state (`0x1015F280`/`0x1015F290`).

### 2.2 Roles

Role keys: 1 general, 2 **player**, 3 observer, 4 moderator, 5 staff (`0x10137830` reads keys 1..5; join
dialog uses role 2 to play, 3 to observe `0x104500F0`; Quick Join uses role 2 `0x1045582F`).
- "Cur #" (0xFB5) = number of members (translator `0x10459B70`), the canJoin "full" test counts **role 2**
  (`0x10132290`: `FUN_10131910(.., 2)` vs 0xFA8).
- Host shown in the list = **first role-2 member** (virtual key 0x104F/0xFB8 in `0x10131BB0`).
- So the Match world sends lists the players in role 2, **host first**.

### 2.3 Property keys (general PropertySet, `Lobby::getProperty` `0x10131BB0`)

ValueData types (`lon_achievements.md` 6): 1 bool, 2 int, 3 string, 6 int list, 7 string list. A value is
`U(key) Header(0x16,4,1) U(type) U(1) <value>`.

**Stored keys** (written by the Create dialog setup `0x103D80E0`; Quick/practice `0x10455903..0x10455BBE`):

| Key | Type | Meaning | Evidence | World sends |
|---|---|---|---|---|
| 0xFA5 | **6 (int list)** | option flags: 1 = Friends only, 2 = Password, 3 = Observers allowed, 4 = Guild only | `0x10131160` + switch in `0x10131BB0` (0xFAE→1, 0xFBA→2, 0xFAC→3, 0xFAF→4); canJoin `0x10132290`; practice sends `{3}` (`0x10455B8A`) | `{}` or `{2}` if password. **Must be type 6 or absent: any other type asserts `mTypeID == kIntegerListTypeID` (ValueData.h:0x146) in `0x10131160`** |
| 0xFA6 | 3 | title (default "#owns([*actor*]) Match") | `0x103D80E0`; tile truncates to 15 chars `0x102C6470` | sanitized title, ≤ 40 chars |
| 0xFA7 | 2 | GameType: 1 Constructed, 2 Sealed, 3 Booster Draft, 4 Other | translator `0x10459790` | 1 |
| 0xFA8 | 2 | NumberPlayers (2 / 4) | `0x102C1060`, canJoin `0x10132290` | 2 |
| 0xFA9 | 2 | PlayFormat: 1 Standard, 0x11174 Practice, 0x11183 Open, ... | EQ translator `0x102C1A80` | 1, 0x11174 or 0x11183 (clamp) |
| 0xFAA | 2 | MatchStructure: 1 single, 2 best of 3, 3 best of 5 | `0x10459890` | **1** (multi-game not supported) |
| 0x273 | 2 | Duration minutes (0, 30..90 "chess clock") | `0x102C1690` | **0** (Duration > 0 turns on turn timers; LossType-2 crash notes in `lon_pvp_host_black_screen.md` 7) |
| 0xFAD | 3 | password | `0x103D80E0`, joined via Join.Password `0x104500F0` | **never broadcast** (strip; keep server-side) |
| 0xFB0 | 3 | deck name chosen in the dialog | `0x103D80E0`; Lobby write turns it into the deck object `0x10137A00` | **strip** (private) |
| 0xFAC / 0xFAE / 0xFB1 | 1 | Observers / BuddiesOnly / ? flags as sent by the client | practice `0x10455AEC..0x10455B3E` | **strip**; the UI reads 0xFAC/0xFAE through 0xFA5 anyway |
| 0x1013 | 2 | guildID (guild-only) | `0x103D80E0` | strip (a non-zero value triggers a guild lookup `0x1011DDD0` in the room config `0x10464130`) |
| 0xFBC | 2 | match state at add time: ≥ 4 marks the match started | AddGroups execute `0x101399D0` | optional: 4 for a match already in play |
| 0xFBD + 0xFC1 | 6 + 7 | account ids + names to introduce | `0x101399D0` (assert "account" :0x8C only if get-or-create fails) | never |
| 0x1052 | ? | if present the match is not joinable from the user list | `0x104CB800` (`vt+0x40(0x1052)`) | never |

**Virtual keys** (computed by `0x10131BB0`, never send them): 0xFB2 (ID: GroupID; 0xFD5/0xFE0 for types
5/9/10), 0xFB3 (status: started byte), 0xFB4 (= 0xFA9), 0xFB5/0xFDA (member count), 0xFB6 (= 0xFA8),
0xFB7 (= 0xFA7), 0xFB8/0x104F (host name), 0xFB9 (= 0x273 or 0xFB1), 0xFBA (password flag), 0xFBB (= 0xFAA),
0xFBE (type), 0xFBF (lower-case title).

Restriction keys read by canJoin `0x10132290` (never send): 0xFDC (string list of account classes), 0xFDB
(invited ids), 0xFDE/0xFDD/0xFDF (rating gate; rating keys 0xFC8-0xFCB).

### 2.4 What the list shows

Table columns (`0x10451DE0`, model `0x10452740`, translator `0x10459B70` → EQ `0x102BFF80`):

| # | Header | Key | Source |
|---|---|---|---|
| 0 | ID | 0xFB2 | GroupID |
| 1 | Status | 0xFB3 | started byte (+0x7C) |
| 2 | Title | 0xFA6 | |
| 3 | Play Format | 0xFB4→0xFA9 | "Standard", "Practice", "Open"... (`0x102C1A80`) |
| 4 | Cur # | 0xFB5 | member count |
| 5 | Max # | 0xFB6→0xFA8 | |
| 6 | (game type) | 0xFB7→0xFA7 | "Constructed" |
| 7 | Observers | 0xFAC (FA5∋3) | |
| 8 | (host) | 0x104F | role-2 member [0] name (cached account) |
| 9 | (time) | 0xFB9→0x273 | "No Time Limit" |
| 10 | Password | 0xFBA (FA5∋2) | |
| 11 | Friend | 0xFAE (FA5∋1) | |
| 12 | Guild | 0xFAF (FA5∋4) | |

Grid tile (`0x102C6470`): "#<GroupID>", title (type-3 check), play format text, icons buddy/guild/password/
timed from 0xFA5, buttons join/info/observe. All reads are type-checked (no assert found).

### 2.5 Recommended Match as world sends it

```
Header(0x68,3,2)
  U(0) Header(0x17,1,1) U(n)                       // general set
       U(0xFA5) Header(0x16,4,1) U(6) U(1) U(0)              // flags {} (or U(1) U(2) for password)
       U(0xFA6) Header(0x16,4,1) U(3) U(1) S(title)
       U(0xFA7) Header(0x16,4,1) U(2) U(1) U(1)
       U(0xFA8) Header(0x16,4,1) U(2) U(1) U(2)
       U(0xFA9) Header(0x16,4,1) U(2) U(1) U(format)
       U(0xFAA) Header(0x16,4,1) U(2) U(1) U(1)
       U(0x273) Header(0x16,4,1) U(2) U(1) U(0)
  4 x U(0) Header(0x17,1,1) U(0)                   // roles 2-5 sets: present, empty
  U(0)                                             // role 1 members
  U(k) U(host) [U(guest)]                          // role 2 members, host first
  U(0) U(0) U(0)                                   // roles 3-5
  U(2) U(M) U(2)                                   // Containing, GroupID, type
  U(1)                                             // deck: null
  U(0) U(0)                                        // round, subState
  U(0)                                             // GameID
```
Key bytes: 0xFA5 `25 FA`, 0xFA6 `26 FA`, 0xFA7 `27 FA`, 0xFA8 `28 FA`, 0xFA9 `29 FA`, 0xFAA `2A FA`,
0x273 `23 27`, 0x11174 `44 17 11`, 0x11183 `43 18 11`.

Inside an AddGroups (no flag in front, `FUN_10177300`):
`Header(0x5E,1,3) U(recipient) U(1) <Match>`.

---------------------------------------------------------------------------------------------------------------

## 3. Create, Quick Join, practice, joining a listed match, leaving

### 3.1 What each client button sends

| UI | Function | Sends |
|---|---|---|
| Create → OK | `0x103D71D0` (WACreateMatchDialog vtable slot, dialog ctor `0x103D6C20`, form `:/forms/dialogs/creatematch.ui`) | `AddGroups{self, [Match]}`. Match: `0x1015F1C0` ctor, Containing = the dialog's lobby (`0x1012D780(param_1[0x20])`), general set = the dialog's set (`0x1012DBC0(1, set)`), **deck = the EQDeck named by 0xFB0** (Lobby write `0x10137A00` looks it up in the local DeckDB `0x100A89F0`). For raid formats 0x11176/0x11184 it bumps 0xFA8 to 2/4. Send `0x10068380` |
| practice button (`handlePracticeButtonClicked`, UNVERIFIED name↔function: the body creates PlayFormat 0x11174 = "Practice") | `0x10455410` | Looks for an open child Match with GameType 1, Max 2, format 0x11174, structure 1, not started, not password, canJoin ok (`0x10455540..0x104557F5`). **Found:** `Join{M, role 2}` (`0x10455803..0x10455849`), **no deck**. **None:** `AddGroups` with {0xFA7:1, 0xFA8:2, 0xFA9:0x11174, 0xFAA:1, 0x273:0, 0xFA6:"<name>'s ..." (`walobbyscreen.cpp.1`), 0xFAC:true, 0xFAE:false, 0xFB1:false, 0xFAD:"", 0xFA5:{3}}, **deck null** |
| Quick Join | `0x10455CC0` | Needs saved prefs (`/prefs/quickjoin/*` + `match/quickjoin/deck/create`); else shows "One or more of your quick join settings are not valid" + the settings dialog. Finds a matching Match → `Join{M,2}` (`0x10456B1D`) + `MatchCommand_SelectDeck{M, deck}` (`0x10456BFC..0x10456C41`); none → `AddGroups` (`0x10457275`) |
| Click a match (list/tile join button) and user list "Join hosted game" | factory +0x9C → `WAJoinMatchDialog` (`0x1044F8F0`, vtable `0x1068B7C4`, setup `0x1044FA70`), accept `0x104500F0` | `Join{AccountID self, GroupID M, Role 2 (3 if its observe box), Password (0xFAD)}`, then (unless 0xFD9 rejoin) `MatchCommand_SelectDeck{M, deck}`; closes itself. Client-side deck checks: deck validator for the format; archetype check `deck->vt+0x78` → `0x100A3F10()->vt+0x58` ("Your deck's archetype is not allowed...") |
| Room close / Cancel | WAMatchDialog dtor `0x104662E0..` → `0x10464A00` | if +0x50: `Leave(M)` (`0x10156950`) then `ChangeStatus 2` (or 8 in a tournament) |

### 3.2 Client → world wire

**AddGroups (create)** — read `0x1013A510`:
```
Header(0x5E,1,3) U(acct) U(1)
  Header(0x68,3,2)
    PS role 1: U(0) Header(0x17,1,1) U(n) {U(key) ValueData}xn
    PS roles 2-5: U(1) x4                         // null (ctor; UNVERIFIED all four)
    role lists: U(0) x5                           // UNVERIFIED: ctor leaves them empty
    U(containing=2) U(groupId: client default) U(type=2)
    deck: U(0) <EQDeck: Header(0x11172,10,2) ...>  or U(1)
    U(0) U(0) U(GameID 0)
```
World must parse the PropertySet with a generic ValueData reader (types 1, 2, 3, 6, 7; see `DecodeProps`
in lon_service.cpp:345 for the existing pattern) and **reject anything else**. Treat the deck bytes as the
same EQDeck format as `AddOnlineDeck`/`lon_deck` (already parsed by `DeckName`/`DeckCards`). The 3 trailing
varints follow the deck; the safest extraction is: deck = the remaining bytes minus the trailing
`00 00 00` (refuse the create if the tail is not `00 00 00`). Fallback when the deck is null but 0xFB0 is
set: look the name up in the account's `lon_deck` rows (DeckDB lookup is by that string; whether it is the
name or the key is UNVERIFIED, so compare both).

**Join (match)** — read `0x10145C40` (v4): `Header(0x5C,4,3) U(acct) U(M) U(role) S(password) U(intlist n)…
S(name) <nullable PS> U(gender)`. Use group, role, password.

**MatchCommand_SelectDeck 0x65 v2, 4 levels** — read `0x10160980`:
`Header(0x65,2,4) U(acct) U(M) S(DeckID) U(DeckIDOnly) <nullable EQDeck>`. Writer sets DeckIDOnly 0 and the
deck's string id (`0x10160670`). The client's execute is `return 1` (`0x101608D0`): **never echo it**.

**MatchCommand_SetReady 0x63 v1, 4 levels** — read `0x10162520`: `Header(0x63,1,4) U(acct) U(M) U(ready)`.

**Leave(M)** — `Header(0x5D,3,3) U(acct) U(M) U(0) U(0)`.

### 3.3 World's answer to a create

Validate: rule flag on, sender in group 2 (status 2), not in a match/trade/PvP, at most one open match per
account, deck present (or resolvable) and passes `DeckProblem` (owned, ≥ 50 cards), format clamped, structure
1, players 2, duration 0. Practice (0x11174) with no deck: world picks the deck (owned starter deck, or a
random Fighter/Mage starter deck as the tooltip promises; design choice).

Then, in this order (each step needs the previous one on the client):
1. Assign `M = kPvPMatchBase + n` (never 0, 2, 3, 9..19, 1000000+S). Store `CasualMatch{M, host, title,
   format, password, host_deck, guest, guest_deck, ready[2], state}`.
2. `AddGroups{recipient, [Match M with role 2 = {host}]}` to **every session that has seen group 2's
   lobby** (simplest: every logged-in session; keep a per-session `known_matches` set so RemoveGroups goes only
   where AddGroups went). Duplicate AddGroups for the same id is ignored by the client but leaks an object
   (`0x101539C0` insert) — never re-send to a session that already knows M.
3. To the creator: **Join echo** `Header(0x5C,4,3) U(host) U(M) U(2) S("") U(0) S("") U(0) Header(0x17,1,1)
   U(0) U(0)`.
   - Execute `0x10145340`: group found, self → current lobby = M (`0x10068560`), AddMember(host, 2), event 5.
   - Event 5 → `0x10453170` self branch: type 2, `0x10153990` Match cast ok, not started, controller+0xBC
     (challenge flag) clear, a screen is showing → builds `matchDialog` (factory +0x88 `0x10477710` →
     `WAMatchDialog` `0x10466450`, form `:/forms/dialogs/match.ui`), sets its match id (`0x10464930`), shows
     it, emits addParticipant for every other member and then for self, sends **ChangeStatus 4**
     (`0x10150B60(4)`), enters the lobby (`0x10156B60(M)`, type-2 branch is harmless).
4. To everyone else in group 2: nothing more (the host is already in the Match's role-2 list).

If the create is refused: send nothing to the client (the create dialog's dtor clears the busy flag,
`0x103D6D1F`) and explain in EQ chat (`TellCharacter`, as `PvPTell`). UNVERIFIED: whether the client shows a
wait state; test it.

### 3.4 World's answer to a join (list click, Quick Join match found, practice match found)

Validate: M open and not started, role 2 (refuse observers: role 3), 1 player inside, not the same account,
password equal when set, guest free (group 2, status 2, no trade/match), not on the host's ignore list.
Then:
1. To the joiner: **Join echo** (as 3.3 step 3 with the joiner's id) → the joiner's room opens and shows the
   host (already a role-2 member) and itself.
2. To the host: **Join announcement** `Header(0x5C,4,3) U(guest) U(M) U(2) S("") U(0) S(guest_name) U(0)
   <PS with 0xFC5 "RegisteredUser"> U(0)` (same as the trade announce). Execute: AddMember, event 5 →
   `0x10453170` other-account branch → `0x104DF5A0(guest, M)` → `addParticipant` → host room adds the
   guest's widget (`0x10466540`).
3. To every other group-2 session: the same announcement (keeps Cur # right). UNVERIFIED: whether the table
   repaints on member change; harmless if not.
4. Deck: take it from the `SelectDeck` that follows (validate with `DeckProblem`). Practice joins send no
   SelectDeck: world picks it. Quick Join sends SelectDeck. Until a guest deck is known the guest cannot be
   ready (ignore SetReady 1 and tell them).

Refusal: send nothing; tell in EQ chat. (The join dialog already closed itself, `0x10450710`.)

### 3.5 Leaving / cancelling before the start

- **Guest Leave(M)** (Cancel, close, or `Leave(M)` from a lobby switch, or disconnect): world removes it,
  then sends `Leave{guest, M}` (`Header(0x5D,3,3) U(guest) U(M) U(0) U(0)`) to the host and to the other
  group-2 sessions. Host room: `removeParticipant` handler `0x10465D70` removes the widget (closes only when
  the removed account is self, the Match is gone, or a tournament case). Clear both ready flags.
- **Host Leave(M)** (or disconnect): close the match:
  1. to the guest (if any): `Leave{host, M}`, then `Leave{guest, M}` → the guest's room closes
     (`0x10465D70`: removed account == self → close; its dtor sends Leave(M), which world ignores for an
     unknown M);
  2. `RemoveGroups{M}` to every session that knows M: `Header(0x5F,1,3) U(recipient) U(1) U(M)` (bytes
     `2F 05 01` ×3). Execute `0x10148040`: event 4 to the list, resets the current lobby to the parent (2)
     if it was M, deletes the object. Rooms that still look it up close themselves
     (`0x10466540`/`0x10465D70`: group missing → close).
- Timeout an open match with nobody joining after e.g. 15 minutes (RemoveGroups).

---------------------------------------------------------------------------------------------------------------

## 4. The match room and the start

### 4.1 Room widgets (`WAMatchDialog`, vtable `0x1068F394`)

- Setup `0x10464F30`: `playerListScrollArea`, `matchConfiguration` (`0x10464130`, disabled copies of the
  create buttons filled from the Match keys), chat focus. **It calls `0x10153990(M)` and connects
  `receiveReady(int,bool)` / `receiveSeatSelection(int,int)` on it, then dereferences it with no null check**
  (`0x10464F30`: `(pQVar2+0xC)->vt+4(0xFA9)`). Reached only after `0x10453170` found the Match, so the rule
  is: **the Match must exist on the client before the Join echo** (3.3 step 2 before step 3).
- Slots (metacall `0x104E06A0`): 0 handleAddParticipant `0x10466540`, 1 handleRemoveParticipant
  `0x10465D70`, 2 handleReceiveReady `0x10465E70`, 3 handleReceiveSeatSelection `0x10465EF0`.
- Player widget per role-2 member (`0x104680B0`, fill `0x104688D0`): avatar (`0x1039D3F0`, safe), account
  name from the account cache, **"Ready to go:" checkbox enabled only on your own widget**
  (`handleReadyButtonToggled(bool)`), a position drop-down only for raid formats (0x11176/0x11177/0x11184)
  or 4 players. Observers (role 3) get no widget.
- There is **no Start button**: the game starts when the server launches it.

### 4.2 Messages in the room

| Dir | Message | Notes |
|---|---|---|
| C→S | `SetReady{M, 0}` when the own widget is added | `0x10466540` loop calls `0x104682E0` for the own widget: sends SetReady(!checked); the widget ends `setChecked(true)` (`0x104688D0`), so 0. UNVERIFIED reading of the toggle semantics; world should simply store the last value |
| C→S | `SetReady{M, 1/0}` from the checkbox | |
| S→C | `SetReady{player, M, ready}` to the **other** member | execute `0x10162300`: event 0x7B and `receiveReady(acct, ready)` (`0x104D28C0`) → `0x10465E70` updates that widget; it ignores the own account, so echoing to the sender is useless |
| C→S | `SelectDeck` (joiner, right after Join) | store; never echo |
| C→S | `ChangeStatus 4` (room) / 2 (back to lobby) / 5 (game) | store; relay to group 2 like the trade lobby |
| S→C | optional `SetTimer{M, seconds}` 0x127 v1, 4 levels: `Header(0x127,1,4) U(acct) U(M) U(duration)` | execute `0x10162750`: event 0x49 + controller timer `0x10068320`. Not needed |
| S→C | `SetDeckArchetype` 0x1D6 | client execute is `return 1` (`0x10161D30`): useless |

Optional chat in the room: SendText with GroupID M (not traced; leave it out of phase 1).

### 4.3 Start (both ready, both decks known)

1. Mark the match started for everyone else in group 2: **`SetMatchState{M, 4}`**
   `Header(0x7C,1,4) U(recipient) U(M) U(4)` (bytes `2C 07 01` ×4 ...). Execute `0x10161EB0`: event 0x7E,
   `setStarted(state > 3)` (`0x1015F2D0`), state = 4 (`0x1015F280`/`0x1015F350`). The list then shows the
   started status and canJoin refuses ("The match has already started.", `0x10132290`). UNVERIFIED: the
   meaning of states 0..3; only "> 3 = started" is proven.
2. **`StartPvP(host, &guest, ...)` with the match id = M** (today `m.id = kPvPMatchBase + g_next_match++`;
   pass M in instead). Host = random of the two (spec section 2.2 of `lon_pvp_build_spec.md`), seat 1 gets
   `'NMSH'`. Order unchanged: guest LaunchGame + SetupGame(2), host LaunchGame + SetupGame(1) +
   SelectDeckForPlayer(1) + (2).
3. On each client, LaunchGame's UI event 0x11 (`0x10454210` case 0x11) finds the `matchDialog`, calls
   `0x10464950(0)` (clears +0x50, so the dtor sends **no** Leave(M)), closes it, opens the game screen
   (`0x1045FE90(0xB, M, 0)`), resets the controller timer. The client sends ChangeStatus 5.

How live did it: the live server owned the master Game; its match layer sent LaunchGame and then the
authority's SetupGame/SetPlayer/SelectDeckForPlayer stream (`lon_pvp.md` 2.5). Our mapping is exactly the
existing StartPvP: world plays the match layer, the host's client plays the master.

### 4.4 After the game

- Game screen exit sends `Leave(M)` (`0x10156950`) and then the client returns to the lobby (Join(2)/Join(3)
  from the navigator or ChangeStatus). Keep the existing `PvPLeftScreen` logic and extend it to Join(2).
- When the PvP match is closed (`ClosePvP`), send `RemoveGroups{M}` to every session that knows M.
- `Leave(M)` for a match that is already in play is the game-screen exit, not a room cancel: route it to
  `PvPLeftScreen` (as now) when `pvp_match == M`.

---------------------------------------------------------------------------------------------------------------

## 5. Trade Lobby right-click Challenge

- Menu build `0x104CAEE0`: +0x38 "Challenge", +0x3C "Join hosted game", +0x40 "Observe match", +0x44
  "Trade", +0x48/0x4C friends, +0x50/0x54 ignore, +0x58/0x5C guild, +0x60 "Get info", +0x64 "(User not
  logged in)".
- `0x104CB800`: for any other user, `QAction::setEnabled(+0x38, false)` unconditionally.
- **`handleContextMenuTriggered` `0x104CC130` has no branch for +0x38.** Enabling the action with a patch
  would show a clickable item that does nothing. A working Challenge needs a new DLL handler (for example: send
  a custom frame to world), i.e. new client code, plus all of the match machinery above.
- The server-side challenge path exists independently: `MatchCommand_InitiateChallenge` 0x1E6 v2 (4 levels,
  read `0x10160010`: `U(acct) U(M) U(a) U(b) U(c)`) is never built by the client (ctor `0x101600D0` has no
  code callers); its execute `0x1015FE20` stores a, b, c in a challenge state and posts UI event 0xC1. The
  Join-to-Match handler `0x10453170` opens `WAChallengeDialog` (`0x103BA4C0`, which sends SelectDeck +
  SetReady, `0x103BA800`) instead of the room when controller+0xBC is set. UNVERIFIED who sets +0xBC
  (probably the 0xC1 handler).
- **Recommendation: drop Challenge.** "Join hosted game" (`0x104CBD60` → `WAJoinMatchDialog`) already works
  from the casual lobby's user list once a user is in an open Match; it needs nothing extra. In the Trade
  Lobby the menu only looks at children of group 3 (`0x104CB5E0` → `FUN_10135170(2,..)` on the user list's
  group), so never put matches under group 3.

---------------------------------------------------------------------------------------------------------------

## 6. Asserts, crashes and jams on these paths

| Trigger | Where | Avoided by |
|---|---|---|
| 0xFA5 present with a type other than 6 | `0x10131160` assert `mTypeID == kIntegerListTypeID` (ValueData.h:0x146), reached from the list, canJoin, room | build 0xFA5 yourself as type 6 |
| A type-2 child of a lobby that is not a Match (class 0x67 with type 2) | `0x104CB5E0`: Match cast then members call, no null check → crash on user-list right-click | only class 0x68 for type 2 |
| Join echo for M before the client has Match M | not a crash: `0x10453170` returns early (group missing) — no room opens | AddGroups first |
| Match object deleted while the room is open | room handlers re-look-up and close; Qt disconnects the signals | RemoveGroups after the Leaves (3.5) |
| RemoveGroups of the current lobby with no parent | `0x10148040` → `0x10068560(0)` (current-lobby setter; asserts lobby≠0 per trade spec, UNVERIFIED for 0) | only remove matches (parent 2) |
| AddGroups 0xFBD/0xFC1 | assert "account" (LobbyCommandAddGroups.cpp:0x8C) only if get-or-create fails | never send |
| Role 0 in AddMember | Lobby.cpp:0x255 | role lists keys 1..5 only |
| Join with a name and no PropertySet | LobbyCommandJoin.cpp:0x5E | announce with PS (existing writer) |
| Wrong class/version/levels or short field | read fails → **the LoN stream jams for good** (`0x101740E0`, build spec 4.1) | Match = `(0x68,3)`×2; MatchCommand_* = 4 levels; RemoveGroups/AddGroups/Join/Leave = 3 levels |
| Duration > 0 / LossType 2 | timer text crash (`lon_pvp_host_black_screen.md` 7) | force 0x273 = 0, SetupGame Duration 0 |
| Guild id non-zero | room config guild lookup `0x1011DDD0` (not traced) | strip 0x1013 and FA5∋4 |
| Duplicate AddGroups for a known id | no crash; object leak (`0x101539C0`) | per-session known set |
| Client-only asserts (not server-driven) | join dialog "deck" asserts if its deck name is not in the local DeckDB (`wajoinmatchdialog.cpp` 0xC2/0xF9); SelectDeck ctor asserts deck≠0 (`0x10160670`); EventScreen `0x10401B40` | none for world |
| Existing PvP asserts (GameID 0, seat 0, decks before SetupGame...) | `lon_pvp_build_spec.md` 4.2 | unchanged StartPvP |

---------------------------------------------------------------------------------------------------------------

## 7. Build plan (ordered)

### Step 1: Casual lobby room (no matches yet)
- Generalise the Trade Lobby code by group: `Session::lobby_group` (0/2/3) instead of `in_trade_lobby`,
  `LobbyMembers(group, except)`, `SendLobbyStats` with both groups, `SendLobbyGroups` adds group 2.
- `HandleLobbyJoin`: group 2 → same as group 3 with group 2 (echo, BulkJoin, announce, stats); keep the
  trade-specific steps (TradePartyLeft) for group 3 only. Join(2) while in 3: the client follows with
  Leave(3), handle it.
- `HandleLobbyLeave`: group 2 → leave casual lobby (announce `Leave{acct,2}` to group-2 members, stats).
- ChangeStatus and SendText relay to the session's current group.
- **Test (one client):** Casual Games opens the casual screen, the user list shows you, chat echoes,
  switching Trade ↔ Casual moves you between rosters, no chooser dialog.

### Step 2: Matches without games
- Parse AddGroups (C→S) with one Match (generic PropertySet reader + deck extraction), validate, build the
  clean Match (2.5), broadcast, Join echo (3.3).
- Join(M) → 3.4; SelectDeck → store/validate; Leave(M), disconnect → 3.5; RemoveGroups on close.
- At login (after group AddGroups) send AddGroups of all open matches to the new session.
- **Test (one client):** Create opens the room with yourself; the list/grid shows ID, title, format,
  1/2 players, your name; Cancel removes it; a second create after cancel works; Quick Join with no prefs
  shows the settings notice; practice button creates a practice match.

### Step 3: Room and start
- SetReady relay; start rule (both ready, both decks) → SetMatchState 4 → `StartPvP` with M.
- `StartPvP(..., uint32_t match_id)`; `ClosePvP` sends RemoveGroups.
- **Test (test server, two clients):** create/join/ready/start, concede, disconnect in the room, disconnect in the
  game, host cancels with a guest inside, guest cancels, password match, practice quick match, second match
  right after the first, a third player trying to join a started/full match.

### Step 4: Remove the chat commands
- Remove `!challenge`, `!accept`, `!decline`, `!decks` (HandlePvPCommand) and the `Challenge` list. Keep
  `!pvptest` (GM, ghost-guest test) until the casual route is proven. Keep the rule flag
  `Custom:LoNPvPEnabled` gating Create/Join (refusal = no reply + EQ chat line).

### Writers to add (byte layouts)

```
AddGroupsMatch(recipient, m)   Header(0x5E,1,3) U(recipient) U(1) <Match 2.5>
JoinMatchEcho(acct, M)         Header(0x5C,4,3) U(acct) U(M) U(2) S("") U(0) S("") U(0) Header(0x17,1,1) U(0) U(0)
JoinMatchAnnounce(acct, M)     Header(0x5C,4,3) U(acct) U(M) U(2) S("") U(0) S(name) U(0) WriteMemberProps U(0)
LeaveGroup(acct, g)            Header(0x5D,3,3) U(acct) U(g) U(0) U(0)
RemoveGroups(recipient, M)     Header(0x5F,1,3) U(recipient) U(1) U(M)
SetReadyRelay(acct, M, r)      Header(0x63,1,4) U(acct) U(M) U(r)
SetMatchState(recipient, M, s) Header(0x7C,1,4) U(recipient) U(M) U(s)
(optional) SetTimer            Header(0x127,1,4) U(recipient) U(M) U(seconds)
```

---------------------------------------------------------------------------------------------------------------

## 8. Open questions (live test settles them)

1. Practice/quick-join function names: `0x10455410` builds practice (0x11174) matches and `0x10455CC0` reads
   `/prefs/quickjoin/*`; which button maps to which slot was not read from the metacall.
2. Whether the client's create Match carries null PS for roles 2-5 and empty role lists (world rebuilds the
   Match anyway, so only the parser cares: accept both).
3. Whether 0xFB0 is the deck name or the deck key in DeckDB (only matters for the fallback when the deck
   object is null).
4. SetReady toggle semantics on the own widget (store the last value; require 1 from both).
5. Match states 0..3 (only > 3 = started is proven).
6. Whether the list repaints Cur # on Join/Leave announcements (cosmetic).
7. What the client shows after a refused create/join (no reply). If it hangs in a wait state, answer with
   `LobbyCommand_GameServiceError 0x8A` (not traced here) or design a different refusal.

---------------------------------------------------------------------------------------------------------------

## 9. Results, achievements and profile stats (coordinator's extra scope)

### 9.1 How the game ends, and what world can see

Loss types (end-text builder `0x102657B0`): **0x11175** avatar killed, **0x11176** out of cards, **0x11177**
four quests, **1/3/4** concede (4 also sub-game concede), **2** timed loss. Sources: 0x11175 from the rules
(`0x101EA147`, `0x1023701B` → `vt+0xAC(p, 0x11175)`); 0x11177 set by gameWonByPlayer `0x10269AC0`; the 0x11176
caller was not found (UNVERIFIED).

`gameLostByPlayer` `0x1026F8B0` → end routine `0x1026ED90` (result map 1/-1) → text `0x102657B0` → end dialog
`0x10264900`, which sets `+0x1C4 = 1`. **No network command is sent on this path**, and after `+0x1C4` the
send helper `0x100F1680` refuses new commands.

| Command | Read | Fields after the GameCommand base | Notes |
|---|---|---|---|
| ConcedeGame 0xA0 v3 | `0x10102B40` | `[v>2]` bool sub-game | execute `0x10102A00` → concede `0x100F8C50`; **the only end signal on the wire**, PlayerID = conceder |
| GameOverReached 0x17F v2 | `0x10106210` | none | **never built by client code** (ctor `0x10106020` uncalled) |
| EndGame 0x1D7 v1 | `0x10104680` | `[v>2]` bool | **never built by the client**; a world-built EndGame v1 calls forceEndGame `0x10261220` ("The administrator has ended this game!") — a clean abort with no loser, no timer text (UNVERIFIED live) |
| RemovePlayer 0x10B v4 | `0x10108990` | `[v>3]` LossType | PlayerID carries the account id |

Consequences for lon_service.cpp:
- `EndPvP` on 0x17F/0x1D7 from the host (line 3261) never fires with a stock client.
- A rule win (avatar, quests, decking) is invisible to world. World can record: concede (loser = PlayerID of
  ConcedeGame; when the host concedes, the guest echoes ConcedeGame{1} upstream, which world drops but can use
  as an acknowledgement), disconnect/abandon, timeout, fatal error.
- Neither client sends `RequestGameplayAchievement` 0x1E3 (stat recorder `0x10261750` needs `+0x304`
  single-player, gate `0x100CEE80`) or `RecordSinglePlayerGame` 0x1E7 (built only by EQMainController
  `0x102C2F60` / `0x102C4580`) in PvP. `HandleGameplayAchievement` therefore never sees a PvP victory.

**To learn real results: phase-2 DLL report on both clients** (build spec 5): hook EQGame vt+0xAC
(`0x1026F8B0`, slot RVA `0x641448`) or the end routine `0x1026ED90`, and inject a world-only frame, e.g.
class 0x7F0 v1 `U(M) U(loserSeat) U(lossType)`, into the relay socket. Only world parses it.

### 9.2 Journal achievements 72 / 73 / 74 (`lon_achievements.md`)

Live awarded them server-side (the client has no trigger; the only client achievement message is the
single-player-gated RequestGameplayAchievement). World awards with NotifyAchievement 0x1E1 (+ IntroduceAccount
for XP) as for the others.

- **72 Friendly Combat** (winner) / **73 Honorable Defeat** (loser): award only when
  1. both DLL reports agree on loser seat and loss type;
  2. the loss type is 0x11175 / 0x11176 / 0x11177 (the achievement text says questing, avatar defeat or
     decking; concede and timeout do not count);
  3. the stream ended normally (no FatalError, no world RemovePlayer/EndGame);
  4. minimum turns (max relayed ButtonPressed 0x3E TurnNumber) and minimum time (e.g. 5 min);
  5. different accounts, different IPs, no GM accounts (the one-card-game-per-PC gate, `IsActiveLooter()`, a stub
     you implement, already applies).
  Both are one-time awards, so farming is limited; apply the same rules plus a per-pair daily cap to any
  repeatable stat or XP.
- **74 Win a Tournament Game**: never from casual play (no tournaments).
- Collusion risk: the host is the authority (sees the guest's hand, owns the RNG, can forge commands; SynchPoint
  check `0x100E59A0` is empty). Two agreeing reports prove both clients saw the same result, not that the game
  was fair; colluding accounts can trade wins. Keep rewards cosmetic.

### 9.3 Profile stats the client can show (`WAUserInfoDialog`, getInfo `0x104C6050`)

Read from the **cached account's** PropertySet (IntroduceAccount / Join / BulkJoin) when the profile opens
(event 0x10 refreshes only header + Journal, `0x104C67B0`).
- `winLossTable` (`0x104C9AF0`): key **0x1055**, type **13 map<int,int>** (missing/wrong type → empty map via
  `0x101D82E0`, safe). Rows drawn when both keys exist: "Casual Timed Games" wins 0x14 / losses 0x15;
  "Tournament Games" 0x0A / 0x0B (column order medium confidence).
  Wire: `U(0x1055) Header(0x16,4,1) U(13) U(1) U(4) U(0x14) U(w) U(0x15) U(l) U(0x0A) U(tw) U(0x0B) U(tl)`
  (map reader `0x1005BC60` → `0x101770C0`).
- `ratingsTable` (`0x104C79E0`): int keys **0xFC8** Overall, **0xFCA** Multiplayer, **0xFC9** Constructed,
  **0xFCB** Limited, **0x1062** Sprint (label↔key medium confidence); non-int shows 0.
- The lobby user list has no win/loss columns. "Get info" sends **AccountCommand_GetAccountInfo 0x129 v2**
  (`0x10394130`: AccountID, nullable PropertySet, `[v>=2]` GroupID); world does not handle it today. Its
  client execute `0x1000DBA0` posts UI event 0x4B (consumer UNVERIFIED). Safe plan: put 0x1055 and the rating
  keys in every account PropertySet world sends, and answer GetAccountInfo with the same set.

### 9.4 Proposed tables (manifest, no foreign keys)

`lon_pvp_match`:
- identity: `match_id` (M, PK), `game_id`, `game_type` (1 casual), `play_format`, `started_at`, `ended_at`;
- players: `host_account`, `guest_account`, `host_char_id`, `guest_char_id`, `host_ip`, `guest_ip`,
  `host_deck`, `guest_deck`, `host_avatar`, `guest_avatar`;
- end: `end_reason` ENUM(concede, rules, disconnect, timeout, fatal, force_end, gm, cancelled), `loss_type`
  INT (raw), `winner_account`, `loser_account` (nullable), `turns`, `host_frames`, `guest_frames`;
- reports: `host_report_loser_seat`, `host_report_loss_type`, `guest_report_loser_seat`,
  `guest_report_loss_type`, `reports_agree`, `counted`, `rewarded`, `note`;
- indexes: (`host_account`, `ended_at`), (`guest_account`, `ended_at`), (`winner_account`).

`lon_pvp_stats(account_id PK, casual_w, casual_l, tourn_w, tourn_l, rating_overall, rating_mp,
rating_constructed, rating_limited, rating_sprint)` — or compute from `lon_pvp_match` rows with `counted = 1`
— sent as 0x1055 `{0x14:w, 0x15:l, 0x0A:0, 0x0B:0}` and the int rating keys.

Phase 1 (no DLL report): store every match with `end_reason` and `winner_account` only for concede and
disconnect; rule endings stay `result unknown`. Do not update 0x1055 from phase-1 data unless you
accept concede/disconnect-only counts.

---------------------------------------------------------------------------------------------------------------

## 10. Key VAs (new in this pass)

| What | VA |
|---|---|
| Navigator Casual Games / chooser + full check / casual-lobby id test | 0x10473900 / 0x10471A50 / 0x10068260 |
| Lobby screen setGroup (casual vs trade look, Create/Quick Join/practice) | 0x10457560 |
| Lobby group model ctor / refresh / row getter / data / columns | 0x10452BD0 / 0x10452670 / 0x104523F0 / 0x10452740 / 0x10451DE0 |
| Lobby translator (EQ) / value→text / play format / game type / structure | 0x1064782C vtable / 0x10459B70 / 0x102C1A80 / 0x10459790 / 0x10459890 |
| Match read / write / ctor / dtor-ish reset / started get,set / state get,set | 0x1015F390 / 0x1015F3E0 / 0x1015F1C0 / 0x1015F260 / 0x1015F300,0x1015F2D0 / 0x1015F290,0x1015F280 |
| Lobby write (deck from 0xFB0) / Lobby::getProperty (virtual keys) / FA5 flag test / canJoin | 0x10137A00 / 0x10131BB0 / 0x10131160 / 0x10132290 |
| Create dialog ctor / setup (keys) / accept (AddGroups) | 0x103D6C20 / 0x103D80E0 / 0x103D71D0 |
| Practice-style quick create/join / Quick Join with prefs | 0x10455410 / 0x10455CC0 |
| Join dialog ctor / setup / accept (Join + SelectDeck) | 0x1044F8F0 / 0x1044FA70 / 0x104500F0 |
| Join-to-group UI handler (room / challenge / lobby screen) | 0x10453170 |
| Room ctor / setup / metacall / add / remove / ready / close (Leave) / +0x50 setter | 0x10466450 / 0x10464F30 / 0x104E06A0 / 0x10466540 / 0x10465D70 / 0x10465E70 / 0x10464A00 / 0x10464950 |
| Player widget fill / ready sender | 0x104688D0 / 0x104682E0 |
| WALobbyManager event filter (3 add, 4 remove, 5 join, 6 leave, 0x11 launch, 0x7E state) | 0x10454210 |
| Enter lobby (leaves other type-1 lobbies) | 0x10156B60 |
| SetReady read/exec, SelectDeck read/exec, SetMatchState exec, SetTimer exec, RemoveGroups exec | 0x10162520/0x10162300, 0x10160980/0x101608D0, 0x10161EB0, 0x10162750, 0x10148040 |
| InitiateChallenge read / exec | 0x10160010 / 0x1015FE20 |
| User-list menu build / show / triggered / Join hosted game / user's match | 0x104CAEE0 / 0x104CB800 / 0x104CC130 / 0x104CBD60 / 0x104CB5E0 |
| Busy flag get/set (manager +0x68) | 0x1045DDF0 / 0x1045DE10 |
