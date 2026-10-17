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
// Chat.h -- RoF2 chat window system  (Section 47)
// ============================================================================
// RoF2 has NO tabbed chat, and it cannot be added with UI XML.
// What it DOES have is up to 32
// independent FLOATING chat windows, each with its own name, filters,
// channel, language and font, plus a 57-slot routing map that sends each
// message category to a chosen window. That is live's *functionality*
// without live's tab strip. See 47.2 for the evidence.
// Static analysis only (Ghidra headless) -- no Cheat
// Engine, no live client. Conventions as eqgame.h: static addresses for image
// base 0x400000; REBASE for the ASLR'd running client.
// Numbering: 43=Housing, 44=Market, 45=Fellowship, 46=Achievement, 47=this.
// ============================================================================


// ----------------------------------------------------------------------------
// 47.1  THE CLASSES (most were already in eqgame.h; collected here for context)
// ----------------------------------------------------------------------------
#define pinstCChatWindowManager_x2                                  0x00F71070   // == eqgame.h
#define CChatWindowManager__CreateChatWindow_x2                     0x006549B0
#define CChatWindowManager__FreeChatWindow_x2                       0x00650420
#define CChatWindowManager__InitContextMenu_x2                      0x006513C0
#define CChatWindowManager__SetLockedActiveChatWindow_x2            0x00650870
#define CChatWindowManager__GetRGBAFromIndex_x2                     0x006508A0
#define CChatWindowManager__SaveToIni_x                             0x0064FE40   // [inferred name]

#define CChatWindow__CChatWindow_x2                                 0x00656100
#define CChatWindow__WndNotification_x2                             0x00656640
#define CChatWindow__AddHistory_x2                                  0x00655AC0
#define CChatWindow__Clear_x2                                       0x00655CB0
#define CChatWindow__vftable_x2                                     0x009E5D70
#define CChatWindow__LoadSettings_x                                 0x00654C80   // reads ChatWindow0_* keys
#define CChatWindow__SaveSettings_x                                 0x00655D10   // [inferred name]

// The UI is a single, tiny screen definition -- no tab control anywhere:
//     uifiles\default\EQUI_ChatWindow.xml  (1,989 bytes)
//         Screen  "ChatWindow"   pieces: CW_ChatOutput (STMLbox), CW_ChatInput (Editbox)
// The pieces are bound BY NAME in CChatWindow's ctor, which is why adding an
// XML <TabBox> does nothing -- see 47.3.


// ----------------------------------------------------------------------------
// 47.2  *** RoF2 HAS NO TABBED CHAT *** -- proven two independent ways
// ----------------------------------------------------------------------------
// (a) THE CONTEXT MENU HAS NO TAB OPTION. Every label built by
//     CChatWindowManager::InitContextMenu was extracted and resolved against
//     eqstr_us.txt (68 ids). The window-management entries are exactly:
//         6644  "New Chat Window"
//         3140  "Rename Window"
//        15009  "Always Chat Here"
//         6645  "Filters"      6646 "Channel"     6647 "Language"
//         6643  "Window"       6047 "Scrollbar"  15038 "Hit Modes"
//     There is no "New Tab", "Add Tab", "Dock", "Combine" or equivalent.
//
// (b) NO CHAT CODE CALLS THE TAB CONTROL. Every caller of the CTabWnd API was
//     enumerated; not one lies in the chat manager/window code range
//     (0x0064F000-0x00658000):
//         CTabWnd::InsertPage        7 callers, none chat
//         CTabWnd::RemovePage        9 callers, none chat
//         CTabWnd::SetPage           5 callers, none chat
//         CTabWnd::GetCurrentTabIndex  0 callers
//         CSidlManagerBase::CreateTabWnd  1 caller, not chat
//     (The tab control is real and works -- Marketplace's Preview/General
//     tabs, item display, LFGuild all use it. Chat simply never touches it.)


// ----------------------------------------------------------------------------
// 47.3  TABS *ARE* A GENERIC XML FEATURE -- the mismatch is subtler than
//       "chat has no tabs"
// ----------------------------------------------------------------------------
// NOTE: tabs are NOT feature-specific
// C++. A <TabBox> listing <Pages>, with each <Page item="..."> declaring its
// own child controls, is built GENERICALLY by the SIDL manager at window
// creation. 27 stock windows do exactly this, e.g. EQUI_MarketplaceWnd.xml:
//
//     <Page item="MKPW_MainPage"> ...controls... </Page>
//     <Page item="MKPW_TaxInfoPage"> ...controls... </Page>          (11 pages)
//     <TabBox item="MKPW_Subwindows">
//         <Pages>MKPW_MainPage</Pages>
//         <Pages>MKPW_TaxInfoPage</Pages> ...
//     </TabBox>
//
// No InsertPage call is needed for XML-declared pages -- that API is only for
// adding pages at RUNTIME, which is why 47.2b finds no chat caller. So a
// <TabBox> added to EQUI_ChatWindow.xml WOULD render and page correctly.
//
// THE ACTUAL LIMIT: a TabBox pages between CONTROLS INSIDE ONE WINDOW, while
// chat streams are SEPARATE WINDOW INSTANCES. CChatWindow's ctor
// (0x00656100) binds exactly one input and one output per instance, by
// SCREENID, through a recursive first-match child lookup:
//
//     CXStr("ChatWindow")   -> build from the screen template
//     CXStr("CWChatInput")  -> GetChildItem(...) -> this[0x89]
//     CXStr("CWChatOutput") -> GetChildItem(...) -> this[0x8A]
//
// (note it looks up the <ScreenID>, not the item= name). First match wins, so
// N pages each holding an STMLbox named CWChatOutput would still leave ONE
// bound box; the rest would be dead. And the client's channel map (47.4)
// routes text per WINDOW INSTANCE, never per control.
//
// => Native, code-free tabbed chat is impossible.


// ----------------------------------------------------------------------------
// 47.4  WHAT ROF2 *DOES* HAVE -- the multi-window model
// ----------------------------------------------------------------------------
// Recovered from CChatWindowManager::SaveToIni (0x0064FE40), which writes the
// whole model to the UI ini under the [ChatManager] section.
//
//   CChatWindowManager layout (offsets confirmed from the save loop):
//     +0x000  CChatWindow* windows[32]    // the window array
//     +0x080  int  window_count           // saved as ChatManager/NumWindows
//     +0x088  int  locked_active_window   // saved as ChatManager/LockedActiveWindow
//     +0x08C  int  channel_map[57]        // saved as ChatManager/ChannelMap%d
//     +0x224  int  hit_mode[8]            // saved as ChatManager/HitMode%d
//     +0x1A4 / +0x1A8 / +0x1B8 / +0x1BC   // context-menu handles (friends lists)
//     +0x180 / +0x194                     // context-menu handles (filters root)
//
// so: **up to 32 chat windows** (array bound), and a **57-entry channel map**
// that assigns every message category (Say/Tell/Group/Guild/OOC/Auction/
// Shout/Emotes/Melee/Spells/Skills/Chat Channel 1-10/...) to a specific
// window. That map is exactly how "put guild chat in its own window" works.
//
//   Per-window ini keys (CChatWindow::LoadSettings, 0x00654C80) -- the "0"
//   is the window index, i.e. ChatWindow0_*, ChatWindow1_*, ...:
//     ChatWindow%d                 ChatWindow%d_Name
//     ChatWindow%d_ChatChannel     ChatWindow%d_DefaultChannel
//     ChatWindow%d_TellTarget      ChatWindow%d_LanguageId
//     ChatWindow%d_FontStyle       ChatWindow%d_Scrollbar
//   (a window that is removed has its _Language / _LanguageId keys deleted)
//
// HOW A PLAYER USES IT (stock client):
//     right-click any chat window -> "New Chat Window"    (creates one)
//                                 -> "Rename Window"      (names the tab-equivalent)
//                                 -> "Filters"            (what it shows)
//                                 -> "Always Chat Here"   (routes typing to it)
// Each new window is independent, dockable by hand, and persists across
// sessions in the ini.
