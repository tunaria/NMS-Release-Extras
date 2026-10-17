/*
 * MacroQuest: The extension platform for EverQuest
 * Copyright (C) 2002-present MacroQuest Authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#pragma once

// ============================================================================
// Housing.h -- RoF2 Player Housing / Real Estate System
// ============================================================================
// Same offset conventions as eqgame.h: static addresses for image base
// 0x400000; the running client is ASLR'd, REBASE every address (observed
// bases have run static-0x20000, but never assume -- read the module base).
//
// What is NOT here: housing rows inside eqgame.h's complete inventory tables
// (pinst list, ctor list, vtable/RTTI scans, wndclass tables) stay in
// eqgame.h so those enumerations keep their holes-free guarantee. Companion:
// Market.h (Section 44, the marketplace).
//
// APPEND-STYLE, like its parent: add new subsections at the bottom with the
// next 43.x number; correct wrong entries in place; never renumber.
// ============================================================================
// ============================================================================
// Section 43: Player Housing / Real Estate System (investigated 2026-07-27)
// ============================================================================
//
// PURPOSE: map the RoF2 client's player housing / real-estate subsystem. EQEmu has never
// implemented any of this -- the stock RoF2 patch conf contains ZERO real-estate
// opcodes -- but the RoF2 CLIENT's housing support is fully intact and reachable.
//
// Static-analysis only (Ghidra headless). Nothing below has
// been exercised against a live server — no server on earth still speaks RoF2 housing,
// so these CANNOT be verified by packet capture. Treat every [inferred] as a hypothesis.
//
// ---------------------------------------------------------------------------
// *** PROVEN IN GAME 2026-07-27 *** — see 43.9. The recovered opcodes are REAL: an
// EQEmu server sent OP_RealEstateResult (0x1228) with code 10 and the RoF2 client
// printed "You cannot add this item because the real estate is full." No client-side
// mod, no housing zone. The native real-estate subsystem is driveable from an emulator server.
//
// 43.1  Dispatch path (how a housing packet reaches the client)
// ---------------------------------------------------------------------------
// CEverQuest::HandleWorldMessage (0x004C3250) is a
// jump-table switch on the opcode. THREE of its cases funnel into one real-estate
// handler. The case block lives at 0x004CC1D7:
//
//      004cc1d7:  PUSH [EBP+0x14]        ; size
//      004cc1da:  PUSH [EBP+0x10]        ; buf
//      004cc1df:  PUSH [EBP+0x0c]        ; opcode
//      004cc1e3:  CALL 0x005B3540        ; RealEstateManagerClient::Instance
//      004cc1ea:  CALL 0x005B2500        ; -> the handler below
//      004cc1ef:  JMP  0x004cd5e0        ; common switch exit
//
// Jump-table entries pointing at that block (i.e. 3 distinct opcodes route here):
#define RealEstate__DispatchCase_x                                 0x004CC1D7  // case body in HandleWorldMessage
#define RealEstate__JumpTableEntry1_x                              0x004D433C  // [inferred] .text switch table
#define RealEstate__JumpTableEntry2_x                              0x004D477C  // [inferred]
#define RealEstate__JumpTableEntry3_x                              0x004D4C7C  // [inferred]
//
// The handler itself is a second switch, keyed on the opcode passed through:
//      void __thiscall RealEstateManagerClient::HandleRealEstateMessage(
//              this, uint opcode, char* buf, uint size)
#define RealEstateManagerClient__HandleRealEstateMessage_x          0x005B2500
//
// Singleton storage (RealEstateManagerClient__Instance_x 0x5B3540 is defined in the
// RealEstateManagerClient block earlier in this file — do not redefine it):
#define pinstRealEstateManagerClient_x                              0x00E64570  // global ptr written by Instance()
#define RealEstateManagerClient__SIZE                               0xB4        // operator new size in Instance()
// 189 call sites reference Instance() across the client = the full client-side surface.
//
// ---------------------------------------------------------------------------
// 43.2  THE RoF2 REAL-ESTATE OPCODES  (recovered 2026-07-27)
// ---------------------------------------------------------------------------
// Extracted from the `opcode ==` comparisons inside 0x005B2500. These are the wire
// values the RoF2 client will ACT ON. None appear in patch_RoF2.conf; none collide with
// any opcode already defined in Section 5 of this file.
//
// Names are [inferred] from each branch's behaviour and are NOT authoritative — the
// numeric value is the fact, the name is a guess. Rename in place as branches get
// labelled; do not add duplicate defines under better names.
//
// -- record feed: allocate a 0x48 (72-byte) record, deserialize, refresh ManageWnd --
#define OP_RealEstate_RecordAdd                                     0x3968      // [inferred] new 0x48 record -> 0x5B7A90 deser -> 0x5B59A0 add -> refresh via 0x7465F0 (pinstRealEstateManageWnd)
#define OP_RealEstate_RecordUpdate                                  0x485D      // [inferred] same 0x48 record + 0x5B7DE0 update -> refresh via 0x7465C0
//
// -- 0x58 (88-byte) record family; all three call 0x007FE100 --
#define OP_RealEstate_PlotPurchase                                  0x63FD      // [inferred] -> 0x743FD0 / 0x74DCB0 (RealEstatePurchaseWnd region 0x74CA70)
#define OP_RealEstate_PlotUpdate                                    0x4F09      // [inferred] -> 0x744070(rec,1)  (RealEstateManageWnd region)
#define OP_RealEstate_PlotRemove                                    0x6FD0      // [inferred] -> 0x7F4FA0
//
// -- deserialize-into-manager branches --
#define OP_RealEstate_ManagerFeedA                                  0x1CEF      // [inferred] -> 0x5B0D00
#define OP_RealEstate_ManagerFeedB                                  0x087F      // [inferred] -> 0x5B0E30
#define OP_RealEstate_ManagerFeedC                                  0x0351      // [inferred] near 0x5AFD50 / 0x5AFE80(0x1BAF,..)
//
// -- single-dword payload branches (likely id-only notifications) --
// 0x1228 was "OP_RealEstate_NotifyA" here — CONFIRMED in 43.7 as the result-code
// channel and defined there as OP_RealEstate_Result. Do not re-add it under this name.
#define OP_RealEstate_NotifyB                                       0x66BF      // [inferred] -> 0x5AD930(*(dword*)buf) / 0x5ADAD0
//
// -- window / world-object feeds --
#define OP_RealEstate_PlacedNpcItem                                 0x21EA      // [inferred] -> 0x5996E0 spawn lookup (placed NPC/familiar items, NpcItem=1 in layouts)
#define OP_RealEstate_WindowFeedA                                   0x21EC      // [inferred] -> 0x7375F0(buf)
#define OP_RealEstate_WindowFeedB                                   0x2AA6      // [inferred] -> 0x741670
#define OP_RealEstate_WindowFeedC                                   0x2AA7      // [inferred] -> 0x5B4060 -> 0x7C9AA0
#define OP_RealEstate_WindowFeedD                                   0x3D94      // [inferred] -> 0x5B40A0 -> 0x7C9B10
// CONFIRMED (see 43.8): 0x2698 is the DEFINITIONS/CATALOG feed — it carries the data
// the RealEstateDefs.txt / RealEstateCosts.txt files would otherwise supply.
#define OP_RealEstate_Definitions                                   0x2698      // -> 0x7F5B60 (defs) + 0x7FC6B0 (costs) + 0x5AAAD0
#define OP_RealEstate_TextMessage                                   0x58C7      // [inferred] string parse 0x5559A0 -> 0x6AE610
//
// UNLABELLED BRANCH (no opcode constant — reached via the 0x5AFE00(opcode,..) fallthrough):
// allocates 0xBC, references the literal "this real estate" (0x805C20) and raises a
// confirm dialog 0x71EB20(wnd, 0x89, text, 0xC, 300000, ..) — a 5-minute confirmation
// prompt. [inferred] eviction / abandon-plot confirmation.
//
// ---------------------------------------------------------------------------
// 43.3  EQPlacedItemManager list layout
// ---------------------------------------------------------------------------
// From EQPlacedItemManager__GetItemByRealEstateAndRealEstateItemIds_x (0x506120,
// defined earlier): a singly-linked list walked with an (id,id) key. Both -1 = miss.
#define EQPlacedItem__NEXT_OFS                                      0x08        // next node
#define EQPlacedItem__REALESTATEID_OFS                              0x24        // realEstateId
#define EQPlacedItem__REALESTATEITEMID_OFS                          0x28        // realEstateItemId
//
// ---------------------------------------------------------------------------
// 43.4  Placement is CLIENT-side (why a hybrid is viable)
// ---------------------------------------------------------------------------
// The client owns the whole placement interaction — ghost preview, collision, and all
// four modes (collision / cursor / wall / free), ALT=resize, CTRL+wheel=rotate,
// CTRL+move=pitch/roll, SHIFT=fine-grain. Every mode + reset keybind string is already
// catalogued in the "RealEstate System - Key Bind Command Name Strings" block earlier in
// this file (s_CMD_REAL_ESTATE_ITEM_PLACEMENT_*). The server only ever receives the
// committed transform.
//
// ---------------------------------------------------------------------------
// 43.5  Placement data model (captured, not inferred)
// ---------------------------------------------------------------------------
// The modern 64-bit LIVE client dumps a plot's contents to
// `userdata\RE_Layout_<id>.xml` (Real Estate Manage -> Layouts -> name -> Save All).
// NOTE: the RoF2-era name in s_field_RealEstateSaveFile_x ("%s-RealEstate.txt") is
// OBSOLETE — that is why no .txt ever appears on a current client.
// A 113-item capture from a real plot + house gave the exact per-item model:
//
//    Id, Name, TransformationMold, ImageNum(icon), Tag(actor/model e.g. IT23335;
//    IT63 = NPC-type), NpcItem(0/1), Heading, Pitch, Roll, Scale, X, Y, Z, Type
//
//    * Heading/Pitch/Roll use EQ's native 0-512 angle scale (128=90deg, 256=180,
//      384=270) as floats — the SAME convention EQEmu stores.
//    * Scale observed 0.1 .. 2.5.
//    * Type 4 = in the yard (the house itself and the storage crate are Type 4),
//      Type 3 = inside the house interior. Yard and interior are SEPARATE coordinate
//      spaces.
//    * Stock EQEmu `object` rows already carry heading + tilt_x/tilt_y + size
//      (zone/object.cpp), so the model maps onto EQEmu's object model nearly 1:1.
//
// Client assets all ship with the RoF2 client: neighborhood.eqg, guildhall/guildhallsml/
// guildhallLrg/guildhall3, many phinterior* floor plans, furniture*.eqg, and all six
// EQUI_RealEstate*Wnd.xml. The client ALSO references RealEstateDefs.txt /
// RealEstateCosts.txt / RealEstateItemGroups.txt which exist in NO client copy (not
// RoF2, not current live) => that data is server-fed at runtime; costs are server-defined.
//
// ---------------------------------------------------------------------------
// 43.6  Opcode labelling — round 2 (payload deserializers)
// ---------------------------------------------------------------------------
// All real-estate payloads are read through a sequential stream-reader struct passed as
// the 2nd arg: { +0x00 = buf base, +0x04 = length, +0x08 = read cursor }. Every field read
// is bounds-checked (`cursor + 4 <= length`) and advances the cursor by 4 — so the packets
// are plain little-endian dword sequences, NOT bit-packed. Good news for reimplementation.
#define RealEstateStreamReader__BUF_OFS                             0x00
#define RealEstateStreamReader__LEN_OFS                             0x04
#define RealEstateStreamReader__CURSOR_OFS                          0x08
//
// -- the 0x48 record deserializer (shared by OP_RealEstate_RecordAdd 0x3968 and
//    OP_RealEstate_RecordUpdate 0x485D — same object type, add vs update) --
#define RealEstateManagerClient__ReadRecord48_x                     0x005B7A90
//    Layout: 7 consecutive (dword, dword) PAIRS filling record+0x00 .. record+0x37,
//    then a dword COUNT, then COUNT sub-entries each parsed by 0x005B4A90 into the
//    collection at record+0x38. Total object 0x48.
#define RealEstateRecord48__PAIR_COUNT                              7
#define RealEstateRecord48__LIST_OFS                                0x38
#define RealEstateManagerClient__ReadRecord48SubEntry_x             0x005B4A90
//    [inferred] the 7 pairs are (id, value) tuples — plausibly the per-area caps the
//    RealEstateItemsWnd displays (yard/house/crate item caps, vault cap, pet cap); the
//    +0x38 list is then the actual contents. UNCONFIRMED.
//
// -- confirmation that 0x3968 and 0x485D are the same object --
//    Both refresh through the SAME path: 0x7465F0 / 0x7465C0 -> Instance() -> 0x00744200(0).
#define RealEstateManageWnd__RefreshFromRecord_x                    0x00744200  // [inferred]
//
// -- the 0x58 record family (OP_RealEstate_PlotPurchase 0x63FD /
//    OP_RealEstate_PlotUpdate 0x4F09 / OP_RealEstate_PlotRemove 0x6FD0) --
//    All three allocate 0x58 and route through 0x007FE100.
#define RealEstate__Read58Record_x                                  0x007FE100  // [inferred]
//    0x63FD's handler 0x00743FD0 then does: if the id is NOT already present
//    (0x0086A880 returns -1), insert it into the collection at MANAGER+0x38
//    (0x0086AE50(mgr+0x38, id)) and refresh THREE windows
//    (0x00741470, 0x00741930(0), 0x00742CD0(1)). So:
#define RealEstateManagerClient__IDLIST_OFS                         0x38        // [inferred] known/owned real-estate id set
//
// -- list-populate for the purchase/plot UI --
#define RealEstate__PopulatePlotList_x                              0x0074DCB0  // [inferred] formats " (%d)" suffixes, per-row fields at rec+0x20/+0x28/+0x2C/+0x3C
//
// STILL UNLABELLED after round 2: 0x0351, 0x087F, 0x1228, 0x1CEF, 0x21EA, 0x21EC,
// 0x2698, 0x2AA6, 0x2AA7, 0x3D94, 0x58C7, 0x66BF. Next lever: decompile each branch's
// window-side callee and match it to the owning RealEstate*Wnd via the ctor/vftable
// addresses already catalogued in this file.
// ============================================================================


// ---------------------------------------------------------------------------
// 43.7  OP_RealEstate_NotifyA (0x1228) IS THE RESULT-CODE CHANNEL  <-- big one
// ---------------------------------------------------------------------------
// Handler 0x005AE1F0 takes ONE dword (the payload of opcode 0x1228) and maps it to an
// eqstr id, then prints it. So 0x1228 is the server->client OPERATION RESULT channel:
// every failed real-estate action is reported by sending this opcode with a code.
//   code -1 = no-op (return), code 0 = SUCCESS (refreshes RealEstateItemsWnd via
//   pinstRealEstateItemsWnd 0xD1FCB8 / 0x00733CB0).
// Renaming the define accordingly (the 0x1228 value is what matters):
#define OP_RealEstate_Result                                        0x1228      // was OP_RealEstate_NotifyA
#define RealEstate__ShowResultCode_x                                0x005AE1F0
//
// The 52 codes below were resolved through the client's eqstr_us.txt. This is effectively
// a FREE SPECIFICATION of the server-side rule set: every rule the live server enforced
// has a code here, so a reimplementation can be checked against this list.
//   10     0x1463   You cannot add this item because the real estate is full.
//   0xb    0x1479   There are no items to transfer.
//   0xc    0x1498   You cannot add this item.  The vault is full.
//   0xf    0x147a   You cannot add another house to this yard.
//   0x10   0x147b   You cannot place another moving crate on this property.
//   0x11   0x1464   You can't put that item in this real estate.
//   0x13   0x146d   You can only do this with empty containers.
//   9      0x146a   The item was not found at the specified location.  It is likely that you are out of sync with the zone.  Try camping or zoning to fix the issue.
//   0x14   0x146f   You are too far away from the item.  Move closer to it and try again.
//   0x15   0x149b   This item must be placed in the zone for you to interact with it.
//   0x16   0x1478   You must have a moving crate placed in your yard or house for this operation.
//   0x17   0x1483   This can only be done by the owner of the property.
//   0x18   0x1929   You cannot do this because the upkeep on this plot has expired.
//   0x19   0x192a   You cannot do this because the upkeep on the item or house has expired.
//   0x1c   0x1475   You cannot place this on top of a player.
//   0x1d   0x1499   You cannot place this item here because the surface is not flat enough.
//   0x1f   0x1924   You cannot place this here because you've reached the limit for that item.
//   0x20   0x1925   Non-empty houses and crates can only be placed in yards and houses.  They may not be put in the shed or the closet.  While non-empty houses are allowe...
//   0x21   0x1926   You cannot pick this up because you may only have one non-empty house and crate in your inventory at a time.
//   0x23   0x1482   You cannot place that item in this zone.
//   0x24   0x146e   You can only do this with placeable items.  See the item's description.
//   0x26   0x1928   You cannot place this here because you have reached the pet limit for this yard.
//   0x27   0x14a2   Moving crates can only contain one non-empty house and this moving crate already has one.
//   0x2a   0x14a3   You cannot interact with this NPC item because its NPC was not found in the zone.
//   0x2b   0x14ab   You cannot add this item because it is No Storage.
//   0x2c   0x14ac   You cannot add this item because it contains an augment that is No Storage.
//   0x2d   0x14c0   Part of the stack was successfully moved.
//   0x2e   0x14c1   You cannot place an item from that stack because the real estate does not have room for the item that would be added from splitting the stack.
//   0x2f   0x14c7   This activity is disabled until you own the House of Thule expansion.
//   0x30   0x14d3   Your house swap failed because the house you are attempting to swap in does not have enough space to hold all of the placed house's items.
//   0x31   0x14d4   Your house swap failed because the house you are attempting to swap in does not have enough space to hold all of the placed house's archived items.
//   0x32   0x14d5   Your house swap failed because the house you are attempting to swap in does not have enough space to hold all of the placed house's NPC items.
//   0x35   0x14d6   Your house swap failed because you do not have a house placed.
//   0x37   0x14d7   Your house swap failed because the house you are attempting to swap in is already placed.
//   0x3b   0x14da   Your house swap failed because you cannot swap houses while you have a placed crate in your placed house.  Move the crate to storage,  the yard or you...
//   0x40   0x2db    You cannot remove this item because it is no drop and you are not its owner.
//   0x41   0x2dc    You cannot remove this item because it is a lore item that you already have.
//   0x42   0x2e7    You cannot do that because you are the owner of the item.
//   0x43   0x2e8    You cannot do that because the item contains a real estate.
//   0x44   0x2ed    You cannot add another guild hall to this yard.
//   0x46   0x2f8    You must be guild member who has modify trophy access to do that.
//   0x47   0x151c   You cannot do that because you do not have add item access.
//   0x48   0x151d   You cannot do that because you do not have remove item access.
//   0x49   0x151e   You cannot do that because you do not have move item access.
//   0x4a   0x151f   You cannot do that because you do not have interact item access.
//   0x4b   0x1520   You cannot do that because you do not have add house access.
//   0x4c   0x1521   You cannot do that because you do not have remove house access.
//   0x4d   0x1526   You cannot do that because you do not have add crate access.
//   0x4e   0x1527   You cannot do that because you do not have remove crate access.
//   0x4f   0x1525   You cannot do that because you do not have mail item access.
//   0x51   0x152a   You cannot store this house in the desired moving crate because the house contains a placed moving crate.  The moving crate inside the house must eith...
//   0x52   0x1535   This activity is disabled until you own the Veil of Alaris expansion.
//
// NOTE the PERMISSION MODEL that falls out of codes 0x47-0x4F: the system has NINE
// distinct per-real-estate grants — add item, remove item, move item, interact item,
// add house, remove house, add crate, remove crate, mail item. Guild halls add
// "modify trophy access" (0x46). Any reimplementation needs this granularity.
// Codes 0x2F / 0x52 are EXPANSION GATES (House of Thule / Veil of Alaris).
// ============================================================================


// ---------------------------------------------------------------------------
// 43.8  The RealEstate*.txt files are NOT needed — the catalog is SERVER-FED
// ---------------------------------------------------------------------------
// EARLIER ASSESSMENT WAS WRONG and is corrected here. The 2026-07-27 first pass listed
// the missing RealEstateDefs.txt / RealEstateCosts.txt / RealEstateItemGroups.txt as a
// blocking unknown ("another reversing project"). They are not blocking at all.
//
// Evidence:
//  1. NEITHER the RoF2 client NOR a fully-patched current LIVE install ships these
//     files, yet housing demonstrably works on live (plot + guild hall verified in
//     game 2026-07-27). So they cannot be required at runtime.
//  2. Each filename string is referenced by exactly ONE loader, and each of those
//     loaders also references "RealEstateErrors.txt" (a parse-error log):
#define RealEstate__LoadDefsFile_x                                  0x007F5380  // reads RealEstateDefs.txt
#define RealEstate__LoadCostsFile_x                                 0x007FC480  // reads RealEstateCosts.txt
#define RealEstate__LoadItemGroupsFile_x                            0x007FCE80  // reads RealEstateItemGroups.txt
//  3. Sitting immediately after each file loader is a PACKET loader for the same data,
//     and OP_RealEstate_Definitions (0x2698) calls them. They take the standard
//     {buf,len,cursor} stream reader — NOT a FILE* — read a dword count, then loop
//     parsing fixed records (~22 dword fields each):
#define RealEstate__ReadDefsFromPacket_x                            0x007F5B60
#define RealEstate__ReadCostsFromPacket_x                           0x007FC6B0
//
// CONCLUSION: the .txt files are a local/dev-time fallback path. The live path is the
// server pushing the whole catalog over 0x2698. For a reimplementation this is GOOD
// NEWS twice over:
//   * nothing has to be reverse-engineered out of a missing file format, and
//   * PLOT COSTS AND DEFINITIONS ARE SERVER-DEFINED -- the server sends them; the client just
//     renders what it is told. No pricing data needs to be recovered from anywhere.
// ============================================================================


// ---------------------------------------------------------------------------
// 43.9  LIVE PROOF — 2026-07-27  (the decompile is CORRECT)
// ---------------------------------------------------------------------------
// First end-to-end test of any recovered real-estate opcode. Setup:
//   * server: an EQEmu server with OP_RealEstateResult=0x1228 registered, queueing a
//     4-byte packet containing one dword.
//   * client: STOCK, unmodified.
//   * location: Crushbone. NOT a housing zone, no plot, no real-estate window open.
// Result: sending code 10 -> the client printed
//     "You cannot add this item because the real estate is full."
// exactly as predicted by the eqstr mapping in 43.7.
//
// WHAT THIS PROVES
//   1. The opcode value 0x1228 recovered from the 0x005B2500 switch is CORRECT, which
//      by extension validates the method that produced all 17 opcodes in 43.2.
//   2. The client's RealEstateManagerClient is fully reachable from an EQEmu server —
//      HandleWorldMessage dispatches real-estate opcodes exactly as decompiled.
//   3. No client-side patching is required to drive the native housing system.
//   4. The payload really is a plain little-endian dword (43.6), not bit-packed.
//   5. Confirms the handler has NO zone/plot/window gate on the text path (43.7) —
//      only code 0 (success) touches a window, which is why it is a poor probe.
//
// ============================================================================


// ---------------------------------------------------------------------------
// 43.10  THE CLIENT-SIDE ENTRY GATE — why /realestate refuses to open
// ---------------------------------------------------------------------------
// Observed in game 2026-07-27 (standing in Sunrise Hills / "neighborhood", zone id 712):
//     /realestate plot    -> "You must be in a neighborhood to use that."   (eqstr 8078)
//     /realestate manage  -> "You must be inside a plot or house to use that." (eqstr 727)
// The gate is NOT the zone id. It is one field on the manager singleton:
#define RealEstateManagerClient__CURTYPE_OFS                        0x90    // current real-estate TYPE enum
#define RealEstate__IsNeighborhoodType_x                            0x007F5F70
#define RealEstateWnd__OpenPlotSearchGate_x                         0x0074BB00
//
//   0x0074BB00:  mgr = RealEstateManagerClient::Instance();
//                if (IsNeighborhoodType(*(dword*)(mgr + 0x90)))  { open window; return 1; }
//                else print eqstr 0x1F8E ("must be in a neighborhood")
//
// 0x007F5F70 is a PURE SWITCH over a type enum with cases 1..0xB and returns true for
// EXACTLY ONE VALUE: **type 5**. Everything else returns false. So:
//     >>> the client believes you are "in a neighborhood" iff  mgr->[0x90] == 5  <<<
//
// NOT written by any of the packet handlers decompiled so far (0x5AAAD0, 0x5AFD50,
// 0x5AFE00, 0x5B0E30, 0x5B0F80, 0x5B4060, 0x5B40A0, 0x5B4680, 0x5AD930, 0x7375F0,
// 0x741670 all checked - none touch +0x90). Leading hypothesis: the client resolves the
// CURRENT zone against the DEFINITIONS catalog pushed by OP_RealEstate_Definitions
// (0x2698, see 43.8) and sets the type from the matching definition. That makes decoding
// the 0x2698 definition record (0x007F5B60, ~22 dword fields per entry) the next task.
//
// FIELD OBSERVATION: the client sends NOTHING real-estate related on its own — a zone
// log from a full neighborhood zone-in shows no real-estate opcodes inbound. The system
// is SERVER-DRIVEN: nothing happens until the server establishes context first.
// ============================================================================


// ---------------------------------------------------------------------------
// 43.11  GATE PROVEN + FIRST CLIENT->SERVER CAPTURE + OPCODE LIST CORRECTED
//        (in-game session 2026-07-27, Sunrise Hills / neighborhood 712)
// ---------------------------------------------------------------------------
//
// (a) THE TYPE FIELD IS THE ONLY GATE — CONFIRMED.
//     A live memory read reported: currentType(+0x90) = 10
//     Poking it to 5 and running "/realestate plot" OPENED THE PLOT SEARCH WINDOW.
//     So 43.10 is correct and complete: mgr->[0x90] == 5 is the entire neighborhood gate.
//     NOTE the default value was **10**, not 0 — the client sets a type on its own (10 =
//     some non-neighborhood value; 0x007F5F70 case 10 returns false). Whatever writes 10
//     is the same path that must be made to write 5, so it is worth finding.
//
// (b) FIRST CLIENT->SERVER PACKET EVER CAPTURED.  With "Packet C->S" logging at Detail,
//     opening the plot window made the client send:
//         [OP_Unknown] [0x2691]  Size 226
//         0:   04 00 00 00  FF FF FF FF  00 00 00 00 00 00 00 00
//         ...  (zero filled)
//         208: 00 00 00 00  00 00 00 00  00 00 00 00  01 01 D0 00
//     [inferred] the PLOT SEARCH QUERY: leading dword 4 (a sub-command / search kind),
//     0xFFFFFFFF as an "unfiltered/any" sentinel, a large zeroed filter block, and a
//     trailing 01 01 D0 00. 226 bytes is a fixed-size request struct.
#define OP_RealEstate_PlotSearchRequest                             0x2691  // [inferred] C->S, 226 bytes
//     KEY ARCHITECTURAL POINT: 0x2691 also appears INSIDE the client's own inbound
//     handler (0x005B2500), so this opcode is used in BOTH directions — the client sends
//     a query on it and expects the reply on it. Expect that request/reply-on-one-opcode
//     pattern across this subsystem.
//
// (c) THE OPCODE LIST IN 43.2 WAS INCOMPLETE — it is 30, not 17.
//     The first extraction only matched `param_2 == 0x....`; the handler also dispatches
//     via `!=` tests and nested branches. Full set of opcodes the client's real-estate
//     handler recognises:
//       0x0351 0x087F 0x0C13 0x1097 0x1228 0x1967 0x1BAF 0x1CEF 0x21EA 0x21EC
//       0x2691 0x2698 0x2AA6 0x2AA7 0x2CF7 0x3968 0x3D94 0x3E00 0x45BE 0x485D
//       0x4F09 0x58C7 0x5A63 0x63FD 0x66BF 0x66DF 0x68BA 0x6FD0 0x76C6 0x7D28
//     Newly found beyond 43.2's seventeen (all currently UNLABELLED):
#define OP_RealEstate_Unk0C13                                       0x0C13  // [inferred] unlabelled
#define OP_RealEstate_Unk1097                                       0x1097  // [inferred] unlabelled
#define OP_RealEstate_Unk1967                                       0x1967  // [inferred] unlabelled
#define OP_RealEstate_Unk1BAF                                       0x1BAF  // [inferred] unlabelled
#define OP_RealEstate_Unk2CF7                                       0x2CF7  // [inferred] unlabelled
#define OP_RealEstate_Unk3E00                                       0x3E00  // [inferred] unlabelled
#define OP_RealEstate_Unk45BE                                       0x45BE  // [inferred] unlabelled
#define OP_RealEstate_Unk5A63                                       0x5A63  // [inferred] unlabelled
#define OP_RealEstate_Unk66DF                                       0x66DF  // [inferred] unlabelled
#define OP_RealEstate_Unk68BA                                       0x68BA  // [inferred] unlabelled
#define OP_RealEstate_Unk76C6                                       0x76C6  // [inferred] unlabelled
#define OP_RealEstate_Unk7D28                                       0x7D28  // [inferred] unlabelled
//     NOT opcodes (binary-search boundaries the compiler emitted, do not register them):
//       0x1CF0 0x2699 0x3969 0x485E 0x63FE 0x6FD1
//
// METHOD NOTE for future extraction: a compiled C++ switch becomes a binary search, so
// scraping only `==` misses cases AND picks up `< boundary+1` values that are not real
// opcodes. Always separate by operator and sanity-check any value that is exactly
// (known opcode + 1).
// ============================================================================


// ---------------------------------------------------------------------------
// 43.12  OP_RealEstate_PlotSearchRequest (0x2691) — STRUCT PARTIALLY DECODED
//        from live client captures, 2026-07-27
// ---------------------------------------------------------------------------
// Captured with "Packet C->S" logging at Detail while using the Plot Search window in
// Sunrise Hills. SIX requests in ~30s — the window appears to RETRY when the server
// never answers, which is good news: it will accept a late reply.
//
// Fixed 226-byte request. Two sub-commands observed, differing in the leading dword:
//
//   sub-cmd 4 (the plain/unfiltered search sent when the window opens):
//      0: 04 00 00 00  FF FF FF FF  00 00 ... all zero ...
//    208: 00 00 00 00 00 00 00 00  00 00 00 00 01 01 D0 00
//
//   sub-cmd 5 (search BY OWNER NAME -- note the character name in the body):
//      0: 05 00 00 00  FF FF FF FF  00 00 ... zero ...
//     80: 00 00 00 00 00 00 00 00  00 00 00 00 <owner name bytes ...>
//     96: <... owner name continued, NUL-padded>
//    208: 00 00 00 00 00 00 00 00  00 00 00 00 00 01 AA 00
//
// Layout so far (offsets are byte offsets into the packet body):
//   +0x00  uint32  search_type / sub-command   (4 = unfiltered, 5 = by owner name)
//   +0x04  uint32  0xFFFFFFFF                  [inferred] "any" sentinel — an unset
//                                              id filter (plot id? zone id? real estate id?)
//   +0x08 .. +0x5B  zeroed                     [inferred] further unset filter fields
//   +0x5C  char[]  OWNER NAME, NUL-padded      (observed at byte 92)
//   +0x9C .. +0xDB  zeroed                     [inferred] remaining filter fields
//   +0xDC  4 bytes trailer, VARIES:            04 -> 01 01 D0 00
//                                              05 -> 00 01 AA 00
//                                              [inferred] paging / sort / result-cap
//   total 226 bytes
//
// Not yet mapped: everything between +0x08 and +0x5B and between +0x9C and +0xDB — the
// Plot Search window exposes several filter controls, so driving them one at a time and
// re-capturing will light up individual fields. That is the cheapest way to finish this
// struct: change ONE control, capture, diff.
//
// NEXT: the reply. 0x2691 is handled by the client's own inbound switch (0x005B2500), so
// decompiling that branch gives the RESPONSE format — answer it and the window populates.
// ============================================================================


// ---------------------------------------------------------------------------
// 43.13  OP_RealEstate 0x2691 — THE REPLY FORMAT (server -> client)
// ---------------------------------------------------------------------------
// 0x2691 is REQUEST AND REPLY ON ONE OPCODE. Inbound it is dispatched by the client's
// handler (0x005B2500) to:
#define RealEstate__ParseSearchReply_x                              0x005B0F80
//
// *** THIS SECTION WAS PARTLY WRONG AND WAS CORRECTED 2026-07-28 (re-decompiled before
// *** anything was sent). Two errors, both fatal to a sender: the branch test was
// *** described backwards, and every embedded STRING was missing. Fixed below.
//
// Reply envelope, read with the standard {buf,len,cursor} stream reader:
//   +0x00  uint32   SKIPPED by the parser (cursor advanced 4, value not consumed here)
//   +0x04  uint8    OWNER-TYPE byte, tested by 0x007F6030
//   then THE BRANCH — note which way round it goes:
//     0x007F6030(t) == FALSE (t is anything but 4/8, e.g. 0)  -> LIST reply:
//   +0x05  uint32   result kind -> selects the destination window:
//                      4 or 5 -> RealEstatePlotSearchWnd   (pinst 0xD1FCC4) -> 0x0074C4C0
//                      3      -> ignored
//                      other  -> RealEstateNeighborhoodWnd (pinst 0xD1FCC0) -> 0x0074AB50
//     0x007F6030(t) == TRUE  (t is 4 or 8)                   -> SINGLE-RECORD reply
//                      (one owned real estate, parsed inline — layout at the end of section)
//   So a LIST must carry an owner-type byte that is NOT 4 and NOT 8. Sending 4 silently
//   takes the single-record branch and no list is ever filled.
#define RealEstate__IsOwnerType_x                                   0x007F6030  // true for 4 and 8 ONLY
#define RealEstate__FillPlotSearchWnd_x                             0x0074C4C0
#define RealEstate__FillNeighborhoodWnd_x                           0x0074AB50
//
// NEIGHBORHOOD ROW BLOCK (0x0074AB50, kind 0/1) — *** VERIFIED IN GAME 2026-08-01 ***
// (a server answered a guild-lobby gate click; the Neighborhood Search window opened and
// rendered every field):
//   uint16  row_count           <-- 2 bytes, like the plot-search form
//   uint8   show_flag           nonzero -> window SHOWN via vtable+0xD8(1,1,1); an empty
//                               list (count 0) + show 1 still pops the window
//   [only when row_count > 0]
//   uint32  header_id           stored at wnd[0x8b] ([inferred] selected row id)
//   repeat row_count times:
//       uint32  id              row key (neighborhood id)
//       string  name            (0x0080C9B0 reader) -> the NEIGHBORHOOD column
//       uint32  guild_id        -> the GUILD column via 0x00425670; -1 renders blank, so
//                               0x00425670 is a guild-id -> guild-name resolver [observed
//                               blank at -1; name resolution inferred from the column]
//       uint32  pplots_total    -> "PPlots" column   (player plots)
//       uint32  pplots_used     -> "Used" column
//       uint32  gplots_total    -> "GPlots" column   (guild plots)
//       uint32  gplots_used     -> "Used" column
//   kind == 2 fills the SECOND list (wnd[0x93], the Guild Plots pane): per row a string
//   + one uint32 (not yet exercised).
//   The window's Search / My Plots / My Items / Go buttons and row activation send C->S
//   packets that are NOT yet answered server-side (0x218C, 14 bytes, sent from this
//   window's region 0x0074A7E8, is the travel/selection candidate).
//
// PLOT-SEARCH ROW BLOCK (0x0074C4C0) — this is the part the server must produce.
// *** VERIFIED IN GAME 2026-07-28 with a 118-byte, 3-row reply: the window
// *** rendered every field. The column mapping below is OBSERVED, not inferred.
//   uint16   row_count            <-- 2 bytes, NOT 4
//   string   header               <-- length-prefixed (0x0080C9B0), READ EVEN WHEN
//                                     row_count == 0, then handed to the window via
//                                     vtable +0x124 == set the WINDOW TITLE. Sending
//                                     a test header string retitled the Plot Search window.
//   repeat row_count times:
//       uint32  real_estate_id    -> resolved via 0x00665A60 into the ROW ICON (each id
//                                  produced a different icon). 0x00665A60 CLAMPS an
//                                  out-of-range id into [mgr+0x18, mgr+0x1C] and
//                                  null-guards, so an unknown id is safe, not fatal.
//                                  It does NOT feed the Address text.
//       uint32  owner_type        **8 -> "Guild", 4 -> "Player"** in the TYPE column
//                                  (literal strings in the binary); any other value and
//                                  the row is SKIPPED — but its bytes are still CONSUMED,
//                                  so a skipped row must still be well-formed
//       uint32  owner_id          stored on the row object, NOT displayed in any column
//                                  ([inferred] used when the row is clicked)
//       string  s1                -> the **OWNER** column
//       string  s2                -> the **ADDRESS** column
//   rows are only read at all when row_count != 0 AND kind is 4 or 5 AND the window's
//   [0x8E] member is non-null.
//   after the loop: 0x0074BB50(0) then vtable +0x184 on window[0x8E] to refresh the list.
//
// Minimum well-formed empty list = 4 + 1 + 4 + 2 + 4 = 15 bytes.
// One row with empty strings adds 4+4+4+4+4 = 20 bytes.
//
// SINGLE-RECORD BRANCH (owner-type byte 4 or 8) — *** VERIFIED IN GAME 2026-07-28 ***
// 80 bytes, id 1000, owner_type 4 => the client opened the real-estate PURCHASE window
// (Owner / Type / Price / Upkeep / Capacity / Vault / Pets / Category, buttons
// Manage / Items / Buy). So THIS is the packet that drives the buy-a-plot UI.
//   Read order:
//   uint32 skipped, int16 flag, uint32 real_estate_id (hash key), uint32 a, uint32 b,
//   uint32 c, uint32 d, string s0, string s1, string s2, string s3,
//   4 x uint32 skipped, uint8 refresh_flag.
//   Existing ids update in place; unknown ids allocate a 0x58-byte object via 0x007FE1E0.
//   0x007FE1E0 installs RealEstate::vftable and has EXACTLY ONE caller in the binary
//   (0x005B1224, in this branch) — this packet is the only wire route to construct a
//   client-side RealEstate object.
//
// *** THE PLOT MUST POINT AT A DEFINITION — CONFIRMED 2026-07-28 ***
// With a marker catalog loaded (definition id 100, f[i] = 100 + i) the Purchase window
// showed Capacity/Vault/Pets = 2000/2000/2000 (client defaults) for as long as a/b/c/d
// were 201/202/203/204. Zone made NO difference — PoK and a live neighborhood
// (Sunrise Hills) behaved identically. Setting **a/b/c = 100** (the definition id) made
// the client resolve the catalog and the marker values appeared immediately:
//
//     Capacity 104  =>  definition field f[4]   is CAPACITY
//     Vault    105  =>  definition field f[5]   is VAULT capacity
//     Pets     112  =>  definition field f[12]  is PET capacity
//
// That is 3 of the 17 catalog fields identified, by observation rather than inference.
//
// *** `b` IS THE DEFINITION ID — the SECOND dword after real_estate_id (ctor obj[8]). ***
// Bisected 2026-07-28 with FRESH ids (a first attempt on a single reused id was retracted;
// see the caching trap below):
//     id 2001, a=100  -> did NOT resolve, and never took over the window
//     id 2002, b=100  -> RESOLVED: Capacity 104 / Vault 105 / Pets 112 from the catalog
//     id 2003, c=100  -> did NOT resolve, sent twice, window stayed on 2002
// Second signal worth remembering: a record whose definition cannot be resolved does not
// become the displayed record at all — the window keeps showing the last one that did.
//
// Read order, final:
//     uint32 real_estate_id   hash key            -> obj[5]
//     uint32 a                unidentified        -> obj[7]
//     uint32 b                DEFINITION ID       -> obj[8]
//     uint32 c                unidentified        -> obj[9]   (updatable in place)
//     uint32 d                unidentified        -> obj[10]
//     ...    s3 (owner name)                      -> obj[0xB]
//     ...    s2                                   -> obj[0xC]
//
// *** THE CACHING TRAP — it invalidates any probe that reuses a real_estate_id ***
// The client keeps RealEstate objects in a hash map keyed by real_estate_id. The
// constructor runs ONLY for an unseen id. For an id already in the map, 0x005B0F80 takes
// the UPDATE path, which writes exactly two things:
//       FUN_005B4FE0(obj, &s3);                  // the s3 string
//       (**(code **)(*mgr + 8))(obj, c);         // the c dword
#define RealEstate__UpdateOwnerString_x                             0x005B4FE0
// definition_id, b, d and the other strings are NOT updated, and the window keeps showing
// the FIRST record's values indefinitely. A bisect run against a single id therefore
// returns a CONSTANT result that reads exactly like a confirmed answer.
// => every probe that varies a field MUST use a fresh real_estate_id.
// This is also a genuine finding for the real implementation: to change a plot's
// definition, owner_type, or most of its strings, the server cannot just resend — the
// client has no path to rebuild an existing record from this opcode.
//
// OBSERVED FIELD -> WIDGET MAPPING for this record (marker strings PS0..PS3):
//     string s1        -> the WINDOW TITLE ("PS1 (1000)")
//     string s3        -> the OWNER field
//     real_estate_id   -> shown in parentheses in the title
//     owner_type 4     -> Type "Player"  (8 => "Guild")
//     s0, s2           -> not seen on screen yet
//   NOTE this contradicts the static reading that s0/s1 are read-then-freed while s2/s3
//   go to the constructor — s1 clearly reaches the UI. Trust the screen, not the decompile.
//
// A record with a resolvable definition also flips the BUTTONS from the purchase set
// (Manage / Items / Buy) to the owned set (Manage / Items / Move Out / Transfer), and
// blanks the Price/Upkeep dropdowns. So owner_type + a resolvable definition is what makes
// the client treat a plot as OWNED.
//
// *** THE MANAGEMENT ACTIONS ARE GATED ON PHYSICAL OCCUPANCY — 2026-07-28 ***
// Move Out and Transfer render ENABLED but emit NOTHING when clicked. The Transfer tooltip
// says why, verbatim:
//     "Transfers the current plot to the targeted player. You must be standing inside this
//      plot and be the primary owner of this plot."
// and /realestate manage answers "You must be inside a plot or house to use that".
// So three gates: (1) standing INSIDE the plot, (2) primary owner, (3) a targeted player
// for Transfer. A record in the client's map is not bound to any physical space, so gate
// (1) fails and every management action is inert. Manage being greyed is gate (1), NOT an
// ownership test — that misread cost a round of probes.
//
// RULED OUT for ownership: a, c and d are NOT a character id. Sent as the test
// character) in each slot on fresh ids 4001/4002/4003 — nothing unlocked.
//
// STILL UNKNOWN, and it is the next real blocker: what tells the client "you are standing
// in plot N". Until that is solved the native transfer/move-out flow cannot be exercised at
// all — which also means the A-vs-B transfer question (43.13 caching) cannot be settled yet.
//
// TESTED AND RULED OUT 2026-07-28 — doors do NOT carry plot identity.
// peq.doors holds a real plot grid for the neighborhood zone: 152 OBP_LOTSQUARE rows with
// per-plot doorid/position/heading (NOTE: every lot square is DUPLICATED — 152 rows for ~76
// plots, e.g. ids 33956 and 149781 are both doorid 1. Stock PEQ data issue, but it
// will bite anything that keys plots to doorids). `door_param` is -1 on all of them, which
// looked like the obvious carrier for a real-estate id. It is not: setting doorid 1's
// door_param to a valid plot id, reloading doors, standing exactly on the square and
// loading that plot still left Manage greyed and every button silent.
//
// *** SOLVED 2026-07-28: THE OCCUPANCY GATE IS RealEstateManagerClient + 0x98. ***
#define RealEstateManagerClient__CURRENT_ID_OFS                     0x98
#define RealEstate__RequireInsidePlot_x                             0x00747E50
#define RealEstate__OpenManageWnd_x                                 0x00747740
// Found by taking the runtime refusal text back to its string id instead of guessing wire
// formats: "You must be inside a plot or house to use that." is eqstr **727** (0x2D7), and
// only three functions push 727. 0x00747E50 is the real-estate one, and it IS the gate:
//
//     mgr = RealEstateManagerClient::Instance();      // 0x005B3540
//     uint32 cur = *(uint32 *)(mgr + 0x98);           // CURRENT real-estate id
//     if (cur != 0xFFFFFFFF) {
//         // hash lookup in the manager's OWN map: buckets at mgr+0x04, count at mgr+0x08
//         for (node = buckets[cur % count]; node; node = node->next)
//             if (node->key == cur && node->obj != 0) { OpenManageWnd(); return 1; }
//     }
//     print eqstr 727;  return 0;
//
// So "standing inside plot N" is literally just this dword. 0xFFFFFFFF = nowhere, which is
// what it has been all session — which is why Manage stayed greyed and Move Out / Transfer
// emitted nothing no matter what was sent.
//
// TWO CONSEQUENCES:
//  * It is directly pokeable in memory exactly like the +0x90 type gate (43.10), so the
//    whole management flow can be exercised without knowing which opcode sets it.
//  * The plot must ALSO be present in the manager's map with a non-null object — i.e. the
//    single-record 0x2691 must have been received for that id first. Setting +0x98 to an id
//    the client has never been told about fails the inner loop and still refuses.
//
// This also closes an earlier loose end: the map at mgr+0x04/+0x08 is the SAME map the
// single-record branch walks in 0x005B0F80, so the `local_24` the decompiler could not
// resolve there is just the manager instance.
//
// *** +0x98 IS DERIVED, NOT SETTABLE — PROVEN IN GAME 2026-07-28 ***
// Writing 9001 and reading back immediately returns -1. The client RECOMPUTES this field
// on a timer, so poking it can never stick and no wire trick will make it stick either.
//
// THE OCCUPANCY MACHINE (0x005B0A10, periodic, on the manager):
#define RealEstateManagerClient__UpdateOccupancy_x                  0x005B0A10
#define RealEstateManagerClient__ResolveCurrentChild_x              0x005B00E0
#define RealEstate__FindChildAtPosition_x                           0x004D6790  // [inferred]
#define RealEstate__GetLocalPosition_x                              0x007F62B0  // [inferred]
#define RealEstate__GetCurrentParentId_x                            0x004B3040  // [inferred]
#define RealEstateManagerClient__MAP_ELEMENT_COUNT_OFS              0x0C
#define RealEstateManagerClient__LAST_UPDATE_TICK_OFS               0x88
#define RealEstateManagerClient__FALLBACK_PARENT_ID_OFS             0x8C
#define RealEstateManagerClient__CURRENT_PARENT_ID_OFS              0x94
//     if (*(int *)(mgr + 0x0C) < 1) {          // map is EMPTY -> nothing can be occupied
//         mgr[0x94] = mgr[0x8C];
//         mgr[0x98] = mgr[0x9C] = mgr[0xA0] = 0xFFFFFFFF;
//     } else if (mgr[0x88] + 1000 <= now) {    // otherwise once per second
//         if (DAT_00DCEE30) mgr[0x94] = FUN_004B3040(DAT_00DD2630);   // current PARENT
//         if (mgr[0x94] == -1) mgr[0x94] = mgr[0x8C];
//         FUN_005AFFC0(); FUN_005B00E0(); FUN_005B0140();
//     }
//
// and 0x005B00E0 computes the gate itself:
//     node = buckets[ mgr[0x94] % bucketcount ];        // look up the PARENT estate
//     for (...) if (node->key == mgr[0x94]) obj = node->obj;
//     result = FUN_004D6790(obj, FUN_007F62B0(0), 0);   // which CHILD contains my position?
//     mgr[0x98] = result ? *(uint32 *)(result + 0x14) : 0xFFFFFFFF;
//
// >>> ARCHITECTURAL CONSEQUENCE <<<
// Occupancy is HIERARCHICAL and position-based. The client finds the PARENT real estate (the
// neighborhood) and then asks which CHILD plot contains the player. A standalone plot record
// with no parent can never satisfy it.
//
// To make it work the server must give the client:
//   1. a NEIGHBORHOOD real-estate record to act as the parent (mgr+0x94 / +0x8C resolve to it),
//   2. that neighborhood's definition carrying its CHILD PLOT IDS in the trailing `ids[]`
//      array of the 0x2698 record (43.14), and
//   3. per-plot geometry good enough for 0x004D6790 to do the point-in-plot test.
// DECODED 2026-07-28 — 0x004D6790 is NOT a point-in-polygon test, it is a PARENT-CHAIN WALK:
//     FUN_004D6790(mgr, estate, type_mask, depth):
//       if (depth < 1) {
//           def  = FUN_007F4FA0(*(uint32 *)(estate + 0x20));  // its definition
//           bits = FUN_007F5E30(def);                          // that definition's TYPE bits
//           if (type_mask & bits) return estate;               // match
//       }
//       estate = map_lookup(*(uint32 *)(estate + 0x1C));       // WALK UP TO THE PARENT
//       ... at most 10 levels
//
// >>> THIS IDENTIFIES `a` — THE FIRST UNKNOWN DWORD IN THE PLOT RECORD <<<
// The real-estate object lays out exactly onto the wire record (ctor 0x007FE1E0):
//     obj+0x14 = obj[5]  = real_estate_id
//     obj+0x1C = obj[7]  = **PARENT real_estate_id**   <-- the wire's `a`
//     obj+0x20 = obj[8]  = definition_id               <-- the wire's `b`
// `a` is the PARENT LINK that stitches plots into a neighborhood. A plot sent with 0 is
// an orphan with nothing to walk up to.
//
// THE TYPE ENCODING. FUN_007F5E30 is just `1 << (type - 1)`:
//     type 1->0x001  2->0x002  3->0x004  4->0x008  5->0x010  6->0x020
//     type 7->0x040  8->0x080  10->0x200  0xB->0xFFFFFFFF (wildcard)
// FUN_007F62B0 returns the constant mask **0x88 = 0x80|0x08 = types 8 and 4** — i.e. the
// OWNER types (Player 4 / Guild 8, the same pair 0x007F6030 tests). So +0x98 becomes the id
// of the nearest ancestor estate whose DEFINITION TYPE is 4 or 8.
//
// AND THAT LOCATES THE TYPE FIELD. FUN_007F4FA0(defmgr, definition_id) walks the definition
// map (key at node+0x6C, next at node+0x70) and returns **node+0x3C**, defaulting to 10 when
// the definition is missing. Per the 43.14 field map, obj[0x3C] is **f09**:
#define RealEstate__DefinitionTypeOf_x                              0x007F4FA0
#define RealEstate__TypeToBitmask_x                                 0x007F5E30
#define RealEstate__OwnerTypeMask_x                                 0x007F62B0  // returns 0x88
#define RealEstateDefinition__TYPE_OFS                              0x3C        // = wire f09
//     ***** f09 IS THE REAL-ESTATE TYPE. *****
// Type 0 falls through FUN_007F5E30's switch and yields 0, so `mask & 0` is always false and
// +0x98 can never resolve. Setting f09 = 4 (player-owned) or 8 (guild-owned) is required
// before any plot can be "occupied".
//
// mgr+0x94 (the innermost current estate) comes from FUN_004B2F40, and that is where the
// trail ends for now. DECODED 2026-07-28:
#define RealEstate__ResolveCurrentFromWorld_x                       0x004B2F40
#define RealEstate__FindEstateForObject_x                           0x004B30F0
#define RealEstate__PositionContainsTest_x                          0x008D2AD0  // [inferred]
#define RealEstate__DefinitionNodeOf_x                              0x007F4F70
//     for each node in <an object map>:
//         obj = node->value;
//         if (obj && FUN_008D2AD0(pos)) {                   // containment test
//             estate = FUN_004B30F0(mgr, *(obj + 4), mgr[0x8C]);
//             if (estate && IsOwnerType(TypeOf(estate->definition)))
//                 return *(uint32 *)(estate + 0x14);        // -> mgr+0x94
//         }
// and FUN_004B30F0 searches the estate map for one whose DEFINITION field +0x1C — that is
// the wire's **f01** — equals the world object's +0x04, rooted at mgr+0x8C or a descendant.
//
// >>> WHAT THIS MEANS <<<
// Occupancy is anchored to WORLD OBJECTS, not to doors and not to the plot record.
// The client scans objects it is tracking, tests whether the player is
// inside one, and reads the real estate off THAT object via the definition's f01. Lot-square
// doors are ordinary doors with no real-estate association, which is why standing on one has
// never mattered. The collection being scanned is [inferred] EQPlacedItemManager.
//
// THE ROOT: mgr+0x8C. Everything is constrained to it or its descendants, and the
// constructor (0x005B21E0) inits it to 0xFFFFFFFF:
//     param_1[0x23] = 0xFFFFFFFF;   // +0x8C root estate
//     param_1[0x24] = 10;           // +0x90 type  <-- why a fresh client reports type 10
//     param_1[0x25..0x28] = 0xFFFFFFFF;  // +0x94 +0x98 +0x9C +0xA0
// Nothing we have found yet sets +0x8C to a real value. Finding that writer is the next step:
// it is the "this zone IS real estate N" signal, and without it the whole resolution chain is
// rooted at -1 no matter what else is correct.
//
// TESTED 2026-07-28: setting the definition's f09 (type) to 4 is NECESSARY but NOT sufficient
// — +0x98 stays -1, because mgr+0x94 never resolves in the first place.
//
// NOT A BUTTON, for the record: C->S capture during the clicks produced exactly one
// unknown packet, OP 0x2FA0, 338 bytes, almost all zeros, 2s AFTER the window opened and
// none at all during the clicks. Its senders are a family of ~6 small functions around
// 0x004A69B0-0x004A70xx, each throttled to 3000 ms. That is a periodic sync, unrelated.
//
// *** A RETRACTED CLAIM, KEPT AS A METHOD WARNING ***
// This branch was predicted to be DEAD — a guaranteed READ fault at 0x00000008 — because
//       005B1196  MOV ECX,[ESP+0x24]   /   005B119E  DIV [ECX+0x8]
// loads a slot that is zeroed in the prologue (XOR EDI,EDI / MOV [ESP+0x24],EDI) with no
// other writer visible, and Ghidra's decompiler independently renders it as read-but-
// never-assigned. IT IS LIVE. The warning sign was present and was correctly treated as
// disqualifying rather than ignored: tracking ESP linearly through this function puts
// [ESP+0x4C] OUTSIDE the frame, which is impossible, so the frame arithmetic was unsound
// and every conclusion drawn from it was worthless. RULE: if the frame math does not
// close, do not assert — hedge and test. Two wrong "the client is broken" calls in one
// day (this and 43.16) both came from over-trusting stack analysis.
//
// >>> ARCHITECTURAL CONSEQUENCE <<<
// The rows carry IDs, not descriptions. The client renders a plot list by looking each id
// up in data it ALREADY HAS — i.e. the definitions catalog pushed over
// OP_RealEstate_Definitions (0x2698, see 43.8). So the working order is:
//     1. send the DEFINITIONS catalog (0x2698) so the client knows what plots exist,
//     2. then answer 0x2691 searches with id + owner_type + owner_id rows.
// Answering 0x2691 alone would produce rows the client cannot describe. Decoding the
// 0x2698 definition record (0x007F5B60, ~22 dword fields) is therefore the true next task
// and is also the likely writer of the type gate in 43.10.
//
// Also note the client RETRIES 0x2691 (six sends in ~30s with no answer), so a late
// reply is still accepted.
// ============================================================================


// ---------------------------------------------------------------------------
// 43.14  OP_RealEstate_Definitions (0x2698) — RECORD LAYOUT DECODED
// ---------------------------------------------------------------------------
// Chain:  0x2698 -> 0x007F5B60 (packet loop) -> 0x007F4C90 (per-record reader)
//                -> 0x007F52F0 (insert into the client's definitions map)
#define RealEstate__ReadOneDefinition_x                             0x007F4C90
#define RealEstate__StoreDefinition_x                               0x007F52F0
#define RealEstate__ResizeDefIdArray_x                              0x0064F910
#define RealEstate__ReadLengthPrefixedString_x                      0x0080C9B0
//
// PACKET (0x007F5B60):
//     uint32  definition_count
//     repeat definition_count times: <definition record>
//
// DEFINITION RECORD (0x007F4C90) — bounds-checked little-endian dwords WITH ONE EMBEDDED
// LENGTH-PREFIXED STRING sitting between f10 and f11. *** CORRECTED 2026-07-28 *** — the
// first pass through this function missed the string because the decompiler renders the
// call as `FUN_0080c9b0(param_1 + 3)` with the reader passed in a register, so it reads
// like a default-initialise rather than a stream read. It is a stream read, and omitting
// its 4-byte length field is what crashed the client all through 2026-07-27 (43.16).
//
//     uint32  f00   -> obj[0x00]      definition id (read first, used as the map key) CONFIRMED
//     uint32  f01   -> obj[0x1C]
//     uint32  f02   -> obj[0x20]
//     uint32  f03   -> obj[0x24]
//     uint32  f04   -> obj[0x28]      *** CAPACITY ***      (observed in game, 43.13)
//     uint32  f05   -> obj[0x2C]      *** VAULT capacity *** (observed in game, 43.13)
//     uint32  f06   -> obj[0x30]
//     uint32  f07   -> obj[0x34]
//     uint32  f08   -> obj[0x38]
//     uint32  f09   -> obj[0x3C]      *** REAL-ESTATE TYPE *** (see 43.13 occupancy)
//     uint32  f10   -> obj[0x40]
//     uint32  name_len                <-- THE FIELD THAT WAS MISSING
//     char    name[name_len]  -> obj[0x0C]   raw bytes, ONLY present when name_len > 0
//     uint32  f11   -> obj[0x44]
//     uint32  f12   -> obj[0x48]      *** PET capacity ***  (observed in game, 43.13)
//     uint32  f13   -> obj[0x4C]
//     uint32  f14   -> obj[0x50]
//     uint32  f15   -> obj[0x54]
//     uint32  f16   -> obj[0x58]
//     uint32  id_count
//     uint32  ids[id_count]           (array resized via 0x0064F910, then filled in order)
//
//   => 17 payload dwords, NOT contiguous: 11 dwords, a string, 6 dwords, then the id list.
//   A one-definition catalog with an empty name and no ids is 84 bytes on the wire
//   (def_count + 11 + name_len + 6 + id_count + cost_count = 21 dwords).
//
// THE STRING READER (0x0080C9B0), which is a generic CXStr-from-stream helper:
//     int32 len = read_dword();                 // truncation-trap default, see 43.15
//     if (len < 1) { release(dest); dest = NULL; return; }   // consumes ONLY the 4 bytes
//     copy_string(buf + cursor, len); cursor += len;         // len counts BYTES, NUL included
//   So name_len == 0 is legal and cheap — but the dword itself is never optional.
//
// NOTES
//  * The record object carries THREE string members (obj+0x04, +0x08, +0x0C — all released
//    by the map's clear routine 0x007F5A50 via 0x00805B20), but only obj+0x0C is ever read
//    from the wire. The other two are client-side only.
//  * The caller (0x007F5B60) pre-seeds the record with defaults before reading:
//    field@0x44 = 10 (decimal) and field@0x3C = 0x150 (336), other members 0, one = 1.
//    Those are the values a definition keeps if the server sends nothing meaningful.
//  * The trailing id array is [inferred] the list of member/child real-estate ids belonging
//    to this definition — e.g. the plot ids inside a neighborhood. That matches the
//    neighborhood -> 146 OBP_LOTSQUARE plot layout already present in peq.doors.
//
//
// *** CONFIRMED WORKING 2026-07-28 (PoK, no crash, catalog accepted): ***
//     marker catalog (f[i] = 100 + i)      -> 84 bytes = exactly the predicted 21 dwords
//     same with name "Testplot"            -> 93 bytes = 84 + 9 ("Testplot" + NUL)
//   Two different string lengths both parsed cleanly, so name_len handling is correct and
//   the record layout in this section is CONFIRMED, not inferred.
//
// *** RETRACTED 2026-07-28 — THIS ENTIRE PARAGRAPH RESTED ON A MISIDENTIFIED GLOBAL. ***
// It read as: "the only real writer of RealEstateManagerClient+0x90 lives in a camera/mode
// routine gated on DAT_00d1fd9c == 6, doing *(DAT_00dd2630 + 0x90) = DAT_00de0d7c[0xc]".
//
// 0x00DD2630 IS NOT A REAL-ESTATE MANAGER. It is pinstLocalPlayer_x, already defined at the
// top of this file (line ~132). So that store writes localPlayer+0x90 — an unrelated field on
// a completely different object — and says nothing whatsoever about the manager. The
// "_Alt_x" define below was a duplicate of a known global under an invented name; defining
// one address twice under two names is how a wrong label survives review, so it is gone.
//
// The REAL writer of RealEstateManagerClient+0x90 is the reset path (43.19): every one of
// them stores the constant 0xA, which is the "none" sentinel.
// #define pinstRealEstateManagerClient_Alt_x   0x00DD2630   <- DELETED, it was pinstLocalPlayer_x
#define pinstCameraTransformObject_x                                0x00DE0D7C   // [inferred]
#define RealEstate__CameraModeGlobal_x                              0x00D1FD9C   // [inferred]
// CORRECTION 2026-07-28: 0x00DE0D7C was first labelled "current real estate object". It is
// NOT. Its single writer is 0x00533C9E, inside a startup constructor that does
// operator new(0x48), zeroes ~16 floats and installs vftable 0x009D1188 — a camera/transform
// object built once at init. All 18 refs in .text are reads except that one store. So
// "which plot am I standing in" is tracked somewhere else entirely and is STILL UNFOUND.
// So +0x90 is copied from the CURRENT real-estate object (+0x30), not from the catalog.
// Identifying the definition fields needs a different readout channel — the name string
// is the obvious one, since anything the UI renders is directly observable.
// ============================================================================


// ---------------------------------------------------------------------------
// 43.15  *** TRAP: A SHORT 0x2698 PACKET CRASHES THE CLIENT ***
//        (learned the hard way, 2026-07-27)
// ---------------------------------------------------------------------------
// OP_RealEstate_Definitions (0x2698) does NOT contain one block. Its handler runs TWO
// parsers over the SAME buffer, back to back, then a post-process that reads nothing:
//     0x007F5B60  definitions block   (43.14)
//     0x007FC6B0  COSTS block         <-- easy to miss
//     0x005AAAD0  post-process (operates on the parsed tables, consumes no bytes)
//
// FULL WIRE FORMAT:
//     uint32  definition_count
//     repeat: <definition record — see 43.14; 11 dwords, a length-prefixed string,
//              6 dwords, uint32 id_count, uint32 ids[id_count]>
//     uint32  cost_count
//     repeat: <cost record>            (per-record reader 0x007FC2B0, format NOT yet decoded)
//
// WHY TRUNCATION IS FATAL RATHER THAN IGNORED. Both loops open with the same idiom:
//     piVar = param_2;                                   // <-- seeded with the READER POINTER
//     if (cursor + 4 <= len) piVar = read_dword();       // only overwritten if data remains
//     for (; piVar != 0; piVar = piVar - 1) { ... }
// So when the buffer is exhausted the count is NOT zero — it is the address of the reader
// struct (e.g. 0x0D8C1FE4), and the loop runs ~3.6 billion iterations allocating and
// writing through garbage until it faults.
// Observed: sending definitions WITHOUT the trailing cost_count crashed the client with
//     c0000005, "trying to WRITE invalid data", virtual address 0x00000000,
//     ADDR=0x00B6DF45, gamestate 5, in neighborhood.
//
// RULE FOR THE SERVER IMPLEMENTATION: every 0x2698 must carry BOTH counts even when both
// are zero (a valid empty catalog is 8 bytes). Assume the same pattern applies to every
// other multi-block real-estate opcode — audit each one for trailing blocks before
// sending it, because the failure mode is a client crash, not a dropped packet.
// ============================================================================


// ---------------------------------------------------------------------------
// 43.16  CRASH SOLVED -- it was the packet: a missing string-length dword
//        (diagnosed 2026-07-27, root-caused 2026-07-28)
// ---------------------------------------------------------------------------
// Symptom: 0x2698 with definition_count=0 is fine; definition_count=1 CRASHED the client
// every time - regardless of field values (all-zero and all-marker both died), regardless
// of zone (neighborhood and PoK both died), with:
//     c0000005, "trying to WRITE invalid data", virtual address 0x00000000,
//     EIP=0x00B6DF45, EDI=0, ECX=0x20, gamestate 5.
//
// The packet bytes were verified first, by capturing the server's send with Packet S->C logging:
//     [OP_RealEstateDefinitions] [0x2698]  01 00 00 00 then 76 zero bytes
//   = definition_count 1, seventeen zero fields, id_count 0, cost_count 0 — i.e. exactly
//   what was intended. That ruled out packet construction, and (wrongly)
//   pointed the investigation at the client.
//
// *** THE FIRST DIAGNOSIS IN THIS SECTION WAS WRONG AND HAS BEEN DELETED. *** It claimed
// the definitions hash map at 0x007F52F0 had a NULL bucket array. That was reasoned from
// the code alone and never checked against the EIP — because the EIP had not been REBASED
// (43.17). Rebasing it moves the crash out of 0x007F52F0 entirely.
//
// THE REAL CHAIN, after subtracting the ASLR delta 0x00360000 from every dbg.txt address:
//     0x005B27C4   return addr — the instruction AFTER `CALL 0x007FC6B0` in the 0x2698 handler
//     0x007FC729   inside 0x007FC6B0, the COSTS parser (not the definitions parser)
//     0x007FC32B / 0x007FBDF9 / 0x007FBF7B   cost record reader -> vector reserve
//     0x008DBB5A -> 0x008DC3EF -> 0x008E5597 operator new -> _malloc -> __callnewh
//     0x0080DF45   EIP
//
// `__callnewh` only runs WHEN MALLOC HAS ALREADY FAILED, and 0x0080DF30 is EQ's registered
// new-handler — a deliberate crash-on-out-of-memory:
#define EQ__NewHandler_CrashOnOOM_x                                 0x0080DF30
#define EQ__NewHandler_FailedAllocSize_x                            0x015D3664   // [inferred]
#define EQ__NewHandler_Flag_x                                       0x015D3660   // [inferred]
//     _DAT_015d3664 = requested_size;  _DAT_015d3660 = 1;  DAT_00000000 = 0;
//                                                          ^^^^^^^^^^^^^^^^ intentional
// So "WRITE to 0x00000000" in a RoF2 dbg.txt does NOT mean a null-pointer bug. It is the
// OOM handler. Whenever you see EIP resolve to 0x0080DF30+0x15, read it as
// "an allocation was refused" and go looking for an absurd size, not a null deref.
//
// WHY THE ALLOCATION WAS ABSURD: the record omitted the name_len dword (43.14), so the
// definitions parser consumed one dword too few per record and left the cursor short. The
// costs parser then hit the truncation trap (43.15), took the READER POINTER (~0x01FF7898)
// as its cost_count, and asked 0x007FBEF0 to reserve count * 0x14 bytes ≈ 670 MB.
//
// Both crash modes, exactly:
//   `zeros`   name_len read as 0 (benign) -> record then ate the cost_count as its
//             id_count -> cursor landed exactly at end-of-buffer -> costs count = pointer.
//   `markers` name_len read as 111 -> client copied 111 bytes out of an 80-byte buffer and
//             advanced the cursor to 163, far past len -> same truncation, same OOM.
//   empty     definition_count 0 skips the record, so cost_count sat where expected. Fine.
//
// FIX: emit name_len (0 is fine) between f10 and f11; a one-definition catalog is
// 84 bytes. The client-side map needs no
// special initialisation — that whole theory was an artefact of the un-rebased EIP.
// ============================================================================
//
// THE 0x2698 CALL SITE, in full (inside 0x005B2500, EBP = RealEstateManagerClient):
//     005B279A  LEA  EAX,[ESP+0x14]              ; the stream reader, built on the stack:
//     005B279E  MOV  [ESP+0x14],ECX              ;   +0x00 buf
//     005B27A6  MOV  [ESP+0x1C],EDX              ;   +0x04 len
//     005B27AA  MOV  [ESP+0x20],0                ;   +0x08 cursor
//     005B27A2  MOV  ECX,[EBP+0x74]              ; <-- definitions manager
//     005B27B2  CALL 0x007F5B60                  ;     definitions block
//     005B27BC  MOV  ECX,[EBP+0x78]              ; <-- costs manager
//     005B27BF  CALL 0x007FC6B0                  ;     costs block
//     005B27C4  MOV  ECX,[EBP+0x78]
//     005B27C7  CALL 0x005AAAD0                  ;     post-process, consumes no bytes
#define RealEstateManagerClient__DEFINITIONS_OFS                    0x74   // [inferred]
#define RealEstateManagerClient__COSTS_OFS                          0x78   // [inferred]
//
// 0x007F5B60 is a __thiscall despite Ghidra showing one arg. Its prologue does
// `LEA EBX,[ECX + 0x4]` and later `MOV ECX,EBX` before CALL 0x007F52F0, so the definitions
// hash map lives at (definitions_manager + 0x04). Map layout, from 0x007F52F0/0x007F5A50:
//     [0x00] bucket array ptr   [0x04] bucket count
//     [0x08] element count      [0x0C] used-bucket count
// Nodes are 0x74 bytes, key at node+0x6C, chain-next at node+0x70; the map self-rehashes
// (0x007F4BB0, doubling) once load exceeds 70%. Nothing here needs server-side priming —
// the earlier theory that this map was left unallocated is retracted, see above.
// ============================================================================


// ---------------------------------------------------------------------------
// 43.17  *** READ THIS BEFORE INTERPRETING ANY CLIENT dbg.txt ADDRESS ***
//        RoF2 eqgame.exe is ASLR-relocated — every logged address needs rebasing
// ---------------------------------------------------------------------------
// eqgame.exe PE header: ImageBase 0x00400000, SizeOfImage 0x012C3000,
// DllCharacteristics 0x8140 -> DYNAMICBASE **set**. Windows therefore relocates the image
// on every launch, and EVERY address printed in Logs\dbg.txt (the "fatal error ... ADDR="
// line and the whole ADDR= stack dump) is a RUNTIME address, not a static one.
//
// This burned a full day on 2026-07-27: crash EIP 0x00B6DF45 was read literally, landed in
// .data, and produced an entire false theory about an unallocated hash map (43.16). The
// static image only holds code in .text = 0x00401000-0x009BF3FF; anything above that is
// data, and an EIP "in .data" is the tell that you forgot to rebase.
//
// HOW TO RECOVER THE BASE WITH NO EXTRA INSTRUMENTATION. A genuine return address always
// sits immediately after a CALL. So: scrape every ADDR= value out of dbg.txt, then for each
// 64 KB-aligned candidate base, map all of them back (static = runtime - (base - 0x400000))
// and count how many land exactly one instruction past a CALL. The true base wins by an
// order of magnitude.
//
// Result for the 2026-07-27 poknowledge crash — decisive, not a judgement call:
//     base=0x00760000  delta=0x00360000  call-preceded hits=60   <-- winner
//     base=0x00AD0000                    call-preceded hits=6
//     base=0x00B30000                    call-preceded hits=5
// The delta is per-launch; recompute it for every new crash log.
//
// Corollary: any in-process code must resolve every offset as
// `addr - 0x400000 + (DWORD)GetModuleHandle(NULL)` rather than using the literal. Any new
// client-side code that hardcodes a raw eqgame address will work on no machine at all.
// ============================================================================

// ============================================================================
// 43.18  RTTI CLASS MAP + LIVE DEBUGGING (Cheat Engine) -- 2026-07-28
// ============================================================================
//
// WHY THE UNREAL TRICK DOES NOT PORT. Unreal ships RUNTIME REFLECTION: each UClass owns a
// linked list of FProperty carrying name + type + offset, indexed by GObjects/GNames. The
// engine hands over its own schema, so a script can print GEngine/World/PlayerArray/... with
// real labels. RoF2 is 2013 MSVC C++ with NO reflection: MEMBER NAMES DO NOT EXIST ANYWHERE
// IN THE BINARY. Every field name in this header is inference and always will be. Do not go
// looking for a name table; there isn't one.
//
// WHAT DOES EXIST: MSVC RTTI, compiled in. 436 type descriptors resolve to 546 vftables.
// That gives the CLASS half of the walker -- read [ptr+0], look up the vftable, get a real
// name. So "what is this pointer?" is answerable; "what is +0x94 called?" is not.
// Only POLYMORPHIC classes appear, so a miss means non-virtual, not wrong.
// Notable:
//     0x00A1052C  RealEstate        <- the housing class
//     0x00A1E9C4  PlayerBase
//     0x00A1A338  CXWndManager
// These are STATIC VAs at ImageBase 0x400000 -- rebase anything read live (43.17).
//
// THE ACTUAL LESSON, AND IT IS A METHOD ERROR WORTH NOT REPEATING.
// Cheat Engine's "Find out what writes to this address"
// sets a HARDWARE breakpoint (debug registers) and names the writing instruction the instant
// it fires. Section 43.13 closes on "find what writes mgr+0x8C" -- and that was being chased
// by byte-scanning for `mov [reg+0x8C]` and trying to force Ghidra to disassemble a region
// it had skipped. A write breakpoint answers it in one click, with a call stack.
//
//     STATIC ANALYSIS ANSWERS "WHAT DOES THIS CODE DO".
//     IT IS THE WRONG TOOL FOR "WHO TOUCHES THIS FIELD, AND WHEN".
//     For the second question reach for the breakpoint FIRST, then read the function Ghidra
//     already has. The two are complementary; using only one cost days on this system.
//
// RECIPE (ASLR-proof -- CE addresses modules relatively, so no rebasing during the hunt):
//     pinstRealEstateManagerClient_x = 0x00E64570 is a static global, RVA 0xA64570.
//       1. CE -> attach eqgame.exe
//       2. Add Address Manually -> [eqgame.exe+A64570]+8C  type 4 Bytes   (the root estate id)
//       3. right-click -> Find out what writes to this address
//       4. zone in / walk onto a lot square
//       5. rebase the reported EIP (subtract the module base, add 0x400000) before it lands
//          in this header.
//     Dissect Data/Structures on [eqgame.exe+A64570] lays out all RealEstateManagerClient__SIZE
//     (0xB4) bytes at once; combined with the vftable map above, embedded pointers self-identify.
//
// CE also has a Lua engine, so a CE Lua script plus an RTTI vftable dump
// is a genuine auto-naming structure walker for this client.
// It just names CLASSES, never fields.
// ============================================================================

// ============================================================================
// 43.19  THE OCCUPANCY CHAIN, DECOMPILED -- AND +0x8C IS NOT THE ROOT  (2026-07-28)
// ============================================================================
// *** THIS SUPERSEDES THE CHAIN DIAGRAM IN 43.13. READ THIS ONE. ***
//
// HOW IT WAS FOUND: Cheat Engine hardware breakpoint on the live manager (43.18).
//   "Find out what WRITES" -> NOTHING, ever, during play.
//   "Find out what ACCESSES" -> exactly two readers, 110 hits each in seconds:
//       runtime 0x00812F4A -> static 0x004B2F4A  (inside ResolveCurrentFromWorld 0x004B2F40)
//       runtime 0x00910A7B -> static 0x005B0A7B  (inside UpdateOccupancy      0x005B0A10)
//   Live value: 0xFFFFFFFF. (Session base was 0x760000, delta 0x360000 -- rebase per 43.17.)
//
// THE CORRECTION THAT MATTERS. 43.13 called +0x8C "the root of the whole resolution chain"
// and closed on "find what writes it -- while it stays -1 nothing else can resolve". That was
// BACKWARDS and it burned real time. A static scan of .text for every `mov [reg+0x8C], x`
// finds only FOUR writers inside RealEstateManagerClient, and ALL FOUR ARE RESETS TO -1:
//       0x005AFF20  mov [ecx+0x8C], -1 ; mov [ecx+0x90], 0xA ; jmp ...   (reset thunk)
//       0x005B1CF0  Reset()      -- 0x8C/0x94/0x98/0x9C/0xA0 = -1, 0x90 = 0xA, 0x88 = 0
//       0x005B239F  re-init      -- same block, after re-installing vftables
//       0x005B2443  constructor  -- installs 0x9D910C / 0x9D9108 / 0x9D90F0, then the block
// +0x8C IS A FALLBACK, NOT A SOURCE. It is SUPPOSED to sit at -1 forever. Nothing was missing.
//
// Corollary, and it explains a logged observation that was recorded but never explained:
// +0x90 == 0xA (10) IS THE "NONE" SENTINEL, written by every reset. A live read of
// currentType = 10 never meant "type 10", it meant "this manager has been reset and is idle".
//
// TWO NEW FIELDS fall out of the reset block: the live state is 0x88..0xA0, not 0x88..0x98.
#define RealEstateManagerClient__CURRENT_CHILD_OFS                  0x9C    // -1 when unresolved
#define RealEstateManagerClient__CURRENT_EXTRA_OFS                  0xA0    // -1 when unresolved
#define RealEstateManagerClient__PENDING_FLAG_OFS                   0xA4    // request in flight
#define RealEstateManagerClient__PENDING_TICK_OFS                   0xA8    // + 0x1388 = 5s timeout
#define RealEstateManagerClient__Reset_x                            0x005B1CF0
#define RealEstateManagerClient__Ctor_x                             0x005B2405
//
// The record hash map is the FIRST members of the manager, and 0x5AFFC0 / 0x5B00E0 / 0x5B0140
// all walk it identically (`div [this+8]`, bucket table `[this+4]`, node = {value, key, next}):
#define RealEstateManagerClient__MAP_BUCKETS_OFS                    0x04
#define RealEstateManagerClient__MAP_BUCKETCOUNT_OFS                0x08
//                                  ..._MAP_ELEMENT_COUNT_OFS       0x0C  (defined earlier)
//
// ---------------------------------------------------------------------------
// UpdateOccupancy (0x005B0A10) IN FULL -- runs once per second off the pulse:
// ---------------------------------------------------------------------------
//   tick = GetTickCount();                                    // 0x809810
//   if (this[0xA4]) { if (tick >= this[0xA8] + 5000) Retry(); }   // 0x5ADB60
//
//   if (this[0x0C] <= 0) {                    // *** NO RECORDS -> GIVE UP IMMEDIATELY ***
//       this[0x94] = this[0x8C];
//       this[0x98] = this[0x9C] = this[0xA0] = -1;             // gate CLEARED
//       return;                                                // nothing downstream ever runs
//   }
//   if (tick < this[0x88] + 1000) return;                      // throttle 1/sec
//   this[0x88] = tick;
//
//   if (DAT_00DCEE30 != 0)                                     // [unidentified gate global]
//       this[0x94] = GetCurrentParentId(pinstLocalPlayer);     // 0x4B3040  <- PRIMARY SOURCE
//   if (this[0x94] == -1)
//       this[0x94] = this[0x8C];                               // <- +0x8C's ONLY job
//
//   sub_5AFFC0(this);    // map-lookup this[0x94]; miss => this[0x9C] = -1
//                        // hit  => DefinitionTypeOf(rec[0x20]) (0x7F4FA0) -> test 0x7F60F0
//   sub_5B00E0(this);    // map-lookup this[0x94] -> rec; FindChildAtPosition(rec, pos)
//                        //   (0x4D6790); hit => this[0x98] = result[0x14]; miss => -1
//   sub_5B0140(this);    // tail-called, same shape again
//
// And GetCurrentParentId (0x004B3040) is four instructions:
//       if (arg == 0) return Instance()[0x8C];       // no player -> fall back to root
//       arg += 0x284;                                // pinstLocalPlayer + 0x284 == POSITION
//       jmp ResolveCurrentFromWorld(&pos);           // 0x004B2F40
#define RealEstate__GetCurrentParentId_ARG   0x00DD2630   // == pinstLocalPlayer_x, NOT a manager
#define PlayerBase__POSITION_OFS             0x284        // vec3 fed to the world resolve
// NAME IS MISLEADING -- see 43.81. This manager holds 0x5C-byte EQAreaBase-derived
// objects (areas), NOT placed items. The furniture lives in EQPlacedItemManager
// (instance 0x005061E0). Left named as-is so existing references keep building.
#define pinstPlacedObjectManager_x           0x00DCEE30   // = area manager; PTR to the object -- see 43.24, 43.81
#define PlacedObjectManager__ARRAY_OFS       0x00        // array of object pointers
#define PlacedObjectManager__COUNT_OFS       0x04        // slot count (>=142, all null in test)
// (was labelled OccupancyGateGlobal_x: it is not a flag, it IS the collection scanned)
//
// ---------------------------------------------------------------------------
// WHAT THIS MEANS FOR THE SERVER -- TWO HARD GATES, AND ONE CLEAN INJECTION POINT
// ---------------------------------------------------------------------------
// GATE 1: this[0x0C] must be > 0. If the client holds ZERO real-estate records the function
//         bails at the top and re-clears the gate every single tick. Sending the 0x2691
//         record is therefore not optional -- and it must actually land in the hash map.
// GATE 2: the world path needs ResolveCurrentFromWorld to match the player's position against
//         a placed world object. Guild plots have no such objects authored at all, which is
//         why f09=4 was "necessary but not sufficient" in earlier probes.
//
// THE INJECTION POINT: +0x8C is read every tick and written by NOTHING after construction.
// That is exactly what a "this zone IS real estate N" override looks like -- it is the
// designed escape hatch for an interior where the whole zone is the estate and there is no
// yard geometry to stand on. Set it once and this[0x94] inherits it, the map lookup hits, and
// the chain resolves without any world objects at all.
//
// THIS ALSO EXPLAINS WHY WRITING +0x98 NEVER STICKS: UpdateOccupancy
// recomputes +0x98 from scratch once per second, so the write was erased within ~1000 ms
// every time. Every "still -1 after reboot" observation was that overwrite, not a failed
// write. WRITING +0x98 IS ALWAYS FUTILE. Write +0x8C instead; it is upstream of the recompute.
// ============================================================================

// ============================================================================
// 43.20  *** OCCUPANCY GATE OPENED -- PROVEN LIVE 2026-07-28 ***
// ============================================================================
// The prediction in 43.19 was tested in game with Cheat Engine and it held exactly.
//
// Setup: manager base 0x270C3DB0 (that session), one plot record loaded by clicking a
// lot square, player standing on the lot.
//     +0x0C  =  1            record count -- the 0x2691 record DID land in the hash map,
//                            so UpdateOccupancy was running the FULL path, not bailing.
//     +0x8C  = -1            fallback
//     +0x98  = -1            gate closed
//
// Action: wrote 9001 (the plot id) into +0x8C and nothing else. One second later:
//     +0x8C  = 9001
//     +0x98  = 9001          <-- CASCADED ON ITS OWN. GATE OPEN.
//
// The whole cascade, confirmed end to end:
//     +0x8C = 9001
//       -> ResolveCurrentFromWorld returns -1 (no placed world object under the player)
//       -> +0x94 = +0x8C = 9001                        (the fallback in UpdateOccupancy)
//       -> hash-map lookup of 9001 in [this+0x04] HITS  (needs the record; hence +0x0C > 0)
//       -> FindChildAtPosition (0x4D6790) returns the record
//       -> +0x98 = record[0x14] = 9001
//
// TWO THINGS THIS NAILS DOWN:
//   1. record + 0x14 IS THE REAL-ESTATE ID. +0x98 came back as exactly the id injected.
#define RealEstateRecord__ID_OFS                                    0x14
//   2. THE WORLD-OBJECT PATH IS OPTIONAL. Everything earlier assumed occupancy was anchored
//      to placed world objects (EQPlacedItemManager) and that guild plots were untestable
//      because no lot-square objects were ever authored. NOT SO -- the +0x8C fallback
//      bypasses the world scan entirely. Placed objects are one way in, not the only way.
//
// WHAT IS NOW UNBLOCKED: "you must be inside a plot" (eqstr 727, checked at 0x00747E50) was
// the gate greying out Manage / Move Out / Transfer on every owned-plot window. It reads
// +0x98. With +0x98 resolved those controls should be live.
//
// CONSEQUENCE. Nothing in eqgame.exe found so far writes +0x8C outside construction/reset,
// so no packet appeared able to set it (but see 43.29 -- 0x6FD0 does). Any external write must
// be re-asserted after every manager Reset (0x005B1CF0) and on zone-in, since every reset
// path stores -1. The native alternative is real placed world objects so
// ResolveCurrentFromWorld resolves on its own.
// DO NOT write +0x98 in either case: UpdateOccupancy recomputes it once per second (43.19).
// ============================================================================

// ============================================================================
// 43.21  C->S REAL-ESTATE OPCODES, CAPTURED LIVE  (2026-07-28)
// ============================================================================
// Captured with the occupancy gate forced open (43.20), which is the ONLY state in which the
// owned-plot window's buttons are clickable and therefore the only state that yields these.
// Both arrived as [Packet C->S] [OP_Unknown] -- stock EQEmu has no handler for either, which
// is exactly why clicking Sell/Yes changed nothing server-side. The client did its part.
//
// ---------------------------------------------------------------------------
// 0x792C -- MOVE OUT / SELL request.  Fixed 190 bytes.
// ---------------------------------------------------------------------------
//   +0x00  dword   real_estate_id      observed 0x00002329 = 9001, the plot id
//   +0x04  dword   0xFFFFFFFF
//   +0x08  dword   0xFFFFFFFF
//   +0x0C  178 bytes, all zero
// Client flow: button -> local yes/no confirm ("the placed moving crate will be packed with
// all of the plot's items and added to your inventory ... money in escrow withdrawn") -> send.
// The confirm is CLIENT-SIDE ONLY; the server sees nothing until Yes is pressed.
#define OP_RealEstate_MoveOut                                       0x792C  // C->S, 190 bytes
#define OP_RealEstate_MoveOut__SIZE                                 190
//
// HYPOTHESIS, NOT YET TESTED: the two -1 dwords plus a 178-byte zero tail look like an unused
// target field, so TRANSFER is likely the SAME opcode with a target name/id filled in rather
// than a separate one -- one "dispose of this plot" struct reused. Verify before relying on it.
//
// ---------------------------------------------------------------------------
// 0x2FA0 -- sent when the Real Estate MANAGEMENT window opens.  Fixed 338 bytes.
// ---------------------------------------------------------------------------
//   +0x00  dword   0
//   +0x04  dword   1
//   +0x08  330 bytes, all zero
// Almost certainly a request for the data the window has no values for yet: Escrow contents,
// the Access list, and Price/Upkeep/Time Left. Note it carries NO real_estate_id, so the
// server is expected to answer for "the plot the player is currently in" -- i.e. the reply
// depends on server-side occupancy state, not on anything in the request.
#define OP_RealEstate_ManageWndOpen                                 0x2FA0  // C->S, 338 bytes
#define OP_RealEstate_ManageWndOpen__SIZE                           338
//
// NOT YET CAPTURED (all reachable only with the gate open): Items, Transfer, the Access tab,
// Add Day, Add Week, Withdraw All, Vote, and the Price / Upkeep dropdowns.
//
// STILL UNPOPULATED IN THE UI, and now attributable: Price, Upkeep and "Time Left: Unlimited"
// are all blank/default when cost_count = 0 is sent in the 0x2698 definition. That block
// (reader 0x007FC2B0) is what fills those three fields.
// ============================================================================

// ---------------------------------------------------------------------------
// 43.21b  SECOND CAPTURE PASS -- THE OTHER BUTTONS SEND NOTHING  (2026-07-28)
// ---------------------------------------------------------------------------
// WITH LOGGING ACTUALLY ON, clicking Items / Transfer / Access tab / Add Day / Add Week /
// Withdraw All / Vote produced NO NEW OPCODES AT ALL. Three 0x2FA0 and three 0x792C, and
// every instance of each was BYTE-IDENTICAL to the others:
//       0x2FA0  x3   only non-zero byte: +4 = 0x01          (i.e. Manage opened 3 times)
//       0x792C  x3   only non-zero: +0 = 9001, +4..+11 = -1 (i.e. Move Out clicked 3 times)
//
// RETRACTED: 43.21 guessed Transfer would reuse 0x792C with the target filled into the -1s
// and the zero tail. It does not -- Transfer emits nothing whatsoever.
//
// WHAT THAT MEANS. Those controls are not bound to opcodes still
// to be found; they are INERT BECAUSE THE CLIENT HAS NO STATE TO ACT ON. Escrow has no
// balance, Access has no list, Items has no contents, Price/Upkeep/Time Left are blank -- all
// of it is server-supplied data. (See the 43.37 retraction: 0x2FA0 is not the request.)
//
//   *** CRITICAL PATH: ANSWER 0x2FA0. ***
//   It is the window's "send me the management data" request. Everything else in that window
//   stays dead until it is answered, no matter how open the occupancy gate is. This is ONE
//   reply to decode and implement, not seven opcodes to hunt.
//   Note again it carries NO real_estate_id -- the server must answer for "the plot the
//   player is currently in", so server-side occupancy tracking is a prerequisite.
//
// ALSO SEEN, UNATTRIBUTED: OP_PopupResponse (0x08A6, 10 bytes) two seconds after the third
// Move Out -- +0 = 0, then dword 0x77353501. S->C was not being logged, so what popup it
// answers is unknown. Do not assume it belongs to the Move Out confirm; that dialog is
// client-side and fires 0x792C directly on Yes.
// ---------------------------------------------------------------------------

// ============================================================================
// 43.22  MANAGER LAYOUT FROM A LIVE DISSECT + THE CLIENT'S OWN DUMP FUNCTION
//        (2026-07-28, Cheat Engine "Dissect Data/Structures")
// ============================================================================
// Dissecting the live manager confirmed the ctor analysis independently: +0x00 held
// runtime 0x00D3910C, which rebases (delta 0x360000) to static 0x009D910C -- EXACTLY the
// vftable the constructor at 0x005B2405 installs. Same for 0x9D9108 / 0x9D90F0.
//
// FULL LAYOUT, observed with one plot record + one definition loaded and the gate open:
//   +0x00  vftable 0x009D910C          +0x80  vftable 0x009D9108
//   +0x04  hash map: records           +0x84  vftable 0x009D90F0
//   ...    (six more maps, below)      +0x88  last update tick   (0x002F747A)
//   +0x74  ptr                         +0x8C  root/override id   = 9001 (injected)
//   +0x78  ptr                         +0x90  current type       = 10 (the "none" sentinel)
//   +0x7C  ptr                         +0x94  current parent id  = 9001
//                                      +0x98  THE GATE           = 9001
//                                      +0x9C  = -1     +0xA0 = -1
//                                      +0xA4  pending flag = 0   +0xA8 pending tick = 0
//                                      +0xAC  0        +0xB0 ptr   => SIZE 0xB4 CONFIRMED
//
// The members are SEVEN hash maps of identical shape {buckets, bucketCount, elementCount, ?},
// every bucketCount a prime -- which is how to recognise them on sight:
//   +0x04  20011 buckets, count 1   <- REAL ESTATE RECORDS (plot 9001)
//   +0x14     11 buckets, count 0
//   +0x24     11 buckets, count 1
//   +0x34   5003 buckets, count 1   <- [inferred] DEFINITIONS (definition 100)
//   +0x44    503 buckets, count 0
//   +0x54  10007 buckets, count 0
//   +0x64  10007 buckets, count 0
// Node layout (from 0x5AFFC0 / 0x5B00E0): {+0x00 value, +0x04 key, +0x08 next}.
//
// VTABLE at 0x009D910C, 11 entries -- the manager's virtual methods:
//   0x005AA520 0x005AE1C0 0x005B5A10 0x005B1DA0 0x005B1E40 0x005B71A0
//   0x005B1EE0 0x005B5FF0 0x005B75F0 0x005B5A90 0x005B7C70
//
// ---------------------------------------------------------------------------
// *** THE CLIENT SHIPS ITS OWN REAL-ESTATE STATE DUMPER. USE IT. ***
// ---------------------------------------------------------------------------
// Immediately after the vtable sits a string table (found by dissecting past it), and its
// xrefs lead to two __thiscall dump routines on the manager:
#define RealEstateManagerClient__DumpToFile_x                       0x005B37D0
#define RealEstateManagerClient__DumpToFile2_x                      0x005B3B20
//   void __thiscall DumpToFile(const char *filename);   // arg at [esp+0x258] in-frame
//   filename null or "" -> builds "%s-RealEstate.txt" (0x009D91C0) from the name at
//   [0x00DD261C] (+0x44, [inferred] local character), fopen mode "w", then fprintf's rows.
//
// The string table IS THE ITEMS-WINDOW SCHEMA -- recovered without ever being able to click
// the Items button, because that button is inert until the server answers 0x2FA0 (43.21b):
//   0x009D9138  "this real estate"     <- scope filter
//   0x009D9168  "Neighborhood"         <- scope filter
//   0x009D914C  "Stored"   0x009D9154 "Placed"   0x009D915C "Inventory"   <- Status values
//   0x009D9178  "RealEstateLocation"   0x009D918B "RealEstateName"
//   0x009D919A  "ItemName"             0x009D91A3 "ItemOwner"
//   0x009D91AD  "Status"               0x009D91B7 "Count"
//   0x009D91D4  "%x %s"                <- row format
//
// WHY THIS MATTERS MORE THAN IT LOOKS. Every conclusion in 43.13-43.21 was inferred from
// indirect evidence -- a greyed button, a refusal string, a field that would not change.
// This function makes the client STATE ITSELF: call it in-process and read what the
// client actually holds (records, definitions, items, occupancy) instead of guessing whether
// a packet landed. Do this BEFORE the next round of protocol work; it turns most open
// questions into a file read.
// ============================================================================

// ============================================================================
// 43.23  RECORD STRUCTURE FROM A BREAK-AND-TRACE OFF THE GATE WRITE (2026-07-28)
// ============================================================================
// CE break-and-trace anchored on 0x005B0126 (`mov [esi+0x98], ecx` -- the successful gate
// write inside ResolveCurrentChild). 16k instructions = four passes of the 1/sec update.
// NOTE: CE writes the trace as UTF-16; `grep -c` reports ~2M "lines" for a 16k-line file.
//
// Post-gate call chain, identical on every pass:
//       0x007F62C0 / 0x007F62B0   type-mask helpers
//       0x004D6790                FindChildAtPosition  -- walks UP the parent chain
//       0x005B45B0                <-- the new one, below
//
// 0x005B45B0 is a RECURSIVE DESCENDANT SEARCH -- the mirror image of 0x4D6790:
//       FindDescendantOfType(this, rec, typeMask, ?, depth):
//         if (!rec) return 0;
//         if (flag <= 0) {
//             type = DefinitionTypeOf(rec[0x20]);        // 0x7F4FA0
//             if (typeMask & TypeToBitmask(type))        // 0x7F5E30 == 1 << (type-1)
//                 return rec;                            // MATCH
//         }
//         if (depth >= 10) return 0;                     // hard depth cap of 10
//         for (i = 0; i < rec[0x48]; i++)                // child COUNT
//             if (r = recurse(map_lookup(rec[0x4C][i]))) // child ID ARRAY
//                 return r;
//         return 0;
#define RealEstate__FindDescendantOfType_x                          0x005B45B0
//
// *** RECORD LAYOUT, as used by both walkers ***
//   +0x14  real_estate_id      (confirmed 43.20 -- the gate takes its value from here)
//   +0x1C  PARENT real-estate id                      (wire field `a`)
//   +0x20  definition id                              (wire field `b`)
//   +0x48  child count
//   +0x4C  pointer to child real-estate id array
#define RealEstateRecord__PARENT_ID_OFS                             0x1C
#define RealEstateRecord__DEFINITION_ID_OFS                         0x20
#define RealEstateRecord__CHILD_COUNT_OFS                           0x48
#define RealEstateRecord__CHILD_IDS_OFS                             0x4C
//
// WHY THIS MATTERS: +0x48/+0x4C are populated from the trailing ids[] array of the 0x2698
// definition record. A plot sent with id_count = 0 has zero children and no parent: an
// orphan. Occupancy still resolved in
// 43.20 only because the +0x8C override put the player directly ON the plot record, so
// neither walker had to traverse anything.
//
// A real neighborhood needs the hierarchy: send the NEIGHBORHOOD record as the parent with
// its plot ids in ids[], and each plot's `a` (+0x1C) pointing back at it. Depth cap is 10,
// so nesting deeper than neighborhood -> plot -> interior -> item is not possible.
// ============================================================================

// ============================================================================
// 43.24  THE WORLD PATH IS DEAD BECAUSE THE PLACED-OBJECT ARRAY IS EMPTY
//        (2026-07-28, CE break-and-trace on 0x004B2F4A)
// ============================================================================
// ResolveCurrentFromWorld, prologue -- and it corrects 43.19's reading of the arguments:
//   0x004B2F40  push ecx/ebx/ebp
//   0x004B2F43  mov ebp, ecx            ; ecx is THIS = the COLLECTION, not a position
//   0x004B2F45  call Instance()         ; 0x5B3540
//   0x004B2F4A  mov ebx, [eax+0x8C]     ; DEFAULT RETURN VALUE = the +0x8C override
//   0x004B2F50  mov edx, [ebp+0x04]     ; count
//   0x004B2F5B  jle -> return ebx       ; count <= 0 -> hand back +0x8C unchanged
//   0x004B2F61  mov ecx, [ebp+0x00]     ; array of object pointers
//   loop: edi = *ecx; if (edi) goto 0x4B2F7C; ecx += 4; ...
//   0x004B2F7C: esi = edi[0]; if (esi) { containment test 0x8D2AD0(esi, pos) ... }
//
// So the call in UpdateOccupancy is:  ecx = [0x00DCEE30], stack arg = pinstLocalPlayer,
// and GetCurrentParentId rewrites that arg to localPlayer + 0x284 (the position, which IS
// what reaches the containment test at [esp+0x18]) before tail-jumping here.
// => 0x00DCEE30 IS A POINTER TO THE PLACED-OBJECT MANAGER, NOT a gate flag and NOT the
//    pair itself: the {array_ptr, count} lives INSIDE that object at +0x00 / +0x04.
//    Only two writers exist (0x00532757, 0x0053629C), both construction-time -- so the
//    collection is created once and populated (or not) through its own methods.
//    This is the [inferred] "EQPlacedItemManager" flagged as needing a spike since 43.13.
//
// THE TRACE, and it is unambiguous. CE caps break-and-trace at 1000 instructions; those
// 1000 covered 142 full iterations of the null-check loop, and the non-null branch
// (0x004B2F7C) was hit EXACTLY ZERO TIMES:
//       0x004B2F65 mov edi,[ecx]   x142     <- read slot
//       0x004B2F67 test edi,edi    x142
//       0x004B2F69 jne 0x4B2F7C    x142     <- NEVER TAKEN
//       0x004B2F6B inc eax         x142     <- always falls through to the next slot
// The array is at least 142 entries and every entry is NULL.
//
// CONCLUSION, now proven rather than inferred: the native occupancy path is not subtly
// misconfigured, not gated on a flag, and not rejecting the data. It scans a fixed-size
// array of placed world objects that IS SIMPLY EMPTY, falls out of the loop, and returns
// mgr[0x8C] unchanged -- which is exactly why forcing +0x8C worked (43.20) and why nothing
// else ever did. Note the function's DEFAULT RETURN IS +0x8C, so the "fallback" in
// UpdateOccupancy is really a second layer of the same override.
//
// TO MAKE THE NATIVE PATH WORK, the collection at 0x00DCEE30 has to be populated with placed
// objects. No known message created one at this point. That is the remaining spike, and it is now a
// bounded question -- find what pushes entries into {0x00DCEE30}[array] -- rather than the
// open-ended "how does occupancy work" it has been since 43.13.
// ============================================================================

// ============================================================================
// 43.25  0x6573 -- OPT OUT TOGGLE (captured 2026-07-28)
// ============================================================================
// Third C->S real-estate opcode, and the first whose payload is fully understood.
// Captured with the gate forced open; two sends 3 seconds apart, identical except one byte.
//   0x6573, 8-byte body:
//       +0x00  dword  real_estate_id     observed 0x00002329 = 9001
//       +0x04  byte   state              1 then 0  -- a checkbox ticked, then unticked
//       +0x05  D1 FF 01                  constant across both sends; [inferred] struct
//                                        padding, do NOT treat as meaningful without proof
#define OP_RealEstate_OptOut                                        0x6573  // C->S, 8-byte body
#define OP_RealEstate_OptOut__SIZE                                  8
//
// NOTE ON PACKET SIZES IN THE ZONE LOG: EQEmu prints "Size [N]" INCLUDING the 2-byte opcode,
// so the body is N-2. Consistent across all three captures (10/8, 190/188, 338/336).
//
// A 1-then-0 pair identifies this as the Opt Out CHECKBOX, not the Vote button. VOTE SENT
// NOTHING -- same as Items / Transfer / Access / escrow (43.21b): with the gate open every
// control is now CLICKABLE, but anything needing server-supplied state stays inert until
// 0x2FA0 is answered. Clickable != functional, and that distinction is what 43.21b missed.
// ============================================================================

// ============================================================================
// 43.26  HOW THE PLACED-OBJECT COLLECTION IS POPULATED -- THE NATIVE PATH
//        (2026-07-28)
// ============================================================================
// Context: the question is how the client populates the collection at
// 0x00DCEE30 that ResolveCurrentFromWorld scans (43.24).
//
// THE COLLECTION IS A HASH MAP, not a flat array. Same node shape as the manager's maps:
//   [ptr+0x00] bucket array   [ptr+0x04] bucket count   node = {value, key, next}
// ResolveCurrentFromWorld walks EVERY bucket and chain:
//   obj = node[0];
//   if (ContainmentTest(obj, playerPos))                  // 0x008D2AD0
//       est = FindEstateForObject(mgr, mgr[0x8C], obj[4]); // 0x004B30F0
//       if (TypeTest(DefinitionTypeOf(est[0x20])))         // 0x7F4FA0 -> 0x7F6030, types 4/8
//           return est;
//   node = node[8];                                        // chain, then next bucket
// => EVERY PLACED OBJECT CARRIES ITS ESTATE KEY AT +0x04 and a containment volume.
#define PlacedObject__ESTATE_KEY_OFS                                0x04
#define PlacedObjectManager__Add_x                                  0x004B2BF0  // builds a 4x4
                                                    // identity transform + position, inserts
#define PlacedObjectManager__AddBool_x                              0x004B2F20  // thin wrapper
//
// WHO CALLS Add: two sites, 0x004B2F2F (the wrapper) and 0x005AE07F. And 0x005AE010 -- the
// function containing that second site -- HAS NO DIRECT CALLERS. It is virtual:
//       0x005AE010 lives at .rdata 0x009D90F4 == vftable[1] of 0x009D90F0,
//       and 0x009D90F0 is one of the THREE vftables the RealEstateManagerClient ctor
//       (0x005B2405) installs -- the interface at object + 0x84.
// So RealEstateManagerClient IMPLEMENTS A LISTENER INTERFACE and this is its callback.
#define RealEstateManagerClient__IFACE_84_VFTABLE                   0x009D90F0
#define RealEstateManagerClient__IFACE_80_VFTABLE                   0x009D9108
#define RealEstateManagerClient__OnItemActorReady_x                 0x005AE010  // iface[1]
//   iface@+0x84: [0]0x5B7F90 [1]0x5AE010 [2]0x5B1F90 [3]0x5D7540
//                [4]0x5AA530 [5]0x5AE160 [6]0x5ADE90 [7]0x5AA520
//
// WHAT 0x005AE010 DOES -- and this is the whole answer:
//       actor = arg[0x38];  if (!actor) return;      // the arg OWNS A SPAWNED ACTOR
//       if (![0x00DCEE30]) return;
//       actor->vt[0x80](&outA);                      // position   [inferred]
//       actor->vt[0x84](&outB);                      // bounds     [inferred]
//       result = PlacedObjectManager::Add(&outB, &outA, ...);   // 0x004B2BF0
//       *out = result;
//
//   *** THE COLLECTION IS POPULATED FROM REAL-ESTATE ITEMS THAT HAVE SPAWNED ACTORS. ***
//   Not from doors. Not from the 0x2691 plot record. Not from the 0x2698 definition.
//   Nothing the server currently sends creates an entry, which is exactly why the array was
//   142/142 empty in the trace (43.24) and why occupancy only ever resolved via the override.
//
// CONSEQUENCE FOR THE SERVER: the native path needs the ITEM/LAYOUT message -- the placed
// objects for an estate -- the data model captured in a live client's
// RE_Layout_<id>.xml dump (43.5). That message
// is one of the 30 recovered opcodes and its format is NOT yet decoded. Decoding it is the
// remaining work for a stock-client implementation.
//
// OPEN QUESTION THAT DECIDES THE DATA MODEL, and it needs answering before any of this is
// built: an EMPTY plot has no placed items, so what does a player stand on to buy one?
// Two possibilities:
//   (a) The lot square itself is a real-estate placed object supplied by the neighborhood
//       layout. PEQ imported the RoF2 lot grid as DOORS (opentype 160, 43.x) because that is
//       what the .eqg gave it -- but live EQ may have served them as placed objects, in which
//       case the server must send a lot-square item per plot and occupancy follows for free.
//   (b) Buying is gated on something other than occupancy and only OWNED plots ever resolve.
// (a) is far more likely and fits every observation so far, but it is UNVERIFIED -- do not
// build on it until the item message is decoded and one lot square is seen registering.
// ============================================================================

// ============================================================================
// 43.27  THE S->C DISPATCH TABLE + THE 'RealEstate' CLASS  (2026-07-28)
// ============================================================================
// HandleRealEstateMessage (0x005B2500) dispatches by opcode through a binary-search compare
// chain. Extracted in full -- 18 entries, of which 3 were already known and 12 are NEW.
// This is the map of the S->C side that has been missing since 43.7.
//
//   opcode  handler       calls (first few)                what it looks like
//   0x0004  0x005B294D    5B7A90, 5B59A0, 5B78D0, 7465F0   record add + Manage-wnd refresh
//   0x087F  0x005B25D1    5B0E30, 5AFE80, 5AFD50
//   0x1097  0x005B27EE    5AFE00, 58C9A0, 5B4060
//   0x1228  0x005B263E    5AE1F0, 5B0D00                   OP_RealEstateResult (known)
//   0x1967  0x005B262C    5AFD50, 5AE1F0, 5B0D00
//   0x1CEF  0x005B2654    5B0D00
//   0x2698  0x005B278C    7F5B60, 7FC6B0, 5AAAD0           definitions catalog (known)
//   0x3968  0x005B28EB    8DBB3B, 5B5250, 5B5830           record add (known, [inferred])
//   0x485D  0x005B2ABD    8DBB3B, 5B5250, 5B5830, 4048B0   same shape as 0x3968
//   0x4F09  0x005B2D18    8DBB3B(0x58), 7FE100             *** constructs a RealEstate ***
//   0x58C7  0x005B2C7A    5559A0, 404630, 875BD0
//   0x5A63  0x005B2BBA    805C20, 466030, 805D90, 8061C0   string-heavy
//   0x63FD  0x005B2DD8    8DBB3B, 7FE100, 466030           *** constructs a RealEstate ***
//   0x6FD0  0x005B3054    8DBB3B, 7FE100, 7F4FA0           *** constructs a RealEstate ***
//   0x76C6  0x005B30F4    58C9A0, 58C9A0, 7C9AA0
//   0x0351 / 0x0C13 / 0x7D28 -> fall through to the default at 0x005B3150 (no-op)
// NOTE 0x2691 (search) is NOT in this table -- it is handled on its own path (43.12/43.13).
#define RealEstate__Ctor_x                                          0x007FE100
#define RealEstate__SIZE                                            0x58
#define RealEstate__VFTABLE                                         0x00A1052C
//
// *** 0x007FE100 IS THE CONSTRUCTOR FOR THE CLASS LITERALLY NAMED 'RealEstate'. ***
// It installs vftable 0x00A1052C, and 0x00A1052C is the ONE game class the RTTI sweep
// (43.18) resolved by name: '.?AVRealEstate@@'. Independent confirmation from two
// completely different directions -- an RTTI type-descriptor walk and a message handler.
//
// What the ctor initialises (operator new size 0x58):
//       [+0x00] = 0x00A1052C            vftable
//       [+0x08] = node, self-linked (n->prev = n->next = n), [+0x20]=[+0x21]=1
//       [+0x0C] = 0
//       [+0x24] = -1
//       [+0x14..+0x40] = 0, [+0x44] = 0 (byte), [+0x54] = 1 (byte), [+0x48..+0x50] = 0
//       [+0x38] = 0   <<< THE ACTOR SLOT
#define RealEstate__ACTOR_OFS                                       0x38
//
// +0x38 IS EXACTLY THE FIELD RealEstateManagerClient::OnItemActorReady (0x005AE010, 43.26)
// DEREFERENCES before calling PlacedObjectManager::Add. So the chain is now complete:
//
//    server sends 0x4F09 / 0x63FD / 0x6FD0
//        -> handler news a 0x58-byte RealEstate and runs 0x007FE100
//        -> the object acquires an ACTOR at +0x38
//        -> iface[1] 0x005AE010 reads its position+bounds and inserts it into the
//           placed-object map at [0x00DCEE30]
//        -> ResolveCurrentFromWorld finally has something to hit (43.24)
//        -> occupancy resolves with NO +0x8C override.
//
// THIS SUPPORTS HYPOTHESIS (a) IN 43.26: a lot square is world-present real estate, so an
// empty plot CAN resolve natively -- it does not depend on furniture being placed on it.
// Still not proven: which of the three opcodes carries it, and the wire format of each.
// NEXT: decode 0x4F09 / 0x63FD / 0x6FD0 payloads (their handlers continue past the ctor into
// the field reads) and identify which one the live server used for lot squares.
// ============================================================================

// ============================================================================
// 43.28  RealEstate::Deserialize -- THE RECORD WIRE FORMAT  (0x007FDF00)
//        + A CORRECTION TO 43.26/43.27
// ============================================================================
// *** CORRECTION FIRST. 43.27 claimed RealEstate+0x38 is 'THE ACTOR SLOT ... exactly the
// field 0x005AE010 dereferences'. THAT IS WRONG. This deserializer reads a STRING into
// +0x38 via 0x0080C9B0 -- the same length-prefixed string reader used for +0x2C and +0x30.
// The +0x38 in 0x005AE010 belongs to a DIFFERENT CLASS: its argument holds an object there
// with virtual getters at vt+0x80/+0x84, which a string does not have. Two offsets collided
// and the match was reported as confirmation. It was a coincidence.
// What survives from 43.26 is only the OUTLINE: some object owning an actor is registered
// into the placed-object map by iface[1]. WHICH class that is remains UNIDENTIFIED.
// The manager's map holds RealEstate records; the collection at 0x00DCEE30 holds placed
// objects whose +0x04 is an estate key (43.24). They are NOT the same class -- do not merge
// them again.
//
// ---------------------------------------------------------------------------
// THE WIRE FORMAT (this is what 0x4F09 / 0x63FD / 0x6FD0 each carry, one record)
// ---------------------------------------------------------------------------
// Reader idiom throughout:  if (cursor+N <= len) { field = read; cursor += N; }  -- the
// FAIL-OPEN truncation pattern (43.x): a short buffer silently leaves the field at whatever
// it was and keeps parsing. Under-sending does NOT error, it desyncs.
//
//   dword   -> +0x14   real_estate_id      (confirmed: 43.20 took the gate value from +0x14)
//   dword   -> +0x18   ?
//   dword   -> +0x1C   PARENT real-estate id            (wire field 'a', 43.23)
//   dword   -> +0x20   DEFINITION id                    (wire field 'b', 43.23)
//   dword   -> +0x24   ?   (ctor seeds this one to -1)
//   dword   -> +0x28   ?
//   string  -> +0x30   length-prefixed (0x0080C9B0)
//   string  -> +0x2C   NOTE THE ORDER: +0x30 is read BEFORE +0x2C
//   dword   -> +0x34   ?
//   dword            child_count  (n)  -- read into a local, NOT stored directly
//   string  -> +0x38
//   n x dword        appended to the child id array: count at +0x48, array at +0x4C (43.23)
//   byte             flag; if non-zero a further block follows (dword, then more)
//
// THIS INDEPENDENTLY CONFIRMS 43.23, which was derived from the recursive descendant search
// rather than from any parser: +0x1C parent, +0x20 definition, +0x48 count, +0x4C child ids.
// Two unrelated derivations agreeing is the strongest evidence in this section.
#define RealEstate__Deserialize_x                                   0x007FDF00
#define RealEstate__Vtbl_Deserialize_OFS                            0x0C
//
// NEXT: the three opcodes differ in what they do AFTER the shared deserialize, so decoding
// 0x005B2D18 / 0x005B2DD8 / 0x005B3054 past the ctor is what separates 'add a record' from
// 'add a world-present plot'. Only one of them is likely to produce a placed object.
// ============================================================================

// ============================================================================
// 43.28b  RECORD TAIL COMPLETED -- the escrow balances ride IN the record  (2026-08-01)
// ============================================================================
// 43.28 stopped at "byte flag; if non-zero a further block follows (dword, then more)".
// Decoded that tail fully (runtime 0x7DDF00 = file 0x007FDF00; base 0x3E0000, rt=file-0x20000;
// string reader 0x7EC9B0, map-insert helper 0x7DDD40). The "flag" is actually a COUNT byte:
//
//   byte  N                                   <- 0x7DE054 : movzx eax,al; count = N
//   N x { u32 key;  f64 value }               <- 0x7DE061 loop: read u32 -> key, read qword(double)
//         inserted into the map at RECORD+0x04 (0x7DE08E: lea ecx,[edi+04]; call 0x7DDD40)
//     == THE PER-CURRENCY ESCROW BALANCES. Value is a DOUBLE -- exactly what the Escrow-tab
//        Quantity column formats (row-setup 0x741930 reads a double at map-entry+0x18). So the
//        left "Quantity" list is fed BY THE ESTATE RECORD, coin-free -- NOT by a standalone
//        S->C 0x2AA6. Currency key set: coin most
//        likely 0cp/1sp/2gp/3pp (matches the deposit coin_type); alt currencies own ids.
//   dword -> +0x3C   UPKEEP COST-SET ID       <- 0x7DE0B4  (IDENTIFIED 43.28g: cost-set id, NOT a
//                                                 raw cost. Client resolves it via FUN_007FB8D0 to
//                                                 get the per-currency daily upkeep -> Time Left.)
//   dword -> +0x40                            <- 0x7DE0CB  still unidentified (price cost-set id? / paid-until?)
//   byte  -> +0x44                            <- 0x7DE0E2  still unidentified (a flag; read by 0x2AA6 sender)
//     +0x3C now decoded (43.28g). +0x40/+0x44 still need a live capture before sending
//     (need a live capture before relying on them). ret 4.
//
// NOTE: a record that ends with escrow N=0 and nothing after it leaves +0x3C/+0x40/+0x44 at
// client defaults (fail-open reader) -> plot reads "Time Left: Unlimited" and the escrow list
// stays 0. Emitting the escrow {key->double} map (N>0) plus +0x3C/+0x40/+0x44 lets
// 0x6FD0/0x63FD paint the balance and the upkeep/time line.
// ============================================================================

// ============================================================================
// 43.28g  RealEstate::Deserialize -- COMPLETE FIELD ORDER + cost-set id  (2026-08-01, overnight)
// ============================================================================
// Full read order of FUN_007FDF00 (file 0x007FDF00 / runtime 0x007DDF00). Cursor is a
// {base,len,off} struct; each read is bounds-checked then advances off. In order:
//   record+0x14  dword                          (re_id / plot id)
//   record+0x18  dword
//   record+0x1C  dword
//   record+0x20  dword   DEFINITION ID          (confirmed: FUN_007F4F70(record+0x20) -> definition)
//   record+0x24  dword
//   record+0x28  dword
//   record+0x2C  container init (FUN_0080C9B0)  (CXStr -- name or owner string)
//   record+0x30  container init (FUN_0080C9B0)  (CXStr -- the other string)
//   record+0x34  dword
//   dword itemCount ; record+0x38 container init ; itemCount x { dword } appended to the
//                                                  placed-item id array at record+0x48(count)/+0x4C(base)
//   byte escrowN ; escrowN x { dword key ; qword f64 value } -> escrow std::map (record+0x04..+0x0C)
//   record+0x3C  dword   UPKEEP COST-SET ID      (43.28e/f: resolved to daily_cost -> Time Left)
//   record+0x40  dword   (unidentified)
//   record+0x44  byte    (unidentified flag)     ; ret 4
//
// -- the cost-set accessors record+0x3C flows through --
#define RealEstate__FindCostSetById_x                              0x007FB8D0  // (costContainer this, id): linear search, entries stride 0x14, key at +0, match -> entry
#define RealEstate__ReadCostLeaf_x                                 0x007FB7D0  // (costEntry, idx, &key,&sub,&val64): leaf stride 0x10 = {u32 key, u32 sub, u64 amount}
//   So a cost-set = { +4 count, +8 entries[] (stride 0x14) }; each entry -> leaves (stride 0x10).
//   RefreshUpkeep sums every leaf into Balance_List row+0x08 (daily_cost, 64-bit) by (key,sub).
//   The per-estate cost container is fetched from GetManager()+0x78 hash by estate id (0x006AEDD0),
//   and only exists once the estate's 0x2698 definitions/cost-blocks (43.31/43.36) have loaded.
//
// -- the MANAGE window WndNotification + escrow send mechanism (43.28h) --
//   (RealEstateManageWnd__WndNotification_x = 0x745090 is already #defined far above; this
//    ANALYSIS confirms that address independently -- its member offsets match RefreshUpkeep.)
//   __thiscall(REMW* this, ctrl* clicked, int notif, int* data). Offsets MATCH RefreshUpkeep
//   (this = the Manage window): +0x238 Upkeep_List, +0x2FC Upkeep_Combobox, +0x300/+0x308/+0x30C
//   labels, +0x314 Upkeep CListWnd, +0x260/+0x264/+0x268 access-view btns, +0x270/+0x274/+0x278
//   the three Access subwindows, +0x288/+0x28C Name edit, +0x290/+0x294 add/remove-access btns.
//   ESCROW SEND: this+0x340 = pending escrow ACTION code (set when Add Day/Add Week/Withdraw All is
//   clicked). On the amount-dialog CONFIRM (notif == 0x24) it does:
//        if (this+0x340 != -1) { FUN_00741EE0(this+0x340 /*action*/, *data /*amount*/);
//                                this+0x340 = -1; }
//   Selecting the Upkeep_Combobox (this+0x2FC) calls FUN_00741EE0(action, 0xFFFFFFFF) = recompute
//   only (the out-of-int-range branch in the builder sends nothing).
//   So the escrow packet is built by FUN_00741EE0 (0x00721EE0 rt), body from its send path:
//     { dword action ; dword currencyIdx ; dword amount_lo ; dword amount_hi ; dword estate_id ; ... }
//   sent as C->S 0x2AA6 (0x8C-byte buffer).  ESCROW ACTION CODES -- CONFIRMED live from the zone log
//   2026-08-01 (server received each button, 23-byte bodies, no amount field):
//     0 = DEPOSIT (39-byte body, coin_type@0x1B + amount@0x1F)
//     1 = WITHDRAW ONE CURRENCY (39-byte body, SAME coin fields; the escrow-list Quantity popup ->
//         Accept; amount is in the clicked denomination, e.g. Platinum click -> coin_type 3)
//     3 = ADD DAY    4 = ADD WEEK    7 = WITHDRAW ALL    (2 = an upkeep-combo/sell path, see 0x74D500)
//   REMW button member -> action (read directly from the WndNotification decompile; the ones tied to a
//   confirmed action code):
//     this+0x2FC  Upkeep_Combobox   -> action 2 (set upkeep plan)
//     this+0x31C  Add Day button    -> action 3   (matches the live log: Add Day = 3)
//     this+0x318  Withdraw All btn  -> action 7   (matches the live log: Withdraw All = 7)
//     this+0x2F8  Transfer button   -> C->S 0x1DD3 confirm dialog (not escrow)
//     this+0x310  Balance_List CListWnd (click -> recompute)   this+0x314 Upkeep_List CListWnd
//   (Add Week's button -> action 4 is confirmed by the live log but its member offset sits in the
//    convoluted list-selection branch of the decompile and is not cleanly pinned -- do not rely on a
//    guessed offset for it; the ACTION code 4 is what the server keys on and is certain.)
//   Reopening the Manage window sends NO escrow action, so 3 is uniquely Add Day (NOT a tab refresh
//   -- an earlier note said "3=refresh"; that was wrong). Add Day/Week carry no amount; the server
//   computes the charge from the daily upkeep cost.
//
// -- the PURCHASE window WndNotification (Buy/Sell/Transfer) --
//   (RealEstatePurchaseWnd__WndNotification_x = 0x74D500 is already #defined far above; matches the
//    43.38 correction. Kept here as the button->opcode dispatch reference.)
//   __thiscall(REWnd* this, ctrl* clicked, int notif, uint data). Different member layout than REMW.
//   Compares clicked vs member ptrs this[0x8A..0x9E] (i.e. +0x228..+0x278) and dispatches:
//     [0x93] +0x24C -> C->S 0x45BE (190B, buy/plot-update)   [0x8E] +0x238 -> C->S 0x2AA6 (escrow, 0x8C body)
//     [0x94] +0x250 -> sell-confirm dialog eqstr 0x1DD2      [0x97] +0x25C -> transfer-confirm eqstr 0x1DD3
//     [0x95] +0x254 -> open "items" window                   [0x96] +0x258 -> open "manage" window
//     [0x9C] +0x270 -> teleport-to-anchor                    [0x9E] +0x278 -> C->S 0x6573 (opt-out, 10B)
//   The 0x2AA6 body starts { dword action ; dword estate_id(GetManager+0x98) ; dword amount ; ... }.
//   Sell/withdraw path builds action=2. (Deposit-from-cursor uses FUN_00741EE0, below.)
#define RealEstateWnd__SendEscrowFromCursor_x                      0x00741EE0  // file addr; runtime 0x00721EE0
//   Reads pending coin amounts staged at DAT_00DD2660+0x33C8/+0x33CC/+0x33D0/+0x33D4
//   (=platinum/gold/silver/copper on the cursor) and builds the escrow deposit packet.
//   Escrow C->S action codes CONFIRMED live 2026-08-01: 0=deposit, 3=Add Day, 4=Add Week,
//   7=Withdraw All (2 = combo/sell path). See 43.28h.
// ============================================================================

// ============================================================================
// 43.28i  ITEM REAL-ESTATE MODEL: crate/house upkeep + placeable bitmask  (2026-08-01, overnight)
// ============================================================================
// Found via the item-tooltip builders (file addrs; runtime = file - 0x20000):
//
// -- per-item DAILY UPKEEP (the "This item has a daily upkeep cost of" line, eqstr 0x1D5F) --
#define ItemDisplay__RealEstateUpkeepLine_x                        0x006A0880  // file; rt 0x006A0880-0x20000
//   FUN_006A0880: id = item's real-estate id (FUN_007B0B80); looks it up in GetManager()+0x78 hash
//   (node+0x14 = key, +0x18 = next -- SAME hash as 0x006AEDD0 / RefreshUpkeep's per-estate cost obj).
//   If the cost object exists and has entries (+4 > 0), FUN_005AA760(&s, costObj, 0) formats the
//   amount and the line prints.  => a house/crate's upkeep is NOT an item field; it lives in the
//   client's per-realestate cost object (built from the 0x2698 definitions/cost-blocks, 43.31/36),
//   keyed by the item's real-estate id.  The PLOT's Manage-window daily cost (43.28e/f) uses the
//   very same hash + cost-set machinery, keyed by the plot's estate id.
#define RealEstate__GetManagerPlus78Hash_COMMENT                   0x78        // GetManager()+0x78 = hash{ realEstateId -> costObject }
//
// -- placeable-in bitmask (the "This item is placeable in ..." line, eqstr 0x190E) --
#define ItemDisplay__PlaceableInLine_x                             0x0069E740  // file; rt 0x0069E740-0x20000
//   FUN_0069E740: builds a placeable-location bitmask from the item def (FUN_007F62B0 | FUN_00410C30
//   | FUN_007F6290, optionally AND FUN_007B0B30 override), popcount FUN_005FFB30, then tests bits via
//   FUN_007B0AF0(bit) and appends the matching eqstr:
//        bit 0x01 -> 0x190F (neighborhoods)   bit 0x02 -> 0x1911 (guild halls)
//        bit 0x04 -> 0x1910 (houses)          bit 0x08 -> 0x1491 (yards)
//        bit 0x80 -> 0x02EC (guild yards)
//   item+0x88 = count / item+0x8C = array of a type-restriction list ("(of type: ..." if item+0x74
//   set, else "(but not in type: ..."), each id formatted via FUN_004866C0(id,0x1B,0).
//   So an item is a placeable house/crate/etc. iff its placeable bitmask is non-zero; the bit set
//   picks which real-estate kinds accept it.  This is the field the server sets to make an item a
//   "moving crate" or "house" (matches the eqstr housing vocabulary).
// ============================================================================

// ============================================================================
// 43.28j  THE COST-BLOCK WIRE FORMAT + how mgr+0x78 gets its cost data  (2026-08-01, overnight)
// ============================================================================
// This is the server-side recipe for a non-zero daily_cost (=> real Time Left instead of "Unlimited").
// The client reads cost data (in the 0x2698 definitions stream) via a 3-level reader chain and stores
// it in the GetManager()+0x78 hash of per-definition cost containers.  All file addrs (rt = file-0x20000).
#define RealEstate__ReadCostBlocks_x                               0x007FC6B0  // outer list
#define RealEstate__ReadCosts_x                                    0x007FC2B0  // one cost-set
#define RealEstate__ReadCostEntry_x                                0x007FBDB0  // one entry's leaves
//   Wire order (each field bounds-checked; cursor = {base,len,off}):
//     ReadCostBlocks : dword N ; N x <cost-set>
//     <cost-set>     : dword costSetId ; dword entryCount ; entryCount x <entry>     (ReadCosts)
//     <entry>        : dword entryKey  ; dword leafCount  ; leafCount x <leaf>       (ReadCostEntry)
//     <leaf>         : dword currencyKey ; dword sub ; u64 amount                    (stride 0x10)
//   In memory: cost-set entries are stride 0x14 {+0 key,+4 leafCount,+8 leafPtr,...}; leaves stride
//   0x10 (matches the accessors FUN_007FB8D0/970/9B0/7D0 in 43.28g).  PopulateCostCombo (0x0074D240)
//   and RefreshUpkeep both find their container in GetManager()+0x78 (hash: +4 buckets, +8 count,
//   node+0x14 key, +0x18 next) then pick a cost-set by id (record+0x3C for upkeep; combo selection for
//   the dropdown).  FUN_005AA760(&out, container, costSetIdx) formats one cost-set's amount string.
//
//   SO, to light up Daily Cost / Time Left server-side:
//     1. In the 0x2698 definition stream, send a cost-block whose costSetId matches what you'll put at
//        record+0x3C, containing >=1 leaf { currencyKey, sub, amount } (currencyKey 0 = coin; amount
//        in the currency's base unit -- coin = copper).
//     2. In the estate record, set record+0x20 = that definition id and record+0x3C = that upkeep
//        costSetId (plus the escrow balance map, 43.28c).
//   Then RefreshUpkeep computes days = balance/amount and paints a real Time Left.  CONFIRM the exact
//   0x2698 field order against a LIVE capture before serializing.
// ============================================================================

// ============================================================================
// 43.28k  ACCESS RECORD: per-slot meaning CONFIRMED + rebase correction  (2026-08-01, overnight)
// ============================================================================
// 43.46/43.107 left "the MEANING of the two dwords per slot is unconfirmed" and said the deserializer
// 0x005B7A90 "did not verify" (it disassembled as a cleanup loop). That was a REBASE ERROR: runtime =
// file - 0x20000, so 0x005B7A90 -> runtime 0x0059BA90 (they looked at 0x0091BA90). Decompiled correctly
// now; the 0x48-byte record is:
//   7 role slots x { dword roleId ; dword permBitmask } (= 0x38)  then  dword count ;
//   count x { length-prefixed name ; per-name permission data }   (individual overrides)
//   -- CONFIRMS the two dwords per slot: slot[i] = { roleId, permBitmask }.
//   * ctor 0x005B5250 seeds the 7 roleIds 0..6 (Owner,Co-Owner,Trustee,Friend,Acquaintance,Visitor,
//     Everyone -- 43.41) at slot+0; slot stride 8.
//   * 0x005B5830 packs the permBitmask: for perm 0..12 (THIRTEEN, 43.41) it sets bit
//     (record + 4 + (perm>>5)*4) from the role default FUN_005B3D50(role,perm). So permBitmask is a
//     13-bit mask, bit p = permission p granted to that role. This is the field the server fills.
//   * insert into manager: 0x005B7DE0 (keyed by the leading real_estate_id -- 43.107 confirmed the id
//     prefix is REQUIRED; without it the fail-open reader walks off the end and crashes).
//   * per-record SAVE C->S 0x3146 (sender 0x00747ED0): iterates the edited list at ManageWnd+0x2C8,
//     resolves each in the access hash at GetManager()+0x54/+0x58, serializes (0x00749420), one 0x3146
//     per record (200-byte buffer).  (Access hash is mgr+0x54; the COST hash is mgr+0x78, 43.28j.)
// ============================================================================

// ============================================================================
// 43.28m  CONSOLIDATED MAPS: REMW members + RealEstate record + RE manager  (2026-08-01 overnight)
// ============================================================================
// One place for every offset this overnight pass pinned. Cross-refs in 43.28e-l.
//
// RealEstateManageWnd (REMW) members (this = window ptr):
//   +0x228  staged dword (copied from DAT_00DD2660+0x154 on some actions)
//   +0x234  byte  "computed" flag (RefreshUpkeep clears on entry)
//   +0x238  int   Upkeep_List row count      +0x23C ptr rows (stride 0x18; row+0x08 u64 currency key,
//                                             row+0x10 char selected/enabled)
//   +0x248  int   Balance_List row count      +0x24C ptr rows (row+0x00 key, +0x08 u64 daily_cost,
//                                             +0x10 f64 days = balance/cost)
//   +0x260/+0x264/+0x268  access-view buttons (ViewIndividual / ViewGroupTotals / ViewOwnerClubs)
//   +0x270/+0x274/+0x278  the three Access subwindows (Individual / GroupTotals / OwnerClubs pages)
//   +0x27C  (access sub-control)              +0x288/+0x28C Name edit box + state
//   +0x290  add-access btn                    +0x294  remove-access btn
//   +0x2C8  edited access list (iterated by the 0x3146 save sender)   +0x2CC dirty flag
//   +0x2FC  Upkeep_Combobox
//   +0x300  TimeRemainingVal_Label   +0x304 its text color (0xFFFF0000 red when Expired)
//   +0x308  LimitedItemVal_Label     +0x30C DailyCharge_Label ("Daily Cost:" value)
//   +0x310  Balance_List CListWnd    +0x314 Upkeep_List CListWnd
//   +0x324/+0x328  column helper ptrs
//   +0x32C/+0x330/+0x334/+0x338  4 currency descriptor ptrs (Plat/Gold/Silver/Copper; 43.28c)
//   +0x340  pending escrow ACTION code (committed on notif 0x24 -> FUN_00741EE0)
//
// RealEstate RECORD (0x58-ish; ctor 0x007FE100, deser 0x007FDF00):
//   +0x00 vftable ; +0x04 escrow-map head ; +0x08 escrow-map buckets ; +0x0C escrow-map count
//   +0x14 re_id ; +0x18,+0x1C dwords ; +0x20 DEFINITION ID ; +0x24,+0x28 dwords
//   +0x2C,+0x30 CXStr (name/owner) ; +0x34 dword ; +0x38 placed-item collection
//   +0x3C UPKEEP COST-SET ID ; +0x40 dword(?) ; +0x44 byte(?) ; +0x48 item count ; +0x4C item array
//   escrow-map node: +0x10 currency key, +0x18 f64 balance (in base unit; coin=copper), +0x21 rb-color
//
// RealEstate MANAGER (GetManager 0x005B3540; singleton ptr [0x00E44570]):
//   +0x04 record-hash buckets ; +0x08 = 20011 bucket count ; +0x0C record count ; +0x38 owned-id set
//   +0x54/+0x58 ACCESS hash (buckets/count) ; +0x74 definitions hash ptr ; +0x78 COST hash ptr
//   +0x98 current estate id.  record-hash node: +0x00 record, +0x04 key(re_id), +0x08 next.
//   cost-hash node: +0x04 cost-set count, +0x08 cost-set entries, +0x14 key(cost-group id), +0x18 next.
//   defs-hash: +0x04 buckets, +0x08 count; def node +0x20 price cost-group, +0x24 upkeep cost-group.
// ============================================================================

// ============================================================================
// 43.28l  LIVE GROUND-TRUTH of the whole model (2026-08-01, overnight, example plot 9001)
// ============================================================================
// Walked the live RE manager and an example plot record to confirm every offset above end-to-end.
// (Addresses are per-session heap; the OFFSETS are what matter.)
//   GetManager singleton: instance ptr at [0x00E44570] (init flag [0x00E44574]); manager fields:
//     +0x04 record-hash buckets ; +0x08 = 0x4E2B (20011) bucket count ; +0x0C record count (=1) ;
//     +0x38 owned-id set ; +0x54/+0x58 access hash ; +0x74 gate ptr ; +0x78 cost hash ;
//     +0x98 = current estate id = 0x2329 = 9001 (the example plot).
//   Record-hash node: +0x00 record ptr ; +0x04 key(re_id) ; +0x08 next.  Bucket = id % 20011.
//   THE PLOT RECORD confirmed field-for-field (matches 43.28b/g Deserialize):
//     +0x00 vftable(RealEstate) ; +0x04 escrow-map head=0 ; +0x08 map buckets ; +0x0C map count=1 ;
//     +0x14 = 0x2329 = 9001 (re_id)  ; +0x20 = 0 (definition id) ;
//     +0x3C = 0 (UPKEEP COST-SET ID -- EMPTY) ; +0x40 = 0 ; +0x44 = 0 ; +0x48/+0x4C item list = 0/null.
//   ESCROW MAP entry (std::map node): +0x10 = key = 0 (coin) ; +0x18 = f64 = 100000.0 (= a 100pp
//     deposit, in COPPER).  So key 0 = coin, value = total copper as a double -- 43.28c CONFIRMED live.
//   => record+0x3C == 0 on the example plot is EXACTLY why Time Left reads "Unlimited" (43.28e/f seed -1.0).
//      The whole escrow+upkeep chain is now verified against real memory, not just static RE.
//
// COST HASH (mgr+0x78) IS ALREADY POPULATED -- the daily-cost data exists client-side:
//   mgr+0x78 -> hash obj {+0x04 buckets, +0x08 = 0xD3 (211) bucket count, +0x0C = 2 entries}.
//   The 2 containers are keyed 10 and 20 (bucket = key%211).  Container node: +0x04 cost-set count(=1),
//   +0x08 cost-set entries, +0x14 key, +0x18 next.  Container 10's cost-set (entry id 1) has ONE leaf:
//   { currencyKey 0 (coin), sub 0, u64 = 0x29040 = 168000 copper }.  So a real upkeep cost-set is
//   loaded (from the purchase 0x2698 definitions) and ready to use.
//   THE CHAIN to light it up (inferred, matches FUN_00741470 which calls PopulateCostCombo(def+0x24,
//   combo, record+0x3C)):  record+0x20 (def id) -> definition (FUN_007F4F70) -> definition+0x24 =
//   cost-group id (10/20) -> mgr+0x78 container -> record+0x3C picks the cost-set id (1) -> leaves.
//   A record with +0x20=0 and +0x3C=0 matches NEITHER, so daily_cost stays 0 -> "Unlimited".
//   => likely NO new 0x2698 needed: set record+0x20 to a definition whose +0x24 is 10 (or 20) and
//      record+0x3C to that cost-set id. CONFIRM the exact def id / cost-set id from a live 0x2698
//      capture (or by reading definition+0x24) before relying on it.
//
// DEFINITIONS read live too (mgr+0x74 -> hash {+0x04 buckets, +0x08 = 0x65 (101), +0x0C = 1 entry}):
//   ONE plot definition loaded.  definition node fields:
//     +0x00 = 100 (leading id) ; +0x20 = 10 (PRICE cost-group id) ; +0x24 = 20 (UPKEEP cost-group id,
//     the value PopulateCostCombo/FUN_00741470 passes to mgr+0x78) ; +0x28 = 0x78 ; +0x3C = 4 ; +0x48 = 6.
//   So this definition maps: price -> mgr+0x78 container 10, upkeep -> container 20; container 20's
//   cost-set id 1 = { coin, 168000 copper } (same as container 10 in this test data).
//   *** RECIPE -- PROVEN IN GAME 2026-08-01 (poked a live record, window rendered correctly) ***
//   The estate record needs: record+0x20 = definition id (resolves via FUN_007F4F70), and
//   record+0x3C = the UPKEEP COST-SET ID that FindCostSetById locates in that def's upkeep cost-group.
//   The example plot (9001) already had record+0x20 = 100; the ONLY
//   missing value was record+0x3C = 0.  A live breakpoint confirmed the client resolves def 100 ->
//   def+0x24 = upkeep cost-group 20 (cost_lookup 0x0068EDD0 got key=20) but then FindCostSetById(
//   group20, record+0x3C=0) missed because group 20's cost-set id is 3 (not 0), so daily_cost stayed
//   0 -> the formatter got days = -1.0 (captured on the stack) -> "Unlimited".
//   FIX PROVEN: wrote record+0x3C = 3, reopened the Manage window -> it rendered:
//       Default Upkeep: "12 platinum 6 copper"   (group 20 cost-set 3 leaf = {coin, 0x2EE6=12006cp})
//       Daily Cost: "12 platinum"                (DailyCharge_Label rounds off the 6cp)
//       Time Left: "1w 1d by Platinum"           (100000cp escrow / 12006cp = 8.33 days = 1w1d)
//   => The record must carry record+0x3C = the upkeep cost-set id
//      that matches the upkeep cost-block the server sends in the 0x2698 definitions (here 3), NOT 0.
//      def+0x20 = price cost-group (10), def+0x24 = upkeep cost-group (20); each group's cost-set has
//      its own id (group10 -> id 1, group20 -> id 3 this session). No new opcode; just the right +0x3C.
// ============================================================================

// ============================================================================
// 43.28c  ESCROW BALANCE DISPLAY -- CONSUMPTION SIDE, PROVEN IN GAME (2026-08-01)
// ============================================================================
// The record+0x04 escrow map (43.28b) is consumed by the Escrow-tab balance-list refresh.
// All file addrs 0x400000-based; runtime this session = file - 0x20000 (base 0x3E0000).
#define RealEstate__EscrowBalanceRefresh_x                          0x00741930  // REMW balance-list refresh
//   this = REMW window (ecx/esi). 3 callers, all data-update (none window-open):
//     0x00744056 & 0x007440B6 (per-record update, refresh if [RREmgr+0x98]==estate),
//     0x00747B2C (full populate: walk mgr estate map -> rebuild -> refresh).
//   So the tab repaints ONLY on an estate-record event (0x6FD0/0x63FD), never on window open.
//   REMW window fields: +0x310 = REMW_Balance_List (CListWnd); +0x32C/+0x330/+0x334/+0x338 =
//   the 4 currency DESCRIPTOR ptrs (Platinum/Gold/Silver/Copper). descriptor+0x04 = currency
//   object (obj+0x14 = name cstr "A_CopperCoin"/"A_PlatinumCoin"/...; obj+0x08 = icon-ish, NOT a
//   unique key -- silver and copper both 12, so do NOT use it as the map key).
//
// *** KEY 0 == "COIN" (the whole reason the fix is one entry, not four) ***
//   Refresh iterates the record+0x04 map. Per entry: entry+0x10 = KEY, entry+0x18 = f64 value.
//     0x00741CF1  cmp [entry+0x10],0 ; jne <alt-currency path 0x00741E13>
//     0x00741CFB  fld qword [entry+0x18]  -> the COIN branch: value is TOTAL COPPER, the client
//                 DECOMPOSES it itself into the four coin rows via a digit loop, labels eqstr
//                 0xFB6..0xFB9 (0x00741D24 sets 0xFB6; loop end cmp 0xFB9 at 0x00741DCF).
//     entry key != 0  -> 0x00741E13 = ALT-CURRENCY path (doubloons/orux/etc.), different rows.
//   => server sends ONE map entry { u32 key=0, f64 = total escrow copper }. Pre-splitting into
//      keys 0/1/2/3 only lit copper (keys 1/2/3 were parsed as alt-currencies). PROVEN: a
//      128666cp balance renders Platinum 128 / Gold 6 / Silver 6 / Copper 6, no coin paid.
//
// RE-manager (GetRealEstateManager 0x005B3540) map, used to resolve the record for a plot:
//   mgr+0x04 = bucket array base; mgr+0x08 = bucket count (0x4E2B = 20011); mgr+0x0C = record
//   count; mgr+0x98 = current estate id. node: +0x00 = record ptr, +0x04 = key(re_id), +0x08 = next.
//   record+0x04 escrow map object: {+0x04 head=0, +0x08 bucket/array base, +0x0C entry count}.
#define RealEstate__GetManager_x                                    0x005B3540  // returns RE manager
//
// RESOLVED (Daily Cost / Time Left / "Unlimited"): see 43.28e below. It is NOT record-tail
// +0x3C/+0x40/+0x44 -- Time Left is computed client-side as min(escrow_balance / daily_cost)
// across currencies, and "Unlimited" is simply the -1.0 sentinel meaning "no object with an
// upkeep cost is on the plot." Empty plot => no daily cost => -1.0 => "Unlimited". CONFIRMED.
// ============================================================================

// ============================================================================
// 43.28e  TIME LEFT / DAILY COST / "Unlimited" -- FULLY DECODED  (2026-08-01, overnight)
// ============================================================================
// Found by back-referencing the render-vocabulary literals in .rdata (all runtime addrs,
// static = runtime + 0x20000):
//   0x009D859F "@Expired"   0x009D85A8 "%dd"   0x009D85AC "%dw "   0x009D85B4 "<1d"
//   0x009D85C0 "Guild - %s" 0x009D85CC "Daily Cost:" 0x009D85D8 "Plot Upkeep" 0x009D85E4 " by "
//   0x009D85B8 double 0x3FC2492449249224 = 1/7 (days->weeks divisor)
//   0x009D85F0 double 0xBFF0000000000000 = -1.0 (the "no upkeep" SENTINEL)
// "Unlimited" IS a plain literal at 0x009BF82C (rt) -- the formatter push's it directly (live-verified
// 2026-08-01; note: eqstr 3475 also = "Unlimited" but the formatter does NOT use it). "Expired" =
// 0x009D85A0 (rt), also pushed directly.  Live disasm confirms the whole branch:
//   0x00720A87 fcom days vs 0.0 ; test ah,41 ; jne 0x00720BC7 (days<=0)
//   0x00720BC7 fcompp ; test ah,05 ; jp 0x00720BF1 -> push "Expired"(0x9D85A0)  [days==0]
//              fall-through push "Unlimited"(0x9BF82C)                          [days< 0]
//
// -- THE TIME-LEFT LEAF FORMATTER --
//   Ghidra FUN_00740a50  (file 0x00740A50 / runtime 0x00720A50).
//   Signature: CXStr* __?(CXStr* out, double days).  Turns a day-count into the label text:
//        days  < 0.0  ->  "Unlimited"   (the -1.0 sentinel; NO priced object on the plot)
//        days == 0.0  ->  "Expired"
//        days  > 0.0  ->  weeks = round(days/7); if weeks==0 && days rounds to 0 -> "<1d",
//                         else "%dw " (weeks) then "%dd" (days % 7).
//   So the entire Unlimited/Expired/Nw-Nd decision is driven by ONE double = days remaining.
#define RealEstate__FormatTimeRemaining_x                           0x00740A50  // file addr; runtime 0x00720A50
//
// -- THE MANAGE-WINDOW UPKEEP/ESCROW POPULATE (computes that double) --
//   Ghidra FUN_00742cd0  (file 0x00742CD0 / runtime 0x00722CD0). __thiscall(REMW* this, char rebuild).
//   This is the third of the three refreshers fired on a record event (43.6: 0x741470,
//   0x741930(0), 0x742CD0(1)).  rebuild!=0 rebuilds the Upkeep_List + Balance_List rows;
//   rebuild==0 just recomputes the time/daily labels.
#define RealEstateManageWnd__RefreshUpkeep_x                        0x00742CD0  // file addr; runtime 0x00722CD0
//   Data model it walks:
//     GetManager (0x5B3540) -> record for mgr+0x98 (current estate id) via the mgr hash.
//     record+0x08 = escrow std::map<u32 currencyKey, f64 balance> (node value at node+0x18) -- 43.28b/c.
//     record+0x3C = the placed-object list handle (house/crate/etc.) -- source of upkeep costs.
//   Window (REMW) fields USED here (add to the 43.28c/43.41 map):
//     +0x234  byte   "computed" flag (cleared on entry)
//     +0x238  int    Upkeep_List row count      +0x23C  ptr  Upkeep_List rows (stride 0x18)
//                    row+0x08 = u64 currency key; row+0x10 = char selected/enabled flag
//     +0x248  int    Balance_List row count     +0x24C  ptr  Balance_List rows (stride 0x04 ptr)
//                    per row: +0x00 currency key; +0x08 = DAILY COST (longlong); +0x10 = days = bal/cost (f64)
//     +0x300  ptr    TimeRemainingVal_Label   (SetText via vtbl+0x124 with FormatTimeRemaining out)
//     +0x304  dword  its normal text color; forced 0xFFFF0000 (red) when days==0 ("Expired")
//     +0x308  ptr    LimitedItemVal_Label     (name of the item that limits the time)
//     +0x30C  ptr    DailyCharge_Label        (the "Daily Cost:" value; built by formatting each
//                                              Balance_List currency amount, FUN_005AA5E0 + CXStr append)
//     +0x314  ptr    Upkeep_List CListWnd; +0x324/+0x328 = column helper ptrs
//   THE CALC (per currency k with a positive daily cost):
//     days_k = escrow_balance[k]  /  daily_cost[k]        (line ~347 of the decompile)
//     TimeLeft = MIN over k of days_k ; the limiting key -> iStack_c0 -> LimitedItemVal_Label.
//     Seed = -1.0. If at least one priced object exists but no balance covers it, force 0.0 (Expired).
//   => "Unlimited" (seed never overwritten) == the plot has NO placed object carrying an upkeep
//      cost.  The PLOT itself has zero upkeep; daily cost is the SUM of the placed house/crate
//      upkeep cost-sets (definition.f03, 43.36).  This matches live: an empty plot reads
//      "Time Left: Unlimited".
//
// CONSEQUENCE FOR THE SERVER (stock EQEmu):
//   daily_cost comes from a cost-set embedded in the estate record at +0x3C (see 43.28f), NOT
//   summed client-side from placed objects.  So to make Time Left show a real value the server
//   must serialize a non-empty upkeep cost-set into record+0x3C (same wire format as the 0x2698
//   Price/Upkeep cost blocks) AND send the escrow balance map (43.28c).  With record+0x3C
//   empty, daily_cost is 0 -> seed -1.0 -> "Unlimited", which is CORRECT.
// ============================================================================

// ============================================================================
// 43.28f  DAILY-COST SOURCE = the record's upkeep cost-set at +0x3C  (2026-08-01, overnight)
// ============================================================================
// RefreshUpkeep (43.28e, 0x00742CD0) turns record+0x3C into per-currency daily_cost like this:
//
//   record+0x3C  =  handle to the estate's UPKEEP COST-SET.  Same 2-level cost-block format as the
//                   0x2698 Price/Upkeep blocks (43.31/43.36): outer entries stride 0x14, each with
//                   a sub-list; leaf = { currency key, subtype, u64 amount }.  This same +0x3C is
//                   passed to PopulateCostCombo (0x0074D240) to fill the Upkeep_Combobox, and to
//                   the daily-cost summer below.  It is read from the wire by RealEstate::Deserialize.
//
#define RealEstate__SumUpkeepIntoBalanceList_x                      0x007413F0  // file addr; runtime 0x007213F0
//   FUN_007413F0(perEstateObj, record+0x3C): if perEstateObj valid (count at +4 > 0), opens the
//   cost-set (FUN_007FB8D0), reads each entry (FUN_007FB7D0 -> key,sub,u64 val) and calls:
#define RealEstateManageWnd__AccumulateBalanceRow_x                0x007411C0  // file addr; runtime 0x007211C0
//   FUN_007411C0(REMW* this, key, sub, val_lo, val_hi): scans Balance_List (this+0x248 count /
//   this+0x24C rows); the row whose [0]==key && [1]==sub gets its 64-bit daily_cost at row+0x08/
//   +0x0C incremented by val.  So daily_cost[currency] = SUM of the cost-set's entries for it.
//   Cost-block accessors: 0x007FB970 = entry sub-count, 0x007FB9B0 = sub-entry iterator,
//   0x007FB7D0 = read leaf (key,sub,u64), 0x007FB8D0 = open cost-set from the +0x3C handle.
//
// -- the SECOND refresher fired on a record event (43.6 list: 0x741470, 0x741930(0), 0x742CD0(1)) --
#define RealEstateManageWnd__RefreshUpkeepCombo_x                  0x00741470  // file addr; runtime 0x00721470
//   FUN_00741470(REMW* this): if this+0x2FC (Upkeep_Combobox) exists, looks up the plot DEFINITION
//   by record+0x20 (def id) via FUN_007F4F70, then PopulateCostCombo(def+0x24, this+0x2FC,
//   record+0x3C).  def+0x24 = the cost-combo template; record+0x3C = current upkeep cost-set.
#define RealEstate__DefinitionLookupById_x                         0x007F4F70  // hash: this+4 buckets, node+0x6C key, +0x70 next
#define RealEstate__PerEstateObjLookup_x                           0x006AEDD0  // hash at this+0x78, node+0x14 key, +0x18 next
//
// -- currency formatters used to paint the Daily Cost label + Balance_List (general-purpose) --
#define EQ__FormatCurrencyName_x                                   0x005AA540  // (out,type,..): type 0 -> eqstr 0xFB6 (coin); else alt-currency name+amount
#define EQ__FormatCurrencyAmount_x                                 0x005AA5E0  // (out,type,cur,val64): "<name> %I64d"; coin via 0x004DC5F0
// ============================================================================

// ============================================================================
// 43.29  *** 0x6FD0 IS THE WRITER OF mgr+0x8C. THE NATIVE PATH EXISTS. ***
//        (2026-07-28)
// ============================================================================
// Handler 0x005B3054, tail at 0x005B3090. ebp is THIS (set by `mov ebp, ecx` at 0x005B2523),
// esi is the freshly deserialized RealEstate record:
//
//       eax = record[0x14];                  // real_estate_id
//       [ebp+0x8C] = eax;                    // <<< THE ROOT ESTATE ID
//       eax = DefinitionTypeOf(record[0x20]);// 0x7F4FA0
//       [ebp+0x90] = eax;                    // <<< THE CURRENT TYPE
//       mgr->vt[9](record);                  // [[ebp]+0x24]
//
#define OP_RealEstate_SetCurrent                                    0x6FD0
#define RealEstate__SetCurrentHandler_x                             0x005B3054
//
// THIS IS THE MESSAGE 43.13 WENT LOOKING FOR AND 43.19 DECLARED DID NOT EXIST.
// It is server-sendable and sets exactly the field that CE set by hand in
// 43.20 -- so the gate that opened Manage in game is reachable natively, from the server, on a
// stock client.
//
// *** HOW THIS WAS MISSED FOR TWO SECTIONS -- THE LESSON IS THE POINT ***
// 43.19's static scan for `mov [reg+0x8C], x` DID list 0x005B30B1. It was discarded as a
// stack local because the ModRM byte was mod=10 rm=101 -> [ebp+disp32], and ebp+disp32
// "obviously" means a frame variable. In THIS function ebp is `this`. One filter assumption,
// applied silently to a correct result, produced the confident and wrong conclusion that
// +0x8C had no writer -- which then anchored 43.20-43.26 onto the override and the whole
// placed-object investigation.
//   RULE: NEVER filter [ebp+X] out of a member-write scan. MSVC uses ebp as a general
//   register in frame-pointer-omitted builds, and this binary is full of them. Filter on
//   whether the base register is provably esp, and on nothing else.
// (43.24's finding stands on its own: the placed-object collection really is empty, and
//  ResolveCurrentFromWorld really does return +0x8C unchanged. That is now a FEATURE --
//  0x6FD0 sets +0x8C, the world scan finds nothing, and the fallback delivers it. The two
//  mechanisms are complementary, not alternatives.)
//
// ---------------------------------------------------------------------------
// WHAT THE OTHER TWO RealEstate-CARRYING OPCODES DO (same 0x007FDF00 payload, 43.28)
// ---------------------------------------------------------------------------
// 0x4F09 (0x005B2D18): record add + UI refresh.
//       if (pinstRealEstateManageWnd /*0x00D1FCBC*/) RefreshWnd(record[0x14], 1); // 0x744070
//       if (record[0x2C] == pinstLocalPlayer+0xA4) [0x00F75AD8]->[0x28C] = 1;
// 0x63FD (0x005B2DD8): same shape, but first maps record[0x14] through 0x00466030 and
//       compares the RESULT's +0x2C when that lookup hits.
//
// => record +0x2C IS THE OWNER NAME. Both handlers string-compare it (0x008068C0) against
//    pinstLocalPlayer + 0xA4, i.e. "is this MY plot", and set a flag when it matches.
#define RealEstateRecord__OWNER_NAME_OFS                            0x2C
#define PlayerBase__NAME_OFS                                        0xA4    // [inferred]
#define pinstRealEstateManageWnd_ALT                                0x00D1FCBC  // == known value
//
// NEXT: send 0x6FD0 with a record whose +0x14 is the plot id, and watch
// mgr+0x8C. If it takes, occupancy resolves with no memory writes at all.
// ============================================================================

// ============================================================================
// 43.30  *** NATIVE PATH PROVEN IN GAME -- STOCK CLIENT, SERVER DRIVEN ***
//        (2026-07-28)
// ============================================================================
// A server sends OP_RealEstateSetCurrent (0x6FD0) on the lot-square click, carrying one
// RealEstate record serialized per 43.28.
//
// MEASURED LIVE with Cheat Engine, on a FRESH client (new PID, new manager allocation)
// so nothing could be left over from the earlier manual pokes:
//
//   BEFORE the click -- honest baseline, no memory written by anyone:
//       +0x0C  record count = 0
//       +0x8C  root estate  = -1
//
//   AFTER the click -- entirely the server's doing:
//       +0x0C  record count = 1        the 0x2691 record landed in the map
//       +0x8C  root estate  = 9001     *** SET BY THE 0x6FD0 PACKET ***
//       +0x90  current type = 4        NOT the 0xA "none" sentinel
//       +0x98  THE GATE     = 9001     resolved by UpdateOccupancy on its own
//   and in game every control on the plot window is live.
//
// +0x90 == 4 IS THE LOAD-BEARING DETAIL. It is not a copy of anything sent literally: the
// client took the definition_id off the wire, ran DefinitionTypeOf (0x7F4FA0) against its own
// definitions map, and produced 4. That proves the 0x2698 definition, the wire field `b`, and
// the f09 column are all genuinely driving client behaviour -- not just being stored.
//
// WHAT THIS UNBLOCKS: the occupancy gate was the single blocker on the entire feature. Every
// control that reads it -- Manage, Items, Move Out, Transfer, Opt Out, Vote -- is now
// reachable on a stock client. What those controls DO still depends on server-supplied
// data (escrow, access, items, price and upkeep).
//
// STILL UNIDENTIFIED in the record, sent as 0 / -1 / 0 and evidently harmless:
// wire fields -> +0x18, +0x24, +0x28. Also untested: a non-zero parent (+0x1C) and a
// non-empty child id array, which is what a real neighborhood will need.
// ============================================================================

// ============================================================================
// 43.31  THE 0x2698 COST BLOCK -- Price / Upkeep / Time Left   (2026-07-28)
// ============================================================================
// These three fields stayed blank even after the gate opened natively (43.30), because they
// are fed by the DEFINITION packet's trailing cost block, not by occupancy and not by 0x2FA0.
//
// ReadCosts, 0x007FC2B0 (this = the costs member, arg = stream):
//       this[0] = read_dword();                 // header / kind
//       count   = read_dword();
//       Resize(this+4, count);                  // 0x7FBEF0
//       for (i = 0; i < count; i++)
//           ReadCostEntry(array + i*0x14, stream);   // 0x7FBDB0, 0x14 bytes IN MEMORY
#define RealEstate__ReadCosts_x                                     0x007FC2B0
#define RealEstate__ReadCostEntry_x                                 0x007FBDB0
//
// ReadCostEntry, 0x007FBDB0 -- itself a nested array:
//       this[0] = read_dword();                 // entry key / type
//       n       = read_dword();
//       Resize(this+4, n);                      // 0x748380
//       for (i = 0; i < n; i++) {               // stride 0x10 ON THE WIRE
//           sub[0x00] = read_dword();
//           sub[0x04] = read_dword();
//           sub[0x08] = read_qword();           // one 8-byte read, cursor += 8
//       }
//
// SO THE WIRE LAYOUT OF THE WHOLE BLOCK IS:
//       dword  header
//       dword  cost_count
//       cost_count x {
//           dword  key
//           dword  sub_count
//           sub_count x { dword a; dword b; qword c; }        // 16 bytes each
//       }
//
// NOTE: an EMPTY cost block must carry every dword the reader takes before any entry.
// A sender that writes too few survives only because the fail-open reader (43.28) leaves
// count at 0 when the buffer is exhausted -- the same silent desync as 43.15/43.16.
// (Superseded by 43.32: the outermost dword is a block count.)
//
// NOT YET KNOWN: which key selects Price vs Upkeep, and what a/b/qword mean per sub-entry.
// The qword strongly suggests a money amount (copper, 64-bit) with a/b as currency or period
// selectors -- consistent with "Default Upkeep" being a dropdown and Time Left being a
// duration. Confirm empirically: send one entry, vary key, and read what the window shows.
// ============================================================================

// ============================================================================
// 43.32  CORRECTION TO 43.31 -- THE COST DATA IS THREE LEVELS DEEP, NOT TWO
//        (2026-07-28, found by hardware breakpoint)
// ============================================================================
// 43.31 described the cost data as "dword header, dword cost_count, entries..." and called
// the missing dword a one-off shortfall. THAT FRAMING WAS WRONG, and it was wrong in a way
// static reading could not catch: the outermost dword is a COUNT OF COST BLOCKS, and
// ReadCosts' own header+count live INSIDE each block.
//
// HOW IT WAS CAUGHT. Hardware breakpoints on ReadCosts (0x007FC2B0) and ReadCostEntry
// (0x007FBDB0) plus a POSITIVE CONTROL on RealEstate::Deserialize (0x007FDF00) and on the
// 0x2698 handler itself (0x005B278C). Result after sending a definition carrying one cost:
//       control_deser  HIT  (EAX = 0x00D7052C -> rebased 0x00A1052C, the RealEstate vftable)
//       defs_handler   HIT  (EAX = 0x00002698, so the definitions packet really did arrive)
//       readcosts      0 hits
//       costentry      0 hits
// A definition that demonstrably parses (capacities render, +0x90 = 4 comes from
// DefinitionTypeOf on the sent definition) while ReadCosts never runs is only possible if the
// call is gated. It is -- at 0x007FC6E5:
//       edi = stream; eax = read_dword();      // <- THE OUTER BLOCK COUNT
//       if (eax <= 0) goto 0x007FC771;         // 0x007FC704 jbe -- skips everything
//       loop eax times: ReadCosts(...)         // 0x007FC724
// 0 was being sent there, so the loop body never executed even once.
//
// *** THE ACTUAL WIRE LAYOUT ***
//   dword  block_count
//   block_count x {
//       dword  header                                  // ReadCosts this[0]; 0 accepted
//       dword  entry_count
//       entry_count x {
//           dword  key                                 // selects Price / Upkeep / Time Left
//           dword  sub_count
//           sub_count x { dword a; dword b; qword amount }   // 16 bytes each
//       }
//   }
#define RealEstate__ReadCostBlocks_x                                0x007FC6B0
//
// METHOD NOTE, and it is the real lesson of this section. 43.31 was assembled by reading
// ReadCosts and ReadCostEntry in isolation and assuming the caller framed them the way the
// names suggested. The caller was never checked. A single non-breaking hardware breakpoint
// answered "does this code even run" in one round trip and invalidated two sections of
// confident structure. When a decoded format does not produce the expected effect, BREAKPOINT
// THE READER BEFORE REFINING THE GUESS -- and always arm a positive control alongside it, or
// a dead breakpoint is indistinguishable from dead code.
// ============================================================================

// ============================================================================
// 43.33  *** COSTS RENDER -- Price / Upkeep / Buy CONFIRMED IN GAME ***
//        (2026-07-28)
// ============================================================================
// With the corrected three-level nesting from 43.32, sending ONE cost entry produced, on a
// stock client:
//       one cost entry: key=0 a=1 b=1 amount=12345   (owner left VACANT)
//   ->  Price:  "12345 Delivery Voucher"
//       Upkeep: "12345 Delivery Voucher"
//       and the BUY button appeared (previously Move Out / Transfer).
//
// WHAT IS NOW ESTABLISHED:
//   * The outer block_count really was the whole problem (43.32). Costs parse.
//   * `amount` (the qword) is the QUANTITY -- 12345 rendered verbatim.
//   * `a` and/or `b` select a CURRENCY / ITEM TYPE: the value 1 resolved to the real EQ item
//     name "Delivery Voucher", so the client is looking the id up, not echoing it.
//   * The BUY view requires BOTH a vacant owner AND a price. Sending your own character name
//     as owner suppresses the purchase UI entirely -- which is why the earlier key sweep
//     showed nothing and looked like a format failure. It was a plot-STATE failure.
//
// STILL OPEN:
//   * WHICH of a / b is the currency selector (vary each independently).
//   * HOW Price and Upkeep separate. One entry with key=0 populated BOTH, so either the key
//     is not the discriminator, or the two combos read the same list and the split lives in
//     the block `header` dword (sent as 0 here), or in sub_count ordering.
//   * Whether "Time Left" comes from the same data or from the 0x2FA0 reply.
//
// ============================================================================

// ============================================================================
// 43.34  COST WIRE FORMAT CONFIRMED CORRECT -- THE Price/Upkeep SPLIT IS ELSEWHERE
//        (2026-07-28)
// ============================================================================
// Sent TWO cost blocks (header 0 / amount 500, header 1 / amount 501) and breakpointed
// ReadCosts with the 0x2698 handler as a positive control:
//       defs (0x005B278C)  HIT   EAX = 0x00002698
//       rc   (0x007FC2B0)  HIT TWICE, EAX = 2 then 1   <- the remaining block count
// So BOTH BLOCKS PARSE. The three-level format in 43.32 is right and complete.
//
// In game, Price and Upkeep BOTH showed "500 Delivery Voucher" and the dropdown on each held
// exactly ONE option. 501 appeared nowhere. The client therefore reads both blocks, keeps
// both, and renders one -- so the Price/Upkeep discriminator is NOT any of:
//       * the per-entry key   (varied 0..5, no effect)
//       * the block header    (varied 0 vs 1, no effect -- this section)
//       * list ordering       (only one option ever reaches either dropdown)
//
// WHAT IS CONFIRMED ABOUT THE FIELDS:
//       amount (qword)  = the QUANTITY, rendered verbatim
//       a               = CURRENCY id, resolved to a name by the client:
//                           a=1 -> "Delivery Voucher"
//                           a=2 -> "Unknown DB String 0-18"  (missing from stock dbstr_us.txt)
//                         => whatever currency ships for real plots must exist in the
//                            client's dbstr or players see that placeholder.
//       b               = STILL UNIDENTIFIED, never successfully varied.
//       entry key       = no observed effect once the block header was introduced.
//
// NEXT CANDIDATES for the split, in order of cheapness: vary `b`; vary the per-entry key
// while the block header is held constant; or find what the Purchase window reads when it
// populates the two combos ([wnd+0x234] Price, [wnd+0x238] Upkeep, read at 0x0074D586 /
// 0x0074D5A8 via 0x0086A780/0x0086A7F0 -- find their AddItem side).
//
// ============================================================================

// ============================================================================
// 43.35  THE COST CURRENCY FIELD `a` -- IDENTIFIED  (2026-07-28)
// ============================================================================
// Tested in game against the server's own alternate_currency table:
//       a = 0  ->  "5 Gold" for amount=500  built-in COIN, amount is in COPPER
//       a = 1  ->  "Delivery Voucher"      alternate_currency.id 1 (item 55249)
//       a = 2  ->  "Unknown DB String 0-18"  no such row exists
//       a = 6  ->  "Echo of Memory"        alternate_currency.id 6 (item 46779)
//
// => `a` IS NOT a string-table id and NOT a coin-denomination enum.
//    a == 0 selects ordinary coin; a >= 1 is a straight alternate_currency.id lookup,
//    and the client renders the NAME OF THE MAPPED ITEM.
// The a=2 miss is what rules out the denomination reading: if 1/2/3 were silver/gold/plat it
// would have named one instead of falling through to the missing-string placeholder.
//
// *** THE AMOUNT UNIT DEPENDS ON `a`. *** This is easy to get wrong:
//       a == 0  -> amount is COPPER; the client converts to denominations for display.
//                  amount  500 -> "5 gold";  amount 5000 -> "5 plat".
//                  Standard EQ denominations: 1 plat = 10 gold = 100 silver = 1000 copper.
//                  Two independent data points, so this rule is solid - NOT "500" verbatim.
//       a >= 1  -> amount is a RAW ITEM COUNT; amount 500 rendered as "500 Delivery Voucher".
// 43.33/43.34 said "amount is the quantity, rendered verbatim". That holds ONLY for the
// alternate-currency case. A coin price entered as if it were gold would be 100x too cheap.
//
// PRACTICAL CONSEQUENCE: plot pricing currency is pure server data. Price in coin with a=0,
// or add a row to alternate_currency for a purpose-made housing currency. No client change
// and no code change either way -- but note a currency with no alternate_currency row (or
// whose item is missing) displays as "Unknown DB String 0-N" to the player, so validate the
// id before shipping one.
// ============================================================================

// ============================================================================
// 43.36  *** THE COST MODEL IS COMPLETE -- Price/Upkeep SPLIT SOLVED ***
//        (2026-07-28, confirmed in game)
// ============================================================================
// The split is NOT in the cost data at all. It is two fields on the DEFINITION, each naming
// a cost SET, and the block header is that set's id.
//
//   0x0074DE35   mov ecx,[esi+0x234]   ; Price combo
//                mov edx,[edi+0x20]    ; <- PRICE cost-set id, off the definition
//                call 0x0074D240
//   0x0074DE46   mov ecx,[esi+0x238]   ; Upkeep combo
//                mov edx,[edi+0x24]    ; <- UPKEEP cost-set id
//                call 0x0074D240
//
// and PopulateCostCombo (0x0074D240) is generic -- it takes the combo AS AN ARGUMENT, looks
// the id up in RealEstateManagerClient+0x78, and AddItem()s each option. That is why every
// field varied inside the cost data did nothing: the same routine fills both combos, and the
// caller alone decides which set each gets.
#define RealEstate__PopulateCostCombo_x                             0x0074D240
#define RealEstateManagerClient__COSTSET_MAP_OFS                    0x78   // node: key +0x14, next +0x18
#define RealEstatePurchaseWnd__PRICE_COMBO_OFS                      0x234
#define RealEstatePurchaseWnd__UPKEEP_COMBO_OFS                     0x238
#define RealEstateDefinition__PRICE_COSTSET_OFS                     0x20   // wire f02
#define RealEstateDefinition__UPKEEP_COSTSET_OFS                    0x24   // wire f03
//
// FIELD MAPPING, cross-checked rather than assumed: f09 is known to sit at definition+0x3C,
// so stepping back 4 bytes per wire field gives f00 +0x18 ... f04 +0x28, f05 +0x2C -- and
// those are exactly the capacity and vault values already observed rendering. So
//       f02 -> +0x20 = PRICE cost set,   f03 -> +0x24 = UPKEEP cost set.
//
// CONFIRMED IN GAME: definition with f02=1, f03=2; cost set 1 = 5000 copper, set 2 = 10000.
//       Price:  "5 platinum"      Upkeep: "10 platinum"
// Sending f02 = f03 = 0 is why every earlier probe showed the same value twice -- both combos
// looked up set 0, and the second set parsed correctly but had nothing pointing at it.
//
// *** THE COST MODEL IS NOW FULLY SPECIFIED AND ENTIRELY SERVER-DRIVEN ***
//   definition.f02 -> price cost-set id
//   definition.f03 -> upkeep cost-set id
//   cost block header == the cost-set id
//   entry sub-fields  { a, b, qword amount }
//        a == 0  ordinary coin, amount in COPPER (5000 -> "5 platinum")
//        a >= 1  alternate_currency.id, amount is a raw item count
//        b       still unidentified; varying it had no observable effect
// Everything above is DB-drivable with no client change of any kind.
// ============================================================================

// ============================================================================
// 43.37  *** THE C->S OPCODE ENCODER -- AND WITH IT EVERY OPCODE THE CLIENT
//        CAN SEND ***  (2026-07-28, overnight pass)
// ============================================================================
// Live captures only reveal C->S opcodes for controls someone actually clicks
// with logging on. That is no longer the limit. Every
// outbound message in the RoF2 client is assembled by ONE fixed idiom:
//
//      mov  ecx,[0x00B828B0]        ; opcode-translation object
//      push 0
//      push <OPCODE>                ; <-- the wire opcode, as a literal
//      call 0x007D03C0              ; returns the wire opcode in AX
//      push <SIZE>                  ; TOTAL bytes, INCLUDING the 2-byte opcode
//      mov  [0x00DD00E0],ax         ; opcode word at the head of the out buffer
//      mov  ecx,<N>                 ; N dwords, N*4 == SIZE-2
//      lea  esi,[esp+X]             ; the payload this method built on its stack
//      mov  edi,0x00DD00E2          ; body starts 2 bytes past the opcode
//      push 0x00DD00E0
//      repe movsd
//      mov  ecx,[0x00DD25AC]        ; the connection
//      push 4
//      call 0x008C51F0              ; send
//
#define EQ__EncodeOutboundOpcode_x                                  0x007D03C0
#define EQ__SendMessage_x                                           0x008C51F0
#define pinstOutboundPacketBuffer_x                                 0x00DD00E0  // opcode word; body at +2
#define pinstConnection_x                                           0x00DD25AC  // NULL => "Attempt to send message %d on a void connection."
//
// 0x007D03C0 is effectively IDENTITY in RoF2 -- its tail is `mov eax,[ebp+8];
// pop ebp; ret 8`, i.e. it hands back its first argument. So *the constant pushed
// immediately before the call IS the wire opcode*.
//
// SCANNING FOR IT gives the complete inventory: 813 call sites, 449 distinct
// opcodes (opcode, count, size, every call site).
//
// FOUR-WAY CONFIRMATION that the scan reads the right constant: the opcodes and
// sizes it reports for 0x792C (190), 0x6573 (10), 0x2691 (226) and 0x08A6 (10)
// are byte-for-byte what 43.21 / 43.21b / 43.25 captured off the wire.
//
// ---------------------------------------------------------------------------
// THE REAL-ESTATE C->S SURFACE  (site = static addr of the encode call)
// ---------------------------------------------------------------------------
// The five real-estate windows own 0x00741000..0x0074E000 (ManageWnd notify
// 0x745090, NeighborhoodWnd 0x74A3A0, PlotSearchWnd 0x74C110, PurchaseWnd
// 0x74D500 -- RespawnWnd starts at 0x74E7B0, so ANYTHING AT OR ABOVE 0x0074E000
// IS A DIFFERENT SUBSYSTEM. An earlier draft of this table listed 0x0ECB, 0x2B8E,
// 0x4742, 0x66F0, 0x1256, 0x50F5, 0x3358, 0x0EFA and 0x217C as real-estate
// opcodes purely because their call sites were "nearby". They are not: they
// belong to RespawnWnd, ServerListWnd, SkillsWindow, SoulmarkWnd and others.
// Address proximity is not attribution -- check the window table above.)
//
//   opcode  size  call sites                        what
//   0x2AA6   ?    742083 742614 745985 745B29
//                 746153 74DA60                     6 sites, incl. the plot wnd
//   0x3146   ?    747FDC                            (Manage-window region)
//   0x2691   226  74A4EC 74A891 74B9D1 74BEB8        plot search (43.12)
//   0x218C   14   74A7E8                            (Neighborhood region)
//   0x796C   6    74C2DC                            GO TO PLOT -- one dword,
//                                                   the id taken from the plot
//                                                   list's selected row; sent by
//                                                   PlotSearchWnd's Go button
//   0x792C   190  74C91D                             MOVE OUT / sell (43.21),
//                                                    answered by 0x66BF (43.49)
//   0x45BE   190  74D5F4                             *** PURCHASE *** (43.38)
//   0x6573   10   74DC0E                             voting opt-out (43.25)
//   ---- manager region, less certain: 0x005B14D9 and 0x005B2117 sit squarely
//   ---- inside RealEstateManagerClient; the 0x5A0xxx / 0x5BCxxx ones may not.
//   0x507F   190  5B14D9                             the SAME 190-byte struct as
//                                                    purchase -- a strong hint
//                                                    that it is a sibling request
//   0x711A   98   5B2117
//   0x0971   10   5A0D55 5A3B42                      [unverified]
//   0x7EB9   22   5A9519                             [unverified]
//   0x7273   ?    5BC9A5 5BCBB2 5BCDC5 5BD348        [unverified]
//
// 0x2698 and 0x6FD0 have NO C->S sender, which is the negative control: they are
// server->client only, exactly as 43.14 / 43.29 assumed.
//
// ---------------------------------------------------------------------------
// *** RETRACTION: 0x2FA0 IS NOT A REAL-ESTATE OPCODE. ***
// ---------------------------------------------------------------------------
// 43.21b called answering 0x2FA0 "THE CRITICAL PATH" and said the Manage window
// stays dead until it is answered. That is WRONG and must not be acted on.
// 0x2FA0 has SIX senders, all in one unrelated class at 0x004A6920 / 0x004A69B0 /
// 0x004A6BE0 / 0x004A6D20 / 0x004A6E60 / 0x004A6F70, and their only callers are
// 0x004D3E8C, 0x004FE140, 0x0052EAB3, 0x00692EAE, 0x00693FEC, 0x0069537E,
// 0x00695FD3 and 0x0069708D -- not one of them in the real-estate window code
// (0x0074xxxx) or the manager (0x005Axxxx/0x005Bxxxx). Each sender is also
// rate-limited to one send per 3000 ms against a global tick, which is a POLLING
// idiom, not a window-open request. It merely happened to fire while the Manage
// window was open.
// LESSON, the same one as 43.32: correlation in a packet log is not attribution.
// The sender's CODE LOCATION is the attribution.
// ============================================================================

// ============================================================================
// 43.38  *** RealEstatePurchaseWnd FULLY DECODED -- INCLUDING BUY ***
//        (2026-07-28)
// ============================================================================
// The window a lot-square click opens is RealEstatePurchaseWnd. Its
// WndNotification is 0x0074D500 and it acts on message 1 (left-click) only.
// Every control was matched to its XML element name by taking the REPW_* string
// block at 0x009F9064..0x009F91E8 in REVERSE order against the member offsets the
// notification compares against -- and every one lands on a control whose observed
// behaviour matches its name, including the two combos pinned independently in
// 43.36. Eighteen controls agreeing at once; this is not a guess.
//
// (RealEstatePurchaseWnd__WndNotification_x is defined once, far above in the
//  window table -- corrected there from 0x74C110 to 0x74D500 by this section.)
#define RealEstatePurchaseWnd__REAL_ESTATE_ID_OFS                   0x228
// (0x234 / 0x238 are already defined in 43.36 as PRICE_COMBO_OFS / UPKEEP_COMBO_OFS
//  -- confirmed here as REPW_Price_Combobox / REPW_Upkeep_Combobox.)
#define RealEstatePurchaseWnd__CAPACITY_LABEL_OFS                   0x23C  // REPW_CapacityVal_Label
#define RealEstatePurchaseWnd__VAULT_CAPACITY_LABEL_OFS             0x240  // REPW_Vault_CapacityVal_Label
#define RealEstatePurchaseWnd__NPC_CAPACITY_LABEL_OFS               0x244  // REPW_Npc_CapacityVal_Label
#define RealEstatePurchaseWnd__CATEGORY_LABEL_OFS                   0x248  // REPW_Category_Label
#define RealEstatePurchaseWnd__PURCHASE_BTN_OFS                     0x24C  // REPW_Purchase_Button  -> sends 0x45BE
#define RealEstatePurchaseWnd__SELL_BTN_OFS                         0x250  // REPW_Sell_Button      -> confirm 0x1DD2 -> 0x792C
#define RealEstatePurchaseWnd__ITEMS_BTN_OFS                        0x254  // REPW_Items_Button     -> runs the local "items" command
#define RealEstatePurchaseWnd__MANAGE_BTN_OFS                       0x258  // REPW_Manage_Button    -> runs the local "manage" command
#define RealEstatePurchaseWnd__TRANSFER_BTN_OFS                     0x25C  // REPW_Transfer_Button  -> confirm 0x1DD3, NO packet
#define RealEstatePurchaseWnd__TYPE_LABEL_OFS                       0x260  // REPW_TypeVal_Label
#define RealEstatePurchaseWnd__OWNER_LABEL_OFS                      0x264  // REPW_OwnerVal_Label
#define RealEstatePurchaseWnd__GUILD_SCREEN_OFS                     0x268  // REPW_Guild_Screen
#define RealEstatePurchaseWnd__GUILD_LABEL_OFS                      0x26C  // REPW_GuildVal_Label
#define RealEstatePurchaseWnd__VOTE_BTN_OFS                         0x270  // REPW_Vote_Button
#define RealEstatePurchaseWnd__OPTOUT_LABEL_OFS                     0x274  // REPW_VotingOptOut_Label
#define RealEstatePurchaseWnd__OPTOUT_CHECKBOX_OFS                  0x278  // REPW_VotingOptOut_Checkbox -> 0x6573
//
// THIS SETTLES 43.21b's "the other buttons send nothing". They send nothing
// because they are NOT packet buttons: Items and Manage run local UI commands,
// Transfer only raises a confirmation dialog, and Sell's packet is emitted by the
// dialog's YES callback (a different function, 0x0074C918), never by the button.
//
// ---------------------------------------------------------------------------
// OP_RealEstate_Purchase  0x45BE   C->S, 190 bytes total / 188-byte body
// ---------------------------------------------------------------------------
#define OP_RealEstate_Purchase                                      0x45BE
#define OP_RealEstate_Purchase__SIZE                                190
//
// Built at [esp+0x2C] by a ctor at 0x0090D6F0 (which zeroes it), then:
//   +0x00   dword   real_estate_id        <- this->[0x228], the plot on screen
//   +0x04.. zero                          (the ctor fill)
//   +0xAC   dword   PRICE  selection      <- Price  combo item DATA
//   +0xB0   dword   UPKEEP selection      <- Upkeep combo item DATA, 0 if no combo
//   +0xB4   dword   global [0x00DD5D00]
//   +0xB8   dword   (tail, left as the ctor set it)
//
// The two selections are read as  combo->GetItemData(combo->GetCurChoice())  via
// CComboWnd__GetCurChoice_x / __InsertChoice_x / __GetItemCount_x already exist
// in the CComboWnd block far above (0x86A780 / 0x86AE50 / 0x86A770); only these
// two were missing:
#define CComboWnd__GetItemData_x                                    0x0086A7F0
#define CComboWnd__SetSelection_x                                   0x0086A740
//
// *** SO THE COST ENTRY'S KEY FIELD IS NOT DEAD AFTER ALL. *** 43.32 recorded the
// per-entry key as "no observed effect". Its effect is HERE: PopulateCostCombo
// puts a per-entry value into the combo item's data, and that value is what comes
// back in the purchase request to say WHICH price the player picked. See 43.39.
//
// GUARDS the client applies before it will send (all client-side -- the server
// still has to re-check every one of them):
//   * 0x005B3540(real_estate_id, 1) must return an object, and 0x005B1890 on that
//     object must return true -- the "may I buy this" test;
//   * this->[0x234] (the Price combo) must be non-NULL.
// That is the code behind 43.36's empirical "Buy needs BOTH a vacant owner AND a
// price".
//
// ---------------------------------------------------------------------------
// 0x6573 REFINED -- the opt-out packet is decoded now, not inferred
// ---------------------------------------------------------------------------
//   +0x00   dword   real_estate_id   <- manager+0x98 (the CURRENT estate, 43.30)
//   +0x04   dword   checkbox state   <- byte at checkbox+0x1E4, zero-extended
// 10 bytes total. Note it uses manager+0x98, NOT the window's own [0x228] -- so
// opt-out only functions once server-side occupancy has been set (via 0x6FD0).
// ============================================================================

// ============================================================================
// 43.39  THE S->C DISPATCHER, COMPLETE -- AND 0x45BE IS BIDIRECTIONAL
//        (2026-07-28, decompiled from 0x005B2500)
// ============================================================================
// 43.27 listed 18 branches read off a compare chain. The decompile of the whole
// function shows 25, and corrects two claims:
//
//   * CORRECTION: 43.27 said "0x2691 (search) is NOT in this table". It IS --
//     0x2691 dispatches to 0x005B0F80. 43.12/43.13 reached the same reader by a
//     different route, which is why nothing broke.
//   * CORRECTION: 43.2 named 0x6FD0 "OP_RealEstate_PlotRemove [inferred]" and
//     0x4F09 "PlotUpdate". Both are wrong. 0x6FD0 is SetCurrent (43.29, proven
//     live). 0x4F09 is the REMOVE: it calls manager vt[0x28](id,0,0,0) whereas
//     0x63FD and 0x6FD0 both call vt[0x24](record) to ADD. Do not act on the
//     43.2 names; they predate any of this.
//
// The reader idiom is uniform: the handler builds a 3-dword stream cursor on its
// stack -- { buffer, length, position=0 } -- and hands its ADDRESS to the reader.
// Every field read is the fail-open form from 43.28. Under-sending desyncs
// silently; it never errors.
//
//   opcode  what it actually does
//   ------  ---------------------------------------------------------------
//   0x0004  new 0x48 record -> 0x005B7A90 deser -> 0x005B59A0 add -> ManageWnd
//   0x0351  explicit no-op (returns immediately)
//   0x087F  0x005B0E30(stream)
//   0x0C13  placed-item feed; branches on body[0x68], -> 0x007C9B10/0x007BFB90
//   0x1097  falls into the 0x005AFE00(opcode, stream) generic tail
//   0x1228  0x005AE1F0(*(dword*)body)             result code (43.7)
//   0x1967  0x005AFD50(body)
//   0x1BAF  0x005AFE80(0x1BAF, body)              *** not previously listed ***
//   0x1CEF  0x005B0D00(stream)
//   0x21EA  placed NPC item: spawn lookup 0x005996E0(id), name at body+4,
//           spawn[0x198]=body+0x18, spawn[0x420]=body+0x1C
//   0x21EC  0x007375F0(body) -- only if the ITEMS window exists (0xD1FCB8)
//   0x2691  0x005B0F80(stream)                    plot-search reply (43.13)
//   0x2698  0x007F5B60(stream) defs, 0x007FC6B0(stream) costs, 0x005AAAD0()
//   0x2AA6  0x00741670(stream) -- only if the MANAGE window exists (0xD1FCBC)
//   0x2AA7  count-prefixed id list -> 0x007C9AA0 each
//   0x2CF7  generic tail 0x005AFE00                *** not previously listed ***
//   0x3968  new 0x48 record + 0x005B5250/0x005B5830, add
//   0x3D94  explicit no-op (returns immediately)   *** 43.2 was wrong about this
//   0x3E00  count-prefixed id list -> 0x007C9B10   *** not previously listed ***
//   0x45BE  *** THE PURCHASE REPLY *** see below
//   0x485D  new 0x48 record, deser, 0x005B7DE0 update, refresh
//   0x4F09  new 0x58 RealEstate, deser, MANAGE-wnd 0x00744070(id,1),
//           then manager vt[0x28](id,0,0,0)        = REMOVE
//   0x58C7  string message -> 0x006AE610 (chat)
//   0x5A63  eviction/vote confirm: 0xBC callback via 0x005AD760(body), text id
//           0x1DD4, 300000 ms (5 min) dialog. body: dword real_estate_id at +0,
//           string at +0x0C.
//   0x63FD  new 0x58 RealEstate, deser, add via vt[0x24], tell the MANAGE
//           window (0x00743FD0); and if the PURCHASE window is open and
//           record.id == manager[0x98] (the estate you are standing in),
//           repopulate it from the record via SetFromRecord 0x0074DCB0.
//   0x66BF  0x005AD930(*(dword*)body)
//   0x66DF  count-prefixed id list -> 0x007C9B80   *** not previously listed ***
//   0x68BA  0x005ADAD0(*(dword*)body)              *** not previously listed ***
//   0x6FD0  SetCurrent (43.29): mgr[0x8C]=rec[0x14], mgr[0x90]=type(rec[0x20]),
//           then vt[0x24](record)
//   0x76C6  placed-item feed; branches on body[0x68]
//   0x7D28  0x005B4680(*(dword*)body)              *** 43.2 called this a no-op
//
// ---------------------------------------------------------------------------
// *** 0x45BE IS BOTH DIRECTIONS. THE PURCHASE ACK IS ONE BYTE. ***
// ---------------------------------------------------------------------------
// The S->C branch is four lines:
//
//      if (opcode != 0x45BE)        return;
//      if (body[0] == 0)            return;            // success flag
//      if (pinstRealEstateManageWnd == NULL) return;
//      manageWnd->vt[0xD8](1, 1, 1);                   // = CXWnd::Show(...)
//
// vt+0xD8 is the ordinary CXWnd show/hide slot -- (visible, bringToTop,
// updateLayout). SetFromRecord (43.44) drives every button through the same
// slot, which is what fixes its meaning.
//
// *** AND THAT FULLY EXPLAINS WHY CLICKING BUY "JUST CLOSES THE WINDOW". ***
// The Purchase handler's own tail is  purchaseWnd->vt[0xD8](0, 1, 1)  -- it HIDES
// the plot window the instant the request goes out, and expects the server's
// 0x45BE ack to SHOW the Manage window in its place. With no reply, the
// plot window closes and nothing opens. Nothing is broken; the reply is
// simply missing.
#define CXWnd__Vtbl_Show_OFS                                        0xD8
// pinstRealEstateManageWnd_x (0xD1FCBC), pinstRealEstateNeighborhoodWnd_x
// (0xD1FCC0) and pinstRealEstatePurchaseWnd_x (0xD1FCC8) are ALREADY defined in
// the window-pointer block near the top of this file, and they are CORRECT --
// verified 2026-07-28 by reading each live object's vftable and matching it
// against the window-name string blocks (Items 0x009F82E0, Manage 0x009F8600,
// Neighborhood 0x009F8C28, PlotSearch 0x009F8EA8, Purchase 0x009F9200).
// An earlier draft of this section had all three swapped. They are not.
// pinstRealEstateItemsWnd_x (0xD1FCB8) and pinstRealEstatePlotSearchWnd_x
// (0xD1FCC4) are in that same block and also check out -- so all FIVE
// real-estate window pointers near the top of this file are now confirmed
// against the live process, not just inferred.
#define RealEstatePurchaseWnd__SetFromRecord_x                      0x0074DCB0
#define RealEstateManageWnd__NoteRecordRemoved_x                    0x00744070
#define RealEstateManageWnd__NoteRecordAdded_x                      0x00743FD0
//
// THE MINIMUM SERVER-SIDE PURCHASE ROUND TRIP, therefore, is:
//    C->S 0x45BE {id, price_key, upkeep_key, ...}
//    S->C 0x45BE { byte 1 }                        -> Manage window opens
//    S->C 0x63FD { the updated RealEstate record } -> owner name, buttons, and
//                                                     the manager's copy all
//                                                     follow, because 0x63FD
//                                                     re-runs SetFromRecord when
//                                                     record.id == mgr[0x98].
// No second window, no extra opcode.
// ============================================================================

// ============================================================================
// 43.40  THE COST ENTRY `key` IS THE PURCHASE SELECTOR -- LOOP CLOSED
//        (2026-07-28)
// ============================================================================
// 43.32 wrote the per-entry `key` down as "no observed effect" because nothing
// in the plot window changed when it varied. It has an effect; it just is not
// visible until Buy is pressed.
//
//   PopulateCostCombo (0x0074D240) walks the matched cost set and, for entry i,
//   inserts a combo choice whose ITEM DATA is  0x007FB930(costset, i)  which is
//
#define RealEstateCostSet__ENTRY_COUNT_OFS                          0x04
#define RealEstateCostSet__ENTRY_ARRAY_OFS                          0x08
#define RealEstateCostSet__ENTRY_STRIDE                             0x14
//        return *(dword*)(costset[0x08] + i * 0x14);      // == entry.key
//
//   i.e. the FIRST dword of the 20-byte in-memory entry, which is exactly the
//   `key` dword on the wire. The purchase request then sends back
//   combo->GetItemData(combo->GetCurChoice()) at +0xAC (price) and +0xB0
//   (upkeep) -- so those two wire fields are the server's own key values coming home.
//
// PRACTICAL CONSEQUENCE: give every cost entry a DISTINCT, MEANINGFUL
// key (a row id from the cost table). The server then reads +0xAC/+0xB0 out of
// 0x45BE, looks the two rows up, and knows precisely which price and which
// upkeep the player chose -- no positional guessing, no trusting the amount.
// A key of 0 for every entry makes every choice
// indistinguishable on the wire.
//
// The cost-set map itself hangs off the manager:
//   mgr[0x78] -> hash map; bucket = map[4][key % map[8]]; node key at +0x14,
//   next at +0x18, entry count at +0x04, entry array at +0x08.
// The manager's RECORD map is a different shape -- node = { record*, key, next* }
// at +0x00/+0x04/+0x08, buckets at mgr[0x04], modulus at mgr[0x08].
//
// The combo LABEL is built by 0x005AA760, which iterates the entry's sub-entries
// (0x007FB970 = count, 0x007FB9B0(entry,i,&a,&b,&amount)) and joins them with a
// separator. That is the same three-level shape as the wire format in 43.32 and
// is why an entry can carry several sub-entries: a compound price such as
// "5 platinum and 3 Echoes of Memory" is ONE choice with two sub-entries.
//
// CanBuy (0x005B1890, args: mgr, real_estate_id, bool showErrorPopups) is the
// gate the client applies before it will even send. In order:
//   1. mgr[0xA4] (the request-pending flag, from RealEstateManagerClient.cpp's
//      SetRequestPending) must be clear;
#define RealEstateManagerClient__REQUEST_PENDING_OFS                0xA4
//   2. the id must resolve in the manager's record map;
//   3. DefinitionTypeOf(record[0x20]) must pass 0x007F6030 (a purchasable-type
//      test);
//   4. 0x005AD900(pinstCharInfo) true -> allowed unconditionally (a bypass);
//   5. flags = 0x005B5B60(record, myId): bit 0x10000 -> refuse (string 0x14C3),
//      bit 0x20000 -> refuse (string 0x1536);
//   6. 0x005B5B90(record, me) non-zero -> refuse (string 0x14F);
//   7. 0x005B41D0(record) true -> refuse (string 0x191B);
//   8. 0x005B5130(record, charInfo[0x204C]) false -> refuse (string 0x14C5);
//   9. if 0x007F6280(type) is false -> ALLOWED; otherwise a guild path with
//      strings 0x2F2 / 0x2F3.
// All of it is client-side advisory. The server must re-check ownership, funds
// and eligibility itself -- the client will happily be told to send.
// ============================================================================

// ============================================================================
// 43.41  THE WHOLE HOUSING UI, BY NAME  (2026-07-28)
// ============================================================================
// Every control of every real-estate window, read straight out of the XML-name
// string block at 0x009F7E00..0x009F9400. Windows bind their controls in
// DESCENDING string order (see 43.38), so this doubles as a member-offset map
// once one offset in a window is known.
//
// RealEstatePurchaseWnd (REPW_) -- offsets in 43.38, plus:
//      REPW_Eviction_Label                                  -> +0x27C
//
// RealEstateManageWnd (REMW_) -- the window 43.21b watched stay dead:
//      Eviction_Label, AddWeekTemplate_Btn, AddDayTemplate_Btn, AddWeek_Btn,
//      AddDay_Btn, WithdrawAll_Btn, Upkeep_List, Balance_List,
//      DailyCharge_Label, LimitedItemVal_Label, TimeRemainingVal_Label,
//      Upkeep_Combobox, Transfer_Btn, Property_Label, EscrowProperty_Combobox,
//      Remove_Btn, Add_Btn, Access_Combobox, Name_EditBox, Permissions_List,
//      GroupTotals_List, OwnerClubs_List, Name_List, Permissions_Checkbox,
//      Groups_Combobox, ViewOwnerClubsPermissions_Label, ViewGroupTotals_Btn,
//      ViewOwnerClubsPermissions_Btn, ViewIndividualPermissions_Btn,
//      AccessPage, Subwindows
//
// RealEstateItemsWnd (REIW_) -- placement and LAYOUTS:
//      LayoutRotateYard_Combobox, LayoutStoreExtraItems_Btn,
//      LayoutUseInventoryItems_Btn, LayoutUsePlacedItems_Btn, LayoutDelete_Btn,
//      LayoutDetails_Btn, LayoutLoadHouse_Btn, LayoutLoadYard_Btn,
//      LayoutLoadAll_Btn, LayoutSaveHouse_Btn, LayoutSaveYard_Btn,
//      LayoutSaveAll_Btn, LayoutName_EditBox, LayoutList, ItemList,
//      Trophies_Button, Unpack_Moving_Crate_Button, Pack_Moving_Crate_Button,
//      Stow_Placed_Button, Locate_Button, Place_Button, Move_Crate_Button,
//      Move_Shed_Button, Move_Closet_Button, Move_Inventory_Button,
//      ShowUnownedItemsButton, ShowOwnedItemsButton, ShowUnplaceableButton,
//      ShowPlaceableButton, ShowMovingCrateButton, ShowYardButton,
//      ShowHouseButton, ShowPlacedButton, ShowStoredButton, SelectedItem,
//      SelectedItemLocation, SelectedItemLabel, and the seven count labels
//      (MovingCrate/House/Yard, with NonRealEstateItems and NpcItems variants).
//      The layout file path format lives here too: ".\%s\%s_%d.xml" with
//      "Auto Save" / "Yard and House" -- i.e. the RE_Layout_*.xml files 43.5
//      captured are written BY THIS WINDOW, client-side.
//
// RealEstateNeighborhoodWnd (RENW_): ShowOpenGPlots_Btn, ShowFullGPlots_Btn,
//      ShowOpenPPlots_Btn, ShowFullPPlots_Btn, ShowGuild_Btn, ShowNonGuild_Btn,
//      GuildList, PlayerList, NeighborhoodList, Go_Button, Search_Button,
//      MyItems_Button, MyPlots_Button, SearchType_Combobox, Search_EditBox
//
// RealEstatePlotSearchWnd (REPSW_): ShowVacant_Btn, ShowOwned_Btn,
//      ShowGuild_Btn, ShowPlayer_Btn, PlotList, Go_Btn, MyItems_Btn,
//      MyPlots_Btn, Search_Btn, Search_EditBox
//
// ---------------------------------------------------------------------------
// THE ACCESS MODEL, IN THE CLIENT'S OWN WORDS (0x009F84A0..0x009F8584)
// ---------------------------------------------------------------------------
// Thirteen permissions:
//      Manage, Interact Item, Move Item, Add Item, Remove Item, Mail Item,
//      Add House, Remove House, Add Crate, Remove Crate, Withdraw Upkeep,
//      Enter, Receive Buffs
// Seven roles, most to least privileged:
//      Owner, Co-Owner, Trustee, Friend, Acquaintance, Visitor, Everyone
// Plus the upkeep vocabulary the Manage window renders: "Daily Cost:",
// "Plot Upkeep", "%dw " (weeks remaining), "@Expired", "Guild - %s".
// This is the shape any access/escrow implementation has to produce.
// ============================================================================

// ============================================================================
// 43.42  OP_RealEstate_Result (0x1228) -- THE COMPLETE RESULT-CODE TABLE,
//        RESOLVED TO THE ACTUAL ENGLISH THE PLAYER SEES  (2026-07-28)
// ============================================================================
// 43.7 identified 0x1228 as the result-code channel but not what any code means.
// The handler is 0x005AE1F0, called as handler(*(dword*)body) -- a single dword.
// It is a plain switch from code to an eqstr_us.txt string id, and the ids
// resolve cleanly against the shipped client's own eqstr_us.txt.
//
//   code  -1   silently ignored (no message, no refresh)
//   code   0   *** SUCCESS ***  no text; refreshes the items window and, if the
//              neighborhood window exists, 0x00733CB0 on it. THIS is the "it
//              worked" signal, and it is the code to send after any accepted
//              real-estate operation. (The refresh it triggers is on the ITEMS
//              window, 0xD1FCB8, via 0x00733CB0.)
//
//   code  str     the message the player gets
//   ----  ------  --------------------------------------------------------------
//     9   0x146A  The item was not found at the specified location...out of sync
//    10   0x1463  You cannot add this item because the real estate is full.
//    11   0x1479  There are no items to transfer.
//    12   0x1498  You cannot add this item.  The vault is full.
//    15   0x147A  You cannot add another house to this yard.
//    16   0x147B  You cannot place another moving crate on this property.
//    17   0x1464  You can't put that item in this real estate.
//    19   0x146D  You can only do this with empty containers.
//    20   0x146F  You are too far away from the item.
//    21   0x149B  This item must be placed in the zone for you to interact.
//    22   0x1478  You must have a moving crate placed in your yard or house.
//    23   0x1483  This can only be done by the owner of the property.
//    24   0x1929  ...the upkeep on this plot has expired.
//    25   0x192A  ...the upkeep on the item or house has expired.
//    28   0x1475  You cannot place this on top of a player.
//    29   0x1499  ...the surface is not flat enough.
//    31   0x1924  ...you've reached the limit for that item.
//    32   0x1925  Non-empty houses and crates can only be placed in yards/houses.
//    33   0x1926  ...only one non-empty house and crate in your inventory.
//    35   0x1482  You cannot place that item in this zone.
//    36   0x146E  You can only do this with placeable items.
//    38   0x1928  ...you have reached the pet limit for this yard.
//    39   0x14A2  Moving crates can only contain one non-empty house...
//    42   0x14A3  ...its NPC was not found in the zone.
//    43   0x14AB  ...because it is No Storage.
//    44   0x14AC  ...contains an augment that is No Storage.
//    45   0x14C0  Part of the stack was successfully moved.
//    46   0x14C1  ...no room for the item that would be added from splitting.
//    47   0x14C7  Disabled until you own the House of Thule expansion.
//    48   0x14D3  House swap failed: not enough space for placed items.
//    49   0x14D4  House swap failed: not enough space for archived items.
//    50   0x14D5  House swap failed: not enough space for NPC items.
//    53   0x14D6  House swap failed: you do not have a house placed.
//    55   0x14D7  House swap failed: that house is already placed.
//    59   0x14DA  House swap failed: crate placed in your placed house.
//    64   0x02DB  ...it is no drop and you are not its owner.
//    65   0x02DC  ...it is a lore item that you already have.
//    66   0x02E7  ...you are the owner of the item.
//    67   0x02E8  ...the item contains a real estate.
//    68   0x02ED  You cannot add another guild hall to this yard.
//    70   0x02F8  Must be a guild member with modify trophy access.
//    71   0x151C  ...you do not have add item access.
//    72   0x151D  ...you do not have remove item access.
//    73   0x151E  ...you do not have move item access.
//    74   0x151F  ...you do not have interact item access.
//    75   0x1520  ...you do not have add house access.
//    76   0x1521  ...you do not have remove house access.
//    77   0x1526  ...you do not have add crate access.
//    78   0x1527  ...you do not have remove crate access.
//    79   0x1525  ...you do not have mail item access.
//    81   0x152A  ...house contains a placed moving crate.
//    82   0x1535  Disabled until you own the Veil of Alaris expansion.
//
// Codes 71..79 are the ACCESS refusals and map one-for-one onto nine of the
// thirteen permissions in 43.41 -- which is independent confirmation that the
// permission list is the real model and not just UI labels.
//
// NOTE the codes are NOT contiguous: 1..8, 13, 14, 18, 26, 27, 30, 34, 37, 40,
// 41, 51, 52, 54, 56..58, 60..63, 69, 80 all fall through the switch and produce
// NOTHING AT ALL. A wrong code is silent, not an error -- do not use an unlisted
// value as a "generic failure".
//
// ---------------------------------------------------------------------------
// The purchase-specific refusals live in CanBuy (43.40) and never travel over
// 0x1228; the client raises them locally:
//     0x14C3  You must purchase the House of Thule expansion before you can own
//             a plot.
//     0x1536  You must purchase the Veil of Alaris expansion before you can buy
//             a guild plot.
//     0x191B  You may not purchase a plot that is already owned.
//     0x14C5  Your plot purchase attempt failed because this is a guild
//             neighborhood and you are not a member of the guild.
//     0x02F2  Your guild has not granted you the ability to buy guild plots.
//     0x02F3  ...you are not in a guild!
// CAUTION: one CanBuy branch passes 0x14F through a DIFFERENT string API
// (0x008061C0, not 0x007D0660), so it is NOT an eqstr id -- eqstr 0x14F is "You
// have no skill in alchemy", which is obviously not the message. Do not resolve
// ids across the two APIs.
//
// And the three confirmation dialogs, resolved, which settle what those buttons
// actually promise the player:
//     0x1DD2  Sell:     "...the placed moving crate will be packed with all of
//                       the plot's items and added to your inventory.  Also, all
//                       of the money in the plot's escrow account will be
//                       withrawn and given to you."   [sic, 'withrawn']
//     0x1DD3  Transfer: "Are you sure you wish to transfer your plot to %1?  All
//                       items and money in the escrow will remain with the plot."
//     0x1DD4  0x5A63:   "Do you wish to accept %1 from %2?"  -- so 0x5A63 is an
//                       OFFER/transfer-accept prompt, not an eviction notice as
//                       43.2's unlabelled-branch note guessed.
//     0x1DE2  Vote:     "There are no active polls to vote on."
// ============================================================================

// ============================================================================
// 43.43  THE 0x2698 DEFINITION FIELD MAP, FIELD BY FIELD -- AND f09 IS THE TYPE
//        (2026-07-28)
// ============================================================================
// The definition reader is 0x007F4C90. Read in wire order it writes:
//
//   wire     dword index   byte ofs   note
//   ----     -----------   --------   --------------------------------------
//   f00      [0x00]        0x00       definition id
//   f01      [0x07]        0x1C
//   f02      [0x08]        0x20       PRICE cost-set id     (43.36, confirmed)
//   f03      [0x09]        0x24       UPKEEP cost-set id    (43.36, confirmed)
//   f04      [0x0A]        0x28       capacity              (confirmed in game)
//   f05      [0x0B]        0x2C       vault capacity        (confirmed in game)
//   f06      [0x0C]        0x30
//   f07      [0x0D]        0x34
//   f08      [0x0E]        0x38
//   f09      [0x0F]        0x3C       *** TYPE ***  ctor default 10
//   f10      [0x10]        0x40
//   (string) [0x03..]      0x0C       the name
//   f11      [0x11]        0x44       ctor default 0x150 (336)
//   f12      [0x12]        0x48       pet capacity          (confirmed in game)
//   f13      [0x13]        0x4C
//   f14      [0x14]        0x50
//   f15      [0x15]        0x54
//   f16      [0x16]        0x58
//   count    [0x17]        0x5C       trailing id array count
//   ids...   [0x18]        0x60       the array ([inferred] child definition
//                                     ids -- the client carries the error text
//                                     "Tried to add RealEstateDefinition %d to
//                                     parent %d when it was already there")
// The stored object appends hash-node fields: id at +0x6C, next at +0x70 (what
// DefinitionTypeOf 0x007F4FA0 walks).
//
// Confirmed from both ends: 43.36 pinned the cost-set fields at +0x20/+0x24 by
// experiment and they land on f02/f03 here; and f09 lands on +0x3C, exactly the
// offset DefinitionTypeOf returns -- with the SAME default, 10, in the record
// ctor and in DefinitionTypeOf's miss path.
//
// ALSO NOTE: the reader consumes THREE strings' worth of cleanup at the end of
// 0x007F5B60 but only one string is read here. Sending one
// name and that is correct; the other two slots are simply never populated by
// this message.
//
// ---------------------------------------------------------------------------
// TYPE VALUES  (0x007F6030 "purchasable", 0x007F6280 "is guild plot")
// ---------------------------------------------------------------------------
//     1, 2, 3, 5, 6, 7, 10, 11   -> NOT purchasable
//     4                          -> PURCHASABLE, player plot
//     8                          -> PURCHASABLE, guild plot (0x007F6280 is
//                                   true for 8 and nothing else)
//     anything else, incl. 0     -> NOT purchasable (falls out of the switch)
// A definition with type 0 makes CanBuy fail at a branch that returns false
// WITH NO POPUP -- so Buy would look enabled, accept the click, and do nothing.
//
// ---------------------------------------------------------------------------
// SO WHY DID CLICKING BUY APPEAR TO DO NOTHING?  (live state, read 2026-07-28)
// ---------------------------------------------------------------------------
// 0x005B3540 turned out NOT to be a per-call helper at all -- it is the
// singleton accessor, RealEstateManagerClient::Instance(), taking no arguments.
// Ghidra shows it "with different arg counts" everywhere only because the
// caller's pushes for the NEXT call sit in front of it. Its storage:
// (RealEstateManagerClient__Instance_x 0x5B3540 is already defined far above --
//  what is new is that it is a MAGIC-STATIC accessor and where it stores the
//  instance:)
#define pinstRealEstateManagerClientSingleton_x                      0x00E64570
//
// Reading the live client through that pointer while standing on a plot:
//     vftable  0x009D910C   (matches the ctor analysis in 43.22 exactly)
//     +0x0C    2            record count
//     +0x78    non-null     the cost-set map
//     +0x8C    9009         the occupancy root set by 0x6FD0
//     +0x90    4            DefinitionTypeOf(definition 100) == 4, PURCHASABLE
//     +0x98    9009         current estate
//     +0xA4    0            no request pending
// and record 9009 itself: definition_id 100, owner-name string EMPTY,
// +0x24 == -1.
//
// Walk CanBuy (43.40) against that state:
//     1. mgr[0xA4] clear                                   -> pass
//     2. id resolves in the record map                     -> pass
//     3. type 4 passes 0x007F6030                          -> pass
//     5. expansion bits: the branch needs 0x10000 (House of Thule) for a player
//        plot / 0x20000 (Veil of Alaris) for guild. With World:ExpansionSettings
//        = 524287 = 0x7FFFF, both are set -> pass
//     6/7. 0x005B41D0 "already owned" needs a NON-EMPTY owner name (player) or
//        +0x24 != -1 (guild). Here empty and -1                -> pass
//     9. 0x007F6280(4) is false -> CanBuy returns TRUE
//
// EVERY GATE PASSES. The conclusion, and it is an inference rather than a
// capture: the client DID send 0x45BE, and it arrived as OP_Unknown because the
// server has no handler. The "window just closed" is not a failure at all --
// the sender's own tail call is  purchaseWnd->vt[0xD8](0, 1, 1), i.e.
// CXWnd::Show(false), which HIDES the plot window the moment the request goes
// out. The server's 0x45BE ack is what opens the Manage window in its place
// (43.39). Packet logging was off at that moment, which is why the request was
// never seen going out.
// The client expects the server to answer 0x45BE with 0x45BE {byte 1} + 0x63FD {record}
// per 43.39.
//
// STILL UNIDENTIFIED: f01, f06, f07, f08, f10, f11 (non-zero default 336),
// f13..f16. f11 is the only one with a non-zero default and is worth probing
// first.
// ============================================================================

// ============================================================================
// 43.44  RealEstatePurchaseWnd::SetFromRecord (0x0074DCB0) -- EXACTLY WHICH
//        FIELD DRIVES EACH CONTROL  (2026-07-28)
// ============================================================================
// This is the function 0x63FD calls when the arriving record is the estate you
// are standing in (43.39). It is the single place the plot window gets its
// contents, so it is the definitive answer to "what do we have to send for the
// window to look right".
//
// It bails out entirely, drawing nothing, if either:
//     mgr[0x74] == 0                          (the definitions map is empty)
//     0x007F4F70(record[0x20]) == 0           (no DEFINITION with that id)
// -- so a record whose definition_id was never sent renders as a blank window,
// not as an error.
//
//   window member          <- source
//   ---------------------  ---------------------------------------------------
//   [0x228] estate id      <- record[0x14]
//   [0x23C] Capacity       <- definition[0x28]  = f04
//   [0x240] Vault Capacity <- definition[0x2C]  = f05
//   [0x244] Npc Capacity   <- definition[0x48]  = f12, and if NEGATIVE the
//                             client falls back to definition[0x28]
//   [0x248] Category       <- definition[0x40]  = f10, rendered through
//                             0x004866C0(id, 0x1B) -- i.e. *** f10 IS A DBSTR
//                             ID, NOT A NUMBER ***, type 0x1B
//   [0x260] Type           <- "Guild" if DefinitionTypeOf == 8, else "Player"
//   [0x264] Owner          <- record[0x2C] (the owner-name string) for a player
//                             plot; for a guild plot, the guild name looked up
//                             from record[0x24]. Empty string -> the "vacant"
//                             placeholder at 0x009CA544.
//   [0x268] Guild screen   <- shown only when the plot's NEIGHBOURHOOD ancestor
//                             carries a guild id (walked by 0x004D6790)
//   [0x26C] Guild name     <- that ancestor's [0x24]
//   [0x234] Price combo    <- PopulateCostCombo(definition[0x20], combo, -1)
//   [0x238] Upkeep combo   <- PopulateCostCombo(definition[0x24], combo,
//                                               record[0x3C])
//                             i.e. the UPKEEP combo preselects a value carried
//                             on the RECORD at +0x3C, the PRICE combo never
//                             preselects.
//
//   window state bytes, all computed here:
//   [0x22C] canBuy        = !AlreadyOwned(record) && the price combo actually
//                           got at least one entry.  *** This is 43.36's
//                           "Buy needs an owner-less plot AND a price", in the
//                           client's own code. ***
//   [0x22D]               = !canBuy && (0x004FF6C0(0xC, record, me) || isOwner)
//   [0x22E] isOwner       = !canBuy && (owner-or-GM || guild-owner-match)
//   [0x22F]               = [0x22E] && the type is NOT a guild plot
//
//   and those bytes drive the buttons:
//     Purchase [0x24C]  enabled = canBuy,   shown = ![0x22E]
//     Sell     [0x250]  shown   = [0x22E]
//     Manage   [0x258]  shown   = AlreadyOwned(record)
//     Transfer [0x25C]  shown   = [0x22F]
//
// THAT IS A THIRD INDEPENDENT CONFIRMATION of the control map in 43.38: the
// name assigned by the reverse-order string match, the behaviour seen in
// WndNotification, and now the DATA SOURCE, all agree for every control.
//
// AlreadyOwned is 0x005B41D0(record):
//     player plot -> true when record[0x2C] is a NON-EMPTY string
//     guild plot  -> true when record[0x24] != -1
// Nothing else marks a plot as owned. So to flip a plot to "sold" the server
// only has to resend the record with the owner name filled in.
//
// PRACTICAL: to make a plot render fully we must send, in the DEFINITION,
// f04 capacity, f05 vault, f12 pet capacity, f10 a valid dbstr category id, and
// f09 = 4 or 8; and in the RECORD, the id, the definition id, the address
// string and the owner-name string. Everything else on that window is derived.
// ============================================================================

// ============================================================================
// 43.45  FIVE MORE OPCODES DECODED -- THE CHARACTER'S OWNED-ESTATE ID LISTS
//        (2026-07-28)
// ============================================================================
// 43.2 filed 0x2AA7 / 0x3D94 / 0x21EC etc. as "window / world-object feeds".
// They are not world-object feeds. 0x007C9AA0 / 0x007C9B10 / 0x007C9B80 and
// 0x007BFB10 / 0x007BFB90 are add-unique / remove helpers over THREE plain
// id arrays that live on the local character object (the one 0x0058C9A0
// returns), and the client keeps the player's real-estate ids in them:
//
#define LocalPC__OwnedEstates_A_COUNT_OFS                           0x29AC
#define LocalPC__OwnedEstates_A_ARRAY_OFS                           0x29B0
#define LocalPC__OwnedEstates_B_COUNT_OFS                           0x29BC
#define LocalPC__OwnedEstates_B_ARRAY_OFS                           0x29C0
#define LocalPC__OwnedEstates_C_COUNT_OFS                           0x29CC
#define LocalPC__OwnedEstates_C_ARRAY_OFS                           0x29D0
//
// These are the lists behind the save keys the client writes out --
// "OwnedRealEstatesList=" / "OwnedRealEstatesCount" and
// "OwnedItemRealEstatesList=" / "OwnedItemRealEstatesCount" (0x00A02624,
// 0x00A02660, 0x00A03C1C..0x00A03C6C) -- and therefore behind the My Plots /
// My Items buttons on the neighborhood and plot-search windows.
//
//   opcode  form                                        effect
//   ------  ------------------------------------------  ---------------------
//   0x2AA7  dword count, then count x dword id          add-unique to LIST A
//   0x3E00  dword count, then count x dword id          add-unique to LIST B
//   0x66DF  dword count, then count x dword id          add-unique to LIST C
//   0x76C6  dword id at +0x00, byte flag at +0x68       flag!=0 add to LIST A,
//                                                       flag==0 remove from A
//   0x0C13  dword id at +0x00, byte flag at +0x68       flag!=0 add to LIST B,
//                                                       flag==0 remove from B
//
// All five additionally require the local character to exist, and the three
// bulk ones first call 0x005B4060 / 0x005B40A0 / 0x005B40E0 respectively --
// [inferred] a clear of the matching list, which would make them FULL
// REPLACEMENTS rather than increments. Not verified; check before relying on
// it. 0x2AA7 and 0x76C6 also set the dirty
// byte at [0x00F75AD8]+0x28C.
//
// 43.2's names for these (WindowFeedC / WindowFeedD / etc.) are wrong and
// should not be used. 0x3D94, which 43.2 also listed, is an explicit no-op in
// the dispatcher -- it returns immediately.
//
// ============================================================================

// ============================================================================
// 43.46  0x0004 / 0x485D / 0x3968 CARRY THE ACCESS TABLE -- WHICH IS WHAT
//        43.21b BLAMED ON 0x2FA0  (2026-07-28)
// ============================================================================
// The 0x48-byte record these three opcodes build is deserialized by 0x005B7A90,
// and its shape is unmistakable:
//
//      for (i = 0; i < 7; i++) {          // SEVEN fixed slots
//          dword -> record[i*8 + 0];
//          dword -> record[i*8 + 4];
//      }                                  // 7 * 8 = 0x38 bytes
//      dword count;
//      repeat count times {
//          string  name;                  // length-prefixed, 0x0080C9B0
//          dword   value;
//          insert (name -> value) into the map at record + 0x38;
//      }
//
// SEVEN fixed slots. 43.41 read seven role levels out of the client's own
// strings -- Owner, Co-Owner, Trustee, Friend, Acquaintance, Visitor, Everyone.
// A seven-entry fixed array followed by a NAME -> VALUE map is exactly an access
// table: per-role defaults, then per-character overrides.
// [inferred, but on strong evidence -- the count matches the role list exactly
// and the map is keyed by name, which is what REMW_Name_List / REMW_Name_EditBox
// / REMW_Access_Combobox / REMW_Permissions_Checkbox operate on.]
//
#define RealEstateAccess__SIZE                                      0x48
#define RealEstateAccess__ROLE_SLOTS                                7
#define RealEstateAccess__ROLE_STRIDE                               8
#define RealEstateAccess__NAME_MAP_OFS                              0x38
#define RealEstateAccess__Deserialize_x                             0x005B7A90
//
// The dispatcher then does, for 0x0004:  deserialize -> 0x005B59A0 (a hash-map
// REMOVE by key; it unlinks the node, frees it and decrements the counts -- do
// not read the 43.27 note "add record" as literal) -> 0x005B78D0 (the insert)
// -> 0x007465F0 to refresh the Manage window. 0x485D is the same with
// 0x005B7DE0 as the update step and 0x007465C0 as the refresh.
//
// *** THIS IS THE ANSWER 43.21b WAS LOOKING FOR AND MISATTRIBUTED. *** The
// Access tab, and the Add/Remove/Permissions controls on the Manage window, are
// fed by 0x0004 / 0x485D -- not by any reply to 0x2FA0, which is not even a
// real-estate opcode (43.37). Escrow and upkeep are still unaccounted for and
// are the remaining unknown on that window; the likely carriers are the ones we
// have not yet opened, 0x087F (0x005B0E30) and 0x1CEF (0x005B0D00).
//
// NOT YET KNOWN: what the two dwords in each role slot mean. The obvious guess
// is (permission bitmask, something) with the thirteen permissions of 43.41 as
// the bits, but nothing has been read that confirms it -- do not build on it
// without opening the Manage window's populate path first.
// ============================================================================

// ============================================================================
// 43.47  RealEstateManagerClient -- THE FOUR COLLECTIONS, AND 0x1CEF / 0x087F
//        (2026-07-28)
// ============================================================================
// Pulling 43.22, 43.40 and this pass together, the manager owns four separate
// collections and it matters which one a given opcode feeds:
//
#define RealEstateManagerClient__RECORDS_BUCKETS_OFS                0x04
#define RealEstateManagerClient__RECORDS_MODULUS_OFS                0x08
#define RealEstateManagerClient__RECORDS_COUNT_OFS                  0x0C
//      node layout: [+0x00] RealEstate*, [+0x04] key, [+0x08] next
//      fed by 0x63FD / 0x6FD0 / 0x4F09 (vt[0x24] add, vt[0x28] remove)
//
#define RealEstateManagerClient__SUMMARY_BUCKETS_OFS                0x64
#define RealEstateManagerClient__SUMMARY_MODULUS_OFS                0x68
//      node layout: [+0x00] object*, [+0x04] key, [+0x08] next
//      objects are 0x28 bytes, allocated by 0x005AB230(mgr, id)
//      fed by 0x1CEF and 0x087F -- see below
//
//      RealEstateManagerClient__DEFINITIONS_OFS (0x74) is already defined above.
//      It is a pointer to the definition catalog; DefinitionTypeOf (0x007F4FA0) and
//      the by-id lookup (0x007F4F70) take THAT object as `this`, not the
//      manager. SetFromRecord refuses to draw anything if mgr[0x74] is null.
//      fed by 0x2698.
//
//      the cost-set map is already defined above as
//      RealEstateManagerClient__COSTSET_MAP_OFS (0x78); fed by the cost block
//      of 0x2698. Do not add a second define for it.
//
// plus the scalars already known: [0x8C] gate, [0x90] current type, [0x98]
// current estate, [0xA4] request-pending.
//
// ---------------------------------------------------------------------------
// 0x1CEF -- one plot summary
// ---------------------------------------------------------------------------
//      dword   id
//      ...     read by 0x005AC050 into the 0x28-byte object
// find-or-create by id in the SUMMARY map, fill it, and on success refresh BOTH
// the neighborhood window (0x00730880) and the purchase window (0x007410F0).
// If 0x005AC050 fails, a freshly created object is destroyed again and
// 0x005B4680 is called on the id instead -- which is the same function 0x7D28
// dispatches to, so 0x7D28 is [inferred] "forget this summary".
//
// ---------------------------------------------------------------------------
// 0x087F -- one plot summary WITH a name and N sub-records
// ---------------------------------------------------------------------------
//      dword   id
//      string  name
//      dword   count
//      count x <sub-record read by 0x005ABD20>
// Same find-or-create against the SUMMARY map. This is the richer form and is
// the likely carrier for the neighborhood / plot-search listings.
//
// NEITHER of these is the escrow feed. Escrow and upkeep remain the one part of
// the Manage window with no identified source; 43.46 accounted for the Access
// tab, and these two account for the plot listings. What is left unclaimed in
// the S->C table (43.39) after all of this is 0x1097, 0x1967, 0x1BAF, 0x2CF7,
// 0x66BF, 0x68BA and 0x2AA6 -- start there.
// ============================================================================

// ============================================================================
// 43.48  THE LAST FOUR SUMMARY OPCODES, AND THE HOUSING MESSAGE VOCABULARY
//        (2026-07-28)
// ============================================================================
// All four operate on the manager's SUMMARY map (43.47, mgr+0x64/+0x68) and all
// four start with a dword id and do nothing at all if that id is not already in
// the map. So they are UPDATES to a summary 0x1CEF or 0x087F created earlier --
// send them first or these are silently discarded.
//
//   0x1097   dword id, then 0x005AB4B0(stream)      general summary update
//   0x2CF7   dword id, then 0x005ABD20(stream, 0)   ONE sub-record -- the same
//                                                   reader 0x087F loops over, so
//                                                   0x2CF7 appends a single item
//                                                   to a summary's list
//   0x1967   dword id, then a STRING at body+0x04   -> 0x005AB030, a rename
//   0x1BAF   dword id, dword value                  -> if 0x00556DC0(id,value)
//                                                      the in-progress item
//                                                      placement is cancelled
//                                                      with string 0x1468,
//                                                      "Item placement has been
//                                                      cancelled because the
//                                                      item being placed has
//                                                      been removed from the
//                                                      real estate."
//                                                      then 0x005AB910(value).
// 0x1BAF is therefore an ITEM-REMOVED notification, and the second dword is the
// real-estate ITEM id, not an estate id.
//
// Still completely unopened after tonight: 0x66BF (0x005AD930) and 0x68BA
// (0x005ADAD0), both single-dword; and the S->C side of 0x2AA6 (0x00741670,
// Manage window). Everything else in the 43.39 table has a decoded shape.
//
// ---------------------------------------------------------------------------
// THE MESSAGE VOCABULARY -- and the three lines a purchase should produce
// ---------------------------------------------------------------------------
// Separate from the 0x1228 result codes (43.42), the client ships ordinary
// formatted-message strings for housing -- 188 of them in eqstr_us.txt.
// The three that belong to the purchase flow:
//
//      0x191C  6428  "You spend %1 %B2(18) to purchase the plot."   item cost
//      0x191D  6429  "You spend %$1 to purchase the plot."          coin cost
//      0x191E  6430  "Plot successfully purchased."
//
// -- which incidentally CONFIRMS the two-currency cost model of 43.35 from a
// completely independent direction: the client has one purchase-confirmation
// line for coin and a different one for an item currency, exactly matching the
// cost sub-entry's `a` field being 0 for coin and an alternate_currency id
// otherwise.
//
// Others worth knowing before the upkeep/escrow work starts:
//      0x191F  "You do not have enough funds in escrow to cover the upkeep for
//               %1... the item will be moved back to the shed or closet after
//               the next upkeep charges are applied."
//      0x1920  "You cannot enter %1 because the upkeep on it has expired."
//      0x1923  "You already have a non-empty moving crate in your inventory or
//               bank so you may not sell this plot."
//      0x1927  "That operation may not be performed because the target player is
//               logged in.  Use "/worldkick %1" first."   (transfer)
// ============================================================================

// ============================================================================
// 43.49  *** EVERY PLOT OPERATION NOW HAS ITS REPLY CHANNEL ***  (2026-07-28)
// ============================================================================
// 0x66BF and 0x68BA were the last two "single dword, unknown" S->C branches.
// They are both result channels, and they pair one-to-one with the two C->S
// requests we already had. With 0x45BE (43.39) that closes the round trip for
// all three plot operations:
//
//      C->S 0x45BE  purchase   ->  S->C 0x45BE  { byte, non-zero = success }
//      C->S 0x792C  sell       ->  S->C 0x66BF  { dword result code }
//      (transfer)              ->  S->C 0x68BA  { dword result code }
//
// Both new channels take a single dword and, for a non-zero code, sprintf an
// eqstr string and put it on screen through 0x0051F1A0(text, 0xD, 1, 1).
// Code 0 is silence, i.e. success.
//
// ---------------------------------------------------------------------------
// 0x66BF -- SELL / MOVE OUT result (the answer to 0x792C)
// ---------------------------------------------------------------------------
//    0   success, no message
//    1   0x149C  ...the plot does not contain a placed crate with enough space
//    2   0x149D  ...because you are not its owner
//    3   0x1923  You already have a non-empty moving crate in your inventory or
//                bank so you may not sell this plot
//    4   0x02EF  You may not sell a guild plot if you are not a member of its
//                guild!
//    5   0x14A1  ...the placed crate cannot add another house containing items
//    6   0x152B  ...you do not have permission to remove the crate
//    7   0x14C9  You must either empty all of the items from this plot or
//                purchase the House of Thule expansion before you can move out
//    8   0x1537  You may not sell a guild plot until you purchase the Veil of
//                Alaris expansion
//   10   0x02F0  ...it has funds in escrow and you do not have withdraw upkeep
//                access
//   11   0x152C  ...the active moving crate on it ...
//   12   0x152D  ...the active moving crate on it ...
//   13           uses the OTHER string API (0x008061C0), NOT an eqstr id --
//                do not try to resolve it against eqstr_us.txt
//   any other    0x149E  "Your attempt to sell this plot has failed."
//
// ---------------------------------------------------------------------------
// 0x68BA -- TRANSFER result
// ---------------------------------------------------------------------------
//    0   success, no message
//    2   0x149F  ...because you are not its owner
//    3   0x1DD5  ...because there is a NO TRADE house or moving crate in the
//                plot
//   any other    0x14A0  "Your attempt to transfer this plot failed."
//
// NOTE what this implies about TRANSFER. 43.21b established that the Transfer
// button emits no packet -- it only raises confirm dialog 0x1DD3 (43.42). Yet
// the client has a dedicated result channel for it. So the transfer request is
// sent from the CONFIRM CALLBACK, exactly as Sell's 0x792C is, and it must be
// one of the C->S opcodes in the 43.37 table we have not yet attributed. It is
// NOT 0x796C -- that turned out to be the plot list's Go button. The remaining
// candidates in the real-estate window range are 0x3146 (0x00747FDC) and
// 0x218C (0x0074A7E8, 14 bytes); and note a transfer needs a target NAME, which
// neither of those is big enough to carry inline, so the name is probably set
// by an earlier message or the request carries a character id instead.
//
// PRACTICAL: a server that answers 0x45BE with {1}, 0x792C with 0x66BF {0} and
// the transfer request with 0x68BA {0} gets correct, silent success on all
// three; and every failure mode already has a written, player-facing sentence
// that costs one dword to send.
// ============================================================================

// ============================================================================
// 43.50  0x3146 IS THE ACCESS-TABLE SAVE, AND 0x2AA6 IS THE MANAGE-WINDOW
//        COMMAND CHANNEL  (2026-07-28)
// ============================================================================
// ---------------------------------------------------------------------------
// 0x3146  C->S, variable length -- SAVE THE ACCESS TABLE
// ---------------------------------------------------------------------------
// Its payload is built by 0x00749420, and that function is the EXACT MIRROR of
// the 0x0004 / 0x485D reader in 43.46:
//
//      for (i = 0; i < 7; i++) { write dword obj[i*8+0]; write dword obj[i*8+4]; }
//      write dword obj[0x40];                    // the entry count
//      walk the hash map at obj[0x38] (obj[0x3C] buckets):
//          for each node: write string node->name; write dword node->value;
//
// Read side and write side agree field for field, which upgrades 43.46 from
// "[inferred] access table" to certain: seven fixed role slots of two dwords
// each, then a name -> value map. So the object is
//
#define RealEstateAccess__NAME_MAP_BUCKETS_OFS                      0x38
#define RealEstateAccess__NAME_MAP_NBUCKETS_OFS                     0x3C
#define RealEstateAccess__NAME_MAP_COUNT_OFS                        0x40
#define RealEstateAccess__Serialize_x                               0x00749420
#define OP_RealEstate_SaveAccess                                    0x3146   // C->S, variable
//
// The round trip for access is therefore complete and needs nothing further
// discovered:  S->C 0x0004 (or 0x485D) pushes the table, the player edits it in
// the Manage window, C->S 0x3146 sends the whole thing back.
//
// Note the send path: if the serialized payload exceeds 0x1FE bytes the client
// heap-allocates size+2 instead of using the static 0x00DD00E0 buffer. That is
// the general variable-length idiom and it is worth recognising -- the fixed
// `push <SIZE>` shape of 43.37 does NOT appear for these, which is why the
// opcode scan reports size = -1 for them.
//
// ---------------------------------------------------------------------------
// 0x2AA6  C->S and S->C, variable length -- THE MANAGE COMMAND CHANNEL
// ---------------------------------------------------------------------------
// Six C->S sites. Three of them are inside RealEstateManageWnd::WndNotification
// (0x00745090) and each writes a different leading dword before the same two
// fields:
//
//      dword   subcommand      2, 4 or 5 depending on which control was clicked
//      dword   real_estate_id  <- ALWAYS manager[0x98], the estate you are in
//      dword   value           <- combo->GetItemData(combo->GetCurChoice())
//      ...     (more, serialized)
//
// A fourth site is at 0x0074DA60, which is INSIDE the plot window's notification
// (43.38) in the branch that fires when the UPKEEP COMBO changes. So the same
// channel carries "set my upkeep plan" from the plot window and the Add Day /
// Add Week / Withdraw All style commands from the Manage window.
//
// The S->C direction is 0x00741670, gated on the Manage window existing, so the
// server answers on the same opcode.
//
// WHICH subcommand is which control is not yet pinned -- the notification is a
// 780-line switch and the sends sit in different arms. SIX distinct subcommand
// values appear in it: 2, 3, 4, 5, 6 and 7. What is known so far:
//      control +0x2FC  -> subcommand 2   (and +0x2FC is a CComboWnd)
//      control +0x318  -> subcommand 7
//      control +0x31C  -> subcommand 3
//      control +0x2F8  -> the arm containing subcommands 5 and 6
//      subcommand 4 is reached further down the same arm
//
// To help place them, the live ManageWnd's member types were read out of the
// running client (vftable -> RTTI):
//      +0x260 +0x268                      CCheckBoxWnd
//      +0x270 +0x274 +0x278 +0x310 +0x314 CListWnd
//      +0x290 +0x294 +0x2F8 +0x318 +0x31C
//      +0x320 +0x324                      CButtonWnd
//      +0x2FC                             CComboWnd
// and the REMW_ name list in 43.41 contains exactly 6 lists, 4 combos, 11
// buttons and 1 checkbox, so the offsets above are a partial slice, not the
// whole control table. Enumerate every member and match counts before naming
// any of them -- the reverse-order string trick that worked for the plot window
// (43.38) is NOT safe here, because the REMW_ string block has four coin-icon
// texture names (A_CopperCoin .. A_PlatinumCoin) embedded in the middle of it
// that are not window members and would shift the whole mapping by four.
//
// That one function is the next thing to read if escrow/upkeep is the goal, and
// it is ONE function, not a hunt.
//
// ---------------------------------------------------------------------------
// 0x218C  C->S, 14 bytes -- from the NEIGHBORHOOD window
// ---------------------------------------------------------------------------
//      +0x00  dword  window[0x228]   (the selected neighborhood / plot id)
//      +0x04  dword  0xFFFFFFFF      hardcoded
//      +0x08  dword  0               hardcoded
// The two constants are literals in the code, not variables, so this is not the
// transfer request in disguise. [inferred] a "show me this neighborhood" query.
// ============================================================================

// ============================================================================
// 43.51  *** THE PLACEMENT / ITEMS SURFACE -- RealEstateItemsWnd DECODED ***
//        (2026-07-28)
// ============================================================================
// This is the window that places furniture. Its notification is 0x0073CC50 and
// its constructor 0x00733E00; the whole translation unit is 0x00733000..0x00741000.
//
// THE CONTROL TABLE IS PROVEN, NOT INFERRED. The 44 REIW_ names of 43.41 were
// matched against the LIVE window object read out of the running client, by
// resolving each member's vftable to its RTTI class. Five consecutive groups
// agree on their COUNTS exactly, and the table starts and ends where the name
// list does:
//
//   +0x2A4..+0x2C4   9  labels      the 7 item-count labels, SelectedItemLabel,
//                                   SelectedItemLocation
//   +0x2C8           1  CButtonWnd  REIW_SelectedItem (an item slot)
//   +0x2CC..+0x2EC   9  CCheckBoxWnd  the nine Show* filters, in order:
//                                   ShowStored, ShowPlaced, ShowHouse, ShowYard,
//                                   ShowMovingCrate, ShowPlaceable,
//                                   ShowUnplaceable, ShowOwnedItems,
//                                   ShowUnownedItems
//   +0x2F0..+0x314  10  CButtonWnd  Move_Inventory, Move_Closet, Move_Shed,
//                                   Move_Crate, Place, Locate, Stow_Placed,
//                                   Pack_Moving_Crate, Unpack_Moving_Crate,
//                                   Trophies
//   +0x318           1  (NULL)      unaccounted -- no matching name
//   +0x31C           1  CListWnd    REIW_ItemList
//   +0x320           1  CListWnd    REIW_LayoutList
//   +0x324           1  CEditWnd    REIW_LayoutName_EditBox
//   +0x328..+0x344   8  CButtonWnd  LayoutSaveAll, LayoutSaveYard,
//                                   LayoutSaveHouse, LayoutLoadAll,
//                                   LayoutLoadYard, LayoutLoadHouse,
//                                   LayoutDetails, LayoutDelete
//   +0x348..+0x350   3  CCheckBoxWnd  LayoutUsePlacedItems,
//                                   LayoutUseInventoryItems,
//                                   LayoutStoreExtraItems
//   +0x354           1  CComboWnd   REIW_LayoutRotateYard_Combobox
//
#define RealEstateItemsWnd__WndNotification_x_43_51                  0x0073CC50
#define RealEstateItemsWnd__SHOW_FILTERS_FIRST_OFS                   0x2CC
#define RealEstateItemsWnd__MOVE_INVENTORY_BTN_OFS                   0x2F0
#define RealEstateItemsWnd__MOVE_CLOSET_BTN_OFS                      0x2F4
#define RealEstateItemsWnd__MOVE_SHED_BTN_OFS                        0x2F8
#define RealEstateItemsWnd__MOVE_CRATE_BTN_OFS                       0x2FC
#define RealEstateItemsWnd__PLACE_BTN_OFS                            0x300
#define RealEstateItemsWnd__LOCATE_BTN_OFS                           0x304
#define RealEstateItemsWnd__STOW_PLACED_BTN_OFS                      0x308
#define RealEstateItemsWnd__PACK_CRATE_BTN_OFS                       0x30C
#define RealEstateItemsWnd__UNPACK_CRATE_BTN_OFS                     0x310
#define RealEstateItemsWnd__TROPHIES_BTN_OFS                         0x314
#define RealEstateItemsWnd__ITEM_LIST_OFS                            0x31C
#define RealEstateItemsWnd__LAYOUT_LIST_OFS                          0x320
#define RealEstateItemsWnd__LAYOUT_NAME_EDIT_OFS                     0x324
#define RealEstateItemsWnd__LAYOUT_SAVE_ALL_BTN_OFS                  0x328
#define RealEstateItemsWnd__LAYOUT_ROTATE_COMBO_OFS                  0x354
//
// ---------------------------------------------------------------------------
// THE C->S PLACEMENT OPCODES  (from the 43.37 scan, restricted to this TU)
// ---------------------------------------------------------------------------
//   opcode  size  sites                            what
//   0x6917    2   7354E3                           no body; sent on dialog
//                                                  response 100 = "Stow all
//                                                  placed items" (confirm text
//                                                  0x1462)
//   0x613D    3   7355B0 735684                    one BYTE body, 1 or 0
//   0x21BA   26   735789                           {-1, 0, then 4 dwords}
//   0x5B41    ?   73CAEA                           variable length
//   0x2036   46   73CF80 73EA6A 73EF63             *** MOVE ITEM TO INVENTORY ***
//   0x3424   50   73D1A5 73D329 73E96C 73F15B      *** MOVE ITEM, with a
//                 73F2C2                           destination selector at +0x2C:
//                                                  3 = closet, 2 = shed,
//                                                  6 = moving crate
//   0x626E   62   73D4AF 73D5F3 73E85E             *** SAVE LAYOUT ***
//                                                  (Save All / Yard / House)
//   0x218C   14   73E752                           also sent by the neighborhood
//                                                  window (43.50)
//   0x7AD1  150   73EB3E
//   0x58C7   10   73ECE8                           note: 0x58C7 is ALSO an S->C
//                                                  opcode (a chat message) --
//                                                  the same number is used in
//                                                  both directions
//   0x3C8B  354   73F505                           sent from the LayoutName edit
//                                                  box arm; the big one
//
// The five senders at 0x0050xxxx are NOT placement opcodes -- that address range
// is general item/movement code, not the EQPlacedItemManager. Checked against
// patch_RoF2.conf, which already names three of them, and the names are plainly
// unrelated to housing:
//   0x747C  121   500FF1        OP_SomeItemPacketMaybe (already in the conf)
//   0x7DFC   48   507D93        OP_ClientUpdate        (already in the conf)
//   0x2EB3   18   50958C        OP_UnderWorld          (already in the conf)
//   0xCADF    6   506583        unnamed; one dword body, argument straight through
//   0x65C7    2   506EAD        unnamed; no body
// Listing them here at all was the address-proximity mistake again (43.37). They
// are recorded only so nobody re-derives them as housing opcodes.
//
// ---------------------------------------------------------------------------
// THE SHARED "ITEM OPERATION" STRUCT (0x2036 / 0x3424 / 0x626E)
// ---------------------------------------------------------------------------
// All three build one struct on the stack and differ only in how much of it
// they send -- 44, 48 and 60 body bytes respectively. Known fields:
//
//   +0x00 .. +0x10   *** NOT A NAME -- CORRECTED 2026-07-28 FROM A LIVE CAPTURE ***
//                    A 16-character zero-padded DECIMAL string plus its NUL: the
//                    item's SERIAL NUMBER. *** DECIMAL, NOT HEX. *** It is the
//                    exact string the SERVER put in the item header -- EQEmu's
//                    rof2.cpp writes fmt::format("{:016}", inst->GetSerialNumber())
//                    into hdr.unknown000, and {:016} is zero-padded decimal. The
//                    client stores it and echoes it back verbatim.
//                    Parsing it base-16 turns serial 25 into 37 and every lookup
//                    misses; that mistake is how the field was confirmed.
//                    Observed "0000000000000012" = 12 and "0000000000000025" = 25.
//                    THIS IS THE WHOLE ITEM DATA MODEL: the client identifies the
//                    item by a SERIAL the server already owns, NOT by an index into
//                    a list the server had to send first. Deposit is therefore
//                    implementable with no item-list feed at all.
//                    The earlier "string copy of the selected item's name from
//                    item->[0]+0xD8" reading was inferred from the disassembly and
//                    is wrong -- the capture shows hex digits, not a name.
//   +0x14   dword    window[0x250]
//   +0x18   dword    window[0x254]   (the selected ITEM INDEX; the notification
//                                     reads it as a signed short and bounds-
//                                     checks it against window[0x230])
//   +0x1C   dword    window[0x258]
//   +0x20   dword    item[+0x08]
//   +0x24   dword    item[+0x0C]
//   +0x28   dword    (a pointer-derived value)
//   +0x2C   dword    DESTINATION  (0x3424)
//                    *** RESOLVED 2026-08-02: 2 = CLOSET, 3 = SHED, 6 = CRATE. ***
//                    Straight from the WndNotify at 0x0073CE00-region (items window):
//                    dest DEFAULTS to 3; the Closet button (wnd+0x2F4) overrides to
//                    2; the Crate button (wnd+0x2FC) to 6. The old "2 shed/3 closet"
//                    guess was swapped -- this note's own suspicion ("3 may well be
// shed") was correct, confirmed by decompile.
//   +0x30   dword    1
//   +0x34   dword    global [0x00DDF684]
//   +0x38   dword    global [0x00DDF688]
//
// The two globals are read straight from memory at send time, which is the
// signature of a CURSOR / TARGET POSITION pair. Confirming that is the obvious
// next probe.
//
// The item accessor is  0x0073F800(index) -> item record, with item[+0x10] a
// "valid" byte, item[+0x00] a pointer to the underlying object, and the display
// name at object+0xD8.
//
// ---------------------------------------------------------------------------
// THE THREE CONFIRM DIALOGS AND THEIR RESPONSE CODES
// ---------------------------------------------------------------------------
// Stow / Pack / Unpack do NOT send directly. Each raises a confirmation and the
// packet goes out from the response handler, keyed by the dialog's response id:
//
//   Stow_Placed  (+0x308)  text 0x1462  response 100 (0x64)
//        "Are you sure you wish to move all of the placed items at your current
//         location into storage?"
//   Pack_Crate   (+0x30C)  text 0x1476  response 101 (0x65)
//        "...pack the active moving crate with all of your real estate items at
//         the current location that can fit in the moving crate?"
//   Unpack_Crate (+0x310)  text 0x1477  response 102 (0x66)
//        "...unpack all of the items in your active moving crate that are
//         allowed in your current location?"
//
// The response handler is the `param_2 == 100 / 0x65 / 0x66 / 0x67` chain at the
// top of 0x00735480, and it is unambiguous in the decompile:
//      response 100 (0x64) -> 0x6917, 2 bytes, NO BODY          = stow all
//      response 101 (0x65) -> 0x613D, body byte = 1             = PACK crate
//      response 102 (0x66) -> 0x613D, body byte = 0             = UNPACK crate
//      response 103 (0x67) -> 0x21BA, 26 bytes:
//              +0x00 dword  0xFFFFFFFF
//              +0x04 dword  0
//              +0x08 dword  obj[+0x34]
//              +0x0C dword  obj[+0x38]
//              +0x10 dword  window[+0x164]
//              +0x14 dword  window[+0x168]
//          Two pairs of values taken from two different objects; [inferred] a
//          source/destination or a coordinate pair. Its confirm dialog was not
//          identified -- it is the fourth arm and has no button in the control
//          table above, so it is probably raised from the item list's context
//          menu rather than a button.
#define OP_RealEstate_StowAllPlaced                                 0x6917  // C->S, 2 bytes, no body
#define OP_RealEstate_PackUnpackCrate                               0x613D  // C->S, 3 bytes, byte 1=pack 0=unpack
#define OP_RealEstate_MoveItemToInventory                           0x2036  // C->S, 46 bytes
#define OP_RealEstate_MoveItem                                      0x3424  // C->S, 50 bytes, dest at +0x2C
#define OP_RealEstate_SaveLayout                                    0x626E  // C->S, 62 bytes
//
// Trophies (+0x314) sends nothing -- it toggles the trophy window at
// [0x00F75AD8] through CXWnd::Show, the same vt+0xD8 slot as 43.39.
// ============================================================================

// ============================================================================
// 43.52  *** THE OPCODE SCAN, VALIDATED AGAINST patch_RoF2.conf --
//        AND 170 CLIENT MESSAGES EQEmu HAS NO NAME FOR ***  (2026-07-28)
// ============================================================================
// This section is NOT about housing. It is the general-purpose result that fell
// out of the 43.37 method once it was run over the whole binary.
//
// THE VALIDATION FIRST, because it settles whether any of this can be trusted.
// The scan says the client can send 449 distinct opcodes. Cross-checking those
// against EQEmu's utils/patches/patch_RoF2.conf:
//
//        279 of the 449 are ALREADY NAMED in the conf.
//
// and the names are right, not coincidental -- 0x0ECB is OP_RespawnWindow and
// the scan found it inside RespawnWnd's notification; 0x08A6 is OP_PopupResponse
// and 43.21b captured it live as exactly that; 0x0971 is OP_SpawnAppearance;
// 0x0BCF OP_MoveCoin; 0x0ADF OP_LootRequest; 0x0DDD OP_ShopPlayerBuy. Two
// completely independent sources -- a decade of EQEmu packet work and a static
// scan of the client's own code -- agree on 279 values. The method is sound.
//
// It also retro-corrects two of my own guesses tonight: 0x0971 (which 43.37
// listed as a possible real-estate manager opcode, flagged [unverified]) is
// OP_SpawnAppearance, and the 0x0050xxxx senders 0x747C / 0x7DFC / 0x2EB3 are
// OP_SomeItemPacketMaybe / OP_ClientUpdate / OP_UnderWorld, not placement.
//
// ---------------------------------------------------------------------------
// THE REMAINING 170
// ---------------------------------------------------------------------------
// 170 opcodes that a STOCK RoF2 client will send, with sizes and call sites,
// that patch_RoF2.conf has never had a name for. Every one of them is a client
// feature that currently arrives at an EQEmu server as OP_Unknown and is dropped.
//
//
// *** READ THE DISTANCE COLUMN. *** It is a heuristic and nothing more. Section
// 43.37 and 43.51 both record the same mistake made twice: address proximity is
// NOT attribution. A distance over ~0x1000 means the label is worthless, and
// even a small distance only tells you which translation unit you are in. The
// `WndNotify=` comments) or a decompile of the containing function.
//
// The scan needs nothing but the module base and a byte scan; no debugger, no
// running client, no packet capture.
// ============================================================================

// ============================================================================
// 43.53  *** THE COMPLETE SERVER->CLIENT DISPATCH MAP -- AND PROOF THAT ALL 28
//        REAL-ESTATE OPCODES ARE REACHABLE ***  (2026-07-28)
// ============================================================================
// CEverQuest::HandleWorldMessage (0x004C3250) keeps the incoming opcode in
// [ebp-0x5360] and dispatches with a HYBRID scheme. Both halves must be
// harvested or the result is badly wrong:
//
//   (a) direct comparisons, a binary-search tree --   586 cases
//         81 BD <disp32> <imm32>      cmp [ebp+disp32], OPCODE  (or 83 BD .. imm8)
//         0F 84 <rel32>               je  <case body>           (or 74 rel8)
//   (b) jump-table leaves for dense clusters --       120 cases
//         2D <imm32> OR 81 EA <imm32>     sub reg, BASE
//         81 BD <disp32> <imm32>          cmp ..., COUNT
//         0F 87 <rel32>                   ja  default
//         0F B6 8x <idxmap>               movzx reg, byte [reg + idxmap]
//         FF 24 8x <table>                jmp [table + reg*4]
//
//   704 distinct opcodes handled in total.
//
// Each opcode was recorded with its case body and its conf name.
//
// VALIDATION, the same test as 43.52: 345 of the 704 are already named in
// patch_RoF2.conf and the names are right -- 0x5089 OP_ZoneEntry, 0x7291
// OP_SpawnDoor, 0x5070 OP_TimeOfDay, 0x700C OP_RemoveAllDoors, 0x72DF
// OP_ReadBook. The remaining 359 are messages this client will ACT ON that
// patch_RoF2.conf has never had a name for.
//
// *** TWO MISTAKES THIS SECTION EXISTS TO PREVENT. I MADE BOTH TONIGHT. ***
//
// FIRST: scanning only the jump tables finds 93 opcodes and misses the direct
// compares entirely, including OP_RealEstateResult (0x1228) -- a direct compare
// at 0x004C38A6 that is KNOWN to work in game (43.9). A known-good case that the
// extraction cannot explain is the cheapest possible check; run it every time.
//
// SECOND, and worse because it looked like it had already worked: hard-coding
// ONE stack displacement and ONE `sub` encoding. The first full pass matched
// only `cmp [ebp-0x5360]` and `81 EA`, reported a confident 425 opcodes, and was
// short by 279. The function uses at least two stack slots for the opcode and
// both the `2D` and `81 EA` forms of the subtract. Match the OPERATION, not the
// exact bytes you happened to see in the first example.
//
// The housing conclusion below survived both errors unchanged -- the group is
// still exactly 28 -- but the totals did not. If any other section disagrees
// with 704 / 345 / 359, that section is the older one.
//
// ---------------------------------------------------------------------------
// *** ALL 28 REAL-ESTATE OPCODES SHARE ONE CASE BODY ***
// ---------------------------------------------------------------------------
// Every one of these jumps to 0x004CC1D7, which calls
// RealEstateManagerClient::HandleRealEstateMessage (43.27/43.39):
//
//    0x0351 0x087F 0x0C13 0x1097 0x1228 0x1967 0x1BAF 0x1CEF 0x21EA 0x21EC
//    0x2691 0x2698 0x2CF7 0x3968 0x3D94 0x3E00 0x45BE 0x485D 0x4F09 0x58C7
//    0x5A63 0x63FD 0x66BF 0x66DF 0x68BA 0x6FD0 0x76C6 0x7D28
//
// Every opcode the handler's internal switch understands is genuinely
// reachable from an EQEmu server -- the
// purchase ack 0x45BE, the record push 0x63FD, the sell result 0x66BF, the
// transfer result 0x68BA, the access table 0x0004/0x485D. None of them depend on
// a client change, and none of them can be swallowed on the way in.
//
// It also CORRECTS 43.1, which said "THREE of its cases funnel into one
// real-estate handler" and listed the three jump-table slots 0x004D433C /
// 0x004D477C / 0x004D4C7C. Those three slots are real -- a scan for the case
// body address finds exactly them and nothing else -- but they only account for
// the jump-table half. The other 24 opcodes get there by direct compare. Read
// 43.1's "three" as "three jump-table slots", not "three opcodes".
//
// NOTE 0x0351 and 0x3D94 are on the list even though the handler treats them as
// explicit no-ops (43.39), and 0x58C7 appears in BOTH directions -- the client
// also sends 0x58C7 from the items window (43.51). An opcode number being used
// for two different things in the two directions is normal here; do not assume
// symmetry.
// ============================================================================

// ============================================================================
// 43.54  0x2691 REPLY -- THE TYPE BYTE ROUTES IT, AND ONE FORM OPENS THE PLOT
//        WINDOW WITH NO DOOR CLICK  (2026-07-28)
// ============================================================================
// Reader: 0x005B0F80. It begins by consuming a dword it throws away, then reads
// ONE BYTE and hands it to IsPurchasableType (0x007F6030, 43.43 -- true only for
// type 4 and type 8). That byte decides which of two completely different
// payload shapes follows.
//
// ---------------------------------------------------------------------------
// FORM A -- type byte is NOT a purchasable type: a LIST FILL
// ---------------------------------------------------------------------------
//      dword   (discarded)
//      byte    type
//      dword   target
//          target == 4 or 5  -> RealEstatePlotSearchWnd  (0x00D1FCC4) via 0x0074C4C0
//          target == 3       -> dropped
//          anything else     -> RealEstateNeighborhoodWnd (0x00D1FCC0) via 0x0074AB50
//      ...     the rest is consumed by whichever of those two readers ran
// Both readers require their window to EXIST -- a reply aimed at a closed window
// is silently discarded.
//
// ---------------------------------------------------------------------------
// FORM B -- type byte IS purchasable (4 or 8): ONE PLOT RECORD
// ---------------------------------------------------------------------------
//      dword   (discarded)
//      byte    type          (4 player plot / 8 guild plot)
//      dword   (discarded)
//      word    autoOpen      *** non-zero is half of the auto-open condition ***
//      dword   real_estate_id
//      dword   a
//      dword   b
//      dword   c
//      dword   d
//      string  s1
//      string  s2
//      string  s3
//      string  s4
//      dword   (discarded) x4
//      byte    flag          *** the other half of the auto-open condition ***
//
// If the id is ALREADY in the manager's record map the reader takes an update
// path: 0x005B4FE0(record, &s4) and mgr->vt[8](record, c). Otherwise it
// allocates 0x58 bytes and builds the record through
//
#define RealEstate__ConstructFromFields_x                           0x007FE1E0
//
//      RealEstate::ConstructFromFields(id, 0, a, b, c, d, &s3, &s4, 0, &..., 0, 0, 0)
//
// -- note this is a DIFFERENT constructor from the deserializer path of 43.28:
// it takes every field as an argument instead of pulling them off a stream. Then
// mgr->vt[0x24](record) adds it, the same add 0x63FD and 0x6FD0 use.
//
//   *** AND THEN, IF autoOpen != 0 AND flag != 0 AND THE PLOT WINDOW EXISTS: ***
//
//      RealEstatePurchaseWnd::SetFromRecord(record);   // 0x0074DCB0
//      purchaseWnd->vt[0xD8](1, 1, 1);                 // CXWnd::Show(true)
//
// SO THE PLOT WINDOW CAN BE OPENED PURELY BY ANSWERING A SEARCH. The other
// route is the lot-square door click (43.x), which needs the player
// physically standing on the plot. This one needs neither a door nor an
// occupancy gate -- send a Form B 0x2691 with both flags set and the window
// appears, populated, wherever the player is standing.
//
// That is worth having for two reasons: it makes the plot window testable
// without a housing zone at all, and it is presumably how the live game's
// "Plot Search -> Go" flow showed you a plot you were not standing on.
//
// CAUTION: SetFromRecord still refuses to draw anything unless the record's
// definition id resolves in the catalog (43.44), so 0x2698 must have been sent
// first. The window will open EMPTY otherwise, which looks like a different bug.
// ============================================================================

// ============================================================================
// 43.55  THE RealEstate RECORD, COMPLETE -- CONFIRMED BY A SECOND CONSTRUCTOR
//        (2026-07-28)
// ============================================================================
// 43.28 mapped the record by reading its DESERIALIZER. 0x007FE1E0 builds the
// same object from explicit arguments, and the two agree everywhere they
// overlap -- which is the strongest form of confirmation available without a
// debugger. It also fills in three fields the deserializer never touches.
//
//   +0x00   vftable (0x00A1052C)
//   +0x08   node, self-linked; +0x20 and +0x21 set to 1
//   +0x0C   0
//   +0x14   real_estate_id
//   +0x18   ?
//   +0x1C   PARENT real-estate id
//   +0x20   DEFINITION id
//   +0x24   ?              (guild id on a guild plot -- 43.44 tests it against -1)
//   +0x28   ?
//   +0x2C   OWNER NAME     (CXStr, refcounted)
//   +0x30   ADDRESS        (CXStr, refcounted)
//   +0x34   ?
//   +0x38   CXStr, refcounted
//   +0x3C   *** the UPKEEP cost-entry preselect *** -- SetFromRecord (43.44)
//           passes exactly this to PopulateCostCombo for the upkeep combo
//   +0x40   ?
//   +0x44   byte     (the opt-out flag: 0x005B09D0 writes the checkbox state here)
//   +0x48   child count
//   +0x4C   child id array
//   +0x54   byte, 1
//   size    0x58
//
// So a record the server sends can preselect the player's upkeep plan by putting
// the right cost-entry key at +0x3C -- the field 43.32 called "no observed
// effect" turns up for the third time (43.40 in the purchase request, 43.44 in
// the combo, here in the record).
//
// ---------------------------------------------------------------------------
// CORRECTION TO 43.54's Form-B FIELD ORDER
// ---------------------------------------------------------------------------
// Now that the constructor's parameter order is known, the 0x2691 Form B record
// maps like this (the ctor is called as ConstructFromFields(id, 0, a, b, c, d,
// &s3, &s4, 0, &s?, 0, 0, 0)):
//      dword a  -> +0x1C   parent real-estate id
//      dword b  -> +0x20   DEFINITION id
//      dword c  -> +0x24
//      dword d  -> +0x28
//      string   -> +0x30   the ADDRESS
//      string   -> +0x2C   the OWNER NAME
// -- i.e. the same address-before-owner inversion 43.28 found in the
// deserializer. Two different code paths, the same surprising order; it is real.
//
// ---------------------------------------------------------------------------
// LAYOUT SCOPE CONSTANTS
// ---------------------------------------------------------------------------
// The three Save-layout buttons differ only in a scope value ORed together from
// two constant functions:
#define RealEstate__LayoutScopeA_x                                  0x007F62B0  // returns 0x88
#define RealEstate__LayoutScopeB_x                                  0x00410C30  // returns 0x06
//      LayoutSaveAll  (+0x328)  ->  0x88 | 0x06  = 0x8E
//      LayoutSaveYard (+0x32C)  ->  0x88
//      LayoutSaveHouse(+0x330)  ->  0x06
// 6 is the same value the Move-to-crate button puts in the 0x3424 destination
// field, so these are members of one shared real-estate LOCATION enum rather
// than a layout-specific flag. The individual meanings are NOT established --
// do not assume 0x88 means "yard" just because the Yard button uses it; the
// button-to-constant pairing above is what was read, and the button order comes
// from the name-list alignment of 43.51.
//
// ---------------------------------------------------------------------------
// RealEstateLayoutDetailsWnd (0x005AD040 notification, 0x005AC730 ctor)
// ---------------------------------------------------------------------------
// A small window with three interesting controls:
//      +0x238   a list; its selection is handled with param_4 != 0
//      +0x23C   selection -> stored at this+0x22C
//      +0x240   a checkbox; its state (ctl[0x79] as a byte) -> this+0x229, then
//               0x0073BE30(this[0x230], thisState, this+0x22A)
//      +0x244   a second checkbox -> this+0x22A, same call with the arguments
//               swapped
// 0x0073BE30 is the layout-preview refresh, taking (layout, flagA, flagB) --
// the two checkboxes are almost certainly the "use placed items" / "use
// inventory items" pair that also exist on the Items window (43.51).
// ============================================================================

// ============================================================================
// 43.56  THE TWO 0x2691 LIST READERS (Form A)  (2026-07-28)
// ============================================================================
// Form A of 0x2691 (43.54) hands the rest of the stream to one of two readers
// depending on the `target` dword. Both are count-prefixed row lists and both
// silently do nothing if their window is closed.
//
// ---------------------------------------------------------------------------
// target 4 or 5 -> RealEstatePlotSearchWnd, reader 0x0074C4C0
// ---------------------------------------------------------------------------
//      word    row_count
//      string  header        -> set as the window's title (vt+0x124)
//      row_count x {
//          dword   real_estate_id
//          dword   TYPE          -- the reader branches on == 4 or == 8, the
//                                   same player/guild plot types as 43.43
//          dword   ?
//          string  ?
//          string  ?
//      }
// The type dword selects which of two row layouts is built for the list, so a
// row with a type that is neither 4 nor 8 takes the other path.
//
// ---------------------------------------------------------------------------
// any other target -> RealEstateNeighborhoodWnd, reader 0x0074AB50
// ---------------------------------------------------------------------------
//      word    row_count
//      row_count x {
//          byte    flag
//          dword   -> window[0x8B]
//          ...
//      }
// (Only partially walked; the row body is longer and feeds several list
// columns. Finish this one before implementing the neighborhood browser.)
//
// PRACTICAL: the plot-search list is the cheaper of the two to implement and it
// is what the Plot Search window and its Go button need. Combined with 43.54's
// Form B -- which opens the plot window outright -- a server can drive the
// entire "browse plots, pick one, look at it, buy it" flow without the player
// ever standing on a lot square.
// ============================================================================

// ============================================================================
// 43.57  SUBSYSTEM MAP OF THE UNIMPLEMENTED CLIENT -- BY SHARED CASE BODY
//        (2026-07-28)
// ============================================================================
// Opcodes that share a case body in HandleWorldMessage belong to ONE feature.
// Grouping the 425 S->C opcodes (43.53) that way turns 215 anonymous numbers
// into a handful of named subsystems. This is far more productive than chasing
// opcodes one at a time, and it is how player housing was found in the first
// place -- 28 opcodes on a single body.
//
//   case body   n   unnamed   what it is
//   ---------  --   -------   ---------------------------------------------
//   004CC1D7   28      24     PLAYER HOUSING (43.39 onward)
//   004C53A6   20      12     MERCENARIES -- twelve opcodes EQEmu does not name
//   004C6EC5   16       7     target / NPC-mark / delegate-ability
//   004C5FB6   13       3     guild tribute
//   004C594D   11      11     *** NOTHING. See below. ***
//   004C6382    9       1     tasks / shared tasks
//   004C5FD1    9       0     tribute (the part EQEmu already drives)
//   004C6427    4       0     bazaar / trader
//   004C5FE8    4       4     TRIBUTE, the missing half -- decoded below
//   004C5389    4       4     VOICE CHAT (Vivox) -- decoded below
//   004C865C    2       2     a name-vs-name comparison, then a branch
//   004C64C7    2       1     item preview (0x2B10 special-cased)
//   004CD3C2    2       2     unknown
//
// ---------------------------------------------------------------------------
// *** 004C594D IS A NO-OP. DO NOT IMPLEMENT THESE. ***
// ---------------------------------------------------------------------------
// The case body is one instruction:  JMP 0x004CD5E0  -- the common switch exit.
// So these ELEVEN opcodes are explicitly accepted and thrown away by the client:
//
//      0x01D6 0x0D07 0x11F3 0x1DA2 0x272F 0x413F 0x5147 0x5182 0x51F8
//      0x68B2 0x6F2B
//
// It is the single largest unnamed group in the client and it does nothing at
// all. Worth knowing before someone spends a night on it -- a big group is not
// automatically a big feature.
//
// ---------------------------------------------------------------------------
// 004C5FE8 -- THE MISSING TRIBUTE OPCODES  (all four decoded)
// ---------------------------------------------------------------------------
// The case body is  mov ecx, instTribute_x (0x00ABFCD0);  call 0x00485990.
// instTribute_x is ALREADY NAMED in this file, which is what identified the
// group. The handler manages TEN slots, addressed by 0x007C4920(index), each
// holding { tribute_id, tier }:
//
//   0x08BF   no meaningful body. Clears all ten slots, refreshes each, and sets
//            charInfo+0x2DD4 = 1 / charInfo+0x2F6C = 0.  = TRIBUTE RESET
//   0x4B65   +0x00 dword slot (0..9, bounds-checked)
//            +0x04 dword tribute_id   -- if -1, the slot is REMOVED
//            +0x08 dword tier
//            = SET ONE TRIBUTE SLOT
//   0x51AE   +0x00 dword -> charInfo+0x20C8   = a single tribute counter/timer
//   0x7697   the bulk form: ten ids followed by ten tiers, written to slots
//            0..9 with a refresh per slot.  = SET ALL TRIBUTE SLOTS
//
// EQEmu already drives OP_TributeInfo / OP_TributeItem / OP_SendTributes /
// OP_SelectTribute, but those are a DIFFERENT case body (0x004C5FD1). These four
// are the half nobody has ever sent.
//
// ---------------------------------------------------------------------------
// 004C5389 -- VOICE CHAT  (0x22B8, 0x2B57, 0x5512, 0x7BFF)
// ---------------------------------------------------------------------------
// The case body calls 0x005D5230(opcode, buf, len) and then 0x005D3700 on the
// result. That function carries the strings
//      "Failed to create voice channel."
//      "Accept voice call from %s?"
//      "Your voice tell request was accepted."
//      "Your voice tell request was denied."
// 0x7BFF reads THREE 256-byte fixed strings and passes them to 0x005D2FE0 --
// [inferred] a channel name, a URI and a credential, i.e. the Vivox session
// handoff. The RoF2 client has a complete voice-chat client built in and EQEmu
// has never spoken a word of it.
//
// ---------------------------------------------------------------------------
// HOW TO CONTINUE THIS
// ---------------------------------------------------------------------------
// 1. Take an unnamed group from the S->C opcode dump.
// 2. Decompile its case body -- it is usually 3-8 instructions ending in a call.
// 3. Look up the callee's globals in THIS FILE first. instTribute_x is what
//    named the tribute group in one step; guessing from behaviour would have
//    taken hours.
// 4. If the body is a bare JMP to 0x004CD5E0, the opcodes are ignored; move on.
// ============================================================================

// ============================================================================
// 43.58  TWO MORE SUBSYSTEMS OPENED: MERCENARIES (13 missing opcodes) AND RAID
//        DELEGATION  (2026-07-28)
// ============================================================================
// ---------------------------------------------------------------------------
// MERCENARIES -- case body 0x004C53A6
// ---------------------------------------------------------------------------
// The body is  call 0x00569B00 (lazy-create the manager)  then  call 0x00569920
// (dispatch). The manager is a 0x2DC-byte singleton:
#define pinstMercenaryManager_x                                     0x00E63E88
#define MercenaryManager__Instance_x                                0x00569B00
#define MercenaryManager__Dispatch_x                                0x00569920
//
// 0x00569920 is a clean opcode switch with a DISTINCT handler per message --
// twenty of them. patch_RoF2.conf names seven; the other thirteen are messages
// this client will act on that no EQEmu server has ever sent:
//
//   opcode  handler     conf name
//   ------  ----------  ------------------------------
//   0x038F  0x00567890  <<none>>
//   0x0763  0x00567CA0  OP_MercenaryTimer
//   0x0E52  0x00567920  <<none>>
//   0x1B37  0x005680C0  <<none>>
//   0x20B9  0x00567A20  <<none>>
//   0x2749  0x005677E0  <<none>>
//   0x27A0  0x00567590  OP_MercenaryUnsuspendResponse
//   0x27F2  0x00567EA0  <<none>>
//   0x29EC  0x00565890  <<none>>
//   0x2B4F  0x00567650  <<none>>
//   0x3CBC  0x00567FF0  <<none>>
//   0x4333  0x00565560  <<none>>
//   0x5D26  0x00567DF0  OP_MercenaryUnknown1
//   0x5DF8  0x00565320  <<none>>
//   0x5F88  0x00567710  <<none>>
//   0x61A4  0x005684A0  OP_MercenaryDataUpdate    (takes buf AND len)
//   0x69CA  0x005662F0  <<none>>
//   0x6E83  0x00567C00  OP_MercenaryDismiss       (takes no payload at all)
//   0x6F03  0x00567500  OP_MercenarySuspendResponse
//   0x7169  0x005695B0  OP_MercenaryHire
//   0x72CE  0x00568670  OP_MercenaryDataResponse  (takes buf AND len)
//
// Note 0x27F2 does not appear in the 43.53 group listing -- it reaches the
// dispatcher by another route. When enumerating a subsystem, read the
// subsystem's OWN switch as well as the outer one; they do not always agree.
//
// Only three handlers take a length argument (0x61A4, 0x72CE, and the case body
// itself); the rest take a bare buffer, which means they are FIXED-SIZE structs.
// That is useful: a fixed-size handler cannot be probed incrementally, it either
// matches or it reads garbage.
//
// ---------------------------------------------------------------------------
// RAID DELEGATION / MARK NPC -- case body 0x004C6EC5
// ---------------------------------------------------------------------------
// Sixteen opcodes, seven unnamed. The body is guarded twice before it will do
// anything: an expansion check (bit 0x40, via 0x004106C0 on the local character)
// AND a singleton at
#define pinstRaidDelegationManager_x                                0x00E63B58   // [inferred name]
// then  call 0x00560260(this, opcode, len, buf).
//
// The feature is identified by its own messages, which resolve cleanly:
//      0x2248 "%1 has been delegated Raid Main Assist."
//      0x2249 "%1 has been delegated Raid Mark NPC."
//      0x224A "%1 has been delegated Raid Mark Waypoint."
//      0x2242 / 0x2243 / 0x2244  the matching "is no longer delegated" lines
// -- so this is raid leadership DELEGATION plus the NPC/waypoint marking that
// goes with it. Already-named members include OP_MarkNPC, OP_MarkRaidNPC,
// OP_ClearNPCMarks, OP_RaidClearNPCMarks, OP_DelegateAbility,
// OP_RaidDelegateAbility, OP_SetGroupTarget, OP_TargetHoTT and OP_InspectBuffs;
// the seven unnamed ones (0x0EB4, 0x1C89, 0x4015, 0x4786, 0x4C9D, 0x4EE2,
// 0x6261) are siblings of those.
//
// CAUTION, the 43.42 trap again: one string id in this handler (0x14A) goes
// through the OTHER string API and resolves against eqstr_us.txt to a forge
// message, which is obviously wrong. Only ids passed to 0x007D0660 are eqstr
// ids.
// ============================================================================

// ============================================================================
// 43.59  SUBSYSTEM MAP, REDONE ON THE FULL 704-OPCODE HARVEST  (2026-07-28)
// ============================================================================
// 43.57's table was built on the incomplete 425-opcode extraction (see the
// correction in 43.53). Everything it says is still true, but it was missing
// whole subsystems. This is the table to use.
//
//   case body   n   what it is
//   ---------  --   ---------------------------------------------------------
//   004CC1D7   28   PLAYER HOUSING           (43.39 onward)
//   004C53A6   20   MERCENARIES              (43.58; 13 unnamed)
//   004C6EC5   17   raid delegation / marks  (43.58)
//   004D1D6F   16   GUILD MEMBER LIST / ranks / notes / banker flags
//   004D233F   13   SHARED TASKS             (invite, member list, changes)
//   004C5FB6   13   guild tribute
//   004D22E2   12   ADVENTURES (LDoN)        (finish, update, stats)
//   004C594D   11   *** nothing -- a bare JMP to the switch exit ***
//   004D2320   10   DYNAMIC ZONES / EXPEDITIONS (DzChooseZone, lockouts, ...)
//   004CF656   10   GROUPS                   (delete, disband, leader change)
//   004C6382    9   tasks
//   004C5FD1    9   tribute (the half EQEmu already drives)
//   004D2964    8   *** all unnamed ***
//   004D36BB    6   *** all unnamed ***
//   004C5FE8    4   tribute, the missing half (43.57, all four decoded)
//   004C5389    4   VOICE CHAT / Vivox       (43.57)
//   004C6427    4   bazaar / trader
//
// The two all-unnamed groups at 0x004D2964 (8 opcodes) and 0x004D36BB (6) are
// now the largest unexamined features in the client, after 0x004C594D which is
// confirmed dead. Take those next.
// ============================================================================

// ============================================================================
// 43.60  *** FURNITURE: 0x1045 PLACES AN OBJECT, AND HERE IS ITS WIRE FORMAT ***
//        (2026-07-28)
// ============================================================================
// EQPlacedItemManager::Add (0x00506030) has exactly ONE caller in the whole
// client: a case body inside HandleWorldMessage at 0x004D0940. Tracing which
// opcode reaches it gives the answer the placement work has needed:
//
#define OP_RealEstate_PlaceObject                                   0x1045  // S->C
#define OP_RealEstate_UpdatePlacedObject                            0x2A9E  // S->C
//
// Neither is in patch_RoF2.conf. 0x1045 lands at case body 0x004D080A and
// 0x2A9E at 0x004D0998, both as direct `cmp/je` cases and both in a group of
// one. 43.24 recorded the placed-object collection as permanently EMPTY; it is
// empty because nothing had sent 0x1045.
//
// ONE CAVEAT ON EXCLUSIVITY. Those two case bodies are not the only code that
// jumps into that region -- 0x004CD8F9 jumps to 0x004D080A and 0x004CDCEF to
// 0x004D0998, from the post-switch tail. So some other path may share the same
// handling. That does not weaken anything below: the code at 0x004D080A
// demonstrably allocates an EQPlacedItem, deserializes it and adds it to the
// manager, and 0x1045's own je points straight at it. Both facts stand on their
// own. It only means "0x1045 is the ONLY way to place an object" is one claim
// too strong -- say "0x1045 places an object".
//
// The 0x1045 case body, in order:
//      p = operator new(0xC0);  EQPlacedItem::ctor(p)          // 0x00504460
//      stream = MakeStream(buf, len)                            // 0x00410C40
//      EQPlacedItem::Deserialize(p, &stream)                    // 0x00505400
//      if (0x004B3940(p)) p->[0x98] = 1                         // a validation
//      existing = EQPlacedItemManager::Instance()->GetItemByGuid(p + 0x10)
//      if (existing) existing->UpdateFrom(p)                    // 0x00504FD0
//      else          EQPlacedItemManager::Add(p)                // 0x00506030
// -- so the object is keyed by a GUID STRING at +0x10, and re-sending the same
// GUID updates in place instead of duplicating. That is the natural "move this
// piece of furniture" path.
//
// ---------------------------------------------------------------------------
// 0x2A9E -- REMOVE A PLACED OBJECT  (case body 0x004D0998)
// ---------------------------------------------------------------------------
// Far simpler, and it does NOT use the GUID string:
//      id     = body[0x20]        real_estate_id
//      itemId = body[0x24]        real_estate_item_id
//      p = EQPlacedItemManager::Instance()->GetItemByRealEstateAndRealEstateItemIds(id, itemId)
//      if (p) { Instance()->Remove(p) (0x005061A0); ...; 0x008B8540(body) }
// So the pair (real_estate_id, real_estate_item_id) is the SERVER-side identity
// of a placed object and the GUID string is the client-side one. Both must be
// kept consistent, and the two lookup helpers that use them were already in this
// file as EQPlacedItemManager__GetItemByGuid_x and
// EQPlacedItemManager__GetItemByRealEstateAndRealEstateItemIds_x.
//
// ---------------------------------------------------------------------------
// EQPlacedItem::Deserialize (0x00505400) -- THE PAYLOAD
// ---------------------------------------------------------------------------
#define EQPlacedItem__Deserialize_x                                 0x00505400
#define EQPlacedItem__Ctor_x                                        0x00504460
#define EQPlacedItem__UpdateFrom_x                                  0x00504FD0
#define EQPlacedItem__SIZE                                          0xC0
//
//   NUL-TERMINATED string, TRUNCATED TO 63 CHARS   -> +0x3C
//        the actor/model tag ("IT23335" style, per the layout XML of 43.5).
//        NOTE this one is C-string terminated, NOT length-prefixed like every
//        other string in the real-estate protocol. Do not send a length dword.
//   byte    -> +0x2C
//   dword   -> +0xBC
//   dword   -> +0x0C          (ctor seeds -1)
//   NUL-TERMINATED string -> +0x10   *** THE ITEM KEY ***
//        0x00505A50 is not a GUID reader -- it scans for a NUL, measures the
//        length and stores it as a small-string-optimised std::string (inline
//        below 0x10 chars, heap above). The case body then passes (item + 0x10)
//        by POINTER to GetItemByGuid, so the "GUID" this collection is keyed by
//        is a STRING, not a number.
//   dword   -> +0x24          real_estate_id       (ctor seeds -1)
//   dword   -> +0x28          real_estate_item_id  (ctor seeds -1)
//   dword   -> +0x7C          *** ctor seeds 1.0f ***
//   dword   -> +0x80
//   dword   -> +0x84
//   dword   -> +0x88
//   dword   -> +0x8C
//   dword   -> +0x90
//   dword   -> +0x94
//   byte    -> +0x98          (also force-set to 1 if the validation passes)
//   byte    -> +0x99
//   byte    -> +0x9A
//   dword   -> +0x9C
//   dword   -> +0xA0 ... +0xB8   (seven more dwords; +0xA8 also seeds 1.0f)
//
// +0x24 and +0x28 land EXACTLY on EQPlacedItem__REALESTATEID_OFS and
// EQPlacedItem__REALESTATEITEMID_OFS, which were already in this file from
// 43.3 -- derived from a completely different function. That is the check that
// the whole mapping is anchored correctly.
//
// THE TRANSFORM. +0x7C..+0x94 are SEVEN CONSECUTIVE DWORDS and the first is
// seeded to 1.0f by the constructor. 43.5's capture of a real plot's layout XML
// lists exactly seven per-item transform fields: Scale, Heading, Pitch, Roll,
// X, Y, Z. Seven fields, seven slots, and the one that defaults to 1.0 is the
// only one that would sensibly default to 1.0.
//   +0x7C Scale, +0x80 Heading, +0x84 Pitch, +0x88 Roll, +0x8C X, +0x90 Y,
//   +0x94 Z -- all floats.
//
// *** THIS IS NO LONGER A GUESS. *** The consumers of EQPlacedItem read those
// fields as floats and use them as a position:
//      0x0051C740-ish:  param3 - *(float*)(item+0x8C)
//                       param4 - *(float*)(item+0x90)
//                       param5 - *(float*)(item+0x94)      a distance test
//      elsewhere:       f(x + *(float*)(item+0x8C),
//                         y + *(float*)(item+0x90),
//                         z + *(float*)(item+0x94), ...)   a 3D point
//      and:             { item+0x8C, item+0x90, item+0x94 } packed into one
//                       struct and passed together with item+0x80 to 0x005024B0
//                       -- position plus heading, the classic pairing.
//      +0x84 and +0x88 are likewise read with float instructions.
// So +0x8C/+0x90/+0x94 are a position TRIPLE in that memory order, +0x80 travels
// with them as the facing, +0x84/+0x88 are the other two angles and +0x7C is the
// scale the constructor seeds to 1.0f. Seven floats, and 43.5's layout XML lists
// exactly seven: Scale, Heading, Pitch, Roll, X, Y, Z.
// The only residual guess is which of +0x84/+0x88 is Pitch and which is Roll.
//
// Remember 43.5's other captured facts, which the server must respect: heading,
// pitch and roll use EQ's native 0-512 angle scale as floats (128 = 90 degrees),
// scale runs about 0.1 to 2.5, and the yard (Type 4) and house interior
// (Type 3) are SEPARATE coordinate spaces.
// ============================================================================

// ============================================================================
// 43.61  THE ITEMS WINDOW'S ITEM IDENTITY, AND 0x7AD1  (2026-07-28)
// ============================================================================
// RealEstateItemsWnd keeps the currently-selected placed item's identity in two
// members, and every C->S message about "the selected item" copies them:
//
#define RealEstateItemsWnd__SEL_REALESTATE_ID_OFS                   0x384
#define RealEstateItemsWnd__SEL_REALESTATE_ITEM_ID_OFS              0x388
//
// -- the same (real_estate_id, real_estate_item_id) pair that 0x2A9E uses to
// find a placed object server-side (43.60) and that
// EQPlacedItemManager__GetItemByRealEstateAndRealEstateItemIds_x takes. So that
// pair is THE identity of a piece of furniture across the whole protocol.
//
//   0x7AD1  C->S, 150 bytes / 148 body:
//        +0x00  dword  real_estate_id       <- window[0x384]
//        +0x04  dword  real_estate_item_id  <- window[0x388]
//        +0x08  140 bytes, explicitly memset to zero
//
// That is the same "id plus a long zero tail" shape as 0x792C (43.21) and
// 0x45BE (43.38). Three unrelated requests using it means the zero tail is a
// shared request STRUCT with optional fields the client never fills, not
// padding -- so do not assume the tail is meaningless when implementing.
//
// ---------------------------------------------------------------------------
// THE ITEMS WINDOW'S CONFIRM-DIALOG DISPATCH
// ---------------------------------------------------------------------------
// Its dialog responses are NOT a simple switch on the response id. The handler
// at 0x0073E150 does:
//
//      switch ( *(dword *)(this + (response - 0x0B) * 0x10) )
//
// i.e. the response id indexes a 0x10-byte-stride TABLE ON THE WINDOW ITSELF,
// and the switch runs on the value stored there. Cases 0..5, 7..9, 0x0B..0x0E,
// 0x11 and 0x13 exist. Case 4 sends 0x2036 and case 5 sends 0x7AD1, both using
// the identity pair above.
//
// PRACTICAL CONSEQUENCE: you cannot predict which packet a confirm dialog will
// produce from the dialog's response id alone -- the window rewrites that table
// before raising the dialog. Any attempt to drive these from outside has to
// read the table, not guess.
// ============================================================================

// ============================================================================
// 43.62  *** 0x2FA0 IS KRONO / MARKETPLACE -- THE 43.37 RETRACTION IS NOW
//        PROVEN, NOT INFERRED ***  (2026-07-28)
// ============================================================================
// 43.37 retracted 43.21b's claim that 0x2FA0 was the real-estate Manage window's
// data request. The argument then was circumstantial: its senders live in an
// unrelated class and are rate-limited to one send per 3000 ms. The S->C side
// now settles it outright.
//
// 0x2FA0 shares case body 0x004D3E79 with 0x1595, 0x25B3 and 0x73A9. The body
// creates a tiny 0xC-byte manager (0x004A68A0) and dispatches at 0x004A6920 --
// which is ALSO one of the six 0x2FA0 senders identified in 43.37, so the same
// class owns both directions. The dispatcher is four lines long and one of them
// is a literal string:
//
//      0x1595  if (charInfo[0x2D28] != body[0]) {
//                  charInfo[0x2D28] = body[0];
//                  ChatMsg("The Krono items in your inventory have been modified!");
//              }
//      0x73A9  charInfo[0x2D24]  = body[0];
//              charInfo[0x2D28] += body[1];
//      0x2FA0  -> 0x004A65A0(body)
//
// It is the KRONO / marketplace currency channel. The 3-second throttle is a
// marketplace poll. It has nothing whatever to do with real estate.
//
#define CharInfo__KRONO_A_OFS                                       0x2D24  // [inferred]
#define CharInfo__KRONO_COUNT_OFS                                   0x2D28  // the count the message text refers to
//
// LESSON, and this is the third time tonight the same one has come up: a packet
// log tells you WHEN a message arrived, never WHOSE it is. Attribution comes
// from the code that handles it. 43.32 (cost format), 43.37 (this) and 43.51
// (the 0x0050xxxx senders) were all the same mistake in different clothes.
//
// ---------------------------------------------------------------------------
// A SECOND DEAD GROUP
// ---------------------------------------------------------------------------
// Case body 0x004D2964 is, like 0x004C594D (43.57), a single JMP to the switch
// exit. Eight more opcodes the client accepts and discards:
//
//      0x1B5D 0x1F6E 0x20D9 0x3042 0x4FFC 0x6E2A 0x740D 0x7AA9
//
// That is 19 opcodes across two groups that do nothing at all. When a group
// looks big and mysterious, disassemble the case body FIRST -- it costs one
// instruction to find out it is dead.
//
// ---------------------------------------------------------------------------
// STILL OPEN, with a strong lead
// ---------------------------------------------------------------------------
// Case body 0x004D36BB (0x15E2, 0x3311, 0x39BB, 0x52B5, 0x6860, 0x7497) builds
// a stream with 0x00410C40 -- the same helper the placed-object path uses
// (43.60) -- then dispatches at 0x005A7A20 to handlers at 0x005A70C0 /
// 0x005A7440 / 0x005A75F0 / 0x005A7840 AND at 0x007FB020 / 0x007FB780.
// Those last two sit inside the SAME address block as the real-estate
// definition and cost readers (0x007FB930 ReadCostEntryIndex, 0x007FBDB0,
// 0x007FC2B0, 0x007FC6B0). So this is a sibling catalog subsystem built from
// the same serialization toolkit as real estate. Identifying it is the most
// promising remaining thread, and it may share format conventions we have
// already decoded.
//
// Case body 0x004D2610 (0x006E, 0x0728, 0x2507, 0x3823, 0x541D, 0x6C04)
// dispatches through a global manager at 0x00B79140 -- a pointer this file does
// not yet name. Naming that global identifies six opcodes in one step, exactly
// as instTribute_x did in 43.57.
// ============================================================================

// ============================================================================
// 43.63  *** THE POLLING / VOTING SYSTEM -- IT IS PART OF HOUSING ***
//        (2026-07-28)
// ============================================================================
// 43.62 left case body 0x004D36BB as "a sibling catalog subsystem, identity
// open". It is the NEIGHBOURHOOD POLL system -- the feature behind
// REPW_Vote_Button and REPW_VotingOptOut_Checkbox on the plot window (43.38).
//
// HOW IT WAS PINNED, and the method is worth reusing. The RTTI sweep of 43.18
// had already recorded six class vftables nobody had placed:
//
//      0x00A10218  PointSystemBase
//      0x00A10234  PollVote
//      0x00A10270  PollAnswer
//      0x00A10290  PollResult
//      0x00A103FC  Poll
//      0x00A10410  PollResults
//
// Searching the module for those vftable CONSTANTS finds every constructor:
//
//      Poll         @ 0x005A7F82, 0x007FA7E2, 0x007FA8AD
//      PollAnswer   @ 0x007F9A96, 0x007F9AB6, 0x007FA36D, 0x007FB085, 0x007FB17B
//      PollResult   @ 0x007F9B46, 0x007FB415, 0x007FB47F
//      PollResults  @ 0x007FAD5F, 0x007FB694
//      PollVote     @ 0x007F99FF
//
// -- and 0x007FB085 / 0x007FB780's neighbourhood and 0x005A7Fxx are EXACTLY the
// handlers case body 0x004D36BB dispatches to. Two independent locators, the
// RTTI names and the dispatcher, landing on the same addresses.
//
// Note also where those vftables sit: 0x00A102xx-0x00A104xx, a few hundred bytes
// below RealEstate's own vftable at 0x00A1052C (43.27). Polls and real estate
// are the same translation unit. This is not a coincidence of naming; the
// feature genuinely belongs to housing.
//
// ---------------------------------------------------------------------------
// THE POLL OPCODES  (all six S->C, none of them in patch_RoF2.conf)
// ---------------------------------------------------------------------------
#define pinstPollManager_x                                          0x00E643EC  // 0x294-byte singleton
#define PollManager__Instance_x                                     0x005A72A0
#define PollManager__Dispatch_x                                     0x005A7A20
//
//   opcode  handler     note
//   ------  ----------  ------------------------------------------------
//   0x15E2  0x005A7440
//   0x3311  0x007FB780  in the PollResults constructor's neighbourhood
//   0x39BB  0x005A70C0
//   0x52B5  0x005A75F0
//   0x6860  0x007FB020  then sets manager[0x28E] = 1 and calls 0x005A7360
//   0x7497  0x005A7840
//
// The dispatcher builds its stream with 0x00410C40 -- the same helper the
// placed-object path uses (43.60) -- so these payloads are ordinary fail-open
// streams like everything else in this protocol.
//
// ---------------------------------------------------------------------------
// WHAT THIS UNBLOCKS
// ---------------------------------------------------------------------------
// 43.38 recorded that the plot window's Vote button calls 0x00795FC0 on
// manager[0x98] (the current estate) and, if it returns false, prints string
// 0x1DE2 "There are no active polls to vote on." -- otherwise it shows the
// window at [0x00D1FD50]. So:
//
//      0x00795FC0  = "does estate N have an active poll?"
//      [0x00D1FD50] = the poll/vote window
//
// Send one of the six opcodes above to populate the poll manager and the Vote
// button starts working. Until then it will always print "no active polls",
// which is correct behaviour and not a bug.
//
// Also note PointSystemBase (0x00A10218, constructed at 0x007F7FE8 / 0x007F804B)
// in the same block. [inferred] the scoring the polls are counted with.
// ============================================================================

// ============================================================================
// 43.64  NEW RESOURCE: RTTI CLASS -> CONSTRUCTOR ADDRESS  (2026-07-28)
// ============================================================================
// The RTTI sweep of 43.18 gave 546 vftable -> class-name pairs, which is only
// half useful: a name with no address does not put you in the code. Searching
// the module for each vftable CONSTANT finds every store of it, i.e. every
// constructor. 400 of the 546 classes are located that way.
//
// This is what identified the polling system in 43.63 in a single step, and it
// is the fastest route from "the client has a class called X" to "here is X's
// code". Use it before decompiling anything by hand.
//
// It also turns up a THIRD RealEstate constructor. The class's vftable
// (0x00A1052C) is stored at:
//
//        0x007FDE4F   <- NOT previously examined
//        0x007FE10C   = RealEstate::ctor, 43.27 (entry 0x007FE100)
//        0x007FE1ED   = RealEstate::ConstructFromFields, 43.54/43.55
//                       (entry 0x007FE1E0)
//
// So there is a constructor at roughly 0x007FDE40 that neither 43.27 nor 43.55
// covers. Given 0x007FDF00 is the deserializer, 0x007FDE40 sits immediately
// before it -- [inferred] a copy constructor or a default one used by the
// deserialize path. Worth ten minutes before anyone assumes the two known ctors
// are the whole story.
//
// CAVEAT on the resource: a "hit" is any dword in the module equal to the
// vftable address. Most are genuine `mov [this], vftable` stores, but some will
// be data (a vftable can appear in another vftable, or in a relocation table).
// Disassemble before trusting a single isolated hit.
// ============================================================================

// ============================================================================
// 43.65  CASE BODY 0x004D2610 = MONSTER MISSIONS / "MONSTER ALT" PROGRESSION
//        (2026-07-28)
// ============================================================================
// The last of the sizeable unidentified groups. Dispatcher 0x00414A90, driven
// off a global manager at 0x00B79140. SEVEN opcodes, none of them in
// patch_RoF2.conf:
//
//   opcode  handler                 note
//   ------  ----------------------  --------------------------------------
//   0x006E  0x00413BC0(buf, len)
//   0x0728  0x004136F0()            takes nothing -- a reset/clear
//   0x2507  0x00413B40(&{buf,len,0})  the fail-open stream idiom
//   0x2EC3  0x00414320(buf, len)
//   0x3823  0x00413D60(buf, len)
//   0x541D  0x004172D0() then 0x00413D60(buf, len)   -- clear, then the same
//                                                        reader as 0x3823
//   0x6C04  0x00413C10(buf, len)
//
// Note 0x2EC3 is in the dispatcher but was not in the outer group listing --
// the same "read the subsystem's own switch too" point as the mercenary
// dispatcher in 43.58.
//
// IDENTIFIED BY ITS OWN ERROR TEXT. Scanning the handler range 0x00413600 ..
// 0x00417600 for pushed string literals gives, among the CRT noise:
//
//        "Monster Alt: did not receive the number of progressions expected"
//        "PROGRESSION"    "BRANCH"    "TEMPLATE"
//
// -- so this is EQ's MONSTER MISSIONS system (the "monster alt" you play as
// during a mission), with a progression tree of templates and branches. It is
// entirely unimplemented on EQEmu.
//
// THE TECHNIQUE, which is cheap and worked three times tonight: once a group's
// dispatcher is known, scan the address range its handlers live in for
// `push <imm32>` where the immediate points at printable ASCII, and read the
// strings. A subsystem almost always names itself in an error message. Filter
// out the CRT boilerplate ("string too long", "list<T> too long", "bad cast",
// "invalid string position", "Attempt to send message %d on a void
// connection.") -- what is left is the feature.
// ============================================================================

// ============================================================================
// 43.66  THE LAST MULTI-OPCODE GROUPS -- AND 0x7273 IS NOT REAL ESTATE
//        (2026-07-28)
// ============================================================================
// After 43.57 / 43.58 / 43.62 / 43.63 / 43.65 only six case bodies with two or
// more unnamed opcodes remained. Two more fall to the same string-scan:
//
// ---------------------------------------------------------------------------
// 0x004D1E84 and 0x004D1EE2 -- THE POINT MERCHANT
// ---------------------------------------------------------------------------
//      0x004D1E84  (0x0D68, 0x27A2, 0x532A, 0x62D6)  -> 0x006FD790
//      0x004D1EE2  (0x3899, 0x6D10)                  -> 0x006FBB20
// both guarded on a global at 0x00F740B8. The handler range carries the whole
// window's element table:
//      "NewPointMerchantWnd", "NewPointMerchant_ItemList",
//      "NewPointMerchant_PurchaseButton", "NewPointMerchant_SellButton",
//      "NewPointMerchant_EquipButton", "NewPointMerchant_DoneButton",
//      "NewPointMerchant_PointsAvailableValue", "OnlyUseable", ...
// -- the LDoN / loyalty POINT MERCHANT. Four of its six opcodes are unnamed.
// The guard global 0x00F740B8 is ALREADY in this file as pinstNewPointMerchantWnd_x
// (line ~246), which is an independent confirmation of both that entry and this
// identification -- two different methods, same address, same feature.
//
// ---------------------------------------------------------------------------
// 0x004D3E27 -- CHAT CHANNELS, *** NOT REAL ESTATE ***
// ---------------------------------------------------------------------------
//      opcodes 0x1C9E, 0x5F17, 0x7273
//      body: call 0x005BC7B0 (create) then 0x005BD640 (dispatch)
// and the surrounding code carries "%s %s #EverQuest" and "Defaults" -- an
// IRC-style channel join. This is the chat-channel client (the thing EQEmu's
// ucs.exe talks to).
//
// *** CORRECTION TO 43.37. *** That section listed 0x7273 among the real-estate
// C->S opcodes, flagged [unverified], on the strength of its senders living at
// 0x005BC9A5 / 0x005BCBB2 / 0x005BCDC5 / 0x005BD348 -- addresses "near" the
// real-estate manager. They are near it and they are not part of it. 0x7273 is
// chat. The three [unverified] entries in 43.37 now resolve as:
//      0x0971  = OP_SpawnAppearance   (43.52)
//      0x7273  = chat channels        (here)
//      0x7EB9  = still unidentified
// Nothing in 43.37's real-estate table above those three lines is affected --
// they were flagged precisely because address proximity is not attribution, and
// that caution was right.
//
// ---------------------------------------------------------------------------
// WHAT IS LEFT
// ---------------------------------------------------------------------------
// Only three case bodies with 2+ unnamed opcodes remain unidentified, and none
// of them makes a call in the case body itself (they inline their work), which
// is why the dispatcher-plus-string-scan method does not reach them:
//      0x004CD3C2  (0x3036, 0x3BD3)   -- 0x3036 is special-cased inline
//      0x004C865C  (0x3F84, 0x6E5C)   -- compares two names byte by byte
//      0x004C64C7  (0x0E48, 0x2B10)   -- 0x2B10 special-cased, reads body[0x44]
// Everything else unnamed is a singleton opcode, which the group method cannot
// help with. The subsystem-level sweep is effectively finished.
// ============================================================================

// ============================================================================
// 43.67  0x21EC -- THE ITEMS-WINDOW DETAIL FEED  (2026-07-28)
// ============================================================================
// Dispatched only if the ITEMS window exists (0x00D1FCB8), straight to
// 0x007375F0 with the RAW BODY as the argument. Note that unlike almost
// everything else in this protocol it is NOT read through a fail-open stream --
// the handler indexes the packet buffer directly as a fixed struct, so a short
// packet reads past the end. Send the full size.
//
//   +0x04   dword
//   +0x08   dword
//   +0x0C   dword
//   +0x20   dword   real_estate_id       -> window[0x384]
//   +0x24   dword   real_estate_item_id  -> window[0x388]
//   +0x2C   char[]  a C string (taken with 0x00805C20, i.e. from a char*)
//   +0x70   an ARRAY of 0x10-byte entries, indexed as param[(n + 7) * 4]
//           dwords; entry field 0 is compared against 1 in a filter test
//   +0x2EC  byte    a flag; when zero a whole branch of the populate is skipped
//   +0x2F0  dword
//   +0x2F4  dword
//   +0x2F8  dword   -> window[0x36C]
//   +0x2FC  dword   -> window[0x38C]
//
// The body is therefore at least 0x300 bytes.
//
// +0x20 / +0x24 land on the window's selected-item identity members from 43.61,
// which is the THIRD independent sighting of the (real_estate_id,
// real_estate_item_id) pair as the identity of a placed object -- after
// EQPlacedItem +0x24/+0x28 (43.60) and the 0x2A9E remove path. Treat that pair
// as the canonical key everywhere.
//
// The 0x10-byte array at +0x70 is the item list itself. Its entry layout is not
// decoded; the only field observed in use is the first dword, tested == 1 by a
// filter that also branches on a mode value of 2 or 4 (the same 2/3/4/6
// location enum family as 43.51's move destinations and 43.55's layout scopes).
// ============================================================================

// ============================================================================
// 43.68  THE TRANSFER REQUEST -- 43.21's ORIGINAL HYPOTHESIS IS BACK IN FRONT
//        (2026-07-28)
// ============================================================================
// Status: still not proven, but the field has narrowed to one candidate and the
// reasoning that eliminated it was flawed.
//
// WHAT WE KNOW NOW that 43.21b did not:
//   * The Transfer button (+0x25C) only raises confirm dialog 0x1DD3. It sends
//     nothing, by design (43.38).
//   * Its dialog response code is 0x65; Sell's is 100. Both are raised with the
//     purchase window as parent.
//   * The ONLY 0x792C sender in the whole client is at 0x0074C91D, inside a
//     function that takes the plot id as a parameter ([ebp+8] -> body +0x00) and
//     is gated on a bit test of a mode byte (`test byte ptr [esp+0xC8], 5`).
//   * 0x792C is 190 bytes: id, two -1 dwords, then 178 ZERO bytes -- easily
//     enough room for a 64-byte character name.
//   * Transfer HAS a dedicated result channel, 0x68BA, whose messages are all
//     "Your attempt to transfer this plot failed because..." (43.49). A feature
//     with a result channel has a request.
//
// 43.21 guessed transfer reuses 0x792C with the target filled into the -1s and
// the zero tail. 43.21b retracted that because "Transfer emits nothing
// whatsoever" -- but that capture only ever pressed the BUTTON, which correctly
// emits nothing. The dialog's YES was never answered with a name. The retraction
// tested the wrong thing.
//
// So: the leading candidate is again that TRANSFER AND SELL ARE THE SAME
// OPCODE, 0x792C, distinguished by the mode bits and by whether the name field
// is populated. That is consistent with everything above and with there being no
// unaccounted C->S opcode left in the real-estate window range (43.37, as
// corrected in 43.66).
// ============================================================================

// ============================================================================
// 43.69  C->S SITES RE-ATTRIBUTED BY CONTAINING FUNCTION (proximity retired)
//        (2026-07-28)
// ============================================================================
// All 813 C->S call sites were run through Ghidra's getFunctionContaining() and
// the function ENTRY looked up in this file. That replaces the nearest-symbol
// proximity column that eqgame_cs_opcodes.txt used to carry -- proximity caused
// three wrong subsystem claims in one night (43.37, 43.51, 43.66) and it is now
// gone from the file. 222 of the 813 sites land in a function this file already
// names; the rest report a bare FUN_xxxxxxxx, which is honest ignorance rather
// than a misleading hint.
//
// THE REAL-ESTATE SENDERS, now attributed properly:
//
//   function      window / role                  opcodes it sends
//   ------------  -----------------------------  --------------------------
//   0x0073CC50    RealEstateItemsWnd notify      0x2036 0x218C 0x3424 0x3C8B
//                                                0x58C7 0x626E 0x7AD1
//   0x00735480    items dialog-response handler  0x21BA 0x613D 0x6917
//   0x0073BEC0    items helper                   0x5B41
//   0x00741EE0    manage-window helper           0x2AA6
//   0x00745090    RealEstateManageWnd notify     0x2AA6
//   0x00747ED0    access-save helper             0x3146
//   0x0074A3A0    RealEstateNeighborhoodWnd      0x2691
//   0x0074A4EC    search helper                  0x2691
//   0x0074A7E8    neighborhood query helper      0x218C
//   0x0074B920    search helper                  0x2691
//   0x0074BE00    search helper                  0x2691
//   0x0074C110    RealEstatePlotSearchWnd notify 0x796C
//   0x0074C918    sell/transfer sender           0x792C
//   0x0074D500    RealEstatePurchaseWnd notify   0x2AA6 0x45BE 0x6573
//
// *** AND THAT IS A FOURTH INDEPENDENT CONFIRMATION of 43.38's window fix. ***
// 0x0074D500 is the function that sends 0x45BE AND 0x6573 -- the opt-out packet
// captured live from the plot window in 43.25 -- while 0x0074C110 sends only
// 0x796C, the plot list's Go button. The window-table comment in this file said
// 0x74C110 was RealEstatePurchaseWnd; that comment has now been corrected too
// (the #define was already fixed in 43.38, the comment above it was not).
//
// Note 0x2691 has FOUR senders in three different helper functions plus the
// neighborhood window itself -- plot search is issued from several places, so
// "which window asked" cannot be inferred from the opcode alone.
// ============================================================================

// ============================================================================
// 43.70  0x5B41 -- THE LAYOUT UPLOAD (variable length)  (2026-07-28)
// ============================================================================
// Sent from 0x0073BEC0, an items-window helper, and it is the only variable-
// length message in the real-estate C->S set apart from 0x3146 and 0x2AA6.
//
// The payload is built by a chain of serializer calls, each taking the same
// thirteen buffers plus two MODE arguments:
//
//      0x00739FF0(..., 3, 0, arg)     0x00739D60(..., 0, 1, arg)
//      0x00739830(..., 2, 1, arg)     0x00739830(..., 3, 1, arg)
//      0x00739FF0(..., 2, 1, arg)     0x00739FF0(..., 3, 1, arg)
//      0x0073BC50(..., arg)
//      0x00738A20(...) x2
//
// then the accumulated buffer is sent whole:
//
//      if (size > 0x1FE) buf = malloc(size + 2); else buf = 0x00DD00E0;
//      *(word*)buf = opcode(0x5B41);
//      memcpy(buf + 2, payload, size);
//      Send(4, buf, size + 2);
//
// The 2 and 3 in those mode arguments are the same location-enum values that
// appear as 0x3424's move destinations (2 shed, 3 closet, 6 crate -- 43.51) and
// as the layout scope constants (43.55). So each serializer pass emits the items
// belonging to one location, and the message carries SEVERAL locations at once.
//
// [inferred] 0x5B41 IS "APPLY THIS LAYOUT". The Save buttons send the small,
// fixed 62-byte 0x626E, and 43.5 established that layouts are STORED client-side
// as RE_Layout_*.xml -- so saving needs no bulk upload. Loading does: the client
// reads its own XML and has to tell the server where every single item should
// end up, which is exactly a variable-length multi-location blob. That also
// explains why it is the only real-estate message that can exceed the static
// 0x1FE output buffer.
//
// NOT DECODED: the per-item record format inside the blob. It will be the same
// (real_estate_id, real_estate_item_id) identity plus a transform as everything
// else (43.60/43.61/43.67), but that is an expectation, not a reading.
// ============================================================================

// ============================================================================
// 43.71  THE NEIGHBORHOOD LIST READER (0x0074AB50)  (2026-07-28)
// ============================================================================
// 43.56 left this one "only partially walked". Its shape, as far as the reads
// go -- it is 0x2691 Form A with a target other than 3, 4 or 5:
//
//      word    row_count
//      byte    flag
//      (window[0x8A] is forced to -1 here)
//      dword   -> window[0x8B]
//
//      row_count x {
//          dword   id
//          string  name          (length-prefixed, 0x0080C9B0)
//          dword   a
//          dword   b
//          dword   c
//          dword   d
//          dword   e
//      }
//
//      then, if window[0x93] is null, a further run of row_count STRINGS
//
//      word    count2
//      count2 x { dword id, ... }
//
// So the message carries TWO lists: the neighborhoods themselves (id + name +
// five numeric columns) and a second, shorter list keyed by id. The RENW_ control
// names of 43.41 -- GuildList, PlayerList and NeighborhoodList, three lists on
// one window -- account for that.
//
// The conditional string run is the part to be careful with: whether those
// strings are on the wire depends on a CLIENT-SIDE window state (window[0x93]),
// not on anything in the packet. A server that always sends them will desync a
// client whose window is in the other state, and vice versa. That is unusual for
// this protocol and worth confirming against a live capture before implementing
// -- everything else here is unconditional.
// ============================================================================

// ============================================================================
// 43.73  CHASING THE UNKNOWN DEFINITION FIELDS -- TWO OF THEM MOVE
//        (2026-07-28)
// ============================================================================
// 43.43 left f01, f06, f07, f08, f10, f11, f13..f16 unidentified. Rather than
// guess, every caller of GetDefinitionById (0x007F4F70) was decompiled and the
// offsets they read off the returned pointer collected. Only seven callers
// exist -- 0x004B3134, 0x005B46FF, 0x0074059A, 0x007414CC, 0x00742D79,
// 0x0074D946 and 0x0074DCFA -- so this is exhaustive, not a sample.
//
// FIELDS ANYTHING ACTUALLY READS:
//
//   +0x1C  = f01   read and RETURNED by a manager-level getter:
//                     GetX(mgr, real_estate_id):
//                         record = mgr.records[id];        else return -1
//                         if (mgr[0x74] == 0)              return -1
//                         def = GetDefinitionById(record[0x20]);  else return -1
//                         return def[0x1C];
//                  So f01 is a per-definition value the manager exposes BY
//                  ESTATE ID, with -1 meaning "unknown". That is the shape of a
//                  zone id or a category key. IDENTITY STILL UNKNOWN -- see the
//                  caution below.
//
//   +0x24  = f03   the upkeep cost-set id (already known, 43.36)
//
//   +0x38  = f08   tested `!= 0` inside 0x005B46E0, a bounded recursive walk
//                  over a record and its parent chain with a depth argument.
//                  [inferred] "this definition may contain children" -- a
//                  container flag. It is only consulted at depth < 1.
//
//   +0x74         NOT a wire field (the wire stops at +0x60). A runtime member,
//                 tested `!= 0` as a precondition by three separate functions
//                 including the getter above. [inferred] the child/contents
//                 collection.
//
// EVERY OTHER unknown field -- f06, f07, f10, f11, f13..f16 -- is read by
// NOTHING that goes through GetDefinitionById. They are either dead in this
// client build or reached through a pointer the client already holds (the
// purchase window's cached definition, for instance, which 43.44 showed reads
// +0x28, +0x2C, +0x3C, +0x40 and +0x48 directly). Do not spend more time on them
// blind; probe them from the server one at a time and watch the window.
//
// ---------------------------------------------------------------------------
// A HYPOTHESIS I TALKED MYSELF OUT OF, recorded so nobody redoes it
// ---------------------------------------------------------------------------
// The client contains the string "RealEstateZoneId" (0x00A03CAC), which looks
// like exactly the name for f01. It is not evidence. Its ONLY reference is at
// 0x007C8B3A, inside a long run of
//        push 0 / push <string> / mov ecx,ebx / call 0x00B6E1B0
// -- a schema/field-name registration loop for a SAVE FILE, sitting between two
// other adjacent name strings. It names a persisted field, not a definition
// offset, and there is no path from it to +0x1C. A plausible-sounding string is
// not a citation.
// ============================================================================

// ============================================================================
// 43.74  0x3C8B -- 352 BYTES WITH A NAME IN IT  (2026-07-28)
// ============================================================================
// The last undecoded items-window sender. It is NOT reached from a button; the
// arm requires message 0x26 with data 1 and a window at 0x00D1FCF4 to exist --
// i.e. it is the OK of a text-entry dialog.
//
//   payload, 0x58 dwords = 352 bytes:
//     +0x00  dword  2            a literal, written as a constant
//     +0x04  dword  ?
//     +0x08  dword  real_estate_id       <- window[0x384]
//     +0x0C  dword  real_estate_item_id  <- window[0x388]
//     +0x20  char[0x80]  a 128-byte string, filled from the dialog's text
//     ...    the rest of the 352 bytes
//
#define OP_RealEstate_ItemWithName                                  0x3C8B  // C->S, 354 bytes
//
// [inferred] THIS IS MAIL ITEM. The evidence: it carries the placed-item
// identity pair plus a 128-byte name typed into a dialog; the items window has
// a Mail Item permission (43.41); and the 0x1228 result table has a dedicated
// refusal for it -- code 79, "You cannot do that because you do not have mail
// item access" (43.42). A feature with its own permission and its own refusal
// code has a request, and this is the only items-window message that carries a
// typed name.
// Not proven. The leading alternative is Transfer-an-item rather than mail; both
// would look identical from here. One capture with the dialog answered settles
// it, exactly like the plot transfer in 43.68.
//
// The literal 2 at +0x00 is the same "mode" slot the other item operations use
// (0x3424's destination, 0x5B41's serializer passes), so 0x3C8B is probably one
// member of a family and other modes exist. Do not hard-code 2 as a magic
// number on the server -- read it and switch on it.
// ============================================================================

// ============================================================================
// 43.75  RealEstateManageWnd's CONTROL TABLE, READ LIVE -- AND WHY IT CANNOT
//        BE NAMED THE WAY THE ITEMS WINDOW WAS  (2026-07-28)
// ============================================================================
// The Items window's 44 controls were named with certainty (43.51) because its
// member table is DENSE and its type runs match the REMW.../REIW_ name list
// group for group. The Manage window is not like that. Read live through
// pinstRealEstateManageWnd_x, by resolving each member's vftable to its RTTI
// class:
//
//   +0x258        1 x (unknown class 0x00A1AD2C)
//   +0x25C        1 x (unknown class 0x00A1C0AC)
//   +0x260..0x268 3 x CCheckBoxWnd
//   +0x26C        1 x label
//   +0x270..0x27C 4 x CListWnd
//   +0x280        1 x CCheckBoxWnd
//   +0x284        1 x CComboWnd
//   +0x288        1 x CEditWnd
//   +0x28C        1 x CComboWnd
//   +0x290..0x294 2 x CButtonWnd
//   +0x298..0x2C4     NULL / scalars
//   +0x2D4..0x2EC 7 x (class 0x00A1A0D8)
//   +0x2F4        1 x label
//   +0x2F8        1 x CButtonWnd
//   +0x2FC        1 x CComboWnd
//   +0x300..0x30C     labels
//   +0x310..0x314 2 x CListWnd
//   +0x318..0x328 5 x CButtonWnd
//   +0x32C..0x338 4 x (class 0x00A1A0D8)
//   +0x364        1 x CCheckBoxWnd
//
// *** THE TABLE IS SPARSE. *** Whole runs are NULL, which means the UI skin in
// use does not create those controls at all -- the member exists, the widget
// does not. The REMW_ name list has 4 combos and this skin has 3; it has 11
// buttons and this skin creates 8. So the count-matching argument that made the
// Items window certain does NOT hold here, and neither does reverse-order string
// alignment: the REMW_ string block additionally has four coin-icon TEXTURE
// names (A_CopperCoin .. A_PlatinumCoin) embedded in the middle of it, which are
// not controls and would shift everything after them by four.
//
// ANY name assigned to a Manage-window offset without clicking it is a guess.
// Do not add one.
//
// WHAT IS SOLID, from the notification itself (43.50):
//      control +0x2FC  (a CComboWnd)   -> 0x2AA6 subcommand 2
//      control +0x318  (a CButtonWnd)  -> 0x2AA6 subcommand 7
//      control +0x31C  (a CButtonWnd)  -> 0x2AA6 subcommand 3
//      control +0x2F8  (a CButtonWnd)  -> the arm holding subcommands 5 and 6
//      subcommand 4 is reached further down that same arm
// and every one of those sends {subcommand, manager[0x98], comboValue, ...}.
//
// The 5-button run at +0x318..+0x328 is where the escrow controls must live --
// the name list's only run of five adjacent buttons is WithdrawAll / AddDay /
// AddWeek / AddDayTemplate / AddWeekTemplate. That is suggestive and it is NOT
// a conclusion; two of the six subcommands are already known to come from that
// run, so clicking those five buttons once with C->S logging on would finish
// the escrow protocol outright.
// ============================================================================

// ============================================================================
// 43.76  *** THE 0x2691 PLOT-SEARCH REQUEST, FULLY DECODED *** (2026-07-28)
// ============================================================================
// 43.12 called this "PARTIALLY DECODED". It is a flat 226-byte struct built on
// the stack and copied whole (0x38 dwords = 224 body bytes + the 2-byte opcode):
//
//   +0x00   dword   TYPE          written as the literal 4
//   +0x04   dword   estate id     from manager[0x8C] -- the occupancy gate of
//                                 43.29 -- or 0xFFFFFFFF when there is none
//   +0x08   dword   0
//   +0x0C   80 bytes  explicitly memset to 0
//   +0x5C   char[64]  THE SEARCH TEXT (written with a 0x40 cap)
//   +0x9C   64 bytes  explicitly memset to 0
//   +0xDC   byte      a caller-supplied flag
//   +0xDD   byte      1
//
// Three of the four senders build exactly this; they differ only in the flag at
// +0xDC and in what they put in the name field.
//
// *** WHY THIS MATTERS MORE THAN A SEARCH BOX. ***
// The TYPE at +0x00 is 4 -- the same value IsPurchasableType (0x007F6030, 43.43)
// accepts, and the same value the REPLY's leading type byte is tested against in
// 43.54. A reply whose type byte is purchasable takes Form B: ONE plot record,
// which the client constructs, adds to the manager, and -- if the two flag
// fields are set -- passes to SetFromRecord before calling
// purchaseWnd->Show(true).
//
// So the complete "show a player any plot, anywhere" flow is:
//
//      C->S 0x2691  {type 4, estate id or -1, search text}
//      S->C 0x2691  Form B: type 4, autoOpen != 0, flag != 0, and the record
//                   fields (43.54/43.55)
//      -> the plot window opens, populated, with no door click, no lot square
//         and no occupancy gate.
//
// That is a second, completely independent way into the plot window besides the
// lot-square click, and it is the one that does not need a housing zone at all.
// It is the cheapest possible end-to-end test of the purchase flow: no door,
// no 0x6FD0.
//
// REMEMBER the precondition from 43.44: SetFromRecord draws nothing unless the
// record's definition id resolves in the catalog, so 0x2698 must have gone out
// first. A window that opens empty is that, not a new bug.
// ============================================================================

// ============================================================================
// 43.77  EQPlacedItem +0x2C IS THE NpcItem FLAG -- AND 43.28's OFFSET COLLISION
//        IS FINALLY EXPLAINED  (2026-07-28)
// ============================================================================
// A consumer of EQPlacedItem branches like this:
//
//      if (item->[0x2C] != 0 && owner->[0x28] != 0) {
//          spawn = GetSpawnById(owner->[0x28]);          // 0x005996E0
//          actor = spawn->vt[0x4C]();
//          if (moving) {
//              pos = { item->[0x8C], item->[0x90], item->[0x94] };
//              SetPositionAndHeading(&pos, item->[0x80], 0);   // 0x005024B0
//          }
//          actor->vt[8](0);
//      } else {
//          existing = EQPlacedItemManager::Instance()->GetItemByGuid(item + 0x10);
//          if (existing && existing->[0x38]) existing->[0x38]->vt[8](0);
//      }
//
// So the byte at +0x2C -- the SECOND field the deserializer reads (43.60) --
// selects between two entirely different representations:
//
#define EQPlacedItem__NPCITEM_OFS                                   0x2C
#define EQPlacedItem__ACTOR_OFS                                     0x38
//
//      +0x2C != 0  the object is an NPC. It is a real SPAWN, looked up by spawn
//                  id, and the transform is applied to the spawn's actor.
//      +0x2C == 0  the object is a static placed item and its actor pointer
//                  lives at +0x38.
//
// 43.5's capture of a live plot's layout XML lists a field called NpcItem(0/1)
// and notes "IT63 = NPC-type". That is this byte. Independent confirmation from
// a 64-bit client's XML export and a RoF2 disassembly.
//
// ---------------------------------------------------------------------------
// AND THIS CLOSES 43.28
// ---------------------------------------------------------------------------
// 43.27 claimed RealEstate+0x38 was "THE ACTOR SLOT ... exactly the field
// 0x005AE010 dereferences". 43.28 retracted it: RealEstate::Deserialize reads a
// STRING into RealEstate+0x38, and the retraction said the class owning an actor
// at +0x38 "remains UNIDENTIFIED".
//
// It is EQPlacedItem. Its +0x38 really is an actor pointer, dereferenced exactly
// as 43.26/43.27 described, and it belongs to the placed-object collection --
// not to the RealEstate record. Two unrelated classes with an actor and a string
// at the same offset; the coincidence that produced a wrong claim in 43.27 is
// now accounted for from both sides.
//
// USEFUL COROLLARY: a housing NPC (a familiar, a pet, a vendor in a yard) is
// placed with the SAME 0x1045 message as furniture -- set +0x2C and give the
// object a spawn id. There is no separate NPC-placement opcode to find. That
// also explains 0x21EA (43.39), which marks an existing spawn as real-estate
// owned: 0x21EA labels the spawn, 0x1045 places it.
// ============================================================================

// ============================================================================
// 43.78  THE LAST GROUPS -- AND TWO OF TONIGHT'S INFERENCES CONFIRMED BY
//        ENTRIES THAT WERE ALREADY IN THIS FILE  (2026-07-28)
// ============================================================================
// Looking up each remaining group's guard global in THIS FILE finished the sweep
// in minutes. It also checked two inferences I had made from behaviour alone --
// both were already named here, and both agree:
//
//   0x00D1FD50  pinstVoteWnd_x
//        43.63 deduced from the plot window's Vote button that [0x00D1FD50] is
//        the poll/vote window. It is. The polling-system identification stands
//        on two independent legs now: the Poll* RTTI constructors sitting on the
//        opcode group's handlers, and this.
//
//   0x00D1FCF4  pinstTextEntryWnd_x
//        43.74 deduced that 0x3C8B is sent from the OK of a text-entry dialog,
//        because its arm requires a window at 0x00D1FCF4. It is literally the
//        text-entry window. The "carries a typed name" part of that section is
//        confirmed; only "it is Mail Item" remains inferred.
//
// TWO MORE GROUPS NAMED THE SAME WAY:
//
//   0x004D22B9  (0x071F, 0x25B7)   PVP LEADERBOARD
//        The case body's guard is `cmp [0x00F743F8], 0` and 0x00F743F8 is
//        pinstPvpLeaderboardWnd_x, already in this file.
//
//   0x004CD3C2  (0x3036, 0x3BD3)   *** WRONG -- CORRECTED 2026-08-04, see Market.h ***
//        This was recorded as "AvaZoneWnd (the GM zone picker)". It is not.
//        The two opcodes do not even share a window: 0x3036 is guarded on
//        0x00D1FD08 (= pinstCMarketplaceWnd, ctor 0x006DDB90) and 0x3BD3 on
//        0x00D1FD4C. AvaZoneWnd's ctor 0x00625870 stores to 0x00D1FD40, not
//        0x00D1FD08 -- the two globals are 0x38 apart and got conflated.
//        0x3036 is therefore a MARKETPLACE opcode (Market.h).
//        The define has been annotated in place rather than renamed, so nothing
//        that references it breaks.
//
// ALSO, two siblings of already-named opcodes:
//        0x02B4 shares its case body with OP_ItemPreview
//        0xB07F shares its case body with OP_ItemPreviewRequest
//
// ---------------------------------------------------------------------------
// STILL UNIDENTIFIED, and now it really is the whole list
// ---------------------------------------------------------------------------
//        0x004C865C  (0x3F84, 0x6E5C)  compares two name strings byte by byte
//        0x004C64C7  (0x0E48, 0x2B10)  0x2B10 special-cased, reads body[0x44]
// Everything else unnamed in eqgame_sc_opcodes_unknown.txt is a SINGLETON --
// one opcode on its own case body -- which the group method cannot help with.
//
// THE METHOD, worth stating plainly because it beat decompilation every time:
//   1. get the group's case body,
//   2. read the global it is guarded on or dispatches through,
//   3. GREP THIS FILE FOR THAT ADDRESS FIRST.
// instTribute_x (43.57), pinstNewPointMerchantWnd_x (43.66),
// pinstPvpLeaderboardWnd_x and pinstUnknown_00D1FD08_x (here) each identified a
// whole subsystem in one lookup. Years of accumulated names are already sitting
// in this header; use them before reasoning from behaviour.
// ============================================================================

// ============================================================================
// 43.79  A SUBSYSTEM LEAD FOR 133 OF THE UNNAMED OPCODES, GENERATED
//        (2026-07-28)
// ============================================================================
// 43.78 said the group method was finished because only two multi-opcode groups
// were left. That was true and also the wrong stopping point: the SINGLETON
// opcodes -- one opcode on its own case body -- are the bulk of what is unnamed,
// and the same trick works on them, in bulk.
//
// For every case body whose opcodes are all unnamed, the first ~50 bytes were
// scanned for call targets and absolute globals, and each was looked up in this
// header. 133 of 246 reference at least one symbol this header names.
//
// READ IT AS A LEAD, NOT A LABEL. `pinstLocalPlayer_x` only says the handler
// touches the player. `pinstAchievementsWnd_x` or `pinstGuildManagementWnd_x` is
// nearly conclusive. Confirm before naming.
//
// ---------------------------------------------------------------------------
// HOUSING-RELATED LEADS
// ---------------------------------------------------------------------------
//   0x49DF  -> pinstPlacedObjectManager_x (0x00DCEE30)
//        A THIRD opcode touching the placed-object collection, alongside 0x1045
//        and 0x2A9E (43.60).
//
//   0x626E  -> EQPlacedItem__Ctor_x, at case body 0x004D058A
//        *** 0x626E IS BIDIRECTIONAL. *** 43.51 has it as the 62-byte C->S
//        "save layout" from the items window; its S->C side CONSTRUCTS AN
//        EQPlacedItem. Do not assume a real-estate opcode means the same thing
//        in both directions -- 0x58C7 (43.53) is the same story.
//
//   0x2B35  -> pinstRealEstatePlotSearchWnd_x
//        A plot-search opcode we had not seen; 43.56 only knew the 0x2691
//        Form A route into that window.
//
//   0x593A  -> PollManager__Instance_x
//        A SEVENTH poll opcode, on its own case body, outside the group of six
//        in 43.63.
//
//   0x2EC3  -> pinstProgressionSelectionWnd_x
//        Confirms 43.65's monster-mission/progression identification from a
//        second direction.
//
//   0x37A9  -> CDisplay__is3dON_x
//        Resolves the loose end in 43.60: the code near 0x004CD8F9 that jumps
//        into the placed-object region belongs to a DISPLAY case, not a
//        placement one.
//
// Other clean leads worth knowing exist for achievements (0x28E0, 0x618F,
// 0x059D, 0x2B42), guild management (0x56C9, 0x0001, 0x1E50), fellowship
// (0x62AB, 0x074D), alt storage (0x7B71, 0x1115), the spell manager (0x7B32,
// 0x4FE6, 0x6CD0, 0x2AD4), find-location (0x2956, 0x6A68), soulmark, race
// change, server list and the switch manager.
//
// The remaining 113 bodies reference nothing this header names -- they inline
// their work or call unnamed helpers, and they need decompiling by hand.
// ============================================================================

// ============================================================================
// 43.80  *** 0x49DF IS THE BULK PLACED-OBJECT LIST *** (2026-07-28)
// ============================================================================
// The lead in 43.79 paid off immediately. Case body 0x004CA7A5:
//
//      stream = MakeStream(buf, len);              // 0x00410C40
//      if (pinstPlacedObjectManager_x != NULL) {
//          mgr->0x004B2740();                      // no args -- a CLEAR
//          mgr->0x004B2AB0(&stream);               // read the whole collection
//      }
//
// *** RETRACTED IN 43.81 -- 0x49DF is a bulk AREA list, not a placed-object list.
// The define is kept only so the number is findable; do not use the name. ***
#define OP_RealEstate_PlacedObjectList                              0x49DF  // S->C -- MISNAMED, see 43.81
#define PlacedObjectManager__Clear_x                                0x004B2740  // [inferred]
#define PlacedObjectManager__DeserializeAll_x                       0x004B2AB0
//
// So the placement protocol has BOTH shapes, exactly like the character's
// owned-estate id lists in 43.45:
//
//      0x49DF   the whole collection at once, replacing what was there
//      0x1045   one object, added or updated by its GUID string
//      0x2A9E   one object removed by (real_estate_id, real_estate_item_id)
//
// That is the natural zone-in / incremental split, and it means a server does
// NOT have to send a hundred separate 0x1045s when a player walks onto a
// furnished plot.
//
// NOT YET DECODED: the per-object record format inside 0x49DF's stream. It is
// almost certainly the same EQPlacedItem::Deserialize layout as 0x1045 (43.60)
// with a count in front, since 0x004B2AB0 is a manager-level reader over the
// same objects -- but that is an expectation. Read 0x004B2AB0 before relying on
// it; a bulk reader that differs from the single reader would be the kind of
// detail that costs a night.
// ============================================================================

// ============================================================================
// 43.81  *** CORRECTION TO 43.80 (AND TO 43.3 / 43.24 / 43.26): THERE ARE TWO
//        MANAGERS AND THEY HAVE BEEN CONFLATED ALL ALONG ***  (2026-07-28)
// ============================================================================
// 43.80 called 0x49DF "the bulk placed-object list" on the strength of its case
// body touching [0x00DCEE30], which this file names pinstPlacedObjectManager_x.
// Reading the reader it calls kills that claim outright.
//
// 0x004B2AB0, the thing 0x49DF invokes, does:
//
//      clear();
//      dword count;
//      repeat count times {
//          p = operator new(0x5C);            // <-- 0x5C, not 0xC0
//          *p = EQAreaBase::vftable;          // 0x00A1EB70, an RTTI-named class
//          ctor(p);
//          *p = &PTR_LAB_009CA6A8;            // then a DERIVED vftable
//          deserialize(p, stream);
//          insert into the hash map keyed by p[1];
//      }
//
// It allocates 0x5C-byte EQAreaBase-derived objects. EQPlacedItem is 0xC0 bytes
// (43.60). These are different classes in different containers:
//
//   EQPlacedItemManager   instance 0x005061E0, holds EQPlacedItem (0xC0)
//                         fed by 0x1045 / 0x2A9E.   *** THIS IS THE FURNITURE ***
//
//   [0x00DCEE30]          holds EQAreaBase-derived objects (0x5C)
//                         fed by 0x49DF.            *** THIS IS NOT FURNITURE ***
//
// So:
//   * 0x49DF is a bulk AREA list, not a placed-object list. Retract 43.80's
//     opcode name; the three-shapes story (bulk / add / remove) it told about
//     furniture is wrong, and 0x1045 + 0x2A9E are all we actually have.
//   * The name pinstPlacedObjectManager_x on 0x00DCEE30 is MISLEADING. It should
//     be read as pinstAreaManager. It is left in place so nothing that
//     references it breaks -- but do not infer anything from the name.
//   * 43.26 said iface[1] 0x005AE010 "reads its position+bounds and inserts it
//     into the placed-object map at [0x00DCEE30]". POSITION AND BOUNDS is an
//     AREA. That description was right and only the label was wrong.
//
// HOW I GOT IT WRONG, since it is the same shape as the other four this session:
// I trusted a NAME IN THIS FILE (pinstPlacedObjectManager_x) that an earlier
// session had inferred, instead of reading the allocation size two calls deeper.
// 43.78 recommends grepping this header for a global before reasoning about it,
// and that is still right -- but a name found that way is a LEAD, exactly as
// 43.79 says in bold, and this is what it looks like when a lead is wrong.
// ============================================================================

// ============================================================================
// 43.82  THE POSITIVE HALF OF 43.81 -- 0x49DF IS PROBABLY THE PLOT BOUNDARIES,
//        AND THAT IS THE MISSING PIECE OF 43.19/43.24  (2026-07-28)
// ============================================================================
// 43.81 retracted "0x49DF is the placed-object list". What it actually carries is
// a count-prefixed list of EQAreaBase-derived objects -- position AND BOUNDS --
// into the manager at [0x00DCEE30].
//
// Now recall what 43.19 and 43.24 were doing. They traced the client's
// occupancy chain, the thing that answers "which real estate am I standing in",
// and found it walks that same collection and finds NOTHING, because the
// collection is empty. 43.24 concluded "THE WORLD PATH IS DEAD". 43.26 added
// that some object with position and bounds gets registered into it by
// iface[1] 0x005AE010.
//
// Put those together with 43.81:
//
//   * the collection at [0x00DCEE30] holds AREAS -- volumes with bounds;
//   * the occupancy chain resolves by testing the player against those areas;
//   * 0x49DF is the message that FILLS the collection;
//   * if it is never sent, the collection stays empty and the world path
//     "looks dead".
//
// [inferred, and it is the natural reading rather than a proven one] 0x49DF
// carries the PLOT BOUNDARIES for the zone. Send it and the client can resolve
// occupancy from the world by itself.
//
// WHAT THAT WOULD CHANGE, and it is worth being clear that the answer is "not
// much, but it is nice": 0x6FD0 (43.29/43.30) already sets occupancy directly
// and is PROVEN in game. It needs one small packet and no geometry. Areas would
// make the client work it out itself, which is more faithful to how the live
// game did it and would matter for anything that depends on entering and
// leaving a plot rather than on being told. It is an improvement, not a
// prerequisite.
//
// CONFIRMED, in the same sitting. The per-record deserializer 0x008D2E30 reads:
//
//      dword                 -> obj[0x04]     the manager's hash KEY for this
//                                             record -- i.e. its id
//      0x008D3200(stream)    -> obj[0x08]     }
//      0x008D3200(stream)    -> obj[0x18]     }  four identical 0x10-byte
//      0x008D3200(stream)    -> obj[0x28]     }  sub-structures
//      0x008D3200(stream)    -> obj[0x38]     }
//      ... and continues
//
// Four sixteen-byte blocks read back to back by the same helper is a GEOMETRIC
// VOLUME -- four vectors, or a min/max pair plus an orientation. Together with
// the class literally being called EQAreaBase and the occupancy chain testing
// the player against this collection, the reading is settled: 0x49DF sends
// AREAS with bounds, keyed by an id.
//
// The one thing NOT nailed down is whether that key at +0x04 is a real-estate
// id or a generic area id. It is the hash key the manager stores under, and
// 43.24 observed the same +0x04 being used as an estate key, so the two agree
// -- but 43.24 is the section whose labelling 43.81 just had to correct, so do
// not lean on it. Send one area with a known id and watch the occupancy chain.
#define EQAreaBase__Deserialize_x                                   0x008D2E30
#define EQAreaBase__ID_OFS                                          0x04
#define EQAreaBase__SIZE                                            0x5C
// ============================================================================

// ============================================================================
// 43.83  THE ITEMS-WINDOW LIST IS A 24-BYTE ARRAY ON THE WINDOW ITSELF, AND
//        WHY ONLY "SHED" EVER LIGHTS UP  (2026-07-28)
// ============================================================================
// 43.67 decoded 0x21EC as the items-window DETAIL feed for one selected item.
// It is not the list. The list is a plain dynamic array hanging off the window,
// reached through one accessor:
//
#define RealEstateItemsWnd__GetItemByIndex_x                        0x0073F800
#define RealEstateItemsWnd__ITEM_ARRAY_OFS                          0x220
#define RealEstateItemsWnd__ITEM_RECORD_STRIDE                      0x18
//
//      int GetItemByIndex(container, index) {
//          if (index < container[0] && index >= 0 && container[1] != 0)
//              return container[1] + index * 0x18;
//          fatal();
//      }
//
// So: count at container[0], base pointer at container[1], 0x18-byte records.
// The container is reached in the populate code as
//      piVar6 = pinstRealEstateItemsWnd(0x00D1FCB8) + 0x88;
// and that 0x88 is an INT* offset, i.e. BYTE OFFSET 0x220 on the window.
//
// The accessor has 183 references from exactly NINE containing functions
// (attributed with getFunctionContaining, not by proximity):
//      0x00730600  0x00730DE0  0x007313F0  0x00731750  0x00735A60
//      0x00736BC0  0x00738600  0x0073CC50  0x00745090
// all inside 0x730000..0x746000, which is the RealEstateItemsWnd code range.
// 0x0073CC50 is already named in 43.71 as the window's WndNotification.
//
// RECORD FIELDS OBSERVED SO FAR (0x18 bytes, most still unattributed):
//      +0x08   dword   passed to 0x005B0CC0 as an id
//      +0x0C   dword   passed as the 3rd arg of 0x0059C260 alongside +0x08
//      +0x10   byte    VALIDITY -- every caller tests it before using the row
//      +0x11   byte    tested for zero in the same guard
// The populate also reaches into EQPlacedItemManager (0x005061E0 instance,
// 0x005060C0 accessor) and reads +0x8C/+0x90/+0x94 off a placed item -- the
// position triple from 43.60. So window rows and placed objects are related,
// which is the lead worth pulling next.
//
// ---------------------------------------------------------------------------
// *** WHY ONLY THE "SHED" BUTTON EVER LIGHTS UP *** -- answered in game
// ---------------------------------------------------------------------------
// Reported repeatedly during testing as if it were an occupancy or ownership
// bug. It is neither:
//
//      SHED     is the deposit action for an item held in INVENTORY, so it is
//               enabled whenever the player is standing on any real estate.
//      CLOSET / CRATE / PLACE   all act on a row SELECTED IN THE LIST above.
//
// The list is empty because the server has never fed it, so those three can
// never enable. No amount of standing in the right place, owning the plot, or
// fixing occupancy changes it. THE LIST FEED GATES STORAGE, PLACEMENT, CLOSET
// AND CRATE ALL AT ONCE -- it is the single keystone for the whole items half
// of the feature.
//
// COROLLARY, and it cost real time to learn: because Shed is the only button
// ever enabled, EVERY captured 0x3424 so far came from Shed. That is why 43.51's
// destination enum could be contradicted but not corrected -- Closet and Crate
// cannot be pressed until the list exists.
//
// ---------------------------------------------------------------------------
// THE BUTTON STATE IS NOT A PERMISSION CHECK
// ---------------------------------------------------------------------------
// Confirmed in game 2026-07-28: Shed lights up on ANY plot the player stands on,
// including vacant lots and other players' plots. The client has no reason to
// think otherwise -- an access table (0x0004 / 0x485D, 43.45) has never been
// sent, so it defaults permissive.
//
// A SERVER MUST THEREFORE VALIDATE OWNERSHIP ITSELF on every item operation and
// treat the greyed/ungreyed state as pure decoration. Answering with result code
// 0x17 on 0x1228 gives the client's own sentence, "This can only be done by the
// owner of the property.", for four bytes.
// ============================================================================

// ============================================================================
// 43.84  *** THE 0x49DF AREA RECORD IS FULLY DECODED -- IT IS A 4x4 MATRIX ***
//        (2026-07-28)
// ============================================================================
// 43.82 left the per-area format as the open question ("read the loop in
// 0x004B2AB0"). Done. EQAreaBase::Deserialize (0x008D2E30) is:
//
//      dword                       -> +0x04
//      0x008D3200(stream)  x4      -> four sub-blocks
//      dword                       -> +0x48
//      dword                       -> +0x4C
//      dword                       -> +0x50
//      dword                       -> +0x54
//      dword                       -> +0x58
//
// and 0x008D3200 reads exactly FOUR dwords into consecutive slots. So the four
// sub-blocks are 16 dwords occupying +0x08 .. +0x47.
//
// WHAT THOSE 16 DWORDS ARE. The constructor (0x008D2970) seeds them all to zero
// EXCEPT four, which it sets to 0x3F800000 (= 1.0f):
//
//      +0x08   +0x1C   +0x30   +0x44
//
// Lay +0x08..+0x44 out as a row-major 4x4 and those are exactly m[0][0],
// m[1][1], m[2][2], m[3][3] -- THE DIAGONAL. The constructor is seeding an
// IDENTITY MATRIX. Sixteen floats, four rows read by four calls, and the only
// four seeded to 1.0 are the diagonal: this is not a guess.
//
//      row 0   +0x08 +0x0C +0x10 +0x14
//      row 1   +0x18 +0x1C +0x20 +0x24
//      row 2   +0x28 +0x2C +0x30 +0x34
//      row 3   +0x38 +0x3C +0x40 +0x44
//
// Row-major with m[3][3] at +0x44 is the D3D row-vector convention, which puts
// TRANSLATION at m[3][0..2] = +0x38 / +0x3C / +0x40 and SCALE on the diagonal.
// So an area is a unit volume transformed into place: position, orientation and
// extent in one matrix, which is why 43.26 described the registering function as
// reading "position AND bounds".
//
#define EQAreaBase__ID_OFS_CONFIRMED                                0x04
#define EQAreaBase__MATRIX_OFS                                      0x08   // 4x4 row-major
#define EQAreaBase__MATRIX_TRANSLATION_OFS                          0x38   // m[3][0..2]
#define EQAreaBase__TAIL0_OFS                                       0x48
#define EQAreaBase__TAIL1_OFS                                       0x4C
#define EQAreaBase__TAIL2_OFS                                       0x50
#define EQAreaBase__TAIL3_OFS                                       0x54   // ctor seeds -1
#define EQAreaBase__TAIL4_OFS                                       0x58   // ctor seeds -1
#define EQAreaBase__WIRE_SIZE                                       88     // 4 + 64 + 20
//
// ---------------------------------------------------------------------------
// THE WHOLE 0x49DF MESSAGE
// ---------------------------------------------------------------------------
//      dword   count
//      count x {
//          dword       area id            (also the hash key, 0x004B2AB0 keys on p[1])
//          float[16]   4x4 row-major transform
//          dword       +0x48
//          dword       +0x4C
//          dword       +0x50
//          dword       +0x54
//          dword       +0x58
//      }
// 88 bytes per area. Nothing is length-prefixed and there are no strings.
//
// STILL INFERRED, and flagged as such: which of +0x54 / +0x58 (both seeded -1)
// carries the real_estate_id. The natural reading is that +0x04 is the area's
// own key and one of the -1 seeded tail dwords links it to a real estate, since
// a -1 seed is what this codebase uses for "no id yet" everywhere else (compare
// EQPlacedItem +0x0C / +0x24 / +0x28 in 43.60). Send the plot id in +0x54 first;
// if occupancy does not resolve, try +0x58 before doubting anything else.
// ============================================================================

// ============================================================================
// 43.85  *** 0x21EC DRIVEN LIVE: IT OPENS AND FILLS A CLICKABLE PROPERTY LIST ***
//        (2026-07-28, sent from a real server and observed in game)
//        (2026-07-28, observed in game)
// 43.67 decoded 0x21EC's struct statically and called it "the items-window DETAIL
// feed". It was never sent. It has now been sent, and it does considerably more
// than that name suggests.
//
// A 0x300-byte body, zero-filled, with only 43.67's identified fields set, made
// the client OPEN A WINDOW, title it from the body string, and fill it with
// rows -- every one of them a clickable link that opens the Real Estate
// MANAGEMENT window.
//
// CONFIRMED EXACTLY AS 43.67 PREDICTED, read out of the live window afterwards:
//      body +0x20  ->  window +0x384    (value round-trips exactly)
//      body +0x24  ->  window +0x388    (value round-trips exactly)
// Two independent fields landing on the predicted members is the check that the
// whole struct mapping is anchored correctly.
//
// NEW, and none of it was visible statically:
//
//   * body +0x2C's C STRING BECOMES THE WINDOW TITLE.
//
//   * THE ROWS RENDER FROM AN eqstr STRING ID, not from an inline string. Every
//     row displayed "ERROR: String not found. (0)" -- the client's own missing-
//     string text, with the id we sent shown in the parentheses. So one of the
//     four dwords in each 0x10-byte entry at +0x70 is a string id into
//     eqstr_us.txt. Which one is not yet pinned.
//
//   * EVERY ROW IS A LINK. Clicking any of them opens the Real Estate Management
//     window. This is a PROPERTY LIST -- "your real estates" -- not a list of
//     objects inside one property.
//
// *** AND IT IS NOT THE 24-BYTE ARRAY FROM 43.83. *** A hardware WRITE watchpoint
// covering window +0x230 (count) and +0x234 (base) did not fire once, and both
// stayed zero across the whole exchange. So the window at 0xD1FCB8 owns AT LEAST
// TWO lists, and 0x21EC fills a different one. 43.83's array is still unfed and
// still unexplained.
//
// WHICH ALSO MEANS THE NAME pinstRealEstateItemsWnd_x ON 0xD1FCB8 IS SUSPECT.
// A window whose rows are properties and open the Manage window is a property
// list, not the Items window. The pointer was verified against a window-name
// string block earlier in this file, so do not simply rename it -- but do not
// reason from the name either. Read what it actually does.
//
// ============================================================================

// ============================================================================
// 43.86  FURNITURE: THE WHOLE CLIENT-SIDE CHAIN COMPLETES AND IT STILL DOES NOT
//        RENDER -- WHAT IS PROVEN, AND WHAT IS LEFT  (2026-07-28)
// ============================================================================
// 0x1045 was driven from a real server and traced through the live client with
// hardware breakpoints. EVERY STAGE WORKS:
//
//      packet arrives
//        -> EQPlacedItem::Deserialize  0x00505400      fires
//        -> EQPlacedItemManager::Add   0x00506030      fires
//        -> 0x00504B60  (actor create)                 fires
//        -> 0x0048B710  (actor factory)                fires
//        -> actor allocated, item +0x38 non-null and readable
//
// and the object read back out of client memory is correct field for field:
// real_estate_id, real_estate_item_id, is_npc, the "IT20075" tag at +0x3C, scale
// 1.0 at +0x7C and the position triple at +0x8C/+0x90/+0x94.
//
// THE MODEL IS NOT DRAWN.
//
// ---------------------------------------------------------------------------
// RULED OUT. Do not spend time re-checking any of these.
// ---------------------------------------------------------------------------
//   * The wire format. 43.60's field order was re-verified against the
//     decompiled deserializer and matches exactly.
//   * The art. IT20075 -> furniture06.eqg and IT20018 -> phexterior1a.eqg are
//     both registered in Resources/OnDemandResources.txt and both archives are
//     on disk. Nothing needs adding to OnDemandResources or GlobalLoad.
//   * The actor definition lookup. The factory appends "_ACTORDEF" to any tag
//     shorter than 10 chars and resolves it; on failure it returns NULL unless
//     the pitch argument's low byte is non-zero. We get NON-NULL, so the
//     definition resolved.
//   * Position. Tested at the player's world position AND at 0,0,0. No
//     difference either way.
//   * The opcode mapping and a value collision.
//
// ---------------------------------------------------------------------------
// TWO MISTAKES THAT COST HOURS -- BOTH ARE MEASUREMENT ERRORS, NOT CLIENT BUGS
// ---------------------------------------------------------------------------
//   1. A BAD REBASE. 0x0048B710 + 0x360000 is 0x007EB710, NOT 0x0084B710. The
//      wrong address produced "the actor factory is never called", which was
//      false and sent the investigation down a dead end. ALWAYS re-derive the
//      rebase; this file's own warning about ASLR is not enough on its own.
//   2. A CACHED KEY. Re-sending 0x1045 with a key string the client already
//      holds takes UpdateFrom (0x00504FD0), NOT the add-and-create path. Several
//      "the actor is never created" results were really "this was an update".
//      ANY placement probe must vary the key per send or it silently tests the
//      wrong branch.
//
// ---------------------------------------------------------------------------
// WHERE TO RESUME
// ---------------------------------------------------------------------------
// Find the PER-FRAME RENDER WALK -- whatever iterates placed items and decides
// to draw them -- and read what it filters on. Everything upstream of "a
// populated actor exists" is confirmed working, so the gap is entirely between
// that actor and the screen.
//
// One untested hypothesis worth trying first because it is nearly free: live EQ
// neighborhoods were INSTANCED, and every test so far ran in the base zone
// (instance_id 0). If the client gates real-estate rendering on being in a real
// estate instance, that would explain why every packet-level check came back
// clean. Nothing read so far checks a zone or instance id -- so this is a
// hypothesis, not a finding -- but it is one zone-in to test.
// ============================================================================

// ---------------------------------------------------------------------------
// 43.85b  THE 0x10-BYTE ROW ENTRY, DECODED IN GAME BY BISECTION  (2026-07-28)
// ---------------------------------------------------------------------------
// Entry at body +0x70 + n*0x10:
//
#define RealEstateListRow__TYPE_OFS                                 0x00  // no effect on the label
#define RealEstateListRow__STRINGID_OFS                             0x04  // *** eqstr id -> the row text ***
#define RealEstateListRow__INDEX_OFS                                0x08  // small index; 5219 here killed every row
#define RealEstateListRow__PRESENT_OFS                              0x0C  // non-zero or the row does not render
//
// THE ROW LABELS ARE eqstr IDS, not inline text. A row cannot carry an
// arbitrary name.
//
//   * +0x0C's VALUE is irrelevant; only zero vs non-zero matters. 5219 and 5220
//     behaved identically, 0 blanked every row.
//   * +0x00 does not select the text. 1 and 2 rendered the same string.
//
// AND A CORRECTION TO 43.67: it described the entry's first dword as being
// "compared against 1 in a filter test". Whatever that filter does, it is NOT
// row visibility -- rows whose +0x00 is zero still render (as the client's
// missing-string error), and rows with +0x00 = 2 render normally.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 43.85c  WHAT THE 0x21EC WINDOW ACTUALLY IS -- CORRECTING 43.85  (2026-07-28)
// ---------------------------------------------------------------------------
// 43.85 called it "a PROPERTY LIST -- your real estates". Confirmed wrong by
// 43.85b: the row labels are eqstr IDS, so a row CANNOT carry a property name,
// an address or any other per-property text. A list of properties that cannot
// name a property is not a property list.
//
// What it demonstrably is: a list of CANNED MESSAGES, each row clickable, each
// click opening the Real Estate MANAGEMENT window. Set the row string to 5219
// and every row reads "You cannot add this item because the real estate is
// full." That is the shape of a NOTIFICATION / PROBLEM list -- "here is what is
// wrong with your properties, click a line to go and deal with it" -- and it
// fits the fact that 43.7's whole result-code vocabulary is eqstr ids of
// exactly this kind.
//
// [inferred] from the evidence above; the window's own name string has not been
// read, and 0xD1FCB8's label in this file (pinstRealEstateItemsWnd_x) is now
// doubly suspect -- it is not the window that owns the 24-byte item array of
// 43.83 either, since a write watchpoint on that array never fired while this
// window was being filled.
//
// PRACTICAL: useful for telling a player their upkeep lapsed or their yard is
// full, with a click-through to Manage. NOT a route to listing owned plots by
// name, and NOT the item list that gates storage/Place/Closet/Crate.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 43.86b  THE INSTANCE HYPOTHESIS IS DEAD  (2026-07-28, tested in game)
// ---------------------------------------------------------------------------
// 43.86 suggested that live EQ neighborhoods were instanced and that the client
// might gate real-estate rendering on being in an instance, which would have
// explained every clean packet-level result. TESTED: a real instance of
// `neighborhood` was created (#instance create / #zoneinstance), entered, and
// 0x1045 sent while standing on an owned plot inside it.
//
//      actor factory fires. Nothing renders. Nothing appears in any list.
//
// Identical to the base zone. INSTANCING IS NOT THE BLOCKER -- do not spend
// time on it again.
//
// What remains is the render path itself: an actor is allocated and populated
// and never appears. The next thing to read is what 0x00504B60 does AFTER it
// stores the actor at item+0x38 -- if there is an attach / register-with-scene
// step there, and it is conditional, that condition is the whole answer.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 43.83b  THE ITEMS LIST: WHAT IT IS *NOT* FED BY  (2026-07-28, all tested live
//         with a hardware WRITE watchpoint on window +0x230 / +0x234)
// ---------------------------------------------------------------------------
// The 0x18-byte-record array of 43.83 gates storage, Place, Closet and Crate.
// It stays count 0 / base 0 through everything below. A write watchpoint cannot
// be fooled by a wrong packet body -- if anything had touched the array, it
// would have fired. It never fired once.
//
//   * 0x21EC does NOT fill it. It demonstrably fills a DIFFERENT list on the
//     same window (43.85) while this array stays zero.
//   * PLACED OBJECTS do not fill it. 0x1045 completes end to end - parse, add,
//     actor built - and no row appears (43.86).
//   * A REFRESH does not rebuild it. 0x1228 with code 0, whose documented side
//     effect is the items-window refresh at 0x00733CB0 (43.8), changes nothing.
//     So the list is NOT a derived view rebuilt from the manager's collections.
//   * NONE of the unclaimed real-estate S->C opcodes fill it. Swept one at a
//     time with 1024-byte bodies: 0x0C13, 0x1097, 0x1BAF, 0x21EA, 0x2AA7,
//     0x2CF7, 0x3E00, 0x66DF, 0x76C6. No writes, and no crash from any of them.
//   * No function that READS the array also writes it. All 9 accessor callers
//     were decompiled and checked, and no call passes the container by pointer
//     either, so the writer is not among them.
//
// WHAT IS LEFT. Two handlers reference the window pointer 0x00D1FCB8 and have
// NOT been driven yet, because both need a structured body a zero-fill cannot
// provide: 0x087F (0x005B0E30, "id + name + N sub-records") and 0x1CEF
// (0x005B0D00). Their sub-record wire format is behind 0x005AC0B0 / 0x005AC050,
// which are reference-counting wrappers -- the actual field reads are one level
// deeper again and were not reached.
//
// If neither of those is it, the feed is not in the real-estate opcode set at
// all and the search has to widen to the full S->C table.
// ---------------------------------------------------------------------------

// ============================================================================
// 43.87  *** THE ITEMS LIST HAS ONLY ONE FEED, AND WE HAVE NEVER BEEN ABLE TO
//        SEND IT. THE FURNITURE HAS NO RENDER-SIDE GATE AT ALL ***  (2026-07-28)
// ============================================================================
// 43.83b spent a long time proving, with hardware write watchpoints, that no
// opcode sent ever wrote the Items window's 24-byte array. That result was
// correct and it was the wrong question. NOTHING writes that array directly in
// response to a packet. The array is DERIVED.
//
// *** THE HARD RESULT, and it is a static one, not a hypothesis: ***
// The ONLY code that grows that array is FUN_00735A60 (Items wnd) and
// FUN_00742740 (sibling wnd), and the ONLY caller of either is FUN_005ABD20,
// the per-item record reader. FUN_005ABD20 is reached from exactly three opcode
// handlers -- 0x1CEF (0x005B0D00), 0x087F (0x005B0E30) and 0x2CF7 -- and NONE
// of the three is mapped in EQEmu's patch_RoF2.conf, so an EQEmu server cannot
// send the only thing that can fill this list. Every candidate 43.83b
// eliminated was eliminated correctly; the real feed was never on the list.
//
// The rest of this section is the surrounding machinery. Some of it is inferred
// rather than observed -- the split is called out explicitly below, because an
// earlier draft of this note overstated it.
//
// The chain, read out of the decompiler:
//
//   CRealEstateManager::Tick                         0x005B0A10
//     |
//     |   if (mgr->record_map.entry_count < 1) {     // mgr +0x0C
//     |       mgr->current_re_id (+0x94) = mgr->fallback (+0x8C);
//     |       mgr->yard_re_id    (+0x98) = -1;       // <<<< HARD RESET
//     |       mgr->house_re_id   (+0x9C) = -1;
//     |       mgr->crate_re_id   (+0xA0) = -1;
//     |   }
//     |   else once per second {
//     |       mgr->current_re_id = FUN_004B3040(local_player);   // WHICH AREA AM I IN
//     |       if (current == -1) current = mgr->fallback;
//     |       FUN_005AFFC0();   // derive HOUSE  (+0x9C) from current
//     |       FUN_005B00E0();   // derive YARD   (+0x98) from current
//     |       FUN_005B0140();   // derive CRATE  (+0xA0) from current
//     |   }
//     v
//   CRealEstateItemsWnd::Update                      0x0073B7E0
//         yard  = mgr[+0x98];  house = mgr[+0x9C];  crate = mgr[+0xA0];
//         wnd[+0x26C] = yard;  wnd[+0x270] = house; wnd[+0x274] = crate;
//     v
//   every row in the 24-byte array is filtered against those THREE ids
//   (0x00730880 keeps a row only if row.re_id == one of +0x26C/+0x270/+0x274)
//     v
//   CRealEstateItemsWnd placement gate                0x00730DE0
//         requires the placement session DAT_00E641F8, a matching row, and
//         wnd[+0x250] == 0x0E -- this is what lights the Place button.
//
// If that map is empty the manager forces all three container ids to -1, the
// window copies -1 into all three slots, and every row is filtered out even if
// one existed -- so Place can never engage either.
//
// *** BUT DO NOT ASSUME THAT IS THE ACTIVE CAUSE. *** THE MANAGER HAS TWO HASH
// MAPS AND THEY ARE NOT THE SAME MAP:
//       map A   +0x04 buckets, +0x08 bucket count, +0x0C ENTRY COUNT, +0x10 used
//       map B   +0x64 buckets, +0x68 bucket count, +0x6C entry count, +0x70 used
// The tick's gate reads map A (+0x0C), and so do the yard/house/crate
// derivations. But FUN_005B5170 -- the insert used by 0x1CEF and 0x087F --
// writes map B. Map A is most likely fed by the record path
// (0x63FD / 0x6FD0 / 0x2691, see 43.39), in which case map A is already
// populated after a purchase and the hard-reset branch is NOT the active
// problem. This has not been checked in memory yet and it is one read to settle:
// take DAT_00E64570, then look at +0x0C, +0x6C, +0x94, +0x98, +0x9C, +0xA0.
// Do that BEFORE building anything on the hard-reset theory.
//
// The un-gating result above does not depend on which map is empty: the feed is
// missing either way.
//
// ---------------------------------------------------------------------------
// THE FEED
// ---------------------------------------------------------------------------
//   0x1CEF  handler 0x005B0D00 -> reader 0x005AC050
//   0x087F  handler 0x005B0E30 -> reader 0x005AC0B0  (also carries a NAME string)
//
// Both do: read a dword key -> find-or-create a record -> read a count -> loop
// FUN_005ABD20 once per item. On success the new record is inserted with
// FUN_005B5170; on failure it is removed with FUN_005B4680 and both windows are
// told to drop its rows.
//
// FUN_005ABD20 (0x005ABD20) is the per-ITEM record reader. It allocates a
// 0x3C-byte record and -- this is the missing write -- pushes it into BOTH
// windows itself:
//         if (DAT_00D1FCB8) FUN_00735A60(record);   // Real Estate Items wnd
//         if (DAT_00D1FCBC) FUN_00742740(record);   // the sibling wnd
// 0x00735A60 and 0x00742740 are the functions that grow the 24-byte array. They
// are reached ONLY from here. That is why no packet ever appeared to write it.
//
// ---------------------------------------------------------------------------
// THE MANAGER, 0xB4 BYTES  (allocated in 0x005B3540, ctor 0x005B21E0)
// ---------------------------------------------------------------------------
//   +0x04  map A: bucket array   (+0x08 bucket count)
//   +0x0C  map A: ENTRY COUNT   <-- the tick's gate; read by the derivations too
//   +0x64  map B: bucket array   (+0x68 bucket count, +0x6C entries, +0x70 used)
//          map B is what 0x1CEF / 0x087F insert into, via FUN_005B5170
//   +0x88  last tick ms (1s throttle)
//   +0x8C  fallback real-estate id
//   +0x94  CURRENT real-estate id -- set from the AREA the player stands in
//   +0x98  YARD real-estate id
//   +0x9C  HOUSE real-estate id
//   +0xA0  MOVING CRATE real-estate id
//   +0xA4  dirty byte  (+0xA8 its timestamp; 5s re-request throttle -> 0x005ADB60)
//
// THE ITEMS WINDOW  (ctor 0x00733E00, confirmed by its own "RealEstateItemsWnd"
// string, so the pinstRealEstateItemsWnd_x name on 0x00D1FCB8 IS correct --
// 43.85's doubt about the name was wrong; what 43.85 saw was a DIFFERENT list on
// a DIFFERENT window, which remains true.)
//   +0x230  list count        +0x234  list base   (24-byte stride, acc 0x0073F800)
//   +0x250  selected container type -- must be 0x0E for placement
//   +0x254  selected row index (short)     +0x25C  selected item
//   +0x26C  yard re id   +0x270  house re id   +0x274  crate re id
//   +0x27C..+0x294  the seven counters, in the order of the UI labels below
//   +0x298  "list changed" flag
//   row entry: +0x00 refcounted record ptr, +0x08 re id, +0x10 valid byte,
//              +0x11 placeable byte, +0x14 classifier dword
//              (classified by 0x007F5EC0 / 0x007F6030 / 0x007F60F0 / 0x007F61B0)
//
// THE THREE CONTAINERS ARE NAMED BY THE UI ITSELF. From the ctor:
//   REIW_YardItemsCount / REIW_YardNonRealEstateItemsCount / REIW_YardNpcItemsCount
//   REIW_HouseItemsCount / REIW_HouseNonRealEstateItemsCount
//   REIW_MovingCrateItemsCount / REIW_MovingCrateNonRealEstateItemsCount
// plus REIW_Place_Button, REIW_Move_Shed_Button, REIW_Move_Closet_Button,
// REIW_Move_Crate_Button, REIW_Move_Inventory_Button, REIW_Stow_Placed_Button,
// REIW_Pack_Moving_Crate_Button, REIW_ShowYardButton / ShowHouseButton /
// ShowMovingCrateButton, and a whole LAYOUT subsystem (REIW_LayoutSaveYard_Btn,
// REIW_LayoutLoadHouse_Btn, REIW_LayoutRotateYard_Combobox, ...).
// FUN_00730B10(container) is the per-container clear and is called with 3, 2 and
// 6 for yard, house and crate respectively -- so those are the container type
// constants, and 0x0E is a fourth one used by the placement path.
//
// ---------------------------------------------------------------------------
// WHY THIS ALSO EXPLAINS THE FURNITURE  (supersedes 43.86's "resume here")
// ---------------------------------------------------------------------------
// 43.86's conclusion -- packet, parse, manager, actor and art all verified, and
// still nothing on screen -- was accurate about every stage it checked. What it
// could not see is that 0x0051C740, the real per-frame walk, does NOT filter
// placed items at all: it just creates any actor that is missing (+0x38 == 0)
// and moves on. There is no render-side gate to find. So the model is not being
// rejected by the renderer; the client simply has no real estate to associate it
// with. A placed item carries a real_estate_id at +0x24, and 0x00537D30 compares
// that against mgr[+0x94] -- which, with an empty record map, is the fallback and
// never matches.
//
// NOTE ON 0x0051C740: it also proves the AREA collection matters. The current
// real-estate id comes from FUN_004B3040(local_player), an AREA lookup against
// DAT_00DCEE30 -- the collection that 0x49DF fills (43.84). So 0x49DF is not
// cosmetic: it is how the client decides which property it is standing in.
//
// ---------------------------------------------------------------------------
// WHERE TO RESUME
// ---------------------------------------------------------------------------
// Send 0x1CEF (or 0x087F) with a real record and at least one item, so the map
// stops being empty. The remaining unknown is the exact wire layout of the
// 0x3C-byte item record read by FUN_005ABD20 -- Ghidra loses the field reads
// there because it mis-infers `this`, so that one needs disassembly rather than
// decompilation, or a live breakpoint on 0x005ABD20 with a probe body.
// Everything else on the path is now identified by address.
// ============================================================================

// ============================================================================
// 43.88  THE REAL-ESTATE RECORD WIRE FORMAT (0x1CEF / 0x087F)  (2026-07-28)
// ============================================================================
// 43.87 established that the manager's record map being empty is what kills the
// Items list, the Place button and the furniture together. This is the format
// needed to stop it being empty.
//
// The reads are NOT in FUN_005ABD20 -- it delegates to four field-group helpers
// in the shared 0x7FC000 serialization library, each taking ECX = a sub-object
// of the record and the stream as its one stack argument. Read straight off the
// disassembly at 0x005ABDAC..0x005ABDE9, so the ORDER below is the wire order.
//
// ---------------------------------------------------------------------------
// OUTER BODIES
// ---------------------------------------------------------------------------
//   0x1CEF   handler 0x005B0D00
//       u32   real_estate_id                 (the map key)
//       u32   item_count                     (read by 0x005AC050)
//       item_count x ITEM RECORD
//
//   0x087F   handler 0x005B0E30
//       u32   real_estate_id
//       str   name                           (length-prefixed, FUN_0080C9B0)
//       u32   item_count
//       item_count x ITEM RECORD
//
// Both find-or-create a 0x28-byte real-estate record (ctor 0x005AB230) and, on
// success, insert it with FUN_005B5170 -- which is what makes map entry_count
// (+0x0C) non-zero. On failure they remove it (FUN_005B4680) and tell both
// windows to drop its rows.
//
// ---------------------------------------------------------------------------
// ITEM RECORD, 0x3C bytes -- reader 0x005ABD20, five groups IN THIS ORDER
// ---------------------------------------------------------------------------
//   group 1  ECX=rec+0x34  FUN_007FCA90   u32 -> +0x34
//                                         u32 -> +0x38
//   group 2  ECX=rec+0x00  FUN_007FC9A0   u8  -> +0x00
//                                         u32 -> +0x04
//   group 3  ECX=rec+0x08  FUN_007FC9E0   u32 -> +0x08
//                                         u32 -> +0x0C
//                                         u32 -> +0x10
//                                         u32 -> +0x14
//                                         u32 -> +0x18
//                                         u32 -> +0x1C
//                                         u32 -> +0x20
//   group 4  ECX=rec+0x24  FUN_007FC960   str -> +0x24   (FUN_0080C9B0)
//                                         str -> +0x28   (FUN_0080C9B0)
//                                         +0x2C is NOT on the wire -- it is
//                                         stamped locally from the clock
//   group 5  ECX=rec+0x30  FUN_005AC0B0(stream, 0)
//                                         *** FORMAT NOT YET PROVEN ***
//                                         a refcounted shared-ptr assigned via
//                                         FUN_005AAD80. Almost certainly the
//                                         item-instance blob. Decode this before
//                                         sending, or the stream desyncs here.
//
// Every one of these is the usual FAIL-OPEN reader (`if (cursor+N <= len)`), so
// a short body will not error -- it will silently leave fields at their ctor
// defaults, exactly the failure mode that has cost time all session.
//
// ---------------------------------------------------------------------------
// THE REAL-ESTATE TYPE ENUM -- decoded from the classifiers
// ---------------------------------------------------------------------------
// FUN_007F5EC0(type) maps a real-estate type to the container index that
// FUN_00730B10 clears and that the Items window groups by:
//       type 1        -> 1
//       type 2, 3     -> 2    HOUSE          (FUN_007F60F0 returns true)
//       type 4, 8     -> 3    YARD / PLOT    (FUN_007F6030 returns true)
//       type 5        -> 4
//       type 6        -> 5                   (FUN_007F61B0 returns true)
//       type 7        -> 6    MOVING CRATE
//       type 10       -> 8
//       type 11       -> 9
//       anything else -> 0
//
// This is what confirms 43.87's slot labels rather than assuming them:
//   FUN_005AFFC0 writes mgr+0x9C by taking the CURRENT record, checking it is a
//   yard (007F6030) and then walking its child list (rec+0x48 count, rec+0x4C
//   array) for the child that is a house (007F60F0). A yard contains a house.
//   So mgr+0x98 = YARD, mgr+0x9C = HOUSE, mgr+0xA0 = MOVING CRATE, and the
//   window's 0x26C/0x270/0x274 follow in that same order.
//
// ---------------------------------------------------------------------------
// STILL MISSING
// ---------------------------------------------------------------------------
// Neither 0x1CEF nor 0x087F carries the real estate's own TYPE -- yet
// FUN_005AFFC0 reads it from real-estate record +0x20 (via FUN_007F4FA0) to do
// the yard/house classification. So a third opcode sets it. The candidates are
// the two handlers already dumped but never driven: 0x1967 (0x005AFD50, reads a
// string then FUN_005AB030) and 0x1BAF (0x005AFE80). Find which one populates
// +0x20 before assuming 0x087F alone is enough.
// ============================================================================

// ============================================================================
// 43.89  *** CONFIRMED IN GAME: THE ITEMS LIST POPULATES ***  (2026-07-28)
// ============================================================================
// 43.87/43.88 were static findings. They are now proven live. Sending 0x087F
// with one item record put a row into the
// Real Estate Items window: Location "Yard", Status "Placed", the owner name
// filled in, and the counters went to "Yard: 1/500, 1/200 vault, 0/12 pets"
// with House and Crate correctly N/A.
//
// MEASURED, not inferred -- the whole chain read out of live memory:
//
//   manager (DAT_00E64570)      before 0x087F        after
//       +0x0C  map A entries    1                    1     (already fed by 0x63FD)
//       +0x6C  map B entries    0                    1     <- the insert
//       +0x8C  fallback         9001                 9001
//       +0x90  type             4                    4     <- type 4 = YARD, as 43.88 predicted
//       +0x94  current          9001                 9001
//       +0x98  YARD             -1                   9001  <- derivation alive
//       +0x9C  HOUSE            -1                   -1    (correct: no house estate)
//       +0xA0  CRATE            -1                   -1    (correct: no crate)
//
//   items window (DAT_00D1FCB8)
//       +0x26C yard id          -1                   9001
//       +0x230 row count        0                    1
//
// TWO THINGS THAT ONLY THE LIVE RUN COULD SHOW:
//
//   1. THE WINDOW MUST BE OPEN. FUN_0073B7E0 is the window's Update and it does
//      not tick while the window is closed, so +0x26C/+0x270/+0x274 sit at the
//      ctor's -1 no matter how healthy the manager is. A first read with the
//      window shut showed EVERY field still at its 0x00733E00 default and looked
//      like a failure. It was not. Open the window before judging this path.
//
//   2. RECORD FIELD GROUP 5 (rec+0x30) IS NOT REQUIRED *** TO DRAW A ROW ***.
//      Its reader is fail-open like every other one: with the stream exhausted
//      it consumes nothing, leaves +0x30 at the ctor's null, and the row is
//      still built and still added.
//      *** BUT SEE 43.93 -- THIS WAS OVER-GENERALISED. *** Group 5 is the ITEM
//      ITSELF. Without it the row exists but is inert: no icon, no name, and the
//      Place button stays dead. Not-required is true only of the row appearing.
//
// THE ROW ENTRY, 0x18 bytes, read out of the live array:
//       +0x00  refcounted record pointer
//       +0x08  real_estate_id     <- MUST equal one of the window's three ids
//                                    (+0x26C/+0x270/+0x274) or FUN_00735A60
//                                    drops the row on the floor
//       +0x0C  real_estate_item_id
//       +0x10  valid byte
//       +0x11  PLACED-IN-WORLD byte  <- from the record's leading u8; drives the
//                                    Status column (Placed/Stored), the window's
//                                    Placed/Stored filter checkboxes, and
//                                    0x00730DE0 (interact: sit needs it non-zero).
//                                    [corrected 2026-08-02; originally misread
//                                    as "placeable"]
//       +0x14  classifier         <- this is the REAL ESTATE's type, not the
//                                    item's; here it read 4 and FUN_007F5EC0(4)=3
//                                    put the row in the Yard bucket
//
// WHAT THE FIELDS DRIVE, from the visible row: the two strings at rec+0x24 and
// rec+0x28 are the item NAME (sent empty, the name column came up blank),
// and the owner-name column is filled from the real-estate record rather than
// from the item. The Type column showed "V" and Qty "1" off an all-zero group 3,
// so those come from the seven dwords at rec+0x08..+0x20 -- still unassigned
// individually, and now cheap to bisect the same way 43.85b did.
// ============================================================================

// ============================================================================
// 43.90  FURNITURE: THE REAL-ESTATE ASSOCIATION WAS NOT THE BLOCKER EITHER
//        (2026-07-28, measured live right before the client was lost)
// ============================================================================
// 43.87 suggested the furniture and the Items list were one problem: placed
// items carry a real_estate_id at +0x24, 0x00537D30 compares it against
// mgr+0x94, and with no real estate loaded nothing could ever match. The Items
// half of that is now fixed and PROVEN (43.89). The furniture half is DISPROVEN.
//
// With the manager fully healthy -- mgr+0x94 = mgr+0x98 = 9001, a live yard,
// the Items window showing a row -- a 0x1045 placement of IT20075 STILL renders nothing.
//
// The placed object was read out of the live manager and it is field-perfect.
// Manager instance DAT_00DCFC70 -> list head -> the one entry:
//
//      +0x00  vtable 0x00D2F724
//      +0x04  0                    prev  (only entry in the list)
//      +0x08  0                    next
//      +0x0C  -1
//      +0x10  "0000000000000000"   16-char decimal serial, unset
//      +0x24  9001                 real_estate_id  <-- MATCHES mgr+0x94 EXACTLY
//      +0x28  1                    real_estate_item_id
//      +0x2C  0                    is_npc
//      +0x30  0                    npc spawn id
//      +0x34  0                    secondary actor
//      +0x38  0x3DD7DEF0           *** ACTOR POINTER, NON-NULL ***
//      +0x3C  "IT20075"            tag
//      +0x48  1
//      +0x4C  50
//      +0x7C  1.0f                 scale
//      +0x80..+0x88  0,0,0         heading / tilt / roll
//      +0x8C  368.375f  \
//      +0x90  -164.125f  |         position -- matches the server's echo of
//      +0x94  1.8760f   /          (368.4, -164.1, 1.9) to the last digit
//      +0x98  1
//
// So the object is correct, the actor exists, the id association is correct,
// there is no render-side filter (0x0051C740, 43.87), and it is still invisible.
//
// *** THE ACTOR IS REAL. CHECKED, AND THE SUSPICION WAS WRONG. ***
// The +0x38 pointer looked suspect -- ~1.03 GB in, while every other live
// pointer this session sat in 0x14xxxxxx-0x27xxxxxx -- and 43.86 records having
// been burned once by exactly that. So it was tested rather than assumed. A
// second placement (item_id 4) gave +0x38 = 0x3DD7CD68, and it reads clean:
//
//      +0x00  0x6FD7841C   vtable -- INSIDE EQGraphicsDX9.dll
//                          (module base 0x6FC40000, so vtable RVA 0x13841C)
//      +0x04  2
//      +0x08  0.50391f
//      +0x0C..+0x18  1.0f x4        scale row
//      +0x28  0.05620f
//      +0x2C  363.03f  //      +0x30  -151.04f  |  position, the placement point for that send
//      +0x34  1.8770f  /
//
// A real graphics-layer object with a real vtable, correct scale and correct
// position. So 43.86 was right and the "unreadable pointer" theory is DEAD --
// do not spend time on it again. The high address is just where EQGraphicsDX9
// allocates; it is not evidence of anything.
//
// WHICH LEAVES THE HARD VERSION OF THE QUESTION: a valid actor, in the manager,
// with a matching real-estate id, no render-side filter (0x0051C740), correct
// position and scale -- and no pixels. The remaining candidates are that the
// actor was never attached to the scene/render list, or that its actor
// definition resolved to something with no geometry. Note the factory's fallback
// chain is IT63_ACTORDEF then RED_BLOB_ACTORDEF; we see NEITHER, so it did not
// silently fall back to a placeholder.
//
// NEXT MOVE: stop reasoning about the placed actor in isolation and DIFF IT against a
// working one. Zone objects are built through the same FUN_0048B710 with the
// same "%s_ACTORDEF" pattern (0x0051C740's first loop, list from FUN_004DC1B0,
// actor at +0x18, tag at +0x1C). Read one of those actors and compare fields.
// Whatever differs is the answer, and that is a much shorter path than another
// pass over the creation code.
// ============================================================================

// ============================================================================
// 43.91  ZONE-OBJECT MANAGER AND THE TWO ACTOR-CREATION CALL SITES
// ============================================================================
// The zone-object manager is DAT_00DCF120 (getter FUN_004DC1B0); its list nodes
// link on +0x04, hold the actor at +0x18 and the tag -- WITH the "_ACTORDEF"
// suffix already on it -- at +0x1C.
//
// Actor header fields observed on a settled zone-object actor:
//        +0x00   vtable inside EQGraphicsDX9.dll
//        +0x04   2
//        +0x08   float (524288.0f == 2^19 on zone objects)
//        +0x0C..+0x18  1.0f x4
//        +0x28   float
//        +0x2C   position
// (+0x1C/+0x20 read zero on settled actors too; they are NOT the geometry.)
//
// The two callers of the actor factory:
//   zone object 0x0051C740:  FUN_0048B710(tag, x, y, z, heading,  0,   0, 1, 1)
//   placed item 0x00504B60:  FUN_0048B710(tag, x, y, z, heading, tilt, 0, 1, 0)
// The final argument is cVar8 inside the factory; it decides whether the
// param_6/param_7 block runs before the geometry call at DAT_015D46A8
// vtable+0x60 and what is passed to the actor's own vtable+0x50 (2 vs 3).
// Ghidra's decompile of FUN_0048B710 is unreliable (unaff_EDI/unaff_retaddr).
// ============================================================================

// ============================================================================
// 43.92  THE FACTORY CALL, ARGUMENT ORDER
// ============================================================================
// Register/stack layout at the factory entry for a placed item:
//      ECX  = this (CDisplay)
//      ESI  = the EQPlacedItem
//      arg1 = ESI+0x3C, the tag
//      arg2..arg4 = X, Y, Z (floats)
//      arg5 = heading (item +0x80)
//      arg6 = tilt    (item +0x84)
//      arg7 = 0, arg8 = 1, arg9 = 0
// So the call is  FUN_0048B710(this, tag, x, y, z, heading, tilt, 0, 1, 0).
//
// 0x0051C740 is NOT a per-frame walk: nulling a zone object's actor pointer
// (list DAT_00DCF120 -> node +0x18) is never repaired, so that function runs on
// zone load or another event. Nulling +0x18 leaks the old actor.
// ============================================================================

// ============================================================================
// 43.93  REAL ESTATE ITEMS WINDOW: THE ROW NEEDS AN ITEM (GROUP 5)
// ============================================================================
// The Place button only arms when the selected row carries a valid item
// (icon, name). Status "Placed" does not grey it out. Group 5 of the content
// feed supplies the item. The chain is
//        FUN_005AC0B0  ->  FUN_005AAD80  ->  FUN_005559A0
// and FUN_005559A0 is an item-instance deserializer: its other call site (the
// 0x58C7 handler) runs the result into a refcounted item pointer and on into a
// cursor/loot path. 0x58C7 is NOT OP_ItemPacket (0x368E) -- they share a
// deserializer, not an opcode. Group 5 carries a fully serialized item in the
// same format the client accepts for OP_ItemPacket.
// ============================================================================

// ============================================================================
// 43.94  ROF2 CONTAINS THE ENTIRE PLACEMENT UI
// ============================================================================
// Flow: click Place -> the object appears tinted green, attached to the player
// -> an instruction overlay lists the controls -> move/rotate/scale -> confirm.
// The eqstr_us.txt strings for it:
//        5260 Set Mode Cursor          5266 Collision Mode
//        5261 Set Mode Free            5267 Cursor Mode
//        5262 Reset Pitch              5268 Free Placement Mode
//        5263 Reset Heading            5271 Reset Roll
//        5264 Reset Size               5274 Placement Data
//        5286 Wall Mode                5287 Set Mode Wall
//        5294 Rotate Pitch 90 Degrees  5295 Rotate Roll 90 Degrees
//        5296 Rotate Heading 90 Degrees
//        5273 You cannot place this item here because the surface is not flat enough.
//        5275 This item must be placed in the zone for you to interact with it.
//        5270 The item you are placing does not allow %1
// The client owns placement end to end; the server supplies valid items,
// accepts the result and replays it on zone-in.
// ============================================================================

// ============================================================================
// 43.95  THE PLACE REQUEST: C->S 0x626E, 60 BYTES
// ============================================================================
// Preconditions for the client to send it: the item must be flagged Placeable
// in its serialized item data, and the row must carry a real item (43.93).
// Without them the client sends a 12-byte 0x06C8 give-up instead.
//
//   +0x00  char[17]  the 16-character item/object key + NUL (the key string the
//                    server put in the item header; compared as a string)
//   +0x11  u8
//   +0x12  u32       0x1C2 observed
//   +0x16  u16
//   +0x18  u16       INVENTORY SLOT the item is in
//   +0x1A  u32       -1
//   +0x1E  u16
//   +0x20  u32       -1
//   +0x24  u32       -1
//   +0x28  u32       0
//   +0x2C  u32       1
//   +0x30  u32       513 observed
//   +0x34  u32       CURSOR X
//   +0x38  u32       CURSOR Y
//
// Coordinates identified by clicking at two screen positions and diffing:
// only the last two dwords change.
// 0x626E is a Place path, not "save layout" (it has three call sites).
// ============================================================================

// ============================================================================
// 43.96  PLACEMENT FLOW AND GENERIC RECORD SHAPES
// ============================================================================
// Reading EQ UDP traffic:
//   * Payload = frame + 42 (14 eth + 20 ip + 8 udp).
//   * First two bytes are the PROTOCOL opcode: 0x0009 OP_Packet, 0x0003
//     OP_Combined, 0x000D OP_Fragment, 0x0015 OP_Ack.
//   * A third byte of 0x5A ('Z') means the rest is ZLIB (stream begins 78 9c).
//     A trailing CRC may need trimming.
//   * Large messages arrive as OP_Fragment pieces that must be reassembled
//     before inflating.
//
// THE FLOW:
//   ZONE IN      S->C  real-estate list (yard/house/crate)
//                S->C  placed-object records, several per packet
//   PLACE CLICK  C->S  0x626E place request (43.95)
//                S->C  place reply -> client enters its own placement UX;
//                      no server traffic while moving the object
//   CONFIRM      C->S  key + final transform (0x711A, 43.113)
//                S->C  small result + placed-object record
//
// The real-estate list carries the owner name and one address string per real
// estate (plot / plot+house / plot+crate) -- the YARD / HOUSE / CRATE triple of
// 43.87 (mgr +0x98 / +0x9C / +0xA0).
//
// Placed-object records carry: actor tag, real_estate_id, real_estate_item_id,
// a 16-char key, scale, position, and a per-item scale-limit triple (e.g.
// 0.1 / 2.5 / 1.0, varies per item). The confirm echoes the key verbatim.
// ============================================================================

// ============================================================================
// 43.97  EQPlacedItem ACTOR BUILD AND THE FACTORY'S NAME HANDLING
// ============================================================================
// EQPlacedItem's actor build is at 0x00504B60. It splits on the NpcItem byte at
// item +0x2C:
//   [esi+0x2C] != 0   -> NPC branch: builds a spawn via 0x00599BD0
//   [esi+0x2C] == 0   -> 0x00504DCB, the furniture branch:
//
//   00504DCB  fldz
//   00504DCD  push 00                   ; arg9, hardcoded
//   00504DCF  push 01                   ; arg8
//   00504DD8  lea ecx,[esi+0x3C]        ; the tag
//   ...       [esi+0x84] pitch, [esi+0x80] heading, [esi+0x94] z,
//             [esi+0x90] y, [esi+0x8C] x  pushed as floats
//   00504E13  call 0048B710             ; the actor factory
//   00504E18  mov [esi+0x38],eax        ; eax = actor, esi = EQPlacedItem
//
// Inside the factory (0x0048B710):
//   0048B760  cmp eax,0A                ; strlen(tag) < 10 ?
//   0048B763  jae 0048B7A4
//   0048B769  push 009C7BB0             ; "PLAYER_1"
//   0048B76F  call strstr
//   ...       appends "_ACTORDEF" (string at 0x009C5708)
//   0048B7A4  mov ecx,[015D46A0]        ; actordef manager
//   0048B7B9  call [vtable+0x08]        ; resolve tag
//   0048B7BB  cmp [esp+0x6C],00
//   0048B7C0  jne 0048B874              ; found
//   0048B7C6  ...                       ; miss branch
// 0x0048B874 strncmp's the tag's first 4 chars against constants at
// 0x009C7B6C/74/7C/84 to choose a default scale, converging at 0x0048B978.
// ============================================================================

// ============================================================================
// 43.98  OP_RealEstateItemMove (C->S 0x3424)
// ============================================================================
// 50 bytes, opens with the 16-char object key, e.g. "0000000000000021".
// The client keys placed objects by the key string the item header carried.
// The client sends nothing back after receiving a place-object.
// ============================================================================

// ============================================================================
// 43.100 - 43.102  ACTOR COMPLETION FIELDS AND EQPlacedItemManager
// ============================================================================
// Every actor returned by the factory has its header completed ~2 s later by
// two writes:
//     +0x74   <- OWNER back-pointer (e.g. the EQPlacedItem that owns it)
//     +0x78   <- owner TYPE tag: 9 = placed item, 3 = zone object
//
// The code choosing the type after the factory call in the furniture branch:
//   00504E26  cmp byte ptr [esi+0x2C],0   ; NpcItem
//   00504E2A  je  00504E36                ; -> push 09
//   00504E2C  cmp dword ptr [esi+0x30],0
//   00504E30  je  00504E36                ; -> push 09
//   00504E32  push 01
//   00504E36  push 09
//   00504E38  mov edx,[ecx]               ; the actor
//   00504E3A  mov eax,[edx+0x2C]
//   00504E3D  call eax                    ; actor->vt[0x2C](value) -> +0x78
//
// EQPlacedItemManager = 0x00DCFC70; getter 0x005061E0 is a lazy-init singleton.
// List head at manager+0x00, nodes linked on +0x08, each node's actor at +0x38.
//
// The actor-rebuild pass at 0x0051C740 walks three collections, one of them
// the placed-item manager:
//   0051C8E2  call 005061E0        ; EQPlacedItemManager singleton
//   0051C8E7  mov esi,[eax]        ; list head
//   0051C8F0  cmp [esi+38],00      ; actor missing?
//   0051C8F4  jne 0051C905
//   0051C900  call 00504B60        ; EQPlacedItem actor builder
//   0051C905  mov esi,[esi+08]     ; next
// The zone-object path there only stores the actor:
//   0051C955  call 0048B710
//   0051C95A  mov [esi+34],eax
// An owner can hold its actor in more than one slot (e.g. owner+0xA4 too).
//
// Actor link fields: +0x178 / +0x17C are the EQGraphicsDX9 actor chain links
// (see 43.124). +0x158 points at a small graphics-side record on some actors.
// ============================================================================

// ============================================================================
// 43.104  RealEstateManagerClient FIELDS
// ============================================================================
// Manager global: 0x00E64570, object at [global].
//     +0x0C  records map
//     +0x8C  fallback / current estate id set by 0x6FD0 SetCurrent
//     +0x90  type
//     +0x94  CURRENT estate (tracks plot as the player walks)
//     +0x98  YARD
//     +0x9C  HOUSE   (-1 when none)
//     +0xA0  CRATE   (-1 when none)
// ============================================================================

// ============================================================================
// 43.106  REAL ESTATE RESULT STRINGS
// ============================================================================
// eqstr 5223 "Your real estate item request failed." is the DEFAULT arm of the
// result-code-to-string switch. 0x005AE623 (push 0x1467) is reached only after
// a chain of cmp/jne against 0x4C, 0x4D, 0x4E, 0x4F, 0x51. The message means
// the display path got a code it has no sentence for (code 1 is client-internal,
// see 43.111).
//     5217  Real estate items window toggle
//     5219  You cannot add this item because the real estate is full.
//     5220  You cannot put that item in this real estate.   [wording trimmed]
//     5223  Your real estate item request failed.           <-- the catch-all
//     5224  Item placement has been cancelled because the item being placed
//           has been REMOVED FROM THE REAL ESTATE.
//     5247  ...the item is no longer selected in the real estate items window.
//
// The Layouts tab writes userdata\RE_Layout_<id>.xml (<RealEstateLayout .../>).
// Alt+Mouse is "Change Size"; Change Height is gated on free mode.
// ============================================================================

// ============================================================================
// 43.108  SEEING EVERY C->S OPCODE
// ============================================================================
// Every C->S opcode is a literal pushed into the opcode encoder at 0x007D03C0.
// A breakpoint there sees every packet the client emits, and stack[0] at the
// breakpoint is the wire opcode. Verified: 0x46C6 = OP_FloatListThing,
// 0x7DFC = OP_ClientUpdate, 0x0971 = OP_SpawnAppearance.
// ============================================================================

// ============================================================================
// 43.110  ACCESS TAB PERMISSIONS
// ============================================================================
// Thirteen permission rows x seven role columns (Owner .. Visitor):
//     Receive Buffs, Enter, Withdraw Upkeep, Remove Crate, Add Crate,
//     Remove House, Add House, Mail Item, Remove Item, Add Item, Move Item,
//     Interact Item, Manage
// The Owner column is locked (always full, never editable). View radios:
// Individuals / Owner's Clubs / Group Totals. The client renders the tab from
// memory; the access table arrives with the estate load, not on demand.
// ============================================================================

// ============================================================================
// 43.111  THE PLACEMENT GATE: THE 0x626E REPLY CARRIES A PERMISSION MAP
// ============================================================================
// 0x004B2BF0 is the position validator (NOT an "Add"; it inserts nothing). It
// builds the EIGHT TRANSFORMED CORNERS of the candidate box and QUERIES,
// returning a result code. Its callers are 0x004B2F20 (a wrapper reducing the
// code to a bool) and 0x005AE07F inside 0x005AE010.
//
// `this` is *(0x00DCEE30) (`mov ecx,[0xdcee30]` at 0x005AE06F). Two hash maps:
//      +0x00 buckets  +0x04 bucket count   MAP 1: area id -> EQAreaBase*
//      +0x10 byte                          debug-draw flag (0x004B2920 draws
//                                          every area's matrix as wireframe)
//      +0x14 buckets  +0x18 bucket count   MAP 2: id -> RESULT CODE
// Node shape for both is {value, key, next} at +0x00/+0x04/+0x08. MAP 2 is
// constructed with TEN buckets at 0x00536275; its element is a plain int.
//
// The query loop:
//      for each area in MAP 1:
//          node = MAP2[ area->id ]              // area+0x04
//          if (!node) continue;                 // area skipped
//          if (!AllEightCornersInside(area))    // 0x008D2B00, all 8 or nothing
//              continue;
//          found = true;
//          code  = MAP2[area->id].value;
//          if (code == 0) return 0;             // 0 == PERMITTED
//      if (found) return code;
//      // nothing contained the box -- fall back to the CURRENT ESTATE
//      mgr  = 0x005B3540();                     // RealEstateManagerClient
//      node = MAP2[ mgr[0x8C] ];
//      if (!node) return 1;                     // 0x004B2EEF: mov eax,1
//      return node->value;
// `mov eax,1` at 0x004B2EEF is the internal code 1 behind the catch-all 5223,
// reached with no network traffic. The unreferenced helper 0x004B2BB0 is
// `bool(uint id) { return MAP2[id] == 0; }`: value 0 = allowed, a miss = denied.
#define AreaManager__INSTANCE                                       0x00DCEE30
#define AreaManager__AREAS_BUCKETS_OFS                              0x00
#define AreaManager__AREAS_NBUCKETS_OFS                             0x04
#define AreaManager__DRAWAREAS_FLAG_OFS                             0x10
#define AreaManager__PERM_BUCKETS_OFS                               0x14
#define AreaManager__PERM_NBUCKETS_OFS                              0x18
#define AreaManager__Query_x                                        0x004B2BF0  // NOT Add
#define AreaManager__QueryBool_x                                    0x004B2F20
#define AreaManager__GetPermMap_x                                   0x004B3C50  // lea eax,[ecx+0x14]
#define AreaManager__ClearPermMap_x                                 0x004D5B90  // add ecx,0x14; jmp clear
#define AreaManager__IsPermitted_x                                  0x004B2BB0  // dead code, states the rule
#define AreaManager__DrawAreas_x                                    0x004B2920
#define EQAreaBase__ContainsAllEightCorners_x                       0x008D2B00
#define HashMap__Insert_x                                           0x008D6C50  // Insert(&value,&key)
//
// WHO FILLS THE MAPS: 0x49DF fills MAP 1 only (handler 0x004CA7A5 calls a reset
// 0x004B2740, then the bulk reader 0x004B2AB0). MAP 2 is filled only by the S->C
// handler at 0x004D058A -- jump-table target index 3 of the array at 0x004D5198,
// byte index table 0x004D51B8, switch base 0x61DB -- opcode 0x626E.
//
// THE FULL S->C 0x626E BODY:
//      0x00505400(stream, &item)     the EQPlacedItem
//      0x004D60E0(stream, &small)    byte + three uint16  (7 bytes)
//      0x00404890 x3                 three bytes          b1, b2, b3
//      0x004048B0 x3                 three dwords         d1, d2, d3
//      if (*(0x00DCEE30)) {          else abandon placement
//          clear MAP 2
//          dword count
//          count x { dword key; dword value }
//          dword key; dword value                         <-- one extra pair
//          0x005ADB60(0x005B3540())  refresh
//      }
//      0x00558BB0(iface@+0x84, &item, &small, b1,b2,b3, d1,d2,d3, 0)
// Wire order within a pair is KEY THEN VALUE; the insert helper is
// Insert(&value, &key) (0x008D6C50: node[0]=value, node[1]=key).
// The readers are fail-open (`if (cursor+N <= len) { read; cursor += N; }`),
// so a truncated body clears MAP 2 and leaves a single {0 -> 0} entry.
#define OP_RealEstatePlaceRequest__SC_HANDLER                       0x004D058A
#define RealEstate__PlaceReply_ReadSmall_x                          0x004D60E0
#define RealEstate__BeginPlacement_x                                0x00558BB0
#define RealEstate__PlacementSingleton_x                            0x00558540
#define RealEstate__PlaceReply_Refresh_x                            0x005ADB60
//
// THE SIX SCALARS (0x00558BB0, param_5..param_10 = b1..d3):
//   b1  ->  session+0x24; `if (session[9])` gates the whole actor setup.
//           Must be non-zero.
//   b2  ->  selects the ghost copy source (see 43.124).
//   b3, d1, d2, d3  ->  passed to 0x005056B0 as the ghost's parameters.
// If the ghost ends up null or actorless, BeginPlacement resets session+0x24 to
// 0 and the session dies silently.
//
// AREA GEOMETRY: the unit volume is [-1,+1]^3. 0x00505F60 is the per-point
// test, three absolute-value comparisons against 1.0:
//      |m00*px + m10*py + m20*pz + m30| <= 1.0
//      |m01*px + m11*py + m21*pz + m31| <= 1.0
//      |m02*px + m12*py + m22*pz + m32| <= 1.0
// (row-vector convention). 0x008D2B00 takes the area's own 4x4 (`lea ebx,
// [ecx+8]`) and inverts it via 0x00505BA0 (2x2 cofactor determinants), so the
// record carries the LOCAL->WORLD transform and the client builds world->local.
#define EQAreaBase__PointInVolume_x                                 0x00505F60
#define EQAreaBase__InvertMatrix_x                                  0x00505BA0
//
// The candidate box: 0x004B2BF0 scales the actor's own bounds (vt[0x84]) by the
// globals at 0x009CA6AC and 0x009C7E78 before building the eight corners; all
// eight must fit. Neither path in 0x004B2BF0 reads area record +0x54 / +0x58.
// ============================================================================

// ============================================================================
// 43.112  THERE ARE THREE PACKET DISPATCHERS
// ============================================================================
// 0x004CC1D7 dispatches on [ebp-0x55b0] via 244 direct compares and SIX
// jump-table ranges:
//      0x2507..0x25B7   idxtab 0x004D4E94   jt 0x004D4E80
//      0x4EFA..0x4FED   idxtab 0x004D4F5C   jt 0x004D4F48
//      0x575B..0x5829   idxtab 0x004D5064   jt 0x004D5050
//      0x5F42..0x5F8E   idxtab 0x004D5148   jt 0x004D5134
//      0x61DB..0x62D6   idxtab 0x004D51B8   jt 0x004D5198
//      0x7273..0x7323   idxtab 0x004D52C8   jt 0x004D52B4
// with 0x004D3F43 as the unhandled default. 0x49DF, 0x6FD0, 0x2691, 0x2698 and
// 0x63FD are not in it but are handled elsewhere: RealEstateManagerClient has
// its own dispatcher at 0x005B2500 (compare chain on eax), and a third chain
// compares [ebp-0x5360] inside 0x004C3250. "Not in the big dispatcher" does not
// mean "not handled" -- search all of .text for the immediate.
#define ZoneDispatcher_Big_x                                        0x004CC1D7
#define ZoneDispatcher_Big_UNHANDLED_x                              0x004D3F43
#define RealEstateManagerClient__Dispatch_x                         0x005B2500
//
// THE 0x485D (ACCESS TABLE) HANDLER: case at 0x005B2ABD inside 0x005B2500
// (`cmp eax,0x485d` at 0x005B29CC):
//      new(0x48)                        a 0x48-byte record
//      ctor 0x005B5250, then 0x005B5830
//      0x004048B0(stream, &key)         a LEADING DWORD KEY (real_estate_id)
//      0x005B7A90(record, stream)       deserialize
//      0x005B7DE0(mgr, key, record)     insert, keyed by that dword
// 0x005B7A90 reads:
//      7 x { dword ; dword }                        role slots -- 0x38 bytes
//      dword count
//      count x { length-prefixed string ; dword }   name map at record+0x38
//                                                   (string via 0x0080C9B0, first)
// Omitting the leading key makes the fail-open reader run off the end and
// crash the client. The meaning of the two dwords per role slot is unconfirmed.
#define OP_RealEstateAccess__SC_HANDLER                             0x005B2ABD
#define RealEstateAccess__Deserialize_x                             0x005B7A90
#define RealEstateAccess__Insert_x                                  0x005B7DE0
#define RealEstateAccess__RECORD_SIZE                               0x48
#define RealEstateAccess__NAMEMAP_OFS                               0x38
#define Stream__ReadString_x                                        0x0080C9B0
// ============================================================================

// ============================================================================
// 43.113  THE PLACEMENT CONFIRM: C->S 0x711A, ANSWERED ON 0x1228
// ============================================================================
// RealEstateManagerClient's 1/sec update, 0x005B0A10:
//       if (mgr[0xA4] && mgr[0xA8] + 5000 <= GetTickCount()) 0x005ADB60();
// +0xA4 is a pending-request flag, +0xA8 the timestamp, 5000 ms the give-up.
// This is why an unanswered request greys a control and releases it ~5 s later.
#define RealEstateManagerClient__PENDING_FLAG_OFS                   0xA4
#define RealEstateManagerClient__PENDING_STAMP_OFS                  0xA8
#define RealEstateManagerClient__REQUEST_TIMEOUT_MS                 5000
#define RealEstateManagerClient__Update_x                           0x005B0A10
//
// THE CONFIRM -- C->S 0x711A, 96-byte body. Sent at 0x005B2117
// (`push 0x711a; call 0x007D03C0; push 0x62`, 98 = 2 + 96; `mov ecx,0x18`,
// 24 dwords). Serialized by 0x005B3FB0:
//      +0x00  char[16]  the 16-character key, echoed verbatim
//      +0x10  word      the key's NUL and one pad byte
//      +0x14  dword x3  from a pointer argument, not yet attributed
//      +0x20  dword     real_estate_id      (echoes what the place reply sent)
//      +0x24  dword     real_estate_item_id (echoes what the place reply sent)
//      +0x28  dword     always 0
//      +0x2C  float x7  final transform:
//                       [0] HEADING [1] PITCH [2] ROLL [3] SCALE [4] X [5] Y [6] Z
//                       angles on EQ's 0-512 scale
//      +0x48  float x3  a vector
//      +0x54  float x3  a second vector
#define OP_RealEstatePlaceConfirm                                   0x711A  // C->S
#define OP_RealEstatePlaceConfirm__SIZE                             96
#define RealEstate__SendPlaceConfirm_x                              0x005B2117
#define RealEstate__SerializeConfirm_x                              0x005B3FB0
//
// THE ANSWER: S->C 0x1228 (OP_RealEstateResult), body's first dword is a code.
// Only three things call 0x005ADB60, the clear:
//      0x004D076D   the 0x626E place-reply handler
//      0x005B0A35   the 5-second timeout
//      0x005AE692   inside 0x005AE1F0  (the only packet-driven one)
// 0x005AE1F0 is reached from 0x005B263E, the `cmp eax,0x1228` arm of
// RealEstateManagerClient's dispatcher:
//      code == -1   ->  return, do nothing
//      code ==  0   ->  SUCCESS: 0x00558540() the placement singleton,
//                       0x00556D70() confirm a session is live, then
//                       0x00557B60(1,1) -- commit the session and tear the ghost
//                       down -- plus a window refresh via 0x00733CB0
//      otherwise    ->  resolve an eqstr and print it as an error
//                       (10 -> 0x1463, 11 -> 0x1479, 12 -> 0x1498, 9 -> 0x146A,
//                        0x11 -> 0x1464, 0x13 -> 0x146D, 0x14 -> 0x146F, ...)
// Only code 0 tears the placement session down. The success arm does not
// create or promote any object; the sited object must already exist.
#define RealEstate__OnResultCode_x                                  0x005AE1F0
#define RealEstate__CommitPlacement_x                               0x00557B60
// ============================================================================

// ============================================================================
// 43.114  S->C 0x6FCA: THE GROUND-SPAWN OBJECT PATH
// ============================================================================
// Handler 0x004D0438: new(0x80), ctor 0x004DBBD0, deserialize 0x004DBD50, then
// CDisplay::CreateActor (0x0048B710); actor stored at obj+0x18 with a transform
// applied through its vt[0x74]/vt[0x78].
// WIRE FORMAT (0x004DBD50), all reads fail-open:
//      dword                  -> +0x0C
//      cstring, max 63        -> +0x1C   model/actor tag
//      dword                  -> +0x10
//      dword                  -> +0x14
//      dword                  -> +0x5C
//      float x3               -> +0x60 +0x64 +0x68
//      float x3               -> +0x6C +0x70 +0x74
//      float                  -> +0x78
// and the handler calls CreateActor(tag, f70, f74, f78, f60, f64, 0.0, 1, 1).
// Its C->S twin is CEverQuest::DropHeldItemOnGround: this is the ground-spawn
// path, not a real-estate one.
#define OP_RealEstate_SpawnGroundObject                             0x6FCA  // S->C
#define GroundObject__Deserialize_x                                 0x004DBD50
#define GroundObject__TAG_OFS                                       0x1C
#define GroundObject__ACTOR_OFS                                     0x18
// ============================================================================

// ============================================================================
// 43.115  BeginPlacement LOOKUP AND THE CURSOR ENTRY POINT
// ============================================================================
// BeginPlacement (0x00558BB0) opens with a lookup BY KEY:
//      obj = FUN_005060C0(item + 0x10);            // the 16-char key
//      if (obj && obj[0x38]) {                     // found AND has an actor
//          obj->actor->vt[2](1);
//          if (b2) { ...ghost built FROM obj... }
//      }
//      if (session[5] == 0)
//          FUN_005056B0(item, ...);                // ghost from the packet item
// The 0x626E handler destroys its local item at 0x004D07FA after BeginPlacement
// returns.
//
// ENTRY POINT: putting a placeable item on the cursor and clicking the ground
// also arms placement. The C->S 0x626E send site is inside
// CEverQuest::LMouseUp (0x005384D0). The client does not pre-filter world
// clicks by the item's Placeable flag.
// ============================================================================

// ============================================================================
// 43.116  EQPlacedItem LAYOUT, LIFECYCLE AND TEARDOWN CHAIN
// ============================================================================
// Teardown chain:
//   0x00506080  EQPlacedItemManager::DestroyAll -- walks the list from the head,
//               advancing the head before each dtor
//   0x00504530  EQPlacedItem::~EQPlacedItem
//                 0x005045AC  mov  eax,[esi+0x38]      ; the actor
//                 0x005045B3  mov  ecx,[0x00DD2660]    ; CDisplay
//                 0x005045BA  call 0x00490A10
//   0x00490A10  CDisplay::DeleteActor(actor)
//                 0x00490A8B  mov  ecx,[0x015D46A8]    ; graphics-layer singleton
//                 0x00490A91  mov  eax,[ecx]
//                 0x00490A93  mov  edx,[eax+0x68]
//                 0x00490A96  push edi                 ; the actor
//                 0x00490A97  call edx                 ; into EQGraphicsDX9
//
// EQPlacedItem is 0xC0 bytes (from the ctor/dtor/copy-ctor):
//     +0x00 vftable (0x009CF724)       +0x04 prev    +0x08 next
//     +0x0C dword (-1)                 +0x10 key, 16 chars inline (+0x10..+0x1F)
//     +0x24 real_estate_id (-1)        +0x28 real_estate_item_id (-1)
//     +0x2C NpcItem byte               +0x30 spawn id
//     +0x34 light                      +0x38 THE ACTOR
//     +0x3C actor tag, 64-byte buffer (+0x3C..+0x7B)
//     +0x7C..+0x94 transform (+0x80 heading, +0x84 pitch, +0x8C/+0x90/+0x94 X/Y/Z)
//     +0x98/+0x99/+0x9A/+0x9C flags
//     +0xA0 scale min  +0xA4 scale max  +0xA8..+0xB4 1.0f row
//     +0xB8 light type (byte)          +0xBC float
//
//   * 0x00504460  ctor initializes every field (prev/next/light/actor = 0,
//     +0x0C/+0x24/+0x28 = -1, tag empty, +0x98/+0x9A/+0x9C/+0xB8 = 0, the scale
//     row = 1.0f).
//   * 0x00506030  Add; 0x00506050 RemoveOne; 0x005061A0 Remove+delete.
//   * 0x00504530  the dtor unlinks prev/next but never touches the manager head.
//   * 0x00504FD0  UpdateFrom copies +0x0C, +0x10, +0x24, +0x28, +0x2C, +0xBC,
//     the tag, +0x98..+0xB0 -- NOT +0x34/+0x38.
//   * 0x005056B0  copy ctor zeroes prev, next, +0x34 and +0x38, then builds its
//     own actor.
//   * 0x00504B60 destroys +0x38 on the furniture path without nulling it (only
//     reachable via the early return when the NPC actordef lookup fails).
//   * 0x00504FD0 and 0x005056B0 copy the tag with an unbounded strcpy into the
//     64-byte buffer at +0x3C.
//   * The all-'0' 16-char key is the client's "unset" sentinel (0x00505FF0).
//   * Lookup by (real_estate_id, real_estate_item_id) is FUN_00506120.
//
// LIFECYCLE -- 0x1045 handler, inside dispatcher FUN_004CC1D7:
//     0x004D080A  malloc(0xC0) + ctor 0x00504460
//     0x004D0889  ReadFrom(stream)  (0x00505400)
//     0x004D08BE  lookup by key (0x005060C0)
//       FOUND  -> 0x004D08DF UpdateFrom, 0x004D090D delete the temp, and skip
//                 the actor rebuild: a same-key re-send updates data only.
//       ABSENT -> 0x004D0940 Add, then 0x004D0975 build the actor (0x00504B60).
// BeginPlacement 0x00558BB0: finds the manager node by key, calls actor vt[2](1)
// on it, then malloc(0xC0) + copy-ctor -> session+0x14 = the GHOST with its own
// actor. Commit (0x00557B60 -> 0x005571D0) calls actor vt[2](0) on the manager
// node and destroys the ghost. vt[2] is SetHidden.
//
// EQGraphicsDX9.dll shader name table at +0x131BE0 (MPLRB..MPLBasic, Chroma,
// Alpha, CBS1, CBSG1, CBGG1, CBSGE1, AddAlpha, Lava, Lava2, Terrain, Water,
// WaterFall) is not an allow-list; Max* shader names (e.g. Opaque_MaxCB1.fx,
// in 317 of 1073 stock archives) are common stock content.
//
// dbg.txt is truncated on every client launch; its access-kind line is wrong
// for DEP faults (see 43.124) -- use Logs\debug.dmp.
// ============================================================================

// ============================================================================
// 43.117  ZONE-OBJECT / DOOR CONSTRUCTOR AND RELATED
// ============================================================================
// The door constructor is 0x00503E20 (vftable 0x009CF6B0, list head
// DAT_00DD2610, dtor 0x00504070), single construction site 0x004CA146:
//     scale = *(record + 0x40); if (scale <= 0.0f) scale = 1.0f;
//     sprintf(buf, "%s_ACTORDEF", name);
//     actor = CreateActor(buf, [0x11],[0x12],[0x13],[0x14],[0x15], 0, 1, 0);
//     actor->vt[0x28](this);            // owner back-pointer
//     actor->vt[0x78](scale, 1);        // scale applied
// A door name must therefore be sent bare. No vt[0x74] (3-angle) call is made.
//
// CDisplay::CreateActor is 0x0048B710: CreateActor(name, x, y, z, heading,
// pitch, ...). It auto-appends "_ACTORDEF" only for names shorter than 10
// characters and not "PLAYER_1". The placed-item path applies all three angles
// later via actor->vt[0x74](&{heading,pitch,roll}).
//
// OP_GroundSpawn handler (0x004D0360) calls CreateActor(name, x, y, GROUND_Z,
// heading, 0.0f): it snaps Z to the ground, forces pitch to zero and ignores
// scale/tilt.
//
// Door array: [0x00DCFD00], layout [count][door...]. Door entry: name at the
// start, opentype byte at +0x26, actor pointer at +0xA4. FUN_00510BD0 is the
// door teardown loop.
//
// eqgame.exe references three optional data files: RealEstateDefs.txt,
// RealEstateCosts.txt, RealEstateItemGroups.txt. The ItemGroups loader is
// 0x007FCE80: rows of (id, group, bool) into a map; on failure it writes
// "Could not open file" to RealEstateErrors.txt and returns an empty table.
// ============================================================================

// ============================================================================
// 43.124  EQGraphicsDX9.dll ACTOR CHAIN UNLINK; BeginPlacement DECOMPILED
// ============================================================================
// EQGraphicsDX9.dll, image base 0x10000000.
// FUN_1006a3d0(mgr+4, node) removes an actor from the +4 chain:
//     iVar2 = *piVar1;                          // neighbor's VTABLE
//     (**(code **)(iVar2 + 0x31c))(next);       // neighbor->SetNext(...) VIRTUAL
//     ... iVar2 = *(node->GetNext()); (**(code**)(iVar2 + 0x324))(prev); // SetPrev
// vt offsets: 0x318 GetNext (returns node+0x178), 0x320 GetPrev (node+0x17C),
// 0x31c SetNext, 0x324 SetPrev. FUN_10054590 = { return *(p+0x178); } (GetNext);
// FUN_10054580 (16 bytes before it) = { return; } (`ret 8`).
// A neighbor that was freed and reused while still linked makes `*piVar1` a
// non-vtable: either a jump into the pushed argument (DEP execute violation,
// ExceptionInformation[0]=8) or __purecall (R6025, no dbg.txt block).
//
// TWO CHAINS:
//   * the +4 chain (head mgr+0x20, count mgr+0x24): neighbor fixups are VIRTUAL.
//   * a second structure at mgr+0x98 (head +0x5c / tail +0x60), fixed up by
//     FUN_1000ebe0 with direct pointer writes.
// FUN_10068af0 is the full detach (FUN_1000ebe0 then FUN_1006a3d0); its 3
// callers (0x100451f9, 0x10052348, 0x10053697) are the actor destroys.
//
// BeginPlacement, decompiled:
// FUN_00558bb0(session, estate, item /*param_3*/, pos, b1 /*param_5*/,
//              b2 /*param_6*/, ...). param_3 is the EQPlacedItem body
// deserialized from the 0x626E reply by FUN_00505400.
// When b1 (param_1[9]) is set:
//   iVar5 = FUN_005060c0(item+0x10);   // look up a manager object by key
//   if (found && found+0x38 != 0) {
//       (**(found+0x38 vt +8))(1);     // hide the found object's actor
//       if (b2 != 0) param_1[5] = FUN_005056b0(found,...);  // ghost copied from found
//   }
//   if (param_1[5] == 0) {
//       param_1[5] = FUN_005056b0(param_3, 1,1,1,1, ...);   // ghost copied from packet
//   }
//   if (param_1[5]==0 || *(param_1[5]+0x38)==0) { disarm; return; } // needs an actor
// b2 only chooses the ghost's copy source. session+0x14 (param_1[5]) is an
// independent 192-byte heap object with its own actor, destroyed when the
// session ends. With no manager object under the key, the ghost is built from
// the packet and the real object can be created once, at commit, by 0x1045.
// ============================================================================

// ============================================================================
// 43.126 / 43.127  S->C 0x087F CONTENT FEED RECORD READER
// ============================================================================
// The 0x087F handler (0x005B0E30) reads [u32 re_id][str name][u32 count], then
// count x record. The record reader is 0x005ABD20:
//   group 1  0x007FCA90   2 dwords            (8 B)
//   group 2  0x007FC9A0   u8 + u32            (5 B)
//   group 3  0x007FC9E0   7 dwords            (28 B)
//   group 4  0x007FC960   2 len-prefixed strs (FUN_0080C9B0: [u32 len][len B]),
//                         then a local clock stamp that consumes no bytes
//   group 5  0x005AC0B0 -> 0x005AAD80 -> 0x005559A0: allocate a 0x150 item;
//            vtable+0x10 (read at [0x009CAF74+0x10]) = the item-body
//            deserializer 0x005553B0, the same one OP_ItemPacket /
//            CharInventory use. Every read is fail-open (`if cursor+N <= limit`).
//            0x005553B0 ends with count-prefixed arrays
//            [array-count][dwords][byte][subitem-count]; sub-items recurse via
//            the same vtable+0x10. Nested readers 0x004D5750, 0x007F6A20.
// A record that is not the last in the packet must match the reader's
// consumption exactly; a short item body makes the next record's bytes read as
// a count -> giant allocation -> EQ's OOM new-handler (0x0080DF30, the
// deliberate write to 0x00000000 at 0x0080DF45).
// The handler find-or-creates the record by re_id and APPENDS rows, so a
// repeated single-item 0x087F appends cleanly. 0x1CEF is the record update.
// ============================================================================

// ============================================================================
// 43.128  REMOVE-OBJECT AND FORGET-SUMMARY
// ============================================================================
// (A) S->C 0x2A9E REMOVE-OBJECT. Handler at 0x004D0998:
//      mov eax,[body+0x24]  push        ; real_estate_item_id  (arg2)
//      mov eax,[body+0x20]  push        ; real_estate_id       (arg1)
//      call GetItemByRealEstateAndRealEstateItemIds ; null -> skip remove
// The body carries real_estate_id at byte 0x20 and real_estate_item_id at byte
// 0x24 (rest zero, >= 0x28 bytes).
//
// (B) S->C 0x7D28 = "FORGET SUMMARY". Dispatches to 0x005B4680: drop the plot
// summary (SUMMARY map, mgr+0x64) and all its items-window rows; does not touch
// the placement permission map. 0x087F only find-or-creates and appends rows, so
// to clear one row send 0x7D28(plot_id) then re-feed the remaining rows.
//
// (C) EQ UDP payloads carry a short EQ header then a zlib stream starting 78 9c;
// inflate from 78 9c to get the application packet.
//
// -----------------------------------------------------------------------------
// (D) RIGHT-CLICK CONTEXT MENU on a placed object = 3D SCENE-PICK path (PARKED)
// -----------------------------------------------------------------------------
// The world right-click menu (Adjust Placement / Place / Move to Storage / Move to
// Inventory / Inspect / Show Items / Help; eqstr ids 5244/719/718/724/720/...) is
// NATIVE. CE on the RoF2 client: right-click
// the object -> CContextMenuManager::PopupMenu (0x862220->rt 0x842220) and
// CContextMenu::AddMenuItem (0x85B7B0->rt 0x83B7B0) NEVER fire (baseline: right-
// clicking the health bar DOES fire PopupMenu). GetItemByGuid (0x5060C0) and
// GetItemByRealEstateAndRealEstateItemIds (0x506120) also do not fire. The menu is
// client-built (no server packet -- confirmed by live capture). Placed objects are
// NOT click-targetable on live either; the only interaction is this menu, via the
// 3D scene-pick input path (NOT the CXWnd right-click path the health bar uses).
// NEXT: find the 3D-world scene-pick handler and how it resolves a picked actor to
// an EQPlacedItem (render walk 0x0051C740 stores the actor at EQPlacedItem+0x38).
//
// -----------------------------------------------------------------------------
// (E) *** ACCESS TABLE (0x485D S->C / 0x3146 C->S): FORMAT CORRECTED + DEFAULTS ***
// -----------------------------------------------------------------------------
// 43.46/43.112 modeled the record as 7 x {dword granted; dword DENIED}. WRONG.
// Decompressed live add/remove captures (add-fake->reject, add-real->accept,
// remove->ok) show the SECOND dword per slot is the GROUP INDEX, not "denied", and
// there are 6 groups, not 7. The C->S save and S->C table share one structure:
//
//      dword real_estate_id                 (live 0x07EE04A6)
//      7 x { dword group_index, dword granted_bits }   group_index 0..6 (SEQUENTIAL,
//            which is how it is identified), granted default masks:
//            g0 0x0003  g1 0x0003  g2 0x0803  g3 0x0E03  g4 0x1F87  g5 0x1FFF  g6 0x0000
//      dword name_count
//      name_count x { dword len, char[len] name (NO null), dword group_index }
//
// So the record IS the 7 {dword;dword} pairs 43.112 read -- pair = {group_index,
// granted}, NOT {granted, denied}. granted bits = the 13 permission rows (bit0
// Receive Buffs .. bit12 Manage). Group index ascends by permission level (g0..g5
// = 0x0003..0x1FFF); g6 = 0 (Everyone/derived; confirm label<->index with a named
// live capture).
//
// VALIDATION (this is a REAL system): on add the server checks the name is a real
// character. Fake -> rejected: server echoes the table WITHOUT the name AND sends
// an error (small opcode 0x8609 exchange, pkts 1584/1585 -- decode before relying).
// Real -> accepted and echoed. The S->C echo is what refreshes the client window.
//
// -----------------------------------------------------------------------------
// (F) PERMISSION-BIT MAP + GROUP INDEX MAPPING
// -----------------------------------------------------------------------------
// The 13 granted bits (E), pinned against the live default masks (each mask is the
// sum of the rows the Access window shows checked for that group):
//      bit0  Receive Buffs     bit7  Mail Item
//      bit1  Enter             bit8  Remove Item
//      bit2  Withdraw Upkeep   bit9  Add Item
//      bit3  Remove Crate      bit10 Move Item
//      bit4  Add Crate         bit11 Interact Item
//      bit5  Remove House      bit12 Manage
//      bit6  Add House
// Cross-checks: 0x0003 = Buffs+Enter; 0x0803 = +Interact(11); 0x0E03 = +Add(9)+
// Move(10)+Interact(11); 0x1F87 = Buffs,Enter,Withdraw,Mail,Remove,Add,Move,
// Interact,Manage; 0x1FFF = all 13. (Matches the (E) default gradient exactly.)
//
//
// GROUP MODEL + INDEX MAPPING (confirmed from named 0x3146 saves -- the C->S
// packet is client-built). The name map's
// per-entry group_index is 0..6, mapping to the assignable levels:
//   g0 Everyone 0x0003 | g1 Visitor 0x0003 | g2 Acquaintance 0x0803 |
//   g3 Friend 0x0E03 | g4 Trustee 0x1F87 | g5 Co-Owner 0x1FFF | g6 reserved 0x0000
// Proof: a name saved at Visitor came back as group 1; one saved at Co-Owner
// came back as group 5 (matching the dropdown). The Individuals dropdown offers 1..5; the Owner's
// Clubs dropdown adds g0 "Everyone" at the top (the guild/fellowship baseline -- and
// why g0==g1==0x0003). There is NO separate "Owner" level (you ARE the owner, always
// full); g6 is unused. Assigning a name to a group grants THAT NAME the level's mask.
//
// OWNER'S CLUBS (guild ranks + Fellowship) ride the SAME name map as individuals, but
// as reserved "_"-prefixed pseudo-names. CE-captured on save: len-7 "_Guild8" with
// group_index -> Guild Leader. So "_Guild<N>" = guild rank N (8 = Leader). The client
// sorts entries into the Individuals vs Owner's Clubs views by the "_" prefix. These
// are NOT characters.
// RANK MAPPING CONFIRMED (labelled 2-token save: Leader->Co-Owner, Recruit->Visitor
// came back as _Guild1 group5 and _Guild8 group1): "_Guild<N>" N == the SERVER guild
// rank verbatim (GUILD_LEADER=1 .. GUILD_RECRUIT=8), NOT remapped -- the RoF2 rank
// translation switch (EQEmu rof.cpp ~1311) is commented out.
// ============================================================================

// ============================================================================
// 43.129  ESCROW -- DEPOSIT PACKET DECODED (0x2AA6 C->S), display still owed
// ============================================================================
// The Escrow tab's "drop coin on the window" sends RoF2 opcode 0x2AA6. Three clean
// samples (1000pp, then 1pp, then 1gp) pin the DEPOSIT body (39 bytes):
//   +0x00  dword  action      0 = deposit
//   +0x04  dword  real_estate_id
//   +0x1B  dword  coin_type   0 cp, 1 sp, 2 gp, 3 pp   (byte at +0x1B, rest of dword 0)
//   +0x1F  dword  amount      (1000 / 1 / 1)
//   (bytes +0x08..+0x1A zero except +0x13 = 0x01; +0x23.. zero)
// So deposit = {action, real_estate_id, coin_type@0x1B, amount@0x1F}. NOT dword-aligned
// for the coin pair -- read at absolute offsets.
// WITHDRAW: the short 23-byte 0x2AA6 (+0x00 dword=3, +0x04 real_estate_id, rest 0) was an
// empty-escrow withdraw click; a real withdraw-with-amount is uncaptured (nothing to pull).
//
// *** 0x2AA6 IS BIDIRECTIONAL -- S->C DISPLAY DECODED. ***
// The escrow balance is NOT in the plot record (43.13 record has no coin fields; SetFromRecord
// 0x0074DCB0 doesn't touch it). It comes back on the SAME opcode 0x2AA6, S->C. The RealEstate
// manager's S->C dispatcher (0x005B2500) decodes it:
//     sub ecx,0x2AA6 / je 0x005B28B0    (0x2AA6 handler)
//     dec ecx / je ...                  (0x2AA7 = probe), sub ecx,0x250 (0x2CF7 = probe)
// 0x2AA6 S->C handler @ 0x005B28B0: builds a stream {buf,len,cursor=0} and calls the escrow
// parser with the escrow-window singleton [0x00D1FCBC]:
#define RealEstate__EscrowSingleton_x                               0x00D1FCBC  // ptr, may be null
#define RealEstate__EscrowRecv_x                                    0x005B28B0  // S->C 0x2AA6 handler
#define RealEstate__EscrowParse_x                                   0x00741670  // (singleton, &stream)
#define RealEstate__EscrowDeserialize_x                             0x00748700  // reads the record
// The parser (0x00741670) deserializes one record via 0x00748700, then routes coin entries
// through a jump table on coin_type 0..3 at 0x0074191C (0 cp,1 sp,2 gp,3 pp) into the balance.
// 0x00748700 read order FULLY DECODED (stream reader {buf@0,len@4,cursor@8}); strings are
// NUL-TERMINATED on the wire (client scans to null, caps the copy, advances cursor past null):
//     uint32   f0                        (-> struct+0x00)
//     uint32   real_estate_id            (-> +0x04)
//     uint32   f8                        (-> +0x08)
//     cstring  s1  (client caps at 63)   (-> +0x0C char[0x40])   [address]
//     cstring  s2  (client caps at 31)   (-> +0x4C char[0x20])   [owner]
//     uint8    b6C                        (-> +0x6C)
//     uint32   f70                        (-> +0x70)
//     uint32   coin_count                 (list length; vector built at +0x74 via 0x00728380)
//     coin_count x { uint32 e0; uint32 coin_type; uint64 amount }   (16 bytes each, reader 0x007DB870)
// The parser 0x00741670 REQUIRES each entry's e0 == 0, then switches on coin_type 0..3 via table
// 0x0074191C. *** CORRECTED IN GAME: S->C 0x2AA6 GIVES THE COIN TO THE PLAYER (it adds
// to the cursor) -- it is the WITHDRAW / refund path, NOT a balance display. *** Proof: the server
// echoed a {type=pp, amount=5} record after a deposit and the client put 5pp on the cursor (5->10),
// while the Escrow tab STILL showed 0. So do NOT send 0x2AA6 to "show" a balance and NEVER auto-send
// it (free coin every occupancy). Server use of 0x2AA6 S->C = hand the player coin on withdraw.
// The escrow balance DISPLAY (REMW_Balance_List) is a SEPARATE, still-undecoded mechanism -- find
// what fills REMW_Balance_List (likely a different S->C packet or the 0x2FA0 manage-open reply).
// The record layout above is still correct as the 0x2AA6 coin-transfer format (used for withdraw).
//
// CE DISPLAY DIG, still incomplete):
// - Escrow tab's balance list child window is cached at REMW_window+0x310, bound at 0x00747064
//   via GetChildItem("REMW_Balance_List") (string const 0x009D8820). Whoever POPULATES that list
//   (reads a balance -> adds coin rows) is the display path; not yet identified.
// - The 0x2AA6 coin handlers (jump table 0x0074191C: cp 0x741773 / sp 0x741795 / gp 0x7417B7 /
//   pp 0x7417D9) each do `mov [obj+0x33C8.. ],amount` where obj = map.find (0x007BB210) on the
//   map at global 0x00D1FC1C[runtime read as 0x00DB261C], then set a refresh byte at
//   [0x00D1FC60+0x11]. i.e. 0x2AA6 STORES coin into an object's fields + flags a reconcile = it
//   hands coin to the player (matches the in-game cursor 5->10). It is NOT the balance-display fill.
// - Fields: pp@+0x33C8, gp@+0x33CC, sp@+0x33D0, cp@+0x33D4 (consecutive, per the handler sequence).
// NEXT: find what writes the CListWnd at REMW+0x310 (the escrow balance rows) and where it reads
// the balance from -- likely a different S->C packet or the 0x2FA0 manage-open reply. Then the
// server sends THAT to show the balance, and uses 0x2AA6 only to hand coin back on withdraw.
// Runtime rebase this session: static-0x20000 (module base 0x003E0000), e.g. 0x5B28B0->0x5928B0.
//
// C->S DEPOSIT recap (already pinned, 43.129 top): 0x2AA6 body {action@0=0 deposit,
// real_estate_id@4, coin_type@0x1B, amount@0x1F}. Withdraw path uncaptured (escrow was empty).
// ============================================================================
//
// 43.129b  ESCROW -- DISPLAY SOURCE FOUND, payout re-confirmed
// ============================================================================
// (All file addresses below are 0x400000-based; this session's module base was 0x003E0000, so
//  runtime = file - 0x20000, e.g. GetEscrowBalance file 0x0077F560 ran at 0x0075F560.)
//
// THE ESCROW TAB'S "Quantity" (col 2 of REMW_Balance_List) IS READ FROM THE MANAGER ESCROW RECORD
// coin fields +0x33C8(pp)/+0x33CC(gp)/+0x33D0(sp)/+0x33D4(cp) via a getter:
#define RealEstate__GetEscrowBalance_x                              0x0077F560  // (coin_type)->amount; map.find on [0x00DD261C], reads rec+0x33C8..D4
// GetEscrowBalance callers (the balance-fill / display users):        0x00673F2A, 0x0075308C, 0x0077FEEF
// The escrow map/data global it (and the 0x2AA6 store path) use: instruction `mov eax,[00DB261C]`
// (runtime); file operand ~0x00DD261C [inferred +0x20000; the 43.129 note's 0x00D1FC1C looks wrong].
//
// *** WHY THE TAB IS EMPTY / how the fill is triggered (CE-proven): ***
// - Opening the manage window runs the ROW-SETUP refresh only:
#define RealEstate__EscrowRowSetup_x                                0x00741930  // this=REMW wnd; builds 4 rows: SetItemData(currency template)+col1=coin name eqstr 0xFB6..0xFB9; col2(Quantity)=EMPTY
//   It does NOT call GetEscrowBalance (BP-confirmed: reopen fires 0x00741930 x3, never 0x0077F560).
//   So window-open shows coin NAMES with blank quantities; the QUANTITY is filled separately, only
//   when an escrow packet arrives (packet-triggered, not window-open).
// - REMW window fields: +0x310 = REMW_Balance_List (CListWnd); +0x32C..+0x338 = 4 currency-template
//   ptrs (shared descriptors, template+0x18 = copper multiplier 1000/100/10/1, NOT per-plot amount);
//   +0x2F0 = per-plot escrow collection (re_id-keyed), NULL until data arrives.
// - REMW_Balance_List columns (EQUI_RealEstateManageWnd.xml): 0=icon, 1="Name", 2="Quantity".
//
// *** PAYOUT RE-CONFIRMED: native S->C 0x2AA6 with a REAL balance pays that coin
// to the cursor every occupancy (21pp landed, twice) and the tab stayed 0. The 0x2AA6 handler
// (0x005B28B0) only calls the parser 0x00741670 (no coin in the dispatch); the parser's action-0
// STORE path (0x00741734 -> rec+0x33Cx) reads statically coin-free, yet in game it still paid.
// UNRESOLVED CRUX: either the sent packet mis-parsed into the give branch (action-2, 0x005B..) OR rec+0x33C8
// is a "coin owed" buffer that a reconcile step both PAYS and shows. Must resolve before a native
// no-coin display is possible. ***
//
// C->S action codes (observed on the wire; supersedes an earlier guess
//   that had 7=Add Day / 4=Withdraw All): 0x2AA6 body {action@0, re_id@4}:
//   action 0 = deposit (39B, coin_type@0x1B, amount@0x1F); 1 = withdraw one currency (39B, same coin
//   fields, from the escrow-list Quantity popup); 3 = Add Day; 4 = Add Week; 7 = Withdraw All.
//   On manage-window open the client also emits unmapped
//   0x2A92 (empty), 0x6D6E (empty), 0x74F4 (14B: dw 0x226, dw 0x11A0, 0) = "window opened" notes.
//   There is NO client request for the balance -> the display packet is server-PUSHED, not requested.
//
// slot, so escrow does NOT ride the estate detail on RoF2 (unlike 64-bit live) -- it is its own path.
// ============================================================================

// ============================================================================
// 43.130  REAL-ESTATE ITEM-DEFINITION FIELDS -- the intrinsic per-item data
//         that classifies a placed object (house / crate / furniture / etc.)
//        , confirmed empirically vs peq.items + live tooltips + wire)
// ============================================================================
// The Items window (0x087F feed) buckets each placed object by REAL-ESTATE data
// carried by the ITEM DEFINITION -- not by anything in the content feed.
// Stock EQEmu never loads or serializes these fields, so under it every object
// goes out with them blanked. The data lives UNUSED in peq.items UNK columns
// (the tail block UNK220..UNK241). CONFIRMED so far:
//
//   * peq.items.UNK223 = REAL-ESTATE PLACEMENT CATEGORY (granular). Verified by
//     grouping the whole table: 15=House (157 rows, all "* Style * Domicile"),
//     16=Storage/Moving Crate (all 12 crates), 35/36/37=Guild Halls
//     (standard / grand-modest / palatial). Also 8=furniture (Draconic/Teir`Dal
//     Chair,Sofa,Bed,Table), 2=small woodcraft, 11-14=building blocks/carvings,
//     17=shrubs, 19=gazebos, 21=banquet, 23/24/25=holiday, 31=rugs/gargoyles,
//     etc. So it is the placement "kind" used for the window filters.
//
//   * peq.items.UNK224 = PLACEABLE-WHERE BITMASK -> client itemdef +0x5AC. This
//     is the AUTHORITATIVE placeable field; the peq.items.`placeable` 0/1 column
//     is STALE (never back-filled -- most live items are placeable yet read 0
//     here, so DO NOT trust `placeable`). Bits (from FUN_0069E740 / the "placeable
//     in" tooltip, 43.28i): 1=neighborhoods, 2=guildhalls, 4=houses, 8=yards,
//     0x80=guildyards. Proven: Evantil's Abode UNK224=8 -> "placeable in yards";
//     Selyrah Saddle UNK224=142 (=0x8E=8|0x80|4|2) -> "placeable in yards, guild
//     yards, houses and guild halls" -- an exact tooltip match. Predicates:
//     is-real-estate FUN_007B0B10 = (itemdef+0x5AC != 0); bit test FUN_007B0AF0.
//
//   * peq.items.UNK227 = REAL-ESTATE DEFINITION ID -> client itemdef +0x5B8
//     ("this item CONTAINS a real estate"). Nonzero for EXACTLY the 173 container
//     rows = 157 houses + 12 crates + 4 guild halls; ZERO for all furniture/decor.
//     This is THE house/crate discriminator. Houses share floorplan defs (27
//     distinct ids across 157 rows -> room-count/model), crates are unique (12/12).
//     Client reads it via FUN_007B0B50 (returns itemdef+0x5B8) then
//     DefinitionTypeOf (0x007F4FA0) -> the estate TYPE (2/3 house, 7 crate; 43.130
//     classifiers 0x7F6030/60F0/61B0). So UNK227 -> DefinitionTypeOf -> section.
//
// NOT YET PINNED (do NOT wire these blind):
//   * The PET/NPC flag. Client is-NPC FUN_007B1640 = (itemdef+0x5CC != 0 &&
//     itemdef+0x608 == 0). Semantically a "pet" = an item that SPAWNS A CREATURE:
//     confirmed live that ALL MOUNTS (saddles) and familiars count as pets, while
//     furniture does not. The UNK column feeding +0x5CC is NOT confirmed -- UNK232
//     (its position by naive contiguous alignment) is 0 for every row, so the pet
//     flag is either a different column or derived from the summon effect.
//   * The scale range floats (UNK228=0.1 / UNK229=2.5 seen on the crate, both on
//     the wire) are probably placement scale min/max but unconfirmed.
// ============================================================================

// ============================================================================
// 43.131  CHILD ESTATES -- how the House/Crate Items-window SECTIONS are really
//         driven, and the exact server->client spec (CE-VERIFIED on RoF2 client
//         + live yard record,. Sections are PER-ESTATE, not per-item.
// ============================================================================
// The Items window sorts each object by ITS ESTATE'S TYPE (DefinitionTypeOf of
// that estate's def id). A plot with only a yard estate (type 4) puts EVERYTHING
// in Yard; House/Crate = N/A is CORRECT until a child estate of type 2/3 (house)
// or 7 (crate) exists. This matches live: placing a house/crate spawns a nested
// real-estate ("<yard address>, <item name>" -- 43.130 live capture).
//
// ESTATE RECORD LAYOUT (CE-read live, a yard record):
//   +0x00 vftable        +0x14 real_estate_id
//   +0x1c CHILD/next estate id (0 = none; FindChildEstate walks this chain)
//   +0x20 definition_id  (=100; DefinitionTypeOf(100)=4=Yard)
//   +0x2c/+0x30 object-list ptrs   +0x3c object count (=3: crate+house+chair)
//   Manager (pinst 0x00E64570 -> 0x2708A238 this session): hash buckets +0x04,
//   size +0x08 (20011), current type +0x90 (=4), current id +0x98,
//   fallback parent +0x8c, definitions catalog +0x74.
//
// THE PARENT->CHILD WALK -- FindChildEstate 0x004D6790(mgr, estate, typeMask, depth):
//   type = DefinitionTypeOf(estate[0x20]); if (typeMask & TypeToBitmask(type)) return estate;
//   else estate = mgr.hash[ estate[0x1c] ]; recurse (depth<=10).
//   ResolveCurrentChild 0x005B00E0 sets mgr+0x98 = matched child's +0x14.
//   TypeToBitmask 0x007F5E30 = 1<<(type-1); owner mask FUN_007F62B0 = 0x88 (types 4|8).
//
// ESTATE-RECORD CREATE opcode = 0x63FD (dispatch 0x005B2500): allocates 88 bytes
// (push 0x58), deserializes the record via vtbl+0xc from the packet, inserts into
// the manager via mgr vtbl+0x24. (0x6FD0 also constructs one.) Content feed 0x087F
// (0x005B0E30) / 0x1CEF (0x005B0D00) find-or-create by real_estate_id and append items.
//
// SERVER SPEC TO MAKE A CRATE (or HOUSE) SECTION APPEAR (verify-not-guess):
//   1. Catalog (0x2698) must carry a definition whose def+0x3C = 7 (crate) / 2|3 (house).
//   2. Send 0x63FD for the child estate: +0x14=new child id, +0x20=that type-7 def id,
//      address "<yard>, <item name>", and set the parent yard record +0x1c -> child id.
//   3. Feed the item via 0x087F with real_estate_id = the CHILD id (not the yard id) -> its
//      row Type = DefinitionTypeOf(child def) = 7 -> lands in the Crate section.
// No item-serialization change is required for the sections; item RE fields (43.130
// UNK224/UNK227) only gate placeability + tell the SERVER an item is a container.
// ============================================================================


// ============================================================================
// 43.132  RealEstateNeighborhoodWnd -- COMPLETE CONTROL + PACKET MAP
// ----------------------------------------------------------------------------
// The gate menu ("Neighborhood Search"). Recovered by decompiling the
// ctor + WndNotification and cross-checking every claim against live packet
// captures and a live populated window read with CE. Nothing here is inferred
// unless it says so.
//
// Opened by: the server answering a door click on the opentype-159 gates
// (guild-lobby OBJ_IRONGATESWITCH, neighborhood OBJ_VOIDPORTAL) with the 0x2691
// neighborhood-list form (43.13).
#define RealEstateNeighborhoodWnd__ctor_x                           0x00749560
#define RealEstateNeighborhoodWnd__WndNotification2_x               0x0074A3A0
#define RealEstateNeighborhoodWnd__ApplyShowFilters_x               0x00749CD0  // local re-filter, sends NOTHING
#define RealEstateNeighborhoodWnd__ResetRequest_x                   0x00749500  // clears the request scratch
//
// ---- MEMBER MAP (from the ctor's literal control names; byte offsets) -------
// Verified live with CE: +0x22C held the header id the server sent and +0x228
// held the id of the highlighted row.
//   +0x218  u32   0x4D constant set by the ctor [unidentified]
//   +0x228  u32   SELECTED neighborhood id, -1 = none.
//   +0x22C  u32   header/current id (echoed as field 2 of every 0x2691 request)
//   +0x230  ptr   RENW_Search_EditBox
//   +0x234  ptr   RENW_SearchType_Combobox   (on change: clears the edit box)
//   +0x238  ptr   RENW_MyPlots_Button
//   +0x23C  ptr   RENW_MyItems_Button
//   +0x240  ptr   RENW_Search_Button
//   +0x244  ptr   RENW_Go_Button
//   +0x248  ptr   RENW_NeighborhoodList      (the main list; fill kind 0/1)
//   +0x24C  ptr   RENW_PlayerList            (right pane; fill kind 2)
//   +0x250  ptr   RENW_GuildList             (right pane)
//   +0x254  ptr   RENW_ShowNonGuild_Btn
//   +0x258  ptr   RENW_ShowGuild_Btn
//   +0x25C  ptr   RENW_ShowFullPPlots_Btn
//   +0x260  ptr   RENW_ShowOpenPPlots_Btn
//   +0x264  ptr   RENW_ShowFullGPlots_Btn
//   +0x268  ptr   RENW_ShowOpenGPlots_Btn
// NOTE the buttons are NOT in XML order -- MyPlots/MyItems come BEFORE Search in
// the ctor. Reading them in XML order mislabels three controls.
//
// ---- WHAT EACH CONTROL SENDS -----------------------------------------------
//   Go              -> C->S 0x218C, 12-byte body (below). Silently does NOTHING
//                      when +0x228 == -1 (no row selected).
//   MyPlots         -> C->S 0x2691 request, subtype 0
//   MyItems         -> C->S 0x2691 request, subtype 1
//   Search          -> C->S 0x2691 request, subtype 0, search text placed in one
//                      of three string slots chosen by the combobox index
//   row click       -> C->S 0x2691 request, subtype 2, field2 = clicked row id;
//                      skipped when the clicked id already equals +0x228, so the
//                      server sees ONE request per genuine selection change
//   6 Show boxes    -> ApplyShowFilters(1) ONLY. No packet: the client filters
//                      the rows it already has. The server must therefore send
//                      the FULL row set and never try to honour these.
//
// ---- C->S 0x218C -- TRAVEL TO REAL ESTATE (12-byte body, 14 on the wire) ----
// *** CONFIRMED TWICE: decompiled here AND byte-identical to live's widened twin
// *** 0x9503 (12B) `e7 aa a4 02 | ff ff ff ff | 00 00 00 00` = {id, -1, 0}.
//   +0x00  u32   real-estate id.  From THIS window = the NEIGHBORHOOD id
//                (whatever the server put in the row's id field).
//   +0x04  u32   secondary id. 0xFFFFFFFF = none.
//   +0x08  u32   0
// 0x218C IS GENERIC, NOT NEIGHBORHOOD-SPECIFIC. The Items window sends the same
// opcode from its right-click menu case 1 (sender 0x0073E752) with
//   +0x00 = ItemsWnd+0x384, +0x04 = ItemsWnd+0x388, +0x08 = 0
// i.e. "take me to this real estate", estate id + a secondary object id. A server
// handler must therefore disambiguate by id, not assume a neighborhood.
//
// ---- C->S 0x2691 REQUEST BODY (224 bytes; 226 on the wire) ------------------
// Reconstructed from the stack layout the sender copies (0x38 dwords from
// aiStack_ec). Consistent with 43.12's partial map of the same struct.
//   +0x00  u32      subtype: 0 = MyPlots/Search, 1 = MyItems, 2 = row selected
//   +0x04  u32      neighborhood id (+0x22C, or the clicked row id when subtype 2)
//   +0x08  u32      [unidentified, 0 in every observed path]
//   +0x0C  char[80] string slot A -- receives the search text when combobox == 2
//   +0x5C  char[64] string slot B -- combobox == 0, and the MyPlots/MyItems paths
//   +0x9C  char[64] string slot C -- combobox == 1
//   +0xDC  4 bytes  trailer (43.12: varies with the result cap)
// The combobox picks the slot, so the SLOT is the search field (Player / Guild /
// Neighborhood). Exact index->slot naming is [inferred] from the drop-down order
// and is the one thing here still worth confirming with a capture.
//
// ---- THE REPLY -------------------------------------------------------------
// List/panes: 0x2691 neighborhood form, see 43.13 (row block VERIFIED in game).
//   fill kind 0/1 -> +0x248 NeighborhoodList,  kind 2 -> +0x24C PlayerList.
//   Live's subtype-2 answer carried plain player names, matching the Player Plots
//   pane exactly.
// Travel: live answers 0x218C's twin with 0x9352 (176B) =
//   {u32 token, u32 -1, float y, float x, float z, float heading, u32 16, char name[]}
// whose floats were y=-2958 x=2042 z=6 h=481 -- i.e. the same point as the
// PEQ zone safe spot for zone 712 'neighborhood' (safe_x 2035, safe_y -2940,
// safe_z 6). Live's packet is (y,x,z,heading) ordered.
// ============================================================================


// ============================================================================
// 43.133  NEIGHBORHOOD ARRIVAL VOLLEY -- what live sends on entering a
//         neighborhood (guild hall -> gate -> select a public neighborhood ->
//         zone in -> run onto an unowned plot). Live opcodes are the 64-bit
//         numbers; map them to RoF2 by STRUCT AND FLOW, never by value.
// ----------------------------------------------------------------------------
// Everything below is [LIVE] unless marked otherwise.
//
// ---- THE ID SCHEME: PLOT IDS ARE ALLOCATED CONTIGUOUSLY AFTER THE HOOD ------
// The travel request selected neighborhood id 0x02A4AAE7. The plot the player
// then walked onto reported id 0x02A4AB2E. The difference is exactly 0x47 = 71,
// the plot count of a neighborhood. So live allocates a neighborhood id and then
// its 71 plot ids immediately after it, one contiguous block per neighborhood.
// Public hoods live low (0x02A4xxxx), guild hoods high (0x07EDxxxx) -- see 43.132.
//
// ---- SetCurrent EQUIVALENT (live 0xA528, 76 bytes) = RoF2 SetCurrent (0x6FD0)
// Sent twice in the capture, and it is the packet that drives mgr+0x90:
//   +0x00  u32   real-estate id
//   +0x04  u32   \  one of these two is the TYPE and the other a sub-selector;
//   +0x08  u32   /  observed pairs below
//   +0x0C  char[] owner name, NUL padded  (empty for an unowned plot)
//   +0x44  u32   the id repeated
//   arriving in the neighborhood : id 0x07EE04E6, (0, 5), name = the hood owner
//   standing on an unowned plot  : id 0x02A4AB2E, (2, 0), name empty
// The 5 is the neighborhood type that 43.10 already proved is the whole gate for
// "the client believes you are in a neighborhood" (mgr->[0x90] == 5), and the 2
// is the House type from DefinitionTypeOf (43.131). So arrival sets type 5 for
// the hood, and walking onto a plot re-points SetCurrent at the PLOT with its own
// type.
//
// ---- LOT-SQUARE AREA LIST (live 0xCB57, 32912 bytes)
// Flat array, NO count header (length / stride gives the count):
//   stride 136 bytes, 242 records in this capture
//   +0x00  char[22]  "OBP_LOTSQUARE" NUL padded  <- the door model name
//   +0x16  u16       lot id (0x024C, 0x0356, ... ) [inferred: the zone's door id]
//   +0x20  float y, +0x24 float x, +0x28 float z, +0x2C float heading
//   +0x34  the same four floats repeated
//   +0x48  u32 100,  +0x50 u32 1,  +0x54 u32 160,  +0x58 u32 -1,  +0x64 u32 101
//   remainder zero
// The first record reads y=-218.06 x=332.21 z=0.127 heading=256, which matches
// the PEQ lot square at x=332.21 y=-219.06 -- confirming live orders these
// packets (y,x,z,heading).
//
// ---- MASS PLACED-OBJECT FEED (live 0xA83D, 813,295 bytes) -------------------
//   +0x00  u32       object count (0x57 = 87 here)
//   then per object the 16-char placement key ("5k4X06O0000N6m80") and the same
//   record shape as the single-object placement feed (live 0xEA09).
// So live pushes EVERY placed object in the whole neighborhood on zone-in, in one
// packet, not per-plot on approach.
//
// ---- TELEPORTER LIST (live 0x0950, 1444 bytes) ------------------------------
//   +0x00  u32 count (0x2C = 44), then records of
//   { u32 flag, float y, float x, float z, float heading, u32 zone_id, ... }
//   first record: (936, 294, -19, 254) zone 344 = guildlobby.
// These are the in-neighborhood teleport pads -- the 2010 walkthrough's "7
// sections of ~10 plots each, each with a little teleporter", plus the exit back
// to the guild lobby.
//
// ---- OTHER TRAFFIC IN THE SAME VOLLEY (not yet needed) ----------------------
//   0xD4B8  75,776 B  zone/object data, first packet after entry
//   0x9A4E   6,608 B  float-heavy, [inferred] placement/collision volumes
//   0xDF82   1,550 B  achievement-style name/id table ("We Have Ways of...")
//   0x5AF5     825 B  resident roster: {count, name, u32 id, u32 102, ...}
//   0x3EF7   4,184 B  x2, paged, keyed by the same {id, 102} pair
// The recurring 102 next to a real-estate id (it is also the unnamed constant in
// every 43.132 list row) reads as the NEIGHBORHOOD DEFINITION id. [inferred, but
// consistent across four independent packet types.]
// ============================================================================


// ============================================================================
// 43.134  THE PARENT/CHILD DIRECTION -- resolving the parked "House/Crate
//         section" question, and the same mechanism the neighborhood needs
// ----------------------------------------------------------------------------



//     record +0x4C = MY child ids[]   /  trailing ids[] of the 0x2698 DEFINITION
//
// The failed test sent the crate as a "child estate" and then poked the YARD's
// +0x1C to the crate id. That does not adopt the crate -- it declares the yard to
// be a CHILD OF the crate, which is exactly the symptom observed in game (the
// window navigated into the child and the yard's own items vanished). The poke
// was not "not taking"; it took, in the wrong direction.
//
// CORRECT CONSTRUCTION for a container to appear as its own Items-window section:
//   1. the CHILD (crate/house) record carries +0x1C = the PLOT's id, and its own
//      +0x20 definition whose f09/DefinitionTypeOf is the section type
//      (7 = Crate, 2/3 = House -- 43.131);
//   2. the PLOT's DEFINITION (0x2698) must list that child id in its trailing
//      ids[] array, which is what fills the plot record's +0x48/+0x4C.
// Both halves are required: (1) alone leaves the parent unaware of the child, and
// the section counts are read from the PARENT's child array.
//
// SAME MECHANISM ONE LEVEL UP -- this is how a neighborhood is really formed:
//   neighborhood record  = parent, its definition's ids[] = every plot id
//   each plot record     = +0x1C -> the neighborhood id
// Which is consistent with 43.133's observation that live allocates a
// neighborhood id and then its 71 plot ids as one contiguous block: the ids[]
// array is a dense run.
//
// Depth cap is 10, so neighborhood -> plot -> interior -> item is within budget.
// ============================================================================


// ============================================================================
// 43.135  *** THE PLACED-OBJECT RIGHT-CLICK MENU IS A SERVER ROUND-TRIP ***
//         -- this UNPARKS the menu and CORRECTS 43.x item (D)
// ----------------------------------------------------------------------------
// (D) concluded the world context menu was "client-built (no server packet)" and
// parked it as a 3D scene-pick problem. That was wrong:
//
//     THE CLIENT ASKS THE SERVER FOR THE OBJECT RECORD, AND BUILDS THE MENU
//     FROM THE REPLY. NO REPLY, NO MENU.
//
// The dispatcher proves it. HandleRealEstateMessage (0x005B2500):
//     if (param_2 == 0x21EC) { if (DAT_00D1FCB8 == 0) return;
//                              FUN_007375F0(param_3); return; }
// and FUN_007375F0 is the MENU BUILDER: four CContextMenu::AddMenuItem calls
// (0x0085B7B0) followed by CContextMenuManager::PopupMenu (0x00862220). Those are
// exactly the two functions CE proved never fire -- because the packet that calls
// them never arrives.
#define RealEstate__BuildObjectContextMenu_x                        0x007375F0
#define pinstRealEstateObjectMenuOwnerWnd_x                         0x00D1FCB8  // must be non-null or the handler returns
//
// MENU ITEM IDS START AT 0x65: the builder adds items as (index + 0x65), and the
// Items window's WndNotification switches on (notify_id - 0x65) -- the same menu,
// which is why its case 1 sends the 0x218C travel request (43.132).
//
// ---- THE EXCHANGE (live capture; live opcode 0x5EDB == RoF2 0x21EC;
//      768-byte body BOTH WAYS) -------------------------------------------------
// C->S request (client fills what it knows, zeroes what it wants resolved):
//   +0x000  char[16]  placement key, ASCII "0000000000000000"  <- ALL ZEROS
//   +0x010  u32       0
//   +0x014  10 x 0xFF then 0x0000
//   +0x020  u32       real_estate_id      (live 0x07EE04A6)
//   +0x024  u32       object index/slot   (live 0x25)
//   ...     zero to 768
// S->C reply (same struct, filled):
//   +0x000  char[16]  THE REAL placement key, e.g. "KgO106O00000l1mO0"
//   +0x020  u32       real_estate_id      (echoed)
//   +0x024  u32       object index/slot   (echoed)
//   +0x028  u32       0
//   +0x02C  char[]    item NAME, e.g. "Vial of Distilled Ma..." (NUL-terminated)
//   remaining fields = the rest of the object record (same shape as live's
//   768-byte 0x5EDB "Secondary Anchor" record).
// Wire size is 0x302 = 770 (2 opcode + 768 body); the client copies 0xC0 dwords.
//
// ---- CLIENT-SIDE GATE BEFORE IT EVEN ASKS ----------------------------------
// The sender only fires when EITHER the object is within reach of the player
// (3D distance from pinstLocalPlayer+0x68/+0x6C/+0x70 against +0x13C plus a
// constant) OR the object's estate matches the player's current estate
// (FUN_005B3540()+0x94 == object+0x24). So a right-click on a far-away object is
// dropped client-side and no packet is sent -- worth knowing before concluding
// "the server never got it" during testing.
//
// ---- WHAT THIS MEANS FOR A SERVER --------------------------------------------
// 0x21EC is an OBJECT-RECORD REQUEST / REPLY, not a feed. The server must answer
// it with the same 768-byte struct carrying the real 16-char key (+0x00) and the
// item name, or the menu silently never builds.
// ============================================================================


// ============================================================================
// 43.136  THE 0x21EC MENU BUILDER, FIELD BY FIELD (0x007375F0)
//         -- the actual recipe for making the right-click menu appear
// ----------------------------------------------------------------------------
// Reading the builder body rather than guessing. `param_2` is
// the 768-byte body viewed as dwords, so param_2[n] == body + 4n.
//
//   for (i = 0; i < 0x28; ++i)              // FORTY candidate menu entries
//       entry = &param_2[(i + 7) * 4];      // == body + 0x70 + i*0x10
//       ... build a label, then
//       CContextMenu::AddMenuItem(..., i + 0x65, ...);   // <-- id is INDEX + 0x65
//       ++added;
//   if (added > 0) {                        // *** NO ITEMS => NO MENU AT ALL ***
//       wnd[0x36C] = param_2[0xBE];         // body +0x2F8
//       wnd[0x384] = param_2[8];            // body +0x20  real_estate_id
//       wnd[0x388] = param_2[9];            // body +0x24  object index
//       wnd[0x38C] = param_2[0xBF];         // body +0x2FC
//       if (wnd[0x196] != 0) FUN_00736A00(body, body+0x20, body+0x24);
//       CContextMenuManager::PopupMenu(...);
//   }
//
// So the ENTRY ARRAY IS THE MENU: +0x70 entries, +0x2F8/+0x2FC tail dwords,
// +0x20/+0x24 ids. A reply with populated entries but wrong label ids still pops
// a menu, whose every row reads "ERROR: String not found. (N)" -- the rows are
// real; only the LABEL is wrong.
//
// LABEL SOURCE, two paths depending on the entry's type field:
//     type == 9  -> FUN_004866C0(label_id, 0x1C, 0)     [inferred: a table lookup]
//     otherwise  -> FUN_007D0660(label_id, 0)           [inferred: GetEQStr]
// The "String not found (N)" text is the eqstr failure string, which is what
// proves the second path takes an EQSTR ID. So the label field must carry the
// real ids -- item (D) already recovered them from the live menu:
//     5244 Adjust Placement, 719 Place, 718 Move to Storage,
//     724 Move to Inventory, 720 Inspect, and the Show Items / Help rows.
//
// THEREFORE, for the world right-click menu to appear the server must:
//   1. answer inbound 0x21EC (43.135);
//   2. answer with the 768-byte record: real key at +0x00, ids at +0x20/+0x24,
//      item name at +0x2C;
//   3. fill one 0x10-byte entry at +0x70 + i*0x10 PER MENU ROW you want, each
//      carrying the eqstr label id, and set the +0x2F0/+0x2F4 counts;
//   4. the client assigns each row id (i + 0x65) and pops the menu itself.
// Row activation then arrives back through the Items window's WndNotification,
// which switches on (id - 0x65) -- its case 1 is the 0x218C travel request
// (43.132), so the menu and the window share one notification path.
//
// -- THE ENTRY LAYOUT, READ OFF LIVE'S ACTUAL REPLY
// (live S->C 0x5EDB len 768, "Vial of Distilled Mana"):
//   entry+0x00  u32  0 on row 0, 1 on row 1, 0xFFFFFFFF on all later rows
//                    (meaning still unknown -- MIRROR IT, do not invent)
//   entry+0x04  u32  THE LABEL, an eqstr id -- all seven verified in eqstr_us.txt:
//                    5244 Adjust Placement, 719 Place, 718 Move to Storage,
//                    724 Move to Inventory, 720 Inspect, 5285 Show Items, 13300 Help
//   entry+0x08  u32  0xFFFFFFFF
//   entry+0x0C  u32  action code: 2,3,4,7,0xB,0xE; 0xFFFFFFFF on Help
// Unused entries (7..39) are 0xFF-FILLED, not zero-filled; the 40th entry's last
// dword (+0x2EC) is 0. Tail: +0x2F0=691, +0x2F4=346, +0x2F8=10253 (record-specific,
// undeciphered), +0x2FC=0, and +0x6C=2 before the array.
// The earlier [inferred] shape (flag 1 / label in dword2 / zero fill / +0x2EC=1 /
// row counts in the tail) fed the client's ITEMS-WINDOW populate branch instead of
// symptom.
// +0x2F0/+0x2F4 DECIPHERED (menu popped top-left until this): they are the CLICK'S
// SCREEN X/Y. The client sends its mouse position in the REQUEST (691/346 in the
// capture) and the reply must echo it -- that echoed pair is where PopupMenu puts
// the menu. Request +0x2F8 = -1; the reply's 10253 there is still undeciphered.
// Still unknown: what wnd[0x196] gates.
// ============================================================================


// ============================================================================
// 43.137  RealEstateItemsWnd -- COMPLETE CONTROL MAP (from the ctor, 0x00733E00)
// ----------------------------------------------------------------------------
// The main housing window. Names are the literal XML item names the ctor looks up,
// so these are exact, not inferred. pinst is 0x00D1FCB8 -- the same pointer the
// 0x21EC context-menu handler null-checks (43.135), i.e. NO ITEMS WINDOW => NO
// RIGHT-CLICK MENU.
//
//   +0x2A4  REIW_YardItemsCount                      \
//   +0x2A8  REIW_YardNonRealEstateItemsCount          |  THE SECTION COUNTERS
//   +0x2AC  REIW_YardNpcItemsCount                    |  (see the note below)
//   +0x2B0  REIW_HouseItemsCount                      |
//   +0x2B4  REIW_HouseNonRealEstateItemsCount         |
//   +0x2B8  REIW_MovingCrateItemsCount                |
//   +0x2BC  REIW_MovingCrateNonRealEstateItemsCount  /
//   +0x2C0  REIW_SelectedItemLabel
//   +0x2C4  REIW_SelectedItemLocation
//   +0x2C8  REIW_SelectedItem
//   +0x2CC  REIW_ShowStoredButton        +0x2D0  REIW_ShowPlacedButton
//   +0x2D4  REIW_ShowYardButton          +0x2D8  REIW_ShowHouseButton
//   +0x2DC  REIW_ShowMovingCrateButton   +0x2E0  REIW_ShowPlaceableButton
//   +0x2E4  REIW_ShowUnplaceableButton   +0x2E8  REIW_ShowOwnedItemsButton
//   +0x2EC  REIW_ShowUnownedItemsButton
//   +0x2F0  REIW_Move_Inventory_Button   +0x2F4  REIW_Move_Closet_Button
//   +0x2F8  REIW_Move_Shed_Button        +0x2FC  REIW_Move_Crate_Button
//   +0x300  REIW_Place_Button            +0x304  REIW_Locate_Button
//   +0x308  REIW_Stow_Placed_Button      +0x30C  REIW_Pack_Moving_Crate_Button
//   +0x310  REIW_Unpack_Moving_Crate_Button          +0x314  REIW_Trophies_Button
//   +0x324  REIW_LayoutName_EditBox
//   +0x328  REIW_LayoutSaveAll_Btn       +0x32C  REIW_LayoutSaveYard_Btn
//   +0x330  REIW_LayoutSaveHouse_Btn     +0x334  REIW_LayoutLoadAll_Btn
//   +0x338  REIW_LayoutLoadYard_Btn      +0x33C  REIW_LayoutLoadHouse_Btn
//   +0x340  REIW_LayoutDetails_Btn       +0x344  REIW_LayoutDelete_Btn
//   +0x348  REIW_LayoutUsePlacedItems_Btn
//   +0x34C  REIW_LayoutUseInventoryItems_Btn
//   +0x350  REIW_LayoutStoreExtraItems_Btn
//   +0x354  REIW_LayoutRotateYard_Combobox
// (REIW_ItemList and REIW_LayoutList are also looked up; their member slots sit
// between the button runs -- read them off the ctor when needed.)
//
// *** WHAT THIS SHOWS ABOUT SECTIONS ***
// The window keeps THREE section families -- Yard, House, MovingCrate -- and each
// family counts TWO kinds, "Items" and "NonRealEstateItems". Only the YARD has a
// third counter, REIW_YardNpcItemsCount. So:
//   * "pets" is not a category an item can be put into -- it is a YARD-ONLY
//     counter for NPC items;
//   * there is no separate "Crate" section list to populate -- the crate family is
//     MovingCrate, and its counters only move when items are parented to a moving
//     crate estate, which is the parent/child construction in 43.134.
// ============================================================================


// ============================================================================
// 43.138  THE REMAINING HOUSING WINDOWS -- control maps from their ctors
// ----------------------------------------------------------------------------
// Same method as 43.132/43.137: the ctor looks each control up by its literal XML
// name, so these pairings are exact.
//
// ---- RealEstateManageWnd (ctor 0x00746620, pinst 0x00D1FCBC) ---------------
// ACCESS PAGE
//   +0x2D4 A_Access1   +0x2D8 A_Access2   +0x2DC A_Access4   +0x2E0 A_Access5
//   +0x2E4 A_Access6   +0x2E8 A_Access7   +0x2EC A_Access8
//     ^ SEVEN access row controls, numbered 1,2,4,5,6,7,8 -- the same seven slots
//       the 0x485D/0x3146 record carries (43.128E). The naming gap is in the XML,
//       not in the wire format; do not renumber the wire to match.
//   +0x258 REMW_Subwindows              +0x25C REMW_AccessPage
//   +0x260 REMW_ViewIndividualPermissions_Btn
//   +0x264 REMW_ViewOwnerClubsPermissions_Btn
//   +0x268 REMW_ViewGroupTotals_Btn     +0x26C REMW_ViewOwnerClubsPermissions_Label
//     ^ the three Access views (Individuals / Owner's Clubs / Group Totals)
//   +0x270 REMW_Name_List               +0x274 REMW_OwnerClubs_List
//   +0x278 REMW_GroupTotals_List        +0x27C REMW_Permissions_List
//   +0x280 REMW_Permissions_Checkbox    +0x284 REMW_Groups_Combobox
//   +0x288 REMW_Name_EditBox            +0x28C REMW_Access_Combobox
//   +0x290 REMW_Add_Btn                 +0x294 REMW_Remove_Btn
// ESCROW PAGE (43.129)
//   +0x2F0 REMW_EscrowProperty_Combobox +0x2F4 REMW_Property_Label
//   +0x2F8 REMW_Transfer_Btn            +0x2FC REMW_Upkeep_Combobox
//   +0x300 REMW_TimeRemainingVal_Label  +0x308 REMW_LimitedItemVal_Label
//   +0x30C REMW_DailyCharge_Label       +0x310 REMW_Balance_List
//   +0x314 REMW_Upkeep_List             +0x318 REMW_WithdrawAll_Btn
//   +0x324 REMW_AddDayTemplate_Btn      +0x328 REMW_AddWeekTemplate_Btn
//   +0x32C A_PlatinumCoin  +0x330 A_GoldCoin  +0x334 A_SilverCoin  +0x338 A_CopperCoin
//   +0x344 REMW_Eviction_Label          <- the eviction warning line
//
// ---- RealEstatePlotSearchWnd (ctor 0x0074B470, pinst 0x00D1FCC4) -----------
//   +0x224 REPSW_Search_EditBox   +0x228 REPSW_Search_Btn
//   +0x22C REPSW_MyPlots_Btn      +0x230 REPSW_MyItems_Btn
//   +0x234 REPSW_Go_Btn           +0x238 REPSW_PlotList
//   +0x23C REPSW_ShowPlayer_Btn   +0x240 REPSW_ShowGuild_Btn
//   +0x244 REPSW_ShowOwned_Btn    +0x248 REPSW_ShowVacant_Btn
// NOTE THE SHAPE: this is the Neighborhood window one level down -- same
// Search / MyPlots / MyItems / Go quartet, same local-filter Show buttons, but
// listing PLOTS instead of neighborhoods. Its Go therefore sends 0x218C carrying
// a PLOT id, which is exactly why 43.132 found 0x218C to be generic and why a
// travel handler must resolve "neighborhood id OR plot id" rather than assume.
//
// ---- RealEstateLayoutDetailsWnd (ctor 0x005AC730, pinst 0x00D1FCCC) --------
//   +0x238 RELDW_ItemList          +0x23C RELDW_Load_Combobox
//   +0x240 RELDW_UsePlacedItems_Btn +0x244 RELDW_UseInventoryItems_Btn
// Small by design: it is the detail pane behind the Items window's Layout block
// (REIW_Layout* at +0x324..+0x354, 43.137). See 43.138a: layouts are client-side.
// ============================================================================


// ============================================================================
// 43.139  *** HOW AN ITEM GETS ITS SECTION -- the categorisation chain closed ***
// ----------------------------------------------------------------------------
// The section an item lands in needs no item-serialisation change at all.
//
// THE COUNTERS ARE A CONTIGUOUS ARRAY, FILLED BY A MAP WALK. In the Items-window
// populate (FUN_00744xxx region):
//     for (i = 0; i < map->count(+0x40); ++i) {
//         bucket = FUN_007489B0(map, i);          // value of the i'th map entry
//         *(wnd + 0x2A8 + bucket * 4) += 1;       // <- the section counters, 43.137
//     }
//     ... FUN_00748910(map, i) returns that entry's KEY (a name); rows whose name
//         begins with '_' are skipped (the same reserved-name convention the access
//         table uses for "_Guild<rank>", 43.128E).
#define RealEstateItemsWnd__MapValueAt_x                            0x007489B0
#define RealEstateItemsWnd__MapKeyAt_x                              0x00748910
// FUN_007489B0 is NOT a classifier -- it is a hash-map iterator (buckets +0x38,
// size +0x3C) returning the stored value. So THE SECTION IS DATA THE CLIENT WAS
// GIVEN, not something it derives from the model or the item type.
//
// WHERE THAT DATA COMES FROM: the content feed. Each feed row's real_estate_id
// MUST match one of the Items window's yard/house/crate ids or the row is dropped.
// The window holds THREE estate ids -- yard, house, moving crate -- and an item is
// counted in whichever family its feed row's real_estate_id matches. That is the
// entire mechanism:
//
//     SECTION == WHICH ESTATE THE ITEM IS FED UNDER.
// ============================================================================


// ============================================================================
// 43.140  THREE UNDOCUMENTED OPCODES THE ITEMS WINDOW SENDS + the menu-action
//         dispatch behind them (from RealEstateItemsWnd::WndNotification 0x73CC50)
// ----------------------------------------------------------------------------
// Everything this window can send: 0x3424 ItemMove (x5 sites), 0x626E Place (x3),
// 0x2036 MoveToInventory (x3), 0x21EC context menu (43.135), 0x218C travel
// (43.132), and THREE that were not in EQEmu's patch_RoF2.conf at all:
//
//   0x7AD1  body 148 bytes (wire 0x96 = 150; a second site sends 44/wire 0x2E)
//           +0x00 u32 real_estate_id   (wnd+0x384)
//           +0x04 u32 real_estate_item_id (wnd+0x388)
//           +0x08 140 bytes explicitly memset to 0 before send
//   0x3C8B  body 352 bytes (wire 0x162 = 354)
//           +0x00 u32 = 2   (a mode/verb selector)
//           +0x08 u32 real_estate_id
//           +0x0C u32 real_estate_item_id
//           +0x10 char[0x80] a STRING built just before the send
//           Fires only from a CONFIRM DIALOG: notification type 0x26 with
//           param 1 and DAT_00D1FCF4 non-null. A dialog + a typed name is the
//           signature of SAVE LAYOUT [inferred -- the verb is not proven].
//   0x58C7  body 8 bytes (wire 10)
//           +0x00 u32 real_estate_id   +0x04 u32 real_estate_item_id
//           NOTE 0x58C7 ALSO has an S->C handler in the dispatcher (0x005B2C7A),
//           so it is a request/reply pair, not one-way.
//
// ---- THE MENU-ACTION DISPATCH (this is the other half of 43.136) -----------
// Row activation arrives as notification id (0x65 + i), and the handler does:
//     i = param_4 - 0x65;  if (i > 0x27) ignore;
//     switch (*(u32 *)(wnd + (param_4 - 0x0B) * 0x10)) { ... }
// i.e. it switches NOT on the row index but on an ACTION CODE the window stored
// for that row at wnd + (0x5A + i) * 0x10. Observed arms:
//     case 0 -> toggle a window via vt+0xD8 on pinst 0x00D1FCBC (Manage)
//     case 1 -> send 0x218C   (travel to this real estate)
//     case 2 -> FUN_0073F720 (local placement/move path, no packet)
//     case 5 -> send 0x7AD1
// So the SERVER controls which action each menu row performs, because the action
// code lives in the same 0x10-byte entry the 0x21EC reply supplies (43.136). That
// is the missing half of the menu recipe: entry = { flag, ids..., label eqstr id,
// action code } -- and it means a wrong action code yields a menu whose rows do
// nothing, distinct from a wrong label id which yields "String not found".
// ============================================================================


// ---------------------------------------------------------------------------
// 43.140a  0x58C7 identified: ITEM INSPECT (the menu's "Inspect" row)
// ---------------------------------------------------------------------------
// The inbound half (dispatcher 0x005B2500, case 0x58C7) does:
//     obj = FUN_005559A0(&scratch, &stream);   // deserialize an ITEM from the packet
//     if (obj) { b = FUN_00875BD0();           // some display preference
//                FUN_006AE610(&obj, !b); }     // hand it to the item display
// and the OUTBOUND half in the Items window reaches the same FUN_006AE610 with a
// locally-resolved item when it already has one. So the exchange is:
//     C->S 0x58C7 {u32 real_estate_id, u32 real_estate_item_id}   "describe this"
//     S->C 0x58C7 <serialized item>                               -> item window
// i.e. it is the placed-object equivalent of an item link / inspect, which lines
// up with the context menu's Inspect row (eqstr 720). [inferred from the call
// shape -- deserialize-then-display -- not yet confirmed on the wire, but the
// body {estate, item} and the menu row leave little else it could be.]
//
// SERVER IMPLICATION: answering this needs a normal item serialization for the
// placed object's item_id, addressed by (real_estate_id, real_estate_item_id).
// Until it is answered, Inspect on a placed object does nothing even once the
// menu itself appears.
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// 43.138a  CORRECTION: the LAYOUT system is CLIENT-SIDE, not a server subsystem
// ---------------------------------------------------------------------------
// 43.138 called the layout system "the largest untouched housing subsystem" on
// the server. That is WRONG and is corrected here before anyone budgets work for
// it. Reading RealEstateItemsWnd::WndNotification for the layout controls:
//
//   Save  (REIW_LayoutSaveAll/Yard/House_Btn, +0x328/+0x32C/+0x330):
//       scope = FUN_007F62B0() | FUN_00410C30();      // All
//       if (btn == SaveYard)  scope = FUN_007F62B0(); // yard bit only
//       if (btn == SaveHouse) scope = FUN_00410C30(); // house bit only
//       ... reads REIW_LayoutName_EditBox (+0x324), then FUN_007300A0 /
//           FUN_007339A0  -- NO opcode encode, NO send.
//   Load  (REIW_LayoutLoadAll/Yard/House_Btn, +0x334/+0x338/+0x33C):
//       two booleans (yard, house) set the same way, plus the three toggle
//       states UsePlacedItems (+0x348) / UseInventoryItems (+0x34C) /
//       StoreExtraItems (+0x350) read from their +0x1E4 checked byte, then
//       FUN_00731710 / FUN_0073BEC0 -- again NO packet.
//
// NOT ONE of the layout buttons encodes an opcode. The whole feature is local:
// it serialises to the client's own RE_Layout_*.xml (which is exactly why the
// live capture of a layout was an XML FILE, not a packet stream), and it then
// drives the ORDINARY place/move opcodes (0x626E, 0x3424,
// 0x2036) to realise the layout.
//
// SERVER WORK REQUIRED FOR LAYOUTS: none of its own. What layouts need is that
// place/move/remove behave correctly. The
// only genuinely server-side unknowns left in the Items window are the three
// opcodes in 43.140 (0x7AD1, 0x3C8B, 0x58C7) -- and 0x3C8B's confirm-dialog +
// typed string, previously guessed to be "save layout", must therefore be
// something else, since saving a layout sends nothing at all.
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// 43.133a  NEIGHBORHOOD ZONE DOORS -- the teleport pads are ordinary doors
// ---------------------------------------------------------------------------
// 43.133 noted live's 44-entry teleporter list. PEQ's doors for zone
// 'neighborhood' version 0 show the pads are ordinary doors:
//
//   opentype  57  x42   OBJ_TELEPADA..G, dest_zone 'neighborhood'
//                        -> intra-zone teleports, handled by the STOCK door path
//   opentype  55  x18   OBJ_TELEPADx / OBJ_TPBASE / a few OBP_LOTSQUARE, dest NONE
//                        -> decorative bases
//   opentype  58  x1    OBJ_GUILDGATE, dest_zone 'guildlobby'  -> the way out
//   opentype 159  x1    OBJ_VOIDPORTAL   <- THE ONLY 159 IN THE ZONE
//   opentype 160  x73   OBP_LOTSQUARE    -> the player plot grid
//   opentype 160  x4    OBP_GUILDSQUARE  -> the FOUR guild plots
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// 43.140b  RETRACTION: 43.140/43.140a rediscovered opcodes THAT WERE ALREADY
//          DOCUMENTED, and one of its guesses contradicts better evidence.
// ---------------------------------------------------------------------------
// Read the earlier sections before acting on 43.140:
//   0x7AD1 -> ALREADY 43.61 (items-window item identity; 150 wire / 148 body,
//             sender 0x0073EB3E, reached as context-menu action case 5).
//   0x3C8B -> ALREADY 43.74, and its identification is BETTER than the guess in
//             43.140: it is [inferred] MAIL ITEM, resting on the Mail Item
//             permission bit (43.41) plus a dedicated 0x1228 refusal, code 79
//             "you do not have mail item access" (43.42). 43.140 speculated
//             "save layout"; that is WRONG and 43.138a independently disproves
//             it (saving a layout sends no packet at all). Treat 0x3C8B as Mail
//             Item, not layout.
//   0x58C7 -> ALREADY #defined as OP_RealEstate_TextMessage (see the opcode
//             block) and read elsewhere as "string message -> 0x006AE610 (chat)".
//             43.140a called it item INSPECT from the deserialize-then-display
//             shape. THE TWO READINGS CONFLICT AND NEITHER IS PROVEN: both agree
//             the body is {real_estate_id, real_estate_item_id} and that the
//             inbound half parses something out of the stream and hands it to
//             0x006AE610; they disagree on whether that payload is an ITEM or a
//             STRING, i.e. whether the result is an inspect window or a chat
//             line. DO NOT BUILD ON EITHER until one capture settles it -- click
//             Inspect on a placed object on live and look at what comes back.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 43.141  THE CLIENT'S ZONE DISPLAY NAME IS BAKED INTO THE EXE -- per-instance
//         zone names are impossible on client-side surfaces
//
// "Sunrise Hills" for zone 712 lives in eqgame.exe itself:
//
//   0x00A0B220  .rdata  "Sunrise Hills"   (file offset 0x609A20; 16-byte slot,
//   0x00A0B210  .rdata  "neighborhood"     the short name sits directly before,
//   0x00A0B230  .rdata  "thuledream"       the next zone's entry directly after)
//
// This is the client's internal zone table, keyed by ZONE ID. Character select,
// the loading screen ("Entering ..."), and the map title all read it -- the
// server's NewZone long_name never reaches those surfaces, so a per-instance
// neighborhood name CANNOT be shown there: every instance of zone 712 is the
// same zone id and gets the same string. Server-side surfaces (zone process
// long_name -> /who) are fed from a different place.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 43.142  0x2698 IS A FULL CATALOG REPLACE -- every definitions packet WIPES
//         the whole catalog first, live CE + decompile)
//
// The 0x2698 case in HandleRealEstateMessage (0x5B2500) runs THREE stages:
//     FUN_007F5B60   definition-array reader
//     FUN_007FC6B0   cost-section reader (the 43.31 block loop lives here)
//     FUN_005AAAD0   post-update notify
//
// FUN_007F5B60's FIRST act is FUN_007F5A50 = CATALOG.CLEAR(): it walks every
// bucket of the definition catalog and frees every node before reading a single
// def from the wire. 0x2698 is REPLACE, never merge. Consequences, all observed
// def from the wire. 0x2698 is REPLACE, never merge. Consequences, all observed
// live via CE:
//   - The catalog contains ONLY whatever the most recent 0x2698 carried.
//   - Any def sent earlier is erased by the next 0x2698 that omits it, including
//     child ids added to a def by an earlier send.
//   - A server must therefore send the COMPLETE definition set it wants live in
//     every single 0x2698, or batch-and-resend; per-def incremental sends are
//     self-destructive.
//
// THE DEFINITION CATALOG GLOBAL: mgr+0x74 (RealEstateManagerClient, pinst
// 0xE64570). Found via `mov ecx,[ebx+74]` feeding every DefinitionTypeOf
// (0x7F4FA0) call. Standard {buckets @+4, count @+8} hash; nodes are the def
// objects themselves, intrusive: key at +0x6C, next at +0x70.
//
// DEFINITION OBJECT LAYOUT (from the field reader FUN_007F4C90):
//   +0x00 f00 id          +0x1C f01        +0x20 f02 (price set)
//   +0x24 f03 (upkeep set) +0x28 f04 CAPACITY   +0x2C f05 vault cap
//   +0x30 f06  +0x34 f07  +0x38 f08
//   +0x3C f09 = THE TYPE (DefinitionTypeOf returns this; 4 = plot, 7 = crate)
//   +0x40 f10  +0x0C..    name (CXStr block read at struct dword 3)
//   +0x44 f11  +0x48 f12 PET CAPACITY  +0x4C f13  +0x50 f14  +0x54 f15  +0x58 f16
//   +0x5C ids COUNT   +0x60 ids ARRAY ptr   (the trailing ids[] block)
//   +0x6C map key (id again)   +0x70 map next
// Live-verified against a plot def: +0x3C=4, +0x28=120, +0x48=6, key mirrored at
// +0x00/+0x6C.
//
// RECORD-SIDE: a child record pointing at a parent via +0x1C does NOT populate
// the parent's +0x48/+0x4C child list by itself (see 43.144 for where it comes
// from). Open: whether def ids[] are per-plot child ESTATE ids at all -- a
// definition is shared by many plots, so they may be allowed child DEFINITION
// ids instead.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 43.143  WHY HOUSE/CRATE READ N/A -- the Items window's sections are driven by
//         a per-estate GROUP TABLE,
//         live CE walk + decompile of the populate 0x00744200)
//
// The Items-window populate (0x00744200 -- the ONLY caller of the group
// iterator 0x007489B0) does NOT consult the estate records, the definitions,
// FindChildEstate, or the relation test at window-open (hardware-breakpoint
// verified: zero hits opening the window). Everything it renders comes from
// TWO per-estate objects in the manager:
//
//   mgr+0x54 map[estate_id] -> the GROUP/SECTION object:
//     +0x04..+0x30  six (mask, counter_index) pairs, observed live:
//                   (3,1) (3,2) (0x803,3) (0xE03,4) (0x1F87,5) (0x1FFF,6)
//                   -- location-bit masks per section/filter index
//     +0x38/+0x3C   inner hash: NAMED GROUP entries, keyed by NAME HASH
//                   (0x00808D60 of the group name); group value [0] = its
//                   counter index. The populate's counter loop iterates THIS
//                   (counters at wnd+0x2A8+idx*4 -- one ++ per entry), and the
//                   section list is built from group names via 0x00748910
//                   (names starting '_' are skipped; "Fellowship" special-cased).
//     +0x40         entry count. When 0, the counter loop runs zero times
//                   and House/Crate never leave N/A.
//
//   mgr+0x64 map[estate_id] -> the ROW COLLECTION (vtable obj, estate id +0x04,
//     inner hash +0x18/+0x1C keyed by real_estate_item_id, counts +0x20/+0x24).
//
//   ITEM ROW object (0x3C bytes, reader 0x005ABD20):
//     +0x00 flag   +0x04 f04   +0x08..+0x20 f08[7]
//     +0x24 name CXStr   +0x2C name hash   +0x30 name2
//     +0x34 real_estate_id   +0x38 real_estate_item_id
//
// RealEstateItemGroups.txt (string 0xA10464) is the client-side name for this
// grouping concept. (43.144 completes the House/Crate section chain.)
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 43.144  THE HOUSE/CRATE SECTION CHAIN, COMPLETE -- record wire's INLINE CHILD
//         LIST -> mgr+0x9C/+0xA0 -> window +0x270/+0x274
//
// Closing 43.143's open question. The full chain, every link decompiled:
//
//   1. THE RECORD WIRE CARRIES THE CHILD LIST INLINE: after the +0x34 dword,
//      { u32 child_count; u32 child_ids[count]; } then the third string. The
//      reader fills record +0x48 (count) / +0x4C (ids array) from HERE and
//      ONLY here -- the definition's trailing ids[] do NOT feed it (measured:
//      a def holding child ids left the record's +0x48 at 0).
//   2. mgr+0x94 = the current SELECTED estate id. Three computed ids follow:
//      +0x98 yard (0x005B00E0), +0x9C house (0x005AFFC0), +0xA0 crate
//      (0x005B0140). The house computer: look up record(mgr+0x94); if its def
//      type IS house -> itself; else if PLOT type (0x007F6030) -> walk
//      record+0x48/+0x4C, look up each child record, first one whose
//      DefinitionTypeOf passes the house predicate (0x007F60F0, types 2/3)
//      wins; else -1. Crate identical with the crate predicate (0x007F61B0,
//      type 7). Resetters: 0x005B0A10 / 0x005B21E0 (all three at once).
//   3. RealEstateItemsWnd copies mgr+0x98/+0x9C/+0xA0 into ITS OWN
//      +0x26C/+0x270/+0x274 in 0x0073B7E0 (the ONLY setter besides the ctor;
//      callers 0x0073BEA0 thunk + WndNotification 0x0073CC50). -1 = section
//      renders N/A. Per-class item counters live behind them: +0x27C/280/284
//      yard, +0x288/28C house, +0x290/294 crate (see remover 0x00730880,
//      which classifies by def type of the row's estate).
//
// LIVE CROSS-CHECK (captures of placing/removing a house and a storage crate,
// live feed op 0x1900): placing a house/crate makes the
// server mint a NEW CHILD ESTATE with its own definition (house def 42, crate
// def 183 -- sequential estate ids 0x07EE04A4/A5) whose address is the plot
// address + ", <container name>" ("..., Evantil's Abode" / "..., Estate
// Storage Crate"), delivered as record + summary + item-detail segments. So
// the server-side model for containers is: one estate row per placed
// house/crate, linked via the parent's inline child list.
// ---------------------------------------------------------------------------

// 43.144a  WIRE-ORDER CORRECTION for the record's child list: RealEstate::
// Deserialize (0x7FDF00) reads { u32 child_count; STRING (+0x38); u32
// child_ids[count] } -- the +0x38 string sits BETWEEN the count and the ids,
// not after them. Sending count/ids/string instead makes the client consume the
// child id as the string length, and the ids loop then reads the empty string's
// length dword: record shows childcnt=1, ids=[0] (measured live). Full tail
// order after +0x34: count, string(+0x38), ids -> +0x48/+0x4C, escrow byte-map,
// +0x3C upkeep set, +0x40, +0x44 byte. Field order in this protocol is never
// guessable -- read the reader.

// 43.145  ESTATE TYPE VALUES + the items-table real-estate fields
// DefinitionTypeOf values (predicates decompiled from bucket.txt targets):
//   house predicate 0x7F60F0: TRUE for types 2 and 3
//   crate predicate 0x7F61B0: TRUE for type 7 only
//   plot / guild plot observed: 4 / 8; 10 = default/unknown
//   TRAP: the manager's background container walk (the 1s tick and the
//   mgr+0x9C/+0xA0 computers' mask) uses typemask 0x88 = types 3|7 ONLY.
//   A house def with type 2 therefore passes the section predicate but is
//   NEVER picked up by the walk -- in game the placed house classified as a
//   PET. House definitions must be TYPE 3.
// SERVER-SIDE ITEM FIELDS: the peq items table carries the RoF2 real-estate
// item block as UNK columns -- UNK227 = real-estate definition id (live
// parity exact: Estate Storage Crate = 183 = the def live minted in the
// capture), UNK223 = 16 crate class, UNK236 = 1 house flag (164 items),
// UNK228/UNK229 = placement scale range as float bits (0.1..2.5 / 0.9..1.1),
// matching the RE_Layout scale bounds.

// ---------------------------------------------------------------------------
// 43.146  THE ITEMS-WINDOW "P" IS THE PET CLASSIFICATION -- and it reads a byte
//         a misaligned item serialization fills with GARBAGE
//
// Pet predicate 0x007B1640 (vtable slot 2 of the row's item -> ItemBase-like
// object; live-verified via HW breakpoint at 0x...1647, EAX = the object,
// item id at +0xEC):   is_pet = (byte(+0x5CC-region cx) != 0 && byte(+0x608) == 0)
// Sibling 0x007B0B10 reads +0x5AC (placed/live flag). The P letter, the pets
// counter (wnd+0x284 family) and the PLACEMENT PET-SLOT GATE all key off this
// classification. With pet slots full (or capacity 0) the client refuses to
// commit placement of a P-classified item: the ghost hangs the full 5-second
// pending timeout -- looks exactly like a broken Place button.
//
// MEASURED live: house item (61894) +0x608 = 0 -> P; crate (62595) = 0xFF -> no
// P -- with the two items' DB rows byte-identical except id/name/lore/idfile/
// icon/charmfile (full-row copy test). And the P set FLAPS across re-places and
// def changes while item rows are provably unchanged. Conclusion: +0x608 is
// deserialized from the item stream (reader 0x007F6BF0: 32-char string region
// at +0x5CC, then ONE BYTE -> +0x608, then +0x5EC dword, +0x5F0/1 bytes,
// +0x5F4..+0x604 dwords, +0x609 count byte + loop) and EQEmu's RoF2 SerializeItem
// tail is MISALIGNED with what the client parses -- the byte lands on shifting
// stream content. EQEmu's ItemQuaternaryBodyStruct tail ("evolve_string" is a
// guessed 1-byte placeholder; unknown23-27 never written) is the suspect region.
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// ===========================================================================
// 43.147  THE COMPLETE ITEM-STREAM FIELD MAP -- and the REAL cause of "P"
//        , closes 43.146; every function decompiled headless)
// ===========================================================================
// Readers (all fail-open `if (cursor+N <= len)`):
//   0x005553B0  outer item reader (vtable[4] of the 0x150 item instance object)
//   0x007F6BF0  item-DEFINITION body reader (fills the shared def object)
//   sub-readers: 0x00505A50 serial STRING (cap 16+nul, self-delimiting);
//     0x004D60E0 slot group = u8 + 3x u16 (7 bytes); 0x004D5750 = 5x u32
//     (Material group); 0x007F6A20 = 6x {u32,u8,u8} (aug slots);
//     0x007F68D0 = effect group, called 6x: u32 + u8 + u8 + 6x u32 (30 fixed;
//     NOTE reader order differs from EQEmu's click/proc structs internally but
//     TOTALS match, so alignment survives) + STRING(cap 63) + u32;
//     0x004048D0(stream, dest, cap) = length-capped null-terminated STRING
//     read that always consumes strlen+1.
//
// EVERY region of EQEmu rof2.cpp SerializeItem aligns byte-exactly with the
// client -- header 77, ornament 2, hdrf 25+1 (ItemClass is the body reader's
// first byte -> def+0x11A), name/lore/idfile/idfile2 strings, ItemBody 255,
// charm string, secondary 74, filename string, tertiary 76 (the u16
// expendablearrow's high byte falls into the client's trailing 20-byte
// filler loop -> def+0x4DC), 6 effect groups, quaternary through SubType --
// EXCEPT THE QUATERNARY TAIL. After SubType the client truly reads:
//   u8 -> +0x5A8; u32 -> +0x598; u8 HEIRLOOM -> +0x10F; u32 PLACEABLE ->
//   +0x5AC; u8 -> +0x5B0; 6x u32 -> +0x5B4..+0x5C8;
//   STRING (0x004048D0 cap 0x20) -> +0x5CC   *** THE PET ACTOR STRING ***
//   u8 -> +0x608                             *** THE PET FLAG ***
//   u32 -> +0x5EC; u8 +0x5F0; u8 +0x5F1; 5x u32 -> +0x5F4..+0x604;
//   u8 count -> +0x609 + count x u32 -> +0x60C[]; 4x u8 -> +0x110..+0x113;
//   u32 -> +0x114; u8 -> +0x118. Then back in 0x005553B0: u32 N + N x u32
//   (a dword list into +0x8C), u8 -> instance+0x74, u32 M + M x { u32 bag
//   index; recursive item } (bag contents).
//
// Stock EQEmu GUESSED this tail (evolve_string/unknown23..39) and never wrote
// the pet string. The client ate `unknown28 = FF FF FF FF` as the string:
//   - normal packets (unknown29=0): string ate FF FF FF + nul(un29.b0),
//     +0x608 <- un29.b1 = 0 -> EVERY item def = PET. Invisible outside the
//     housing window, and by pure accident the remaining tail realigned
//     EXACTLY (un39=1 landed on instance+0x74, subitem count landed on M) --
//     which is why inventory/bags always worked.
//   - housing feed (with unknown29=0xFF): string ate one more byte,
//     +0x608 <- un30.b0 = 0xFF -> not a pet, tail shifted 1 byte (benign for
//     single-item packets; N read un39 -> the ItemPacketInvalid un39=0 probe).
//
// WHY "P" FLAPPED: 0x005553B0 dedups DEFINITIONS by item id (0x00556570 map
// find / 0x00556610) -- first packet to show the client an item id mints the
// session's def. Items sitting in bags -> def minted by the INVENTORY stream
// (pet-flagged); items only ever seen via the housing feed (the crate) ->
// def minted non-pet. Relogs re-rolled which source won. The crate's
// charmfile was a red herring (charm string self-aligns).
// charmfile was a red herring (charm string self-aligns).
// ---------------------------------------------------------------------------

// ===========================================================================
// 43.148  STORAGE-BUTTON ENABLE PREDICATES (Shed/Closet/Crate greying)
//         -- mapped)
// ===========================================================================
// The Items window Update (0x0073B7E0 region) enables/disables the destination
// buttons every frame through manager predicates -- greying is DATA-DRIVEN:
//   inventory item -> dest:  0x005AEE70(mgr, cur_estate=mgr+0x94, DEST, &item, 0)
//   window row     -> dest:  0x005AF1E0(record=row+8, DEST, &item, &x, row+0x11)
//   row -> inventory:        0x005B0C60
//   result -> enabled bool:  0x0072FC60 -- a whitelist switch; notable DISABLED
//     codes: 1, 0xC, 0x22-0x26, 0x28-0x2D, 0x30-0x3F, 0x42/43/45, 0x50, 0x53+.
// DEST codes (0x007F5F40/50/60): 3 = SHED (default), 2 = CLOSET, 6 = CRATE --
// see the corrected 43.51 table.
//
// The real verdicts are manager virtuals (vtable 0x009D910C): +0xC=0x005B1DA0
// -> inner 0x005B69B0; +0x10=0x005B1E40 -> 0x005B6EB0; +0x1C=0x005B5FF0 (row).
// Inside, the interesting refusals map to 0x005AE1F0's eqstr table (10 = full,
// 0xC = vault full -> GREYS, 0x26 = pet cap -> GREYS, 0x24/0x11 access, etc.).
//
// THE CAPACITY READERS (all __fastcall, ctx = a RECORD-shaped object):
//   0x005ABBA0: def_total_cap(0x007F5040(rec+0x20)) <= ctx+0x20  -> "full" (10)
//   0x005ABC00: def_vault_cap(0x007F5080(rec+0x20)) <= ctx+0x0C  -> 0xC, paired
//               with !IsPlaceable (0x007B0B10 reads item def +0x5AC)
//   0x005ABC60: def_pet_cap  (0x007F50C0(rec+0x20)) vs ctx+0x10  -> 0x26, paired
//               with IsPet (0x007B1640)
// ctx +0x0C/+0x10/+0x20 are the RECORD's dwords -- i.e. the GROUP-3 counters of
// the record wire (43.88's seven unattributed dwords at rec+0x08..+0x20):
// +0x0C = vault used, +0x10 = pets used, +0x20 = total used (by these readers'
// pairing).
// 0x007B0C10 (the 0x46 path) = item def +0x5AC bits 1|7 with +0x5EC != -1.
//
// ***: greying needs NO server data change. ***
// The used-counters are ACCUMULATED CLIENT-SIDE, not wire-fed: the 0x087F row
// append 0x005AB2C0 increments summary +0x0C (vault: item !placeable via
// 0x007FCAD0), +0x10 (pets: 0x007FCB00), +0x14 (0x007FCB40) per fed row -- which
// per fed row. The button predicate (0x005B69B0) then greys via def caps
// from the 0x2698 catalog:
//   def+0x28 total (getter 0x007F5040), def+0x2C vault (0x007F5080),
//   def+0x48 pet (0x007F50C0).
// The non-placeable -> shed/crate grey is code 0xC: item !placeable
// (0x007B0B10 = item def +0x5AC != 0) AND def_vault_cap <= vault_used
// (0x005ABC00).
//
// *** (live BP on the row predicate 0x005AF1E0 + the
// enable mapper 0x0072FC60): ***
// For a stored non-placeable item the predicate returns, per destination:
// Closet 0x01 (-> 0072FC60 = 0 = greyed, correct: already there), Shed 0x1F and
// Crate 0x03 (-> both map to 1 = ENABLED). 0072FC60 polarity confirmed: 1=lit
// (Inventory returns 0x00 = lit), 0=grey. So shed/crate stay lit because the
// predicate BAILS EARLY with a "can't resolve this estate / n/a" code (0x03 =
// no crate estate exists; 0x1F = record helper 0x005AB270 miss) that happens to
// map to enabled -- it never reaches the placeable/vault check. The item's
// +0x5AC is already 0 for the item and changes nothing. The real lever is why
// 0x005B5FF0/0x005AF1E0 can't resolve the dest estate.
// ---------------------------------------------------------------------------

// ===========================================================================
// 43.149  "STORE ALL" button = 0x6917 (C->S)
//        ; wire id CE-CONFIRMED on a live send breakpoint)
// ===========================================================================
// *** CORRECTION: STORE ALL IS 0x6917, NOT 0x7AD1. *** The decompiled click
// switch (case 5 -> 0x7AD1, 150 bytes) was the WRONG button; a live CE
// breakpoint on the packet builder 0x007D03C0 (runtime base-0x20000 = 0x7B03C0)
// caught the actual Store All click sending wire 0x6917 with an EMPTY body
// (distinct call signature: EAX=0x64, its own ESP frame, amid the 0x46C6/0x7DFC
// position heartbeats); the empty body means the server derives the
// location from the player. The button arms the usual request-pending wait, so
// it must be answered on 0x1228 (else the ~5s freeze).
//
// 0x7AD1 is a SEPARATE, still-unwired button -- almost certainly PACK MOVING
// CRATE (150-byte body, matches 43.61's 0x7AD1 finding); 0x58C7 is another
// (10-byte). Neither is Store All.
// Per-button opcodes from the switch (button-case IDs were unreliable, treat as
// hints): 0x218C travel, 0x626E place, 0x3424 item-move, 0x2036 to-inventory.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 43.150  HOUSE INTERIOR ZONE IDS -- recovered from the client's own zone
//         registration function, CE on the running client)
// ---------------------------------------------------------------------------
// House entry needs a real zone to send the player to, and peq has NOTHING in
// the 713-723 range -- the interiors were never imported. The client knows them
// all: eqgame.exe registers every zone in one long function, one call per zone:
//
//     push 0 / push 0 / push 0 / push 0x00800000 / push 0x000004C0
//     push <long_name_ptr>          ; e.g. 0x009EB1DC "House Interior"
//     push <short_name_ptr>         ; e.g. 0x009EB1CC "phinterior3a1"
//     push <ZONE ID>                ; e.g. 0x2CA = 714
//     push <flags>                  ; 0x00, 0x11, 0x12 seen
//     mov ecx,esi / call 0x007BC430 ; the register-zone method
//
// Anchor that proves the read: the neighborhood entry at 0x007C330C pushes
// 0x009EB210 ("neighborhood") + id 0x2C8 = 712, which matches peq exactly, and
// argath (0x2D4 = 724) matches peq's 724 too. The interiors sit in the gap.
//
// NOTE the string pool at 0x009EAF00+ is PACKED and variable-length, and long
// names are SHARED by several zones (all the plain house interiors point at one
// "House Interior" string). Do not assume a fixed 0x20 stride -- read the ids
// out of the code, which is what this table is.
//
// *** THE INTERIOR ZONES (client-authoritative) ***
//   714  phinterior3a1        "House Interior"
//   715  phinterior1a1        "House Interior"
//   716  phinterior3a2        "House Interior"
//   717  phinterior3a3        "House Interior"
//   718  phinterior1a2        "House Interior"
//   719  phinterior1a3        "House Interior"
//   720  phinterior1b1        "Dragon House Interior"
//   723  phinterior1d1        "Dragon House Interior"
//   738  guildhallsml         "Greater Guild Hall"
//   739  plhogrinteriors1a1   "One Bedroom House Interior"
//   740  plhogrinteriors1a2   "One Bedroom House Interior"
// 721 and 722 are NOT registered by the client (no phinterior1c1 exists in the
// string pool either) -- do not invent rows for them.
//
// The client's baked long name is NOT what live displays: a live capture of
// entering a house shows phinterior1d1 arriving as "Hermit's Hideaway
// Interior" in the real-estate feed while the exe's own string says "Dragon
// House Interior". Per 43.141 the exe string wins on client-side surfaces
// (loading screen, map title); the feed name is the house's display name.
//
// WHICH interior a house uses is DEFINITION DATA, not a constant: the live feed
// carries the interior zone short name inside the house's real-estate
// definition record. So a house item -> its definition -> its phinterior zone.
// Never hardcode one interior for all houses.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 43.151  HOUSE ENTRY, END TO END -- one opcode, second dword decides; the
//         interior zone id travels IN THE DATA, capture + in game)
// ---------------------------------------------------------------------------
// PROVEN IN GAME: native house entry works on the stock client. Click the door
// of a placed house -> C->S 0x218C {plot_id, house_id, 0} -- the SAME opcode as
// the neighborhood window's Go button, whose form is {id, -1, 0}. The second
// dword is the whole disambiguation; there is no separate house opcode, so the
// server branches on dword2 != -1.
//
// LIVE'S ANSWER (live house-entry capture): travel reply 0x9352 carries entry
// coords {x=-12, y=0, z=5, h=0} + the house address string; the interior's own
// record (0x1900, 793 B) carries, right after its three name strings:
//     u32 1, u32 ZONE_ID (0x2D3 = 723 for phinterior1d1), f32 1.0, u32 100 ...
// -- i.e. LIVE SENDS THE INTERIOR ZONE ID IN THE ESTATE'S DATA. That is the
// second, independent confirmation of the 43.150 id table (first was the exe's
// zone-registration function), and it settles the architecture question: the
// interior zone is per-house DATA, never a constant, and the SERVER resolves
// and moves the player -- the client only ever asks.
//
// THE EXIT DOOR IS A SERVER SPAWN, NOT ZONE GEOMETRY. Every interior .zon
// names exactly one door MODEL (present in the .eqg, rendered only if a
// `doors` row spawns it -- peq has none for these zones, hence "the exit door
// is missing"):
//     phinterior1a1/1a2/1a3/3a1/3a2/3a3   OBJ_door_plhA
//     phinterior1b1                       OBJ_pinterior1b_door
//     phinterior1d1                       OBJ_ph1d1_door
//     plhogrinteriors1a1/1a2              OBJ_doorA
// Zone family naming: trailing digit = interior material variant (a1/a2/a3 =
// the item names' Stucco/Wood/Stone), leading digit = floor-plan family (1a vs
// 3a); 1b1/1d1 are one-off "Dragon" plans. The repeated long names in 43.150
// are one SHARED string in the exe pool, not duplicate zones.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 43.151a  EXIT-DOOR CORRECTIONS -- measured in game + read off live's wire
//         ; supersedes two 43.151 details)
// ---------------------------------------------------------------------------
// 1. DOOR NAMES MUST GO OUT UPPERCASE. Seeding the .zon's exact mixed case
//    ("OBJ_ph1d1_door") produced an INVISIBLE door: the entity existed (the
//    dev-tools door search listed it) but no model rendered. UPPER()ing the
//    name made it render instantly. Live sends "OBJ_PH1D1_DOOR" uppercase
//    -- peq's all-caps door names are
//    load-bearing, not style: the client's actor lookup is effectively
//    case-sensitive against uppercased keys.
// 2. THE .ZON PLACEMENT IS NOT WHERE LIVE PUTS THE DOOR. For phinterior1d1 the
//    .zon instance says (-3.204, 4.728, -14.504) rot1 270deg (yaw lives in the
//    FIRST rot slot; the .zon instance record is [meshIdx][flags][pos3f]
//    [rot3f][scale] followed by a u32 vertex-color count + colors -- miss that
//    block and the second instance reads as garbage). Live's interior estate
//    record (0x1900) carries the door at (Y=-19.36, X=0, Z=0) heading 128
//    (90deg -- headings ride the wire as 0-512 floats), and that placement is
//    confirmed pixel-perfect in game. No axis mapping connects the two: the
//    exit door is placed from LIVE'S SERVER DATA like any placed object; the
//    .zon spot is the artist's design-time default. Consequence: the other
//    interiors' door transforms cannot be auto-derived from client files --
//    but families share geometry (1a1/1a2/1a3 one spot, 3a1/3a2/3a3 one spot,
//    plhogr pair near-identical), so it is ~4 one-time placements, and live's
//    pattern (X=0 centerline, flush with entry wall, 90deg) is the starting guess.
// ---------------------------------------------------------------------------

// 43.150a  CROSS-REFERENCE: live's ZoneNames.txt (id^display^x^y) covers
// 714-740 and REFINES the family split: 714/716/717 (3a*) = "Three Room House
// Interior", 715/718/719 (1a*) = "One Room House Interior", 723 = "Hermit's
// Hideaway Interior". CAVEATS: the file carries NO short names (the
// id->phinterior link only exists in the exe), and it lists 721/722 as real
// zones while the RoF2 exe registers neither -- a later-era source. Where they
// disagree, the exe wins for the RoF2 client.

// 43.150b  CORRECTION (ZoneNames.txt is FROM LIVE): 720/1b1 is a
// ONE ROOM House Interior on live -- the exe's "Dragon House Interior" label
// on it is stale shared cosmetics (43.141 class). Live lists SIX one-room
// interiors (715/718/719 = 1a1-3, 720/721/722), so 721/722 are almost
// certainly 1b2/1b3 -- one-room variants added AFTER RoF2, which is why live
// names them and the RoF2 exe registers nothing there. Corrected families:
// one-room = 1a1/1a2/1a3 + 1b1 (+ post-RoF2 1b2/1b3, unloadable on RoF2);
// three-room = 3a1/3a2/3a3; specials = 1d1 (Hermit's) + plhogr one-bedrooms.

// ---------------------------------------------------------------------------
// 43.151b  ALL EXIT DOORS SEATED FROM LIVE + collision groundwork
// ---------------------------------------------------------------------------
// House-tour captures settled the remaining door seats, and the placement is
// UNIFORM: live's interior record carries the door at X=0 centerline, Z=0,
// heading 128 (90deg), with Y = -19.36 (one-room family, capture phinterior1a1;
// also Hermit's 1d1 twice) or -19.256 (three-room family, capture
// phinterior3a2). Families share geometry -> siblings inherit; 1b1 (the
// one-room layout reskinned -- NOT dragon-themed inside despite the exe label,
// confirming 43.150b) and the plhogr pair are pattern-inferred pending a
// walk-in.
// "The Wayward Lady" is plhpirateshipint -- a post-RoF2 pirate-ship house
// interior; the RoF2 client ships only plhpirateshipext.eqg, so it cannot be
// loaded on RoF2.
// COLLISION (open): live has BOTH solid (house) and walk-through (plush doll)
// placed objects -- per-object data, zero wire traffic while colliding; the
// deciding field arrives with zone-in data. RoF2 client: placed actors type 9
// vs door actors type 3 at actor+0x78 (creation-time; runtime flip = crash),
// EQPlacedItemManager global rt 0x00DAFC70, getter rt 0x004E61E0 with 13 call
// sites -- triage rt 0x469CBC / 0x473A9E / 0x4FC8E2 for the movement sweep.
// Live volleys reference models NUMERICALLY (u32 20022 == IT20022): string
// greps cannot find house records; parse 0xa83d/0xed0c/0xd4b8/0x6bc6
// structurally.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 43.150c  THE FULL HOUSING-ZONE CATALOG vs THE RoF2 CLIENT; ids read
//          from the exe registration function, same method/anchors as 43.150)
// ---------------------------------------------------------------------------
//   493  weddingchapel      "Wedding Chapel"      in peq (min_status 255)
//   494  weddingchapeldark  "Wedding Chapel"      in peq (min_status 255)
//   495  dragoncrypt        "Lair of the Risen"   in peq (min_status 255)
//   737  guildhalllrg       Grand Guild Hall
//   738  guildhallsml       Greater Guild Hall
//   751  guildhall3         Palatial Guild Hall
//   766  phinteriortree     Evantil's Abode       -- the TREE HOUSE;
//        item 77407 exists in peq (UNK-unflagged, the Hermit's pattern), and
//        the zone ships a literal OBJ_evantil_plyrhouse_door model.
// IMPOSSIBLE on this client (post-RoF2: no geometry, not registered):
//   Bixie Hive house (CotF), The Wayward Lady = plhpirateshipint (TDS),
//   1b2/1b3 one-rooms (721/722). Verified: zero exe hits + no .eqg on disk.
// ---------------------------------------------------------------------------

// 43.150d  TEN MORE HOUSING INTERIORS
// A full-table dump (push-pattern parse over rt 0x7C0000-0x7D4000) surfaced
// TEN registered housing interiors with complete files in the RoF2 client,
// absent from peq:
//   741-744 plhogrinteriors3a1/3a2/3b1/3b2   live: "Three Bedroom House Interior"
//   745-747 plhdkeinteriors1a1-1a3           live: "One Bedroom House Interior"
//   748-750 plhdkeinteriors3a1-3a3           live: "Three Bedroom House Interior"
// (ogr = ogre style, dke = dark-elf style; matching plh*exteriors*.eqg house
// models ship too). The RoF2-loadable catalog is 20 interior zones, not 10.

// 43.150e  HOUSE-TOUR RESULTS
// Guild-hall naming, live-authoritative (exe labels stale again):
//   345 guildhall = the old "Guild Hall"; 737 guildhalllrg = PALATIAL;
//   738 guildhallsml = GRAND; 751 guildhall3 = MODEST (live also has 885,
//   another "Grand", post-RoF2). Palatial capture confirms 737 in game and
//   carries guildhalllrg's FULL interior door set (20+ OBJ_INTDOOR transforms,
//   (Y,X,Z,H) readable).
// Evantil's tree exit door MEASURED: OBJ_EVANTIL_PLYRHOUSE_DOOR at
//   (Y=-5.422, X=-0.003, Z=-0.173) h=128 -- the uniform -19.3 pattern does NOT
//   hold for special layouts; measure, don't extrapolate, for non-standard
//   interiors. Bixie Hive interior = plhbixieint (+ plhbixieintb), post-RoF2,
//   not loadable on RoF2. Grand (738) + Modest (751) interior doors were not
//   captured.

// ---------------------------------------------------------------------------
// 43.152  PLACED-OBJECT SOLID TYPE: 9 vs 31, chosen at actor creation
//        ; see 43.154 for the gate that actually matters)
// ---------------------------------------------------------------------------
// OBSERVED: placed objects walk-through from OUTSIDE, solid from INSIDE.
// On live: house solid, plush doll walk-through (per-object behavior).
//
// MECHANISM. EQPlacedItem's actor creation (the real call site is rt
// 0x004E4DCB..4E38, static 0x00504DCB -- the 43.91/43.109-era notes recorded
// these addresses MIS-BASED, re-found by byte signature) calls the actor
// factory (rt 0x0046B710, static 0x0048B710; consumes arg9 via
// movzx edi,[esp+98], appends _ACTORDEF to short tags) and then virtual
// actor->SetType (vtable+0x2C) with a HARDCODED type:
//     cmp byte [esi+2C],0 ; NpcItem from the wire   -> push 1 (NPC item)
//     else                                          -> push 9 (placed item)
// Engine solid-type semantics (matches Shendare's classic doors/scenery
// research: opentype 9/31 on scenery doors): 9 = MOSTLY WALK-THROUGH (a few
// surfaces stay solid -- the inside-solid house), 31 = FULLY SOLID unless the
// MODEL is designed walk-through (chairs, plush dolls -- exactly live's doll).
// Other types seen: 3 = doors/switches, 5 = the placement-session ghost
// (site rt 0x004E3FA1 pushes 5 after the same factory).
//
// The 13 EQPlacedItemManager-getter call sites include NO movement path.
// ---------------------------------------------------------------------------

// 43.152c  LIVE'S MODEL FOR PLACED OBJECTS, READ OFF THE WIRE.
//  * A guild-neighborhood zone-in S->C 0xcb57 DOOR LIST (32,912 B): besides the
//    zone's own doors (OBJ_GUILDGATE, OBJ_TELEPAD*, OBP_LOTSQUARE x76) it carries
//    ~166 PLACED-OBJECT models as door entities: IT20028 (Storage Crate line) x2,
//    IT12043 (Steadfast Cherry Tree) x8, IT22980/IT23002 (tables/benches),
//    IT23353 (stained glass), etc.
//  * Placing a storage crate emits S->C 0xea09 (759 B, "IT20028_ACTORDEF") + its
//    estate record -- the SAME object type is a placed item at placement AND a
//    door in the next zone-in's list. No 0xcb57 door add at placement time --
//    live's door twin only materializes on re-zone.
//  * The yard right-click is a placed-object record round-trip on 0x5EDB (key
//    echoed back) -- interaction rides the placed item even in a zone full of
//    door twins.
// Also: RoF2 ships COL_<model> collision actordefs for 15 models (e.g.
// COL_IT23651, Evantil's Abode; see OnDemandResources.txt) -- special-case
// collision hulls, not the general mechanism. "GIT#####_ACTORDEF" strings in
// live captures are a parsing artifact: a 0x47 marker byte in front of plain
// IT tags, not a ghost-model family.
//
// PLACE-CONFIRM TRANSFORM (decoded from ROTATED place-confirms,;
// corrects the 43.109 "which angle is which" unknown):
//   confirm transform[0]  ("heading" col)  0 in every commit logged
//   confirm transform[1]  ("pitch" col)    THE YAW, EQ 0-512 circle
//                                          (samples: 9.9 / 46.4 / 483 = -29)
//   confirm transform[2]  ("roll" col)     tilt (small deviations so far)
// Door headings use the SAME 0-512 circle (PoK doors at 256/384 = 180/270
// deg). A door can only yaw.

// ============================================================================
// 43.153  SOLID-TYPE SELECTION AT ACTOR CREATION
//         (its original conclusion is superseded by 43.154)
// ----------------------------------------------------------------------------
// The type is chosen at actor creation, static 0x00504E26:
//
//     cmp byte [esi+0x2C], 0    ; the NpcItem WIRE BYTE
//     je   push9
//     cmp dword [esi+0x30], 0   ; set only inside the NpcItem branch
//     je   push9
//     push 1                    ; reachable
//     jmp  ...
//   push9:
//     push 9                    ; reachable
//     call [actor_vtbl+0x2C]    ; SetType -> actor+0x78
//
// Only 1 and 9 are reachable, selected by is_npc; a full .text scan finds no
// `push 31` anywhere in the client, so the solid type never comes from data.
// The 3-axis orientation call has exactly four call sites in the whole client,
// two in the placed-item path and two in the mob path, NONE in the door path --
// a door receives heading + incline and nothing else.
// ============================================================================

// ============================================================================
// 43.154  *** PLACED OBJECTS DO COLLIDE, FROM A PACKET.
//         THE GATE IS THE WIRE BYTE AT +0x98. ***, proven in game)
// ----------------------------------------------------------------------------
// The solid-type immediate (9 vs 31, 43.153) is only ONE of two gates, and the
// OTHER one is a wire field.
//
// Static 0x00504E4A, immediately after the actor is created:
//
//     cmp byte [esi+0x98], 0     ; the WIRE BYTE from the place packet (43.60)
//     je   skip                  ; 0 -> skip the whole block
//     mov  ecx,[esi+0x38]        ; the actor
//     eax = actor->[vtbl+0x54]() ; get flags
//     or   eax, 5                ; *** THESE TWO BITS TURN COLLISION OFF ***
//     actor->[vtbl+0x50](eax)    ; set flags
//   skip:
//
// Send 0 in that byte and the client takes the je, the flags are never OR'd,
// and the placed object's faces COLLIDE -- solid enough to stand on, on a
// completely stock client.
//
// Proven in game by sending 0 in the wire byte on a stock client.
//
// The client only sets it itself for NpcItem (0x004B3940 is just
// `return this->[0x2C]`) -- which is precisely why pets are walk-through. A
// server should send +0x98 = is_npc ? 1 : 0.
//
// NOT PARTIAL: with only the wire byte at 0, the WHOLE model collides, every
// material included (verified in game). The type-9-vs-31 immediate is real but
// is not a required gate.
// ============================================================================
//
// 43.156  PLACEMENT MODES + THE REAL-ESTATE RESULT-CODE TABLE
//         Modes measured in game; codes read straight out of the dispatcher.
//
// *** PER-ITEM PLACEMENT-MODE CAPABILITIES: +0x99 / +0x9A / +0x9C. ***
// Predicate 0x005AE160 is `bool CanUseMode(int mode, EQPlacedItem*)`, stdcall, ret 8:
//     mode 0 COLLISION  allowed unless (+0x99 != 0 AND +0x9C == 1)
//     mode 1 WALL       allowed IFF +0x9C == 1
//     mode 2 CURSOR     identical test to collision
//     mode 3 FREE       allowed IFF +0x9A == 0 AND +0x2C == 0 (not an NPC item)
//     anything else     always false
// Mode NUMBERS are pinned by the client's own mode-name switch at 0x00556E80:
//     0 -> eqstr 5266 "Collision Mode"   1 -> 5286 "Wall Mode"
//     2 -> eqstr 5267 "Cursor Mode"      3 -> 5268 "Free Placement Mode"
// Players switch with the keybinds registered at 0x00708090 (5258 "Toggle Mode",
// 5259/5287/5260/5261 "Set Mode <X>"). A denied mode raises eqstr 5270,
// "The item you are placing does not allow %1".
//
// Because modes 0 and 2 share one test, +0x99 cannot separate them -- it can only
// kill BOTH, and only when +0x9C == 1. So +0x99 means WALL-ONLY. The usable set:
//     furniture     99=0 9A=1 9C=0   collision + cursor
//     picture frame 99=1 9A=1 9C=1   wall only
//     either        99=0 9A=1 9C=1   collision + cursor + wall
//     all zero      99=0 9A=0 9C=0   collision + cursor + FREE  <- lets a player
//                                    float an object in mid-air or sink it through
//                                    the floor. Zero is maximally permissive.
//: sending 9A=1 does deny Free mode in game (client refuses the
// mode switch), confirming the predicate. These are PER-ITEM on live and must come
// from the item definition -- a rug should lie flat, a table should not hang.
//
// *** THE RESULT-CODE TABLE. THE CLIENT RAISES THESE ITSELF -- NOT ONLY THE SERVER. ***
// RealEstateManagerClient::OnPlacementResult (0x005AE1F0) dispatches a code to an
// eqstr; -1 and 0 are the success/no-op paths, every other value is a refusal.
//
// An earlier draft of this note called the codes SERVER-SUPPLIED and concluded that
// placement validation is server-side. THAT WAS WRONG, and it was inferred from the
// function's shape without a test. Measured: with the server
// sending no result codes whatsoever, the placement ghost goes RED with "the surface
// is not flat enough" while underground, GREEN once above ground, and RED over a
// player -- all in real time, continuously, during the drag. The client evaluates
// the position itself and feeds the same code enum into the same display path.
//
// *** CONSEQUENCE: THE VALIDATION ALREADY WORKS. FREE MODE IS THE BYPASS. ***
// Collision and Cursor mode validate. Mode 3 FREE does not -- it is unconstrained by
// design, which is what "free placement" means. With +0x9A = 0 every item can enter
// Free mode, so nothing is validated -- which is how an object gets floated in
// mid-air or sunk through a floor. The control is the per-item capability byte.
// For "the object went through the floor", the code is 0x1D.
//
//   0x09   5226  The item was not found at the specified location.  It is likely that you
//   0x0A   5219  You cannot add this item because the real estate is full.
//   0x0B   5241  There are no items to transfer.
//   0x0C   5272  You cannot add this item.  The vault is full.
//   0x0F   5242  You cannot add another house to this yard.
//   0x10   5243  You cannot place another moving crate on this property.
//   0x11   5220  You can't put that item in this real estate.
//   0x13   5229  You can only do this with empty containers.
//   0x14   5231  You are too far away from the item.  Move closer to it and try again.
//   0x15   5275  This item must be placed in the zone for you to interact with it.
//   0x16   5240  You must have a moving crate placed in your yard or house for this opera
//   0x17   5251  This can only be done by the owner of the property.
//   0x18   6441  You cannot do this because the upkeep on this plot has expired.
//   0x19   6442  You cannot do this because the upkeep on the item or house has expired.
//   0x1C   5237  You cannot place this on top of a player.
//   0x1D   5273  You cannot place this item here because the surface is not flat enough.
//   0x1F   6436  You cannot place this here because you've reached the limit for that ite
//   0x20   6437  Non-empty houses and crates can only be placed in yards and houses.  The
//   0x21   6438  You cannot pick this up because you may only have one non-empty house an
//   0x23   5250  You cannot place that item in this zone.
//   0x24   5230  You can only do this with placeable items.  See the item's description.
//   0x26   6440  You cannot place this here because you have reached the pet limit for th
//   0x27   5282  Moving crates can only contain one non-empty house and this moving crate
//   0x2A   5283  You cannot interact with this NPC item because its NPC was not found in 
//   0x2B   5291  You cannot add this item because it is No Storage.
//   0x2C   5292  You cannot add this item because it contains an augment that is No Stora
//   0x2D   5312  Part of the stack was successfully moved.
//   0x2E   5313  You cannot place an item from that stack because the real estate does no
//   0x2F   5319  This activity is disabled until you own the House of Thule expansion.
//   0x30   5331  Your house swap failed because the house you are attempting to swap in d
//   0x31   5332  Your house swap failed because the house you are attempting to swap in d
//   0x32   5333  Your house swap failed because the house you are attempting to swap in d
//   0x35   5334  Your house swap failed because you do not have a house placed.
//   0x37   5335  Your house swap failed because the house you are attempting to swap in i
//   0x3B   5338  Your house swap failed because you cannot swap houses while you have a p
//   0x47   5404  You cannot do that because you do not have add item access.
//   0x48   5405  You cannot do that because you do not have remove item access.
//   0x49   5406  You cannot do that because you do not have move item access.
//   0x4A   5407  You cannot do that because you do not have interact item access.
//   0x4B   5408  You cannot do that because you do not have add house access.
//   0x4C   5409  You cannot do that because you do not have remove house access.
//   0x4D   5414  You cannot do that because you do not have add crate access.
//   0x4E   5415  You cannot do that because you do not have remove crate access.
//   0x4F   5413  You cannot do that because you do not have mail item access.
//   0x51   5418  You cannot store this house in the desired moving crate because the hous
//   0x52   5429  This activity is disabled until you own the Veil of Alaris expansion.
//
// Dispatcher walks a cmp/jne chain from 0x005AE240; codes not listed fall through
// silently. Codes 0x47-0x4F are the per-permission access denials and line up with
// the 13 access bits in 43.128E.
// ============================================================================
//
// 43.157  THE `items` TABLE CARRIES LIVE'S REAL-ESTATE FIELDS AS "UNK" COLUMNS
//        , all measured against peq data + confirmed in game)
//
// These are not offsets -- they are DATABASE columns, recorded here because they
// feed the placed-object record documented in 43.156 and there is nowhere better.
// EQEmu imported live's item rows wholesale, unknown fields included, so the
// housing behaviour of an item has been sitting in the item table all along.
//
//   UNK225  WALK-THROUGH        -> record +0x98 = 1 (the same byte pets use)
//   UNK226  WALL-MOUNTABLE      -> record +0x9C = 1 (enables WALL placement mode)
//   UNK228  MINIMUM SCALE       -> record +0xA0, raw float BITS in an int32
//   UNK229  MAXIMUM SCALE       -> record +0xA4, raw float BITS in an int32
//   UNK223  model family / mount style (see below -- NOT decoded)
//
// *** UNK225 -- WALK-THROUGH. CONFIRMED IN GAME. ***
// Set on exactly THREE of the 793 placeable items, and they are precisely the three
// anchors: Primary 52584, Secondary 52585, Guild Hall 50874. Nothing else carries
// one -- so with anchors solid, a player teleported onto one gets stuck against
// it. Sending +0x98 = 1 for these avoids that; verified in game.
// HONEST LIMIT: with n = 3 and all three being anchors, this could equally mean "is
// an anchor" rather than "is non-solid". The data cannot separate those, and the
// effect is identical for every item that exists today.
//
// *** UNK226 -- WALL-MOUNTABLE. ***
// Set on 14 of 793 placeable items: every "Painting: <x>" piece plus a lobby door --
// exactly the things that belong on a wall. Drives +0x9C, without which the client's
// WALL mode is impossible for that item (43.156's CanUseMode, mode 1).
//
// *** UNK228 / UNK229 -- PER-ITEM SCALE LIMITS, AS FLOAT BITS. ***
// int32 columns holding the raw bit pattern of a float, NOT a scaled integer:
// 1036831949 = 0x3DCCCCCD = 0.1f, 1069547520 = 0x3FC00000 = 1.5f. Across the
// placeable set: 584 items read 1.0/1.0 -- resizing DISABLED -- 157 read 0.9/1.1,
// 25 read 0.1/2.5, and both original Paintings read 0.1/1.5, which is byte-for-byte
// really min/max/default scale). A handful of rows are 0/0 -- fall back rather than
// use a degenerate range.
//
// *** UNK223 -- NOT DECODED. DO NOT FABRICATE IT. ***
// Groups by MODEL family, not by item: 9 = IT11903-11906, 20 = IT12039,
// 29 = IT23328-23342, 44 = IT23561-23563, 0 = IT10666. Best guess is a mount
//
// SPELL EFFECTS 437/438 belong to the same system: base_value[0] is the ANCHOR'S
// ITEM ID (52584 / 52585 / 50874), not a coordinate, and the destination is wherever
// that item was PLACED. 18 spells share the three anchors. EQEmu has both effects
// commented out as "not implemented".
// ============================================================================
//
// 43.155  OPEN-WORLD OBJECTS COLLIDE WITHOUT A DOOR -- object_type 255
//        , stock EQEmu + stock client, measured in game)
//
// SEPARATE SYSTEM FROM HOUSING -- do not confuse the two. Housing placement is
// the RealEstate packet (RoF2 0x1045 / live 0xea09) and its collision gate is
// the +0x98 byte above. Open-world scenery is OP_GroundSpawn carrying
// Object_Struct, a different 96-byte struct handled by different client code.
// There is no +0x98 in it and no value in it can reach 0x00504E4A.
//
// THE STANDARD EQEMU WORKAROUND, for reference: for `object` rows of type 0
// (ObjectTypes::StaticLocked) the server fabricates a DOOR on top of the object
// purely for collision -- ids in a 1,000,000,000+ out-of-range band, doorid -1,
// opentype taken from the row's solid_type (0->31, 1->9). Stock EQEmu, both at
// zone load (zone.cpp LoadZoneObjects) and in #object save (object_manipulation
// .cpp).
//
// MEASURED: an object spawned by `#object add` and NOT saved is SOLID with no
// door of any kind in the zone (verified: no rows in the fabricated band, the
// object absent from the `object` table because it was never committed). An
// unsaved object is object_type 255 = ObjectTypes::StaticUnlocked -- #object add
// silently converts a requested type 0 into 255, and only #object save turns it
// into a real StaticLocked 0. 255 is a persistable configuration, not merely an
// editing state: peq ships 378 such rows.
//
// Repro, crushbone, stock everything:
//     #object add 0 FORGE_ACTORDEF 0 100 0 0      (do NOT #object save)
// Size is a PERCENTAGE -- 100, not 1. Model must be an _ACTORDEF the zone's own
// _obj archive actually contains (crushbone has 44; IT-anything is not among
// them). A type outside the ObjectTypes enum, e.g. 2, spawns a row that renders
// nothing at all.
//
// NOT ESTABLISHED, and the door workaround hangs entirely on it: whether a
// type-0 object is walk-through. It cannot be tested by saving, because saving
// also fabricates the door and the result becomes unreadable -- the same masking
// that produced the wrong conclusion in 43.153. Settling it needs the door
// fabrication suppressed at its two call sites.
//
// ALSO UNVERIFIED: whether the client reads Object_Struct.solid_type at +0x10 at
// all. Its only demonstrated effect in EQEmu is choosing the fabricated door's
// opentype. The column was named `unknown10` until an EQEmu schema migration
// renamed it to solid_type -- the name is an inference, not a decode. If a 255
// object is solid regardless of it, the field is decoration.
// ============================================================================

// ============================================================================
// 43.158  *** 0x2B35 OPENS RealEstatePlotSearchWnd -- THE SERVER CAN SHOW IT ***
//        
// ============================================================================
// An earlier reading held that "kind 4/5 has no show flag, the server can only
// FILL the window". Half right: FillPlotSearchWnd (0x0074C4C0) indeed never
// shows anything. But showing is a SEPARATE OPCODE, found by walking every
// opener of the window instead of staring at the fill.
//
// THE THREE WAYS THE WINDOW OPENS (all references to pinst 0xD1FCC4 in .text):
//   1. /realestate plot -- cmd_realestate_handler 0x004E4E60. Subcommands, each
//      a strcmp then the SAME vtable+0xD8 toggle:  Show(!IsVisible, 1, 1):
//          "items"     0x9CB720 -> [0xD1FCB8] RealEstateItemsWnd
//          "manage"    0x9CB718 -> [0xD1FCBC] RealEstateManageWnd
//          "trophies"  0x9CB70C -> [0xF75AD8] (trophy case wnd, NOT in the
//                                   D1FCxx pinst block)
//          "plot"      0x9CB704 -> [0xD1FCC4] RealEstatePlotSearchWnd
//      No subcommand match -> eqstr 0x1D57 usage print.
//   2. __CreateAllRealEstateWindows / teardown (the 0x4991F8 cluster) -- ctor
//      and destroy, not interesting.
//   3. *** S->C WIRE OPCODE 0x2B35 ***, a case in CEverQuest::HandleWorldMessage
//      (0x004C3250). Dispatched from the THIRD compare chain (the [ebp-0x5360]
//      one, 43.111's lesson holding up again -- NOT the big 0x004CC1D7 table):
//          0x004C3E97:  cmp [ebp-0x5360], 0x2B35   ->  je case 0x004CA8BF
//      Case body 0x004CA8BF, in full:
//          if ([0xD1FCC4] == NULL) return;
//          Show(!IsVisible, 1, 1)              // vtable+0xD8 -- the same toggle
//          wnd->SetMode(body[0])               // 0x004B3C60
//      0x004B3C60 is two instructions: mov [ecx+0x24C], al; ret 4. So the whole
//      packet is ONE BYTE: a mode flag stored at wnd+0x24C.
//
// WHAT +0x24C MEANS (read in the window's row-select handling, live 0x0072C2B7
// = static 0x0074E2B7): on selecting a row, if +0x24C != 0 the window takes a
// one-shot action with the selected plot -- builds a little {u16,u32} record in
// a global (static 0xDD00E0) and hands it to RealEstateManagerClient::SendMsg
// (type 4, 6 bytes) -- then CLEARS +0x24C in every branch. If +0x24C == 0,
// row-select just selects (0x0057C2C0 path) and Go reads the selection from
// wnd+0x228 as 43.132 measured. So: 0 = browse mode, non-zero = select-and-send
// mode.
//
// THE COMPLETE FLOW:
//     -> S->C 0x2B35 {00}                        server OPENS the window
//     -> C->S 0x2691 search request              the SHOWN window asks itself
//     -> S->C 0x2691 kind-4 rows                 FillPlotSearchWnd (43.148)
//     -> select + Go                             travel via find (43.160)
// This also explains the live capture's shape: the client's request appears
// "first" only because the 1-byte open that precedes it is easy to miss.
#define OP_RealEstatePlotSearchOpen__WIRE                           0x2B35
#define HandleWorldMessage__case2B35_x                              0x004CA8BF
#define RealEstatePlotSearchWnd__SetMode_x                          0x004B3C60  // wnd+0x24C = arg byte
#define RealEstatePlotSearchWnd__ModeByte_OFFSET                    0x24C
#define cmd_realestate_subcmd_strings_x                             0x9CB704    // "plot","trophies","manage","items" ascending
// ============================================================================

// ============================================================================
// 43.159  *** THE WINDOW GATES ITSELF: "You must be in a neighborhood" ***
// ============================================================================
// Opening the window outside a neighborhood, the client answers with red chat:
// eqstr 8078 "You must be in a neighborhood to use that." That string appears at
// exactly ONE site in .text (static 0x0074DB34), inside a virtual the window
// overrides at *** vtable+0xDC -- the slot right after Show(+0xD8) ***, i.e.
// its on-show/activate hook. Read straight from the live object:
//     PlotSearchWnd vt (static 0x9F8EA8): +0xD8 = 0x845290 (base CXWnd::Show),
//                                         +0xDC = 0x74DB00 (the GATE)
// The gate, in full (static 0x74DB00):
//     mgr = RealEstateManagerClient::Instance()          // 0x5B3540
//     if (TypeGroupOf(mgr+0x90) == 4)  -> 0x74DA70       // proceed: the window
//                                        reads its combo (+0x224) and asks the
//                                        server for its list itself
//     else -> eqstr 0x1F8E print, abort show
// TypeGroupOf = 0x7F5F70, a jump table over definition TYPE 1..11:
//     type 1->1  2,3->2  4,8->3  *** 5->4 ***  6->5  7->6  9->0  10->8  11->9
// So the ONLY type whose group is 4 is *** TYPE 5 = NEIGHBORHOOD ***. And
// mgr+0x90 is set by exactly one thing: the SetCurrent packet (0x6FD0),
// as DefinitionTypeOf(record+0x20) through the 0x2698 catalog (43.29).
//
// Types pinned by this table: 3 house, 4 lot square, 5 neighborhood,
// 7 crate, 8 guild plot (43.165).
//
// Standing between plots in a neighborhood IS an estate on live -- the
// neighborhood itself.
#define RealEstatePlotSearchWnd__vftable_x                          0x9F8EA8
#define RealEstatePlotSearchWnd__ShowGate_x                         0x0074DB00  // vt+0xDC, on-show
#define RealEstatePlotSearchWnd__ShowGateProceed_x                  0x0074DA70  // sends the search
#define RealEstate__TypeGroupOf_x                                   0x007F5F70  // def type -> group
#define eqstr_MustBeInNeighborhood                                  0x1F8E      // 8078
// ============================================================================

// ============================================================================
// 43.160  *** THE PLOT WINDOW TRAVELS THROUGH /find -- 0x5CEA TYPE 4 ***
//        . "Mysterious forces prevent you from finding your
//          destination easily." = eqstr 6779, THE FIND-SYSTEM FAILURE LINE.)
// ============================================================================
// Go on RealEstatePlotSearchWnd does NOT send 0x218C (that was the NEIGHBORHOOD
// window's Go, 43.132). It arms the FIND MANAGER (instance [0xE641F8]) and the
// manager sends OP_FindPersonRequest -- RoF2's find request is TYPED:
//
//   SetDestination overloads (manager methods; each stores then submits):
//     0x0059C2C0(id, flag)     type 4  REAL ESTATE   id -> mgr+0x34
//     0x0059C310(vec3*, flag)  type 5  POSITION      xyz -> mgr+0x3C
//     0x0059C370(id, flag)     type 6  SWITCH        id -> mgr+0x1C
//   Submit 0x0059A420 builds 40 bytes at 0xDD00E2 and sends internal msg id
//   0x5CEA (= OP_FindPersonRequest's wire id):
//     +0x00 u32 find_type      +0x04 u32 (u16 switch id; -1 for type 4)
//     +0x08 u32 estate id      +0x0C u32 secondary (-1)
//     +0x10 f32 player x,y,z   +0x1C f32 dest x,y,z (zeros for type 4)
//   EQEmu's rof2 structs::FindPersonRequest_Struct already HAS this exact
//   40-byte shape -- unknown00 IS the find type, unknown08 IS the estate id.
//   The stock decode kept only "npc_id" (the -1 at +0x04), GetMob(junk) found
//   nothing, no reply went out, and the manager failed into eqstr 6779
//   (single .text site per print: 0x0059C8D3 / 0x0059CEC4, failure handler
//   0x0059C870 -- which on mode 1 also shows the find HUD at a fallback pos).
//
// TRANSPORT NOTE: the manager sends through [0xDD25AC] which RTTI names
// UdpLibrary::UdpConnection -- it IS the zone connection, and the "00 03 len"
// framing in the sender 0x008C49B0 is just OP_Combined. Nothing rides a side
// channel.
//
// STILL OPEN HERE: whether plain row-select also submits (0x0074E370's browse
// branch calls SetDestination(id, 1) -- if a click alone flings, the flag or
// the notify code distinguishes Go and needs a second look). And the row
// "icon": FillPlotSearchWnd performs NO animation lookup at all -- the left
// column art is client template art, not server data; compare against live
// before hunting "missing assets".
#define FindManager__Instance_ptr_x                                 0x00E641F8
#define FindManager__SetDestRealEstate_x                            0x0059C2C0  // type 4
#define FindManager__SetDestPosition_x                              0x0059C310  // type 5
#define FindManager__SetDestSwitch_x                                0x0059C370  // type 6
#define FindManager__Submit_x                                       0x0059A420  // sends 0x5CEA
#define FindManager__FailHandler_x                                  0x0059C870  // prints 6779
#define eqstr_MysteriousForces                                      0x1A7B      // 6779
// ============================================================================

// ============================================================================
// 43.161  *** KIND-4 ROW FIELDS: GO TRAVELS ON u32 #3, u32 #1 IS THE ART ***
//        , proven by a character id coming back on the wire)
// ============================================================================
// The kind-4/5 row block (43.148) was half right:
//     u16 count, string header(title), per row:
//         u32 #1  ROW ART slot        <- "(row icon)" in the old note. A plot
//                                        id here draws the PEARL fallback.
//                                        It is an item icon id (43.162).
//         u32 #2  owner_type          <- 4 Player / 8 Guild, else row skipped
//                                        (confirmed before, still true)
//         u32 #3  *** REAL ESTATE ID *** what the row SELECTS and Go TRAVELS
//                                        to (find type 4, 43.160)
//         string  OWNER               <- the only source of the Owner column;
//                                        no u32 is ever displayed for it
//         string  ADDRESS
// PROOF: with an owner's character id in #3, Go armed the find manager with
// that character id as the estate.
//
// Row-select does NOT submit the find -- the day's log shows typed-find
// requests only at Go presses (and estate 0 for Go with no selection; answer
// the empty reply so the manager resolves instead of failing twice).
// ============================================================================

// ============================================================================
// 43.162  *** THE ROW-ART u32 IS AN ITEM ICON ID -- A_DragItem, 500..8492 ***
//        . The pearls were never missing art.)
// ============================================================================
// FillPlotSearchWnd hands row u32 #1 to the ITEM-ICON CACHE at [0xF71398]
// (method 0x00665A60):
//     if (id < this[+0x18] || id > this[+0x1C]) id = this[+0x18];  // CLAMP
//     hash lookup on id; on miss:
//         template = CXWndManager([0x15D3D08])->GetAnim(this[+0x10])
//         anim = new(0x4C) CTextureAnimation (ctor 0x0085A9A0), copy template
//         anim->SetCurCell(id - this[+0x14])                       // 0x0085A860
// Read off the LIVE object: +0x10 = "A_DragItem", base +0x14 = 500,
// min +0x18 = 500, max +0x1C = 8492.
//
// So the field is a plain *** items.icon value *** into the ordinary
// dragitem##.dds sheets the client already ships. NOTHING under 500 is legal:
// it clamps to 500, and cell (500-500)=0 of the first sheet is the PEARL. The
// clamp goes to MIN (+0x18) both ways per the disasm (jl/jle both land on
// mov esi,[edi+0x18]): out-of-range or non-icon values = pearl.
//
// 3337 is the house icon every RoF2 housing item carries; any items.icon id in
// range works.
#define RealEstateIconCache_ptr_x                                   0x00F71398
#define RealEstateIconCache__GetIcon_x                              0x00665A60  // arg = items.icon id
// (SetCurCell = the long-known CTextureAnimation__SetCurCell_x 0x87A860 -- the live-rebased
// read here landed on exactly that entry, a clean cross-check of the whole decode.)
#define CTextureAnimation__ctor_x                                   0x0087A9A0
// ============================================================================

// ============================================================================
// 43.163  *** LIVE'S PLOT VOLUMES DECODED -- 86x86 ROTATED, HALLS 175.5x350 ***
//        , from a live neighborhood capture; live opcode 0x9A4E
//          = RoF2 0x49DF, 75 records, stride 88 -- the RoF2 area record
//          shape byte-for-byte)
// ============================================================================
// Every one of live's 75 area matrices for the neighborhood template:
//   * 71 standard lots: half-extents exactly 86 x 86 (NOT 85, NOT 105), Z half
//     350, CENTRED ON THE LOT-SQUARE MARKER, ROTATED -- and the rotation equals
//     the OBP_LOTSQUARE door heading (0-512 scale) to <1 degree on all 71,
//     modulo the square's 90-degree symmetry. Zero shear anywhere.
//   * 4 guild halls: 175.5 x 350, centres up to 65 units OFF their
//     OBP_GUILDSQUARE markers, rotations near axis (-3.6/0.7/87.9/-3.5 deg).
//   * wire row 3 (translation) is (worldY, worldX, Z) -- the usual pair order.
//     Matrix build: r0 = h1*(cos,sin,0,0), r1 = h2*(-sin,cos,0,0),
//     r2 = (0,0,350,0) -- reproduces live's bytes exactly (verified on ids
//     101/105/107).
//   * tail dword +0x48 is a per-record FLOAT on live (values ~0.02..6.3,
//     radians-like; sending 0 still works).
//
// An 86-half square rotated ~45 degrees reaches 121 units along a world axis,
// so any containment test must use the rotated box, not an axis-aligned one.
// ============================================================================


// ============================================================================
// 43.164  *** GUILD HALLS -- INDEX ***
// ============================================================================
// A guild hall is NOT a new subsystem. Live builds it out of the same pieces as
// houses -- a GUILD PLOT (an ordinary plot whose definition type is 8) with a
// HALL ITEM placed on it, which mints a CHILD ESTATE whose definition names an
// interior zone. Live's address strings prove the nesting:
//   "<neighborhood>, 200 Guild Way, Modest Guild Hall"
// i.e. <neighborhood>, <plot address>, <building>. Identical in shape to
// "<neighborhood>, <plot address>, Evantil's Abode" (a house) and
// "<neighborhood>, <plot address>, Estate Storage Crate" (a crate).
// Guild plots exist in BOTH public neighborhoods and guild-created ones -- no
// separate zone family.
//
// Sub-sections: 43.165 type-name table, 43.166 guild-rank permissions,
// 43.167 0x85c7/0x15b2 identified, 43.168 the hall/house DEFINITION record,
// 43.169 entry-transform field order correction, 43.170 the live door list.
// ============================================================================


// ============================================================================
// 43.165  *** THE COMPLETE REAL-ESTATE TYPE-NAME TABLE (static 0x00B616A0) ***
// ============================================================================
// A char* array indexed by the DefinitionTypeOf (0x007F4FA0) return value, read
// by 0x007F5021 (its only xref). Out-of-range prints "Bad Type" (0x00A100E4).
//
//   0  "None"            0x009CA544
//   1  "Regular Zone"    0x00A100A8
//   2  "Guild Hall"      0x00A0C310
//   3  "Player House"    0x00A10098
//   4  "Plot"            0x00A10090
//   5  "Neighborhood"    0x009D9168
//   6  "Town"            0x00A10088
//   7  "Moving Crate"    0x009F7D78
//   8  "Guild Plot"      0x00A1007C
//
// This CLOSES the naming of every type 43.145 had numbers for, and it settles
// two things that were previously guesses:
//   * f09 = 8 is a GUILD PLOT (the land), not "guild hall"; live's own
//     guild-plot definition id is 187 (43.168).
//   * type 2 is the name live gives a guild-hall BUILDING. But 43.145 measured
//     that the RoF2 manager's container walk uses typemask 0x88 (types 3|7
//     only), so a type-2 def renders but is never picked up and classifies as a
//     PET. House/hall definitions must be TYPE 3 on this client. (Live's 64-bit
//     client evidently widened that mask; RoF2 did not.)
// Also nearby, same style of table: "RealEstateDefs.txt" (0x00A100B8) and
// "RealEstateDefRel.txt" (0x00A100CC) -- the client-side definition files the
// live tools ship; not used on an EQEmu server, and not needed (definitions
// arrive on 0x2698).
// ============================================================================


// ============================================================================
// 43.166  *** BUYING/SELLING A GUILD PLOT IS A GUILD *RANK PERMISSION* ***
//         -- and EQEmu already implements the whole mechanism
// ============================================================================
// The RoF2 client guild rank-permission name table (static 0x00B617FC, 30
// entries, indices 1..30) contains, verbatim:
//     27  "Real Estate: Guild Plot - Buy"     0x00A105B4
//     28  "Real Estate: Guild Plot - Sell"    0x00A10594
//     29  "Real Estate: Modify Trophies"      0x00A10574
// The whole table, in index order (1-based), is exactly EQEmu's GuildAction
// enum in common/guilds.h -- BANNER_CHANGE=1 ... MEMBERS_DEMOTE_SELF=30. So the
// Guild Management window's Ranks tab ALREADY draws these three rows, the client
// ALREADY sends the permission edits, and the server ALREADY stores them
// (guild_permissions, perm_id 27/28/29) and answers
// guild_mgr.CheckPermission(guild_id, rank, act).
//
// CheckPermission does (perm_value >> (8 - rank)) & 1; a perm_value of 128 = bit for rank 1
// only, i.e. LEADER ONLY.
// ONLY by default, which is live behaviour and is editable in game.
// Rank constants for reference: GUILD_LEADER=1 .. GUILD_RECRUIT=8; the client
// rank-name strings sit right before the permission table at 0x00B617D8.
// ============================================================================


// ============================================================================
// 43.167  *** LIVE 0x85C7 / 0x15B2 IDENTIFIED = ClickDoor / MoveDoor ***
//         (this retires the "gate click opcode" hand-wave in 43.132/instancing)
// ============================================================================
// Two captures, two different windows, one opcode:
//   buying a guild plot          C->S 0x85C7 {11721, -1, 0, 226} -> plot record 0xAB81
//   guild hall -> neighborhood   C->S 0x85C7 {19579, -1, 0,  38} -> 45KB hood list
//   (both) S->C 0x15B2 {last dword of the request, 0}
// The trailing dword is the DOOR ID and the leading dword the door entity id;
// 0x15B2 is the door-state ack echoing the same door id. So live's "gate click"
// is nothing but OP_ClickDoor, and which window opens is decided ENTIRELY
// server-side by that door's opentype (160 = lot square, 159 = neighborhood
// gate, 161 = real estate exit, 43.170).
//
// Worth keeping: leaving a guild
// hall, the player clicks a door that opens the NEIGHBORHOOD SEARCH window, then
// Go (0x9503 {hood_id,-1,0}) -- a gate, not a teleport. The hall's own
// OBJ_FRONTENTRANCE (43.170) is a separate, opentype-161 door.
// ============================================================================


// ============================================================================
// 43.168  *** THE HOUSE/GUILD-HALL DEFINITION RECORD, READ OFF LIVE ***
//         (live captures of the Modest and Palatial halls)
// ============================================================================
// Live sends each real-estate DEFINITION as its own record inside the 0x1900
// feed. Head of the record, verbatim:
//     cstring  definition tag     "guildhall3_int"    / "guildhalllrg_int"
//     cstring  display name       "Modest Guild Hall" / "Palatial Guild Hall"
//     cstring  zone short name    "guildhall3"        / "guildhalllrg"
//     u32      1
//     u32      ZONE ID            0x0B7F = 2943       / 0x0B77 = 2935   (LIVE ids)
//     f32      1.0
//     u32      100
//     ...      f32 0.33 ... f32 1.0, f32 0.4, u32 50, u32 10 ...
//     u32      0xFFFFFFFF
//     f32 x4   ENTRY TRANSFORM (Y, X, Z, heading) -- see 43.169
//     f32 x4   1000.0, -1000.0, 50.0, 1200.0   [inferred: interior bounds]
//     u32 x8   70737,71737,72737,73737,74737,75737,78737,76737  (Palatial)
//              70751,71751,...,76751                            (Modest)
//              -- same eight prefixes, keyed by the CLASSIC zone id (737/751),
//              NOT by live's own zone id. Not item ids (peq's 70751 etc. are
//              unrelated). UNIDENTIFIED; do not invent a meaning.
// then the estate records that reference it, each ending
//     {estate_id, 0xFFFFFFFF, parent_estate_id, 102, DEFINITION_ID, ...,
//      cstring full address}
//
// *** THE DEFINITION IDS ARE items.UNK227, LIVE-EXACT. *** Confirmed on five
// independent rows in one capture:
//     183 = Estate Storage Crate (peq item 62595 UNK227=183)
//      42 = Evantil's Abode
//      40 = Palatial Guild Hall  (peq item 66620 UNK227=40)
//      41 = Modest Guild Hall    (peq item 66660 UNK227=41)
//     111 = the standard player LOT
//     187 = the GUILD PLOT
// The remaining hall items: 66618 Standard (UNK227=38), 66619 Grand (39).
// All four carry UNK236=1.
//
// LIVE ZONE IDS for the halls are 2935 (guildhalllrg) and 2943 (guildhall3),
// which the RoF2 exe does not know. The RoF2 client registers guildhalllrg=737,
// guildhallsml=738, guildhall3=751 (43.150c).
// ============================================================================


// ============================================================================
// 43.169  *** CORRECTION: THE ENTRY TRANSFORM IS (Y, X, Z, HEADING) ***
//         -- 43.151's "{x=-12, y=0, z=5}" has the pair the wrong way round
// ============================================================================
// 43.133 already recorded that live's travel answer 0x9352 is (y, x, z, heading)
// ordered, and PROVED it on the neighborhood: live sends y=-2958 x=2042 z=6
// while peq.zone 712 'neighborhood' holds safe_x 2035, safe_y -2940. 43.151
// then transcribed the HOUSE answer as "x=-12, y=0, z=5" using the other order.
//
// The same four floats appear in the DEFINITION record (43.168), so both halls
// can be read directly:
//     Palatial (guildhalllrg): Y=-12, X=0,   Z=5,   H=128
//     Modest   (guildhall3)  : Y=2,   X=255, Z=-10, H=386
// and the house family is Y=-12, X=0, Z=5 -- which lands the player ON the
// centerline 7.4 units inside the exit door, whose measured seat is X=0,
// Y=-19.36 (43.151b).
// ============================================================================


// ============================================================================
// 43.170  *** LIVE'S GUILD-HALL DOOR LIST -- 27 DOORS FOR guildhalllrg ***
//         (Guild Hall - Palatial, S->C op 0xCB57, 3672 bytes, stride 136)
// ============================================================================
// LIVE DOOR RECORD (136 bytes) -- the RoF2 Door_Struct with every id widened to
// u32 and the position block sent TWICE:
//   +0x00  char[32]  name          "OBJ_FRONTENTRANCE" / "OBJ_INTDOOR"
//   +0x20  f32 y, f32 x, f32 z, f32 heading, f32 incline
//   +0x34  the same five floats again (identical in all 27 records)
//   +0x48  u32 size        (100 on every row)
//   +0x4C  u32 0
//   +0x50  u32 doorid      (1..27, sequential)
//   +0x54  u32 opentype    161 for the front entrance, 27 for every INTDOOR
//   +0x58  u32 door_param  0xFFFFFFFF front entrance, 5 for INTDOORs
//   +0x5C.. zero except two flag bytes at +0x69 and +0x6B (both 1)
//
// *** opentype 161 IS LIVE'S REAL-ESTATE EXIT DOOR *** -- used for house
// interiors and for a guild hall front entrance alike.
//
// THE 27 DOORS (y, x, z, heading) -- door 1 is the exit, 2..27 are 13 PAIRS of
// interior double doors:
//    1 OBJ_FRONTENTRANCE    0.700   88.000    8.237  384   opentype 161
//    2 OBJ_INTDOOR         86.250  -68.600   37.770    0  |  3   86.250  -62.870  37.770  256
//    4 OBJ_INTDOOR         86.240 -176.000   50.180    0  |  5   86.240 -170.266  50.180  256
//    6 OBJ_INTDOOR         86.250 -226.880   50.160    0  |  7   86.250 -221.150  50.160  256
//    8 OBJ_INTDOOR        -85.120 -221.130   50.170  256  |  9  -85.120 -226.870  50.170    0
//   10 OBJ_INTDOOR        -85.100 -170.160   50.170  256  | 11  -85.100 -175.900  50.170    0
//   12 OBJ_INTDOOR        -85.100  -62.840   37.770  256  | 13  -85.100  -68.580  37.770    0
//   14 OBJ_INTDOOR          3.314 -115.570   21.000  128  | 15   -2.430 -115.570  21.000  384
//   16 OBJ_INTDOOR        -84.790  -51.220   -0.640  256  | 17  -84.790  -56.960  -0.640    0
//   18 OBJ_INTDOOR        -55.460 -158.700   -8.020  256  | 19  -55.460 -164.441  -8.020    0
//   20 OBJ_INTDOOR        -55.460 -209.440   -8.010  256  | 21  -55.460 -215.180  -8.010    0
//   22 OBJ_INTDOOR         85.930  -56.970   -0.640    0  | 23   85.930  -51.236  -0.640  256
//   24 OBJ_INTDOOR         56.510 -164.440   -8.020    0  | 25   56.510 -158.701  -8.020  256
//   26 OBJ_INTDOOR         56.520 -215.200   -8.020    0  | 27   56.520 -209.463  -8.020  256
// (peq doors columns are pos_y/pos_x/pos_z/heading in exactly this order, so
//  these transcribe straight into INSERTs; incline 0, size 100, door_param as
//  above, opentype as above, version 0, dest_zone NONE.)
//
// THE MODELS EXIST IN THE RoF2 CLIENT -- checked archive by archive:
//   guildhalllrg.eqg  : obj_frontentrance.mod, obj_intdoor.mod,
//                       obj_guildhalllrgdoor.mod          (27 doors renderable)
//   guildhallsml.eqg  : obj_guildhall_sml_door.mod        -> OBJ_GUILDHALL_SML_DOOR
//   guildhall3.eqg    : NO door model; guildhall3_assets.txt pulls in
//                       guildhallsmldoor.eqg -> GUILDHALL_DOOR_SWITCH
//   guildhall.eqg     : obj_guild_door.mod + obj_guild_door_switch.mod (zone 345)
// Door names must be UPPERCASE (43.151a) or the entity spawns with
// no model.
//
// AND THE OTHER TWO HALLS HAVE NO GEOMETRY DOORS AT ALL: guildhallsml.zon and
// guildhall3.zon each declare exactly ONE object (their terrain), versus
// guildhallLrg.zon's 27. So Grand and Modest need a single opentype-161 exit
// door each and nothing more -- but their seats were NOT captured (the Modest
// session never sent a 0xCB57 and there is no Grand capture).
// NOTE the .zon is NOT a source for these seats: guildhallLrg.zon's 27 objects
// are OBJ_drwynew* doorway FRAMES at completely different coordinates from
// live's 27 door panels -- the frames are scenery, the doors are server data.
// ============================================================================


// ============================================================================
// 43.171  *** LIVE'S NEIGHBORHOOD DOOR LIST -- GUILD-SQUARE SEATS ***
//            (live door list op 0xCB57, 242 records, stride 136)
// ============================================================================
// Live's complete neighborhood door list: 242 doors, doorids 1..242 with no
// gaps. Diffed row-by-row against peq `doors` where zone='neighborhood' AND
// version=0:
//
//     71 x OBP_LOTSQUARE      -- ALL match live to < 0.5 units.
//      1 x OBJ_PH_TELEFLINGER -- matches.
//      1 x OBJ_GUILDGATE      -- matches.
//      1 x OBJ_VOIDPORTAL     -- 0.59 units out; noise.
//      4 x OBP_GUILDSQUARE    -- see below.
//
// Live's guild-square seats (live doorids 226-229):
//   live 228  "200 Brimming Way"      x  569.590  y -2223.136  z  -0.724  h 253
//   live 229  "200 Vanward Heights"   x 3026.698  y  -756.833  z  -0.420  h 123
//   live 227  "200 Terminus Heights"  x -1087.380 y  2428.815  z 108.797  h 385
//   live 226  "200 Guild Way"         x 2689.737  y  2633.203  z 108.873  h 123
//
// Measured against these markers, live's four guild-hall volume centres (43.163)
// are each off by the SAME ~5.07 units:
//     Brimming  vol (571.0, -2228.0)  vs marker (569.590, -2223.136)  5.07
//     Vanward   vol (3031.5, -755.2)  vs marker (3026.698, -756.833)  5.07
//     Terminus  vol (-1092.3, 2427.8) vs marker (-1087.380, 2428.815) 5.02
//     GuildWay  vol (2694.4, 2635.2)  vs marker (2689.737, 2633.203)  5.07
// i.e. a fixed ~5-unit offset in the plot's own frame, not scatter.
//
// *** AND THE ROTATION IS THE MARKER HEADING. *** Against live's headings the
// guild volumes match exactly, modulo the 90-degree symmetry, just like the
// 71 lots:
//     Brimming  h 253 = 177.9 deg, mod 90 = 87.9  == volume  87.9
//     Vanward   h 123 =  86.5 deg, mod 90 = 86.5  == volume  -3.5 (+90)
//     Terminus  h 385 = 270.7 deg, mod 90 =  0.7  == volume   0.7
//     GuildWay  h 123 =  86.5 deg, mod 90 = 86.5  == volume  -3.6 (+90)
// So only the 175.5 x 350 extents plus the ~5-unit centre offset are special
// for guild plots.
// ============================================================================


// ---------------------------------------------------------------------------
// 43.170a  LIVE DOOR RECORD -- two refinements, and the "73 vs 71 lots" answer
// ---------------------------------------------------------------------------
// Refinements to the 136-byte layout in 43.170, found while diffing the live
// NEIGHBORHOOD door list against peq:
//   * +0x54 is NOT a full u32 opentype. The opentype is the low byte (u8/u16);
//     +0x57 is a SEPARATE FLAG BYTE. Live's neighborhood doors 11 and 67 read
//     0x030000A0 there = opentype 160 with flag 3; every other lot square has
//     flag 0. (A naive u32 read prints 50331808 and looks like corruption --
//     it is not.) None of the 27 Palatial hall doors set it, so 43.170's
//     transcription is unaffected.
//   * +0x64 is a per-door number that equals doorid + 100 in the neighborhood
//     (1 -> 101, 11 -> 111, 67 -> 167) and is 0 in every guild-hall record.
//     Not needed for peq rows; recorded so nobody re-derives it.
//
// *** THE LOT COUNT ***  Live's neighborhood carries 76 OBP_LOTSQUARE doors:
// 73 at opentype 160 (two of them flagged, see above) and 3 at opentype 55 --
// the three stacked decoys at (2052,-2798). peq matches: 73 at 160, 3 at 55.
// But live's neighborhood-LIST row advertises 71 player plots + 4 guild plots,
// so live sells 71 of its 73 markers -- the two it withholds are precisely the
// two carrying flag 3.
//
// LIVE NEIGHBORHOOD-LIST ROW, fully decoded (live 0xAB81 45617 B = a 632-row
// roster, and its 84-byte single-row twin):
//     u32       neighborhood id
//     u32 len + char[len]   full name
//     u32       guild id                     (0 / -1 for public)
//     u32       102                          <- see below
//     u32       71    total player plots
//     u32       used player plots
//     u32       4     total guild plots
//     u32       used guild plots
//     u32 len + char[len]   SHORT name
// The RoF2 row block (43.13, verified in game) has neither the 102 nor the
// trailing short name.
//
// *** WHAT 102 IS (well-supported reading, not a proven fact) ***
// The same constant 102 appears in TWO unrelated places, both exactly where a
// definition id belongs: in every neighborhood list row (after the guild id),
// and in every estate record (between parent_estate_id and the estate's own
// definition id -- cleanest sample: a live op 0x149C, 129 B, which is one
// isolated record:
//     {0x07EE04E6, 0xFFFFFFFF, 0x07EE04E4, 102, 40, 32 zero bytes,
//      u32 len, "<neighborhood>, <plot address>, Palatial Guild Hall"}
// ). Since live's other definition ids are 111 = player lot, 187 = guild plot,
// 40/41 = halls, 183 = Estate Storage Crate (43.168), the reading that fits
// both placements is: **102 is live's NEIGHBORHOOD definition id** (f09 = 5),
// carried on each record as its ROOT estate's definition.
// Flagged as a reading because no capture names it directly.
// ---------------------------------------------------------------------------

// ============================================================================
// 43.172  *** KIND-2 (OWNERS PANES) READ OFF THE READER -- THE CRASH LESSON ***
//        , CE on the running client, base 0x3E0000 -> rebase -0x20000.
//          Getting this wrong crashes the client the moment a neighborhood row
//          is HIGHLIGHTED.)
// ============================================================================
// FillNeighborhoodWnd (0x0074AB50) reads the u16 at the cursor FIRST, then
// dispatches on the kind argument; kind==2 branch at 0x0074ABC2 (runtime
// 0x0072ABC2). The wire after {u32 skip, u8 type, u32 kind} is:
//
//   u16 player_count            <- the u16 read at function entry
//   N x string                  -> RENW_PlayerList (+0x24C). The loop body calls
//                                  ONLY the string reader 0x0080C9B0 -- appending
//                                  an id poisons the next row's length field,
//                                  which is exactly the highlight crash.
//   u16 guild_count             <- read fresh at +0x253 (0x0074ADA3)
//   M x u32 guild_id            -> RENW_GuildList (+0x250). ONE DWORD PER ROW,
//                                  nothing else: the loop resolves it through
//                                  0x00425670 (the same guild-id -> name resolver
//                                  the Guild column uses; ecx = 0x00DD5CF8) and
//                                  the RESOLVED name is the row text. An
//                                  unresolvable id renders "Unknown Guild".
//
// LIVE'S kind-2 (112 B) sends MORE per
// guild row -- {u32 guild_id, u32 102, string name} -- which this RoF2 reader
// never consumes: harmless unread tail with ONE guild row (why the capture
// could not disambiguate), a hard desync with two. The 102 is the same
// unexplained constant that sits between parent_estate_id and definition_id in
// every live estate record (43.134/43.170a). Player rows on live are plain
// strings, one per PLOT, duplicates kept, plot order -- same as this reader.
#define RealEstateNeighborhoodWnd__FillKind2_x                      0x0074ABC2  // branch inside 0x0074AB50
// ============================================================================

// ============================================================================
// 43.173  PLACED CREATURES MOVED OUT -- SEE HousingNpc.h (Section 48)
// ============================================================================
// Merchants, mounts, pets and plushies are NOT placed objects. They are
// ordinary NPC spawns carrying a real-estate key, proven both from the client
// (a creature actordef is unreachable from the placed-item factory) and from a
// live guild-hall capture. Anything creature-related now lives in Section 48 --
// findings, offsets and open questions. Section 43 remains the authority on the
// placed-OBJECT path only: furniture, paintings, structures, storage.
//
// The one thing worth carrying back here: *** THE "BAG" IS NOT A MODEL. *** It
// is the actor factory's hardcoded not-found fallback ("IT63_ACTORDEF", then
// "BLOB_ACTORDEF") at 0x0048B7DA / 0x0048B820. Any time a placed object renders
// as a bag, the actordef name did not resolve -- read it as a failed lookup,
// never as wrong art. This also retires 43.96's vague "length condition": the
// factory appends "_ACTORDEF" iff strlen(tag) < 10 and the tag is not the
// literal "PLAYER_1". See 48.1.
// ============================================================================
