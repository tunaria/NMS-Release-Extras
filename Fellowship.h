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
// Fellowship.h -- RoF2 Fellowship + Fellowship Campsite system  (Section 45)
// ============================================================================
// Recovered by static analysis (Ghidra). Every address below is
// CONFIRMED-BY-DISASSEMBLY unless explicitly tagged [inferred]; nothing here
// has been read off a running client or validated against a packet capture.
//
// Numbering continues the eqlib scheme: 43.x = Housing.h, 44.x = Market.h,
// 45.x = this file. Same offset conventions as eqgame.h: static addresses for
// image base 0x400000; the running client is ASLR'd, so REBASE every address
// (observed bases have run static-0x20000, but never assume -- read the
// module base).
//
// EQEmu implements no fellowship system: the server tree declares one name
// for it -- OP_FellowshipUpdate in common/emu_oplist.h -- with no opcode
// mapping in any patch_*.conf, no handler, no struct and no table. The RoF2
// client, by contrast, ships the whole feature client-side and waits to be fed.
// ============================================================================


// ----------------------------------------------------------------------------
// 45.1  THE WIRE PROTOCOL -- COMPLETE OPCODE SET
// ----------------------------------------------------------------------------
// Recovered two ways and cross-checked:
//   C->S : every call to the opcode encoder 0x007D03C0 inside the fellowship
//          code region, reading the `push imm32` that feeds it
//          code region, reading the `push imm32` that feeds it.
//   S->C : the fellowship functions' call sites inside the giant world-message
//          handler, mapped to the nearest case body, then confirmed by
//          disassembling each case body.
//
// None of these opcodes are mapped in EQEmu's stock patch_RoF2.conf.
//
// TRAP: 0x5602 is NOT fellowship. The 1150-byte sender at 0x00667986 sits in
// the same code region but is OP_Feedback (the bug-report window).

// --- client -> server -------------------------------------------------------
#define OP_FellowshipAction_RoF2                                    0x7121   // 45.2 payload 1076 bytes, FIXED
#define OP_FellowshipCampsiteKitRequest_RoF2                        0x0C12   // 45.6 payload 8 bytes
#define OP_FellowshipWndOpenA_RoF2                                  0x2A92   // 45.7 payload EMPTY
#define OP_FellowshipWndOpenB_RoF2                                  0x6D6E   // 45.7 payload EMPTY

// --- server -> client -------------------------------------------------------
#define OP_FellowshipUpdate_RoF2                                    0x6E4D   // 45.3 payload 2564 bytes (fellowship+campsite)
#define OP_FellowshipInvitePopup_RoF2                               0x62AB   // 45.8 payload: +0x0C = inviter spawn id
#define OP_FellowshipCampsiteKitList_RoF2                           0x074D   // 45.9 payload VARIABLE

// Case bodies inside the world-message handler (for re-verification after a
// client patch -- these are positions in one huge switch, not functions):
#define FellowshipUpdate__CaseBody_x                                0x004C55CD  // 0x6E4D
#define FellowshipInvite__CaseBody_x                                0x004C5733  // 0x62AB
#define FellowshipCampsiteKitList__CaseBody_x                       0x004D19E6  // 0x074D


// ----------------------------------------------------------------------------
// 45.2  C->S 0x7121 -- THE ACTION PACKET (1076-byte fixed struct)
// ----------------------------------------------------------------------------
// ONE struct carries every fellowship command; the first dword selects which.
// Built and sent in three places, all memset(0, 0x434) first, all copied out
// with `mov ecx, 0x10D; rep movsd` (0x10D dwords = 1076) after a 2-byte
// opcode -- hence the 0x436 (1078) size push at the socket call.
//
//   struct FellowshipAction_Struct   // 0x434 = 1076 bytes
//   {
//   /*0x000*/ uint32_t action;       // see the action codes below
//   /*0x004*/ uint32_t member_id;    // target character id (invite/remove/promote/toggle)
//   /*0x008*/ uint32_t unknown008;   // [inferred] zero in every observed path
//   /*0x00C*/ uint32_t unknown00C;   // [inferred] zero in every observed path
//   /*0x010*/ uint32_t context;      // = *(LocalPlayer + 0x2054); purpose unconfirmed
//   /*0x014*/ char     text[0x400];  // MOTD body (strncpy limit 0x3FF), else zeroed
//   /*0x414*/ char     name[0x20];   // [inferred] trailing 32 bytes; unused in observed paths
//   /*0x434*/
//   };
//
// ACTION CODES -- ALL CONFIRMED BY DISASSEMBLY, none inferred.
// Codes 1/2/5/8/0xC are written inline by FellowshipWnd::WndNotification.
// Codes 3/6/7/9/0xA/0xB come from the confirm-dialog handler at 0x00668710,
// which switches on the DIALOG id and writes a DIFFERENT wire value -- the two
// numbering schemes do not match, and assuming they did produced a wrong table
// on the first pass here. The mapping, straight off the jump chain:
//     dialog 1 (accept invite popup)     -> action 0x03
//     dialog 2 "Leave current..."        -> action 0x0A
//     dialog 3 "End fellowship..."       -> action 0x0B
//     dialog 4 "Destroy the...campsite"  -> action 0x09
//     dialog 5 "Remove %s..."            -> action 0x06  + member_id
//     dialog 6 "Promote %s to leader"    -> action 0x07  + member_id
// For 5 and 6 the client resolves member_id itself as
// members[selectedIndex].character_id before sending.
#define FellowshipAction_Create                                     0x01   // FP_CreateFellowshipButton
#define FellowshipAction_SetMotd                                    0x02   // text[] carries the new MOTD
#define FellowshipAction_AcceptInvite                               0x03   // the "yes" on the 0x62AB popup
#define FellowshipAction_Unknown04                                  0x04   // not produced by any UI path (decline?)
#define FellowshipAction_Invite                                     0x05   // member_id = current target
#define FellowshipAction_RemoveMember                               0x06   // member_id = selected member
#define FellowshipAction_PromoteLeader                              0x07   // member_id = selected member
#define FellowshipAction_CreateCampsite                             0x08   // FP_CreateCampsite
#define FellowshipAction_DestroyCampsite                            0x09   // FP_DestroyCampsite
#define FellowshipAction_Leave                                      0x0A   // leave, self
#define FellowshipAction_EndFellowship                              0x0B   // disband, leader only
#define FellowshipAction_ToggleXPSharing                            0x0C   // member_id + context

#define FellowshipWnd__ConfirmDialogHandler_x                       0x00668710

// The senders (all three copy 0x10D dwords then push size 0x436):
#define FellowshipWnd__SendAction_A_x                               0x0066884D
#define FellowshipWnd__SendAction_B_x                               0x006695EA
#define FellowshipWnd__SendAction_C_x                               0x00669A29


// ----------------------------------------------------------------------------
// 45.3  S->C 0x6E4D -- THE STATE PACKET.  *** THE CENTRAL STRUCTURE ***
// ----------------------------------------------------------------------------
// The payload is the client's own fellowship object followed by its campsite
// object, and the handler memcpy's them straight into the manager. Sizes are
// exact, taken from the copy loops, not guessed:
//     fellowship : 0x279 dwords = 2532 = 0x9E4 bytes   (copy at 0x004B3860)
//     campsite   : 8     dwords =   32 = 0x20  bytes   (copy at 0x004B3890)
//     TOTAL PAYLOAD                     = 2564 = 0xA04 bytes
//
// The struct closes EXACTLY: the last per-member flag array ends at 0x9E4,
// which is where the campsite begins. That exact closure is the strongest
// evidence the layout below is complete and correctly ordered.
//
//   struct Fellowship_Struct              // 0x9E4 = 2532
//   {
//   /*0x000*/ uint32_t unknown000;        // [inferred]
//   /*0x004*/ uint32_t fellowship_id;     // 0 = not in a fellowship  (THE validity test)
//   /*0x008*/ uint8_t  unknown008[0x40];  // [inferred] very likely the fellowship NAME
//   /*0x048*/ char     motd[0x400];       // 1024; runs exactly up to member_count
//   /*0x448*/ uint32_t member_count;      // HARD CAP 12 -- see 45.4
//   /*0x44C*/ FellowshipMember_Struct members[12];   // stride 0x54, see 45.5
//   /*0x840*/ char     member_str[12][0x20];         // per-member 32-byte string,
//                                                    // written by IsMember() when its
//                                                    // 3rd arg is non-null; purpose
//                                                    // unconfirmed [inferred]
//   /*0x9C0*/ uint8_t  xp_sharing[12];    // getter 0x007F9350 -> "Sharing"/"Not sharing"
//   /*0x9CC*/ uint8_t  xp_capped[12];     // getter 0x007F9370 -> "Capped"
//   /*0x9D8*/ uint8_t  offline_mode[12];  // getter 0x007F9390 -> eqstr 13338 "OFFLINE MODE"
//   /*0x9E4*/
//   };
//
//   struct FellowshipCampsite_Struct      // 0x20 = 32
//   {
//   /*0x000*/ uint32_t unknown000[3];     // [inferred] very likely x/y/z as floats
//   /*0x00C*/ uint32_t zone_id;           // -> zone name via 0x007E3B60
//   /*0x010*/ uint32_t valid;             // 0 = no campsite  (THE validity test)
//   /*0x014*/ uint32_t unknown014[3];     // [inferred]
//   /*0x020*/
//   };
//
// HANDLER BEHAVIOUR (0x004C55CD), which the server must satisfy:
//   1. IsMember(payload, myCharId)  [0x007F93B0]
//      TRUE  -> if I had no fellowship yet AND payload+0x9F4 != 0 AND the
//               window exists, the client AUTO-SENDS 0x0C12 (campsite kit
//               request). Note 0x9F4 is campsite+0x10, i.e. "this fellowship
//               has a campsite".
//               then SetFellowship(payload) and SetCampsite(payload+0x9E4).
//      FALSE -> if my current fellowship_id == payload->fellowship_id, CLEAR
//               my fellowship (SetFellowship(NULL)). *** THIS IS HOW A
//               REMOVE/DISBAND IS DELIVERED: the same opcode, sent to the
//               removed player, carrying a roster they are no longer in. ***
//   2. FellowshipWnd::UpdateAll [0x00667ED0] refreshes the window.
//   3. If the global at 0x00DE0580 is set (a "just joined" latch) and
//      motd[0] != 0, print eqstr 0x2B06 (11014) "Fellowship MOTD: %1" to
//      chat channel 0x0F, then clear the latch.


// ----------------------------------------------------------------------------
// 45.4  THE 12-MEMBER CAP IS ENFORCED CLIENT-SIDE, TWICE
// ----------------------------------------------------------------------------
// IsMember() at 0x007F93B0 opens with `if (member_count > 12) return 0;` --
// an over-full roster makes the client treat EVERY player as a non-member,
// which (per 45.3 step 1) is indistinguishable from "you were removed". A
// server that sends 13 members does not get a truncated list, it gets a
// silent mass-eviction. The invite button independently refuses at >= 12 with
// "Your fellowship is full".
#define FELLOWSHIP_MAX_MEMBERS                                      12


// ----------------------------------------------------------------------------
// 45.5  THE MEMBER ENTRY (0x54 = 84 bytes)
// ----------------------------------------------------------------------------
// Read off the member-list population loop in FellowshipWnd::UpdateAll, which
// walks `base + 0x44C + i*0x54` and fills the 7 list columns. Every field
// below is a column the client actually renders, so all of them must be sent
// or the window shows blanks.
//
//   struct FellowshipMember_Struct    // 0x54 = 84
//   {
//   /*0x00*/ uint32_t character_id;   // leader test compares this to members[0]
//   /*0x04*/ char     name[0x40];     // col 0 (64 bytes; runs exactly to zone_id)
//   /*0x44*/ uint32_t zone_id;        // col 4, rendered via 0x007E3B60; 0 = blank
//   /*0x48*/ uint32_t level;          // col 1
//   /*0x4C*/ uint32_t class_id;       // col 2, rendered via 0x00514DC0
//   /*0x50*/ uint32_t last_seen;      // col 5, time_t -> "MM/DD/YY"; 0 = blank
//   /*0x54*/
//   };
//
// *** LEADERSHIP IS POSITIONAL: members[0] IS THE LEADER. *** There is no
// leader flag anywhere in the struct. Both the leader test at 0x00667CF0 and
// GetLeaderId at 0x00669E20 read members[0].character_id. So "promote to
// leader" is implemented server-side by REORDERING the roster so the new
// leader is index 0 -- get that wrong and every leader-only button lights up
// for the wrong player.
//
// Column 3 is not a struct field: it is a marker string, drawn when
// members[i].character_id == members[0].character_id.
// Column 6 is not a struct field either: it is derived from the three flag
// arrays -- xp_sharing[i] ? (xp_capped[i] ? "Capped" : "Sharing")
//                         : "Not sharing".
// Column 4 shows eqstr 13338 "OFFLINE MODE" instead of the zone when
// offline_mode[i] is set.
//
// KNOWN ODDITY, do not "fix" blindly: the last_seen read is compiled as an
// 8-byte localtime call at +0x50 inside an 84-byte entry, so it reads 4 bytes
// past the end into the next member's character_id. Either the field is a
// 32-bit time_t and Ghidra picked the 64-bit variant, or the client genuinely
// mis-reads dates for all but the last member. Send a 32-bit time_t and
// verify what the window prints before drawing any conclusion.


// ----------------------------------------------------------------------------
// 45.6  C->S 0x0C12 -- CAMPSITE KIT CATALOG REQUEST (8-byte payload)
// ----------------------------------------------------------------------------
//   /*0x00*/ uint32_t show_all;       // 1 when the "Show All" toggle is OFF
//                                     // (the client sends `checked == 0`)
//   /*0x04*/ uint32_t category_mask;  // 1 << <selected combo index>, or -1 when
//                                     // nothing is selected
// Sent by FellowshipWnd::RequestKits [0x006688E0] on the Refresh button, and
// AUTOMATICALLY by the 0x6E4D handler the first time you gain a fellowship
// that already has a campsite. Answer it with 0x074D.
#define FellowshipWnd__RequestCampsiteKits_x                        0x006688E0


// ----------------------------------------------------------------------------
// 45.7  C->S 0x2A92 + 0x6D6E -- WINDOW-OPEN REQUESTS (empty, sent as a PAIR)
// ----------------------------------------------------------------------------
// Both are 2 bytes (opcode only, size push 2) and are sent back-to-back by one
// function on first activation, latched by a flag at wnd+0x27C so they fire
// ONCE per window lifetime. Treat them as "the player opened the fellowship
// window -- send me current state": answer with 0x6E4D (and 0x074D if you
// want the kit list populated).
#define FellowshipWnd__SendOpenRequests_x                           0x0066BA80


// ----------------------------------------------------------------------------
// 45.8  S->C 0x62AB -- INVITE POPUP
// ----------------------------------------------------------------------------
// Handler [0x004C5733] passes the payload straight to
// FellowshipWnd::OnInvite [0x00667DB0], which reads:
//   /*0x0C*/ uint32_t inviter_spawn_id;   // resolved via GetSpawnById 0x005996E0,
//                                          // name read at spawn+0xA4
// and raises the yes/no dialog "%s has invited you to join a fellowship, do
// you wish to accept?" with dialog id 1 and a 100000 ms timeout. A "yes"
// sends 0x7121 with action 0x03 (see 45.2); a timeout or "no" sends NOTHING,
// so the server must not wait on a reply that may never come.
// NOTE it is a SPAWN id, not a character id -- so the invitee must be able to
// see the inviter's spawn, i.e. same zone. Cross-zone invites cannot use this
// see the inviter's spawn, i.e. same zone. Cross-zone invites cannot use this
// popup.
#define FellowshipWnd__OnInvite_x                                   0x00667DB0


// ----------------------------------------------------------------------------
// 45.9  S->C 0x074D -- CAMPSITE KIT CATALOG (variable length)
// ----------------------------------------------------------------------------
// Parsed by FellowshipWnd::FillCampsiteKits [0x006691D0], which is fully
// length-checked (it takes the payload length as its 2nd arg and bounds every
// read), so a short or malformed packet truncates instead of crashing -- rare
// and welcome in this client.
//
//   /*0x00*/ uint32_t count;              // then `count` rows of:
//   struct {
//       uint8_t  category;                // matches Resources\CampsiteCategories.txt
//       uint32_t kit_id;                  // stored at entry+0x00, matched on selection
//       uint32_t value;                   // stored at entry+0x04; purpose unconfirmed
//       char     description[];           // NUL-terminated, client truncates at 0xFE
//   } rows[count];
//
// The client keeps only an 8-byte pair per row (kit_id, value) plus the text
// it pushes into the listbox.
//
// CLIENT-SIDE DATA FILE: Resources\CampsiteCategories.txt maps category id ->
// label and is read at 0x005338A0. It ships with the client and is NOT sent
// by the server, so category ids must match the file the players have:
//     1 Type: General      2 Type: Spell Focus   3 Type: Melee Focus
//     4 Type: Statistics   5 Type: Resistances   6 Type: Illusions
//     7 Type: Buffs
// i.e. live's campsite kits are BUFF providers grouped by effect type.
#define FellowshipWnd__FillCampsiteKits_x                           0x006691D0
#define CampsiteCategories__Reader_x                                0x005338A0


// ----------------------------------------------------------------------------
// 45.10  THE MANAGER AND ITS ACCESSORS
// ----------------------------------------------------------------------------
// Both objects live INSIDE one manager instance; there is no separate
// fellowship allocation. Note the validity flags are not separate booleans --
// they are fields of the objects themselves (fellowship+0x04 = id,
// campsite+0x10 = valid), which is why the accessors look like they do.
#define pinstFellowshipMgr_x                                        0x00DD2630
#define FellowshipMgr__FellowshipOffset                             0x139C   // 0x9E4 bytes
#define FellowshipMgr__CampsiteOffset                               0x1D80   // 0x20 bytes; == 0x139C + 0x9E4

#define FellowshipMgr__GetFellowship_x                              0x004B3820   // null unless +0x13A0 (= id) != 0
#define FellowshipMgr__GetCampsite_x                                0x004B3840   // null unless +0x1D90 (= valid) != 0
#define FellowshipMgr__SetFellowship_x                              0x004B3860   // NULL arg = clear (leave/disband)
#define FellowshipMgr__SetCampsite_x                                0x004B3890

#define Fellowship__IsMember_x                                      0x007F93B0   // (charId, outName32, outFlagA, outFlagB)
#define Fellowship__GetXPSharing_x                                  0x007F9350   // by member INDEX
#define Fellowship__GetXPCapped_x                                   0x007F9370   // by member INDEX
#define Fellowship__GetOfflineMode_x                                0x007F9390   // by member INDEX
#define Fellowship__IsLeaderIndex_x                                 0x00667CF0   // index -> am I the leader
#define Fellowship__GetLeaderId_x                                   0x00669E20   // = members[0].character_id


// ----------------------------------------------------------------------------
// 45.11  THE WINDOW
// ----------------------------------------------------------------------------
// pinstFellowshipWnd is 0x00F714C0; a second, DIFFERENT pointer at 0x00F712AC
// is not interchangeable -- every S->C handler in this file loads 0x00F714C0.
#define pinstFellowshipWnd_x2                                       0x00F714C0   // == eqgame.h pinstFellowshipWnd_x
#define FellowshipWnd__vftable_x2                                   0x009E7C50   // == eqgame.h FellowshipWnd__vftable_x
#define FellowshipWnd__FellowshipWnd_x2                             0x006689F0   // ctor: binds every UI element
#define FellowshipWnd__WndNotification_x2                           0x00669440   // every button handler
#define FellowshipWnd__UpdateAll_x                                  0x00667ED0   // button enable/disable + member list fill

// Member offsets inside CFellowshipWnd, read off the ctor and the handlers.
#define FellowshipWnd__MemberList                                   0x228   // FP_MemberList
#define FellowshipWnd__SelectedMemberIndex                          0x22C
#define FellowshipWnd__DialogBusyFlag                               0x234   // guards double-confirm
#define FellowshipWnd__LeaveButton                                  0x238   // FP_LeaveFellowshipButton
#define FellowshipWnd__CreateButton                                 0x23C   // FP_CreateFellowshipButton
#define FellowshipWnd__MakeLeaderButton                             0x240   // FP_MakeLeaderButton
#define FellowshipWnd__RemoveMemberButton                           0x244   // FP_RemoveMemberButton
#define FellowshipWnd__InviteButton                                 0x248   // FP_InviteMemberButton
#define FellowshipWnd__EndButton                                    0x24C   // FP_EndFellowshipButton
#define FellowshipWnd__ToggleExpSharingButton                       0x250   // FP_ToggleExpSharingButton
#define FellowshipWnd__MotdLabel                                    0x254   // FP_MOTDLabel
#define FellowshipWnd__UpdateMotdButton                             0x258   // FP_UpdateMOTDButton
#define FellowshipWnd__CreateCampsiteButton                         0x25C   // FP_CreateCampsite
#define FellowshipWnd__DestroyCampsiteButton                        0x260   // FP_DestroyCampsite
#define FellowshipWnd__RefreshListButton                            0x264   // FP_RefreshList
#define FellowshipWnd__CampsiteFilterCombo                          0x268   // FP_CampsiteFilter
#define FellowshipWnd__ShowAllButton                                0x26C   // FP_ShowAllButton
#define FellowshipWnd__CampsiteKitList                              0x274   // FP_CampsiteKitList
#define FellowshipWnd__CampsiteViewerLabel                          0x278   // FP_CampsiteViewer
#define FellowshipWnd__CampsiteKitArray                             0x27C   // malloc'd, 8 bytes/row
#define FellowshipWnd__CampsiteKitCount                             0x280

// The UI itself is stock and complete: uifiles\default\EQUI_FellowshipWnd.xml
// already declares every control above. NOTHING needs to change client-side.


// ----------------------------------------------------------------------------
// 45.12  CLIENT-SIDE PERSISTENCE (a hint about live's own model)
// ----------------------------------------------------------------------------
// The client writes and reads two per-character ini values, which tells us
// live tracks a join cooldown per CHARACTER, not per fellowship:
//     "fellowshipID=%u"        (writer 0x007C07A0, reader 0x007C5BF0)
//     "LastFellowshipJoin=%d"
// and eqstr carries the matching refusals:
//     6480 "Cannot add %1 to your fellowship for another %2 hours and %3
//           minutes because they have joined another fellowship too recently."
//     6481 "You cannot create or join a fellowship for another %1 hours and
//           %2 minutes because you have joined another fellowship too
//           recently."
// So live gates re-joining behind a multi-hour cooldown. These strings are
// client-side and free -- a server that wants the same rule just has to keep
// the timestamp and refuse.
#define Fellowship__IniWrite_x                                      0x007C07A0
#define Fellowship__IniRead_x                                       0x007C5BF0


// ----------------------------------------------------------------------------
// 45.13  EVERY FELLOWSHIP eqstr THE CLIENT ALREADY OWNS
// ----------------------------------------------------------------------------
// All of these are in the shipped eqstr_us.txt, so the server gets correct,
// localized wording for free by sending the id rather than a literal.
//    4573  You are not a member of a fellowship.
//    4574  You say to your fellowship, '%1'
//    4585  %1 tells the fellowship,%2 '%3'
//    4586  Fellowship Messages
//    4588  Fellowship Messages
//    4590  You are no longer in a fellowship.
//    4591  Your fellowship has disbanded.
//    4592  %1 has left the fellowship.
//    6474  You have received vitality from your fellowship.
//    6480  Cannot add %1 ... joined another fellowship too recently.
//    6481  You cannot create or join a fellowship for another ...
//    9196  You do not have enough fellowship members in the zone to create the camp.
//   11009  Fellowship window toggle
//   11010  Target is alredy in a fellowship.          [sic -- live's typo]
//   11011  Fellowship is full
//   11013  That player is already the fellowship leader.
//   11014  Fellowship MOTD: %1                         (= 0x2B06, used by the handler)
//   11017  Your Fellowship does not have a campsite.
//   13338  OFFLINE MODE                                (= 0x341A, member zone column)
//
// Two of these are load-bearing design facts, not just text:
//   * 6474 "You have received vitality from your fellowship" -- fellowships
//     grant VITALITY (rested XP), which is the actual reward mechanic.
//   * 9196 "You do not have enough fellowship members in the zone to create
//     the camp" -- campsite creation has a MINIMUM IN-ZONE MEMBER COUNT on
//     live. The threshold is not in the client; pick one server-side.
//
// Strings hard-coded in the client (NOT eqstr, so they are fixed English and
// cannot be reworded server-side) -- useful because they reveal client-side
// rules the server never gets asked about:
//     "You are not in a fellowship"
//     "Your fellowship is full"
//     "That player is in OFFLINE MODE and may not be invited to a fellowship"
//     "You are already the leader"
//     "Please select a member to promote" / "...to remove"
//     "Please select a member to toggle XP sharing."
//     "Only the fellowship leader can toggle XP for other fellowship members."
//     "Please select a camp"
//     "Sharing" / "Not sharing" / "Capped"
