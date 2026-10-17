# LoN PvP host: black game screen (T1 `!pvptest host 120`) - root cause and fix

Researched 2026-10-06. Static RE only (Ghidra + a capstone sweep of the same
`LegendsOfNorrath.dll`). Nothing was edited, built or copied. VAs are at base 0x10000000; RVA = VA - 0x10000000.
Read with `lon_pvp_build_spec.md` (section 1).

## TL;DR

The master never posts **per-player display actions** to its own UI. `Game::addDisplayActionForPlayer`
(`0x100E1FD0`) posts an action locally only when `+0x304` (single player) is set, **or** when the game is
**not** the server (`+0x139 == 0`). Our host is a server with `+0x304 == 0`, so the function deletes
every per-player action. That includes the one that binds the game screen to the game: display action
**0x60** ("game set up", posted by setupGame for the local player). Without 0x60 the screen never stores
its Game pointer (`screen+0x5C`). Its event filter then drops every later display action, so you get a
black board and nothing else. The chat box shows because it is part of the screen widget itself.

Skirmish works because EQMainController runs with `+0x304 = 1`, which takes the first branch.

**Smallest fix:** one 2-byte code patch in the host's DLL. At VA `0x100E205F` (RVA `0x0E205F`), change
`75 23` (`jne 0x100E2084`, "isServer → delete") to `90 90`. For every stock game type the result is the
same as before; only an isServer game with `+0x304 == 0` (our host) changes (section 4). Keep deleting
`outbox[1]`. Executing those commands would double-apply state.

---

## 1. What the game screen waits for (Q1)

### 1.1 Screen creation is synchronous (not the problem)

- LaunchGame execute `0x102C5DA0` posts UI event 0x11. WALobbyManager filter `0x10454210`, case 0x11
  (jump-table target `0x104544E1`), calls `0x1045FE90(0xB, matchId, 0)` directly. With `param_4 == 0`,
  `0x1045FE90` calls the screen switch `0x1045DE50` at once.
- Case 0xB uses the factory `DAT_1078DF9C` (EQObjectFactory, vtable `0x1064AE9C`), slot +0x78 =
  `0x102CB970`, which runs `new EQGameScreen` (ctor `0x1028D950`, vtable `0x10642A3C`; base WAGameScreen ctor
  `0x1041D5D0`, vtable `0x10683424`).
- The WAGameScreen ctor sets `+0x2C = -1` (gameID, `0x1041D61B`) and `+0x5C = 0` (Game*, `0x1041D63E`). It
  also installs its event filter on the application (`0x1041D82E..0x1041D837`: `installEventFilter(qApp)`).

### 1.2 How display actions reach the screen

- UI pump `0x103F7230` runs in this order: network pump (`0x1006B6B0` → `0x10175530`), then `game->flush()`
  (vtable+0x90 = our HostFlush), then the UI-event list (which includes 0x11 → screen created), and last
  the game's display-action queue `Game+0xD4` (getter `0x100CF7C0`, only caller `0x103F7371`). Each action
  is wrapped in QEvent type 0x3E9 (`0x103F6210`) and sent to the WAEventSink (`0x10404810`, vtable `0x10680454`).
- Actions enter `Game+0xD4` only through `Game::addDisplayAction` `0x100E1F20`. That function requires
  `+0x378 != 0` and `vtable+0x8C(localPlayer, action)` (EQGame `0x102721C0`: display mode `+0x178 == 0` →
  true) and `+0x1C4 == 0`, then pushes onto `+0xD4` (`0x100E1F99`).

### 1.3 The condition that turns black into a board

WAGameScreen event filter `0x10415A80` (reached from EQGameScreen filter `0x1027CEA0` at its tail
`0x1027F00C`), for QEvent 0x3E9:
```
type = action->type
if (type != 0x60 && type != 0xBF && type != 0xBE) {
    if (screen+0x5C == 0) return false;                  // NO GAME BOUND -> every action dropped
    if (arg0 is int && arg0 != game->getGameID()) return false;
}
return screen->vtable+0x118(action);                    // EQGS 0x102A97D0 -> WAGS 0x1041D910
```
- `0x1041D910` case 0x60 → vtable+0x154 = EQGS `0x1028CC40`, the board builder:
  - It first calls `0x10419FE0`. If `arg0 (gameID) != screen+0x2C` and `+0x2C == -1`, that function sets
    `+0x2C = gameID` and **`+0x5C = 0x100DF090(gameID)`** (`0x1041A03A` / `0x1041A045`), stores my player id
    (`+0x30`), and adds a player base per player (vtable+0x120 = `0x102AA0A0`).
  - It then lays out 2-5 bases (`"false"` assert at eqgamescreen.cpp 0x2756 otherwise), sets `+0x15C = 1`
    and draws (vtable+0x100).
  - `0x10419FE0` is the **only** caller of vtable+0x120 (only `add edi,0x120; call [edi]` site,
    `0x1041A0EB..0x1041A10E`). So 0x60 is the one and only event that builds the board.
- 0x60 is posted by EQ setupGame `0x10267950` at `0x10267F12..0x10267F46`:
  `addDisplayActionForPlayer(p = getLocalPlayer() (0x100CF5B0 → +0x128), action{0x60, gameID, +0x128})`
  (also the base `Game::setupGame` `0x100F3260` at `0x100F37FE`, same call shape).

So the screen waits for display action **0x60**. Nothing else is needed: no ReadyForStartOfGame, no
account lookup, no asset handshake.

### 1.4 Why 0x60 never arrived on the host

`Game::addDisplayActionForPlayer(int p, DisplayAction *a)` `0x100E1FD0` (21 call sites; disassembly
`0x100E1FD0..0x100E209B`):
```
if (+0x378 == 0) { delete a; return; }
assert(a->isValid());
if (!vtable+0x8C(p, a)) return;                                    // 0x100E2034
if (+0x304 && !+0x1C4 && (p == 0 || p == +0x128)) goto post;       // 0x100E203A..0x100E2056  single player
if (+0x139 /*isServer*/) goto del;                                 // 0x100E2058: 80 BE 39 01 00 00 00 / 0x100E205F: 75 23
if (!+0x1C4 && (p == 0 || p == +0x128)) goto post;                 // 0x100E2061..0x100E2074  replica
del:  delete a; return;                                            // 0x100E2084
post: addDisplayAction(a) (0x100E1F20);                            // 0x100E2076
```
The design assumed a server never has a UI, except single player (`+0x304`). Our host is isServer with
`+0x304 = 0` (it has to be: `+0x304 = 1` makes `sendToPlayer` `0x100F1680` delete commands instead of
queueing them, so nothing would reach seat 2). Result: the 0x60 for seat 1 is deleted, the screen never
binds, and the screen drops every un-scoped action that did get queued (through `0x100E1F20`).

Not the cause (checked):
- Screen timing. Screen creation is synchronous and runs before the display drain, in the same pump, on
  both host and guest.
- The ghost account. Base creation reads the player's element `player+0x30 → +0x34`
  (`0x101869F0` / `0x10181E90`). The client account cache is used only for a display name, which falls
  back to "Unknown Account" (`0x10186A1F`).
- The display-mode filter. EQGame `vtable+0x8C` `0x102721C0` returns true in mode 0. `+0x178` is
  written only by the ctor, `0x100CF060` (one caller in a command execute, `0x1010AC34`) and Game
  vtable+0x78.

## 2. Is outbox[1] what drives a UI? Comparison with skirmish (Q2)

**No.** The UI is driven by display actions, not by executing commands.
- On a replica, a command's execute runs the same shared rules/state code. That code calls
  `addDisplayAction*` itself, and the replica branch posts it because `p == local`.
- On the master, the same rules code runs directly, without commands, and calls the same
  `addDisplayAction*`.
- **Skirmish** (`EQMainController`, `+0x304 = 1`) never executes or sends its own commands either.
  `sendToPlayer` `0x100F1680` takes the `(+0x1C4 || +0x304)` branch and calls the command's deleting dtor.
  Its UI works because `0x100E1FD0` takes the single-player branch.
- What skirmish has that our host lacks: only that `+0x304` branch. The UI-side differences are the
  ~30 readers of the `+0x304` getter `0x100CEE80`, e.g. `0x10276E7A`, `0x1027F47B`, `0x102B7AA6`,
  `0x10414720`, `0x10415A16`, `0x1041B3A8`. With `+0x304 = 0` they give network-game behaviour (wanted on
  the host). The screen code (0x10270000-0x102C0000, 0x10410000-0x10420000) has **no** isServer reads:
  every caller of `0x100D0000` is in rules or EQGame code. UNVERIFIED: whether any of those 30 sites hides
  a needed widget when `+0x304 = 0`. On a replica they behave the same, so it is unlikely.

**Executing outbox[1] locally would be wrong (double-apply), not just risky.** The master has already
applied every state change. Executing the copies would re-run them, for example:
- CardSelected execute `0x101026B0` → vtable+0xCC, which injects the input a second time;
- SelectDeckForPlayer → `Game::selectDeck` again;
- StateSpecificMessage execute `0x1010C3A0` → vtable+0xF4 → state input 0xD again. With fromNet = 1 this
  **throws** (`_CxxThrowException` in `0x100F8FE0`) if the state rejects it.

The base execute `0x100FEE90` only skips the element map when isServer, so it does not protect against
any of this. Keep HostFlush deleting `outbox[1]`. That matches skirmish.

## 3. Why RemovePlayer showed nothing (Q3)

World's `RemovePlayer(m, m.guest)` (lon_service.cpp:3138: `Header(0x10B,4,3) U(account) U(G) U(Stamp()) U(0) U(0) U(2)`)
is well formed. Execute `0x10108820` → vtable+0xB0 `0x100F7A10`:
`getPlayerByAccount(<guest account>)` (`0x100D91A0`) → `gameLostByPlayer(seat 2, 2)` (EQ `0x1026F8B0`).
- The gate `+0x108` (GameStarted) is 1, so it proceeds. Teams `{1:1,2:2}` make bVar1 true, and it posts
  display action 0x45 (loss type 2) through `0x100E1F20`. It then removes seat 2 from the alive list
  (MatchStructure 1) and calls end-of-game `0x1026ED90`, which posts 0x170 through `0x100E1F20` at
  `0x1026F02C..0x1026F03B` among others.
- **It did not draw anything because the screen was never bound** (`+0x5C == 0`, section 1.3): the filter
  dropped 0x45/0x170/0x66. It was not ignored for being "not started", and it is not wrong for a master.
  `gameLostByPlayer` on the master is the stock path; the master's own no-action timeout uses the same one
  (`0x100F3A30`: builds RemovePlayer, `sendToAll`, then vtable+0xAC).
- "Host sent nothing more": the end routine only sends to the remaining players. Seat 2 is now dead and
  seat 1 is ourselves (deleted). UNVERIFIED in detail: `0x1026ED90` contains no direct `sendTo*` call;
  any EndGame/GameOverReached would go to alive seats only.
- To confirm the execute ran, the host's LoN game log should contain
  `"GAME LOST BY PLAYER: 2, 2"` (`0x1026F8B0`). Optional check.

After the fix in section 4, the same RemovePlayer should show the result.

## 4. Fix (Q4)

### P4 (new, host DLL): let a master post its own player's display actions

| # | Where (RVA) | Original bytes (check all 9) | New bytes |
|---|---|---|---|
| P4 | `0x0E2058` .. `0x0E2060` (`cmp byte [esi+0x139],0` ; `jne +0x23`) | `80 BE 39 01 00 00 00 75 23` | write only the last 2: `90 90` at RVA `0x0E205F` |

Optional stricter context check (all from the dump): RVA `0x0E1FD0` begins with
`56 8B F1 80 BE 78 03 00 00 00 75 20`, and RVA `0x0E2030..0x0E2060` reads
`53 57 8B CE FF D2 84 C0 74 5E 80 BE 04 03 00 00 00 74 15 80 BE C4 01 00 00 00 75 0C 85 FF 74 26 3B BE 28 01 00 00 74 1E 80 BE 39 01 00 00 00 75 23`.

Behaviour after the patch, for every game type:

| Game | +0x139 | +0x304 | Before | After |
|---|---|---|---|---|
| Replica (guest, live network) | 0 | 0 | post if p is 0 or me, else delete | **same** (the patched jump is never taken) |
| Skirmish / scenario / tutorial (EQMainController) | 1 | 1 | branch 1 posts p 0/me; others fall to `jne` → delete | others now reach the replica test (p 0/me already failed in branch 1, or +0x1C4 set) → delete. **Same** |
| Our host EQGame | 1 | 0 | always delete | post if p is 0 or me (= skirmish behaviour), else delete |

So the patch is stock-equivalent for every game the shipped client runs. It affects only our host. It
also needs no per-game test, which is why it can be a code patch rather than a vtable slot (the function
is called directly, not through a vtable).

Code shape (same install rules as spec 1.3: per LoN load, all-or-none with P1-P3, reset on module change):
```cpp
constexpr DWORD kPvPDispJne = 0x0E205F;   // Game::addDisplayActionForPlayer 0x100E1FD0: "isServer -> delete"
static const BYTE kDispCtx[9] = {0x80,0xBE,0x39,0x01,0x00,0x00,0x00,0x75,0x23};   // at RVA 0x0E2058

bool PvPPatchDisplay(BYTE *lon, DWORD size)
{
    BYTE *p = lon + 0x0E2058;
    if (0x0E2058 + sizeof(kDispCtx) > size) return false;
    if (memcmp(p, kDispCtx, sizeof(kDispCtx)) != 0)
        return p[7] == 0x90 && p[8] == 0x90 && memcmp(p, kDispCtx, 7) == 0;   // already patched this load
    DWORD old;
    // .text: must stay EXECUTABLE. PAGE_READWRITE here (as used for the data writes at nms_lon_link.cpp:66/91)
    // would make the page non-executable and crash on the next call.
    if (!VirtualProtect(p + 7, 2, PAGE_EXECUTE_READWRITE, &old)) return false;
    p[7] = 0x90; p[8] = 0x90;
    VirtualProtect(p + 7, 2, old, &old);
    FlushInstructionCache(GetCurrentProcess(), p + 7, 2);
    return true;
}
```
Install it in `PvPHostPulse` next to P1-P3 (nms_lon_link.cpp:388-401): run the byte check together with
the three slot checks, and if any check fails install nothing. Log "P4 installed".

Crash-risk notes:
- **Thread safety.** The write is 2 bytes inside one instruction pair, done while LoN loads. It runs on the
  EQ main thread (NMSLoNPulse), the same thread that runs the UI pump, so no thread can be halfway through
  `0x100E205F`.
- **Wrong build.** The 9-byte context check fails → nothing is installed (the P1-P3 rule).
- **What the host UI now receives.** The display actions a skirmish master already receives for its local
  seat and for p = 0. Seat-2-only actions (`p == 2`) are still deleted, so the host does not see the
  guest's hidden cards in the UI. It can still read them in memory (spec section 5).
- **No double display.** The host never executes `outbox[1]` (HostFlush deletes it), so each action
  is posted once.
- **Remaining unknowns (T1 retest settles them).**
  - Other master-only UI gates may exist. Only `0x100E1FD0` was proven to block, and it blocks 0x60,
    which gates everything else.
  - Watch the LoN log for the eqgamescreen.cpp 0x2756 `"false"` assert (base count outside 2-5).

### Alternative (vtable-slot only, not recommended)

Replace EQGame vtable+0x8C (slot RVA `0x64139C + 0x8C = 0x641428`, original `0x102721C0`). For
`game == g_host && (p == 0 || p == game+0x128)`, call `0x100E1F20(game, a)` yourself and return false (on
false `0x100E1FD0` returns without deleting, `0x100E2038 → 0x100E2098`). But `0x100E1F20` calls vtable+0x8C
again with p = local, so this needs a re-entrancy flag, and the ownership rules are more fragile. P4 is
simpler and provably stock-equivalent.

### Not needed

- No world change. The start sequence is right; 0x60 was produced and then deleted on the host.
- No change to HostFlush. Keep `outbox[1]` deleted.
- No extra UI event. 0x60 must come from setupGame with the real gameID and local player; synthesizing
  one is pointless once P4 lets the original through.

## 5. Other findings worth keeping

- Class ids from the host stream (vtable getId slots +0x18/+0x1C returning the constant):
  - 0x38 IntroduceCard (vtable `0x1060CB8C`)
  - 0x3A SelectDeckForPlayer (`0x1060DC5C`)
  - 0x3C CardSelected (`0x1060B92C`)
  - 0x3F StateSpecificMessage (`0x1060E4C4`)
  - 0x49 SynchPoint (`0x1060E76C`)
  - 0x75 ReadyForStartOfGame (`0x1060D5E4`)
  - 0x10B RemovePlayer (`0x1060D734`)
  - 0x8B LostPlayer (`0x1060CD7C`)
  - 0x17F GameOverReached (`0x1060CA54`)
  - 0x1D7 EndGame (`0x1060C1E4`)

  So the master reached the mulligan step (CardSelected avatars x4, SynchPoint, SSM p1, the 6-card hand
  introduced to seat 2, SSM p2) and is waiting for input. That is correct.
- **Exception risk for T3 (not a T1 issue).** `Game::buttonPressed` base `0x100F7090` and
  `stateSpecificMessage` `0x100F8FE0` call `_CxxThrowException` when the state rejects input and the
  fromNetwork flag is 1. A stale or mistimed guest ButtonPressed or SSM therefore throws on the host.
  UNVERIFIED: whether something up the stack (`0x101740E0` receive loop) catches it. Keep world's
  drop-list strict and watch T3 for this.
- Local display pause: WAEventSink `+0x10`/`+0x14`, with pause `0x104049D0`, resume (timer) `0x10404930`
  and wait-for-type `0x104048A0`. It throttles the drain loop (`0x103F7389`). It is not involved here.

## 6. Retest (T1 again, after P4)

1. Expect: the board draws, the host gets the mulligan dialog for seat 1, and the seat-2 stream is
   unchanged.
2. RemovePlayer at +120 s: the host shows the win (display 0x45 / 0x66 / 0x170 path). Exit works and P3
   logs.
3. Skirmish once: unchanged (stock-equivalent proof in section 4).

(Correction after the retest: item 2 crashed. See section 7. Do not send LossType 2.)

---------------------------------------------------------------------------------------------------------------

## 7. Follow-up after P4 (board works): crash at RemovePlayer, and the "You draw" leak

Static RE only; nothing was edited. The crash stack is taken from an observed crash report (VAs at base 0x10000000).

### 7.1 Which handler crashed, and why `base+0x1E0` is NULL (Q1)

The chain, in order:
- World's `RemovePlayer` carries LossType **2** (lon_service.cpp:3141, `kLossDisconnect`).
- Execute `0x10108820` → `0x100F7A10` → `gameLostByPlayer(2, 2)` `0x1026F8B0`.
- `0x1026F8B0` takes the **`param_3 == 2` branch**, which posts display action **0x45** `{gameID, player 2, 0}`
  through `addDisplayAction` `0x100E1F20`. That function has no player gate, so the action was already posted
  before P4, but the unbound screen dropped it.
- Now the screen is bound. The action takes this path:
  1. EQGS filter `0x1027CEA0` (tail `0x1027F00C/13`);
  2. WAGS filter `0x10415A80` (`0x10415B5C`);
  3. EQGS `+0x118` `0x102A97D0` (default → `0x102A9FB2`);
  4. WAGS `0x1041D910`, case 0x45 at `0x1041DA5D` → `0x10416270`;
  5. `vtable+0x12C` = EQGS **`0x10281160`** (the "update player timer" handler), called with player **2** and 0 s.
- `0x10281160` finds the base for player 2 and writes `mm:ss` through `*(base+0x1E0)` (`QString::operator=` at
  `0x10281539..0x10281546`). That pointer is NULL, so EQ crashes with the WRITE to 0.

Who creates `base+0x1E0`:
- **Only** display action **0x44** ("timer init"): WAGS case 0x44 `0x10416220` → `vtable+0x128` = EQGS
  `0x10280A20`, which allocates the QString and stores it at `0x10280BCA` (`mov [eax+0x1E0], ecx`). It is the
  only writer of a base's `+0x1E0` in the screen code; `0x1028DE75` writes a screen field, not a base.
- The base itself is zero-filled at creation (`0x102AA0A0`: `memset(...,0,0x1E4)` + copy).
- 0x44 is posted only when the game has a turn timer (Duration `Game+0x230 > 0`):
  - EQ setupGame `0x1026800E` (`cmp [ebx+0x230],0 ; jle` skips the 0x44 post);
  - base setupGame `0x100F3260` (`if (param_1[0x8C] > 0)`);
  - the observer resync `0x10270340` (`if (0 < +0x230)`, which posts 0x44 and then 0x45).

We send **Duration 0** (spec 2.3, field 14), so no base has timer items. In stock code 0x45 always follows 0x44
(timer tick `0x100F3E90` needs `+0x230 != 0`). The only 0x45 not tied to a timer is
**`gameLostByPlayer(..., lossType 2)`** ("time ran out"). LossType 2 with Duration 0 is a combination stock never
produced, and it dereferences the missing timer text.

Not related to P4's "seat 2 deleted" behaviour, and not something only replicas receive: it is a property of
LossType 2.

### 7.2 Real games, and the smallest safe fix (Q2)

**A real two-client game would hit the same crash wherever world sends LossType 2:**
- guest disconnects → host gets `RemovePlayer(guest, 2)` → crash on the host (identical to this test);
- host disconnects with the optional `RemovePlayer(host, 2)` to the guest (spec 3.5) → the replica binds its
  screen the same way (0x60 with `p == local`), runs `gameLostByPlayer(1, 2)`, and crashes the guest.

**Turn timers cannot hit it with Duration 0:**
- no 0x44/0x45 from setupGame, the timer tick `0x100F3E90` or the resync `0x10270340`;
- the no-action timeout `0x100F3A30` needs `Game+0x26C != 0`, and its only setter `0x100D6180` has no references
  (the ctor writes 0 at `0x100FB16F`), so it never fires.

**Normal endings are safe:**
- concede → `Game::concede` `0x100F8C50` → `vtable+0xAC(p, 1)`; LossType 1/4 posts **0xBC** (`0x1026F8B0`),
  and the WAGS handler `0x10417470` → `vtable+0x148` = `0x102B5380` only removes that player's cards; it never
  touches the timer items;
- rules wins go through the same `0x1026ED90` end path, with no 0x45.

**Fix (world only, no client change): never send LossType 2 while Duration is 0.** Send **LossType 1** in every
world-built RemovePlayer (GM test timer, guest disconnect, optional host-disconnect follow-up):
- `lon_service.cpp:3141` `.U(kLossDisconnect)` → a constant **1** (rename to e.g. `kLossConcede = 1`). Callers
  are at :3194, :3386 and :3519.
- LossType 1 is exactly what a stock concede produces, so the end-of-game path (0xBC, removal from the alive
  list, `0x1026ED90`) is exercised by every shipped game.
- The cost is cosmetic: the result reads as a concession, not a time-out.
- LossType 0/3 also skip the 0x45 post, but no stock code path is known to produce them. Use 1.
- Alternatives, rejected:
  - a positive Duration: it would turn on real turn timers (`0x100F3E90` ends the game on time-up);
  - a client null-check patch: more code for no gain;
  - a world-forged `ConcedeGame{PlayerID 2}` to the host: same LossType 1 result, but it also makes the master
    broadcast a ConcedeGame. RemovePlayer(…, 1) is simpler.
- Update spec 3.5 and U5: **LossType 2 is unsafe when Duration = 0.**

### 7.3 The "You draw …" leak (Q3)

The text comes from DrawCardState `0x100B3CF0`. The loop `0x100B3EF8..0x100B4086` runs over all players:
- for the drawing player it builds `"You draw [*cardList*]."` (`0x10604E94`, at `0x100B3F66`);
- for the others it builds `"#actor() #does([:DRAW:]) …"`;
- each text then goes to `0x100E2490(player, text)`.

That function → `0x100E2310` posts a **text display action 0x6A** whose gate is the opposite of `0x100E1FD0`:
```
if (isServer && +0x304 != 1) goto post_for_ANY_player;   // a non-single-player server keeps every player's text
if (p != local) skip;
post 0x6A via 0x100E1F20
```

Four text functions share this gate (the second is the varargs Game vtable slot `+0x188`, at `0x1060A374`,
`0x10641524` and `0x10647EAC`):

| Function | `jne post` (RVA) | Original bytes (from RVA) |
|---|---|---|
| `0x100E2310` (wrapper `0x100E2490`; 32 + 53 callers) | `0x0E234F` = `75 30` | `0x0E233E`: `8A 86 39 01 00 00 84 C0 74 09 80 BE 04 03 00 00 01 75 30` |
| `0x100E2760` | `0x0E27A0` = `75 2C` | `0x0E278F`: `8A 86 39 01 00 00 84 C0 74 09 80 BE 04 03 00 00 01 75 2C` |
| `0x100E24B0` (3 callers) | `0x0E24FA` = `75 2C` | `0x0E24E9`: `8A 87 39 01 00 00 84 C0 74 09 80 BF 04 03 00 00 01 75 2C` |
| `0x100E2600` (wrapper `0x100E2740`) | `0x0E2643` = `75 2C` | `0x0E262E`: `8A 86 39 01 00 00 84 C0 8B 6C 24 20 74 09 80 BE 04 03 00 00 01 75 2C` |

So on our host every player's text is queued, including seat 2's "You draw <card names>".
- **It is a UI leak, not only a log line:** 0x6A goes to the bound screen (WAGS `0x1041D910` case 0x6A →
  `vtable+0x170`, the game-text panel). Before P4 it was queued too, but the unbound screen dropped it.
- UNVERIFIED which widget shows it (game log/chat panel vs the `wa_game` log file only). To check: whether the
  in-game text panel shows the second "You draw" line.

**Fix P5 (host DLL, same style as P4):** NOP the four `jne post` with `90 90`, after checking the context bytes
in the table. Then every game type applies the `p == local` test:
- replica: the `je` at +8 already skips the jne → unchanged;
- skirmish (`+0x304 == 1`): the jne is not taken → unchanged;
- host: now only its own and player-0 text.

Write with `PAGE_EXECUTE_READWRITE` and `FlushInstructionCache`, all-or-none with P1-P4.
- Remaining hidden-info exposure: the host still holds the guest's cards in memory (spec 5), and only these text
  paths are verified.
- UNVERIFIED whether the board renders seat 2's hand face-up on the host. That depends on card visibility flags,
  not on these gates. The "Unknown Account draws 6 cards" line suggests normal opponent rendering. To check:
  the guest's hand area during T1.

### 7.4 Addresses added in this pass

| What | VA |
|---|---|
| gameLostByPlayer lossType-2 → display 0x45 / lossType 1,4 → 0xBC | 0x1026F8B0 |
| Timer update handler (crash) / timer init handler (creates base+0x1E0 at 0x10280BCA) | 0x10281160 / 0x10280A20 |
| WAGS display switch cases 0x44 / 0x45 / 0xBC | 0x10416220 / 0x10416270 / 0x10417470 |
| Timer tick (needs Duration) / no-action timeout (needs +0x26C; setter 0x100D6180 unreferenced) | 0x100F3E90 / 0x100F3A30 |
| DrawCardState text builder ("You draw [*cardList*].") | 0x100B3CF0 (loop 0x100B3EF8) |
| Per-player text display (0x6A) with the "server posts all" gate | 0x100E2310, 0x100E24B0, 0x100E2600, 0x100E2760 (vt+0x188) |
