#pragma once
// ============================================================================
// SECTION 55: LEGENDS OF NORRATH (the in-client TCG) -- protocol + embedding
// ============================================================================
// Research started 2026-08-13. Goal: get SINGLE-PLAYER (Tutorial / Scenarios /
// Skirmish) working against a locally written service. PvP, trading,
// tournaments and the store are explicitly OUT OF SCOPE.
//
// Everything below is either (a) read out of the binary with Ghidra, or
// (b) confirmed against a LIVE process / a LIVE packet capture. Anything not
// yet verified is tagged [UNVERIFIED] -- do not build on those without testing.
//
// ----------------------------------------------------------------------------
// 55.0  WHERE IT LIVES -- AND WHICH CLIENT COPY HAS IT
// ----------------------------------------------------------------------------
// The LoN payload is a 645 MB folder, `LegendsOfNorrath/`, next to eqgame.exe.
//
//     ONLY the RoF2 sandbox client ships it:
//         <RoF2 client folder>\LegendsOfNorrath\
//     The Live, THJ and ToB client copies have NO such folder -- it was stripped
//     after the 2013 shutdown. RoF2 is the only surviving source.
//
// The EQ-side UI is stock and present in every client:
//     uifiles/default/EQUI_TCGStartupWnd.xml    the splash (Learn/Play/Challenge)
//     uifiles/default/EQUI_TCGWnd.xml           the 590x470 host window
//     uifiles/default/LON.tga                   its art
//
// Payload inventory (the game is ENTIRELY local -- this is the whole point):
//     cards.rcc                 408 MB   every card's art
//     effects.rcc                83 MB
//     resources.rcc              81 MB   UI forms/images
//     sounds.rcc                 35 MB
//     foil.rcc                   18 MB
//     LegendsOfNorrath.dll      8.4 MB   the entire game engine
//     data/archetypes/*.dat      13 MB   THE RULES ENGINE'S DATA
//     data/decks/*.eqd                   starter decks
//     data/campaign.dat                  scenario definitions
//     data/collections/default.cln       a shipped starter collection
//     data/tutorial/{index,objects}
//     Qt{Core,Gui,Network,Script,Svg,Xml}4.dll + Microsoft.VC80.CRT
//
// It is a Qt 4 / VC80 application from ~2009. *** VERIFIED 2026-08-13: it
// still loads and renders correctly inside eqgame.exe on Windows 11. *** The
// private Microsoft.VC80.CRT assembly in the folder satisfies the SxS bind.
//
// ----------------------------------------------------------------------------
// 55.1  THE EMBEDDING CONTRACT -- eqgame HOSTS IT, IT DOES NOT OWN A WINDOW
// ----------------------------------------------------------------------------
// LoN renders into offscreen surfaces and eqgame blits them into TCGWnd. All
// 13 of these are resolved BY NAME (the strings are in eqgame.exe), so a
// standalone host exe is entirely possible -- the surface is this small.
//
// Export RVAs (DLL image base 0x10000000; the DLL relocates, use RVAs):
//     Initialize               0x1E78B0
//     RunFrame                 0x1E7940
//     Shutdown                 0x1E7D00
//     GetWindows               0x1E7970
//     GetWindowSurfaceData     0x1E7A50
//     GetWindowRepaintRects    0x1E7A10
//     GetCaptureWindow         0x1E79B0
//     GetCurrentCursor         0x1E79E0
//     OnMouseEvent             0x1E7AA0
//     OnMouseWheelEvent        0x1E7B00
//     OnKeyEvent               0x1E7B60
//     OnFocus                  0x1E7BB0
//     OnWindowStateChanged     0x1E7BF0
// Also exported but NOT referenced by name in eqgame.exe:
//     OnMMODisconnect 0x1E7C50, OnMMOSystemMessage 0x1E7C60,
//     OnMMOSoundLoaded 0x1E7C80, OnMusicCompletion 0x1E7C40,
//     OnSoundCompletion 0x1E7C20, SetAvatarImage 0x1E7CA0,
//     SetPortraitImage 0x1E7CC0, SetVolume 0x1E7CE0
//
// Command-line arguments the DLL parses (in Initialize, FUN @ RVA 0x1E2BE0):
//     --realm=      production | stage | plattest | development   (see 55.3)
//     --host=       eq | eq2      *** THIS IS THE HOSTING MMO, NOT A NETWORK
//                                 HOST. Do not confuse the two. ***
//     --username=  --ticket=  --sessionID=  --character=  --characterID=
//     --challenge=  --post-login=   (eqgame.exe contains "--post-login=tutorial")
//     --lang=
//
// eqgame.exe side: `CMD_TOGGLE_TCG`, and player-to-player challenges relayed as
// plain messages -- `TCG:CHALLENGE:` `TCG:ACCEPT:` `TCG:DECLINE:` `TCG:CANCEL:`
// `TCG:DECLINE_AUTO:` `TCG:DECLINE_PLAYING:`. That relay is the ONLY LoN
// surface an EQ server ever had, and it is irrelevant to single-player.
//
// ----------------------------------------------------------------------------
// 55.2  NETWORK ENDPOINT -- GLOBALS, PORTS, AND HOW TO REDIRECT
// ----------------------------------------------------------------------------
// Set at the end of Initialize (RVA 0x1E2BE0). *** VERIFIED LIVE via Cheat
// Engine 2026-08-13 with the DLL loaded at 0x4B6F0000. ***
//
//     RVA 0x78C38C   std::string  sGatewayHost  = "lvsccg-02-01.station.sony.com"
//     RVA 0x78C3A0                  ^ its _Mysize (29)   [obj+0x14]
//     RVA 0x78C3A8   std::string  sDefaultHost  = "lvsccg-02-01.station.sony.com"
//     RVA 0x78C3BC                  ^ its _Mysize (29)
//     RVA 0x78C364   int          sGatewayPort  = 0x3DA7 = 15783
//     RVA 0x78C368   int          sDefaultPort  = 0x3DA6 = 15782
//
// MSVC std::string layout here: object base, union _Bx at +0x04, _Mysize at
// +0x14, _Myres at +0x18. Capacity is 31, so ANY replacement host <= 31 chars
// fits the existing heap buffer -- no reallocation needed.
//
// *** OBSERVED: the client's FIRST connection goes to port 15782 (sDefaultPort),
// NOT 15783. Do not assume "gateway port" means the first socket. ***
//
// The host string literal is at FILE OFFSET 0x62B8D8, ONE occurrence. This
// DLL's sections are mapped 1:1 (raw == virtual), so file offset == RVA here.
// "127.0.0.1" is shorter than the original, so it patches in place (NUL pad).
//
// THREE WAYS TO REDIRECT, in order of preference:
//   1. hosts file: `127.0.0.1 lvsccg-02-01.station.sony.com`. Leaves the DLL
//      byte-original. Needs admin. BEST for anything shippable.
//   2. Patch the literal at 0x62B8D8. No admin, survives restarts. The DLL
//      must not be loaded (the file locks while eqgame has it mapped).
//   3. Write the live globals with CE. *** DOES NOT WORK ONCE CONNECTED --
//      NetworkService copies the host into its own members at construction, so
//      a late write is ignored and you get "Login host not found". Learned the
//      hard way. ***
//
// There IS an override path in NetworkService's ctor (RVA 0x1739A0):
//     RVA 0x78C374  bool  useOverride
//     RVA 0x78C37C  ptr   overrideHost
//     RVA 0x78C380  int   overridePort
//     RVA 0x78C370  int   overridePort2
// *** These globals are READ-ONLY -- xref analysis found no writer anywhere in
// the DLL. Dead dev code. There is NO command-line switch for the network
// host. Do not go looking for one again. ***
//
// A NON-PRODUCTION --realm= SKIPS the hardcoded assignment entirely (jumps past
// it), which would leave the host empty and trip the assert at
// NetworkService.cpp:0x75. So `--realm=stage` is NOT a usable redirect either.
//
// ----------------------------------------------------------------------------
// 55.3  THE WIRE FORMAT   *** the important part ***
// ----------------------------------------------------------------------------
// Plain TCP. No TLS, no HTTP, no compression, no obfuscation.
//
// FRAMING:  4-byte BIG-ENDIAN total length, then that many bytes of payload.
//
// INTEGERS -- a custom variable-length encoding (writer at RVA 0x978C0,
// reader at RVA 0x97620):
//
//     byte0 = (magnitude & 0x0F) | (((extraBytes) * 2 + negFlag) << 4)
//     then `extraBytes` LITTLE-ENDIAN bytes of (magnitude >> 4)
//
//     magnitude = min(v, ~v);  negFlag = (~v < v)   i.e. set for negatives
//     extraBytes is 0..4, so the first byte's high nibble is 0..9.
//
//     Worked example, live-captured: `2E 19`
//         high nibble 2 -> extraBytes=1, negFlag=0
//         low nibble  E -> low 4 bits
//         value = 0xE | (0x19 << 4) = 0x19E = 414
//
//     There is a "wide mode" flag at RVA 0x78BF80: when non-zero, integers are
//     written as a plain htonl 4-byte big-endian value instead. Default is 0
//     (compact). [UNVERIFIED] what turns it on -- assume compact.
//
// STRINGS (writer RVA 0x979C0, reader RVA 0x97A00):
//     varint(length), then length+1 raw bytes -- *** THE NUL TERMINATOR IS ON
//     THE WIRE. *** The reader advances length+1 but assigns only `length`
//     bytes. An EMPTY string is therefore two bytes: `00 00`.
//     Reader rejects any length >= 0x1400001 ("Invalid string length: %d").
//
// Raw byte emitter used by both: RVA 0xE170.
//
// ----------------------------------------------------------------------------
// 55.4  THE OPCODE REGISTRY
// ----------------------------------------------------------------------------
// Every command class has a one-line registration function:
//
//     FUN(RVA 0x94970)(opcode, factory);      // primary registry
//     FUN(RVA 0x94A80)(opcode, factory);      // secondary (same pair)
//
// e.g. GatewayCommand_GetConnectionServer registers as:
//     register(0x19E, factory@0x117AF0);
//
// *** THE COMPLETE TABLE IS EXTRACTED: 488 opcodes, EVERY ONE resolved to a
// class name, in `LoN_opcodes.txt` next to this header. *** 523 registration
// sites yielded 488 distinct opcodes.
//
// To regenerate: find all callers of RVA 0x94970, decompile each, and read the
// (opcode, factory) pair. The class name comes from following the factory to
// its constructor and reading the RTTI/vftable symbol.
// See `ghidra_scripts_lon5\LonOpcodes.java` (internal tool, not included).
//
// The handshake subset that single-player actually needs:
//     0x19E  414  GatewayCommand_GetConnectionServer   <- first message sent
//     0x19D  413  GatewayCommand                        (base)
//     0x04D   77  LoginCommand                          (base)
//     0x04E   78  LoginCommand_GetChallenge
//     0x04F   79  LoginCommand_SendChallenge
//     0x050   80  LoginCommand_RespondToChallenge
//     0x051   81  LoginCommand_Results                  <- login verdict
//     0x19A  410  LoginCommand_SendPassword
//     0x19B  411  LoginCommand_SendSessionID
//     0x143  323  LoginCommand_GuestLogin               <- may skip station auth
//     0x135  309  LoginCommand_DisplayMOTD
//     0x129  297  AccountCommand_GetAccountInfo         <- entitlement flags
//     0x1E7  487  AccountCommand_RecordSinglePlayerGame <- stub it
//     0x034   52  CollectionItem
//     0x052   82  SynchronizationCommand                (collection sync base)
//     0x053   83  SynchronizationCommand_Start
//     0x054   84  SynchronizationCommand_StartInstances
//     0x055   85  SynchronizationCommand_SendInstances
//     0x056   86  SynchronizationCommand_Complete
//     0x168  360  DeckCommand
//
// Command families present (counts from RTTI descriptors):
//     LobbyCommand 55, GameCommand 43, AccountCommand 27, TradeCommand 25,
//     EventCommand 24, GuildCommand 19, UChatCommand 18,
//     SynchronizationCommand 17, NetworkCommand 15, LoginCommand 13,
//     MatchCommand 12, ArchCommand 7, PollCommand 4, DeckCommand 4,
//     ECommCommand 3, GatewayCommand 2, EQMatchCommand 1, EQGameCommand 1
//
// ----------------------------------------------------------------------------
// 55.5  GatewayCommand_GetConnectionServer  -- opcode 0x19E (414)
// ----------------------------------------------------------------------------
// The first message the client sends. Methods (RVAs):
//     register     0x117B50     factory      0x117AF0
//     ctor         0x1174D0     execute      0x1176E0
//     clone        0x117810     debugDump    0x117890
//     deserialize  0x1179C0     serialize    0x117A80
//
// Field map (from the debugDump at 0x117890 -- it prints each by name):
//     +0x04  std::string  SessionID
//     +0x20  std::string  Username
//     +0x3C  std::string  Host        <- server fills this in
//     +0x58  int          Port        <- server fills this in
//     +0x5C  bool         Success
//     +0x60  int          ErrorType
//
// Serialize order (0x117A80) == deserialize order (0x1179C0):
//     [class header] SessionID, Username, Host, Port, Success, ErrorType [footer]
//
// EXECUTE (0x1176E0) -- what the client does with our reply:
//     if (Success) { sGatewayHost = Host; sGatewayPort = Port; reconnect(); }
//     else         { error path }
// *** So our gateway can bounce the client to ANY host:port we choose. ***
//
// LIVE CAPTURE 2026-08-13 (client -> us, 30 bytes on port 15782):
//     00 00 00 1A                    length 26
//     2E 19 01  2E 19 01  2E 19 01   header: (414, 1) x3  -- see below
//     00 00                          SessionID = ""
//     07 41 41 41 41 41 41 41 00     Username  = "account"
//     00 00                          Host      = ""
//     00                             Port      = 0
//     00                             Success   = 0
//     27 05                          [UNVERIFIED] ErrorType=87, or a footer
//
// [UNVERIFIED] The header is the opcode+version pair repeated THREE times --
// once per class level (NetworkCommand / GatewayCommand / the derived class),
// each writing its own type id. The trailing `27 05` is either ErrorType or
// the footer written by vtable slot +0xC. Decompile the base-class write
// (vtable +0x08 and RVA 0x1173E0) and the footer (+0x0C) to settle both.
//
// ----------------------------------------------------------------------------
// 55.6  WHAT ACTUALLY GATES SINGLE-PLAYER
// ----------------------------------------------------------------------------
// The rules engine, the AI and all content are LOCAL:
//     ..\rules\controller\eqaicontroller.cpp     EQAIController::getInput
//     ..\rules\controller\eqraidaicontroller.cpp EQRaidAIController::getInput
//     alongside EQPlayerController -- same engine, different input source.
// `AccountCommand_RecordSinglePlayerGame` exists, which means solo games are
// PLAYED locally and only the RESULT is reported. That is the whole argument
// for feasibility.
//
// What a minimal service must therefore do:
//     1. Answer opcode 0x19E with Success=1 + a Host/Port to connect to.
//     2. Accept a login (LoginCommand_SendSessionID / _GuestLogin / _Results).
//     3. Return account info with ENTITLEMENT FLAGS set. Scenarios are gated:
//        "To access the scenarios, you must either make a purchase of a booster
//        pack, a booster box, or a starter deck, or subscribe to EverQuest or
//        EverQuest II." Skirmish is gated on owning cards.
//     4. Deliver a collection (SynchronizationCommand_* / the .cln format).
//     5. Swallow AccountCommand_RecordSinglePlayerGame.
// Everything else -- lobbies, trading, tournaments, guilds, chat, the store --
// is PvP/commerce and can be ignored or stubbed.
//
// ----------------------------------------------------------------------------
// 55.7  TRAPS
// ----------------------------------------------------------------------------
// * The DLL file LOCKS while eqgame has it mapped. Close the LoN window before
//   patching; if that does not release it, exit the client.
// * A late CE write to the host globals is silently ineffective (55.2 #3).
// * `--host=` is the hosting MMO (eq/eq2), NOT a network address.
// * The DLL is a SHARED codebase with the Free Realms TCG -- it is full of
//   "Free Realms Trading Card Game" strings and FreeRealms fonts. Do not read
//   those as evidence about LoN's own behaviour.
// * Ghidra did NOT name the command classes' vftables, so a
//   `<Class>::vftable` symbol lookup returns nothing. Locate a class by
//   decompiling the neighbourhood of one known method instead -- C++ class
//   methods are laid out contiguously.
// * PDB path in the binary (no PDB shipped):
//   g:\builds\lon\integrated\live\LON\dev\client\release\LegendsOfNorrath.pdb
//
// ----------------------------------------------------------------------------
// 55.8  THE HANDSHAKE -- IMPLEMENTED AND TESTED AGAINST THE REAL CLIENT
// ----------------------------------------------------------------------------
// *** 2026-08-13: a locally written service got the client past login and into
// "Entering main lobby...", and drove its MOTD window. Everything below is
// CONFIRMED BY A LIVE CLIENT unless marked otherwise. ***
//
// THE SEQUENCE:
//     client -> GatewayCommand_GetConnectionServer (0x19E v1)  [no preamble]
//     server -> same class, Success=1, Host/Port filled in
//     client   disconnects and reconnects to that Host:Port
//     client -> LoginCommand_SendPassword (0x19A v7)           [12-byte preamble]
//     server -> LoginCommand_Results (0x51 v6), LoginSucceeded=1
//     client   shows "Entering main lobby..."
//     client -> NetworkCommand_Ping (0x70 v1) every 10s, forever
//
// THE 12-BYTE PREAMBLE (asymmetric -- this matters):
// The client's SEND path (RVA 0x173DC0) writes htonl(payloadLen + 0xC), then
// three htonl words supplied by the command's vtable slot +0x54, then the
// payload. Observed: 0x3B9ACA00 (= 1,000,000,000), 0, 0.
// *** Its RECEIVE path (RVA 0x1740E0) does NOT expect them: it reads the
// ntohl length and goes straight to the class id at +4. So STRIP the preamble
// on input and NEVER send one. Proven -- our preamble-free replies are
// accepted and acted upon. ***
// The gateway socket sends no preamble; the connection-server socket does.
//
// HEADER COUNT VARIES WITH INHERITANCE DEPTH -- do not hardcode 3:
//     GatewayCommand_GetConnectionServer  3   (-> GatewayCommand -> NetworkCommand)
//     LoginCommand_Results                3   (-> LoginCommand -> NetworkCommand)
//     LoginCommand_DisplayMOTD            3
//     AccountCommand_IntroduceAccount     2   *** AccountCommand::write does NOT
//                                             chain to NetworkCommand::write ***
//     NetworkCommand_Ping                 1
// Each header is [varint opcode][varint version]; the version is a per-class
// constant from the class's getVersion() virtual, NOT a protocol-wide value.
//
// VERSIONS SEEN: Gateway_GetConnectionServer 1, Login_SendPassword 7,
//     Login_Results 6, Login_DisplayMOTD 1, Acct_IntroduceAccount 1, Ping 1.
//
// LoginCommand_SendPassword (0x19A v7) -- client -> server, field order:
//     Username, Password (PLAINTEXT -- no TLS anywhere in this protocol),
//     <7 ints>, Platform ("winVista"), MatchChallengeKey, LoginClient ("EQ"),
//     Locale ("en_US"), CharacterName, CharacterID, Ticket
//
// LoginCommand_Results (0x51 v6) -- server -> client. Serialize RVA 0x15B8F0,
// deserialize RVA 0x15BC20. The DESERIALIZER IS VERSION-GATED; at v6 it reads:
//     LoginSucceeded (int/bool)
//     Explanation    (int)      *** 0x4C diverts to username registration ***
//     AccountID      (int)
//     UseExplanation (int/bool) [only when version > 1]
//     ServerID       (int)      [only when version > 3]
//     SessionID      (string)   [only when version > 4]
//     [only when version >= 6:] uncompressedLen, compressedLen, ZLIB BLOB
//         -- the account-restrictions vector, zlib (uncompress at RVA 0x394070,
//            asserts "zres == 0"). An EMPTY vector is one byte 0x00 compressed;
//            that is accepted.
// A working reply, byte for byte:
//     21 05 06  21 05 06  21 05 06     headers
//     01 00 01 00 01                   Succeeded, Explanation, AcctID,
//                                      UseExplanation, ServerID
//     0F "NMS-LON-SESSION" 00          SessionID
//     01 09 78 9C 63 00 00 00 01 00 01 uncompLen=1, compLen=9, zlib(00)
//
// NetworkCommand_Ping (0x70 v1): 4 bytes, `20 07 01 00`. ECHO IT BACK verbatim.
// Unanswered pings eventually drop the connection (NetworkCommand_ConnectionTimedout
// 0x86 exists); answered, the session stays up indefinitely.
//
// AccountCommand_IntroduceAccount (0x72 v1), serialize RVA 0xEDF0:
//     header x2; AccountID (AccountCommand base, RVA 0x9570); AccountName
//     (string); propertiesFlag (1 = none, 0 = object follows); AccountGender
// Accepted by the client without complaint, but does NOT advance the UI.
//
// LoginCommand_DisplayMOTD (0x135 v1), serialize RVA 0x1574E0:
//     header x3; string; int; int; string
// *** VERIFIED: pushing this OPENS THE MOTD WINDOW in the client. *** The body
// came up empty with the text in the FIRST string, so the displayed text is
// probably the SECOND string [UNVERIFIED].
//
// ----------------------------------------------------------------------------
// 55.9  WHERE IT IS STUCK, AND WHAT TO TRY NEXT
// ----------------------------------------------------------------------------
// The session login dialog stays up on "Entering main lobby...". Its own
// result handler (RVA 0x4A6E50) only SETS THAT LABEL AND RETURNS -- the dialog
// is closed later by something else, and the client asks us for NOTHING after
// login (pings only). So the real server PUSHED something unprompted.
//
// *** TRAP: there are TWO login dialogs. `WALoginDialog` (object name
// "loginDialog", closed by RVA 0x45CFF0) is NOT the one on screen after a
// session login -- that is `WASessionLoginDialog` (object name
// "sessionLoginDialog", created at RVA 0x45CF00). Chasing the wrong one wasted
// a pass. ***
//
// Ruled out as the trigger: AccountCommand_IntroduceAccount, DisplayMOTD
// (opens its window but does not dismiss the dialog).
// Still to try: SynchronizationCommand_Start/StartInstances/SendInstances/
// Complete (0x53/0x54/0x55/0x56 -- the collection sync), the LobbyCommand
// family, NetworkCommand_News. The main window builds "motdText" and
// "newsText" widgets (RVA 0x462FB0), so News is pushed at some point too.
//
// METHOD THAT WORKS, for the next command: find the class's debugDump virtual
// -- it sprintf()s EVERY FIELD BY NAME ("Account ID: %d\n") -- then read its
// serialize, which writes the same fields in declaration order. Two decompiles
// per command and you have the wire format.
//
// ----------------------------------------------------------------------------
// 55.10  HOW COMMANDS DRIVE THE UI -- the custom-event bus  *** the key ***
// ----------------------------------------------------------------------------
// A command's execute() does not touch widgets directly. It posts a Qt CUSTOM
// EVENT (QEvent::type == 0x3E9) carrying an integer ID plus parameters; widgets
// install event filters and switch on that ID. Parameters are read back with
// the accessor at RVA 0xACAA0 (arg index -> {value at +4, type at +8}, where
// type 1 = bool, 2 = int); the ID itself comes from RVA 0xAC800.
//
//     poster:        RVA 0xAC810   (takes the event ID)
//     param push:    RVA 0xADC50 (string), 0xADD70 (int)
//     ID accessor:   RVA 0xAC800
//     param access:  RVA 0xACAA0
//
// So "which message makes the client do X" is a lookup, not a guess.
// `LoN_events.txt` next to this header maps 319 posts from 251 call sites.
//
// KNOWN FILTERS:
//     WASessionLoginDialog (RVA 0x4A7330):
//         0x0E -> login result handler (RVA 0x4A6E50) -- sets the status label
//         0x95 -> login failure (RVA 0x4A7210) -- closes the dialog
//         0x96 -> RVA 0x4A71A0
//         *** NONE of these close the dialog on SUCCESS. ***
//     Main window (RVA 0x463650):
//         0xAE -> RVA 0x462C20
//         0xAF -> RVA 0x462960
//     Main window also checks IDs 0x11176 / 0x11177 / 0x11184 (RVA 0x465870),
//     so there is more than one ID namespace.
//
// Posts confirmed: GatewayCommand_GetConnectionServer::execute (RVA 0x1176E0)
// posts 0x95 on failure and 0x96 on success; 0x0E is posted from RVA 0x15B4E0
// (LoginCommand_Results) and RVA 0x1735B0.
//
// *** OPEN QUESTION / START HERE NEXT: nothing in LoN_events.txt posts 0xAE or
// 0xAF, so a SECOND event poster exists. Find it (look at what RVA 0x462C20 and
// 0x462960 do, and xref them), and 0xAE/0xAF will name the command that opens
// the lobby and dismisses the session login dialog. ***
//
// ----------------------------------------------------------------------------
// 55.11  THE COLLECTION SYNC -- the client TALKS BACK.  *** START HERE ***
// ----------------------------------------------------------------------------
// Pushing an empty sync after login made the client send its FIRST ever
// unprompted command (everything before this was login + pings):
//
//     server -> SynchronizationCommand_Start     (0x53 v3)
//     client -> SynchronizationCommand_Update    (0x57 v1)   *** IT REPLIED ***
//
// So the sync is a CONVERSATION and it is the live thread. The Update it sent
// carried NO fields -- 21 bytes = 12-byte preamble + three (0x57,1) headers.
//
// Serialize orders (all three headers -- SynchronizationCommand's base at RVA
// 0x1AE810 chains to NetworkCommand::write and adds no fields of its own):
//     Start          0x53 v3  RVA 0x1B1810: AccountID, CollectionType,
//                                           ArchetypeCount, CollectionModID
//     StartInstances 0x54 v?  RVA 0x1B25C0: the SAME four fields
//     Complete       0x56 v2  RVA 0x1AEF60: DeliveryID
//     Update         0x57 v1
// Other members of the family and their field names (from their debugDumps):
//     SendInstances, SetModID (ModID), SetDesired, StartDelivery (DeliveryID),
//     ProcessFulfillment, ProcessBulkFulfillment, RemoveInstances,
//     AddNonTradable / RemoveNonTradable, LockTournament / UnlockTournament.
//     Fields seen across the family: Account ID, Collection Type, Archetype
//     Count, Collection Mod ID, TransactionID, TransactionSource(ID),
//     DeliveryID, Num Instances, Flags, Silent.
//
// LIKELY SEQUENCE (to try next):
//     Start -> (client: Update) -> StartInstances -> SendInstances -> Complete
// We sent Start and Complete back to back, skipping the middle, and the client
// stalled -- but it DID answer, which is why this is the thread to pull.
//
// FULL FIELD MAPS AND VERSIONS (all three header levels):
//     Start          0x53 v3  AccountID, CollectionType, ArchetypeCount, ModID
//     StartInstances 0x54 v2  the SAME four fields          (RVA 0x1B25C0)
//     SendInstances  0x55 v2  BaseArchetypeID, ArchetypeCount, Instances[],
//                             NonTradableInstances[], DeliveryID (RVA 0x1B04B0)
//     Complete       0x56 v2  DeliveryID                    (RVA 0x1AEF60)
//     Update         0x57 v1  no fields (client -> server; 3 headers only)
//
// THE INSTANCE VECTOR (RVA 0x758B0): varint(count) then each element, where
// elements are 0x10 bytes apart in memory and each is written through its own
// virtual at vtable slot +0x28 (via the thunk at RVA 0x176EE0). So an EMPTY
// vector is a single 0x00 byte -- which is all an empty collection needs.
// The element type is almost certainly `CollectionItem` (opcode 0x34, factory
// RVA 0x77610); its per-field layout is [UNVERIFIED] and is only needed once
// we want to deliver REAL cards. `data/collections/default.cln` on disk is a
// worked example of the same data (8-byte records there; the wire form may
// differ -- do not assume they match).
//
// A version constant is the 4th virtual after the class-name virtual, spaced
// 0x10: name, getDAT, setDAT, version. Verified for 0x51->6, 0x53->3, 0x54->2,
// 0x55->2, 0x56->2, 0x57->1, 0x135->1.
//
// A version constant is the 4th virtual after the class-name virtual, spaced
// 0x10 apart: name, getDAT, setDAT, version. That is how 0x53->v3, 0x56->v2,
// 0x51->v6 and 0x135->v1 were read off.
//
// ============================================================================
// *** RESULT 2026-08-13: THE TUTORIAL PLAYS. Single-player Legends of Norrath
// runs against a locally written service. PARKED here at that point --
// the remaining work is content, not protocol. ***
//
// What works end to end: gateway redirect -> login -> account introduce ->
// MOTD -> full collection sync -> LOBBY, with every menu ENABLED (an account
// with zero restrictions satisfies the entitlement gates), and the Tutorial
// starts and is playable.
//
// Two known rough edges:
//   1. The session login dialog does not auto-dismiss -- it sits on top of a
//      fully live lobby. Click its X and everything behind it works. Some
//      event the real server posted is still missing; it is COSMETIC, not a
//      gate. (Ruled out: it is not the sync, not the account, not the MOTD --
//      all of those are received and executed cleanly per wa_network.log.)
//   2. Everything except the Tutorial crashes. Near-certainly because we serve
//      an EMPTY collection -- no cards, no decks. The fix is to deliver real
//      CollectionItems (class 0x34) in SynchronizationCommand_SendInstances;
//      `data/collections/default.cln` and `data/decks/*.eqd` on disk are the
//      shipped starter data to serve. That is the next job for anyone
//      resuming, and it is content plumbing, not new reverse engineering.
// ============================================================================
//
// ----------------------------------------------------------------------------
// 55.13  THE CLIENT KEEPS ITS OWN ERROR LOG -- READ IT, DO NOT GUESS
// ----------------------------------------------------------------------------
// *** `<client>\wa_error.log` (next to eqgame.exe) is written by the LoN DLL
// and NAMES EVERY FAILED ASSERT WITH FILE AND LINE. It is by far the fastest
// feedback loop in this project -- check it after every test. ***
//     "NETWORKCOMMAND: ERROR: Reached a wa_assert : mPropertySet :
//      ..\..\..\Common\common\networking\account_command\
//      AccountCommandIntroduceAccount.cpp: 62"
// There is a second, much more verbose log, `wa_network.log`, which records
// "SENT %d bytes" and "RECEIVED AND EXECUTING:\n%s" for every message -- it is
// gated by RVA 0x68650 (sink at RVA 0x173920). Flipping that gate would give a
// full protocol trace; not yet attempted.
//
// EMBEDDED SERIALISABLE OBJECTS  *** cost two client crashes to get right ***
// A nullable object member is written as:
//     varint(flag)     1 = absent, 0 = present
//     varint(classId)  ONLY when present -- read by RVA 0x976E0
//     <object body>    its own header (version) then its fields
// Reader: RVA 0xDD90. The classId goes through the SAME registry as network
// commands (RVA 0x94970), so `LoN_opcodes.txt` is really a table of EVERY
// serialisable class -- PropertySet is 0x17, CollectionItem 0x34, and the
// Expression/Node classes occupy 0x19-0x28.
// Omitting the classId makes the client read the next byte as one and die with
// "Couldn't get class 0" in wa_error.log -- and it CRASHES the whole EQ client.
//
// AccountCommand_IntroduceAccount (0x72 v1) -- CORRECTED AND VERIFIED:
//     header x2; AccountID; AccountName;
//     varint(0); varint(0x17); varint(1); varint(0);   <- the PropertySet,
//                                                         MANDATORY, empty map
//     AccountGender
// *** The property set is NOT optional despite the writer supporting NULL: the
// client asserts on it and silently discards the whole command. Verified fixed
// -- wa_error.log records nothing at all for a login after this change. ***
//
// ----------------------------------------------------------------------------
// 55.12  DEAD ENDS -- DO NOT REPEAT THESE
// ----------------------------------------------------------------------------
// * Main-window events 0xAE / 0xAF are the LEADERBOARDS update (RVA 0x462C20),
//   NOT the lobby transition. Chased and discarded.
// * `AccountCommand_IntroduceAccount` is accepted but changes nothing on screen.
// * `LoginCommand_DisplayMOTD` opens the MOTD window but does NOT dismiss the
//   login dialog.
// * The session login dialog's status strings END at "Entering main lobby..."
//   (PUIDs 0-6: Connecting to gateway / Waiting for server / Registering user
//   name / Entering main lobby / Login Failure). There is no later status, so
//   the dialog is torn down by the app moving on, not by a status update.
// * Static xrefs keep dead-ending on Qt: the main window's setup (RVA 0x462FB0)
//   and showLoginDialog (RVA 0x45CFF0) are reached only through qt_metacall
//   slot tables, so "find the caller" does not work on them. Use the
//   custom-event bus (55.10) instead.
//
// Prototype service: a Python service + wire codec (internal tool, not included)
// (2026-08-13).
//
// Ghidra project `lon_proj`,
// program `LegendsOfNorrath.dll` (imported 2026-08-13, full analysis).
// Scripts (internal tool, not included): ghidra_scripts_lon{,2,3,4,5}\ -- probe by string, xref an address,
// dump a vtable, decompile a range, extract the opcode table.
//
// ----------------------------------------------------------------------------
// 55.14  THE WHOLE PROTOCOL, GENERATED (2026-10-06)  *** read this first now ***
// ----------------------------------------------------------------------------
// Four generated tables sit next to this header:
//   LoN_messages.txt   every serialisable class (498): class id, wire version,
//                      vtable, write/read/dump/execute functions and the fields
//                      IN WIRE ORDER with object offset, type, the client's own
//                      label, enum values and "since vN" read gates.
//   LoN_eventflow.txt  every UI event id: the command that posts it and the
//                      widget that handles it (switch tables decoded).
//   LoN_errors.txt     the client's error-code -> message table (RVA 0x46C940).
//   LoN_opcodes.txt    the original registry walk. ITS CLASS COLUMN IS WRONG FOR
//                      ~220 ROWS (it named a base class); LoN_messages.txt
//                      re-verifies each id against the vtable's own classId and
//                      name slots -- trust that one.
//
// How they were made (internal tools, not included):
//   ghidra_scripts_lon7\LonCatalog.java  registry -> factory -> ctor -> vtable,
//                                        decompile every slot (headless, lon_proj)
//   ghidra_scripts_lon8\LonVtables.java  same for an explicit "id vtable" list
//   ghidra_scripts_lon9\LonDump.java     decompile a list of functions
//   a catalog parser script                  -> LoN_messages.txt (+ a JSON form)
//   an event-flow script (capstone)          -> LoN_eventflow.txt
//
// Facts the generator relies on (all checked by hand on several classes):
//   * Every serializer writes trace labels: FUN_10097a80(stream, "Label: ")
//     right before each field -- the labels in the table are the client's.
//   * Command vtable: 2 writeHeader, 3 writeFooter, 4 readHeader, 5 readFooter,
//     6/7 classId, 8 version, 9 deserialize, 10 serialize, 11 className,
//     15 execute, 18 clone, 19 debugDump, 21 preamble words (service id).
//   * Element writers RVA 0x176D30-0x176FA0: int, string, object*, inline
//     object, and std::map<K,V> writers (int/string keys; int, string, bool,
//     object, object* and (int,int) values).
//   * ValueData (0x16 v4) is a VARIANT on TypeID: 1 bool, 2 int, 3 string,
//     5/9/10 nullable object, 6 vector<int>, 7 vector<string>, 8 vector<object*>,
//     13 map<int,int>, 14 map<int,object*>  (serializer RVA 0x1D5DD0).
//
// A generic codec (internal tool, not included) decodes/encodes ANY message from the JSON form.
// Validated: every distinct client message in the service log (22) decodes
// completely, and our hand-built StartDelivery / RemoveInstances / Complete /
// RedeemItemGrant round-trip byte for byte.
//
// ZLIB BLOCKS (found later the same night): some serializers write a run of fields
// into a temporary stream (RVA 0x97560), compress it (RVA 0x1E0CA0, assert 'zres == 0')
// and send varint rawLength, varint compressedLength, then the zlib bytes (RVA 0xE170).
// LoN_messages.txt shows these as 'zlib block { ... }' (7 classes: LoginCommand_Results v6's
// tail, LobbyCommand_BulkJoin, GameCommand_SendSerializedGame/FatalError ...). The generic codec
// reads and writes them.
