# Legends of Norrath live player-to-player trade: wire spec, checked against the client

Researched 2026-10-06. Static RE only: LegendsOfNorrath.dll in Ghidra (base 0x10000000), read-only.
Nothing was renamed or run. Every server-sent field below was checked in the client's **read**
function and its **execute/UI handler**. Every client-sent layout was checked in the **sender** (ctor
plus setters) and in the read, which mirrors the write. Each class's version was read from its
GetVersion slot (vtable+0x20). This file supersedes `research/lon_direct_trade.md` sections 2-6. That file's
leads are corrected in section 0.

Wire primitives, `Header(cls, ver, levels)`, the varint and the strings are those of
`lon_service.cpp` and `research/lon_trade_lobby_spec.md` section 0.

## 0. Corrections and new findings compared with `research/lon_direct_trade.md`

1. **Order matters at session start. The old doc's order (Accept → StartTradeSync → sync) leaves A's
   "waiting" dialog orphaned.** When the trade screen opens, the client calls `0x104BE7B0`, which
   **wipes every WATradeManager map**: the dialog-by-S map, partner, founder, L→S and allowed. After
   that, nothing can close a request dialog. So StartTradeSync (which closes A's dialog), the
   cancellation of all other pending requests, and SetTradeAllowed must all go out **before**
   AcceptTradeRequest. See section 2.3.
2. **SetTradeAllowed is copied into the screen only once, at screen creation** (`0x104BEA10` →
   `0x104BE880` + `0x104C1BD0`). If it is sent after AcceptTradeRequest, it has no effect.
3. **Every chat line from the trade executes dereferences the client's current lobby with no null
   check** (`FUN_10068550()` → `[lobby+0x20]`). This applies to Issue, Accept, Cancel, Add and
   Remove when both accounts are cached, and to **StartTradeSync always**. An access violation
   ends EverQuest. Invariant: **only send trade messages to a session that joined group 3 on this
   connection.** The Join echo sets the current lobby through `FUN_10068560`.
4. **ClearTrade(X) clears only the cards OWNED by X** (`0x104C0680`: if X is the partner it resets
   the partner's pile and model, otherwise the own pile and model), and it unchecks both accept
   buttons.
5. **Founder/Trader list order is now proven.** Founder Card List = cards owned by the founder
   (the issuer, A). Trader Card List = cards owned by B. The same message goes to both: each client
   swaps the lists itself according to `me == founder` (`0x104C1670`, disassembly
   `104c1757..104c1826`).
6. **InstanceID equality compares archetype, series AND number** (`FUN_1012B7C0`).
   AddCardToTrade and RemoveCardFromTrade carry no series, so the client compares with series 0.
   The collection, the type-3 sync and SetupFinal must send **series 0** for every tradable card.
   Our `lon_card.series` defaults to 0.
7. **The trade lobby id L must not be a real group, and must never be 3.** On close, the screen
   dtor sends `Leave(L)`. With L = 3 it would leave the trade lobby right after the Join(3) that
   brought the player back. See section 3.
8. **Closing a request or confirm dialog from the server side probably makes the client send a
   spurious Cancel or FinalAccept(false).** The dialogs are QDialogs whose `rejected()` is wired
   straight to the senders (`0x104E61D0` slot 0 = `0x104BC050` Cancel; confirm dialog `rejected()` →
   `cancelFinalAccept` → `0x104BFB20`). Qt4 `QDialog::closeEvent` calls `reject()` when the dialog
   is visible. The server must ignore a Cancel whose session is not in the Requested state, and a
   FinalAccept(false) when the session is not in the Final state.
9. **Leaving with Esc sends `LobbyCommand_Join{3}` first**, not EndTradeSession (`returnToLobby`
   = metacall case 0x18 → `0x104BFC30`). EndTradeSession and Leave(L) come later, from the screen
   dtor, once the Join echo has replaced the screen. The server must answer that Join (echo and
   BulkJoin) or the player stays on the trade screen.
10. **NetworkCommand_ErrorMsg never disconnects.** Code 0 shows the message string in an "Error"
    box (`0x1045EDD0`). A non-zero code shows the factory text (`0x1045EEC0` → `FUN_102C7E20`; 0x5B
    has a longer store text).
11. A request dialog is only created by the server's Issue echo. Rejecting with ErrorMsg alone
    leaves no client state behind.

---

## 1. Byte encodings (all verified)

Varint quick table (`U(v)`): v < 16 → 1 byte `v`. For 16 ≤ v < 4096 → `0x20|(v&0xF), v>>4`.

| Class | Ver | Levels | Header bytes (×3 levels) |
|---|---|---|---|
| 0x93 IssueTradeRequest | **2** | 3 | `23 09 02` |
| 0x94 AcceptTradeRequest | 1 | 3 | `24 09 01` |
| 0x95 AddCardToTrade | 1 | 3 | `25 09 01` |
| 0x96 AcceptTrade | 1 | 3 | `26 09 01` |
| 0x97 RemoveCardFromTrade | **3** | 3 | `27 09 03` |
| 0x98 CancelTradeRequest | **2** | 3 | `28 09 02` |
| 0x99 EndTradeSession | 1 | 3 | `29 09 01` |
| 0x9A ClearTrade | 1 | 3 | `2A 09 01` |
| 0x9B FinalAcceptTrade | 1 | 3 | `2B 09 01` |
| 0x9C SetupFinalTradeAccept | **2** | 3 | `2C 09 02` |
| 0x9D ClearAcceptState | 1 | 3 | `2D 09 01` |
| 0x110 StartTradeSync | 1 | 3 | `20 11 01` |
| 0x111 LockTrade | 1 | 3 | `21 11 01` |
| 0x15C SetTradeAllowed | 1 | 3 | `2C 15 01` |
| 0x6B NetworkCommand_ErrorMsg | **2** | **2** | `2B 06 02` (×2) |
| 0x15E InstanceID (inline object) | 1 | 1 | `2E 15 01` |
| 0x54 / 0x55 / 0x56 Sync Start/Send/Complete | 2 | 3 | `24 05 02` / `25 05 02` / `26 05 02` |

Levels: the leaf read calls vtable+0x10 (header 1). `FUN_101B9CC0` (TradeCommand read) calls
vtable+0x10 (header 2), then `FUN_1016C670` (NetworkCommand: header 3, no fields). It then reads
**AccountID, TradeSessionID**. So every TradeCommand is:
`Header(cls,ver,3) U(AccountID) U(TradeSessionID) <leaf fields>`.
Versions were confirmed at vtable+0x20 (`MOV EAX,n`) for every class in the table.

A bool is read as an int (`FUN_10097620`, then `!= 0`): send `U(0)` or `U(1)`.

InstanceID (`FUN_1012B820`, vtable `0x10611934`): `Header(0x15E,1,1) U(arch) U(series) U(number)`
= `Writer::Instance`. Inside a vector it is **inline with no null flag** (`FUN_10077790`: `U(count)`,
then per element the object's own Read through vtable+0x24).

Examples below use A = 5, B = 7, S = 1, L = 1000001 (`U` = `41 24 F4`), arch 1234 (`22 4D`) and
card id 99 (`23 06`).

---

## 2. Message-by-message spec

### 2.1 IssueTradeRequest 0x93 v2
- **Read** `0x101BE120`: AccountID, Session, IssueTo (+0xC), `ver>1`: GroupID (+0x10).
- **Client A sends** (`0x104BC500`, ctor `0x101BDBF0`): AccountID = self, Session = 0,
  IssueTo = B (`0x101BDCD0` → +0xC), GroupID = the current lobby GroupID (`0x101BDCF0` → +0x10; it
  stays 0 if there is no current lobby). Before sending, the client refuses locally when B's
  property 0x1012 (ignore list) contains A.
  `[12-byte prefix] 23 09 02 23 09 02 23 09 02 05 00 07 03`
- **Server sends to A and to B** (identical): `23 09 02 ×3 U(A) U(S) U(B) U(3)`
  → `23 09 02 23 09 02 23 09 02 05 01 07 03`
  - Rewrite: Session = S (new, non-zero, unique for the server lifetime). AccountID = A (from the
    session, never the client field). GroupID = 3.
- **Execute** `0x101BDD40`: posts event 0x21(S, AccountID, IssueTo). GroupID is ignored. If both
  accounts are cached it also prints a chat line through the current lobby (null-deref risk,
  section 0 item 3): A sees "You issue a trade request to B", B sees "#Actor asks you to trade".
- **UI** `0x104BD2E0`: if AccountID == me, a waiting dialog "Please wait while #actor considers
  your trade request" with Cancel. Otherwise an invite "#Actor has invited you to a trade
  session. Would you like to join?" (Yes/No). Both are keyed by S and partner. Yes → `accepted()` →
  `0x104BE410`. No or Cancel → `rejected()` → `0x104BC050` (slot table `0x104E61D0`).

### 2.2 CancelTradeRequest 0x98 v2
- **Read** `0x101BC550`: AccountID, Session, IssuedFrom (+0xC), `ver>1`: OriginAccountID (+0x10).
- **Client sends** (`0x104BC050`, ctor `0x101BBF60`): AccountID = self, Session = S,
  IssuedFrom = 0, Origin = 0. `[prefix] 28 09 02 ×3 U(self) U(S) 00 00`.
  It is also sent by `0x104BE210` for every **other** pending request when the player accepts one,
  and possibly by a dialog closing (section 0 item 8).
- **Server sends to A and B**: `28 09 02 ×3 U(B) U(S) U(A) U(origin)`
  - AccountID = **B (target)**, IssuedFrom = **A (issuer)**, Origin = who cancelled (A, B, or 0
    for a timeout or the server).
- **Execute** `0x101BC080`: event 0x23(S, IssuedFrom, **true**). Chat text (both accounts must be
  cached, current lobby deref):
  - me == IssuedFrom and Origin == me: "You cancel your trade request to [B]."
  - me == IssuedFrom and Origin != me: "#Actor(B) cancels your trade request."
  - me != IssuedFrom and Origin == me: "You cancel #owns(A) offer to trade."
  - otherwise: "#Actor(A) cancels the trade request."
- **UI** `0x104BE490`: closes the dialog for S. If the flag is true, `0x104BE090` erases S from
  all maps.

### 2.3 Session start: StartTradeSync, SetTradeAllowed, partner collection, AcceptTradeRequest

**Client B accepts** (`0x104BE410`, ctor `0x101BA6A0`): `AcceptTradeRequest{AccountID = B,
Session = S, IssuedFrom = 0, L = 0}` → `[prefix] 24 09 01 ×3 07 01 00 00`. It then sends
CancelTradeRequest for every other request it has open.

**Server, in this order** (each step's reason is in its bullet):

1. **Cancel every other Requested session that involves A or B.** Send the Cancel to both parties
   of each one. This must happen before step 5, because at step 5 the dialog maps are wiped.
2. **StartTradeSync 0x110 v1 to A and B**: `20 11 01 ×3 U(B) U(S) U(A)`
   → `20 11 01 20 11 01 20 11 01 07 01 05`
   - Read `0x101C1700`: AccountID, Session, IssuedFrom (+0xC).
   - Execute `0x101C1250`: event 0x23(S, IssuedFrom, **false**) closes A's waiting dialog without
     dropping S. Chat: "#Actor accepts your trade request" / "You accept #owns(A) offer". Then,
     **unconditionally**: "Please wait while collections are synchronized." through
     `[currentLobby+0x20]`. **A recipient with no current lobby crashes here.**
   - Expect a spurious `CancelTradeRequest(S)` from A when the dialog closes. Ignore it (the state
     is no longer Requested).
3. **SetTradeAllowed 0x15C v1** (optional; a missing entry means allowed, `0x104C10B0` returns 1):
   `2C 15 01 ×3 U(recipient) U(S) U(n) {U(account) U(0|1)}×n`
   → for example `2C 15 01 2C 15 01 2C 15 01 05 01 02 05 01 07 01`
   - Read `0x101C02B0` → `FUN_101C00D0`: `U(count)`, then pairs of `U(key) U(value)`. Value ≠ 0 → bool.
   - Execute `0x101BFC50`: event 0x5A(S, account, bool) per entry → `0x104BEB60` stores
     manager[S][account]. **Copied into the screen only at creation**, so it must come before
     step 5.
   - Value 0 for self gives "You may not trade any cards away."; for the partner, "%1 may not
     trade any cards away." (client-side add refusal `0x104C2B80`). Use it for trade-banned
     accounts, and still enforce on the server.
4. **Partner collection (type 3) to each side**. To A, B's tradable cards; to B, A's.
   ```
   StartInstances 0x54 v2: 24 05 02 ×3 U(partner) U(3) U(archetypeCount) U(0)
   SendInstances  0x55 v2: 25 05 02 ×3 U(base) U(count) U(nT) {2E 15 01 U(arch) U(0) U(id)}×nT  U(0)  U(0)
                                                        (tradable list)                 (non-tradable list: 0) (DeliveryID 0)
   Complete       0x56 v2: 26 05 02 ×3 U(0)
   ```
   - Reads: StartInstances `0x101B2530` (Sync base `0x101AE7D0` has headers only, no fields; then
     AccountID, Type, ArchetypeCount, `ver>1` ModID). SendInstances `0x101B0C00` (+8, +0xC,
     vector +0x10, `ver≥2` vector +0x24, then `U` → +4). These are the same layouts
     `SendCollection` already uses live, with Type = 3 and the AccountID of the **partner**.
   - `0x101B33C0` → `0x101B2A60(partner, 3)`: key `"%dt"` and name "%s's Trade Collection". If the
     collection already exists (an earlier trade with the same partner) it is reused and
     **cleared** (`FUN_100724B0`), so a re-sync is safe. No assert on this path (the `mCollection`
     assert is unreachable because the collection is created first).
   - The client answers StartInstances with `SynchronizationCommand_Update` 0x57 (`0x101B2DF0`).
     The existing handler ignores it once `s.synced` is set. Keep it that way.
   - Complete `0x101B2C90` for type 3 posts 0x1D and **0x36(key)**. When the screen exists,
     `0x104C0A10` refreshes the models.
   - Content: every `lon_card` of the partner with `tradable=1`, not in posted-trade escrow, not
     locked in another live trade, **series 0**. Optionally, minus the partner's Want count
     (SetDesired, section 2.13). The trade screen's "Theirs" model is keyed `"%dt"` with the
     partner id (`0x104BFA00`). The own model is `"%d"` = the normal own collection, so do **not**
     send the player's own collection here.
   - The own collection is unaffected. RemoveInstances (`0x101B3C50`) and StartDelivery
     (`0x101B4C50`) switch the sync service back to `(self, 1)` by themselves (verified).
5. **AcceptTradeRequest 0x94 v1 to A and B**: `24 09 01 ×3 U(B) U(S) U(A) U(L)`
   → `24 09 01 24 09 01 24 09 01 07 01 05 41 24 F4`
   - Read `0x101BABB0`: AccountID, Session, IssuedFrom (+0xC), TradeLobbyId (+0x10). No version
     gate.
   - Execute `0x101BA7C0`: event 0x22(S, IssuedFrom, AccountID, L). Chat (both accounts cached;
     current lobby deref).
   - UI `0x104BD700`: founder = IssuedFrom, accepter = AccountID; partner = the other of the two
     relative to me. A beep on the A side. Then `0x104BD270(L, S)` (a duplicate L only logs
     "Overwriting existing session value for lobbyID" and may keep the **old** S, so **L must be
     unique**), `0x104BCF80(S, partner)`, `0x104BD040(S, founder)`, and
     **`FUN_1045FE90(6, L, 1)`, which opens the trade screen unconditionally**. Never send it to
     anyone but A and B.
   - Screen open (`0x1045DE50` case 6): the client sends `ChangeStatus 7`. The ctor is
     `0x104C1950`. Then `setGroup(L)` (+0x24), `0x104BEA10` (binds S = map[L], partner → +0x30,
     founder → +0x34, copies the allowed map), setup `0x104C3090` (chat widget group = L; own
     collection key `"%d"`, theirs `"%dt"`, default tab "Theirs"; "Trade Session ID: #S"), then
     `0x104BE7B0` **wipes all manager maps**.

### 2.4 AddCardToTrade 0x95 v1
- **Read** `0x101BB490`: AccountID, Session, arch (+0x10), instance number (+0x18),
  OriginAccountID (+0x1C). The InstanceID's series (+0x14) is **not on the wire** and stays 0 (the
  ctor `0x101BAD40` sets it with `FUN_1012B910`).
- **Client X sends** (`0x104C0FC0`; blocked if screen +0x38 is set, which is a read-only flag
  never set in a normal trade): one message **per copy**. AccountID = **card owner** (`0x101B9B20`
  → +4; X may add the partner's card to request it), Session = S, arch (`0x101BAEC0`), instance =
  `lon_card.id` (`0x101BAED0`), Origin = 0.
  `[prefix] 25 09 01 ×3 U(owner) U(S) U(arch) U(id) 00`
- The client does **not** move the card locally. Pre-checks in `0x104C2B80`: lock (+0x39), the
  allowed map, the want count.
- **Server echo to A and B** (on success only): `25 09 01 ×3 U(owner) U(S) U(arch) U(id) U(X)`
  → A adds own card 99: `25 09 01 25 09 01 25 09 01 05 01 22 4D 23 06 05`
  - Rewrite: Origin = X (the sender's session account). Owner, arch and id are the validated
    values.
- **Execute** `0x101BAF10`: event 0x24(S, Origin, InstanceID{arch,0,id}, 0, owner). Origin not
  cached → log only. Unknown arch → no chat. Chat goes through the current lobby.
- **UI** `0x104C0310`: S must equal screen+0x28. If owner == me, the card moves from the own model
  (+0x48) to the "they get" pile (+0x54); otherwise from the theirs model (+0x4C) to the "you get"
  pile (+0x50). A missing instance in the model only gives `qWarning` (`0x104BA4D0`).
- **Server validation** (all of it, in this order): S is Open (not Final or Done); X ∈ {A, B};
  owner ∈ {A, B};
  `SELECT archetype, series, tradable FROM lon_card WHERE id=? AND account_id=owner` gives
  archetype == arch and tradable = 1 (else **0x2E**); not in posted-trade escrow and not in
  `g_card_lock` (else **0x22**); fewer than 50 cards on the owner's side (else **0x23**); the
  partner's card passes the same filter as the type-3 list. On success: lock, add, echo. If
  anyone had accepted: **ClearAcceptState** to both and reset both accept flags.
- On failure: ErrorMsg to X only. The client moved nothing. Suppress repeated identical errors
  within one burst, because the client sends N adds for "Add N".

### 2.5 RemoveCardFromTrade 0x97 v3
- **Read** `0x101BF380`: AccountID, Session, arch (+0x10), instance (+0x18), `ver>1`: Owner
  (+0x1C), `ver>2`: ErrorCode (+0x20).
- **Client X sends** (`0x104C0ED0`, ctor `0x101BEBD0`): AccountID = **self**, S, arch, id,
  Owner = card owner (`0x101BED90`), ErrorCode = 0.
  `[prefix] 27 09 03 ×3 U(X) U(S) U(arch) U(id) U(owner) 00`
- **Server echo to A and B**: `27 09 03 ×3 U(X) U(S) U(arch) U(id) U(owner) U(0)`
- **Server-forced removal** (for example a card that became invalid):
  `27 09 03 ×3 U(owner) U(S) U(arch) U(id) U(owner) U(1)`. The client prints "[*target*] is
  removed from the trade because an error occurred."
- **Execute** `0x101BEDD0`: event 0x25(S, AccountID, InstanceID, Owner). **UI** `0x104C0410`
  moves the card back **only if it is in the pile** (`0x104BA350`), so a stale remove is harmless.
- Server: act only if the card is in that owner's offer. Unlock it, echo it, and send
  ClearAcceptState if anyone had accepted.

### 2.6 ClearTrade 0x9A v1
- Read `0x101BD020`: AccountID, Session.
- Client X sends (`0x104BFA90`): `[prefix] 2A 09 01 ×3 U(X) U(S)`.
- Server: remove and unlock **all cards owned by X**, reset both accept flags, and send to A and B
  `2A 09 01 ×3 U(X) U(S)`. If the state was Final, also send LockTrade(false) and
  ClearAcceptState (which closes the confirm dialogs), and set the state back to Open.
- Execute `0x101BCEC0` → event 0x28(S, X) → UI `0x104C0680`: resets X's pile and model, unchecks
  both accept buttons.

### 2.7 AcceptTrade 0x96 v1
- Read `0x101BA530`: AccountID, Session, U → bool (+0xC).
- Client X sends (`0x104C2640`): `[prefix] 26 09 01 ×3 U(X) U(S) U(0|1)`. The client refuses to
  check it when **both** piles are empty.
- Server: store the flag and forward `26 09 01 ×3 U(X) U(S) U(b)` to the **partner**. An echo to X
  is harmless: `0x104C0510` only updates the "you get" button when AccountID ≠ me.
  - When both are true: state = Final, then **LockTrade(true)**, then **SetupFinalTradeAccept**
    to both.
  - b = 0 while Final: revert (ClearAcceptState and LockTrade(false) to both, state Open).

### 2.8 ClearAcceptState 0x9D v1
- `2D 09 01 ×3 U(recipient) U(S)` (read `0x101BCCB0`: base only).
- Execute `0x101BCB60` → event 0x2B(S) → `0x104C08D0`: unchecks both accept buttons and emits
  `closeFinalAccept` (`0x104E6720`, signal 0), which closes an open confirm dialog. That may send
  FinalAccept(false); ignore it outside Final.

### 2.9 LockTrade 0x111 v1
- `21 11 01 ×3 U(recipient) U(S) U(1|0)` (read `0x101BE650`).
- Execute `0x101BE390`: if the lock is true **and** a current lobby exists (null-checked here),
  prints "Please wait while collections are resynced." Then event 0x37(S, lock) → `0x104C0870` sets
  screen +0x39. While +0x39 is set the client refuses adds. Removes are not gated client-side, so
  the server must refuse Add and Remove in Final.

### 2.10 SetupFinalTradeAccept 0x9C v2
- **Read** `0x101C0D40`: base, then **only if ver > 1** (`CMP [ESP+8],1 / JLE`) two vectors
  (`FUN_10077790`): Founder (+0xC), Trader (+0x1C). **Sending v1 would carry no lists.**
- Server to A and B (identical):
  `2C 09 02 ×3 U(recipient) U(S) U(nF) {2E 15 01 U(arch) U(0) U(id)}×nF U(nT) {2E 15 01 U(arch) U(0) U(id)}×nT`
  - Founder list = the server's offer of cards owned by **A (the issuer)**. Trader list = cards
    owned by **B**. Build both from the server's state, never from client messages.
- Execute `0x101C0840`: event 0x2A(S, founderA, founderB, traderA, traderB). The int lists are
  built locally, so the type asserts in the handler cannot be reached from the wire. Handler
  `0x104C1670`: S must match. If a `confirmDialog` already exists it does nothing. If
  me == founder (screen +0x34): give = founder list, get = trader list; otherwise swapped. It opens
  `WATradeConfirmDialog` (`0x104BB340`). `accepted()` → `acceptFinalAccept` → `0x104BFBA0`;
  `rejected()` → `cancelFinalAccept` → `0x104BFB20`.

### 2.11 FinalAcceptTrade 0x9B v1
- Read `0x101BDA80`: base, then U → bool.
- Client sends (`0x104BFBA0` for 1, `0x104BFB20` for 0): `[prefix] 2B 09 01 ×3 U(X) U(S) U(1|0)`.
  Both then emit `closeFinalAccept`.
- Server: if the state is not Final, **ignore** (spurious after the server closes a dialog).
  - 0 → ClearAcceptState and LockTrade(false) to both; state Open; reset all flags.
  - 1 → mark X; when both are marked → commit (2.12).
- Echoing to clients is pointless: the UI handler `0x104C0800` only matches S.

### 2.12 Commit and EndTradeSession 0x99 v1
1. One DB transaction: reassign every offered card (`UPDATE lon_card SET account_id=… WHERE id=?
   AND account_id=<owner> AND tradable=1`, RowsAffected must be 1) and write the audit rows. On any
   failure: roll back, send a forced Remove (2.5, ErrorCode 1) for the bad card, ClearAcceptState,
   LockTrade(false), state Open.
2. Per side (the existing writers): `RemoveInstances(given)` (skip if empty);
   `StartDelivery(d, received, silent=false, "trade", +S)` + `Complete(d)` (skip both if nothing
   was received). The Delivery window shows "Trade session: S" for a positive id.
3. **EndTradeSession to A and B**: `29 09 01 ×3 U(recipient) U(S)` (read `0x101BD6A0`: base
   only). Execute `0x101BD550` → event 0x27(S) → `0x104C0830` → `0x104BFC30`: clears screen +0x21
   (the "confirm before leaving" guard) and **sends `LobbyCommand_Join{3, role 1}`**:
   `[prefix] 2C 05 04 ×3 U(self) 03 01 00 00 00 00 00 00 27 01 01 00 00`.
4. Answer that Join like any Join(3): echo + BulkJoin (lobby spec section 3). The echo switches to
   the lobby screen. The trade screen dtor `0x104C1570` then sends **`Leave(L)`**
   (`2D 05 03 ×3 U(self) U(L) 00 00`) and **`EndTradeSession(S)`**, and the client sends
   ChangeStatus 2. Ignore Leave for any group ≠ 3, and ignore EndTradeSession for a session that is
   Done or unknown.

### 2.13 SynchronizationCommand_SetDesired 0x10F v2 (client → server, optional)
- Read `0x101B1130`: headers ×3, then a map via `FUN_100605F0` (assumed `U(count)
  {U(archetype) U(want)}` like the other maps; the reader itself was not opened). Store it per
  account if you want to subtract Want from the type-3 list. Otherwise ignore it. The client also
  checks Want locally for its own cards.

### 2.14 NetworkCommand_ErrorMsg 0x6B v2 (2 levels)
- Read `0x1016F1B0`: headers ×2, S(ErrorMsg), `ver>1`: U(ErrorCode).
- `2B 06 02 2B 06 02 S(text) U(code)`. With a code: `2B 06 02 2B 06 02 00 00 <U(code)>`.
- Execute `0x1016EFB0`: code 0 → event 9 → "Error" box with the string. Code ≠ 0 → event 0x56
  → "Error" box with the factory text. No disconnect, no screen change.

### 2.15 Not needed
- **ChangeTradeSession 0x10E** (`OldTradeSessionID`; renames S on the screen): do not send.
- **SetListenStatus 0x1AF**: that is the posted-trades window (already handled).
- **TradeCommand_PostedTradeError 0x195**: posted-trade UI only (event 0x90), not live trades.

---

## 3. The trade lobby id L

What the client does with L (complete list, all traced):
- `0x104BD270`: map L → S in WATradeManager (`std::map` insert; a duplicate key only warns and
  may keep the old S).
- `FUN_1045FE90(6, L, 1)` → case 6: no group lookup.
- Screen +0x24 = L (`0x104BF9A0`). `0x104BEA10` resolves S = map[L].
- Chat widget group = L (`0x103BAE80`, widget +8). Its chat display shows only events whose group
  == L (`0x103C34B0`). Typing in the trade chat sends `LobbyCommand_SendText{GroupID = L}`.
- Screen dtor `0x104C1570` → `FUN_10156950(L)`: sends `Leave{GroupID L}` and removes L from the
  joined list. The group lookup is null-safe. There is a log-only guard for L == 5.

**Safe handling:**
- L is **not** a real group. **Never AddGroups for L.** A non-Match type-2 child of group 3
  crashes the user-list menu (lobby spec section 1), and nothing in the trade path needs the group
  to exist.
- L = **1,000,000 + S** (unique per session, never 1/2/3/5/8/9..19 or any id we will ever use for
  a lobby). **Never 3**: Leave(3) after the Join(3) would remove the player from the trade lobby.
- Ignore Leave(L) and Join(L).
- Trade chat (optional): relay a `SendText` with GroupID L from A or B to both A and B, rewritten
  with AccountID = the sender and GroupID = L. The SendText execute (`0x1014A770`) does no group
  lookup. The chat-line rendering in the widget was not traced to the end, so this is medium
  confidence and can be left out (the trade still works).
- Side effect, accepted: the trade's own event lines ("X adds Y") are posted with the **current
  lobby** id (3), not L, so they do not show in the trade chat pane.

---

## 4. Partner collection: needed or skippable?

- The screen opens on the **"Theirs" tab** (`setCurrentIndex(theirs)` in `0x104C3090`). The tip
  `0x10650C38` says "At the moment, you see the other player's collection". A player can only
  request the partner's cards from that view.
- Skipping it: the model functions seen (`0x103D2050`, `0x103D00C0`, `0x103CF9E0`) null-check the
  `"%dt"` collection lookup, so an empty "Theirs" tab is **probably** safe. Not every model path was
  traced. Recommendation: **send it** (step 2.3.4). The minimal fallback is an empty but present
  type-3 sync (`StartInstances{partner, 3, 0, 0}`, one SendInstances with 0/0, Complete), which
  guarantees the collection exists.
- Timing: send it before AcceptTradeRequest, so the collection exists when the model is built. If
  the live test shows an empty "Theirs" tab, also re-send it after AcceptTradeRequest: StartInstances
  clears it, and Complete posts 0x36, which refreshes the live screen.

---

## 5. Card identity and server validation

- On the wire a card is `(archetype, instance number)`. Instance number = `lon_card.id` (globally
  unique). Series is 0 on the wire and must be 0 in every InstanceID we send. Foil is its own
  archetype.
- AddCardToTrade's AccountID is the **owner**, not the sender. Take the sender from the session.
- Validate per add as in 2.4. One lock set is shared with posted-trade escrow.
- At commit, re-check with `WHERE id=? AND account_id=<owner> AND tradable=1` (RowsAffected = 1).
  Nothing is written before both final accepts, so a disconnect can never lose or duplicate a card.

---

## 6. Error paths

| Situation | Server response |
|---|---|
| Issue: A already has a pending request | ErrorMsg **0x25** to A |
| Issue: B is trading, has a request, or status ≠ 2 / not in group 3 | ErrorMsg **0x26** ("He is already in a trade session.") to A |
| Issue: A not allowed to trade | ErrorMsg **0x5B** (client shows the long "purchase a booster" text) or **0x79** for a GM ban |
| Issue: B not allowed | ErrorMsg **0x5C** |
| Issue: A == B, GroupID ≠ 3, A not in group 3 | Drop silently (the client UI cannot produce it) |
| Accept: not B, or S not Requested | Ignore |
| Cancel: S unknown or not Requested | Ignore (spurious Cancels are expected, section 0 item 8) |
| Add: non-tradable or not owned | ErrorMsg **0x2E** to the sender |
| Add: locked elsewhere | ErrorMsg **0x22** |
| Add: more than 50 on one side | ErrorMsg **0x23** |
| Add/Remove while Final or Locked | Ignore (or 0x24 "A card is still in the process of being added") |
| Accept with both offers empty | Ignore (the client already refuses) |
| Commit fails | Forced Remove (ErrorCode 1) + ClearAcceptState + LockTrade(false) |
| Request timeout (about 60-120 s) | Cancel to both, Origin 0 (A reads "#Actor(B) cancels your trade request") |

The texts are in `research/eqlib_LoN/LoN_errors.txt`. ErrorMsg code 0 with our own text is allowed, for example
"X left the trade."

---

## 7. Leaving, disconnecting and ordering

- **Esc or the leave button (X)**: X sends Join(3). Server: if X is in an Open or Final trade,
  end it (unlock everything, EndTradeSession to the partner, optionally ErrorMsg code 0 "X left
  the trade."), then answer the Join (echo + BulkJoin). X's dtor then sends Leave(L) +
  EndTradeSession(S): ignore both. The partner's client then sends Join(3): answer it.
- **EndTradeSession from a participant while Open** (any path): the same as above.
- **Disconnect during the request**: Cancel to the survivor with Origin = the disconnected
  account.
- **Disconnect during the trade**: EndTradeSession to the survivor, unlock. Run the lobby cleanup
  (`Leave(3)` to the members) **before** erasing the session.
- **After the commit**: the DB is already correct. A survivor who disconnects later gets the new
  collection at the next login.
- **Ordering requirements** (each one is a hard requirement):
  1. The recipient has a current lobby (joined group 3 this connection) before any Issue, Accept,
     Cancel, StartTradeSync, Add or Remove reaches it.
  2. Issue echo before any Cancel, StartTradeSync or Accept for the same S.
  3. Other-request Cancels, then StartTradeSync, then SetTradeAllowed, then the type-3 sync,
     **then** AcceptTradeRequest.
  4. AcceptTradeRequest before any Add, Remove, AcceptTrade, Clear, Lock, Setup or End (the screen
     must exist; earlier screen events are simply unhandled).
  5. LockTrade(true) then SetupFinal. On revert: ClearAcceptState, then LockTrade(false).
  6. Deliveries, then EndTradeSession.

---

## 8. Every assert or crash reachable from server data

| Trigger | Where | Avoided by |
|---|---|---|
| Null current lobby deref (access violation) | Issue/Accept/Cancel/Add/Remove executes (only when both accounts are cached) and **StartTradeSync (always)** | Ordering rule 1 |
| Partner account not cached passed to the dialog formatter (null safety not proven) | `0x104BD2E0` → `FUN_1012D660` | Trade only between group-3 members that were introduced with Join or BulkJoin |
| `mTypeID == kIntegerListTypeID` / `kInstanceIDTypeID` | `0x104C1670`, `0x104C0310`, `0x104C0410` | Unreachable: the events are built locally |
| `mCollection` | Sync service | Unreachable for type 3 (the collection is created first) |
| `myAccount` | RemoveInstances, StartDelivery | Logged in |
| Wrong version or a missing field → read fails (behaviour not traced; treat as fatal) | All reads | Use exactly the versions and fields in section 1. Never v1 for 0x9C |
| AcceptTradeRequest to a third party opens a stray trade screen | `0x104BD700` | Send only to A and B |
| Stale S for a reused L | `0x104BD270` | Unique L |
| Non-Match type-2 child of group 3 | User-list menu | Never AddGroups for L |

---

## 9. Remaining uncertainties (live two-client test) and the safe choice

1. **Spurious Cancel or FinalAccept(false) when the server closes a dialog** (Qt `closeEvent`
   → `reject()`). Safe: the state guards in 2.2 and 2.11 make them no-ops either way.
2. **"Theirs" tab timing**: whether the model reads the collection at setup or only on event 0x36.
   Safe: sync before Accept; if the tab is empty, also re-sync after.
3. **Duplicate "accepts your trade request" chat line** (StartTradeSync and AcceptTradeRequest both
   print it). Cosmetic. Live may have sent StartTradeSync only to A. Keep both if unsure.
4. **Event 0x1B "trade" in WASynchronizationManager** (StartInstances type 3): whether it shows a
   sync overlay that waits for Complete. Always send Complete.
5. **Trade chat over SendText(L)**: whether the widget shows relayed lines. Optional feature.
6. **SetDesired map layout** (`FUN_100605F0` not opened). Only needed for Want filtering.
7. **Which accept button is which**: "theyGetAcceptButton" = mine and "youGetAcceptButton" = the
   partner's, inferred from `0x104C2640` and `0x104C0510`. Server logic does not depend on it.
8. **Client read-failure behaviour** (drop vs disconnect): unknown. Never deviate from section 1.

## 10. Minimal safe message set

Server → client: IssueTradeRequest, CancelTradeRequest, StartTradeSync, StartInstances,
SendInstances and Complete (type 3), AcceptTradeRequest, AddCardToTrade, RemoveCardFromTrade,
ClearTrade, AcceptTrade, ClearAcceptState, LockTrade, SetupFinalTradeAccept v2, RemoveInstances,
StartDelivery, Complete, EndTradeSession, ErrorMsg. Optional: SetTradeAllowed, SendText(L).

Client → server handled: 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, plus Join(3),
Leave(any), ChangeStatus (relay 7/2), SendText(L) (optional) and 0x10F (optional).

## 11. Key VAs (new in this pass)

| What | VA |
|---|---|
| TradeCommand base read | 0x101B9CC0 |
| Manager map wipe at screen open | 0x104BE7B0 |
| Screen bind (S from L, partner, founder, allowed copy) | 0x104BEA10 |
| Screen setup (chat group L, `"%d"`/`"%dt"` models, tabs) | 0x104C3090 |
| Theirs key `"%dt"` | 0x104BFA00 |
| Leave group (dtor) | 0x10156950 |
| returnToLobby / EndTradeSession → Join(3) | 0x104BFC30 (metacall 0x104E6740 case 0x18) |
| Manager slots (cancel / accept / destroyed) | 0x104E61D0 |
| Clear handler (owner-based) | 0x104C0680 |
| Setup-final handler (founder swap) | 0x104C1670 |
| InstanceID read / equality | 0x1012B820 / 0x1012B7C0 |
| WAMainWindow events 9 / 0x56 | 0x1045EDD0 / 0x1045EEC0 (filter 0x104603B0) |
| Sync Complete (type 3 → 0x36) | 0x101B2C90 |
