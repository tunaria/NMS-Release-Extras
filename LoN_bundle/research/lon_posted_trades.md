# Legends of Norrath -- Posted Trades (the trading post), what the server must do

Research 2026-10-06, read-only. LegendsOfNorrath.dll in Ghidra, image base 0x10000000 (all VAs below are
absolute). Field layouts from `research/eqlib_LoN/LoN_messages.txt`; behaviour from the decompiled
handlers quoted here. "CONFIRMED" = read in the code; "INFERRED" = a reasoned guess, flagged.

Nothing in the server today answers 0x18E / 0x191 / 0x1AF, so the dialog sits on "Requesting posted
trades..." and the list stays at "0 trades available"; a post is never echoed, so it never appears.

---------------------------------------------------------------------------------------------------
## 1. The client's posted-trade store (key to everything)

- Singleton store `DAT_1078c524` (getter/creator `FUN_101c46c0`), a `std::map<int id, PostedTrade*>`.
  - add/replace: `FUN_101c4730(trade)` -- key = `trade+0x08` (getter `FUN_1018bd80` = `return *(this+8)`),
    deletes an older object with the same id, stores the new one, then `FUN_1018c0b0` registers the
    trade's extra archetypes (field +0xF8, section 2).
  - remove: `FUN_101c3f90(id)`.  clear: `FUN_101c4010`.  find: `FUN_101c40d0(id)`.
  - visible list: `FUN_101c4850` (filter, section 6).  my-public-posts count: `FUN_101c3ad0(accountId)`.
- The LIST WIDGET is built from the store only at `EndPostedTrades` (`FUN_1048cec0` re-adds every
  visible id), or one row at a time on `AddPostedTrade` (0x8B) / removed on `CancelPostedTrade` (0x8C).
- Search box, "search by" (Username / Offered / Wanted), sort and the filter panel are ALL client-side
  (`QSortFilterProxyModel` set up in `FUN_10489830`; filter predicate `FUN_1048d470`). The server never
  sees a search. Columns (model ctor `FUN_1048cff0`): 0x1020 (icon), 0x1022 "Username",
  0x1025 "Offered", 0x1028 (Wanted), 0x1023 "Date posted".

---------------------------------------------------------------------------------------------------
## 2. PostedTrade (class 0x194, v3) -- exact layout

vtable 0x1061F6B4, write `FUN_1018c6b0`, read `FUN_1018d560`, ctor `FUN_1018cc70` (all fields 0/"").
It is a data object: written as ONE header pair (0x194, 3) when nested.

Wire order (read `FUN_1018d560`; version gates at `1 < ver` and `2 < ver`):

| # | obj off | wire type | meaning | evidence |
|---|---------|-----------|---------|----------|
| 1 | +0x08 | int | **Posted Trade ID** (store key; `Posted Trade #%1`) | getter `FUN_1018bd80`, store key in `FUN_101c4730` |
| 2 | +0x0C | int | **poster AccountID** | `FUN_1018bda0`; `FUN_10486f30`: `if (myAccount == trade+0xC) "You can not accept a trade that you posted."`; `FUN_1018c1e0` |
| 3 | +0x2C | string | **poster name** ("Posted By", the Username column) | `FUN_10487cb0`: `FUN_1018bde0()` -> `"<b>%1:</b> %2"` with "Posted By" |
| 4 | +0x48 | int | **posted time, unix seconds** ("Posted On", "Date posted") | `FUN_10487cb0`: `QDateTime::setTime_t(FUN_1018be00())` |
| 5 | +0x4C | map<int,int> | **OFFERED**: archetype -> quantity (what the poster gives, the acceptor receives) | `offeredTreeWidget` filled from `FUN_1018be10` (+0x4C); text `"%d x %s"` in `FUN_1018c2d0` |
| 6 | +0x58 | map<int,int> | **WANTED**: archetype -> quantity (what the acceptor must give) | `wantTreeWidget` from `FUN_1018be20` (+0x58); acceptor ownership check `FUN_1018c1e0` walks +0x58 |
| 7 | +0x64 | int | **recipient AccountID** (0 = public) | setter `FUN_1018be30` from the builder's chosen account (`builder+0xB0`) |
| 8 | +0x84 | string | recipient name #2 (2nd string of the account lookup) | setter `FUN_1018be50` <- `builder+0xB8` |
| 9 | +0xA0 | int | **recipient GuildID** (0 = not guild-only) | setter `FUN_1018be90` <- `builder+0xBC` |
| 10 | +0xA4 | string | recipient guild name | setter `FUN_1018beb0` <- `builder+0xC0` |
| 11 | +0x10 | string (v>=2) | UNKNOWN, server-filled (copied by `FUN_1018cb80`, no UI reader found). INFERRED: poster character/display name. Send the poster name or "". | read gate `if (1 < ver)` |
| 12 | +0x68 | string (v>=2) | recipient name #1 (1st string of the account lookup) | setter `FUN_1018be70` <- `builder+0xB4` |
| 13 | +0xF8 | vector<string> (v>=3) | serialized archetype objects the receiver may lack; each string is decoded (`FUN_1018bfb0` -> `FUN_1018bed0`: flag, classId, factory, deserialize) and added to the archetype DB `DAT_1078be54` | `FUN_1018c0b0`. **Send count 0** -- every card we trade is in the shipped DB. |

Maps on the wire: varint count, then (key varint, value varint) pairs. A row whose archetype the client
cannot resolve (`FUN_10044eb0` returns NULL) is silently skipped in the detail trees.
Not on the wire: +0xC0 / +0xDC (cached display strings).

Byte example (public trade id 7, poster account 3 "Bob", posted 1791300000, offers 1 x 101000123,
wants 2 x 101000456):

```
U(0) U(0x194) U(3)                                                 nullable flag 0, header pair
U(7) U(3) S("Bob") U(1791300000)
U(1) U(101000123) U(1)                                             offered
U(1) U(101000456) U(2)                                             wanted
U(0) S("") U(0) S("")                                              recipient acct, name#2, guild, guild name
S("Bob") S("")                                                     v2: +0x10, +0x68
U(0)                                                               v3: archetype blobs
```

---------------------------------------------------------------------------------------------------
## 3. The commands (TradeCommand base 0x92 v1: AccountID, TradeSessionID)

Every TradeCommand_* is a 3-level command: `Header(cls, ver, 3)` then `AccountID, TradeSessionID`
(send our account id and 0), then its own fields. Same convention as the rest of lon_service.cpp.

| class | ver | fields after the base | client execute (what it does on RECEIPT) |
|---|---|---|---|
| 0x18D AcceptPostedTrade | 1 | Posted Trade ID, Response | `FUN_101b9f20`: posts UI event 0x8A(id, Response). **No widget handles 0x8A** (eventflow) -- purely informational. |
| 0x18E AddPostedTrade | 1 | PostedTrade (nullable object) | `FUN_101bb760`: if object != null -> store add (`FUN_101c4730`), post 0x8B(id). |
| 0x18F CancelPostedTrade | 1 | Posted Trade ID | `FUN_101bbc40`: store remove (`FUN_101c3f90`), post 0x8C(id). |
| 0x190 EndPostedTrades | 1 | - | `FUN_101bd1f0`: post 0x8D. |
| 0x191 RequestPostedTrades | 1 | - | `FUN_101bf5f0`: `return 1` (client->server only). |
| 0x192 StartPostedTrades | 1 | Posted Trade Count | `FUN_101c0e50`: post 0x8E(count). |
| 0x193 TransferPostedTrade | 2 | PostedTrade (nullable), vector<PostedTrade> (v2) | `FUN_101c2220`: store add of the single object AND of every vector element, one 0x8F per trade. |
| 0x195 PostedTradeError | 1 | Error (int), From Add (bool) | `FUN_101be870`: post 0x90(FromAdd bool, Error int). |
| 0x1AF SetListenStatus | 1 | Trade Button (bool) | `FUN_101bf8d0`: `return 1` (client->server only). |

Nullable object = varint 1 (absent) or varint 0 then the object (its classId, version, fields)
(`FUN_101c23a0`: `if (obj==0) write(1) else { write(0); obj->serialize }`).
TransferPostedTrade vector elements: count then whole objects (`FUN_101c2010`, INFERRED same
"object*" encoding as the other object vectors: classId, version, fields -- no null flag). Simplest: send each
trade in the single nullable slot with an EMPTY vector (count 0), one message per trade.

UI event handlers (WAPostedTradesDialog eventFilter `FUN_10489d50`, type 0x3E9):
- 0x8B -> `FUN_10488800`: if the id is not already a row and is visible to me (`FUN_1048ca10`), add the
  row and emit `postedTradeAdded(int)` (`FUN_104e25d0`) -- the builder wrapper is connected to this
  signal (`FUN_10489280`: `connect(dialog,"2postedTradeAdded(int)",wrapper,...)`) and closes.
- 0x8C -> `FUN_10488840`: remove the row (`FUN_103a6220`), emit `postedTradeCancelled(int)`
  (`FUN_104e2600`); every open detail dialog is connected to it (`FUN_10488c10`:
  `connect(this,"2postedTradeCancelled(int)",detail,"1handlePostedTradeCancelled(int)")`).
- 0x8D -> `FUN_10489730`: clear the status text, close the progress box, rebuild the list from the
  store (`FUN_1048cec0`).
- 0x8E -> `FUN_10489600`: "Receiving posted trades...", remember the count; **count >= 10 opens a
  progress box**, advanced every 20 transfers.
- 0x8F -> `FUN_10488660`: progress counter only (the store add already happened in execute).
- 0x90 -> builder (`FUN_10486330` -> `FUN_10483510`, only when FromAdd == true) or detail dialog
  (`FUN_10487440` -> `FUN_10486b70`, only when FromAdd != true). Section 5.

---------------------------------------------------------------------------------------------------
## 4. Message sequences

### 4.1 Open the Posted Trades window
Client (CONFIRMED):
1. dialog ctor `FUN_10488460` sends **SetListenStatus(true)** (`FUN_101bf8b0(1)`) -- the 14-byte 0x1AF.
2. dialog init `FUN_10489830` clears the store and calls `FUN_104897b0`: "Requesting posted trades...",
   `FUN_101c4270` = clear store again + send **RequestPostedTrades** (the 13-byte 0x191).

Server must reply, in this order:
1. `StartPostedTrades(count)` (0x192 v1)
2. `TransferPostedTrade` (0x193 v2) once per trade (or batched in the vector)
3. `EndPostedTrades` (0x190 v1) -- without it the list is never built and "Requesting/Receiving..."
   stays up.

Send every trade the account may SEE (section 6). Sending a targeted trade to someone else is
harmless (the client filters it) but leaks it -- filter server-side too.

### 4.2 Post a trade
Client: "Post Trade" (`FUN_10489280`) refuses unless the account flag "RegisteredUser" is set
(already sent by SendAccount) and the account has < 20 public trades in the store (`0x13 < count`
-> "You may only have %1 posted trades at once." with %1 = 0x14). Builder submit `FUN_104853f0`
refuses an empty trade ("Nothing has been added to this trade..."). Optional targeting:
- "playerOfferCheckbox" + name -> `FUN_10482ae0` sends **AccountCommand_GetAccountsByPartialName**
  (0x1CB v2); reply handled at UI event 0xB1 (`FUN_10485cc0`): 0 results "Account Not Found", 1 result
  -> sets +0x64/+0x68/+0x84 and submits, >1 -> "Choose Account" list.
- "guildOfferCheckbox" + name -> `FUN_10482a00` sends **GuildCommand_GetGuildsByPartialName**
  (0x1CD v1), reply event 0xB2.
Then `FUN_104827a0` builds a PostedTrade (ctor zeroes id, poster, time) with the offered map
(+0x4C) and wanted map (+0x58), the optional recipient/guild, and sends **AddPostedTrade** (0x18E).
The client does NOT touch the collection on post (no RemoveInstances, no lock): CONFIRMED -- the
AddPostedTrade path only builds and sends the command.

Server replies:
- success: echo **AddPostedTrade** with the PostedTrade FILLED (new unique id, poster account id,
  poster name, time) -> row appears, the builder closes. Escrow: RemoveInstances for the offered copies
  BEFORE the echo (section 7).
- failure: **PostedTradeError(Error, FromAdd = 1)** -> "Posted Trade Error" box in the builder.
- to OTHER listeners: see 4.6.

The observed 45-byte 0x18E fits: 9 header bytes + AccountID + TradeSessionID + flag 0 + 0x194/3 +
id 0, poster 0, "" , time 0 + one offered and one wanted pair (~6 bytes each) + 0, "", 0, "", "", "",
vector 0. Decode it with a generic LoN object decoder before trusting any field the client filled.

### 4.3 Browse / search / filter
Nothing on the wire. Double-click/"Examine" opens `WAPostedTradeDetailDialog` named
`postedTradeDetail<id>` (`FUN_10488c10`), filled from the store (`FUN_10487cb0`): title "Posted By /
Posted On", offered tree and want tree (columns qty / own / card; "own" = my copy count, colored
#FFCC66 for archetype type 7/8). Warnings: offered empty -> "If you accept this trade you would be
trading away items and receiving nothing in return."; a wanted archetype of type 7 or 8 -> message
0x14 from the string table (INFERRED: the loot-card warning).

### 4.4 Accept someone's trade
Client gates (`FUN_10486f30`): connected; flag "RegisteredUser" OR "BoardGame"; not my own trade;
`FUN_1018c1e0` = I own at least `qty` of every wanted archetype ("You do not have all of the cards
required to carry out this trade."); then "Are you sure you want to accept this trade?" ->
`finalAcceptTrade()` = `FUN_10486af0` -> `FUN_101c2ec0(id)` sends **AcceptPostedTrade(id, Response 0)**.
The client changes NOTHING itself.

Server replies (acceptor):
- success: RemoveInstances(the wanted copies taken from the acceptor), StartDelivery(the offered
  copies, `TransactionSource "trade"`, `TransactionSourceID = -id` -> the Delivery window shows
  "Posted trade: <id>" / tree "Posted Trade #<id>", CONFIRMED `FUN_103e82a0` / `FUN_103e8dc0`:
  `if (source == "trade" && sourceId < 0) arg(-sourceId)`), Complete; then **CancelPostedTrade(id)**
  so the row goes and the open detail dialog gets `postedTradeCancelled(id)`. Optionally echo
  AcceptPostedTrade(id, 1) (harmless, no handler).
- failure: **PostedTradeError(code, FromAdd = 0)** -> "Cannot Trade" box in the detail dialog.

Poster: if online -> StartDelivery(the wanted copies, same "trade"/-id), Complete (and, without
escrow, RemoveInstances of the offered copies first). If offline -> the DB already holds the change; the
next login's collection sync shows it. All listeners -> CancelPostedTrade(id).

### 4.5 Cancel my own trade
Context menu "Cancel" -> "Are you sure you want to cancel this posted trade?" -> `FUN_104886e0` ->
`FUN_101c2e50(id)` sends **CancelPostedTrade(id)**. The client does not remove the row until the
server echoes CancelPostedTrade(id) (execute removes from the store, 0x8C removes the row).
Server: verify poster == account, return escrow (StartDelivery "trade"/-id, Complete), echo
CancelPostedTrade(id) to the poster and to every listener who can see it. There is no cancel error
text; on failure just do nothing (or resend the list).

### 4.6 Live updates while the window is open
SetListenStatus(true/false) is a subscription (see section 5). Pushes to OTHER listeners:
- trade removed (cancel / accept / expiry): `CancelPostedTrade(id)` -- safe.
- new trade: send `TransferPostedTrade(trade)` + `EndPostedTrades` (End rebuilds the list from the
  store). AVOID pushing `AddPostedTrade` to others: its 0x8B emits `postedTradeAdded`, which closes
  their own open builder (the wrapper is connected to that signal) -- INFERRED from the connect; not
  worth the risk.

### 4.7 Close the window
dialog dtor `FUN_10488760` sends **SetListenStatus(false)**. No reply. Stop pushing to that session.

---------------------------------------------------------------------------------------------------
## 5. SetListenStatus (0x1AF v1)
Field: bool (+0x0C). Sent `true` by the WAPostedTradesDialog ctor (`FUN_10488460`, `FUN_101bf8b0(1)`)
and `false` by its dtor (`FUN_10488760`, `FUN_101bf8b0(0)`). Client execute is `return 1`. Meaning:
"this session has the trading post open; push changes to it". **No reply required.** Server keeps a
per-session `listening_posted_trades` flag (also clear it on disconnect). Guild twin: 0x1AD.

---------------------------------------------------------------------------------------------------
## 6. Visibility, limits, what the client enforces

Visible (`FUN_101c4850`, same test in `FUN_1048ca10`): `(recipientAcct == 0 || recipientAcct == me ||
poster == me) && (guildId == 0 || I'm in that guild (FUN_100040b0) || poster == me)`.

| limit | where | value |
|---|---|---|
| public posts per account | client `FUN_10489280` (counts my trades with +0x64 == 0 and +0xA0 == 0) | 20 ("You may only have %1 posted trades at once.", %1 = 0x14). Targeted posts are NOT counted by the client. Server error 0x0E is the authority. |
| cards per trade | client: none found; server error 6 "You tried to add too many cards to the posted trade." | our choice. Suggest 50 per side (live's live-trade cap, error 0x23 "You may not add more than 50 items to the trade."). |
| empty trade | client `FUN_104853f0` | refused if BOTH sides are empty; a give-away (offered only) or a request (wanted only) is allowed. |
| registration | client | "RegisteredUser" to create; "RegisteredUser" or "BoardGame" to accept. |
| expiry | client | **none** -- no expiry field, no expiry text; only "Posted On". Expiry is purely server policy (remove + return escrow + CancelPostedTrade push). |
| progress box | client | shown when Start count >= 10, refreshed every 20 transfers. |

---------------------------------------------------------------------------------------------------
## 7. PostedTradeError (0x195) codes and texts (CONFIRMED, decompiled)

FromAdd = 1 -> builder (`FUN_10483510`), title "Posted Trade Error":
| code | text |
|---|---|
| 6 | You tried to add too many cards to the posted trade. |
| 0x0C | error table 0x79: "You have been blocked from participating in trades." |
| 0x0D, 0x10 | The recipient of this posted trade is not allowed to trade. |
| 0x0E | You have already posted the maximum number of posted trades allowed. |
| 0x0F | error table 0x5B: "Your account is not currently flagged to allow trading." |
| other | "Unknown error: %1" |

FromAdd = 0 -> detail dialog (`FUN_10486b70`), title "Cannot Trade":
| code | text |
|---|---|
| 1 | This trade is being completed by someone else. |
| 2 | You or the person who posted this trade are already involved in a posted trade. Please try again later. |
| 3 | This trade has already been completed. |
| 4 | You are not the correct account to complete this trade. |
| 5 | You are not in the correct guild to complete this trade. |
| 7 | The person who posted this trade no longer has all of the offered cards. |
| 8 | You do not have all of the cards wanted by the person who posted this trade. |
| 9 | You or the person who posted this trade are in a trade. Please try again later. |
| 10 | You or the person who posted this trade are in a tournament. Please try again later. |
| 0x0B | The server was unable to lock and verify the ownership of the items to be traded. Please try again later or report this to support. |
| 0x0C, 0x0F | The person who posted this trade is no longer allowed to trade. |
| 0x0D | error table 0x79 (blocked from trades) |
| 0x10 | error table 0x5B (not flagged to allow trading) |
| 6 / other | empty "Cannot Trade" box |

Note: the 0x90 handlers do not check the trade id -- every open detail dialog shows the error.
Codes 1, 7 and 0x0B prove live did NOT escrow: the poster kept the cards and the server re-checked and
locked both sides at accept time.

---------------------------------------------------------------------------------------------------
## 8. Server design

### 8.1 Escrow -- our choice, the client allows both
The client never removes or locks offered cards itself (post path `FUN_104827a0` only sends). Live
did not escrow (error 7). We CAN escrow: send RemoveInstances on post and StartDelivery on cancel.
Recommended: **escrow** -- a posted trade can then never go stale, error 7 never fires, and the copies
cannot be redeemed (loot card -> EQ item), opened or posted twice while listed. Cosmetic cost: the
poster's "own" column for their own offered cards drops (only they see it).

### 8.2 Tables (schema migration; no FKs, no CASCADE)
```sql
CREATE TABLE IF NOT EXISTS `lon_posted_trade` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,          -- the PostedTrade id (+0x08); never 0
  `account_id` int unsigned NOT NULL,                 -- poster (+0x0C)
  `poster_name` varchar(64) NOT NULL DEFAULT '',      -- +0x2C (and +0x10)
  `recipient_account_id` int unsigned NOT NULL DEFAULT 0,   -- +0x64
  `recipient_name` varchar(64) NOT NULL DEFAULT '',   -- +0x68 / +0x84
  `guild_id` int unsigned NOT NULL DEFAULT 0,         -- +0xA0
  `guild_name` varchar(64) NOT NULL DEFAULT '',       -- +0xA4
  `state` tinyint unsigned NOT NULL DEFAULT 0,        -- 0 open, 1 completed, 2 cancelled, 3 expired, 4 accepting
  `accepted_by` int unsigned NOT NULL DEFAULT 0,
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,   -- +0x48 = UNIX_TIMESTAMP(created_at)
  `closed_at` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `open_by_account` (`state`,`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `lon_posted_trade_line` (
  `trade_id` int unsigned NOT NULL,
  `side` tinyint unsigned NOT NULL,                   -- 0 offered (+0x4C), 1 wanted (+0x58)
  `archetype` int unsigned NOT NULL,
  `quantity` int unsigned NOT NULL,
  PRIMARY KEY (`trade_id`,`side`,`archetype`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- escrow: the exact copies held for an open trade
CREATE TABLE IF NOT EXISTS `lon_posted_trade_escrow` (
  `card_id` int unsigned NOT NULL,                    -- lon_card.id = the InstanceNumber
  `trade_id` int unsigned NOT NULL,
  `archetype` int unsigned NOT NULL,
  `series` int unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`card_id`),
  KEY `trade_id` (`trade_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```
Escrow by moving the lon_card row out of the poster's collection: simplest is
`UPDATE lon_card SET account_id = 0 WHERE id IN (...) AND account_id = <poster> AND tradable = 1`
(check RowsAffected == count) plus an escrow row; on cancel/expiry set account_id back; on accept set it
to the acceptor. The instance id (= lon_card.id) survives the move, so RemoveInstances/StartDelivery
can name the same Inst on both sides. (Alternative: a `held_trade_id` column on lon_card, filtered out
of SendCollection/TakeCards/CopiesOf -- more places to touch.)

### 8.3 Handlers
- **AddPostedTrade**: decode; ignore every client-sent id/poster/time; validate: counts > 0, each side
  <= 50 copies (else 6), open public posts < 20 (else 0x0E; decide whether targeted ones count -- the
  client does not count them), recipient account exists if set (else 0x0D), guild exists if set,
  archetypes known in g_data.cards (unknown -> 6 or a generic code), wanted quantities sane (cap). Pick
  `quantity` TRADABLE copies of each offered archetype (`SELECT id ... tradable = 1 ... LIMIT q`), in
  one transaction insert header + lines + escrow and move the copies (`FOR UPDATE` / conditional
  UPDATE). Short on copies -> error (no exact code: 0x0B or 6; INFERRED best fit 0x0B). Then send
  RemoveInstances(escrowed copies) and the AddPostedTrade echo; push Transfer+End to other listeners
  who can see it.
- **RequestPostedTrades**: Start(count), Transfer x n, End -- open trades visible to this account
  (section 6, using the account's EQ guild if we map guilds; otherwise guild-only trades go to the
  poster only).
- **AcceptPostedTrade** (atomic, double-accept safe):
  1. `UPDATE lon_posted_trade SET state = 4, accepted_by = A WHERE id = N AND state = 0` --
     RowsAffected != 1 -> code 3 (completed) or 1 (being completed) by the stored state. This single
     conditional update is the double-accept lock; a crash mid-way leaves state 4 for a startup sweep.
  2. checks: A != poster (client also checks), recipient/guild match (4 / 5), both accounts allowed (0x0C/0x0D/0x10).
  3. take the acceptor's wanted copies: tradable = 1 only, `SELECT ... FOR UPDATE`, count each archetype
     -> short = 8.
  4. one SQL transaction: wanted copies `account_id = poster`, escrowed offered copies
     `account_id = A`, delete escrow rows, `state = 1, closed_at = NOW()`. Any failure -> ROLLBACK,
     state back to 0, error 0x0B. (lon_service runs every query through `database.QueryDatabase`
     today -- add `START TRANSACTION`/`COMMIT`/`ROLLBACK` on the same connection, or a TransactionBegin
     helper if worlddb has one; world is single-threaded, which already serialises two accepts, but
     the conditional UPDATE keeps it safe if that changes.)
  5. acceptor: RemoveInstances(wanted) + StartDelivery(offered, "trade", -N) + Complete +
     CancelPostedTrade(N). Poster online: StartDelivery(wanted, "trade", -N) + Complete +
     CancelPostedTrade(N). All other listeners: CancelPostedTrade(N). Poster offline: nothing to send;
     optionally a lon_delivery-style "notify at login" row so the Delivery window pops next login.
  6. CheckProgress for both (Journal has "Complete a Posted Trade as Seller / Buyer" achievements --
     strings 0x10671694 / 0x106716E4).
- **CancelPostedTrade**: `UPDATE ... SET state = 2 WHERE id = N AND account_id = me AND state = 0`;
  RowsAffected == 1 -> move escrow copies back, StartDelivery("trade", -N) + Complete to the poster,
  CancelPostedTrade(N) to poster + listeners.
- **SetListenStatus**: set/clear `Session::listening`.
- **Expiry** (our policy, client has none): in Process(), trades older than a rule (e.g. 14 days) get the
  cancel path (escrow back, push CancelPostedTrade).
- **GetAccountsByPartialName / GetGuildsByPartialName**: needed only for targeted trades. Without a
  reply the builder waits forever on a targeted post. Either implement (look up our EQ account /
  character names; reply ids + names + names2 in the zlib block) or answer with an empty result
  ("Account Not Found") until we want the feature.

### 8.4 Loot cards, foils, non-tradable copies
- Foils: a foil is its own archetype (base + 300000); the maps carry it as a separate key and the
  client resolves the foil printing itself (`FUN_10044eb0`). Nothing special server-side -- just never
  match a foil copy against a non-foil want or the reverse (match on archetype id exactly).
- Loot cards (EQ items, archetypes >= 100M): tradable on live; the client shows them gold (#FFCC66,
  type 7/8) and warns when they are WANTED. Value risk: a traded loot card becomes an EQ item for the new
  owner. Decide per policy: allow, or refuse loot archetypes (no client error text fits; 6 or 0x0B).
  Escrow is what stops a poster redeeming a listed loot card -- RedeemItemGrant's CopiesOf/TakeCards
  must never see escrowed copies (they will not if account_id moves to 0).
- Non-tradable copies (`lon_card.tradable = 0`, e.g. starter gifts): never escrow or take them. The
  client's own-count (`FUN_100747b0` = instances minus a second list at CollectionItem+0x24) may count
  differently from ours, so the server check is the only authority (error 8 / short-on-post).
- Products (packs, starter decks) are archetypes too and tradable unless we say otherwise.

### 8.5 Where the client gives us no choice
- An open-window request must end with `EndPostedTrades`, or the list never fills.
- The post only shows (and the builder only closes) when the server echoes AddPostedTrade with a NEW id.
- A row only disappears via CancelPostedTrade (also the only way to close an open detail dialog).
- Error codes/texts are fixed (section 7); there is no free-text error.
- Trades are by archetype + quantity; the client never names instances -- the server picks copies.
- Ids must be non-zero and unique (store map key; the model skips an id already present).

---------------------------------------------------------------------------------------------------
## 9. Open questions
1. PostedTrade +0x10 (v2 string): purpose unconfirmed (no UI reader found). Sending the poster name is safe.
2. +0x68 vs +0x84: which of the account lookup's two name vectors is the account name vs the
   character name -- matters only for targeted trades.
3. Archetype "type" 7/8 (gold, warning 0x14) -- INFERRED loot cards; message 0x14's text not pulled.
4. Whether pushing AddPostedTrade to a listener with an open builder really closes it (hence 4.6's Transfer+End).
5. TransferPostedTrade's vector element encoding is inferred; the single-slot form avoids it.
6. Policy calls: escrow (recommended), per-side cap, targeted-trade limit, expiry length,
   loot cards tradable or not, guild-only trades (map to EQ guilds?).
