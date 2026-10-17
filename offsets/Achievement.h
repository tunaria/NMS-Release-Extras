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
// Achievement.h -- RoF2 Achievements system  (Section 46)
// ============================================================================
// Why `/achievements` does not open a window on its own, and what the client
// needs from the server before it will.
//
// SHORT ANSWER: the window is SERVER-GATED. `/achievements` does not open a
// window -- it asks the SERVER for achievement data (C->S 0x47CB) and only
// shows the window once the client has that data. A server that does not
// answer the request leaves the window permanently closed. See 46.2.
//
// Static analysis only (Ghidra). Same conventions as eqgame.h: static
// addresses for image base 0x400000, REBASE for the ASLR'd running client.
// Numbering: 43=Housing.h, 44=Market.h, 45=Fellowship.h, 46=this file.
// ============================================================================


// ----------------------------------------------------------------------------
// 46.1  WHAT THE CLIENT SHIPS (all present, all unused)
// ----------------------------------------------------------------------------
// UI, fully authored and installed:
//     uifiles\default\EQUI_AchievementsWnd.xml    27,656 bytes
//     uifiles\default\EQUI_AchievementWnd.xml     11,514 bytes   (singular =
//                                                 the single-achievement
//                                                 detail popup)
//     Achievement_*.tga  -- 11 textures (titlebar, borders, subcategory
//     base/mouseover/selected, hunter-collect set, achieved/lock/unlock
//     colour swatches)
// So the feature is fully built client-side. Nothing is missing locally.

#define pinstAchievementsWnd_x2                                     0x00D1FD0C   // == eqgame.h pinstAchievementsWnd_x
#define AchievementsWnd__vftable_x2                                 0x009DEAF8   // == eqgame.h
#define AchievementsWnd__AchievementsWnd_x2                         0x0060F9D0   // ctor
#define AchievementsWnd__WndNotification_x2                         0x0060EE70
#define AchievementsWnd__dtor_vftable_x                             0x009DF0E0   // eqgame.h AchievementsWnd_9DF0E0

#define AchievementManager__Instance_x2                             0x00404060   // == eqgame.h
#define AchievementManager__ctor_x                                  0x00403E20
#define pinstAchievementManagerSingleton_x                          0x00B78758   // the Meyers-singleton slot
#define AchievementManager__initGuard_x                             0x00B7875C   // 1 = ctor has run

// RTTI-confirmed data classes already listed in eqgame.h, repeated here for
// context -- they show how rich the intended data model is:
//     AchievementCategory                            vftable 0x00A0222C
//     AchievementComponent                           vftable 0x00A02220
//     AchievementSubComponentCountData               vftable 0x00A02414
//     SingleAchievementAndComponentsInfo             vftable 0x00A023BC
//     SingleAchievementAndComponentsInfoWithCounts   vftable 0x00A023E4


// ----------------------------------------------------------------------------
// 46.2  *** WHY THE WINDOW NEVER OPENS ***  (the whole answer)
// ----------------------------------------------------------------------------
// `/achievements` -> cmd_achievements_handler (eqgame.h already had this one:
// cmd_achievements_handler_x = 0x004EDB00). Decompiled, it is four lines:
//
//     if (pinstAchievementsWnd == NULL) return;            // window not built
//     if (GetWndFlag_1D4(pinstAchievementsWnd) == 0) {
//         send C->S opcode 0x47CB, payload = one byte 0x30, total size 3;
//         // ...and DOES NOT OPEN ANYTHING
//     } else {
//         (*pinstAchievementsWnd->vftable[0xD8])(0, 1, 1);
//     }
//
// The keybind routes to the same handler, which is why the hotkey behaves
// identically to typing the command: **both send a packet and return.**
//
// The client is therefore waiting on the server for the data that makes the
// window openable. There is no client-side path that opens it unprompted.
#define cmd_achievements_handler_x2                                 0x004EDB00   // == eqgame.h
#define OP_AchievementRequest_RoF2                                  0x47CB       // C->S, 3 bytes total
#define ACHIEVEMENT_REQUEST_PAYLOAD_BYTE                            0x30         // the single payload byte

// The gate itself is a GENERIC window field, not an achievement-specific one:
//     0x00864140:  __fastcall(void* wnd) { return *(uint8_t*)(wnd + 0x1D4); }
// ~60 unrelated windows call this same getter, so +0x1D4 is a CXWnd/
// CSidlScreenWnd member (visibility/ready state), NOT an "achievements
// loaded" flag. Do not document it as achievement state.
#define CXWnd__GetFlag1D4_x                                         0x00864140
#define CXWnd__Flag1D4_Offset                                       0x1D4

// [UNRESOLVED] vftable slot 0xD8 resolves to 0x008630D0, which decompiles as
// generic CXWnd geometry/scroll code, not an obvious Show/Activate. The
// intended semantics of that call (open vs toggle-closed) were NOT settled by
// static analysis and would need a live client to confirm. It does not change
// the conclusion: that branch is unreachable until the flag is set, and the
// flag is only set once the client has been fed.
#define AchievementsWnd__vftable_slot_D8_x                          0x008630D0   // [unresolved semantics]


// ----------------------------------------------------------------------------
// 46.4  WHAT IS *NOT* KNOWN (be honest about the gap before scoping work)
// ----------------------------------------------------------------------------
// The REPLY format is NOT recovered. Specifically:
//   * No S->C case body in the complete dispatch map
//   * No S->C case body in the complete S->C dispatch map (706 opcodes, bodies
//     spanning 0x004C32CA..0x004D3E96) was found to feed the achievements window. The
//     handful of handler-range calls that do reach 0x0060xxxx belong to other
//     features (e.g. the call at 0x004C5A29 sits inside OP_UpdateAura's body).
//   * So either the feed opcode's handler is inlined, lives outside the
//     dispatch function, or the data arrives through a path not yet isolated.
//     Two call sites into the achievements code region from 0x004D8xxx belong
//     to FUN_004D7230 -- a large UI/tick function, NOT a message handler.
//
// The AchievementManager's data classes (five, with counts and
// sub-components) are the starting point for recovering the reply format.


// ----------------------------------------------------------------------------
// 46.5  eqstr / misc
// ----------------------------------------------------------------------------
// Achievement-adjacent leads noted while mapping other systems (Housing.h 43.x):
//     "clean leads worth knowing exist for achievements (0x28E0, 0x618F, ...)"
//     "0xDF82  1,550 B  achievement-style name/id table"
// Those are unverified leads recorded in passing, not confirmed achievement
// opcodes. Anyone picking this up should start there AND from 0x47CB's
// expected reply, and should expect to need a live client or a capture --
// static analysis alone did not close it.
