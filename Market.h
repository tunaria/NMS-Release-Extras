// ============================================================================
// Market.h -- RoF2 Marketplace / Station Cash Store
// ============================================================================
// Companion to eqgame.h, kept as its own file ("Section 44").
//
// *** STATUS: STATIC ANALYSIS ONLY. *** Recovered from Ghidra + capstone against
// eqgame.exe; NOTHING here has been confirmed against a live capture. Field names
// tagged [inferred] are hypotheses about meaning. The wire ORDER, the SIZES, the
// node offsets and the opcode numbers are read directly out of the deserializers
// and the dispatcher, and are solid.
//
// Investigated 2026-08-04.
// ============================================================================

#pragma once

// ----------------------------------------------------------------------------
// 0. PREREQUISITES -- COPIED (not moved) from eqgame.h
// ----------------------------------------------------------------------------
// These already exist in eqgame.h at the line numbers shown and remain there.
// They are reproduced here AS COMMENTS ONLY, so this file reads standalone.
//
// Including both headers together is SAFE -- verified 0 macro-name collisions
// between this file and eqgame.h. Every name that was not already defined there
// got a `Market__` prefix precisely to keep it that way; keep that up when
// adding to this file, and re-check case-sensitively (a case-insensitive grep
// gives false hits on legitimate pairs -- see the eqgame.h header note).
//
//
//   eqgame.h:85    __gWorld_x                              0x00DD25AC
//   eqgame.h:112   pinstCEverQuest_x                       0x00E67CCC  <- the two catalog lists hang off this
//   eqgame.h:340   __eq_delete_x                           0x008DB146
//   eqgame.h:341   __eq_new_x                              0x008DBB3B  <- node allocs (0x2118 / 0x94 / 0x34)
//   eqgame.h:523   CEverQuest__dsp_chat_x                  0x0051F1A0
//   eqgame.h:680   CPacketScrambler__hton_x                0x007D03C0  == EQ__EncodeOutboundOpcode_x
//   eqgame.h:751   CStmlWnd__SetSTMLText_x                 0x00883E10  <- consumes item field +0x54
//   eqgame.h:936   StringTable__getString_x                0x007D0660
//   eqgame.h:1145  UdpConnection__Send_x                   0x008C51F0  == EQ__SendMessage_x
//   eqgame.h:1849  OP_ItemPreview                          0x6B5C
//   eqgame.h:1850  OP_ItemPreviewRequest                   0x7F80      <- the ONE marketplace op EQEmu already maps
//   eqgame.h:4480  CXStr__CXStr3_x                         0x00805C20
//   eqgame.h:4482  CXStr__dCXStr_x                         0x00465AE0
//   eqgame.h:4525  pinstCAvaZoneWnd_x                      0x00D1FD40
//   eqgame.h:5859  RealEstate__ReadLengthPrefixedString_x  0x0080C9B0  == Stream__ReadString_x (eqgame.h:13228)
//   eqgame.h:7560  EQ__EncodeOutboundOpcode_x              0x007D03C0
//   eqgame.h:7561  EQ__SendMessage_x                       0x008C51F0
//   eqgame.h:7562  pinstOutboundPacketBuffer_x             0x00DD00E0  // opcode word; body at +2
//   eqgame.h:7563  pinstConnection_x                       0x00DD25AC  // NULL => "Attempt to send message %d on a void connection."
//
// From eqgame.h's Section 12 (kept there, it corrects an entry that lives there):
//   eqgame.h:3612  pinstUnknown_00D1FD08_x   0x00D1FD08   // = CMarketplaceWnd
//   eqgame.h:3613  pinstCMarketplaceWnd_x    0x00D1FD08   // preferred name
//   eqgame.h:3616  pinstAvaZoneWnd_x         0x00D1FD40   // the REAL AvaZoneWnd
//
// *** THIRD INDEPENDENT CONFIRMATION of that correction, found while assembling
// this file: eqgame.h line 4525 ALREADY carried `pinstCAvaZoneWnd_x = 0x00D1FD40`.
// So the file simultaneously held the right answer (4525) and the wrong one (the
// old 0x00D1FD08 "= AvaZoneWnd" note). Two macro names, two addresses, no
// -Wmacro-redefined to catch it. Worth remembering as a failure mode: a wrong
// entry here does not announce itself. ***

// ----------------------------------------------------------------------------
// 0b. NOT named in eqgame.h -- helpers this subsystem leans on
// ----------------------------------------------------------------------------
#define Market__CSidlScreenWnd__WndNotification_base_x  0x0085E740  // shared base; 12/407 vftables use it -- see 44.8
#define Market__CXStr__c_str_x                          0x008061C0  // also a string-id resolver in other contexts
#define Market__CXStr__Release_x                        0x00805B20
#define Market__CXStr__Format_x                         0x00807B90
#define Market__PacketReader__ctor_x                    0x00410C40  // (buf, len) -> cursor object {base, len, pos}
#define Market__PacketReader__ReadU32_x                 0x004048B0
#define Market__CategoryNode__ctor_x                    0x00610500  // 0x34-byte node; root is "Root Property Category"
#define Market__pinstOpcodeTranslator_x                 0x00B828B0  // ecx for EQ__EncodeOutboundOpcode_x
#define Market__bSuppressSendWarning_x                  0x00DDF654  // gates the "void connection" spam
#define Market__ConfirmationDialog_x                    0x0071EB20  // [inferred]
#define Market__FormatMessage_x                         0x004EDFA0  // [inferred] (fmt id, args) -> buffer

// ============================================================================
// Section 44: Marketplace / Station Cash Store (investigated 2026-08-04)
// ============================================================================
// *** STATUS: STATIC ANALYSIS ONLY. *** Nothing here has been validated against a
// live capture yet. Field NAMES below are [inferred] from how the window consumes
// them; the wire ORDER, SIZES and node offsets are read straight out of the
// deserializers and are solid. Treat names as hypotheses until a packet lands.
//
// ---------------------------------------------------------------------------
// 44.1  THE HEADLINE FINDING: the catalog is SERVER-FED over the zone connection
// ---------------------------------------------------------------------------
// The window's Station Cash *funding* path (Add Funds -> credit card) is SOE HTTP
// and is dead: the RTTI block carries a `Uram::` service layer (FundWalletRequest,
// AddPaymentSourceRequest, PurchaseOrderRequest, CasGetSessionTicketRequest, ...)
// talking to auth.station.sony.com. That half cannot be revived and does not need
// to be -- it is a separate code path from the item catalog.
//
// The ITEM CATALOG is ordinary game traffic. On window open, CMarketplaceWnd
// checks two in-memory lists hanging off CEverQuest and, if a list is empty,
// asks the SERVER for it with a 2-byte request. EQEmu has never implemented any
// of this, so all 20 opcodes below are unmapped in patch_RoF2.conf -- verified,
// no collisions with any existing OP_ name.
//
// ---------------------------------------------------------------------------
// 44.2  Globals
// ---------------------------------------------------------------------------
#define CEverQuest__MarketplaceItemList_off        0x00039584   // list A head -- item catalog  [inferred name]
#define CEverQuest__MarketplaceFundingList_off     0x0003958C   // list B head -- funding tiers [inferred name]
// pinstCMarketplaceWnd_x (0x00D1FD08) is defined in Section 12, where it corrects
// an older entry that mislabeled it as AvaZoneWnd. Both lists above are intrusive doubly-linked
// lists: node+0x00 = prev, +0x04 = next, +0x08 = owner; the head pair is {head, tail}.

// ---------------------------------------------------------------------------
// 44.3  Functions
// ---------------------------------------------------------------------------
#define CMarketplaceWnd__ctor_x                    0x006DDB90   // binds "MarketplaceWnd" + 219 MKPW_* children; Ghidra merges ctor+Init+open path (16535 B)
#define MarketplaceList__Clear_x                   0x007F9070   // wipes a list; BOTH parsers call it first -> every feed is a FULL REPLACE
#define MarketplaceItemList__Parse_x               0x007F90E0   // S->C 0x1E9F body -> list A
#define MarketplaceItem__ctor_x                    0x007F8970   // node alloc size 0x2118
#define MarketplaceItem__Deserialize_x             0x007F8A30   // one item entry off the wire (44.5)
#define MarketplaceFundList__Parse_x               0x007F92B0   // S->C 0x09BF body -> list B
#define MarketplaceFund__Deserialize_x             0x007F9170   // one funding entry off the wire (44.6)
#define MarketplaceItemList__FindById_x            0x007F9000   // lookup in list A by the u32 key at node+0x10
#define MarketplaceList__Count_x                   0x007F9270
#define CMarketplaceWnd__PopulateItems_x           0x006DD260   // list A -> item list + details panel
#define CMarketplaceWnd__PopulateFunding_x         0x006D8DE0   // list B -> amount-select list
#define CMarketplaceWnd__PreItemPopulate_x         0x006DC3D0
#define CMarketplaceWnd__PreFundPopulate_x         0x006D93B0
#define CMarketplaceWnd__SendItemPreview_x         0x006D86C0   // sends 0x7F80 = OP_ItemPreviewRequest (the one op EQEmu already knows)
#define MarketplaceMgr__Send2A44_x                 0x005F76F0   // sends 0x2A44 (74 B), 0x3E15 (var), 0x5148 (2 B).
                                                                // NOT the purchase -- see 44.10. Renamed 2026-08-04.
#define MarketplaceMgr__Send14C3_x                 0x005F7F70
#define MarketplaceMgr__Send6C8B_x                 0x005F7EB0   // 14 B
#define MarketplaceMgr__Send7679_x                 0x005F7E20   // 6 B
// The whole 0x005F32xx-0x005F78xx block is marketplace manager code (166 refs to
// the window global cluster there); 0x006D0000-0x006E4000 is the window itself
// plus its preview/details sub-panels.

// ---------------------------------------------------------------------------
// 44.4  Opcodes
// ---------------------------------------------------------------------------
// C->S, all from the window/manager. Recovered from eqgame_cs_opcodes.txt plus
// disassembly of the window-open tail at 0x006E18C0-0x006E1AC0:
//
//   0x5148   2 B   request ITEM CATALOG      sent iff list A is empty on open
//   0x76C3   2 B   request FUNDING TIERS     sent iff list B is empty on open
//   0x356F   2 B   sent unconditionally on open, AND again after 0x1E9F lands  [inferred: "catalog ack / give me my wallet"]
//   0x2A44  74 B   *** NOT the purchase *** -- 72-byte body, only +0x40 set (0x19 or 2). See 44.10.
//   0x3E15  var    "
//   0x14C3  var
//   0x6C8B  14 B
//   0x7679   6 B
//   0x54E6   2 B   sent from 12 tiny manager fns -- looks like a state/keepalive ping
//   0x5BD5  78 B   shared helper 0x006D1AB0, also called on window open
//   0x7F80  82 B   OP_ItemPreviewRequest -- ALREADY mapped in patch_RoF2.conf
//
// S->C. Extracted by walking the zone dispatcher FUN_004CC1D7's compare chain
// (`cmp dword ptr [ebp-0x55b0], imm32` / `je <case>`) and keeping every case whose
// body references the marketplace window global:
//
//   0x1E9F -> 004D2969   *** ITEM CATALOG ***    parses into list A, then repopulates the window
//   0x09BF -> 004D2AEF   *** FUNDING TIERS ***   parses into list B
//   0x0A37 -> 004D2EE9   looks up list A by body+0x40 -- per-item update / purchase result [inferred]
//   0x4B7F -> 004D2B3E   reads body+0x44/+0x48/+0x4C/+0x50, drives wnd 0x006D7F70 + 0x004B3C00
//   0x77A7 -> 004D2C73
//   0x1EBA -> 004D2E5C
//   0x3171 -> 004D3095   sets a window bool from body+0x48 (enable/disable purchasing) [inferred]
//   0x0114 -> 004D30EF   calls wnd 0x006D2C10, no body read
//   0x3036 -> 004CD3C2   calls wnd 0x006D5F00 with body[0] + 1   (this is the opcode 43.78 mis-filed under AvaZoneWnd)
//   0x1B5D, 0x1F6E, 0x20D9, 0x3042, 0x4FFC, 0x6E2A, 0x740D, 0x7AA9
//                        -> 004D2964, which is a bare `jmp` to the dispatcher tail:
//                        the client ACCEPTS AND IGNORES these. Safe to never send.
//
// ---------------------------------------------------------------------------
// 44.5  WIRE FORMAT -- S->C 0x1E9F, the item catalog   (read off 0x007F8A30)
// ---------------------------------------------------------------------------
//   u32 count
//   count x {                                         node is 0x2118 bytes
//     u32   -> +0x0010    item/entry id      (the key MarketplaceItemList__FindById_x matches on)
//     u32   -> +0x08D4    PRICE                                            [inferred]
//     u32   -> +0x08D8    *** ICON ID *** numeric items.icon (e.g. 3621). VERIFIED 44.10.
//     u32   -> +0x08DC
//     cstr  -> +0x0454    max  128 (cap 0x7F)   display name               [inferred]
//     cstr  -> +0x04D4    max 1024 (cap 0x3FF)  *** CATEGORY PATH *** slash-delimited.
//                         This is what builds the whole MKPW_CategoriesTree -- see 44.9.
//                         VERIFIED, not inferred.
//     cstr  -> +0x0054    max 1024   *** DESCRIPTION (STML) ***  CORRECTED 2026-08-04:
//                         0x006DD260 builds a CXStr from this and hands it to
//                         0x00883E10, which eqgame.h:751 already names
//                         CStmlWnd__SetSTMLText_x -- it is STML body text on
//                         MKPW_ItemOfWeekDescription (wnd+0x358), NOT a texture.
//                         An earlier draft of this section called it the icon; it is not.
//                         WHICH FIELD CARRIES THE ICON IS STILL UNKNOWN -- do not
//                         guess one; get it from a capture. This also means the
//                         `icon` column in any DB schema is UNDECIDED.
//     cstr  -> +0x0014    max   64
//     u8    -> +0x08E0
//     u8    -> +0x08E1
//     cstr  -> +0x08E2    max 1024              description                [inferred]
//     cstr  -> +0x0CE2    max 1024
//     u8    -> +0x10E2
//     u32   -> +0x10E4
//     u32   -> +0x10E8
//     u32   -> +0x10EC
//     cstr  -> +0x10F0    max 1024
//     cstr  -> +0x14F0    max 1024
//     cstr  -> +0x18F0    max 1024
//     cstr  -> +0x1CF0    max 1024
//     u32   -> +0x20F0
//     u32   -> +0x20F4
//     u8    -> +0x20F8
//     u32   -> +0x20FC    "WAS" / pre-sale price. 0x006DD260 shows the strikethrough
//                         label only when  entry+0x8D4 < entry+0x20FC  -- which is what
//                         pins +0x8D4 as the price actually charged.
//     u32   -> +0x2100    RELATIVE seconds; client stores time(NULL) + value, and stores
//                         -1 when the field is 0  -> an expiry timestamp   [inferred]
//     u32   -> +0x2104    same relative-time treatment (sale end?)         [inferred]
//     u32   -> +0x2108
//     u32   -> +0x210C
//     u32   -> +0x2110
//     u8    -> +0x2114
//   }
// Every cstr is NUL-terminated and TRUNCATED to its cap, and the reader bails to an
// empty string (leaving the cursor put) if the declared length would overrun the
// packet -- so a short/late field degrades instead of crashing. Node arithmetic
// checks out exactly: 0x2114 + 1 rounds to the 0x2118 allocation.
//
// ---------------------------------------------------------------------------
// 44.6  WIRE FORMAT -- S->C 0x09BF, the funding tiers  (read off 0x007F9170)
// ---------------------------------------------------------------------------
//   u32 count
//   count x {                                         node is 0x94 bytes
//     u32   -> +0x10    amount   (0x006D8DE0 renders it as (float)v * const, printed
//                       "%s%1.2f" normally or "%s%d" when a currency-code string
//                       matches DAT_009EF95C -- i.e. this is the money column)
//     u32   -> +0x0C
//     cstr  -> +0x14    max 64
//     cstr  -> +0x54    max 64   (0x006D8DE0 keys the row match on this one)
//   }
//   0x00 next / 0x04 prev / 0x08 owner / 0x0C / 0x10 / 0x14[64] / 0x54[64] = 0x94. Exact.
//
// This is the "how much Station Cash do you want to buy" list behind Add Funds --
// NOT the item catalog. The client only ever renders it and posts back a selection.
//
// ---------------------------------------------------------------------------
// 44.7  Window-open sequence (disassembly of 0x006E18C0-0x006E1AC0)
// ---------------------------------------------------------------------------
//     006E18EB  call 0x006D8890                    ; this=wnd
//     006E18F0  call 0x006D1AB0                    ; sends 0x5BD5 (78 B)
//     006E18F5  mov  edx,[0x00E67CCC]              ; pinstCEverQuest
//     006E18FB  mov  eax,[edx+0x39584]             ; list A head
//               <walk, count>
//     006E191B  jne  0x006E1997                    ; non-empty -> call 0x006DD260 (populate)
//     006E192E  push 0x5148 / call 0x007D03C0      ; empty     -> ASK THE SERVER
//     006E19A3  mov  eax,[eax+0x3958C]             ; list B head, same shape
//     006E19CE  push 0x76C3                        ; empty     -> ask
//     006E1A3D  call 0x006D8DE0                    ; non-empty -> populate
//     006E1A53  push 0x356F                        ; unconditional
// Because MarketplaceList__Clear_x runs at the top of both parsers, a feed is a
// FULL CATALOG REPLACE, exactly like the housing definition feed 0x2698 (43.142).
// Send the whole catalog in one packet, or each send wipes the previous one.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.8  THE DLL SURFACE: window layout + button dispatch
// ---------------------------------------------------------------------------
//
// CMarketplaceWnd vftable            0x009EFAF8   (ctor stores it at this+0x00)
// ..sub-object vftables              this+0x220 -> 0x009EFAF4 { 0x006D8570 }
//                                    this+0x224 -> 0x009EFAF0 { 0x006DD200 }
//                                    this+0x228 -> 0x009EFAEC { 0x006DA370 }
//   Those three are single-method list/sort callbacks, NOT the click path.
//
// *** CMarketplaceWnd::WndNotification = 0x006E1F70 *** (vftable slot 34, +0x88)
//   Named on two independent legs, because the obvious one is a trap: slot 8
//   (+0x20) is WndNotification for 108 of the windows already in this file, but
//   the Marketplace's slot 8 is 0x0085E740 -- the SHARED CSidlScreenWnd base,
//   which 12 of 407 vftables carry. Detouring slot 8 would hook nothing.
//   The two legs for slot 34: (a) two windows already in this file have their
//   named __WndNotification_x in that same slot, and (b) 0x006E1F70's body is
//   wall-to-wall `cmp <notified wnd>, [this+<child offset>]` against the child
//   pointers below. Also virtual, slot 55 (+0xDC) = 0x006E35D0 [unidentified].
//
// NOTE Ghidra merges 0x006DDB90 (ctor) + Init + the open path into ONE 16535-byte
// function, so getFunctionContaining() answers "0x006DDB90" for addresses that
// belong to entirely different methods. Do not trust it here; use these offsets.
//
// --- child widget pointers inside CMarketplaceWnd (object size 0x684) --------
// Extracted from the ctor by pairing each `push "MKPW_*"` with the following
// `mov [this+disp], eax`. 198 of them; they run 0x344..0x67C and fill the object.
//   +0x344  MKPW_MainScreen
//   +0x348  MKPW_AddCreditCardScreen
//   +0x34C  MKPW_Landing
//   +0x350  MKPW_ItemOfWeekName
//   +0x354  MKPW_ItemOfWeekIcon
//   +0x358  MKPW_ItemOfWeekDescription
//   +0x35C  MKPW_ItemOfWeekNew
//   +0x360  MKPW_ItemOfWeekPrice
//   +0x364  MKPW_ItemOfWeekWas
//   +0x368  MKPW_ItemOfWeekBuyNowBtn
//   +0x36C  MKPW_ItemOfWeekPrice_Icon
//   +0x370  MKPW_ItemOfWeek
//   +0x374  MKPW_Marketing_Anim
//   +0x378  MKPW_Marketing_Anim_CharSelect
//   +0x37C  MKPW_FeaturedItems
//   +0x380  MKPW_NewItems
//   +0x384  MKPW_Thumbnails
//   +0x388  MKPW_HeaderItems_Screen
//   +0x39C  MKPW_ItemList
//   +0x3A0  MKPW_FindEditBox
//   +0x3A4  MKPW_FindBtn
//   +0x3A8  MKPW_Sort_Combobox
//   +0x3AC  MKPW_IconBtn
//   +0x3B0  MKPW_ListBtn
//   +0x3B4  MKPW_CategoriesTree
//   +0x3B8  MKPW_Subwindows
//   +0x3BC  MKPW_AdditionalDetails
//   +0x3C0  MKPW_SubwindowsPage
//   +0x3C4  MKPW_Buttons_Screen
//   +0x3C8  MKPW_MainPage
//   +0x3D0  MKPW_AddCreditCardTitleLabel
//   +0x3D4  MKPW_DebitCardInfoLabel
//   +0x3D8  MKPW_CreditCardCountryLabel
//   +0x3DC  MKPW_CreditCardTypeLabel
//   +0x3E0  MKPW_CreditCardNumberLabel
//   +0x3E4  MKPW_CreditCardSecurityNumberLabel
//   +0x3E8  MKPW_CreditCardSecurityWhatIsSecCodeLabel
//   +0x3EC  MKPW_CreditCardExpirationDateLabel
//   +0x3F0  MKPW_CreditCardNameOnCardLabel
//   +0x3F4  MKPW_CreditCardBillingAddress1Label
//   +0x3F8  MKPW_CreditCardBillingAddress2Label
//   +0x3FC  MKPW_CreditCardCityLabel
//   +0x400  MKPW_CreditCardStateLabel
//   +0x404  MKPW_CreditCardProvinceLabel
//   +0x408  MKPW_CreditCardZipPostalCodeLabel
//   +0x40C  MKPW_CreditCardParentPasswordLabel
//   +0x410  MKPW_CreditCardConfirmLabel
//   +0x414  MKPW_CreditCardCountryComboBox
//   +0x418  MKPW_CreditCardTypeComboBox
//   +0x41C  MKPW_CreditCardExpirationDateMonthComboBox
//   +0x420  MKPW_CreditCardExpirationDateYearComboBox
//   +0x424  MKPW_CreditCardStateComboBox
//   +0x428  MKPW_CreditCardNumberEditBox
//   +0x42C  MKPW_CreditCardSecurityNumberEditBox
//   +0x430  MKPW_CreditCardNameOnCardEditBox
//   +0x434  MKPW_CreditCardBillingAddress1EditBox
//   +0x438  MKPW_CreditCardBillingAddress2EditBox
//   +0x43C  MKPW_CreditCardCityEditBox
//   +0x440  MKPW_CreditCardProvinceEditBox
//   +0x444  MKPW_CreditCardZipPostalCodeEditBox
//   +0x448  MKPW_CreditCardParentPasswordEditBox
//   +0x44C  MKPW_CancelAddCardBtn
//   +0x450  MKPW_FinishAddCardBtn
//   +0x454  MKPW_Details
//   +0x458  MKPW_DetailsName
//   +0x45C  MKPW_DetailsIcon
//   +0x460  MKPW_DetailsPreview
//   +0x464  MKPW_DetailsInspect
//   +0x468  MKPW_DetailsDescription
//   +0x46C  MKPW_DetailsPlayerStudioLabel
//   +0x470  MKPW_DetailsPlayerStudioIcon
//   +0x474  MKPW_DetailsTimeLeft_Label
//   +0x478  MKPW_DetailsAvailableTimeLeft_Label
//   +0x47C  MKPW_DetailsOnSaleFor_Label
//   +0x480  MKPW_DetailsSaleTimeLeft_Label
//   +0x484  MKPW_BuyPrice_Icon
//   +0x488  MKPW_BuyPrice_MoneyTypeLabel
//   +0x48C  MKPW_BuyPrice
//   +0x490  MKPW_QtyDownBtn
//   +0x494  MKPW_QtyEdit
//   +0x498  MKPW_QtyUpBtn
//   +0x49C  MKPW_BuyTotal_Icon
//   +0x4A0  MKPW_BuyTotal_MoneyTypeLabel
//   +0x4A4  MKPW_BuyTotal
//   +0x4A8  MKPW_BuyBtn
//   +0x4AC  MKPW_Buy
//   +0x4B0  MKPW_WasPrice_Label
//   +0x4B4  MKPW_WasPrice
//   +0x4B8  MKPW_AddFundsBtn
//   +0x4BC  MKPW_CancelBtn
//   +0x4C0  MKPW_AvailableFundsUpper
//   +0x4C4  MKPW_PreviewPage
//   +0x4C8  MKPW_PreviewRenderArea
//   +0x4D0  MKPW_PanUpButton
//   +0x4D4  MKPW_PanDownButton
//   +0x4D8  MKPW_PanLeftButton
//   +0x4DC  MKPW_PanRightButton
//   +0x4E0  MKPW_RotateUpButton
//   +0x4E4  MKPW_RotateDownButton
//   +0x4E8  MKPW_RotateLeftButton
//   +0x4EC  MKPW_RotateRightButton
//   +0x4F0  MKPW_ZoomInButton
//   +0x4F4  MKPW_ZoomOutButton
//   +0x4F8  MKPW_ResetButton
//   +0x4FC  MKPW_NoItemPreviewAvailable_Label
//   +0x500  MKPW_PreviewPlayerStudioLabel
//   +0x504  MKPW_PreviewPlayerStudioIcon
//   +0x508  MKPW_TaxInfoPage
//   +0x50C  MKPW_TaxInfoInstructionLabel
//   +0x510  MKPW_TaxInfoZipCodeLabel
//   +0x514  MKPW_TaxInfoZipCodeEdit
//   +0x518  MKPW_TaxInfoNextButton
//   +0x51C  MKPW_PaymentSourcePage
//   +0x520  MKPW_PaymentSourceInstructionLabel
//   +0x524  MKPW_AddCreditCardButton
//   +0x528  MKPW_CreditCardOnFileButton
//   +0x52C  MKPW_OtherPaymentSourceButton
//   +0x530  MKPW_SCCardButton
//   +0x534  MKPW_AmtSelectionPage
//   +0x538  MKPW_AmountSelectionLayout
//   +0x540  MKPW_AmountCardComboBox
//   +0x544  MKPW_AmountSelectConfirmBtn
//   +0x548  MKPW_AmountSelectionInstructionLabel
//   +0x54C  MKPW_AmountVerisignSeal_Icon
//   +0x558  MKPW_AmountCardSelectLabel
//   +0x55C  MKPW_SCCardRedeemPage
//   +0x560  MKPW_SCCardInstructionLabel
//   +0x564  MKPW_SCCardEditBox
//   +0x568  MKPW_SCCardConfirmBtn
//   +0x56C  MKPW_SCChoicePage
//   +0x570  MKPW_SCChoiceConfirmBtn
//   +0x574  MKPW_SCChoiceCancelBtn
//   +0x578  MKPW_SCChoiceEligibleComboBox
//   +0x57C  MKPW_SCChoiceInstructionLabel
//   +0x580  MKPW_ConfirmPage
//   +0x584  MKPW_ConfirmPurchase_Label
//   +0x588  MKPW_ConfirmPurchase_Icon
//   +0x58C  MKPW_ConfirmPurchase
//   +0x590  MKPW_ConfirmPurchase_Qty
//   +0x594  MKPW_ConfirmPurchase_PriceIcon
//   +0x598  MKPW_ConfirmPurchase_PriceMoneyType
//   +0x59C  MKPW_ConfirmPurchase_Price
//   +0x5A0  MKPW_ConfirmPurchase_TotalIcon
//   +0x5A4  MKPW_ConfirmPurchase_TotalMoneyType
//   +0x5A8  MKPW_ConfirmPurchase_Total
//   +0x5AC  MKPW_ConfirmPurchase_ItemRestrictionLabel
//   +0x5B0  MKPW_ConfirmPurchase_ConfirmBtn
//   +0x5B4  MKPW_CreditCardPurchaseSelectionPage
//   +0x5B8  MKPW_CreditCardPurchaseSelection_Icon
//   +0x5BC  MKPW_CreditCardPurchaseSelectionItem_Label
//   +0x5C0  MKPW_CreditCardPurchaseSelection_Price
//   +0x5C4  MKPW_CreditCardPurchaseSelectionCardComboBox
//   +0x5C8  MKPW_CreditCardPurchaseSelectionVerisignSeal_Icon
//   +0x5CC  MKPW_CreditCardPurchaseSelectionSelectBtn
//   +0x5D0  MKPW_CreditCardPurchasePage
//   +0x5D4  MKPW_CreditCardPurchase_Icon
//   +0x5D8  MKPW_CreditCardPurchase
//   +0x5DC  MKPW_CreditCardPurchase_PriceLabel
//   +0x5E0  MKPW_CreditCardPurchase_Price
//   +0x5E4  MKPW_CreditCardPurchase_TaxLabel
//   +0x5E8  MKPW_CreditCardPurchase_Tax
//   +0x5EC  MKPW_CreditCardPurchase_TotalLabel
//   +0x5F0  MKPW_CreditCardPurchase_Total
//   +0x5F4  MKPW_CreditCardPurchase_TodaysChargeLabel
//   +0x5F8  MKPW_CreditCardPurchase_TodaysCharge
//   +0x5FC  MKPW_CreditCardPurchase_RecurringChargeLabel
//   +0x600  MKPW_CreditCardPurchase_RecurringCharge
//   +0x604  MKPW_CreditCardPurchase_Card
//   +0x608  MKPW_CreditCardPurchase_SecCodeEditBox
//   +0x60C  MKPW_CreditCardPurchase_ParentPasswordScreen
//   +0x610  MKPW_CreditCardPurchase_ParentalPasswordEditBox
//   +0x614  MKPW_CreditCardPurchase_ParentalPasswordLabel
//   +0x618  MKPW_CreditCardPurchase_LegalText
//   +0x61C  MKPW_CreditCardPurchase_WhatIsSecCodeLabel
//   +0x620  MKPW_CreditCardPurchaseVerisignSeal_Icon
//   +0x624  MKPW_CreditCardPurchase_BackBtn
//   +0x628  MKPW_CreditCardPurchase_ConfirmBtn
//   +0x62C  MKPW_FundConfirmPage
//   +0x630  MKPW_FundConfirmStationCash
//   +0x634  MKPW_FundConfirmStationCashPromo
//   +0x638  MKPW_FundConfirmStationCashPromoLabel
//   +0x63C  MKPW_FundConfirm_SC_Promo_Icon
//   +0x640  MKPW_FundConfirmPrice
//   +0x644  MKPW_FundConfirmTax
//   +0x648  MKPW_FundConfirmTotal
//   +0x64C  MKPW_FundConfirm_CompletionBtn
//   +0x650  MKPW_FundConfirm_BackBtn
//   +0x654  MKPW_FundConfirm_CardSelected
//   +0x658  MKPW_FundConfirm_SecCodeEditBox
//   +0x65C  MKPW_FundConfirm_ParentPasswordScreen
//   +0x660  MKPW_FundConfirm_ParentalPasswordEditBox
//   +0x664  MKPW_FundConfirm_ParentalLabel
//   +0x668  MKPW_FundConfirm_WhatIsSecCodeLabel
//   +0x66C  MKPW_FundConfirm_LegalText
//   +0x670  MKPW_FundConfirmVerisignSeal_Icon
//   +0x674  MKPW_ResultsPage
//   +0x678  MKPW_Results_Result
//   +0x67C  MKPW_Results_OKBtn
//
// --- button dispatch sites --------------------------------------------------
// Every `cmp <reg>, [this+<child>]` in 0x006E1B00-0x006E4000.
//   006E1BB3   +0x54C  MKPW_AmountVerisignSeal_Icon
//   006E1BD0   +0x540  MKPW_AmountCardComboBox
//   006E1BF3   +0x558  MKPW_AmountCardSelectLabel
//   006E1ECA   +0x384  MKPW_Thumbnails
//   006E1ED3   +0x39C  MKPW_ItemList
//   006E1FAC   +0x4BC  MKPW_CancelBtn
//   006E1FB8   +0x67C  MKPW_Results_OKBtn
//   006E2071   +0x464  MKPW_DetailsInspect
//   006E207D   +0x460  MKPW_DetailsPreview
//   006E210E   +0x3AC  MKPW_IconBtn
//   006E2124   +0x3B0  MKPW_ListBtn
//   006E213A   +0x3A8  MKPW_Sort_Combobox
//   006E214F   +0x3A4  MKPW_FindBtn
//   006E2163   +0x368  MKPW_ItemOfWeekBuyNowBtn
//   006E2189   +0x45C  MKPW_DetailsIcon
//   006E21B0   +0x54C  MKPW_AmountVerisignSeal_Icon
//   006E21BC   +0x670  MKPW_FundConfirmVerisignSeal_Icon
//   006E21C8   +0x5C8  MKPW_CreditCardPurchaseSelectionVerisignSeal_Icon
//   006E21D4   +0x620  MKPW_CreditCardPurchaseVerisignSeal_Icon
//   006E21E0   +0x524  MKPW_AddCreditCardButton
//   006E2217   +0x44C  MKPW_CancelAddCardBtn
//   006E2238   +0x674  MKPW_ResultsPage
//   006E228B   +0x450  MKPW_FinishAddCardBtn
//   006E2356   +0x4A8  MKPW_BuyBtn
//   006E2362   +0x3C8  MKPW_MainPage
//   006E2373   +0x3C8  MKPW_MainPage
//   006E243B   +0x590  MKPW_ConfirmPurchase_Qty
//   006E24EF   +0x5D8  MKPW_CreditCardPurchase
//   006E2546   +0x5BC  MKPW_CreditCardPurchaseSelectionItem_Label
//   006E259D   +0x5C0  MKPW_CreditCardPurchaseSelection_Price
//   006E26EB   +0x58C  MKPW_ConfirmPurchase
//   006E285D   +0x59C  MKPW_ConfirmPurchase_Price
//   006E28C2   +0x5A8  MKPW_ConfirmPurchase_Total
//   006E2931   +0x59C  MKPW_ConfirmPurchase_Price
//   006E29A4   +0x5A8  MKPW_ConfirmPurchase_Total
//   006E29FB   +0x59C  MKPW_ConfirmPurchase_Price
//   006E2A48   +0x5A8  MKPW_ConfirmPurchase_Total
//   006E2B21   +0x518  MKPW_TaxInfoNextButton
//   006E2B29   +0x508  MKPW_TaxInfoPage
//   006E2B3C   +0x508  MKPW_TaxInfoPage
//   006E2B9B   +0x4B8  MKPW_AddFundsBtn
//   006E2BA3   +0x3C8  MKPW_MainPage
//   006E2BB6   +0x3C8  MKPW_MainPage
//   006E2C0D   +0x528  MKPW_CreditCardOnFileButton
//   006E2C15   +0x51C  MKPW_PaymentSourcePage
//   006E2C28   +0x51C  MKPW_PaymentSourcePage
//   006E2C45   +0x52C  MKPW_OtherPaymentSourceButton
//   006E2C4D   +0x51C  MKPW_PaymentSourcePage
//   006E2C60   +0x51C  MKPW_PaymentSourcePage
//   006E2C74   +0x530  MKPW_SCCardButton
//   006E2C7C   +0x55C  MKPW_SCCardRedeemPage
//   006E2C8F   +0x51C  MKPW_PaymentSourcePage
//   006E2CB7   +0x574  MKPW_SCChoiceCancelBtn
//   006E2CD0   +0x674  MKPW_ResultsPage
//   006E2CF1   +0x570  MKPW_SCChoiceConfirmBtn
//   006E2D94   +0x568  MKPW_SCCardConfirmBtn
//   006E2D9C   +0x55C  MKPW_SCCardRedeemPage
//   006E2DAF   +0x55C  MKPW_SCCardRedeemPage
//   006E2DC3   +0x544  MKPW_AmountSelectConfirmBtn
//   006E2DCF   +0x534  MKPW_AmtSelectionPage
//   006E2DE6   +0x534  MKPW_AmtSelectionPage
//   006E2EBB   +0x650  MKPW_FundConfirm_BackBtn
//   006E2EC3   +0x62C  MKPW_FundConfirmPage
//   006E2ED6   +0x62C  MKPW_FundConfirmPage
//   006E2F2C   +0x64C  MKPW_FundConfirm_CompletionBtn
//   006E2F34   +0x62C  MKPW_FundConfirmPage
//   006E2F47   +0x62C  MKPW_FundConfirmPage
//   006E2FAF   +0x5B0  MKPW_ConfirmPurchase_ConfirmBtn
//   006E2FBB   +0x580  MKPW_ConfirmPage
//   006E2FD2   +0x580  MKPW_ConfirmPage
//   006E30B6   +0x5CC  MKPW_CreditCardPurchaseSelectionSelectBtn
//   006E30C2   +0x5B4  MKPW_CreditCardPurchaseSelectionPage
//   006E30D5   +0x5B4  MKPW_CreditCardPurchaseSelectionPage
//   006E3146   +0x624  MKPW_CreditCardPurchase_BackBtn
//   006E314E   +0x5D0  MKPW_CreditCardPurchasePage
//   006E3161   +0x5D0  MKPW_CreditCardPurchasePage
//   006E3190   +0x628  MKPW_CreditCardPurchase_ConfirmBtn
//   006E3198   +0x5D0  MKPW_CreditCardPurchasePage
//   006E31AB   +0x5D0  MKPW_CreditCardPurchasePage
//   006E31DC   +0x490  MKPW_QtyDownBtn
//   006E31E4   +0x3C8  MKPW_MainPage
//   006E31F7   +0x3C8  MKPW_MainPage
//   006E324D   +0x498  MKPW_QtyUpBtn
//   006E3255   +0x3C8  MKPW_MainPage
//   006E3268   +0x3C8  MKPW_MainPage
//   006E32DB   +0x384  MKPW_Thumbnails
//   006E32F1   +0x380  MKPW_NewItems
//   006E3307   +0x37C  MKPW_FeaturedItems
//   006E3430   +0x674  MKPW_ResultsPage
//   006E344D   +0x3A0  MKPW_FindEditBox
//   006E351C   +0x45C  MKPW_DetailsIcon
//   006E3F0A   +0x628  MKPW_CreditCardPurchase_ConfirmBtn
//   (92 sites)
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.9  CATEGORIES ARE PURE DATA -- there is no category feed  [VERIFIED]
// ---------------------------------------------------------------------------
// Stated plainly: the server never
// sends a category list. The MKPW_CategoriesTree is built entirely from the
// CATEGORY PATH STRING carried on each item (entry+0x4D4, 44.5).
//
#define CMarketplaceWnd__FindOrCreateCategory_x    0x006D89E0   // (this, const char* path) -> node*
#define CMarketplaceWnd__RebuildCategoryTree_x     0x006DCF40   // wipes + refills MKPW_CategoriesTree (this+0x3B4)
#define MarketplaceCategory__ctor_x                0x00610500   // node ctor; root is "Root Property Category"
// window fields: this+0x280 = category count, this+0x284 = category array,
//                this+0x334 = next category id (sequential, client-assigned),
//                this+0x270/0x274 = item row count/array.
// category node (0x34 bytes): +0x0C id, +0x18 visible flag, +0x1C parent,
//                             +0x30 ITEM COUNT.
//
// FindOrCreateCategory, from the decompile at 0x006D89E0:
//   1. linear-search this+0x284 for a node whose stored name == the FULL path;
//      hit -> return it.
//   2. miss -> strrchr(path, '/').  leaf = after the last '/', parentPath =
//      before it.  No slash at all -> leaf = whole string, parentPath = "".
//   3. parentPath empty -> parent is the node with +0x0C == 1 (the root);
//      otherwise RECURSE on parentPath. So a deep path auto-creates every
//      ancestor -- the server never has to declare them.
//   4. allocate 0x34, take the next id from this+0x334, construct, append,
//      grow the array.
//
// Call site in the item-list populate (0x006DD260), which also shows the one
// special case baked into the client:
//     006DD7C1  push 0x0D
//     006DD7C3  lea  edi,[ebx+0x4D4]           ; the item's category path
//     006DD7CA  push 0x009EFC94                ; "Player Studio"
//     006DD7D8  call strncmp                   ; 13 chars
//     006DD7E2  jne  0x006DD7E7                ; no match -> use path as-is
//     006DD7E4  add  edi,0x0E                  ; match   -> skip "Player Studio/"
//     006DD7EC  call 0x006D89E0                ; FindOrCreateCategory
//
// CONSEQUENCES:
//   * To add a category, send an item whose +0x4D4 is that category name.
//     "A" -> one top-level node.  "A/B" -> nested, both
//     created automatically.
//   * *** AN EMPTY CATEGORY IS INVISIBLE. *** RebuildCategoryTree only emits a
//     node when its item count (+0x30) is > 0. A category with no items will
//     silently not appear -- do not debug that as a packet fault.
//   * Category ids are assigned by the CLIENT (this+0x334), not the server.
//     A category id is never sent and never round-trips.
//   * Avoid a literal leading "Player Studio/" unless that is what you mean --
//     the client strips exactly 14 characters from it.
//
// The opcode that LOOKS like a category feed is not one: S->C 0x77A7
// (handler 0x004D2C73) is `u32 count` then per entry {u32 itemId, u32, u32, u32,
// CXStr}, and its receiver 0x006D9490 matches an EXISTING catalog row by
// entry+0x10 == itemId. It is a per-item UPDATE (price/state), not a category
// list. Filed here because the 4-ints-plus-a-name shape is a convincing decoy.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.10  ROUND 2 (2026-08-04, later): wallet, purchase, icon -- and two of my
//        own earlier claims corrected. Still static-analysis only.
// ---------------------------------------------------------------------------

// ===== 44.10a  THE WALLET.  "Current Funds" is set by TWO opcodes ============
#define CMarketplaceWnd__SetAvailableFunds_x       0x006D7F70   // (this, uint32 amount)
#define CMarketplaceWnd__m_cachedFunds_off         0x0000023C   // last value written
//
// void CMarketplaceWnd::SetAvailableFunds(uint32 amount)      // 0x006D7F70, 184 B
// {
//     if (this[+0x23C] == amount) return;                 // CACHED: same value = no-op
//     this[+0x23C] = amount;
//     sprintf(buf, "%d", amount != 0xFFFFFFFF ? amount : 0);
//     if (this[+0x4C0])                                   // MKPW_AvailableFundsUpper
//         pFunds->SetWindowText(buf);                     // vtable +0x124
//     this->0x006D5850();
// }
//
// Consequences:
//   * It is a PLAIN INTEGER ("%d"). No decimals, no currency symbol, no cents
//     scaling.
//   * 0xFFFFFFFF is a sentinel that RENDERS AS 0 -- and is also the "not yet
//     known" value the manager tests for (see 0x005F76F0, which fires the
//     0x5148 catalog request only when this[+0x23C] == -1). Do not send -1 as
//     a real balance.
//   * It is cached and EARLY-OUTS on an unchanged value. Re-sending the same
//     balance is free but also does nothing -- if you need the label refreshed
//     you must actually change the number.
//
// The two callers, both inside the zone dispatcher FUN_004CC1D7:
//
//   S->C 0x4B7F  -> handler 0x004D2B3E, calls SetAvailableFunds at 0x004D2B7C
//         004D2B4D  cmp  dword ptr [body+0x44], 0      ; must be 0   (status/error?)
//         004D2B5D  cmp  dword ptr [body+0x50], 1      ; must be 1   (kind?)
//         004D2B75  push dword ptr [body+0x48]         ; <<< THE BALANCE
//         004D2B7C  call 0x006D7F70
//         004D2B87  movzx eax, byte ptr [body+0x4C]    ; a bool, then ->0x004B3C00
//     Body is the raw packet ([ebp+0x10]), read as a STRUCT -- no reader object --
//     so the packet must be at least 0x54 bytes with those four fields in place.
//     This is the standalone "here is your balance" push.  [field names inferred]
//
//   S->C 0x0A37  -> handler 0x004D2EE9, calls SetAvailableFunds at 0x004D306D
//         004D2EF8  mov  ecx, [body+0x40]              ; entry id
//         004D2F02  add  ecx, 0x39584                  ; -> MarketplaceItemList__FindById_x
//         004D3060  mov  ecx, [body+0xDC]              ; <<< THE NEW BALANCE
//         004D306D  call 0x006D7F70
//     Looks up an EXISTING catalog row by id AND updates the wallet in one go:
//     this is the PURCHASE RESULT.  [inferred, but the shape is unambiguous]

// ===== 44.10b  THE PURCHASE REQUEST is C->S 0x4A74, not 0x2A44 ===============
// Only TWO opcodes are sent from anywhere inside CMarketplaceWnd::WndNotification
// (0x006E1F70): 0x356F at 0x006E3672, and 0x4A74 at 0x006E302A. The latter sits
// inside the MKPW_ConfirmPurchase_ConfirmBtn arm that starts at 0x006E2FAF.
//
//   *** C->S 0x4A74 -- 82 bytes on the wire = 2 opcode + 80 body ***
//
//     body[0x00..0x3F]  ZEROED   (explicit memset 64,0 at 0x006E2FFC)
//     body[0x40..0x47]  NOT initialised -- 8 bytes of stack residue
//     body[0x48]  u32   marketplace entry id   (from item entry +0x10)
//     body[0x4C]  u32   quantity               (from wnd +0x260)
//
//   Build, at 0x006E2FEC..0x006E304A:
//     006E2FEC  push 0x40 / lea eax,[esp+0x24] / push 0 / push eax
//     006E2FFC  call 0x008DB160            ; memset(buf, 0, 64)
//     006E3001  mov  ecx,[esi+0x10]        ; esi = wnd+0x248 = selected entry
//     006E3004  mov  edx,[ebp+0x260]       ; quantity
//     006E300D  mov  [esp+0x68],ecx        ; buf+0x48
//     006E3011  mov  [esp+0x6C],edx        ; buf+0x4C
//     006E3025  push 0x4A74 / call 0x007D03C0
//     006E302F  push 0x52                  ; 82
//     006E3037  mov  ecx,0x14              ; 20 dwords = 80 bytes
//     006E303C  lea  esi,[esp+0x24]        ; == the memset buffer
//
//   Guards the client checks BEFORE sending -- a server test harness must drive
//   the window through them, it cannot just click Buy:
//     - notified widget == MKPW_ConfirmPurchase_ConfirmBtn (wnd+0x5B0)
//     - wnd+0x580 (MKPW_ConfirmPage) is non-null
//     - the CURRENT page of MKPW_Subwindows (wnd+0x3B8) IS that ConfirmPage
//     - wnd+0x248 (selected entry) is non-null
//
//   SERVER SIDE: parse u32 at body+0x48 and u32 at body+0x4C, IGNORE the rest,
//   and *** never trust body[0x40..0x47] -- it is uninitialised client stack and
//   will differ run to run. *** Reply with 0x0A37 (entry id at +0x40, new balance
//   at +0xDC) to update the row and the wallet together.
#define OP_MarketPurchase_x                        0x4A74       // C->S, 82 B  [inferred name]
#define MarketPurchase__EntryId_off                0x00000048
#define MarketPurchase__Quantity_off               0x0000004C
#define CMarketplaceWnd__SelectedEntry_off         0x00000248
#define CMarketplaceWnd__Quantity_off              0x00000260

// ===== 44.10c  CORRECTION: 0x2A44 is NOT the purchase ========================
// Earlier in this file 0x2A44 was tagged "[inferred] purchase" on nothing more
// than its 74-byte size. It is not. Sender 0x005F76F0:
//     005F77A1  cmp  dword ptr [ebp+4], -1     ; only when that field is -1
//     005F77B2  mov  dword ptr [esp+0x58], 0x19
//     005F77C4  push 0x2A44 / push 0x4A (74) / mov ecx,0x12 (72 B) / lea esi,[esp+0x18]
// The 72-byte body is copied from a stack buffer in which exactly ONE dword is
// ever written: body+0x40 (= esp+0x58 - esp+0x18). It is 0x19 here and 2 on the
// other path (0x006D9B00). Everything else is uninitialised stack.
// So 0x2A44 is a discriminated request/notify with a code at +0x40, carrying no
// item id and no quantity. Whatever it is, it is not a purchase.
// LESSON, worth keeping: packet SIZE is not evidence of packet MEANING. 74 bytes
// looked purchase-shaped and was not.

// ===== 44.10d  0x3E15 is a plain string packet ==============================
// From 0x005F7921..0x005F7990: allocates strlen+3 when the string exceeds 0x1FE,
// writes the opcode word at buf+0, then memcpy's a CXStr's c_str to buf+2.
// i.e. { u16 opcode; char text[]; }. Nothing marketplace-specific in its shape.

// ===== 44.10e  THE ICON FIELD -- SOLVED: item entry +0x8D8 ==================
// Confirmed at two independent sites, both reading the field off the item entry
// and handing it to the texture setter 0x0087A860:
//     006DB9FC  mov edx,[ebp+0x8D8]   -> 006DBA14 add edx,0xFFFFFE0C -> 006DBA24 call 0x0087A860
//     006DD943  mov eax,[eax+0x8D8]   -> 006DD95B add eax,0xFFFFFE0C -> 006DD979 call 0x0087A860
//   where 0xFFFFFE0C = -500.  So:
//
//     texture index = iconId - 500        and       iconId == 0  ->  0x150 (336)
//
// It is a NUMERIC items.icon value (e.g. 3621), NOT a
// string. This is the same A_DragItem convention housing found for plot-list row
// art (eqgame.h 43.162), which is good corroboration.
//   * DB schema consequence: the marketplace `icon` column is INT UNSIGNED and
//     can be copied straight from items.icon. It is NOT a VARCHAR.
//   * 0 is the safe "no icon" value; it renders the 0x150 blank rather than
//     indexing off the end of the atlas.
//   * 0xFFFFFE0B (-501) appears in 0x006D7C00 / 0x006D7C20 -- that is the panel
//     CLEAR path, not an icon value. Do not copy it into a feed.
#define MarketplaceItem__IconId_off                0x000008D8   // numeric items.icon
#define Market__SetTextureFromIcon_x               0x0087A860   // (this, iconId - 500)
#define Market__kIconBlankTexture                  0x150        // used when iconId == 0
#define Market__kIconBias                          500          // subtract before calling

// ===== 44.10f  running score on the 0x1E9F item entry ======================
//   +0x0010  entry id          VERIFIED (purchase echoes it; FindById matches on it)
//   +0x08D4  price             VERIFIED (gates the "was" strikethrough vs +0x20FC)
//   +0x08D8  ICON ID           VERIFIED (44.10e)
//   +0x08DC  ?                 still unknown
//   +0x0454  display name      [inferred]
//   +0x04D4  CATEGORY PATH     VERIFIED (44.9)
//   +0x0054  description STML  VERIFIED (CStmlWnd__SetSTMLText_x)
//   +0x20FC  "was" price       VERIFIED
//   +0x2100/+0x2104  relative-seconds timestamps  [inferred]
//   ~13 further fields still unnamed -- send 0, add columns when a capture names them.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.11  ROUND 3 (2026-08-04): the remaining item fields, the small opcodes,
//        and the right-click question. Still static-analysis only.
// ---------------------------------------------------------------------------

// ===== 44.11a  METHOD WARNING -- READ THIS BEFORE TRUSTING ANY SWEEP ========
// capstone's Cs.disasm() is a generator that STOPS SILENTLY at the first byte it
// cannot decode. A single `md.disasm(whole_text, base)` over eqgame.exe's .text
// yields ~13,000 instructions and then quits -- .text is ~6 MB and really holds
// >300,000 in the marketplace block alone. It does not raise; it just ends.
//
// A sweep written that way reported "0 consumers" for EVERY item-entry field,
// including +0x8D8, which 44.10e had already proven is the icon. That is a
// confidently wrong, entirely plausible-looking answer. Always resync:
//
//     off = start
//     while off < end:
//         n = 0
//         for insn in md.disasm(data[off:end], va): emit(insn); n += 1
//         if n == 0: off += 1; va += 1                 # skip the bad byte
//         else:      adv = last.address - va + 1; off += adv; va += adv
//
// Sanity check every sweep against a KNOWN-GOOD hit before believing a negative.
// "0 sites" is a claim about the tool until proven a claim about the binary.

// ===== 44.11b  ITEM ENTRY -- four more fields identified =====================
#define MarketplaceItem__MaxQty_off                0x000008DC   // per-transaction cap; 0 = no cap
#define MarketplaceItem__Kind_off                  0x000010E8   // product-kind enum
#define MarketplaceItem__bFlagA_off                0x000008E1   // renders "*"
#define MarketplaceItem__bFlagB_off                0x000020F8   // pairs with FlagA in list filters
#define MarketplaceItem__MoneyType_off             0x0000210C   // 0 = Station Cash, 1 = real money
//
// +0x8DC  MAX QUANTITY per purchase.  0x006D58B1:
//     mov edx,[entry+0x8DC] / test edx,edx / je skip
//     cmp [wnd+0x260],edx / jae  <cannot increment>
//   i.e. 0 means "no per-item cap". There is ALSO a hard client cap of 10
//   (`cmp edx,0xA` immediately after), so the effective limit is min(0x8DC, 10).
//   A good candidate for a real DB column.
//
// +0x10E8  PRODUCT KIND (enum). Values observed: 3, 4, 6.
//     0x006D2D23  kind == 3  -> takes entry+0x10 and calls 0x00463C10 via [0xB82908]
//     0x006D2DF4  kind == 4 or 6 -> SKIP the affordability test entirely;
//                 otherwise compare entry+0x8D4 (price) against wnd+0x23C (wallet)
//     0x006D58CE  kind == 6 -> the Qty-Up affordability test is bypassed
//   So 4 and 6 are kinds the client does not price-check. Send 0 unless you have
//   a reason; an unexamined kind could silently disable the funds check.
//
// +0x8E1 and +0x20F8  two paired booleans, always tested together
//     0x006D8074  cmp [entry+0x8E1],al / jne .. / cmp [entry+0x20F8],al / je ..
//     0x006DAC81  reads one or the other depending on a branch, then filters
//     0x006D39AD  [entry+0x8E1] != 0  ->  the string "*"  (0x009C4678), else ""
//   They gate which rows land in the MKPW_NewItems / MKPW_FeaturedItems panels,
//   and 0x8E1 additionally paints an asterisk. Exact which-is-which is NOT
//   settled -- "new" vs "featured" is [inferred] and a capture should confirm.
//   Tiny accessors: 0x006D1B87 returns +0x8E1, 0x006D1B90 returns +0x20F8.
//
// +0x210C  MONEY TYPE -- the field that decides the currency icon and format:
//     == 0   icon = the "A_Station" Ui2DAnimation, price printed with base-10
//            _ultoa  -> an INTEGER, no decimals            (0x006D3B45..0x006D3BBD)
//     == 1   NO icon (0x008575B0 clears the cell icon), price = (float)price *
//            const, printed "%1.2f" (0x009CE934)           (0x006D3BC2..0x006D3C4x)
//     >= 2   falls straight through to the epilogue at 0x006D3C5E -- the price
//            cell is left BLANK. Do not send anything but 0 or 1.
//
// +0x2104 is read at 0x006DAD27 / 0x006DBB21 in the timers path -- consistent
// with the 44.5 reading that +0x2100/+0x2104 are relative-seconds timestamps.
//
// STILL UNCONSUMED anywhere in 0x005F0000-0x006E6000 (verified with the resynced
// sweep, not the broken one): +0x8E0, +0xCE2, +0x10E2, +0x10E4, +0x10EC, +0x18F0,
// +0x1CF0, +0x20F0, +0x20F4, +0x2108, +0x2110, +0x2114, and +0x10F0 / +0x14F0
// (each only ever `lea`'d, never read). Send 0 / empty for all of these.

// ===== 44.11c  THE CURRENCY ICON IS AN XML EDIT, NOT A PACKET FIELD =========
// With MoneyType == 0 the client hard-codes the texture name "A_Station"
// (string 0x009EF498) -- it is not settable from the wire. But "A_Station" is a
// UI-defined animation, not a baked-in bitmap:
//     uifiles/default/EQUI_Animations.xml:12920   <Ui2DAnimation item="A_Station">
//     uifiles/default/EQUI_MarketplaceWnd.xml     ~9 <Normal>A_Station</Normal> refs
// So repointing A_Station at a different icon changes the coin everywhere in the
// window with NO code and NO packet work. (Check A_StationColumn at :12939 too.)
// Note the knock-on: A_Station may be referenced by other windows, so prefer
// editing the Marketplace XML's references over redefining the shared animation
// if anything else uses it.

// ===== 44.11d  the small C->S opcodes, decoded =============================
//   0x7679   6 B  = 2 opcode + ONE u32.            sender 0x005F7E20, 6 callers.
//   0x6C8B  14 B  = 2 opcode + { u32 a; u32 b; u32 c }   sender 0x005F7EB0:
//                     body+0x00 = mgr+0x44
//                     body+0x04 = mgr+0x40
//                     body+0x08 = 0xFFFFFF00 | (byte)mgr+0x4C   <- note the
//                                 upper 3 bytes are literally 0xFFFFFF
//   0x14C3  var   = 2 opcode + NUL-terminated C string.  sender 0x005F7F70.
//   0x3E15  var   = same shape as 0x14C3.               (44.10d)
//   0x54E6   2 B  = opcode only, NO body. Sent from twelve near-identical
//                   one-line functions (0x005F33E0, 0x005F3490, 0x005F3540,
//                   0x005F35F0, 0x005F55E0, 0x005F5780, 0x005F5AA0, 0x005F5CB0,
//                   0x005F5FD0, 0x005F60E0, 0x005F6250, 0x005F6720) -- a state
//                   ping fired from many places, carrying nothing.
//   0x5BD5  78 B  sender 0x006D1AB0. Body NOT yet decoded -- still open.
//   0x2A44  74 B  see 44.10c: 72-byte body, only +0x40 written (0x19 or 2).
//                 Purpose still unknown; it is NOT the purchase.
// Both string opcodes switch to a heap buffer above 0x1FE bytes; below that they
// build in the shared static packet buffer at 0x00DD00E0.

// ===== 44.11e  RIGHT-CLICK: there is NO context menu ========================
// Answered definitively. CMarketplaceWnd::WndNotification (0x006E1F70) opens:
//     006E1F7E  mov eax,[esp+0x14]      ; the message code
//     006E1FA3  cmp eax,1               ; XWM_LCLICK
//     006E1FA6  jne 0x006E3448          ; everything else -> the tail
// so the entire 92-site button table in 44.8 is LEFT-CLICK ONLY. The tail
// handles exactly these, and nothing else (values named from eqlib CXWnd.h:41+):
//     1   XWM_LCLICK        the whole button body, 0x006E1FAC-0x006E3448
//     2   XWM_LMOUSEUP      0x006E3584
//     3   XWM_RCLICK        0x006E3517
//     6   XWM_HITENTER      0x006E3448   MKPW_FindEditBox, MKPW_QtyEdit
//     11  XWM_CHILDCLOSED   0x006E2343, 0x006E356A, 0x006E35AB
//     14  XWM_NEWVALUE      0x006E349C   MKPW_QtyEdit, MKPW_AmountCardComboBox
//     43  XWM_SELITEM_DOWN  0x006E3547   walks the 11-entry array at wnd+0x2A0
//                                        (the preview pan/rotate/zoom repeat)
//
// The XWM_RCLICK arm in full:
//     006E351C  cmp ebx,[ebp+0x45C]     ; ONLY MKPW_DetailsIcon
//     006E3522  jne <ignore>
//     006E352E  call 0x00875BD0         ; via [0x015D3D00]
//     006E3535  test al,0xC             ; modifier-key bits
//     006E353D  call 0x006D2DB0
//
// CONCLUSION: right-click is accepted on exactly ONE widget, the item's details
// icon, and does the usual item-icon thing. There is no CContextMenuManager use
// anywhere in the marketplace block, and no right-click handling on MKPW_ItemList
// or MKPW_Thumbnails at all.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.12  REMOVING CONTROLS FROM THE XML: HIDE, DO NOT DELETE
// ---------------------------------------------------------------------------
// Add Funds is NOT packet-reachable: only two opcodes are sent from anywhere in
// WndNotification (0x356F, 0x4A74) and neither is on that path. Past "pick an
// amount" it is Uram/HTTPS to auth.station.sony.com.
//
// *** HOW TO REMOVE THE DEAD CHROME: HIDE IT, DO NOT DELETE IT. ***
// Deleting a control from EQUI_MarketplaceWnd.xml leaves its member NULL, and
// the client does NOT null-check everything. Confirmed unguarded dereferences:
//
//     006E3750  mov eax,[esi+0x4B8]  / lea ecx,[eax+0xE8] / call   <- AddFundsBtn
//     006E379D  mov ecx,[esi+0x4B8]  / lea ecx,[ecx+0xE8] / call   <- AddFundsBtn
//     006D74F3  mov eax,[esi+0x528]  / lea ecx,[eax+0xE8] / call   <- CreditCardOnFileButton
//
// With the control absent those become a near-null deref (ecx = 0xE8) and the
// client dies. Keep every MKPW_* control DECLARED so the ctor binds a real
// pointer, and hide it instead (off-screen position / zero size).
//
// Safer but still not worth deleting -- these are PASSED to the page-switch
// helper 0x006D3C80 rather than dereferenced, and it guards its own state:
//     +0x51C MKPW_PaymentSourcePage        (0x006E2BD4)
//     +0x534 MKPW_AmtSelectionPage         (0x006D621A, 0x006E2EEE)
//     +0x5D0 MKPW_CreditCardPurchasePage   (0x006D975C)
//     +0x62C MKPW_FundConfirmPage          (0x006D8192)
//
// METHOD NOTE: a first pass at this audit counted the ctor's own
// `mov [esi+0xNNN], eax` BINDS as reads and reported "unguarded" for all nine
// controls. Writes are not derefs. Classify by instruction direction before
// calling anything a crash risk.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.13  ROUND 4 (2026-08-04): the last four unknowns, all closed.
// ---------------------------------------------------------------------------

// ===== 44.13a  WHAT ENABLES "Buy Now" -- SOLVED ============================
#define CMarketplaceWnd__UpdateButtonStates_x      0x006D5850   // 11 callers incl. SetAvailableFunds
#define CMarketplaceWnd__CanBuy_x                  0x006D2DF0   // (entry) -> bool
#define CMarketplaceWnd__SelectionNode_off         0x0000024C   // entry == *(node)
//
// From the decompile of 0x006D5850, MKPW_BuyBtn is enabled iff ALL FOUR hold:
//     this[+0x24C] != 0                  selection node
//     this[+0x248] != 0                  the selected item entry
//     *(int*)(this[+0x24C] + 8) == 0     a selection-state flag
//     CanBuy(this[+0x248]) != 0
// and CanBuy (0x006D2DF0) is simply:
//     entry+0x10E8 == 4 || entry+0x10E8 == 6   (kinds that skip pricing)
//     || entry+0x8D4 (price) <= this+0x23C (wallet)
//
// *** A GREYED BUY BUTTON WITH AN EMPTY LIST IS NOT A BUG. *** +0x248/+0x24C are
// both set together at 0x006DB6EE when a row is SELECTED, and the list is empty,
// so nothing can be selected. It resolves itself the moment 0x1E9F lands. None
// of the four conditions is server-controlled except indirectly (price vs wallet).
//
// Same function also gates the quantity buttons:
//     QtyUp   (+0x498): (entry+0x8DC == 0 || qty < entry+0x8DC)
//                       && qty < 10 && entry+0x10E8 != 6
//                       && (qty+1) * entry+0x8D4 <= this+0x23C
//     QtyDown (+0x490): qty > 1
// -- which independently corroborates 44.11b's readings of +0x8DC and +0x10E8.

// ===== 44.13b  NEW vs FEATURED -- SETTLED, no longer inferred ==============
#define MarketplaceItem__bIsNew_off                0x000008E1   // was bFlagA
#define MarketplaceItem__bIsFeatured_off           0x000020F8   // was bFlagB
#define CMarketplaceWnd__FillPanel_x               0x006DAA40   // (panel, mode, arg)
//
// The row-fill 0x006DAA40 takes a MODE and filters on it (0x006DAC70):
//     mode 0 -> no filter at all
//     mode 1 -> require entry+0x20F8
//     mode 2 -> require entry+0x8E1
// PopulateItems calls it three times back to back at 0x006DDB30/47/5E:
//     push 2, [wnd+0x380] MKPW_NewItems       -> mode 2 -> +0x8E1   == IS NEW
//     push 1, [wnd+0x37C] MKPW_FeaturedItems  -> mode 1 -> +0x20F8  == IS FEATURED
//     push 0, [wnd+0x384] MKPW_Thumbnails     -> mode 0 -> the main unfiltered list
// Corroborated: 0x006D39AD paints "*" (string 0x009C4678) when +0x8E1 is set,
// and the XML has MKPW_New_Label. Supersedes the "[inferred]" note in 44.11b.
//
// 0x006D8030 (run after the 0x77A7 update feed) uses the pair to auto-navigate
// the category tree: if either flag is set it selects category[0], else
// category[1] -- and it requires wnd+0x280 > 2, i.e. at least three categories
// must exist before it will do anything.

// ===== 44.13c  0x5BD5 -- an empty ping ======================================
// Sender 0x006D1AB0, 9 call sites (window open, and 8 others). 78 B = 2 + 76:
//     memset(body, 0, 0x40);      // +0x00..0x3F all zero
//     body+0x40 = 0; body+0x44 = 0; body+0x48 = 1;
// It carries no item id, no selection, nothing. The server may ignore it.

// ===== 44.13d  0x2A44 -- the "&Marketplace=" deep link ======================
// Its sender 0x005F76F0 is reached from 0x004FD4A0, which parses the launch/URL
// parameter "&Marketplace=" (0x00479EE0(&buf, "&Marketplace=", &out) at that
// function's ~line 951 in the decompile) before calling it. So 0x2A44 is the
// client announcing "open the marketplace, reason <code at body+0x40>" -- 0x19
// from this path, 2 from 0x006D9B00. It is a deep-link/open notification, NOT a
// purchase (44.10c), and the server may ignore it.
//
// FAMILY NOTE, useful when decoding the rest: 0x2A44 (72 B), 0x5BD5 (76 B) and
// 0x4A74 (80 B) all share one shape -- a ~64-byte zeroed header followed by a
// few dwords. Only 0x4A74 carries real payload (entry id at +0x48, qty at +0x4C).
// Treat a large mostly-zero marketplace packet as this base struct plus a tail.

// ===== 44.13e  the unconsumed item fields -- confirmed, with the caveat =====
// Re-run with the RESYNCED sweep (44.11a), these entry fields have no reader
// anywhere in 0x005F0000-0x006E6000:
//     +0x8E0, +0xCE2, +0x10E2, +0x10E4, +0x10EC, +0x18F0, +0x1CF0,
//     +0x20F0, +0x20F4, +0x2108, +0x2110, +0x2114
// plus +0x10F0 and +0x14F0, which are only ever `lea`'d, never dereferenced.
// They ARE parsed off the wire into the node (44.5) and then never read.
// Send 0 / empty for all of them.
// CAVEAT worth stating: "no reader in the marketplace code" is not "no reader in
// the client". The sweep was bounded to that range because a whole-.text sweep is
// slow; a stray consumer elsewhere would be missed. Nothing observed suggests one,
// but do not upgrade this from "unconsumed" to "unused" without a wider pass.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.15  THE CATALOG ALONE IS NOT ENOUGH -- categories need a SECOND feed
//        (2026-08-04, proven in-game with Cheat Engine)
// ---------------------------------------------------------------------------
#define OP_MarketplaceItemUpdate_x                 0x77A7   // S->C, per-item state
#define MarketplaceRow__State_off                  0x00000008 // row+0x08; 2 = "not counted"
//
// Feeding ONLY the catalog (0x1E9F) produces a window that parses everything
// every category node ends with +0x30 (item count) == 0 -> not rendered.
// and RebuildCategoryTree only emits a node whose count is > 0 (44.9). The only
// category that showed was the client's own built-in "New and Featured".
//
// WHY, from CMarketplaceWnd::PopulateItems:
//     006DD757  mov [esi+8], 2      ; fresh row init
//     006DD7D1  mov [esi+8], 2      ; and again on the CONVERGED path, so both the
//                                   ; new-row and reused-row cases end up at 2
//     006DD7EC  call FindOrCreateCategory
//     006DD7F7  cmp dword ptr [esi+8], 2
//     006DD7FE  mov [esi+0x1C], edx ; row->category id IS always set
//     006DD801  je  006DD814        ; ...but == 2 SKIPS the next line
//     006DD811  inc dword ptr [edi+0x30]   ; <- the category's item count
// So a catalog feed can never raise any category above 0. The count line is
// unreachable from that path alone.
//
// WHAT WRITES row+0x08: only the 0x77A7 receiver 0x006D9490 (`if (row[2] !=
// state)`), and its dispatcher tail calls RebuildCategoryTree (0x006DCF40) plus
// 0x006DC550 / 0x006D8030. So the order is CATALOG THEN STATES, and the
// states packet is what actually makes the tree appear.
//
// WIRE FORMAT, S->C 0x77A7 (handler 0x004D2C73):
//     u32 count
//     count x {
//       u32 entry_id     matched against catalog entry+0x10
//       u32 state        -> row+0x08. ANY VALUE EXCEPT 2.
//       u32 item_id      -> row+0x0C. THE REAL EQ ITEM ID -- drives the
//                          Preview tab (44.15b). 0 = "No item preview
//                          available".
//       u32 ?            -> row+0x10 (written verbatim; purpose unmapped)
//       u32 len + char[len]     *** LENGTH-PREFIXED, not NUL-terminated ***
//                          category-path override -> FindOrCreateCategory
//                          -> row+0x20; raw string kept at row+0x24 (44.15a)
//     }
// The string reader here is 0x0080C9B0 (eqgame.h Stream__ReadString_x), which
// reads a u32 length FIRST -- unlike the catalog's own strings in 44.5, which
// are NUL-terminated. Mixing those two up desyncs the whole packet.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.15a  ...AND THE STATE FEED HAS A TIMING GATE: the rows must ALREADY EXIST
//         (2026-08-04, live disassembly)
// ---------------------------------------------------------------------------
// A 0x77A7 sent before the window has built its rows has no effect: row+0x08
// stays 2. The receiver explains it -- there is no pending-state store anywhere:
//
//   0x006D9490 (receiver, __thiscall, ret 0x18 -- six args):
//     args, left to right: (u32 entry_id, u32 state, u32 f3, u32 f4,
//                           u8* out_changed, CXStr category_path BY VALUE)
//     - bails first thing if wnd+0x318 != 0 (same byte the packet handler
//       0x004D2C73 checks via the one-line getter 0x004B3C30; 0 = accept)
//     - walks the LIVE ROW ARRAY wnd+0x270 matching row->entry->+0x10 ==
//       entry_id. No match (or count 0) -> the update evaporates. Nothing is
//       queued for later.
//     - if the CXStr is non-empty: FindOrCreateCategory(path) -> row+0x20.
//       So the four mystery fields were: state, ?, ?, CATEGORY OVERRIDE. The
//       override also resolves the "?" u32s: f3/f4 are consumed only on that
//       category path (not yet mapped).
//     - writes state into row+0x08 only when it differs, sets *out_changed.
//   Back in 0x004D2C73: if ANY entry changed, the tail itself calls
//   RebuildCategoryTree (0x006DCF40) -- no extra poke needed from the server.
//
// The rows in wnd+0x270 are built when the WINDOW opens, not when the catalog
// packet lands. So a state feed pushed at zone-in writes into an empty array.
//
// THE CUE THE CLIENT GIVES: OP_MarketplaceReady (0x356F, C->S, 2 bytes) is
// sent on window open and again after a catalog lands -- i.e. precisely when
// the row array exists. Answer EVERY Ready with the 0x77A7 state feed and the
// timing solves itself; re-sends are idempotent because the receiver only
// touches rows whose state differs.
//
// Do NOT rely on 0x5148 for this: with the catalog pushed at zone-in the
// client's list is never empty at open, so RequestItems never fires again.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.15b  PREVIEW TAB = row+0x0C, fed by the 0x77A7 item_id field
//         (2026-08-04, live disassembly)
// ---------------------------------------------------------------------------
#define CMarketplaceWnd__SelectedRow_off           0x0000024C // wnd+0x24C -> selected row
#define MarketplaceRow__ItemId_off                 0x0000000C // row+0x0C, from 0x77A7 field 3
#define MarketplaceRow__CategoryOverride_off       0x00000020 // row+0x20, from 0x77A7 CXStr
//
// The full known row layout (from receiver 0x006D9490 + PopulateItems):
//   +0x00  catalog entry*      +0x08  state (2 = uncounted)
//   +0x0C  ITEM ID             +0x10  u32 from 0x77A7 field 4 (unmapped)
//   +0x1C  base category id    +0x20  override category id (-1 = none)
//   +0x24  override path CXStr
//
// SendItemPreview (0x006D86C0, one arg) is called from both call sites as
//     SendItemPreview( selected_row->itemId )   // [[wnd+0x24C]+0x0C]
// and if the arg is 0 it routes to "No item preview available" instead
// (0x006B5AD0 path). Non-zero -> it emits OP_ItemPreviewRequest (0x7F80, 82B,
// item id at body+0x00) -- an opcode STOCK EQEMU ALREADY HANDLES:
// Handle_OP_ItemPreviewRequest serializes the item and replies on the SAME
// opcode (ENCODE(OP_ItemPreviewRequest) in common/patches/rof2.cpp), and the
// client renders it in MKPW_PreviewRenderArea. So the entire preview feature
// costs the server ONE field: put the real item id in 0x77A7 field 3.
//
// State bookkeeping seen in the same receiver: transitions to/from state 2
// walk BOTH row+0x1C and row+0x20 through 0x006B3790 (category-by-id) and
// 0x006C4080(+1/-1) to keep the per-category item counts in step -- more
// proof that 2 means "not counted".
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.16  THE SC COIN GRAPHIC (pure UI-XML)
// ---------------------------------------------------------------------------
// Every Station Cash coin in the window is one of two Ui2DAnimations:
//   A_Station        (40x34 @ 0,0 in station.tga)  -- 11 uses, ALL in
//                    EQUI_MarketplaceWnd.xml (tiles, Price/Total rows, funds)
//   A_StationColumn  (20x17 @ 40,0 in station.tga) -- only user is the
//                    Header_StationCash FrameTemplate (EQUI_Templates.xml)
// The money-type-0 draw path (44.5, +0x210C == 0) resolves the animation BY
// NAME, so re-pointing the animation frames re-skins every coin at once.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.17  SKIPPING THE ZIP-CODE (TAX INFO) PAGE = wallet flag byte
//        (2026-08-04, live disassembly of the Buy click)
// ---------------------------------------------------------------------------
#define CMarketplaceWnd__ZipOnFile_off             0x00000240 // wnd+0x240, u8
#define CMarketplaceWnd__SetZipOnFile_x            0x004B3C00 // mov [ecx+240],arg
#define CMarketplaceWnd__GetZipOnFile_x            0x004B3C10 // (0x4B3C30 = the
                                                              //  +0x318 getter, 44.15a)
//
// After Buy Now populates the confirm labels, the page choice is one byte
// (WndNotification, static 0x006E2AAE):
//     cmp byte [wnd+0x240], 0
//     je  -> SetPage( wnd+0x508 = MKPW_TaxInfoPage )   "Please enter your zip"
//     ... -> SetPage( wnd+0x580 confirm page )         (kind 6 detours to the
//                                                       SC-card pages instead)
// wnd+0x240 is SOE's "billing zip already on file" flag, and the ONLY remote
// writer is the WALLET packet (0x4B7F): its receiver 0x004D2B3E, on the
// kind==1 path, does SetAvailableFunds(body+0x48) then 0x004B3C00(body+0x4C).
// So the wallet struct's u8 at +0x4C IS this flag. Server sends 1 ->
// purchases go straight to the confirm page; the zip page is unreachable.
// (The TaxInfoNextButton handler at 0x006E2B21 also validates+sets it.)
//
// Buy-click entry state, for reference (static 0x006E2356): selected catalog
// entry = wnd+0x248, quantity value = wnd+0x260, and the entry KIND
// (+0x10E8) forks the flow: 3 = dialog, 4 = strcmp(+0x10F0,+0x14F0) special,
// 6 = credit-card pages. Kind 0 takes the plain confirm path.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.18  ITEM PREVIEW: the S->C 0x7F80 reply needs a TRAILING u8
//        (2026-08-04, live disassembly -- why the render stayed empty)
// ---------------------------------------------------------------------------
// With a real item id in row+0x0C (44.15b) the client fires
// OP_ItemPreviewRequest (0x7F80, 82B) and stock EQEmu replies with a
// serialized item, yet the Preview tab can still show "No item preview
// available".
//
// The S->C receiver is the WndProc-level dispatcher at 0x004CD065 (opcode
// 0x7F80 arm). After deserializing the item it:
//   - reads the item's IDFile and string-compares it to "IT63" (0x009CB164)
//     and "IT64" (0x009CB15C) -- the generic placeholder models. A match
//     sets a "no real model" bool.
//   - then reads ONE MORE BYTE off the packet tail (the [esp+14]/[esp+10]
//     "showAsPurchased/isValid" flag) and forwards it, with the item, to
//     the render setup 0x006FFA70 -> 0x006FF980. That routine only clears
//     the "No item preview available" label + arms the 11 model slots
//     (loop of 0xB at +0x258) when the byte says "valid".
//
// Stock ENCODE(OP_ItemPreviewRequest) (common/patches/rof2.cpp) writes ONLY
// the serialized item and stops. The client then reads that trailing flag
// from uninitialised buffer tail -> usually nonzero-garbage-but-wrong, and
// the label never hides. The client expects a trailing u8 after the item:
// 0 selects plain-render, 1 purchased-render.
//
// CORRECTION (see 44.20): the two window globals the receiver checks are NOT
// "in-game vs char-select marketplace". 0x00D1FD08 is pinstCMarketplaceWnd and
// 0x00D1FD4C is pinstObjectPreviewWnd (already in eqgame.h:248) -- two
// DIFFERENT windows, and the receiver picks between them by REPLY OPCODE.
// That is the real reason the pane stayed blank; the trailing byte above is
// necessary but not sufficient.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.20  *** THE PREVIEW PANE IS CHOSEN BY REPLY OPCODE ***  (0xB07F)
//        (2026-08-05, PROVEN with a live 4-point breakpoint trace)
// ---------------------------------------------------------------------------
#define OP_MarketplaceItemPreview_x                0xB07F  // S->C -> INLINE pane
#define OP_ItemPreviewRequest_x                    0x7F80  // C->S, and S->C -> pop-up
#define ItemPreviewReq__Context_off                0x0000004C // u16 in the 80-byte body
//
// There are TWO item-preview render targets and the client picks between them
// ONLY by the opcode the reply arrives on. Dispatcher 0x004CD065, tail:
//     004CD236   cmp  [ebp+0C], 0000B07F        ; [ebp+0C] = REPLY OPCODE
//     004CD23D   jne  004CD2D0
//     004CD243   cmp  [pinstCMarketplaceWnd], 0
//     004CD2B1   mov  ecx, [pinstCMarketplaceWnd]
//     004CD2B7   call 006D5E70                  ; <- MARKETPLACE INLINE PANE
//   004CD2D0:
//     004CD2D0   cmp  [pinstObjectPreviewWnd], 0
//     004CD33E   mov  ecx, [pinstObjectPreviewWnd]
//     004CD344   call 006FFA70                  ; <- POP-UP OBJECT PREVIEW WND
//
// BOTH windows REQUEST with 0x7F80 (82 B). They are distinguished by a u16 at
// body+0x4C, and the two sender functions are otherwise instruction-identical:
//     CMarketplaceWnd::SendItemPreview  0x006D86C0 -> 0x006D86ED  mov word [esp+54],1
//     ObjectPreviewWnd:: (send)         0x006FFC20 -> 0x006FFC4D  mov [esp+54],bx  (0)
// (esp+54 lands at body+0x4C; the body is the 0x50 bytes copied by the
//  repe movsd at 0x006D8732 / 0x006FFC90.)
//
// So the original server read that flag and replied on 0xB07F for marketplace
// requests. EQEmu has NO 0xB07F mapping at all and echoes 0x7F80 for
// everything -> every marketplace preview renders into the ObjectPreviewWnd,
// which is normally closed, so the inline pane shows "No item preview
// available" forever. The item, the model and the packet were all fine.
//
// PROOF (logging breakpoints, clicking a marketplace item):
//     0x006D86C0 SendItemPreview   HIT, EAX = the item id      (request sent)
//     0x004CD236 opcode compare    HIT                          (reply routed)
//     0x004CD344 object render     HIT                          <- WRONG WINDOW
//     0x004CD2B7 marketplace pane  NEVER HIT
//
// So a server must answer marketplace requests (context 1) on 0xB07F and
// object-preview requests (context 0) on 0x7F80. Payload is byte-identical.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.21  ITEM OF THE WEEK + SALE PRICING -- both already on the wire
//        (2026-08-05, static analysis of PopulateItems)
// ---------------------------------------------------------------------------
#define CMarketplaceWnd__PickItemOfWeek_x          0x007F9030 // the selector
#define CMarketplaceWnd__ItemOfWeekEntry_off       0x00000264 // wnd+0x264, entry* or null
#define MarketplaceItem__bIsItemOfWeek_off         0x000010E2 // u8  eligibility
#define MarketplaceItem__ItemOfWeekRank_off        0x000010E4 // u32 tie-break, HIGHEST wins
//
// No new opcode and no new wire field: +0x10E2 / +0x10E4 were already in the
// 44.5 entry layout.
//
// SELECTION. PopulateItems (0x006DD260) runs, near its end:
//     006DD6C3   mov  esi, [CEverQuest + 0x39584]     ; catalog list head
//     006DD6DB   call 007F9030                        ; pick
//     006DD6E4   mov  [wnd + 0x264], eax              ; null => panel stays empty
// and 0x007F9030 is just:
//     for (e = head; e; e = e->next /* +0x04 */)
//         if (e->+0x10E2 && (!best || e->+0x10E4 > best->+0x10E4)) best = e;
//     return best;
// Strictly-greater, so equal ranks resolve to the FIRST match walking the list.
// PreItemPopulate (0x006DC3D0) zeroes wnd+0x264 first, so a catalog with no
// eligible entry reliably clears the panel back to "None currently listed"
// (eqstr 4686) rather than keeping a stale one.
//
// RENDER. The block at 0x006DD8C1..0x006DDB1E is skipped entirely when
// wnd+0x264 is null. Otherwise it reads, from the SAME entry struct as the
// tiles -- no separate feed:
//     +0x454 name        -> wnd+0x350 MKPW_ItemOfWeekName
//     +0x8D8 icon        -> wnd+0x354, drawn as icon-500 (006DD95B: add eax,-500)
//     +0x054 description -> wnd+0x358, via SetSTMLText 0x00883E10
//     +0x8E1 is_new      -> wnd+0x35C MKPW_ItemOfWeekNew
//     +0x8D4 price       -> wnd+0x360 MKPW_ItemOfWeekPrice
//     +0x20FC price_was  -> wnd+0x364 MKPW_ItemOfWeekWas
//                           (006DDA76 cmp [+0x20FC] vs [+0x8D4])
//
// SALES need nothing new either. The client shows the struck-through "was"
// label only when price < price_was (both in the tiles and here), so:
//     price      = what the player actually pays now
//     price_was  = the old figure to strike through; 0 or <= price = no sale
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.19  THE "BUY NOW" AD BANNER + ADD FUNDS (UI-XML)
// ---------------------------------------------------------------------------
// Banner: StaticAnimation MKPW_Marketing_Anim (EQUI_MarketplaceWnd.xml, the
// RoF "Buy Now" art, 395x160) draws animation A_MarketingBanner from texture
// mp_marketing.tga. It can be hidden by setting its <Size> to 0x0 (kept in
// place). (There is also MKPW_Marketing_Anim_CharSelect, 846x160,
// A_MarketingBanner_CharSelect -- only shown on the char-select marketplace.)
//
// Add Funds: MKPW_AddFundsBtn can be hidden by collapsing its anchor offsets to
// a 0x0 rect. It must NOT be deleted: WndNotification derefs it unguarded at
// 0x006E3750 / 0x006E379D (44.12).
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.18  PREVIEW REPLY WIRE FORMAT: serialized item + ONE TRAILING u8
//        (2026-08-04 -- why the render armed but the label never cleared)
// ---------------------------------------------------------------------------
// S->C 0x7F80 receive handler = static 0x004AD065 (shared with 0xB07F, forked
// on the opcode). It:
//   1. deserializes the item (0x005359A0), aborts if the stream is bad;
//   2. reads ONE MORE u8 from the stream -- stock EQEmu's
//      ENCODE(OP_ItemPreviewRequest) does NOT send it, so the client reads
//      UNINITIALISED STACK;
//   3. computes previewable = 1, cleared only if IDFile == "IT63" or "IT64"
//      (the generic placeholder models; skipped entirely when 0x00792280(item)
//      returns nonzero);
//   4. delivers to the PREVIEW window pinst [0x00D1FD4C] (NOT the marketplace
//      pinst 0x00D1FD08!) via 0x006DFA70(item, previewable||u8, u8):
//        - renders through 0x00557700(renderCtx=wnd+0x228, item, u8)

// ---------------------------------------------------------------------------
// 44.22  NO NEW BUTTONS -- and the SECOND buy button only navigates
//        (2026-08-05, static disassembly)
// ---------------------------------------------------------------------------
#define CMarketplaceWnd__NavigateToEntry_x          0x006DCA60  // (entry_id) -> select it
#define CMarketplaceWnd__FindRowByEntryId_x         0x006D4300  // (entry_id) -> row*
#define CMarketplaceWnd__ItemOfWeekBuyArm_x         0x006E2163  // wnd+0x368
#define CMarketplaceWnd__BuyArm_x                   0x006E2356  // wnd+0x4A8
//
// ASKED AND ANSWERED: can EQUI_MarketplaceWnd.xml add a working button? NO, and
// the reason generalises to every stock window. Children are bound to FIXED
// struct offsets by ScreenID at window init (44.8), and WndNotification
// (0x006E1F70) dispatches with nothing but pointer compares against those
// members:
//       006E2163  cmp ebx, [ebp+0x368]     ; MKPW_ItemOfWeekBuyNowBtn
//       006E2356  cmp ebx, [ebp+0x4A8]     ; MKPW_BuyBtn
// A ScreenID the exe has never heard of gets no member, so no compare can ever
// match it and the click falls through to the tail. A DUPLICATE ScreenID does
// not help either -- only the first child binds. XML has no action verb: no
// command, no packet, no script. Behaviour means a new `cmp` arm, means patching
// code. The two buy buttons are NOT copies of one another; they are two separate
// hardcoded members with two separate hardcoded arms.
//
// *** BUT MKPW_ItemOfWeekBuyNowBtn IS NOT A BUY BUTTON. *** Its whole arm:
//       006E216B  mov  eax,[ebp+0x264]     ; the Item-of-the-Week entry (44.21)
//       006E2171  test eax,eax / je done
//       006E2179  mov  eax,[eax+0x10]      ; its entry id
//       006E217F  call 0x006DCA60          ; NavigateToEntry
// It sends no packet and purchases nothing. It jumps the category tree and item
// list to the entry the SERVER nominated -- a "take me to that listing" button
// whose target is chosen entirely by the +0x10E2/+0x10E4 fields.
//
// NavigateToEntry picks the destination shelf like this:
//     row = FindRowByEntryId(id); if (!row) return
//     if (entry->+0x8E1 /*is_new*/ || entry->+0x20F8 /*is_featured*/)
//          -> select category[0]  ("New and Featured")
//     else -> walk the category array at wnd+0x284 for category[i]+0x0C ==
//             row+0x1C, i.e. THE ENTRY'S OWN CATEGORY
// So an entry meant to be navigated to must carry NEITHER flag, or it lands on
// the wrong shelf. This is the same pairing 0x006D8030 uses (44.13b).
//
// --- product kind (+0x10E8): 4 AND 6 ARE THE REAL-MONEY KINDS ---------------
#define CMarketplaceWnd__BuyPageChoice_x            0x006E2AAE  // kind 6 -> credit card
//
// CORRECTION. 44.13a records that CanBuy
// passes for kinds 4 and 6 -- true, but the reason matters: THOSE ARE THE
// STATION CASH KINDS. They skip the wallet check because no in-game wallet is
// involved. The page choice after a Buy click is decisive:
//     006E2AAE  cmp byte [wnd+0x240], 0
//     006E2AB9  je  006E2AEA                  ; -> MKPW_TaxInfoPage (zip)
//     006E2AC1  cmp dword [entry+0x10E8], 6
//     006E2AC8  jne 006E2AE1                  ; -> normal confirm page wnd+0x580
//     006E2ACA  SetPage(wnd+0x5B4)            ; *** THE CREDIT CARD PAGE ***
// Publishing an entry as kind 6 puts the player on "Credit Card Product /
// Product Price: $30.00 / Select credit card" -- the dead Uram HTTPS flow
// (44.1). Kind 4 diverts even earlier, at 0x006E2409 into 0x006D6310 after
// comparing the +0x10F0 / +0x14F0 strings, and never reaches a confirm page.
// Kind 3 is a quest-reward gate; string 0x009F1368 "You must claim one or more
// of your quest rewards before purchasing this item."
//
// *** THERE IS NO KIND THAT BOTH BYPASSES THE WALLET CHECK AND KEEPS THE
// IN-GAME CONFIRM PAGE. *** A listing priced in the wallet currency must be
// kind 0 and satisfy CanBuy. Kind 0 also keeps the quantity buttons, which
// kind 6 disables.
//
// Summary of the purchase gate:
//   * CanBuy (0x006D2DF0) gates Buy on `price <= wallet`, and the wallet is the
//     alt-currency balance.
//   * Kinds 4 and 6 skip that check because they are the Station Cash kinds.
//     Kind 6 sends the Buy click to the credit card page (0x006E2AC1), kind 4
//     diverts at 0x006E2409. Neither reaches the in-game confirm page.
//   * The price formatter (0x006DBEFB) has exactly three renderings -- "%d",
//     the real-money "%1.2f", and "N/A". None is free text, and no other code
//     path writes those labels.
//
// --- money type (+0x210C): 0 = coin, 1 = REAL MONEY, >= 2 = "N/A" ----------
// The price formatter at 0x006DBEFB, exactly:
//     money_type == 0 -> sprintf("%d",    price)          [0x009C3F74]
//     money_type == 1 -> sprintf("%1.2f", price * 0.01f)  [0x009CE934, mul
//                        0x009C4BE4 = 0.01] -- the $30.00 above, i.e. the
//                        REAL-MONEY format
//     else            -> the literal "N/A"                 [0x009D8F8C]
// Visibility follows: the confirm page's coin (wnd+0x594) is shown iff
// money_type == 0 (0x006E2779, sete), and the '$' money-type label (wnd+0x598,
// XML Text="$") iff money_type == 1 (0x006E27B4, cmp eax,1 / sete). Same split
// on the list row at 0x006E26B3.
// So money_type 1 is the real-money format. "N/A" lands in the detail pane AND
// the confirm page's Price Each / Total Price. There is no way to put arbitrary
// text in a price cell; the formatter is the only writer.
//
// --- money type (+0x210C) ---------------------------------------------------
// Corroborated at 0x006E26B3: the confirm page shows its coin icon (wnd+0x584)
// iff money_type == 0 (`test eax,eax / sete al` -> SetVisible). So >= 2 blanks
// the price cell AND drops the coin.
//
// The Item-of-the-Week panel is exempt: its render block (0x006DD8C1-0x006DDB1E)
// reads price +0x8D4 at 0x006DD9F9 and price_was +0x20FC at 0x006DDA76 with NO
// +0x210C test anywhere, so the panel always shows the figure. Its coin is a
// separate XML control (MKPW_ItemOfWeekPrice_Icon, wnd+0x36C) and can therefore
// be pointed at a different animation independently of the A_Station coin used
// by the list rows and the confirm page.
//

// --- the confirm page's three money_type-driven controls, exhaustively -------
// Enumerated by sweeping 0x006D0000-0x006E6000 for every reference to each child
// offset, rather than reading the first hit and inferring the rest.
//   wnd+0x584  MKPW_ConfirmPurchase_Label   SetVisible(money_type == 0)  006E26A9
//   wnd+0x598  MKPW_BuyPrice_MoneyTypeLabel SetVisible(money_type == 1)  006E27B4
//   wnd+0x5A4  (the total's twin)           SetVisible(money_type == 1)  006E2824
//
// MKPW_ConfirmPurchase_Label has EXACTLY TWO references in the client: the bind
// at 0x006E00A7 and that SetVisible. Nothing ever writes its text -- so its stock
// reading "Station Cash Product" comes from the XML <Text> and nowhere else, and
// retitling it is pure UI data.
//
// So with money_type >= 2 none of the three is shown and the page has no header.
// Two other stock "Station Cash" strings exist in the XML -- "Redeem Station Cash
// Card" and "SC Amt" -- both on the SC-card and funding-tier pages.

// ---------------------------------------------------------------------------
// 44.23  THE CATALOG IS REQUESTED ONLY ONCE PER SESSION
// ---------------------------------------------------------------------------
// The client sends OP_MarketplaceRequestItems on the FIRST window open of a
// client session only; every reopen sends Hello / RequestFunding / Ready and
// nothing else, because it still holds the catalog. A server answering only
// RequestItems therefore leaves every reopen with no wallet, which surfaces as
// "Current Funds ..." (the label keeps its XML placeholder) and as the zip-code
// page returning, since wnd+0x240 is the wallet's flag byte (44.17). Ready fires
// on every open and is the place to send it.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 44.24  NESTED CATEGORIES WORK TO ANY DEPTH -- the ancestor count walk
//        (2026-08-05)
// ---------------------------------------------------------------------------
#define MarketplaceCategory__AdjustCountUp_x       0x006E4080  // (node, bAdd) recursive
//
// 44.9 established that FindOrCreateCategory recurses on the parent path, so a
// deep path auto-creates every ancestor. The open question was whether those
// ancestors RENDER, since RebuildCategoryTree only emits a node whose item count
// (+0x30) is > 0 and only the leaf is ever assigned an item. They do:
//
//     006DD803  mov ecx,[edi+0x1C]      ; the LEAF's parent
//     006DD808  je  006DD811            ; no parent -> just bump self
//     006DD80A  push 1
//     006DD80C  call 006E4080           ; walk the ancestors
//     006DD811  inc dword [edi+0x30]    ; then bump the leaf
//
//   sub_6E4080(node, bAdd):
//       if (node->+0x1C) recurse(node->+0x1C, bAdd)   // to the root
//       bAdd ? node->+0x30++ : node->+0x30--
//
// So every ancestor of a populated leaf gets a non-zero count and is emitted.
// "Ornaments/Armor/Head" renders all three levels from one item.
//
// CONSEQUENCES:
//   * Declare LEAVES ONLY. Ancestors are created from the path; giving them
//     their own rows is redundant and risks a duplicate node.
//   * A branch with no items anywhere beneath it is INVISIBLE -- the count walk
//     is what makes ancestors visible, so nothing to count means nothing shows.
//   * The tree renders in first-seen order, i.e. catalog order, so the order
//     of the leaves also decides where the parents land. Keep a branch's
//     leaves contiguous.
//   * Never name a top-level category "Player Studio": 0x006DD7CA strncmps 13
//     chars and strips the "Player Studio/" prefix, re-homing those items.
// ---------------------------------------------------------------------------
