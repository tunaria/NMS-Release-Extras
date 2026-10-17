# Legends of Norrath — single-player revival

**Status: PROVEN AND PARKED (2026-08-13).** The Tutorial plays. A service written
from scratch gets the real LoN client through its entire login sequence and into
a working lobby with every menu enabled.

Parked by choice, not by a blocker. The remaining work is content plumbing.

> Full technical reference lives in **`LoN.h` §55**, with two companion data files in the same folder:
> `LoN_opcodes.txt` (488 classes) and `LoN_events.txt` (319 UI event posts).
> This document is the human-readable handover; `LoN.h` is the detail.

---

## 1. What this is

Legends of Norrath was SOE's in-EverQuest trading card game (2007–2016; last card set August 2013, servers closed by Daybreak 2016-08-17 - 'no active development team'). When the
service died, everyone assumed the game died with it.

It didn't. **The entire game is still on disk** in the RoF2 client — 645 MB of it.
What died was a doorman: login, entitlements, and your card collection.

```
EverQuest - RoF2 Client/LegendsOfNorrath/
    cards.rcc              408 MB   every card's art
    effects.rcc             83 MB
    resources.rcc           81 MB
    sounds.rcc              35 MB
    foil.rcc                18 MB
    LegendsOfNorrath.dll   8.4 MB   the entire game engine
    data/archetypes/*.dat   13 MB   THE RULES ENGINE'S DATA
    data/decks/*.eqd                 starter decks
    data/collections/default.cln     a shipped starter collection
    data/campaign.dat                scenario definitions
    data/tutorial/
```

Critically, **the AI opponent is client-side** — `..\rules\controller\eqaicontroller.cpp`
is compiled into the DLL right next to `EQPlayerController`. And
`AccountCommand_RecordSinglePlayerGame` exists, which means solo games are *played
locally and merely reported*. That is the whole argument for feasibility, and it
held up.

**Only the RoF2 client copy has this folder.** Live, THJ and ToB were stripped
after the shutdown. RoF2 is the only surviving source.

---

## 2. What actually works

Verified against the real client, and confirmed in the client's *own* trace log:

| Step | Status |
|---|---|
| Gateway handshake (redirect to our host) | working |
| Login accepted | working |
| Account introduce (with property set) | working |
| MOTD window | working — we can drive client UI |
| Full collection sync | working |
| Reaches the lobby, **all menus enabled** | working |
| **Tutorial plays** | **working** |

"All menus enabled" matters: an account with **zero restrictions** satisfies the
entitlement gates. We never had to fake a subscription or a purchase.

### Two known rough edges

1. **The login dialog does not auto-dismiss.** It sits on top of a fully live
   lobby — click its X and everything behind it works. Some event the real
   server posted is still missing. Cosmetic, not a gate. Ruled out already: it's
   not the sync, not the account, not the MOTD; all of those are received and
   executed cleanly.
2. **Everything except the Tutorial crashes.** Near-certainly because we serve an
   **empty collection** — no cards, no decks. See "next steps".

---

## 3. Getting it running again

Everything below is already done and left in place, so this is mostly a checklist
to verify rather than redo.

### 3.1 Point the client at your service

The host is a hardcoded string in the DLL:

```
lvsccg-02-01.station.sony.com     gateway port 15783, game port 15782
```

Three ways to redirect, best first:

1. **hosts file** — `127.0.0.1 lvsccg-02-01.station.sony.com`. Leaves the DLL
   byte-original. Needs admin. Best for anything shippable.
2. **Patch the literal** at file offset `0x62B8D8` (one occurrence; `127.0.0.1`
   is shorter so it drops in with NUL padding). No admin. **This is what is
   currently done** in the client folder's `LegendsOfNorrath\`. A byte-verified
   original copy is kept, so `robocopy` restores it.
3. Live memory write — **does not work once connected**; NetworkService copies
   the host at construction, so a late write is ignored and you get
   "Login host not found". Learned the hard way.

Not limited to 29 chars, either: the assignment takes a literal *address*, so a
longer hostname can be parked in padding and the operand repointed.

### 3.2 Start the service

The original prototype was a Python service plus wire codec (internal tool, not included).
It listened on 15782 and 15783 and loaded the 488 opcode names from `LoN_opcodes.txt`
so everything was logged by name.

### 3.3 Turn on the client's own trace  ← do this first, always

**The client narrates itself.** This was the single biggest accelerator in the
whole project and it was sitting there the whole time.

- **`<client>\wa_error.log`** — written automatically. Names every failed assert
  with **file and line**. Check it after every test.
- **`<client>\wa_network.log`** — a full protocol trace, gated behind a flag:

```
logger singleton pointer :  DLL base + 0x78BE78
enable flag              :  *(singleton + 0x1C) = 1
```

Flip it with Cheat Engine and the client logs every message it sends and
receives, decoded by name and field:

```
RECEIVED AND EXECUTING:
Login Results
Login Succeeded: 1
Account ID: 1
Session ID: NMS-LON-SESSION
```

**Gotcha:** the logger object is destroyed and recreated every time the LoN
window opens, so the flag must be set *after* the window is open and *before*
you click Login.

---

## 4. The protocol, in brief

Plain TCP. No TLS, no HTTP, no compression at the transport layer, no obfuscation.

**Framing** — 4-byte big-endian length, then the payload.

**Integers** — a custom variable-length encoding:

```
byte0 = (magnitude & 0x0F) | (((extraBytes * 2) + negFlag) << 4)
then extraBytes LITTLE-ENDIAN bytes of (magnitude >> 4)
magnitude = min(v, ~v) unsigned;  negFlag = (~v < v)
```

**Strings** — `varint(length)` then `length + 1` bytes: **the NUL is on the wire.**
An empty string is two bytes, `00 00`.

**Message layout** — `[varint opcode][varint version]` repeated once per class
level in the inheritance chain, then the fields in declaration order.
The count varies (3 for the login/gateway commands, 2 for AccountCommand, 1 for
Ping) and the version is a **per-class constant**, not protocol-wide.

**Embedded objects** — `varint(flag)` (1 = absent, 0 = present), then
`varint(classId)`, then the object body. The classId resolves through the *same*
registry as network commands, which is why `LoN_opcodes.txt` is really a table of
every serialisable class.

**The 12-byte preamble** — the client *sends* three big-endian words ahead of the
payload on connection-server sockets, but its *receive* path does not expect
them. Strip on input, never send one.

### Finding any command's format

This recipe worked every single time:

1. Find the class's **debugDump** virtual — it `sprintf`s every field **by name**
   (`"Account ID: %d\n"`).
2. Read its **serialize** — same fields, declaration order.
3. The wire **version** is the 4th virtual after the class-name virtual, spaced
   `0x10` apart: name, getDAT, setDAT, version.

---

## 5. Next steps, if this ever gets picked up

**Serve a real collection.** This is the one job that matters, and it's plumbing
rather than reverse engineering:

- `SynchronizationCommand_SendInstances` (0x55 v2) carries the card instances.
- The vector encoding is `varint(count)` then one element per `0x10` bytes, each
  written through its own virtual at vtable slot `+0x28`. An empty vector is a
  single `0x00` — which is exactly what we send today.
- The element type is `CollectionItem`, **class 0x34**. Its per-field layout is
  the one piece still undecoded.
- The source data is already on disk: `data/collections/default.cln` (the shipped
  starter collection) and `data/decks/*.eqd`.

Expected payoff: Skirmish and Scenarios stop crashing, because the client would
finally have cards and decks to work with.

**Secondary:** find whatever event dismisses `WASessionLoginDialog`. Low value —
the X button works.

---

## 6. Traps — do not repeat these

- **`--host=` is the hosting MMO (`eq` / `eq2`), NOT a network address.** Different
  thing entirely.
- **There is no command-line switch for the network host.** The override globals
  in NetworkService's constructor are read-only — no writer exists anywhere in
  the DLL. Dead dev code.
- **A non-production `--realm=` is not a redirect either** — it skips the host
  assignment entirely and trips an assert.
- **There are TWO login dialogs.** `WALoginDialog` ("loginDialog") is *not* the one
  on screen after a session login — that's `WASessionLoginDialog`. Chasing the
  wrong one cost a pass.
- **Main-window events 0xAE / 0xAF are the leaderboards**, not the lobby.
- **Static xrefs dead-end on Qt.** The main window's setup and `showLoginDialog`
  are reached only through `qt_metacall` slot tables. Use the custom-event bus
  instead (command executes post Qt custom events; widgets filter on the ID).
- **Omitting an embedded object's classId crashes the whole EQ client** — the
  client reads the next byte as a class ID ("Couldn't get class 0").
- **`AccountCommand_IntroduceAccount`'s property set is MANDATORY** even though
  the writer supports NULL. Sending "absent" makes the client discard the entire
  command silently. This was the real blocker for hours.
- **Never answer `SynchronizationCommand_Update` more than once per sync.** It's an
  acknowledgement, not a request. Answering each one produces a reply loop that
  floods and freezes the client within seconds. Equally, a *fresh login on an
  existing connection* must reset that guard, or the sync never finishes.
- **The DLL file locks while eqgame has it mapped.** Close the LoN window before
  patching it.
- The DLL is a **shared codebase with the Free Realms TCG** — it's full of "Free
  Realms Trading Card Game" strings. Don't read those as evidence about LoN.

---

## 7. Distribution, if it ever ships

Nothing here requires `dinput8.dll` — the host string is the only client-side
change. That keeps this a standalone preservation project rather than something
welded to the client mod.

Preferred shape: **a small patcher tool** that backs up the original DLL, writes
the user's host, and can restore. One-time, no admin, fully reversible — and the
backup *is* the preservation copy. Runtime config (reading `eqhost.txt`) would
need a code cave injected into a 2009 Qt binary; a lot of risk for a value that
changes approximately never.

---

## 8. Tooling

Ghidra project `lon_proj`, program
`LegendsOfNorrath.dll` (imported with full analysis).

Scripts in `ghidra_scripts_lon{,2,3,4,5,6}\` (internal tools, not included):

| Script | Does |
|---|---|
| `LonProbe.java` | find strings, dump referencing functions decompiled |
| `LonXref.java` | xref given addresses (takes addresses as args) |
| `LonVtable.java` | dump a class's vtable slots, decompiled |
| `LonRange.java` | decompile every function in an address range |
| `LonOpcodes.java` | extract the whole class registry → `LoN_opcodes.txt` |
| `LonEvents.java` | map UI event IDs to posting commands → `LoN_events.txt` |

Own folder per batch — Ghidra compiles a script directory as one OSGi bundle, so
one stale sibling breaks every script in it.

---

## 9. 2026-10-06 session -- COLLECTION, DECKS AND AI MATCHES WORK

Working end to end against the real client (prototype service, (internal tool, not included)):
login (any name) -> full collection -> 44 starter decks -> Skirmish vs the AI played
to the end. No new `wa_error.log` lines once the nameless decks were dropped.

**Collection format** (prototype script, not included; checked by parsing `default.cln` end to end):
- `InstanceID` class 0x15E v1 = one copy: ArchetypeID, SeriesID, InstanceNumber (write RVA 0x12B880).
- `CollectionItem` class 0x34 v7: ArchetypeID, QuantityDesired, Foils, Instances[], NonTradableInstances[]
  (write RVA 0x75D80). Data classes carry ONE header level (commands carry three).
- `.cln` = class 0x33 v3: Name, 4 ints, map { ArchetypeID -> CollectionItem }, trailer.
- `default.cln` = every product x16: 6,660 cards, 1,621 loot cards (EQ + EQ2), every pack and
  starter deck -- 9,445 archetypes, 151,204 copies. Packs/decks/loot cards ARE archetypes.
- `SendInstances` carries InstanceIDs directly; the client files them per archetype itself
  (handler RVA 0x1B02B0 -> 0x1B39D0). Base/Count only feed the progress bar. Sent in 19
  chunks of 500 archetypes (~73 KB each) -- fine.

**Decks** (server-stored on live: "Online Decks"):
- `.eqd` file == a serialised `EQDeck` (class 0x11172, two header levels, v8/9/10; client writes 10).
- `DeckCommand_PopulateOnlineDeckData` 0x16B v1 (server->client): x3 headers, NumberOfDecks,
  per deck varint(0) + the EQDeck bytes (write RVA 0xA6200).
- `DeckCommand_AddOnlineDeck` 0x169 v1 (client->server): len, raw EQDeck bytes, Archetype
  (write RVA 0xA5380). The prototype saved them to disk.
- **TRAP:** a deck with an EMPTY name asserts (`DeckDB.cpp:194`, RVA 0xA8770 compares with "")
  and Select Deck then CRASHES the whole EQ client. 8 shipped files have no name
  (1900002-5, *Starter.eqd) -- skipped. 44 remain, all names unique.

**Single-player report** `AccountCommand_RecordSinglePlayerGame` 0x1E7 v1, sent at game START and END
(fields at +8..+0x1C, debugDump RVA 0x100B0):
- ActivityType 1 Begin / 2 End; GameType 1 Tutorial / 2 Scenario / 3 Skirmish;
  Result 1 Win / 2 Lose; TurnsAtFinish; UniqueGameId (= start unix time, same in both);
  GameScriptName ("aiPriest", or the scenario's script). -> the hook for once-only rewards.

**Other client->server traffic seen, no reply needed:** `LobbyCommand_ChangeStatus` (3 lobby,
5 in game, 6/11 elsewhere), `AccountCommand_SetPreferences`, `LobbyCommand_Leave`,
`SynchronizationCommand_ProcessBulkFulfillment` (one InstanceID of a product = "unpack this";
seen once for starter deck 1900005, cause unconfirmed -- the pack/deck-open hook).

**Design (10-06):** no login server -- the responder moves into dinput8.dll (127.0.0.1 on
every player's own PC); identity = the EQ connection; anything of value (collection, packs,
loot redemption, trades, scenario rewards once per account) goes through the zone server and
the DB. Loot cards map to our items by name (6 of 7 checked exist, e.g. 40610, 40615-40622).

### 9.1 Auto login (NMS-Client 43d14b6, dinput8 md5 37f6397e, 10-06) -- WORKS
- eqgame passes `--username=<account>` and `--sessionID=` (setters eqgame VA 0x7A14B0 / 0x7A1460,
  buffers 0xF802C8 / 0xF800D0, argv built at 0x7A192E; eqgame is RELOCATED -- use base + (VA-0x400000)).
  Live's Station login filled the session id; ours is always "". The DLL REPLACES 0x7A1460 (writes
  `--sessionID=NMS` itself). **TRAP:** detouring it and calling the trampoline crashed eqgame at
  startup -- its copy loops jump back to 0x7A1462, inside the 5 patched bytes.
- With a session id the client sends `LoginCommand_SendSessionID` 0x19B v7 (fields seen: 'NMS', 3, 1,
  1745, 2, 0, 4, 1, 'winVista', '', 1, 'EQ', 'en_US', '<character>', '', '') -- the service answers it
  like a password login. Gateway already carries SessionID + Username.
- The `WASessionLoginDialog` box ("Entering main lobby...") only closes itself on FAILURE
  (QWidget::close in LoN RVA 0x4A6E50's failure path). DLL detours 0x4A6E50 from the pulse once
  LegendsOfNorrath.dll is loaded (bytes checked: 6A FF 68 / getter 56 8B F1) and, on success with reason
  != 0x4C, calls `QTimer::singleShot(0, box, "1close()")` (QtCore4 export). Event fields via LoN RVA
  0xACAA0 (__thiscall(event, idx); type at +8, value at +4).
- MOTD no longer sent (the prototype's SEND_MOTD switch is off).
- Casual Games / Tournaments = player vs player: need ONE shared service both clients reach (lobby
  groups, LobbyCommand_Join/AddGroups, match relay). eqgame already relays TCG:CHALLENGE:/ACCEPT: between
  players. Parked.

### 9.2 Opening packs / starter decks -- WORKS (10-06, prototype)
- client -> `SynchronizationCommand_ProcessBulkFulfillment` 0x139 v1: x3 headers, Instances[] = the product
  copies to open (write RVA 0x1AF0F0). Right-click > open in Collection Manager sends it.
- server -> `SynchronizationCommand_RemoveInstances` 0x8F v1: x3 headers, Instances[] (write 0x1AFA60) -- the pack leaves.
- server -> `SynchronizationCommand_StartDelivery` 0x8D v6 (write 0x1B1BF0): ArchetypeCount, CollectionModID,
  Silent, DeliveryID, TransactionID, TransactionSource (string), TransactionSourceID, TransactionSourceID2,
  Instances[], NonTradableInstances[]. Handler (sync service RVA 0x1B4C50) ADDS the copies itself and raises UI
  events 0x1E/0x1C -> with Silent=0 the client shows its own **Delivery window** ("Transaction ID: #n,
  Number of items: 15", card reveal). Then `Complete` 0x56 with the same DeliveryID.
- Product archetypes < 20M: N900001 = set N booster, N900002-5 = set N starter decks (== the .eqd file number),
  a few others (N200010...). Cards/avatars/quests/loot >= 99M. Deck card list = varint 50 then 50 (card, 0) pairs.
- TEST contents only: starter deck = its 50 cards; booster = 15 random from that set's starter-deck cards.
  REAL boosters need rarity per card + the set's pack layout (archetype data, not decoded yet).
- Ghidra has the LoN DLL analysed (project lon_proj) with RTTI vtable names -- decompile through the MCP.

### 9.3 Card data decoded (10-06) -- real boosters
- `data/archetypes/index.dat`: varint 0, varint timestamp, then (ArchetypeID, compressedSize,
  uncompressedSize, offset). `storage.dat`: one zlib record per archetype = EQArchetype (class 0x11195 v7,
  loader RVA 0x1003D060): headers x2, id, int, int, then flag 0 + class 0x13 + ver + PropertySet (0x17 v1,
  count, {key, ValueData 0x16 v4: type, flag, value}) -- type 2 int, 3 string, 6 int list. All 6,027 parse.
  Code: a prototype script (not included); its catalog dump holds 5,964 cards.
- Keys: 1 name, 5 subtype ("EverQuest Loot Card" / "EverQuest II Loot Card" / "Legends of Norrath Loot
  Card"), 28 card type (70061 item, 70062 ability, 70064 unit...), 73 set, 74 collector number,
  **76 rarity** (filter panel RVA 0x25DA60: 70035 Common, 70036 Uncommon, 70037 Rare, 70049 Ultra Rare,
  70038 Promo, 70039 Fixed), 576 cost/level, 630 loot reward text, 643 flavour, 71177 archetype (70001-5).
- Live booster (Allakhazam): 15 = 11 Common + 3 Uncommon + 1 Rare, the Rare sometimes a Loot card.
  Prototype: LOOT_CHANCE 1/20 (EQ loot only, never EQ2), ULTRA_RARE_CHANCE 1/8 (sets 5-7 have URs) --
  tunable. Unknown products (e.g. 900001 Event Pass) are left in the collection.
- Loot -> item: 401 EQ loot cards / 355 names; **295 match a peq item by exact name**
  (prototype output, not included); 60 need hand mapping (later-set items, "EverQuest 1-Month Subscription").

### 9.4 Loot redemption -- Choose-a-Pack WORKS (10-06, prototype; a booster was redeemed in testing)
- client -> `ECommCommand_RedeemItemGrant` 0x1B9 **v3**, TWO headers (derived + ECommCommand), then
  ECommCommand's AccountID, then (read RVA 0xB5A90 / write 0xB5B80 / dump 0xB5830):
  `ArchetypeID` (the loot card, +0x08), `Properties` (embedded PropertySet, +0x10), `Response` bool (+0x14),
  `ErrorCode` (+0x18), `IsConversion` bool (+0x1C), `AlternateArchetypeID` (+0x0C, v>=2 -- the product the
  player CHOSE), `TradableState` (+0x20, v>=3; ctor default -1).
  Captured: card 100019624 "Legends of Norrath Choose a Pack", choice 6900001 (Against the Void booster),
  Properties {0x802: "1"} = the quantity drop-down. Physical-loot cards also send name/address/phone/email
  props 0x1042-0x104B and `/redeem/%1` form fields (dialog `WARedemptionConfirmDialog`, RVA 0x49B2C0).
- The dialog listens for UI event **0xA3** (eventFilter RVA 0x49A150 -> vtable +0x104 = RVA 0x49A9C0), which the
  command's execute (RVA 0xB56C0) posts with (Response, ErrorCode). Response=1 -> the dialog closes;
  Response=0 -> it shows ErrorCode through the client's message table (RVA 0x46C940, see `LoN_errors.txt`).
- Service reply that works: the same command echoed with Response=1, then RemoveInstances (one loot card per
  unit), StartDelivery (the product, Silent=0) and Complete. EQ-item grants get ErrorCode 0x7A ("not yet
  available") until loot redemption goes through the zone server; a missing choice gets 0x78.
- The 12-byte send preamble's first word is the SERVICE: 0x3B9ACA00 for most commands, **0x3B9ACA04 for
  ECommCommand** (vtable slot 21, RVA 0xB47C0). A shared service has to route on it.

### 9.5 Network-command vtable layout (every *Command class)
  0 dtor | 1 ? | 2 writeHeader | 3 writeFooter | 4 readHeader | 5 readFooter | 6 classId | 7 classId |
  8 **version** | 9 **deserialize** | 10 **serialize** | 11 className | 12 getDAT | 13 setDAT | 14 ? |
  15 **execute** | 16 ? | 17 create | 18 clone | 19 **debugDump** (names every field) | 20 description |
  21 preamble words (service id).
  The full per-message tables generated from this are in `LoN_messages.txt`.

### 9.6 The whole protocol, generated (overnight 10-06)
- `LoN_messages.txt` -- all 498 serialisable classes, fields in wire order with the client's own labels,
  enum values and version gates. `LoN_eventflow.txt` -- which command posts each UI event and which widget
  handles it. `LoN_errors.txt` -- the error-code text table. (LoN.h section 55.14 has the method.)
- A generic codec (internal tool, not included) decodes and encodes ANY message from a JSON form of `LoN_messages.txt`;
  every client message captured so far decodes completely.
- `LoN_opcodes.txt`'s class names are wrong for ~220 rows (base classes); `LoN_messages.txt` re-verified them.

### 9.7 Campaigns, scenarios and their rewards (data/campaign.dat decoded)
- `data/campaign.dat` = varint count (78) + 78 `Campaign` objects (0x1C5 v4) holding 179 `CampaignScenario`
  objects (0x1C6 v7). Parses completely with the generic codec (internal tool, not included). Table: `LoN_campaigns.txt`.
- Each campaign has an **AccountKey** (78011...78123): the account property that unlocks it (the adventure
  pack entitlement). Three optional scenarios carry their own key (78015-78017).
- **LIVE's reward rules, word for word from the scenario summaries:**
  - "Receive one reward for winning this scenario with each different archetype on Easy, Medium, or Hard
    difficulty (12 possible). Rewards for Hard difficulty are foil cards." (a reward CARD per scenario)
  - Later sets add a **loot card**: "Receive one loot card (based on your loot preference) the first time
    you win this scenario with any archetype." 47 scenarios do this (EQ side: Ping's potion packs, Discordian
    and Void weapon ornaments, nimbuses, familiars, visages, Unscribed Glyph, Tear of Tranquility...).
  - Some reward cards depend on the archetype: "X (Fighter, Scout, or Generic) or Y (Mage or Priest)".
  This answers "how did people get loot from AI fights": first-win loot cards on scenarios, not skirmishes.
- 95 of 166 reward lines resolve to card ids in the decoded card catalog (shown as `-> name=id`).
- The client reports results with `AccountCommand_RecordSinglePlayerGame` (0x1E7: Activity 1 begin / 2 end,
  GameType 1 tutorial / 2 scenario / 3 skirmish, Result 1 win / 2 lose, turns, unique game id, script name)
  and `AccountCommand_SetCampaignStatus` (0x19F: Campaign ID, Status, Difficulty Level, CustomInt). A server
  that pays rewards keys off the scenario's script name + archetype + difficulty, once each.

### 9.8 Loot preference (which game's loot cards you get)
- Account property **0x4C4 (1220)**, int. Setter RVA 0x2D0370(value): writes it into the account's PropertySet
  and sends `AccountCommand_SetPreferences` (0x76) with a PropertySet {0x4C4: ValueData int}. Called from the
  player-tip buttons (`EQPlayerTipFactory`, RVA 0x4D424E) -- the tip text: "When you open a booster pack, you
  have a chance to get a special Loot Card ... Currently you will receive loot cards from %1, but you logged in
  from %2" (RVA 0x2D0530; the account object's +0xC0 holds the current value).
- A test client sent {0x4C4: 3} on 10-06 (first login with the tip). CONFIRMED (LoN 0x101E2BE0): `--host=eq` -> 3,
  eq2 -> 2, swg -> 4, fr -> 6; our clients pass --host=eq, so loot defaults to EverQuest. For an EQ-only server the service should force 0x4C4 to the EQ
  value in the account properties and ignore changes. Other readers of 0x4C4: EQCardSprite, EQGameScreen,
  WAItemGrantChoiceDialog (RVA 0x44F460), preferences dialog (RVA 0x3C8140).

### 9.9 PHASE 1 BUILT -- the server owns the cards (2026-10-06 overnight)
- NMS-Server: manifest v431 (lon_* tables, loot-card -> item map), world/lon_service.cpp (the whole service:
  token login, collection/decks from the DB, packs with live's layout, Choose a Pack, loot cards -> EQ item as a
  parcel, scenario rewards once per scenario x difficulty + first-win loot card, starter deck + 5 Choose a Pack a
  month), zone OP_NMSLoNSession 0x141C (the token).
  Rules Custom:LoN* (LoNEnabled is false by default).
- Selling packs: the bundled card game's Buy button does nothing (the store URLs are dropped). The receiving
  server must provide its own way to sell packs; products are granted by inserting into `lon_delivery`.
- NMS-Client: nms_lon_link.cpp -- gateway literal patch at load, relay 127.0.0.1:15782 -> server, token into
  --sessionID=.
- Data the service reads: the client's LegendsOfNorrath/data (archetypes, decks, campaign.dat) copied to the
  server's lon_data/. The Oathbound/Forsworn starter decks ship nameless; the service names them after the
  product so they can be listed.
- Tested with scripted clients (internal, not included) -- not yet with the real card game.
- Open: the hero archetype of a scenario win is not in the client's report; foil rewards; 60 unmapped loot
  cards; PvP (separate notes).
- **Scenario end (LoN 0x102C4850 loop):** the client sends `AccountCommand_SetCampaignStatus` once per difficulty from
  the one won down to 1 (CampaignID = game +0x414, Status = +0x418, Difficulty = loop counter, **CustomInt = game
  property "archetype"** -- 70001 Fighter, 70002 Generic, 70003 Mage, 70004 Priest, 70005 Scout (read off the
  starter decks' key 71177), string "open"/"standard"), THEN `RecordSinglePlayerGame` End. The service keys rewards
  on script x highest difficulty x archetype = live's "12 possible".
- **Foils:** a foil copy is its own archetype id = base + 300000. The client treats any id whose hundred-thousands digit
  is 3 or 4 as the foil printing of id - 300000 (isFoil LoN 0x100457B0; archetype lookup 0x10044EB0), and property
  0x31D = 7/8 marks always-foil printings. The archetype DB holds no foil ids; the client derives them.
- **Reward rules in the data** (all implemented in world/lon_service.cpp `815e2843c`): one reward per scenario x
  archetype x difficulty; winning on a harder difficulty also pays each easier one not yet won; Hard = foil;
  "Loot card(s) - A[, B ...] (EQ)" on the first win (Underfoot gives five); campaign 74 (Priestess of the Anarchs)
  pays its set for all 5 scenarios x 4 non-Generic archetypes, and the foil set when all on Hard.
