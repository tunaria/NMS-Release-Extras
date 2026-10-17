# Legends of Norrath casual PvP: build spec (client-hosted master, world relay)

Researched 2026-10-06. Static RE only: LegendsOfNorrath.dll in Ghidra (base 0x10000000) plus a capstone/pefile
scan of the same file (`EverQuest Clients\EverQuest - RoF2 Client\LegendsOfNorrath\LegendsOfNorrath.dll`).
Read-only: nothing was renamed, built, run or written except this file. VAs are at base 0x10000000. RVA = VA - 0x10000000.

Wire primitives (`U`, `S`, `Header(cls, ver, levels)`, nullable object = `U(0)` + object, plain object = no flag)
are the ones in `lon_service.cpp` and `lon_trade_lobby_spec.md` section 0. Every header level repeats the leaf's
(classId, version). The header reader `0x10176CA0` accepts the leaf id at any level (vtable+0x18 and +0x1C both
return the leaf id: `0x102736D0`/`0x102736E0` = 0x11177). Every `since vN` gate compares against that leaf version.

---------------------------------------------------------------------------------------------------------------

## 0. What changed compared with `lon_pvp.md` (read this first)

| `lon_pvp.md` said | Verified now |
|---|---|
| Host = the skirmish path (`EQMainController`) with the AI removed | **Rejected.** Host = an ordinary network `EQGame`, built by the same server messages as the guest, with **one byte** (`Game+0x139` isServer) set by our DLL before setup runs. No AI, no skirmish, no fake accounts (skirmish uses accounts -1000/-1001), no `RecordSinglePlayerGame`, and no deck injection into the host's deck registry. Section 1.2 explains why. |
| `setIsServer(1)` (vtable+0x1B0) | **Never call it on an EQGame.** The base setter `0x100CFFE0` writes +0x139 **and clears the client-side singleton** `DAT_1078C048` (`0x100CFFEE: mov [0x1078C048],0`). The game screen and the network receive gate use that singleton. `EQMainController` overrides the slot with a plain byte store (`0x102C2D30`). Write the byte directly. |
| Flush detour: pop `outbox[2]`, then run the original | **Wrong, it loses commands.** `Game::flush` `0x100F4180` first pumps the network receive (`0x10173D20` → `0x10175530`), and received guest input executes inside that pump and queues new commands for seat 2. Then `0x100F1A00(local)` **deletes every queue except the local seat's**. Sending before calling the original therefore drops whatever the pump queued. The host flush must be a reimplementation: pump, then send seat 2, then clear (section 1.3 P2). |
| world sends the guest SetupGame + SetPlayer + SelectDeckForPlayer ×2 | **No SelectDeckForPlayer and no SetPlayer to the guest.** Once all players have decks, the master broadcasts `SelectDeckForPlayer` for every seat to every seat itself (`Game::selectDeck` `0x100F4490`: isServer branch → ctor `0x10109D80` + `sendToAll 0x100F2CB0`). A second copy from world would select the guest's decks twice. SetupGame's execute already sets the local player, so SetPlayer adds nothing. |
| Challenge from the Trade Lobby user list | **Disabled in this build.** The user-list menu `0x104CB800` calls `QAction::setEnabled(challenge(+0x38), false)` for every other user, unconditionally. |
| Random seed at Game+0x15C | Same field, different base: setupGame writes it at **Game+0x16C** (`param_1[0x5b]`); the dump's `this` was Game+0x10. |
| (new) | **A message the client cannot read jams the LoN stream permanently** (section 4.1). |
| (new) | **A GameCommand with no leaf fields fails to read on a stock client** unless at least 4 bytes follow GameID (section 3.3). Pad every relayed frame. |
| (new) | **GameID 0 asserts** (`"mGame"` in almost every execute). |
| (new) | **A host EQGame leaves a dangling singleton at destruction** unless our DLL sets +0x304 first (section 1.3 P3). |

---------------------------------------------------------------------------------------------------------------

## 1. Chosen design (with evidence)

### 1.1 Roles

- **Host (seat 1)**: a normal network game (`EQMatchCommand_LaunchGame` → `new EQGame`), turned into the
  **authority** by our DLL setting `Game+0x139 = 1` inside `EQGameCommand_SetupGame::execute`. The host's own
  UI drives seat 1. Seat 2's input arrives as GameCommands from the network.
- **Guest (seat 2)**: a stock replica. **No DLL patch needed on the guest.**
- **world/lon_service.cpp**: matchmaking, launch, relay and bookkeeping. It never runs rules.

This is the live server's own model (a master Game fed by the match layer), with the master living in player 1's
client.

### 1.2 Why EQGame-as-master works (code paths)

1. **Creation.** `EQMatchCommand_LaunchGame::execute` `0x102C5DA0` posts UI event 0x11 (arg MatchID). If
   `getClientSideInstance()` (`0x100CF030`) is null, it runs `new EQGame` (0x408 bytes, ctor `0x10272A40` → Game ctor
   `0x100FAAF0`). The ctor sets +0x139=0, +0x108=0, +0x304=0, +0x378=1, +0x37A=1, +0x174=1745 and
   MatchStructure +0x33C=1, registers map key 0, and becomes the singleton if none exists.
   - WALobbyManager event 0x11 (`0x10454210` case 0x11): closes a `matchDialog` if one exists, then
     `0x1045FE90(0xB, matchId, 0)`. Screen case 0xB in `0x1045DE50` builds the game screen and sends
     ChangeStatus 5. **No Match/group lookup on this path.**
   - The skirmish start (`0x102C2F60`) posts the same event 0x11, with MatchID -5 and no Match object. So launching
     without any lobby or Match object is a path the shipped client already runs.
2. **Authority switch.** `Game+0x139` is read through `0x100D0000` at ~70 sites. With it set, the hidden and
   random steps run locally (RevealState `0x1019B4F0`, DiscardCardState `0x100AC450`), and the replica-only
   `ReadyForStartOfGame` is skipped (`0x10268ACA..0x10268B0C`: built only when !isServer).
3. **Setup is shared.** EQGame and EQMainController use the **same** setupGame (vtable+0x24 = `0x10267950` in
   both, slots `0x106413C0` / `0x10647D48`), the same advanceTurn (`0x102687F0`) and the same selectDeck
   (`0x10264C40` → `Game::selectDeck 0x100F4490`). EQMainController's other overrides are tutorial, scenario and
   AI hooks only: +0x38, +0x3C, +0xB4, +0xBC, +0xC8, +0xCC, +0xE0 (e.g. `0x102C5080` = tutorial check then base
   `0x100F7090`), +0xE4, +0x19C (AI tick `0x102C2E40`), +0x1B0, +0x1C0, +0x214. A plain EQGame has none of
   them, so it has no AI to disable.
4. **Seat 2 has no controller and waits.** On a master, seat 2's choices only enter through the Game virtuals
   that the network executes call: ButtonPressed `0x10101280` → vtable+0xE0(PlayerID, button, 1) after turn/state
   checks; CardSelected `0x101026B0` → +0xCC; CardPlayed `0x10101E60` → +0xBC; ConcedeGame `0x10102A00` → +0x1C4
   / +0x1C8. These are the AI's entry points too (AI `getInput` `0x10213890` calls vtable+0xE0 with its seat).
5. **Inbound.** The receive loop `0x10174F80` → `0x101740E0` does: read (factory, vtable+0x24). If the object is a
   GameCommand, it is dropped silently when `0x100DF090(1)` finds no game. Otherwise it runs **execute
   immediately** (vtable+0x38). GameCommand read `0x100FF060` resolves `mGame = 0x100DF090(GameID)`: map
   `DAT_1078C04C` by key, else the singleton. Master-side base execute `0x100FEE90` skips the command's element
   map when isServer.
6. **Outbound.** On the master, every local or remote action ends in `sendToAllBut(p)` `0x100F2C80` →
   `sendToAllExcept(p)` `0x100F1770` → `sendToPlayer(seat)` `0x100F1680` → `outbox[seat]` (Game+0x12C, a
   `std::map<int, vector<GameCommand*>>`, `operator[]` `0x100F0A00`). Nothing is dropped as long as +0x378=1,
   +0x304=0 and +0x1C4=0, all EQGame defaults.
7. **Flush gate.** `Game+0x108` is "GameStarted". advanceTurn sets it to 1 when GameIsSetup (+0x168) is set and
   +0x108 is still 0 (`0x1026886D: mov byte [esi+0x108],1`, string "GameStarted = false" / "creating
   StartOfGame"). setupGame `0x10267950` sets +0x168=1 and calls advanceTurn at its end. **The gate is open from
   the end of SetupGame's execute onward**, on both host and guest. Commands queued before that stay queued (the
   gate skips the whole send/clear block).
8. **Decks.** World sends the host `SelectDeckForPlayer` for seats 1 and 2. When the last seat has a deck, the
   master sets each player's deck and broadcasts a `SelectDeckForPlayer` per seat to all seats. That is how the
   guest receives both decks. `0x10271AA0` (EQGame +0x1D8, called on the master) only logs the deck. There is
   **no ownership or collection check** in the client.
9. **Guest outbound.** The guest replica's own input goes `sendToAllBut(2)` → not server → `sendToPlayer(2)` →
   `outbox[2]` → stock flush sends `outbox[local=2]` upstream. Already correct.

### 1.3 DLL patches on the HOST (all three in our dinput8; the guest needs none)

Install them per LoN load, inside `NMSLoNPulse()` (eqgame.cpp:619), the same way as the existing
`0x4A6E50` hook: on a new `GetModuleHandleA("LegendsOfNorrath.dll")`, check the bytes, then patch. LoN unloads
on close, so nothing is ever restored. Reset `g_host = nullptr` whenever the module handle changes.

**Patch style: vtable-slot replacement** (one aligned pointer write in .rdata under `VirtualProtect`). It moves
no instructions and only affects classes whose vtable we touch. Before writing, check that the slot holds
`lon + RVA(original)` and that the original's first bytes match. If either check fails, install **nothing**
(all or none).

| # | Slot (RVA) | Original (RVA) | Original first bytes | Replacement |
|---|---|---|---|---|
| P1 | `0x6416A0` = EQGameCommand_SetupGame vtable `0x641664` +0x3C (execute) | `0x2737E0` | `64 A1 00 00 00 00 6A FF` | `SetupExec` |
| P2 | `0x64142C` = EQGame vtable `0x64139C` +0x90 (flush) | `0x0F4180` | `55 8B EC 6A FF 68` | `HostFlush` |
| P3 | `0x64139C` = EQGame vtable +0x00 (scalar deleting dtor) | `0x272BD0` | `56 8B F1 E8` | `HostDtor` |

EQMainController has its own vtable (`0x647D24`; flush slot `0x647DB4`, dtor `0x102C2E60`), so skirmish,
scenario and tutorial games are untouched. The Game base vtable `0x60A1EC` is untouched.

```cpp
// RVAs (base = (BYTE*)GetModuleHandleA("LegendsOfNorrath.dll"))
constexpr DWORD kNetSvcGet = 0x173D20;  // cdecl, no args, returns NetworkService* (singleton DAT_1078C36C)
constexpr DWORD kNetPump   = 0x175530;  // thiscall(netsvc)
constexpr DWORD kNetSend   = 0x173DC0;  // thiscall(netsvc, GameCommand*) -> bool, RET 4 (writes len+12, the 12-byte preamble, the body)
constexpr DWORD kOutboxIdx = 0x0F0A00;  // thiscall(map* = game+0x12C, int* seat) -> vec*, RET 4; vec+4 = first, vec+8 = last
constexpr DWORD kOutboxClr = 0x0F1A00;  // thiscall(game, int keepSeat), RET 4: deletes cmds of seats != keep, empties all vectors
constexpr DWORD kFlushOrig = 0x0F4180;  // thiscall(game)
constexpr DWORD kSetupOrig = 0x2737E0;  // thiscall(cmd)
constexpr DWORD kDtorOrig  = 0x272BD0;  // thiscall(game, int flags), RET 4
constexpr int   kHostMagic = 0x4E4D5348; // 'NMSH' in SetupGame "Server ID" (+0x28)

static BYTE *g_host = nullptr;          // the EQGame we made authoritative

// P1: become the master when world says so. cmd+4 PlayerID, cmd+8 mGame, cmd+0x28 Server ID.
int __fastcall SetupExec(BYTE *cmd, void *) {
  BYTE *game = *(BYTE **)(cmd + 8);
  if (game && *(int *)(cmd + 0x28) == kHostMagic && *(int *)(cmd + 4) == 1) {
    game[0x139] = 1;   // isServer, written directly; NEVER vtable+0x1B0 (clears the singleton)
    g_host = game;     // +0x304 stays 0 (queue sends), +0x378 stays 1 (ctor)
  }
  return ((int(__thiscall *)(BYTE *))(lon + kSetupOrig))(cmd);
}

// P2: Game::flush 0x100F4180 with seat 2 in place of the local seat (+0x128), for the host game only.
void __fastcall HostFlush(BYTE *game, void *) {
  if (game != g_host) { ((void(__thiscall *)(BYTE *))(lon + kFlushOrig))(game); return; }
  void *net = ((void *(__cdecl *)())(lon + kNetSvcGet))();
  ((int(__thiscall *)(void *))(lon + kNetPump))(net);           // receive first, exactly as the original
  if (game[0x108] != 1) return;                                 // gate: GameStarted
  int seat = 2;
  BYTE *vec = ((BYTE *(__thiscall *)(void *, int *))(lon + kOutboxIdx))(game + 0x12C, &seat);
  for (void **it = *(void ***)(vec + 4), **end = *(void ***)(vec + 8); it != end; ++it) {
    void *cmd = *it;
    net = ((void *(__cdecl *)())(lon + kNetSvcGet))();
    ((bool(__thiscall *)(void *, void *))(lon + kNetSend))(net, cmd);
    if (cmd) (*(void(__thiscall **)(void *, int))(*(void ***)cmd))(cmd, 1);   // scalar deleting dtor
  }
  ((void(__thiscall *)(BYTE *, int))(lon + kOutboxClr))(game, 2); // drop outbox[1] (= to ourselves), empty all
  void **vt = *(void ***)game;
  ((void(__thiscall *)(BYTE *))vt[0x58 / 4])(game);              // same tail calls as the original
  ((void(__thiscall *)(BYTE *))vt[0x5C / 4])(game);
}

// P3: the Game dtor (0x100F9BE0) clears the singleton only when (!isServer || +0x304). Make it so.
void *__fastcall HostDtor(BYTE *game, void *, int flags) {
  if (game == g_host) { game[0x304] = 1; g_host = nullptr; }
  return ((void *(__thiscall *)(BYTE *, int))(lon + kDtorOrig))(game, flags);
}
```

Notes and risks:
- **P1 (low risk).** A pure pass-through unless both the marker and seat 1 are present. A client without our
  DLL ignores Server ID (EQ execute `0x102737E0` never reads +0x28; only the unused base execute `0x1010B0A0`
  does), stays a replica, and the match stalls (it does not crash). Live servers never sent this value.
- **P2 (medium risk, the only code with logic).** It mirrors the original flush exactly (disassembly
  `0x100F4180..0x100F42CB`), except: seat 2 replaces `[esi+0x128]`, and the local-player assert is skipped.
  `outbox[1]` holds the commands the master addressed to seat 1, which is ourselves. The master has already
  applied that state, so deleting them matches single player, where +0x304=1 drops everything. If the vector
  layout were different it would crash on the first flush, so test T1 shows it at once.
- **P3 (low risk).** Without it, the destroyed host game stays the singleton (dtor `0x100F9BE0`: `if ((+0x139==0
  || +0x304!=0) && singleton==this) singleton=0`). The next LaunchGame (`0x102C5DA0`) would then reuse freed
  memory: certain crash on the second game. Setting +0x304 in the dtor only disables sends from a dying game.
- Not needed: no AI change (EQGame has none), no +0x120 (the "local AI" flag; vtable+0x190 readers
  `0x101AB690`, `0x101AC290`, `0x103013B0` only hide AI cues), no change to `0x100F1680`/`0x100F2C80`, no
  guest patch.

### 1.4 Rejected alternative (for the record): skirmish-based host

It would need the following, each a crash or desync risk. The EQGame design needs none of them.
- Pause the AI: EQMainController vtable+0x19C → `[game+0x438]` = AI → `0x10213890`. Its `waitForUser` byte AI+0x28
  makes getInput return 1.
- Put the guest's EQDeck into the host's local deck registry: the skirmish looks seat 2's deck up by name with
  `0x102C2E00` → `0x100A89F0`.
- Live with fake accounts -1000/-1001 and GameType 1 set by hand.
- Suppress `RecordSinglePlayerGame` (sent at setup via `0x10068380`).
- Override the EQMainController setIsServer.

---------------------------------------------------------------------------------------------------------------

## 2. Matchmaking and start: fewest crash-prone messages

### 2.1 Comparison

| Option | Client-parsed messages world must add | Verdict |
|---|---|---|
| (a) Casual Games lobby (groups 2/9..19, Create/Quick Join) | `AddGroups` with **Match 0x68** objects (property keys 0xFA5..0xFB0 and 0x273 undecoded), `Join(match)` → WAMatchDialog, `SetMatchState`, `SetBulkMatchState`, `SetTimer`, `SetDeckArchetype`, `RemoveGroups`, `ChangeBulkStatus`, and the room's SelectDeck/SetReady protocol | Most surface. None of it verified. Rejected for phase 1. |
| (b) Trade Lobby user-list "Challenge" | Not reachable: the item is force-disabled (`0x104CB800`). It also lands in the challenge dialog, which needs a Match object (`0x10453170` → `0x10153990` → `challengeDialog` `0x103BA4C0`) | Needs a client patch **plus** all of (a)'s Match machinery. Rejected. |
| (c) EQ `TCG:CHALLENGE:` relay / `--challenge=` / `MatchCommand_InitiateChallenge` 0x1E6 | 0x1E6 only posts UI event 0xC1. The dialog opens from the **Join-to-Match** handler (`0x10453170`, flag manager+0xBC), so again Match objects plus Join echoes | Same Match machinery as (a). Rejected. |
| **(d) Text commands in the Trade Lobby chat** (already relayed by world) | **None new for matchmaking.** Prompts reuse the proven `LobbyCommand_SendText` tell. The start is **LaunchGame + EQ SetupGame** (+ 2 × SelectDeckForPlayer to the host only) | **Chosen.** 3 message classes, all read functions verified below. |

### 2.2 Matchmaking (option d)

Both players are in the Trade Lobby (group 3; joined and cached on both sides through Join/BulkJoin, so names
resolve).
- `HandleLobbySendText`: if the text starts with `!`, **do not relay it**. Parse:
  - `!challenge <name> [deck name]`
  - `!accept [deck name]`
  - `!decline`
  - `!decks` (lists the player's deck names)
- Prompts: `SendText v4` disposition 0 (tell) to the target, AccountID = the challenger (this is the proven
  writer at lon_service.cpp:2398). Example text: "[Challenge] Casual game (no prize). Type !accept [deck] or
  !decline." Send confirmations and errors to the issuer as a tell to self.
- Preconditions:
  - both online and `in_trade_lobby`, with lobby status 2;
  - neither in a live trade or another match;
  - not the same account;
  - optional: not on each other's ignore list;
  - the challenge expires after 60 s.
- Deck selection: the named deck, or the account's most recently saved `lon_deck` row (or an owned starter
  deck, the same set `SendDecks` lists). Server-side validation: the bytes parse (`DeckName` non-empty,
  `DeckCards` non-empty), every card is owned (a count-aware `lon_card` check), and the deck builder's own
  legality (avatar present, minimum card count; see uncertainty U8). The client does **not** check ownership.
- **Host = random** of the two (the host can read hidden cards; randomness removes the incentive to pick).
- IDs: `M = G = 2,000,000 + n` (unique per server lifetime, non-zero, never a lobby group id, never 3 or -5).
  The client uses M as the game screen's group (it later sends `Leave(M)`, which is ignored) and G as the game-map
  key.

### 2.3 Start sequence (byte-exact)

Example values: host account A = 5, guest account B = 7, M = G = 2000001 (`61 48 E8 01`), TS = 1791000000
(`80 DC 07 AC 06`, see section 3.3), magic `0x4E4D5348` (`88 34 D5 E4 04`), GameVersion 1745 (`21 6D`). Class
bytes: 0x11178 → `48 17 11`, 0x11177 → `47 17 11`, 0x3A → `2A 03`.

**Order (each step is a hard requirement):**
1. To the **guest**: LaunchGame, then SetupGame(seat 2).
2. To the **host**: LaunchGame, then SetupGame(seat 1, magic), then SelectDeckForPlayer(seat 1, host deck), then
   SelectDeckForPlayer(seat 2, guest deck).
3. From now on, relay (section 3). Doing the guest first guarantees the guest's game and current turn exist
   before any master traffic reaches it. Both sides are sent from the same world thread.

**EQMatchCommand_LaunchGame 0x11178 v1, 5 levels** (EQ `0x102C5F90` → `0x10160410` → MatchCommand `0x1015F870`
→ LobbyCommand `0x10138460` → NetworkCommand `0x1016C670`): `Header(0x11178,1,5) U(AccountID) U(MatchID)`.
AccountID (LobbyCommand +4) is not used by the execute; send the recipient's account.
```
48 17 11 01 ×5  05  61 48 E8 01          (to A; to B the AccountID byte is 07)
```

**EQGameCommand_SetupGame 0x11177 v12, 4 levels** (EQ read `0x102734B0` → base `0x1010B7C0` → GameCommand
`0x100FF060` → NetworkCommand `0x1016C670`). Fields in wire order. All `since` gates pass at v12.

| # | Field (dest) | Value | Notes |
|---|---|---|---|
| 1 | PlayerID (+4) | host 1 / guest 2 | execute → `setLocalPlayer` `0x100CF5E0`, **asserts if 0** |
| 2 | GameID (+0xC) | G | never 0 |
| 3 | TimeStamp lo (int64, `0x10097960`) | TS | see section 3.3 |
| 4 | TimeStamp hi | 0 | |
| 5 | map<int,int> (+0x18) | count 0 | applied only on replicas |
| 6 | GameID again (base +0xC) | G | base read reads +0xC a second time |
| 7 | Game Server ID (+0x24 → game+0x18) | 1 | |
| 8 | Server ID (+0x28) [v>8] | host: magic / guest: 0 | **not used by the EQ execute = our marker** |
| 9 | Player Count (+0x2C → game+0x140/+0x144) | 2 | players are created only if >1 |
| 10 | Player Order Data (int list +0x30) | `2, 1, 2` | count + ids; each id must be a created player (null deref otherwise) |
| 11 | Ordered Account IDs (int list +0x40) | `2, A, B` | same length as count (assert, eqgame.cpp:0x3FE/0x3FF) |
| 12 | GameVersion (+0x50) | 1745 | a mismatch shows an error box only (not fatal) |
| 13 | MatchID (+0x54 → game+0x60) | M | |
| 14 | Duration (+0x58 → game+0x230) [v>2] | 0 | >0 starts a per-player timer (setupGame `piVar7[0x8c]`) |
| 15 | GameType (+0x5C → game+0x270) [v>3] | 1 | as skirmish (`FUN_100CEF10(1)`). 3 would add player property 0x1161B |
| 16 | PlayType (+0x60 → game+0x274) [v>4] | 0 | 0x11176/0x11184 adds a 3rd player |
| 17 | ExtraFormatFlag (+0x64 → +0x2C0) [v>5] | 0 | skirmish 0 |
| 18 | GameNum (+0x68 → +0x2D4) [v>6] | 0 | |
| 19 | vector of pairs (+0x6C, `0x10063390`) [v>7] | count 0 | |
| 20 | LeagueID (+0x78 → +0x2D8) [v>7] | 0 | |
| 21 | MatchStructure (+0x7C → +0x33C) [v>9] | 1 | the ctor default; 1 = a loss ends the game |
| 22 | EQ int (+0x98 → game+0x404) [v>11] | 0 | |
| 23 | map +0x80 → game+0x3E8 = **positions** [v>10] | `{1:1, 2:1}` | as skirmish (`0x102CCB50` = EQPlayer+0x54) |
| 24 | map +0x8C → game+0x3F4 = **teams** [v>10] | `{1:1, 2:2}` | **required.** The default team is 0 for both, and gameLostByPlayer `0x1026F8B0` treats same-team players as allies, so the game would not end |

```
host:  47 17 11 0C ×4  01 61 48 E8 01 80 DC 07 AC 06 00 00  61 48 E8 01 01 88 34 D5 E4 04 02
       02 01 02  02 05 07  21 6D 61 48 E8 01 00 01 00 00 00 00 00 01  00  02 01 01 02 01  02 01 01 02 02
guest: identical except PlayerID 02 and Server ID 00:
       47 17 11 0C ×4  02 61 48 E8 01 80 DC 07 AC 06 00 00  61 48 E8 01 01 00 02 ...(same tail)
```
Execute `0x102737E0` calls setters in this order: setGameID (removes map key 0, inserts G), +0x18, players,
Duration, order, accounts, setLocalPlayer, MatchID, GameType, PlayType, Extra, GameNum, League, MatchStructure,
the two maps, +0x404, then `setupGame(0)` (`0x10267950`: seed = time, creates players, applies accounts, teams
and positions, sets GameIsSetup, then advanceTurn, which opens the flush gate).

**GameCommand_SelectDeckForPlayer 0x3A v2, 3 levels** (read `0x1010A150`): base fields, `S(Deck ID)`, nullable
`EQDeck` (`0x100A5010`). **To the host only**, seat 1 then seat 2.
```
2A 03 02 ×3  U(seat) 61 48 E8 01 80 DC 07 AC 06 00 00  S(deck id)  00  <stored lon_deck bytes>
```
- The stored deck bytes are exactly what the client wrote in `AddOnlineDeck` (they start `U(0x11172) U(10)` ×2).
  The same bytes after `U(0)` are already live-proven in `DeckPopulate` (lon_service.cpp:878).
- Execute `0x1010A040` asserts `"mGame"`, asserts `"player"` (`getPlayer(PlayerID)`, so seat must be 1 or 2 and
  SetupGame must have run), then calls `deck->vtable+0x74` **with no null check**. The flag must be 0 and the
  object present.
- `Game::selectDeck` `0x100F4490` asserts `getCurrentTurn()`, so **it must come after SetupGame**.
- Deck ID string: unused by the execute; send the deck's key (3rd string, `DeckName(.., &key)`).

Nothing else is needed. No SetPlayer (SetupGame sets the local player). No ReadyForStartOfGame handshake (its
execute `0x101084D0` is `return 1`; the master never waits for it). No Match object.

---------------------------------------------------------------------------------------------------------------

## 3. In-game relay

### 3.1 Framing

- Client → world frames carry the 12-byte preamble. `HandleMessage` already strips it (the first preamble byte
  0x00 parses as op 0). The relay works on the **stripped** command bytes `data[0..len)`.
- World → client frames have **no preamble**: `Writer::Frame()` = 4-byte big-endian length + body.
- **Relay rule:** body = the received command bytes unchanged, **plus 4 zero bytes** of padding, then re-frame.
  Readers never check for trailing data (`0x10097790` is `return 1`; the receive loop consumes exactly the frame
  length), so padding is invisible except that it satisfies the int64 rule (3.3).

### 3.2 Routing (world has a `Match{M, G, host, guest, hostAcct, guestAcct, state, lastTraffic}`)

Parse with a header loop: read (cls, ver), then skip further pairs while they equal (cls, ver). PlayerID and
GameID are always present at any version. Then:

| From | Class | Action |
|---|---|---|
| host | any GameCommand class (catalog list: 0x38-0x49, 0x75, 0x80, 0x82, 0x84, 0x8B, 0x8C, 0x90, 0x9E, 0xA0-0xA2, 0xA5, 0x106-0x108, 0x10B, 0x134, 0x13A, 0x140, 0x142, 0x145, 0x146, 0x17F, 0x1D7) with GameID == G | forward to the guest (padded). PlayerID is not rewritten. Only `outbox[2]` ever leaves the host (P2) |
| guest | GameID == G and **PlayerID == 2** (else drop + log) | forward to the host (padded), **except**: drop 0x75 ReadyForStartOfGame and 0x40 OutOfSync; 0x84 FatalError → abort (3.5); drop adjournment 0x107/0x108 (unsupported); drop **authority-only** classes the guest must never send: 0x38, 0x3A, 0x41, 0x42, 0x43, 0x44, 0x45, 0x49, 0x80, 0x82, 0x8B, 0x8C, 0x90, 0x106, 0x10B, 0x134, 0x17F, 0x1D7. Log every drop (the test shows whether a legitimate class was missed) |
| either | GameCommand with GameID != G, or no active match | drop + log |
| either | non-GameCommand (ChangeStatus, SendText, Leave(M)...) | normal handling. `Leave(M)` is already ignored (group != 3). Optional in-game chat: relay `SendText{GroupID M}` between the two players only, rewritten like the trade chat |

- **Nothing to rewrite.** World never changes PlayerID, GameID, TimeStamp or the element map. The master's
  per-command map (GameCommand +0x18, built by `0x100EC540`, applied on replicas by `0x100FEE90` →
  `0x100E6D90`) must reach the guest verbatim.
- **Turn and sequence numbers** live inside the leaves (ButtonPressed TurnNumber/State). The master validates
  them and drops mismatches with a log line ("buttonPressed with wrong turnNumber received from player %d").
  World keeps per-socket order and never reorders or merges frames.
- Update `lastTraffic` on every relayed frame.

### 3.3 The int64 TimeStamp trap (why padding is mandatory)

`GameCommand::read` reads TimeStamp with `0x10097960`. That function refuses (`return 0` → the read fails) unless
`pos + 4 <= length`, **even in varint mode**. Client-built commands have TimeStamp 0 (ctor `0x100FF350`; the
setter `0x100FED70` has no callers), so TS lo 00 + hi 00 + map count 00 = 3 bytes. **Any GameCommand with no leaf
fields would fail to read** on a stock client: GameOverReached 0x17F, OutOfCards 0x45, OfferDraw 0xA2,
ReadyForStartOfGame 0x75, SetPlayer 0x41, OutOfSync 0x40. Fixes:
- relayed frames: append 4 zero bytes (3.1);
- world-built GameCommands: TS lo = a value ≥ 2^28 (unix seconds, 5-byte varint), hi = 0. All examples above do this.

### 3.4 How a game ends

- **Concede.** The UI calls `Game::concede` `0x100F8C50`: builds ConcedeGame (PlayerID = the conceder), sends it
  (master: to all others; replica: upstream), then `gameLostByPlayer(p, 1)` (vtable+0xAC, EQ `0x1026F8B0`). A
  guest's concede reaches the master as ConcedeGame{PlayerID 2} → execute → `concede(2)` → the master ends the
  game and its end-of-game stream goes to seat 2.
- **Normal win.** The master detects it in the rules, and the guest follows the state stream. GameOverReached
  0x17F / EndGame 0x1D7 travel host → guest.
- **World bookkeeping:**
  - ConcedeGame seen from X → loser X (concede = loss type 1);
  - GameOverReached or EndGame from the host → mark the match ended;
  - keep relaying for 60 s after the end (trailing commands);
  - the match is closed when both have status ≠ 5, sent `Leave(M)`, or disconnected.
- **Back to the lobby (client-driven, nothing to send).** The end dialog's "Exit" leaves the game screen. Screen
  switches out of a WAGameScreen call `0x10156950(gameScreenGroup = M)`, which sends `Leave(M)` (ignored). The
  client then sends ChangeStatus 2/3, or `Join(3)` from the navigator, which the existing handler answers. The
  host EQGame dtor runs P3.

### 3.5 Server-originated end messages (only on disconnect, timeout or abort)

**Guest disconnects → host:** `GameCommand_RemovePlayer 0x10B v4` (read `0x10108990`: base, `[v>3]` Loss Type).
The execute `0x10108820` calls vtable+0xB0 (`0x100F7A10`): `getPlayerByAccount(PlayerID)` (**the PlayerID field
carries the ACCOUNT id**; null-safe, logs if unknown) → `gameLostByPlayer(seat, lossType)`. The host wins
normally.
```
2B 10 04 ×3  07 61 48 E8 01 80 DC 07 AC 06 00 00  02        (PlayerID = guest account B, LossType 2)
```

**Host disconnects → guest:** the guest has no authority any more. Send `GameCommand_LostPlayer 0x8B v2` (read
`0x10106AC0`: base, AccountID +0x24). The execute `0x10106B70` shows "Lost connection to <name>" (name lookup is
null-safe, `0x100CF0C0` → "Player %d") and only logs through vtable+0x1AC (`0x100D6440`). It is **safe**: it
changes no game state. The guest then leaves through the game menu. Optionally follow it with RemovePlayer{PlayerID
= host account A, LossType 2} so the replica runs gameLostByPlayer(1) and shows a result (medium confidence, U7).
```
2B 08 02 ×3  01 61 48 E8 01 80 DC 07 AC 06 00 00  05        (PlayerID 1, AccountID = host A)
```

**Inactivity:** Duration 0 means the client runs no turn timer. World: no relayed traffic for 10 min → send
LostPlayer to both (naming the other) + a SendText notice, close the match, record "timeout". Do not guess a
loser.

**FatalError 0x84 from either side:** the master sets +0x1C4 ("FATAL ERROR CAUGHT! ENDING GAME", `0x100F2DE0`)
and stops sending. Abort as for inactivity.

---------------------------------------------------------------------------------------------------------------

## 4. Asserts and stalls reachable from server data

### 4.1 Read failure = permanent jam (not a crash, but the session is dead)

`0x101740E0`: if the class id cannot be read, the factory returns null, or `vtable+0x24` (Read) fails, it returns
-1 **without consuming the frame** ("couldn't get newObject" / "coulnd't get networkcommand"). The pump loop
`0x10174F80` stops, and every later pump retries the same bytes. **Every following message, including chat and
trade, is stuck until the card game reconnects.** This holds for every message world sends, not just PvP. So:
never send an unregistered class, a wrong version, a short field, or a no-field GameCommand without padding.

### 4.2 Asserts (EverQuest exits)

| Trigger | Where | Avoided by |
|---|---|---|
| GameCommand with **GameID 0** (gate passes on the singleton, then `mGame` = lookup(0) = 0) | `"mGame"` in SetupGame `0x102737E0`, SelectDeck `0x1010A040`, ButtonPressed `0x10101280`, LostPlayer, RemovePlayer, base `0x100FEE90` | G ≠ 0; relay only GameID == G |
| SetupGame PlayerID 0 | `setLocalPlayer` `0x100CF5E0` "playerid" | seat 1/2 |
| OrderedAccountIDs empty / ≠ PlayerCount; PlayerOrderData ≠ PlayerCount | setupGame `0x10267950` (eqgame.cpp 0x3FE-0x400) | `2, A, B` and `2, 1, 2` |
| PlayerOrderData id that is not a player | setupGame: `getPlayer(id)` → `setAccount` (null deref) | ids 1, 2 |
| SelectDeckForPlayer before SetupGame | `Game::selectDeck` `getCurrentTurn()` (Game.cpp 0xB2F) | order in 2.3 |
| SelectDeckForPlayer seat not 1/2 | `"player"` | seat 1/2 |
| SelectDeckForPlayer with a null deck (flag 1) | `deck->vtable+0x74` null deref | flag 0, present deck |
| GameCommand arriving with no client-side game | dropped silently (gate `0x100DF090(1)`) | (safe) |
| LaunchGame while an old game is still the singleton | reuses the old object (no assert, undefined game) | P3; launch only for status 2 players; T1 tests a second game |
| Wrong GameVersion | an error box only | 1745 |
| Lookup assert `resultGame->getGameID() == gameid` | `0x100DF090`, master only | key G ↔ id G by construction |
| `LobbyCommand_Join` with a name and no PropertySet, etc. | trade-lobby spec section 7 | unchanged |

---------------------------------------------------------------------------------------------------------------

## 5. Result integrity (later; phase 1 pays nothing)

- The host is the authority: it sees the guest's hand and deck order, owns the RNG, and can forge any command
  (the replica's SynchPoint check `0x100E59A0` is an empty function). The guest can only send its own inputs, and
  the master validates them. Casual play only.
- World can already infer results from the relay: ConcedeGame (loser), RemovePlayer/LostPlayer (disconnect),
  and GameOverReached/EndGame (end). Store `lon_pvp_game(M, host, guest, decks, start, end, result, reason)`.
- Phase 2 two-sided report: our DLL on **both** clients wraps EQGame vtable+0xAC (`gameLostByPlayer`,
  `0x1026F8B0`, slot RVA `0x641448`) and injects a custom upstream frame into its own relay socket
  (nms_lon_link already owns that socket). For example class 0x7F0 v1: `U(M) U(loserSeat) U(lossType)`. Only
  world parses it, so the client takes no risk. Pay only when both reports agree, then add the anti-collusion rules
  of `lon_pvp.md` section 6.

---------------------------------------------------------------------------------------------------------------

## 6. Test plan

### 6.1 One PC, one card game (single client)

- **T0. Patch install.** Load LoN and check the DLL log: the three slot checks pass, all three are written, and
  they are re-applied after a close and reopen. Play one skirmish. It must be unaffected: EQMainController has its
  own vtable, and `SetupExec` and `HostDtor` pass through for non-host games.
- **T1. Host vs a "ghost" guest (a world debug command, GM-only, e.g. `!pvptest host`).** World runs 2.3 for the
  host only, with B = an offline test account and seat 2's deck = a starter deck.
  - Expect: the game screen opens, the decks load, and the master runs the start of game.
  - World logs every frame the host sends (class id + bytes). This **captures the real seat-2 stream** and proves
    P1/P2 and the flush gate.
  - The game then waits for seat-2 input, which proves that a seat with no controller waits.
  - Then world sends RemovePlayer(B, 2): the host must win, Exit must work, and P3 must log.
  - Run **a second `!pvptest host` in the same session** (stale singleton check).
  - Then concede from the host in a third run: ConcedeGame{1} appears in the log.
- **T2. Guest replay (same PC, same client).** World plays T1's captured host stream back to the client as the
  guest: LaunchGame + SetupGame(seat 2), then the recorded frames, padded.
  - Expect: both decks, the opening hand for seat 2, no assert, no jam.
  - The guest's first input (for example the start-of-game button) arrives at world: log the class, PlayerID 2,
    GameID G.
  - This validates the replica's reading of real master traffic and the padding rule. It cannot go further,
    because the recorded master does not react.
- **T2b (optional enabler).** Two EQ instances on one PC need a second local relay port, because nms_lon_link
  binds 127.0.0.1:15782. LoN's port is the global at RVA `0x78C368`, and its literal must be patched per process.
  The one-card-game-per-PC gate (`IsActiveLooter()`, a stub you implement) also applies. If built, this turns T3
  into a one-PC test.

### 6.2 Two players (two clients on a test server)

**T3.** Each case once, with the host swapped in a second run:
1. full game to a normal win;
2. guest concedes;
3. host concedes;
4. draw offer, accept and decline;
5. guest closes the card game mid-game (host gets RemovePlayer, wins);
6. host closes mid-game (guest gets LostPlayer);
7. 10-minute idle (timeout);
8. a second match right after the first, for both;
9. `!challenge` refusals: in trade, busy, unknown deck, expired.

For every run, check world's drop log is empty or understood, and that neither client logs "wrong turnNumber"
or "wrong state".

---------------------------------------------------------------------------------------------------------------

## 7. Remaining uncertainties and the safe choice

| # | Unknown | Safe choice / how it is settled |
|---|---|---|
| U1 | An EQGame running as master was never exercised by the shipped client (EQMainController always was) | The code paths are shared (1.2). **T1 settles it.** Nothing is sent to other players until T1 passes |
| U2 | Whether every replica input class reaches the master correctly (the full list of classes a replica sends) | Forward all non-authority classes and log the drops. T3 |
| U3 | The int64 4-byte rule (read literally from `0x10097960`) | Padding makes it moot. World-built commands use a 5-byte TS |
| U4 | The host game's destruction timing (when Exit frees the EQGame) | P3 covers the dtor whenever it runs. T1 second game |
| U5 | RemovePlayer LossType meaning (1 concede / 2 timeout / 4 ?) | Use 2 for disconnects. Wording only |
| U6 | Whether a host-disconnect RemovePlayer on a replica behaves | Send LostPlayer (state-free) first; add RemovePlayer only after T3 case 6 |
| U7 | GameType/PlayType/MatchStructure semantics beyond what skirmish uses | Copy skirmish: 1/0/1. Teams `{1:1,2:2}` are mandatory |
| U8 | LoN deck legality rules (minimum count, avatar) | Accept only decks that parse, are owned, have an avatar and a minimum count. Starter decks are known good |
| U9 | Element map (+0x18) contents on master commands | Relay verbatim. Never synthesise world-built commands except the 5 classes in sections 2-3 |
| U10 | Read-failure jam (4.1) | Treat every world-sent field as fatal-if-wrong. Only the 5 verified classes |

## 8. Go / no-go

**GO to build, behind a rule flag (`Custom:LoNPvP`, default off, GM-only debug commands).**

The guest needs nothing. The host needs three vtable-slot patches (two pass-throughs plus one 20-line flush copy).
World adds `!` chat commands, two start messages plus a deck message, a padded relay and three end messages,
all verified in their read functions.

**Before any player sees it, T1 and T2 must pass on a single development PC.** They prove the one unproven hypothesis
(EQGame as master) and the replica's parsing of a real master stream. T3 needs two clients on a test server.

Do **not** build the Casual Games lobby or the Match-object flows for phase 1. They hold most of the unverified
crash surface and add nothing the chat challenge cannot do.

## 9. Key VAs (new in this pass)

| What | VA |
|---|---|
| Network receive: execute-on-read, jam on read failure | 0x101740E0 / loop 0x10174F80 / pump 0x10175530 |
| NetworkService getter (singleton DAT_1078C36C) / send | 0x10173D20 / 0x10173DC0 |
| Game::flush / outbox[] / outbox clear | 0x100F4180 / 0x100F0A00 / 0x100F1A00 |
| advanceTurn (sets +0x108 GameStarted) Game / EQGame | 0x100F2880 / 0x102687F0 |
| setupGame (EQ; GameIsSetup +0x168, seed +0x16C, teams/positions maps) | 0x10267950 |
| EQGameCommand_SetupGame read / execute / vtable | 0x102734B0 / 0x102737E0 / 0x10641664 |
| GameCommand_SetupGame base read | 0x1010B7C0 |
| EQMatchCommand_LaunchGame read / execute / vtable | 0x102C5F90 / 0x102C5DA0 / 0x10648044 |
| WALobbyManager event filter (case 0x11 → game screen) | 0x10454210 |
| Screen switch (case 0xB game screen) | 0x1045DE50 |
| Game::selectDeck (master broadcast) / EQ selectDeck | 0x100F4490 / 0x10264C40 |
| Game dtor (singleton clear condition) / EQGame deleting dtor | 0x100F9BE0 / 0x10272BD0 |
| Game base setIsServer (clears singleton) / EQMainController override | 0x100CFFE0 / 0x102C2D30 |
| EQGame vtable / EQMainController game vtable / Game vtable | 0x1064139C / 0x10647D24 / 0x1060A1EC |
| concede / concedeSubGame / gameLostByPlayer (EQ) | 0x100F8C50 / 0x100F8D20 / 0x1026F8B0 |
| RemovePlayer read / execute / removePlayerFromGame | 0x10108990 / 0x10108820 / 0x100F7A10 |
| LostPlayer read / execute | 0x10106AC0 / 0x10106B70 |
| User-list menu (Challenge force-disabled) / menu build | 0x104CB800 / 0x104CAEE0 |
| AI getInput (waitForUser +0x28) / AI tick slot (EQMainController vt+0x19C) | 0x10213890 / 0x102C2E40 |
| int64 reader (pos+4 <= len rule) | 0x10097960 |
