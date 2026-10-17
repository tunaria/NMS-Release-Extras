# Legends of Norrath — what a service must answer, feature by feature

Written 2026-10-06 overnight from the generated tables (`LoN_messages.txt`, `LoN_senders.txt`,
`LoN_eventflow.txt`). **WORKS** = proven against the real client tonight. **INFERRED** = read from the
client's code, never run. Field lists are in `LoN_messages.txt`; a generic codec (internal tool, not included)
builds and parses any of them.

Direction comes from `LoN_senders.txt`: a class that a `WA*`/`EQ*` UI class creates is sent by the
client. Game commands are the exception (see section 5).

## 1. Login and account — WORKS
Gateway `GetConnectionServer` 0x19E → reconnect → `LoginCommand_SendSessionID` 0x19B → server:
`LoginCommand_Results` 0x51, `AccountCommand_IntroduceAccount` 0x72 (AccountName, Properties, Gender),
`SynchronizationCommand_Start` 0x53. The client answers `Update` 0x57 once. Then StartInstances 0x54,
SendInstances 0x55 × n, Complete 0x56 and `DeckCommand_PopulateOnlineDeckData` 0x16B.
Client follow-ups that need no answer: `LobbyCommand_ChangeStatus` 0x89, `AccountCommand_SetPreferences`
0x76 (loot preference 0x4C4 lives here — README 9.8).

## 2. Collection, packs, loot — WORKS
- Open a product: client `ProcessBulkFulfillment` 0x139 → server `RemoveInstances` 0x8F, `StartDelivery`
  0x8D, `Complete` 0x56.
- Redeem a loot card: client `ECommCommand_RedeemItemGrant` 0x1B9 → server echoes it with Response=1, then
  the same three sync messages. Errors use `LoN_errors.txt` (0x7A "not yet available").
- Wanted counts: client `SynchronizationCommand_SetDesired` 0x10F (built by the collection UI, RVA 0x3CBB60)
  — INFERRED: store it; no reply seen in the code.
- Decks: client `DeckCommand_AddOnlineDeck` 0x169 / `DeleteOnlineDeck` 0x16A → server resends
  `PopulateOnlineDeckData`.

## 3. Single player and rewards — WORKS (reporting), rewards INFERRED
- The client runs Tutorial, Scenarios and Skirmish itself and only reports:
  `AccountCommand_RecordSinglePlayerGame` 0x1E7 (Begin/End, Tutorial/Scenario/Skirmish, Win/Lose, turns,
  game id, script name) and `AccountCommand_SetCampaignStatus` 0x19F (Campaign ID, Status, Difficulty).
- Live's reward rules, with the reward card of every scenario, are in `LoN_campaigns.txt`. To pay one, the
  server sends `StartDelivery` (Silent=0 shows the Delivery window).
- A client can fake these reports. Any server reward needs validation checks:
  once per scenario × archetype × difficulty, minimum turns, a Begin before the End with the same game id,
  and a plausible time between them.

## 4. Lobby, chat, friends — INFERRED
- Client: `LobbyCommand_Join` 0x5C (GroupID, RoleType, Password, AccountName, Gender), `Leave` 0x5D,
  `SendText` 0x61, `SendPing` 0x118, buddy/ignore requests 0x6D-0x6F / 0x11B-0x11D.
- Server: `BulkJoin` 0x6A (the group roster: maps built with `map::operator[]` — read its serializer RVA
  in `LoN_messages.txt` before building one), `AddBuddies` 0x6C, `NotifyOfBuddy` 0x121,
  `ChangeBulkStatus` 0x117, `UpdateLobbyStats` 0x19C, `GameServiceError` 0x8A.
- UI handlers: `WALobbyManager` takes events 3-6, 0x11, 0x19, 0x38, 0x46, 0x51-0x53, 0x7E, 0xDC
  (`LoN_eventflow.txt`).

## 5. Casual games (player vs player) — INFERRED
- Create a match: client `LobbyCommand_AddGroups` 0x5E (from `WACreateMatchDialog` / `WALobbyScreen`);
  join it: `LobbyCommand_Join`.
- In the match room: client `MatchCommand_SelectDeck` 0x65, `SetReady` 0x63, `SelectStartPosition` 0x66;
  server `MatchCommand_SetMatchState` 0x7C, `SetBulkMatchState` 0x126, `SetTimer` 0x127,
  `SetDeckArchetype` 0x1D6, then `MatchCommand_LaunchGame` 0x74 / `EQMatchCommand_LaunchGame` 0x11178
  (posts UI event 0x11 → `EQGame` / `WALobbyManager`).
- In game: server `GameCommand_SetupGame` 0x43 (Game ID, Game Server ID, Server ID, Player Count, Player
  Order, Ordered Account IDs, GameVersion, MatchID) and `EQGameCommand_SetupGame` 0x11177,
  `SelectDeckForPlayer` 0x3A, `SetPlayer` 0x41. After that every `GameCommand_*` is **both ways**. The
  engine, not the UI, builds them (which is why `LoN_senders.txt` lists none): each client sends its own
  plays, the server forwards them to the opponent, and `SynchPoint` 0x49 / `OutOfSync` 0x40 check that both
  engines agree. The rules code is the shared "Common" library (source paths
  `..\Common\common\...`), so a relay that forwards commands should be enough. Confirming that needs two
  clients and a shared service.
- Ending: `ConcedeGame` 0xA0, `OfferDraw` 0xA2 / `AcceptDraw` 0xA1, `RemovePlayer` 0x10B,
  `GameOverReached` 0x17F, `EndGame` 0x1D7. Adjournment: 0x107/0x108 and `LobbyCommand_*AdjournedGame`.

## 6. Trading — INFERRED
- Live trade: client `IssueTradeRequest` 0x93 → other client gets it → `AcceptTradeRequest` 0x94 → server
  `StartTradeSync` 0x110. Then `AddCardToTrade` 0x95 / `RemoveCardFromTrade` 0x97 (both sides),
  `AcceptTrade` 0x96, server `SetupFinalTradeAccept` 0x9C (Founder Card List, Trader Card List),
  `FinalAcceptTrade` 0x9B, `LockTrade` 0x111, `EndTradeSession` 0x99. The server moves the cards with the
  sync messages from section 2.
- Trading needs `TradeCommand_SetTradeAllowed` 0x15C, or the account shows error 0x5B "not flagged to
  allow trading".
- Posted (offline) trades: `AddPostedTrade` 0x18E, `RequestPostedTrades` 0x191, `StartPostedTrades` 0x192,
  `TransferPostedTrade` 0x193, `AcceptPostedTrade` 0x18D, `CancelPostedTrade` 0x18F, `EndPostedTrades` 0x190,
  `PostedTradeError` 0x195.
- **Value risk:** loot cards are tradable and become EQ items. A server must own every collection and
  check ownership on every trade. The client's word is never enough.

## 7. Not needed for our server
Tournaments, drafts, leagues (`LobbyCommand_SetTournament`/`SetDraft`/`*League*`), the web store
(`ECommCommand_OrderHistory`, `Order`, `OrderItem`), UChat, guilds, events and leaderboards
(`EventCommand_*`), avatar images and stadium pieces. All are in `LoN_messages.txt` if they are ever wanted.
