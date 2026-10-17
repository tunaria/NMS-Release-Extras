# Legends of Norrath Trade Lobby (group 3): wire spec, checked against the client's read functions

Researched 2026-10-06. Static RE only: LegendsOfNorrath.dll in Ghidra (base 0x10000000). Read-only:
nothing renamed, nothing run. Every field below comes from the client's **read** function. The
VA is given each time, with the decompile or disassembly as evidence.

**Corrections to `research/lon_direct_trade.md` section 1 (important):**
1. The Lobby objects in `AddGroups` are **NOT** preceded by a nullable flag. The prior doc said
   "flag 0, Lobby object". That is wrong. A leading 0 would be read as the class id, and the read would fail.
2. The PropertySets in BulkJoin's `vector<object*>` are also **not** preceded by a flag.
3. The Lobby object's real read is `0x10137830`, version **3** (`0x10138080: MOV EAX,3`). Its vtable
   (serializing sub-object) is `0x1061287C`. The catalog's "0x67 Lobby UNVERIFIED, vtable 106128BC" is
   the primary (QObject) vtable of the same class.
4. In `UpdateLobbyStats`, the per-lobby max map is at command +0x30 and goes to controller **+0x90**.
   The int at command +0x2C goes to controller **+0x8C** (default max). Current population: map 1 →
   controller **+0x68**.

---

## 0. Encoding primitives (all checked)

| Primitive | Client read | Wire |
|---|---|---|
| int | `FUN_10097620` | our `U()` varint (low nibble, bit4 = negate, bits5-7 = extra byte count). |
| string | `FUN_10097a00` (also `FUN_101772f0` → it) | `S()` = U(len) + bytes + NUL (as everywhere in lon_service.cpp). |
| class header, one per class level | `FUN_10176CA0` (vtable+0x10 of every class) | `U(classId) U(version)`. classId must equal the leaf's `GetClassID` (vtable+0x18/+0x1C), else `"Wanted classID %d, but got %d"` and the read fails. Every level repeats the leaf's (cls, ver), which is our `Header(cls, ver, levels)`. |
| **nullable** object | `FUN_1000DD90`, `FUN_100A5010` | `U(flag)`: flag ≠ 0 → null, nothing more. flag 0 → the object's own Header + fields follow. The class id is only **peeked** (`FUN_100976E0` does not advance), then factory, then `obj->vtable[0x24](stream)` (= its Read, which consumes the Header). |
| **plain** object in a vector | `FUN_10177300` | **no flag**: peek classId, factory, Read. A null cannot be encoded. |
| int list (vector/set) | `FUN_1000A2C0`, `FUN_1000CCC0`, `FUN_1013D5C0` | `U(count)` + count × `U(int)`. |
| string list | `FUN_1000A340` | `U(count)` + count × `S()`. |
| map<int,int> | `FUN_100C95C0` → `FUN_101770C0` | `U(count)` + count × (`U(key) U(value)`). |
| PropertySet 0x17 v1 | read `FUN_10192B90` → `FUN_10192400` → `FUN_10177400` | `Header(0x17,1,1) U(count)` + count × (`U(key)` + ValueData object **without flag**). |
| ValueData 0x16 v4, string list (type 7) | as `SendAccount` already does (live-proven) | `Header(0x16,4,1) U(7) U(1) U(n) S(...)×n` |
| zlib block | BulkJoin read `0x1013E46E..` | `U(rawLen) U(compLen) bytes[compLen]`. `uncompress()` (`FUN_10394070`) must return Z_OK, **else assert `"zres == 0"` (LobbyCommandBulkJoin.cpp:0x119)**. Our `EQ::DeflateData` uses `deflateInit` (zlib wrapper), which is correct, **but it returns 0 on failure**. Never send compLen 0; send the exact rawLen. |

Level counts: `LobbyCommand_*` = 3 levels (leaf read → `FUN_10138460` LobbyCommand read: Header +
`FUN_1016C670` NetworkCommand read + AccountID). `Lobby` 0x67 = **1** level (`0x10137830` calls
vtable+0x10 once and no base read). `PropertySet` = 1, `ValueData` = 1.

Varint bytes used below: 0x5C→`2C 05`, 0x5D→`2D 05`, 0x5E→`2E 05`, 0x61→`21 06`, 0x67→`27 06`,
0x6A→`2A 06`, 0x89→`29 08`, 0x19C→`2C 19`, 0x17→`27 01`, 0x16→`26 01`, 0xFC5→`25 FC`, 1000→`28 3E`.

---

## 1. LobbyCommand_AddGroups 0x5E v1 (send once per connection, group 3 only)

Read `0x1013A510`: Header, `FUN_10138460` (base: AccountID), then `FUN_1013A470(this+8)` =
`U(count)` and count × `FUN_10177300` (**plain object, no flag**). The returned pointer is
PersistentComponent* (full+8), and the code stores `ptr-8`. No version gate (use v1).

```
Header(0x5E,1,3) U(account_self) U(1)              // vector count = 1
  Header(0x67,3,1)                                  // Lobby v3, NO flag in front
  5 x PropertySet (nullable, FUN_1000DD90):          // general, player, observer, moderator, staff
      recommended: U(0) Header(0x17,1,1) U(0)        // present, empty  (null = U(1) is also legal)
  5 x role list (FUN_1000A2C0, keys 1..5 in order):  U(0) U(0) U(0) U(0) U(0)
  U(0)        // ContainingGroupID  (+0x14 → full+0x1C, getter FUN_1012D750)
  U(3)        // GroupID            (+0x18 → full+0x20, getter FUN_1012D7C0)
  U(1)        // eLobbyTypeID       (+0x1C → full+0x24, getter FUN_1012D7E0)
  U(1)        // [v>=2] deck, nullable (FUN_100A5010) -> null
  U(0) U(0)   // [v>=3] tournamentRound (+0x68), subState (+0x6C)
```
Bytes: `2E 05 01 2E 05 01 2E 05 01 <U(acct)> 01 27 06 03 [00 27 01 01 00]x5 00 00 00 00 00 00 03 01 01 00 00`

Evidence, Lobby read `0x10137830` (disassembly): vtable+0x10 header into local `[ESP+8]` (= version).
Five `CALL 0x1000dd90` into this+0x2C/0x30/0x34/0x38/0x3C. Five `FUN_10137330(key 1..5)` +
`CALL 0x1000a2c0`. Then `FUN_10097620` into +0x14, +0x18, and a local stored to +0x1C. Then
`CMP [ESP+0xc],0x1 / JLE` → `CALL 0x100a5010` (+0x54). Then `CMP [ESP+0xc],0x2 / JLE` → two ints +0x68, +0x6C.

**Null PropertySets are safe in AddGroups** (execute `0x101399D0`, disasm `10139b59..10139bdb`: each
`vtable+0x50(role)` result is replaced by a freshly built empty PropertySet when 0). `HasProperty`
(`0x1012D930`) null-checks the general set. Other users of `Lobby::GetPropertySet(role)`
(`0x1012D970`, returns +0x34..+0x44) were not all traced, so **send present-but-empty sets**. They
cannot be dereferenced as null anywhere.

**eLobbyTypeID: use 1.**
- The Lobby ctor default is 1 (`0x10138110`: `MOV [ESI+0x24],EBX` with EBX=1).
- `Lobby::AddMember` (`0x101376B0`): for type 1, it sets the joining account's lobby status to **2**
  (`FUN_10001360(2)`) when the account is cached. Status 2 is what the Trade item needs.
- Join exec: for type 1, it calls `FUN_10155B10` (buddy bookkeeping, no assert).
- Enter-lobby `FUN_10156B60`: for type 1, it leaves the other joined lobbies except group 1. We have none.
- The self-join UI path (`0x10453170`) sends every type except 4/5/6/7/8/9 and Match objects to
  `FUN_1045FE90(3, group, 1)`, which is the lobby screen. Type 1 and type 3 both work. Type 3 has no
  special case at all and would not auto-set status 2.
- Screen case 3 in `0x1045DE50` returns early for types 6/7/8 and treats ContainingGroupID 8
  specially. So use **Containing = 0**.
- The trade-lobby look is keyed on **GroupID 3**, not on type (`0x10457560`: `if (+0x24 == 3)` →
  "tradelobby" help + `postedTradesButton`).

What AddGroups does on the client (`0x101399D0`): it posts event 3(Containing, GroupID, Type, 5 sets).
If Containing exists, it adds the lobby as a child (`FUN_101374B0`). It registers the lobby in a
`std::map` by GroupID with **insert**, so a duplicate is ignored (`0x101539C0`; the new object leaks).
Optional keys 0xFBC (int ≥4 → `FUN_1015F2D0`) and 0xFBD/0xFC1 (int list / string list, `account`
assert at LobbyCommandAddGroups.cpp:0x8C) are read from the Lobby's *embedded* set. Never send them.
Events 0x39/0x3A appear only when count > 1.

**Auto-join:** the event-3 handler `0x10452E00` sends `Join{group 1, role 1}` **only when GroupID == 1**.
For group 3 nothing is sent, so AddGroups alone does not join. **Do not add group 1.**

**Never** add a child group of 3 (Containing = 3) with type 2 unless it is a real Match (class 0x68).
The user-list right-click menu (`0x104CB800` → `FUN_104CB5E0` → `FUN_10153990` Match cast) calls
`(*piVar4)->vtable[0x40](0x1052)` with no null check. A plain Lobby there means a null-deref crash.

## 2. LobbyCommand_UpdateLobbyStats 0x19C v3

Read `0x1014FCE0` (disassembly): header, base, then `FUN_100C95C0` ×3 (this+8, +0x14, +0x20).
`CMP ver,1 / JLE` → int (+0x2C). `CMP ver,2 / JLE` → map (+0x30). Execute `0x1014FB40` copies them in
full (each send **replaces** the whole map): +0x20 → ctl+0x80, +0x14 → ctl+0x74, +8 → ctl+0x68
(`FUN_1006C190`), +0x2C → ctl+0x8C (`FUN_10068200`), +0x30 → ctl+0x90 (`FUN_1006C250`). It then posts event 0xD2.

```
Header(0x19C,3,3) U(account_self)
  U(1) U(3) U(population)      // map 1: current population  (ctl+0x68, read by FUN_1006ACB0, default 0)
  U(0)                         // map 2 (ctl+0x74)  meaning unknown -> empty
  U(0)                         // map 3 (ctl+0x80)  meaning unknown -> empty
  U(1000)                      // default max (ctl+0x8C)
  U(1) U(3) U(1000)            // map 4: per-lobby max (ctl+0x90, read by FUN_1006AC30, falls back to +0x8C)
```
Navigator `0x10471A50(3,0)`: `if (max <= current)` → "Lobby Full" dialog, unless the own account
flags have "Staff"/"StaffLogin". Send this at login and to every online session whenever the
trade-lobby population changes.

## 3. LobbyCommand_Join 0x5C v4

Read `0x10145C40` (disassembly): header, base(AccountID), GroupID (+8), RoleType (+0xC). `ver>1`:
Password string (+0x10). `ver>2`: int list (+0x2C). `ver>3`: AccountName string (+0x3C), **nullable**
PropertySet (`CALL 0x1000dd90`, +0x5C), Gender int (+0x58).

```
Header(0x5C,4,3) U(account) U(3) U(1) S("") U(0) S(name) <nullable PropertySet> U(gender)
```

Execute `0x10145340`:
- **If AccountName is non-empty** (+0x50 = string size): `if (+0x5C == 0) assert "mPropertySet"`
  (LobbyCommandJoin.cpp:0x5E), so **a PropertySet is mandatory**. `FUN_10018430(acct)` get-or-create
  (assert "account" only if creation fails). It then sets the name and gender, **merges** the
  PropertySet (`FUN_10008BC0`), and posts event 0x10. If acct == self, it also reads the int lists
  0xFC0/0x1012 (buddies/ignores, type 6) from the self account.
- Group lookup `FUN_10153920(GroupID)`. If missing: log `"Join: couldn't get group: %d accountID: %d"`
  and stop (no crash, no screen).
- If found: self → `FUN_10068560(lobby)` (current lobby, asserts lobby≠0, guarded); other → sets
  account prop 0xFBE = GroupID. Then event 5(GroupID, acct, role). **Role 0 becomes 1.**
  `Lobby::AddMember(acct, role)` (asserts `kInvalidRole != role`, Lobby.cpp:0x255). For type 1, the
  status becomes 2 here. Then `FUN_1012E8D0`.

**(a) Echo to the joiner** (this is what opens the lobby screen):
```
Header(0x5C,4,3) U(self) U(3) U(1) S("") U(0) S("") U(0) Header(0x17,1,1) U(0) U(0)
bytes: 2C 05 04 2C 05 04 2C 05 04 <U(self)> 03 01 00 00 00 00 00 00 27 01 01 00 00
```
With the name empty, the account update block is skipped. The self account already carries 0xFC5
from `SendAccount(s, true)` at login (line 1443, prefs=true adds "RegisteredUser").

Event 5 handler `0x10453170`, self branch: group 3 is not 1, type 1 is not 4/9, not a Match, not
5/7/8/6, so `FUN_1045FE90(3, 3, 1)` → `0x1045DE50` case 3: `FUN_10156B60(3)`, `FUN_10150B60(2)` (the
client **sends ChangeStatus 2**), and it builds `WALobbyScreen`, then `setGroup(3)`.
**ChangeLobbyDisplay is not needed. Sending it would build the screen a second time.**

**(b) Announcement to every other member** (introduces the account):
```
Header(0x5C,4,3) U(newcomer) U(3) U(1) S("") U(0) S(newcomer_name)
  U(0) Header(0x17,1,1) U(1)
     U(0xFC5) Header(0x16,4,1) U(7) U(1) U(1) S("RegisteredUser")
  U(gender)
```
The Trade item check `FUN_104BCDD0` (verified): own and target account both need 0xFC5 (type 7)
to contain "RegisteredUser" or "BoardGame" (`FUN_10006100` reads key 0xFC5, type 7). There must be no
open request with the target (`FUN_104BCC90`). Target `+0x3C` (`FUN_10001350`) must be **2**. Neither
account may contain "ConventionGuest". The menu `0x104CB800` also requires the target to be cached
(`FUN_10017DF0`), group +0x24 == 3, and the target to be a member of group 3 (`FUN_10135C50`). No other
key is read or asserted by Join/BulkJoin. The merge keeps keys we do not send (SendAccount comment), so
an announcement never wipes another member's data.

## 4. LobbyCommand_BulkJoin 0x6A v3 (the roster, to the joiner)

Read `0x1013E2D0` (disassembly). `ver >= 3` (`CMP [ESP+0xc],3 / JGE 1013e46e`): `U(rawLen)`
(overwrites the version slot), `U(compLen)`, `FUN_100097A0` reads compLen bytes, `new(rawLen)`,
`uncompress`, assert `zres == 0`, then everything below is read **from the decompressed buffer**:

```
Header(0x6A,3,3) U(self)
  U(rawLen) U(compLen) <zlib bytes of RAW>
RAW =
  U(n) U(id_1)..U(id_n)     // role 1 (Player) members   -- FUN_10137330(1)+FUN_1000A2C0
  U(0)                      // role 2
  U(0)                      // role 3
  U(0)                      // role 4
  U(0)                      // role 5
  U(3)                      // GroupID (+0x14)
  U(n) U(id_1)..U(id_n)     // account ids (+0x18, FUN_1000A2C0)
  U(n) S(name_1)..          // names (+0x28, FUN_1000A340)
  U(n) U(gender_1)..        // genders (+0x38, FUN_1000CCC0)
  U(n) PS_1..PS_n           // PropertySets (+0x48, FUN_1000CD40 -> FUN_10177300: NO flag)
       PS_k = Header(0x17,1,1) U(1) U(0xFC5) Header(0x16,4,1) U(7) U(1) U(1) S("RegisteredUser")
  U(n) U(status_1)..        // lobby statuses (+0x58, FUN_1013D5C0): 2 = Online, 7 = trading
```
Execute `0x1013D640`:
- It walks ids, names, genders, and PropertySets in parallel and **stops at the shortest list**.
  `if (propertySet == 0) assert "propertySet"` (LobbyCommandBulkJoin.cpp:0x4F). The account is
  get-or-created (assert "account", :0x54), then name, gender, merge, and event 0x10.
- Event 0x38(GroupID, ...). If the group is missing, it logs `"BulkJoin: couldn't get group"`.
  Otherwise, for each role key and each id, `Lobby::AddMember(id, role)`. **Every id in a role list
  must also be in the id/name/PS lists**, or the member is listed with no cached account.
- Then, for ids ∥ statuses: if cached, status = value, event 0x19.

Keep all five lists the same length and order. Leave out the joiner (it was added by the echo, and
leaving it out avoids overwriting its own name). An empty roster is valid: all counts 0. The raw
bytes are then `00 00 00 00 00 03 00 00 00 00 00`.

## 5. Other lobby messages

**LobbyCommand_Leave 0x5D v3**. Read `0x10146540` (disassembly): GroupID; `ver<2` reads a string,
`ver>=2` reads an int (+0xC); `ver>2` reads an int list (+0x10).
```
Header(0x5D,3,3) U(leaver) U(3) U(0) U(0)
```
Execute `0x10146380`: an empty list means the list is {AccountID}. For each entry: event 6(GroupID, acct,
+0xC), remove member. If acct == self, the group is cleared (`FUN_10137070`). Send it to the remaining
members only (not to the leaver). The client's own Leave has the same layout: ctor `0x101460A0` sets
+0xC = 0 and an empty list.

**LobbyCommand_ChangeStatus 0x89 v1**. Read `0x10140B20`: AccountID, Status.
```
Header(0x89,1,3) U(account) U(status)
```
Execute `0x10140930`: if the account is cached, it sets +0x3C and posts event 0x19. An unknown account
is ignored. The client sends its own status on every screen switch (`0x1045DE50`: lobby 2, home 3,
deck builder 6, trade 7, game 5, match dialog 4, ...). Store it per session and relay it to the other
group-3 members.

**LobbyCommand_SendText 0x61 v4**. Read `0x1014AAC0`: GroupID, TargetUserID, Disposition, Text;
`v>1` int → bool QuickChat (+0x30); `v>2` ChatID (+0x34); `v>3` LocaleID (+0x38). Client write
`0x1014ABA0` uses the same order.
```
Header(0x61,4,3) U(sender) U(3) U(target) U(disposition) S(text) U(quick 0/1) U(chatId) U(locale)
```
The client's group chat (`0x103C3890`) sends target 0, disposition 1, the text, or QuickChat=1 + ChatID.
It does **not** echo locally, so relay to **all** members, sender included. Execute `0x1014A770`:
disposition 0 → event 0xF (private tell, `0x103C3F50` resolves the name in the group); otherwise
event 7 (group line). Rebuild every message: AccountID = the session's account, GroupID = 3. Clamp the text.

**LobbyCommand_ChangeLobbyDisplay 0x123 v1**. Read `0x10140710`: AccountID, GroupID → event 0x46 →
screen switch. **Do not send it.** The Join echo already switches the screen.

## 6. Client side

- **Navigator "Trade Lobby"**: `0x10473920` → `0x10471A50(3, 0)`. If max ≤ current, it shows "Lobby
  Full" and sends **nothing** (this is what happens today: both values default to 0). Otherwise
  `0x1046FB20(3)` sends Join v4. Writer `0x101459F0`, version `0x101458E0` = 4, ctor `0x101457F0`
  makes an empty non-null PropertySet:
  `[12-byte prefix] Header(0x5C,4,3) U(self) U(3) U(1) S("") U(0) S("") U(0) Header(0x17,1,1) U(0) U(0)`
- It waits for the **Join echo with AccountID = self**. Nothing appears until it arrives. After the
  screen opens it sends `ChangeStatus(self, 2)`. The posted-trades bar uses the existing
  RequestPostedTrades handler.
- **Leaving**: going Home sends only `ChangeStatus 3` (case 2). The player stays in group 3. `Leave(3)`
  is sent only by `FUN_10156B60` (entering another type-1 lobby; there is none). The trade screen dtor
  `0x104C1570` sends `Leave(L)` + EndTradeSession for the trade-session id L. Ignore Leave for any
  group ≠ 3. Remove members on disconnect and on a received `Leave(3)`.
- **After a trade**: `0x104BFC30` sends `Join{3, role 1}` again. Answer it like any Join (echo +
  BulkJoin). The server's add is idempotent, and the client re-adds the same role safely (remove + add
  in `0x101376B0`).
- **At login** the client sends no lobby Join. It only sends `ChangeStatus 3` from the home screen
  (currently logged as unhandled). Group 1 (main lobby) is **not** needed for the trade lobby.
  Adding it would auto-send Join(1) and route the self-echo to the home screen (case `param_1 == 1` →
  screen 2).

## 7. Order and what asserts

Safest order:
1. Login (existing): LoginResults, `IntroduceAccount` (with 0xFC5 "RegisteredUser"), and the rest.
2. **AddGroups(3)**, once per connection.
3. **UpdateLobbyStats** {3: population}, max 1000.
4. On `Join(3)` from A (after validating the session):
   1. Add A to the server roster with status 2.
   2. To A: **Join echo**.
   3. To A: **BulkJoin** (everyone else, including statuses).
   4. To every other member: **Join announce(A)** (+ `ChangeStatus(A, s)` if s ≠ 2).
   5. To all online sessions: **UpdateLobbyStats**.
5. On `ChangeStatus` from A: store it and relay to the other members.
6. On disconnect or `Leave(3)`: `Leave(A)` to the others, then UpdateLobbyStats.

Join/BulkJoin before AddGroups only logs "couldn't get group" (no crash, but no screen). Send the
echo before BulkJoin: the screen is created from event 5. Event 0x38 and the per-member events then
reach a live screen, which works whether the user model reads group state at creation or only
listens to events.

Asserts (EverQuest exits):
- Join with a name and a null PropertySet (Join.cpp:0x5E).
- BulkJoin: a null PropertySet (:0x4F, impossible with the correct no-flag encoding), or zlib that does not decompress (:0x119).
- Role 0 in AddMember (impossible: role lists are keys 1..5, and Join turns 0 into 1).
- AddGroups optional key 0xFBD/0xFC1 listing an unknown account (:0x8C). Never send it.
- Indirect null-deref: a type-2 non-Match child group of 3 (section 1).

A wrong class id or a short field fails the read. What the client then does (drop or disconnect) was
not traced. Treat it as fatal.

## 8. Uncertainties only a live test can settle (with the safe choice)

1. **eLobbyTypeID**: 1 (ctor default, auto status 2). Fallback 3. Statuses are always sent
   explicitly, so either one works for Trade.
2. **WALobbyUserModel display needs**: whether the list wants more account keys (avatar, guild,
   rating). Static RE found no assert, but the model was not traced. Start with 0xFC5 only and watch for
   blank cells.
3. **UpdateLobbyStats maps 2/3** (ctl+0x74/+0x80): their meaning is unknown. Send them empty.
4. **Join v3 int list** and **Leave +0xC int**: meaning unknown. Send empty / 0, as the client's own
   messages do.
5. **Chat**: whether live echoed group chat to the sender (the client has no local echo, so echo).
   Tells (disposition 0): send to the target and the sender.
6. **Read-failure behaviour** of the client (drop vs disconnect). Unknown, so never send a field you are not sure of.
7. **Gender values**: the account +0x38 enum is unknown. Use the same value our IntroduceAccount sends (0).
8. **Self in BulkJoin**: leaving it out is verified safe. Including it (same name and keys) should
   also be safe, but it is untested.
