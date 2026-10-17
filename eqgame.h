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

//
// eqgame.exe offsets
//

#define __ClientDate                                               20130510u
#define __ExpectedVersionDate                                     "May 10 2013"
#define __ExpectedVersionTime                                     "23:30:08"
#define __ActualVersionDate_x                                      0x9DD250
#define __ActualVersionTime_x                                      0x9DD25C

#define __EncryptPad0_x                                            0xAE42D0

// EverQuestInfo
#define pinstEverQuestInfo_x                                       0xDDF618
#define instEQZoneInfo_x                                           0xDDF8F0

#define __ScreenX_x                                                0xDDF624
#define __ScreenY_x                                                0xDDF620
#define __ScreenXMax_x                                             0xDDF628
#define __ScreenYMax_x                                             0xDDF62C
#define __InChatMode_x                                             0xDDF66C
#define __RunWalkState_x                                           0xDDF670
#define DI8__Mouse_Copy_x                                          0xDDF684
#define __MouseLook_x                                              0xDDF702
#define __NetStatusToggle_x                                        0xDDF705
#define __pulAutoRun_x                                             0xDDF720
#define __Clicks_x                                                 0xDDF734
#define __RMouseHeldTime_x                                         0xDDF79C
#define __LMouseHeldTime_x                                         0xDDF7A0
#define __ZoneType_x                                               0xDDFAF8
#define __ShiftKeyDown_x                                           0xDDFD00
#define __RangeAttackReady_x                                       0xDE0864
#define __AttackOnAssist_x                                         0xDE09DC
#define __ShowNames_x                                              0xDE0A70
#define __PCNames_x                                                0xDE0BC8
#define __UseTellWindows_x                                         0xDE0D58
#define __pulForward_x                                             0xDE0D90
#define __pulBackward_x                                            0xDE0D94
#define __pulTurnRight_x                                           0xDE0D98
#define __pulTurnLeft_x                                            0xDE0D9C
#define __pulStrafeLeft_x                                          0xDE0DA0
#define __pulStrafeRight_x                                         0xDE0DA4
#define __LastTell_x                                               0xDE1558
#define __ServerName_x                                             0xE15E10
#define __DoAbilityList_x                                          0xE15E50
#define __SocialChangedList_x                                      0xE15E98
#define __Socials_x                                                0xE15F10
#define __Inviter_x                                                0xE63528
#define __Attack_x                                                 0xE635AA
#define __Autofire_x                                               0xE635AB
#define EverQuest__Cameras_x                                       0xDE0D64
#define EverQuestinfo__IsItemPending_x                             0xE635AC


// Misc Globals
#define __do_loot_x                                                0x4E7880
#define __gfMaxZoomCameraDistance_x                                0x9D0DE4
#define __gfMaxCameraDistance_x                                    0xA002C4
#define __CurrentSocial_x                                          0xAB6A28
#define __BindList_x                                               0xACBEE8
#define g_eqCommandStates_x                                        0xDCEF08
#define __CommandList_x                                            0xACD5A8
#define __ScreenMode_x                                             0xD1F3B8
#define __gWorld_x                                                 0xDD25AC
#define __gpbCommandEvent_x                                        0xDD2670
#define __ServerHost_x                                             0xDD003C
#define __Guilds_x                                                 0xDD5CF8
#define __MouseEventTime_x                                         0xE639CC
#define DI8__MouseState_x                                          0xE67884
#define __heqmain_x                                                0xE67CD0
#define DI8__Mouse_x                                               0xE67B50
#define __HWnd_x                                                   0xE678A0
#define __Mouse_x                                                  0xE67B54
#define DI8__Keyboard_x                                            0xE67B4C
#define __LoginName_x                                              0xE66478
#define __CurrentMapLabel_x                                        0xF73E00
#define __LabelCache_x                                             0xF74AB0
#define Teleport_Table_Size_x                                      0xDD25B4
#define Teleport_Table_x                                           0xDD02E0
#define __ExceptionFilter_x                                        0x8E3338

////
//Section 1: Vital Offsets
////
#define instCRaid_x                                                0xDD2690
#define pinstActiveBanker_x                                        0xDD2650
#define pinstActiveCorpse_x                                        0xDD2654
#define pinstActiveGMaster_x                                       0xDD2658
#define pinstActiveMerchant_x                                      0xDD264C
#define pinstAltAdvManager_x                                       0xD20358
#define pinstCEverQuest_x                                          0xE67CCC
#define pinstCamActor_x                                            0xD1FD8C
#define pinstCDBStr_x                                              0xD1F380
#define pinstCDisplay_x                                            0xDD2660
#define pinstControlledPlayer_x                                    0xDD2644
#define pinstCResolutionHandler_x                                  0x15D46CC
#define pinstCSidlManager_x                                        0x15D3D08
#define pinstCXWndManager_x                                        0x15D3D00
#define instDynamicZone_x                                          0xDDF4D0
#define instExpeditionLeader_x                                     0xDDF51A
#define instExpeditionName_x                                       0xDDF55A
#define pinstDZMember_x                                            0xDDF5E0
#define pinstDZTimerInfo_x                                         0xDDF5E4
#define pinstEqLogin_x                                             0xE65DC8
#define instTribute_x                                              0xABFCD0
#define pinstEQSoundManager_x                                      0xD20460
//#define pinstEQSpellStrings_x                                    0x0
#define pinstSGraphicsEngine_x                                     0x15D4698
#define pinstDeviceInputProxy_x                                    0xDDF684
#define pinstLocalPC_x                                             0xDD261C
#define pinstLocalPlayer_x                                         0xDD2630
#define pinstCMercenaryClientManager_x                             0xE63E90
//#define pinstMercenaryStats_x                                      0x0
#define pinstModelPlayer_x                                         0xDD265C
#define pinstRenderInterface_x                                     0x15D46A4
#define pinstSkillMgr_x                                            0xE64610
#define pinstSpawnManager_x                                        0xE641D0
#define pinstSpellManager_x                                        0xE646B0
#define pinstSpellSets_x                                           0xE5C8B4
#define pinstStringTable_x                                         0xDD25C4
#define pinstSwitchManager_x                                       0xDCFD00
#define pinstTarget_x                                              0xDD2648
#define pinstTaskMember_x                                          0xD1F314
#define pinstTrackTarget_x                                         0xDD263C
#define pinstTradeTarget_x                                         0xDD262C
#define instTributeActive_x                                        0xABFCF5
#define pinstViewActor_x                                           0xD1FD88
#define pinstWorldData_x                                           0xDD2600
#define pinstPlayerPath_x                                          0xE641F8
#define pinstTargetIndicator_x                                     0xE64748
#define EQObject_Top_x                                             0xDD2610

////
//Section 2:  UI Related Offsets
////
#define pinstCChatWindowManager_x                                  0xF71070
#define pinstCContainerMgr_x                                       0xD1FD84
#define pinstCContextMenuManager_x                                 0x15D391C
#define pinstCInvSlotMgr_x                                         0xD1FD80
#define pinstCItemDisplayManager_x                                 0xF73A28
#define pinstCPopupWndManager_x                                    0xF74338
#define pinstCSpellDisplayMgr_x                                    0xF749F0
#define pinstCTaskManager_x                                        0xB82C80
#define pinstEQSuiteTextureLoader_x                                0xB64CEC
#define pinstItemIconCache_x                                       0xF71398
//#define pinstLootFiltersManager_x                                  0x0

// Window Instances (Ghidra: DAT_00xxxxxx = FUN_xxxxxxxx() in FUN_004973F0)
// Confirmed by SIDL screen name string found inside each constructor function
// Note: pinstCMapViewWnd_x corrected - was 0xD1FD6C (ZoneGuideWnd), now 0xD1FC54 (CMapViewWnd ctor @ 0x6CFB70)
#define pinstAAWindow_x                                            0xD1FC20
#define pinstAchievementsWnd_x                                     0xD1FD0C
#define pinstAdventureLeaderboardWnd_x                             0xF704C0
#define pinstAdventureRequestWnd_x                                 0xF70500
#define pinstAdventureStatsWnd_x                                   0xF70540
#define pinstAdvancedDisplayOptionsWindow_x                        0xD1FCF0
#define pinstAggroMeterWnd_x                                       0xD1FD3C
#define pinstAlarmWnd_x                                            0xD1FC2C
#define pinstAlertHistoryWnd_x                                     0xD1FD74
#define pinstAlertStackWnd_x                                       0xD1FD64
#define pinstAlertWnd_x                                            0xD1FD68
#define pinstAltStorageWnd_x                                       0xF70690
#define pinstAudioTriggersWindow_x                                 0xB791E0
#define pinstAuraWindow_x                                          0xD1FC10
#define pinstBandolierWnd_x                                        0xD1FC18
#define pinstBarterMerchantWnd_x                                   0xF70C70
#define pinstBarterSearchWnd_x                                     0xF70CB0
#define pinstBarterWnd_x                                           0xF70CF0
#define pinstBazaarConfirmationWnd_x                               0xD1FD7C
#define pinstBazaarWnd_x                                           0xD1FCB0
#define pinstBigBankWnd_x                                          0xD1FC90
#define pinstBlockedBuffWnd_x                                      0xD1FD44
#define pinstBlockedPetBuffWnd_x                                   0xD1FD48
#define pinstBodyTintWnd_x                                         0xD1FD30
#define pinstBookWindow_x                                          0xD1FBF4
#define pinstBreathWindow_x                                        0xD1FC50
#define pinstCastingWindow_x                                       0xD1FC80
#define pinstCastSpellWnd_x                                        0xD1FC84
#define pinstClaimWnd_x                                            0xF710F0
#define pinstCMapViewWnd_x                                         0xD1FC54
#define pinstColorPickerWnd_x                                      0xD1FC64
#define pinstCombatAbilityWnd_x                                    0xD1FCA0
#define pinstCombatSkillSelectWnd_x                                0xD1FC08
#define pinstCompassWindow_x                                       0xD1FCFC
#define pinstDynamicZoneWnd_x                                      0xF712E0
#define pinstEditLabelWnd_x                                        0xD1FC5C
#define pinstEQMainWnd_x                                           0xF713C0
#define pinstExtendedTargetWnd_x                                   0xD1FD38
#define pinstFeedbackWindow_x                                      0xD1FCE4
#define pinstFellowshipWnd_x                                       0xF714C0
#define pinstFileSelectionWnd_x                                    0xD1FCF8
#define pinstFindLocationWnd_x                                     0xF71540
#define pinstFriendsWindow_x                                       0xD1FC0C
#define pinstGemsGameWnd_x                                         0xD1FD04
#define pinstGiveWnd_x                                             0xD1FCD0
#define pinstGroupMentorWindow_x                                   0xF71670
#define pinstGroupSearchFiltersWnd_x                               0xD1FC24
#define pinstGroupSearchWnd_x                                      0xF716F0
#define pinstGroupWindow_x                                         0xF71730
#define pinstGuildBankWnd_x                                        0xF71770
#define pinstGuildCreationWnd_x                                    0xF717B0
#define pinstGuildManagementWnd_x                                  0xF737F0
#define pinstHeritageSelectionWnd_x                                0xD1FBE8
#define pinstHelpWindow_x                                          0xD1FBF0
#define pinstIconSelectionWnd_x                                    0xD1FD60
#define pinstInspectWnd_x                                          0xD1FCDC
#define pinstInventoryWindow_x                                     0xD1FC8C
#define pinstItemExpTransferWnd_x                                  0xF73A6C
#define pinstJournalCatWnd_x                                       0xD1FD2C
#define pinstJournalNPCWnd_x                                       0xD1FD28
#define pinstLayoutCopyWindow_x                                    0xF73BA0
#define pinstLeadershipWindow_x                                    0xF73BE8
#define pinstLFGuildWnd_x                                          0xF73C28
#define pinstLoadskinWnd_x                                         0xD1FC28
#define pinstLootWnd_x                                             0xD1FC98
#define pinstMailCompositionWindow_x                               0xD1FC3C
#define pinstMailWindow_x                                          0xD1FC38
#define pinstMapToolbarWnd_x                                       0xD1FC58
#define pinstMerchantWnd_x                                         0xD1FCA4
#define pinstMIZoneSelectWnd_x                                     0xF73F78
#define pinstMusicPlayerWnd_x                                      0xD1FC30
#define pinstNameChangeMercWnd_x                                   0xD1FD5C
#define pinstNameChangePetWnd_x                                    0xD1FD58
#define pinstNameChangeWnd_x                                       0xD1FD10
#define pinstNewPointMerchantWnd_x                                 0xF740B8
#define pinstNoteWindow_x                                          0xD1FBEC
#define pinstObjectPreviewWnd_x                                    0xD1FD4C
#define pinstOptionsWindow_x                                       0xD1FC6C
#define pinstPetInfoWindow_x                                       0xD1FBF8
#define pinstPetitionQWindow_x                                     0xD1FD1C
#define pinstPlayerCustomizationWnd_x                              0xD1F9E4
#define pinstPlayerNotesWindow_x                                   0xD1FD00
#define pinstPlayerWindow_x                                        0xD1FC68
#define pinstPointMerchantWnd_x                                    0xF742F8
#define pinstPotionBeltWnd_x                                       0xD1FC1C
#define pinstProgressionSelectionWnd_x                             0xF743B8
#define pinstPvpLeaderboardWnd_x                                   0xF743F8
#define pinstPvPStatsWnd_x                                         0xF74438
#define pinstQuantityWnd_x                                         0xD1FC94
#define pinstRaceChangeWnd_x                                       0xD1FD14
#define pinstRaidOptionsWindow_x                                   0xD1FC4C
#define pinstRaidWindow_x                                          0xD1FC48
#define pinstRealEstateItemsWnd_x                                  0xD1FCB8
#define pinstRealEstateLayoutDetailsWnd_x                          0xD1FCCC
#define pinstRealEstateManageWnd_x                                 0xD1FCBC
#define pinstRealEstateNeighborhoodWnd_x                           0xD1FCC0
#define pinstRealEstatePlotSearchWnd_x                             0xD1FCC4
#define pinstRealEstatePurchaseWnd_x                               0xD1FCC8
#define pinstRespawnWnd_x                                          0xD1FC14
#define pinstRewardSelectionWnd_x                                  0xF746F8
#define pinstSelectorWindow_x                                      0xD1FCAC
#define pinstSendMoneyWnd_x                                        0xD1FCD4
#define pinstServerListWnd_x                                       0xD1FD34
#define pinstShortDurationBuffWindow_x                             0xD1FC70
#define pinstSkillsSelectWindow_x                                  0xD1FC04
#define pinstSkillsWindow_x                                        0xD1FC00
#define pinstSocialEditWnd_x                                       0xD1FCE0
#define pinstSoulmarkWnd_x                                         0xD1FD20
#define pinstSpellBookWnd_x                                        0xD1FC88
#define pinstTargetOfTargetWindow_x                                0xF75690
#define pinstTargetWindow_x                                        0xD1FC60
#define pinstTaskSelectWnd_x                                       0xF75710
#define pinstTaskTemplateSelectWnd_x                               0xF75750
#define pinstTaskWnd_x                                             0xF75790
#define pinstTextEntryWnd_x                                        0xD1FCF4
#define pinstTipWindow_x                                           0xF758C0
#define pinstTitleWnd_x                                            0xF75908
#define pinstTrackingWnd_x                                         0xD1FCD8
#define pinstTradeWnd_x                                            0xD1FCA8
#define pinstTradeskillWnd_x                                       0xF75998
#define pinstTrainWindow_x                                         0xD1FBFC
#define pinstTributeBenefitWnd_x                                   0xF75A58
#define pinstTributeMasterWnd_x                                    0xF75A98
#define pinstTributeTrophyWnd_x                                    0xF75AD8
#define pinstVideoModesWindow_x                                    0xD1FCEC
#define pinstVoiceMacroWnd_x                                       0xE64A60
#define pinstVoteResultsWnd_x                                      0xD1FD54
#define pinstVoteWnd_x                                             0xD1FD50
#define pinstZoneGuideWnd_x                                        0xD1FD6C
#define pinstZonePathWnd_x                                         0xD1FD70



////
// Section 3: Miscellaneous Offsets
////
#define __CastRay_x                                                0x4FD1D0
#define __CastRay2_x                                               0x4FCC20
#define __compress_block_x                                         0x7AEBD0
#define __ConvertItemTags_x                                        0x4ED110
#define __CleanItemTags_x                                          0x4ED580
#define __CreateCascadeMenuItems_x                                 0x494BD0
#define __decompress_block_x                                       0x7AEC80
#define __DoesFileExist_x                                          0x80BDD0
#define __EQGetTime_x                                              0x809810
#define __ExecuteCmd_x                                             0x4D7230
#define __FixHeading_x                                             0x8D1F50
#define __FlushDxKeyboard_x                                        0x5F9A70
#define __get_bearing_x                                            0x4EF250
#define __get_melee_range_x                                        0x4DDEB0
#define __GetAnimationCache_x                                      0x665430
#define __GetGaugeValueFromEQ_x                                    0x762410
#define __GetLabelFromEQ_x                                         0x763640
#define __GetXTargetType_x                                         0x8D3BE0
#define __HandleMouseWheel_x                                       0x5F9C90
#define __HeadingDiff_x                                            0x8D1FC0
#define __HelpPath_x                                               0xE63870
#define __InitMouse_x                                              0x5FC850
#define __ProcessGameEvents_x                                      0x53A6C0
#define __ProcessKeyboardEvents_x                                  0x5FDA30
#define __ProcessMouseEvents_x                                     0x539E60
#define __ProcessDeviceEvents_x                                    0xDDF684
#define __SaveColors_x                                             0x489FB0
#define __STMLToText_x                                             0x860620
#define __WndProc_x                                                0x5FCD80
#define CMemoryMappedFile__SetFile_x                               0x8FC1B0
#define DrawNetStatus_x                                            0x573620
#define Util__FastTime_x                                           0x808B50
#define __eq_delete_x                                              0x8DB146
#define __eq_new_x                                                 0x8DBB3B
#define __CopyLayout_x                                             0x55BE50
#define __ThrottleFrameRate_x                                      0x517EE0 // SleepWell

////
// Section 4: Function Offsets
////

// CAAwnd
#define CAAWnd__ShowAbility_x                                      0x60A7A0
#define CAAWnd__Update_x                                           0x609F90
#define CAAWnd__UpdateSelected_x                                   0x608650

// CAdvancedLootWnd
// #define CAdvancedLootWnd__DoAdvLootAction_x                        0x0
// #define CAdvancedLootWnd__DoSharedAdvLootAction_x                  0x0
// #define CAdvancedLootWnd__AddPlayerToList_x                        0x0
// #define CAdvancedLootWnd__UpdateMasterLooter_x                     0x0

// AltAdvManager
#define AltAdvManager__GetCalculatedTimer_x                        0x4A3840
#define AltAdvManager__IsAbilityReady_x                            0x4A3880
#define AltAdvManager__GetAAById_x                                 0x4A3C60
#define AltAdvManager__CanTrainAbility_x                           0x4A44C0
#define AltAdvManager__CanSeeAbility_x                             0x4A40F0

// CharacterZoneClient
#define CharacterZoneClient__BardCastBard_x                        0x432960
#define CharacterZoneClient__CalcAffectChange_x                    0x436670
#define CharacterZoneClient__CalcAffectChangeGeneric_x             0x434DF0
#define CharacterZoneClient__CanMedOnHorse_x                       0x4434B0
#define CharacterZoneClient__CanUseItem_x                          0x44C430
#define CharacterZoneClient__CanUseMemorizedSpellSlot_x            0x433BC0
#define CharacterZoneClient__CastSpell_x                           0x43A830
#define CharacterZoneClient__CharacterZoneClient_x                 0x45AB40
#define CharacterZoneClient__Cur_HP_x                              0x449E00
#define CharacterZoneClient__Cur_Mana_x                            0x4442E0
#define CharacterZoneClient__FindAffectSlot_x                      0x43D300
#define CharacterZoneClient__GetAdjustedSkill_x                    0x449FE0
#define CharacterZoneClient__GetBaseSkill_x                        0x449C80
#define CharacterZoneClient__GetCastingTimeModifier_x              0x4336D0
#define CharacterZoneClient__GetCursorItemCount_x                  0x451FD0
#define CharacterZoneClient__GetEnduranceRegen_x                   0x448EC0
#define CharacterZoneClient__GetFirstEffectSlot_x                  0x432AD0
#define CharacterZoneClient__GetFocusCastingTimeModifier_x         0x42C9A0
#define CharacterZoneClient__GetFocusRangeModifier_x               0x42CB80
#define CharacterZoneClient__GetFocusReuseMod_x                    0x42CEC0
#define CharacterZoneClient__GetHPRegen_x                          0x4527F0
#define CharacterZoneClient__GetItemCountInInventory_x             0x451ED0
#define CharacterZoneClient__GetItemCountWorn_x                    0x451DE0
#define CharacterZoneClient__GetLastEffectSlot_x                   0x438EE0
#define CharacterZoneClient__GetManaRegen_x                        0x452DF0
#define CharacterZoneClient__GetModCap_x                           0x57F550
#define CharacterZoneClient__GetOpenEffectSlot_x                   0x43A6D0
#define CharacterZoneClient__GetPCSpellAffect_x                    0x433FD0
#define CharacterZoneClient__HasSkill_x                            0x44A1B0
#define CharacterZoneClient__IsStackBlocked_x                      0x4341E0
#define CharacterZoneClient__MakeMeVisible_x                       0x4592B0
#define CharacterZoneClient__Max_Endurance_x                       0x582020
#define CharacterZoneClient__Max_HP_x                              0x443FA0
#define CharacterZoneClient__Max_Mana_x                            0x581E60
#define CharacterZoneClient__NotifyPCAffectChange_x                0x4591B0
#define CharacterZoneClient__RemovePCAffectex_x                    0x43A7A0
#define CharacterZoneClient__SpellDuration_x                       0x434420
#define CharacterZoneClient__TotalEffect_x                         0x438990
#define CharacterZoneClient__UseSkill_x                            0x45B2D0


// CBankWnd
#define CBankWnd__WndNotification_x                                0x6293C0

// CBarterWnd
#define CBarterWnd__WndNotification_x                              0x631A10

// CBarterSearchWnd
#define CBarterSearchWnd__WndNotification_x                        0x62D810
#define CBarterSearchWnd__UpdateInventoryList_x                    0x62C960

// CBazaarSearchWnd
#define CBazaarSearchWnd__HandleSearchResults_x                    0x635530

// CBuffWindow
#define CBuffWindow__WndNotification_x                             0x641810

// CButtonWnd
#define CButtonWnd__vftable_x                                      0xA1B41C   // RTTI confirmed

// CChatWindowManager
#define CChatWindowManager__GetRGBAFromIndex_x                     0x6508A0
#define CChatWindowManager__InitContextMenu_x                      0x6513C0
#define CChatWindowManager__FreeChatWindow_x                       0x650420
#define CChatWindowManager__SetLockedActiveChatWindow_x            0x650870
#define CChatWindowManager__CreateChatWindow_x                     0x6549B0

// ChatManagerClient
//#define ChatManagerClient__Instance_x                              0x0

// CChatWindow
#define CChatWindow__CChatWindow_x                                 0x656100
#define CChatWindow__Clear_x                                       0x655CB0
#define CChatWindow__WndNotification_x                             0x656640
#define CChatWindow__AddHistory_x                                  0x655AC0

// CContextMenu
#define CContextMenu__AddMenuItem_x                                0x85B7B0
#define CContextMenu__RemoveMenuItem_x                             0x85BAA0
#define CContextMenu__RemoveAllMenuItems_x                         0x85BAC0
#define CContextMenu__CheckMenuItem_x                              0x85BB40
#define CContextMenu__SetMenuItem_x                                0x85B9C0
#define CContextMenu__AddSeparator_x                               0x85B930

// CContextMenuManager
#define CContextMenuManager__AddMenu_x                             0x861A90
#define CContextMenuManager__RemoveMenu_x                          0x861DD0
#define CContextMenuManager__PopupMenu_x                           0x862220
#define CContextMenuManager__Flush_x                               0x861B00
#define CContextMenuManager__CreateDefaultMenu_x                   0x660480

// CChatService
#define CChatService__GetNumberOfFriends_x                         0x7EFA80
#define CChatService__GetFriendName_x                              0x7EFA90

// CComboWnd
#define CComboWnd__DeleteAll_x                                     0x86A960
#define CComboWnd__Draw_x                                          0x86AB40
#define CComboWnd__GetCurChoice_x                                  0x86A780
#define CComboWnd__GetListRect_x                                   0x86ADE0
#define CComboWnd__GetTextRect_x                                   0x86A9C0
#define CComboWnd__InsertChoice_x                                  0x86AE50
#define CComboWnd__SetColors_x                                     0x86A710
#define CComboWnd__SetChoice_x                                     0x86A740
#define CComboWnd__GetItemCount_x                                  0x86A770
#define CComboWnd__GetCurChoiceText_x                              0x86A790
#define CComboWnd__GetChoiceText_x                                 0x86A800

// CContainerWnd
#define CContainerWnd__HandleCombine_x                             0x65F7B0
#define CContainerWnd__SetContainer_x                              0x65E1F0
#define CContainerWnd__vftable_x                                   0x9E6758

// CDisplay
#define CDisplay__cameraType_x                                     0xD1FD9C
#define CDisplay__ZoneMainUI_x                                     0x4891A0
#define CDisplay__PreZoneMainUI_x                                  0x489190
#define CDisplay__CleanGameUI_x                                    0x4936E0
#define CDisplay__GetClickedActor_x                                0x48B6B0
#define CDisplay__GetUserDefinedColor_x                            0x489F70
#define CDisplay__InitCharSelectUI_x                               0x4967B0
#define CDisplay__ReloadUI_x                                       0x49D6B0
#define CDisplay__WriteTextHD2_x                                   0x48EB40
#define CDisplay__TrueDistance_x                                   0x48C2C0
#define CDisplay__SetViewActor_x                                   0x48F030
#define CDisplay__GetFloorHeight_x                                 0x48C5B0
#define CDisplay__SetRenderWindow_x                                0x494080
#define CDisplay__ToggleScreenshotMode_x                           0x48D030
#define CDisplay__RealRender_World_x                               0x49CA10

// CEditWnd
#define CEditBaseWnd__SetSel_x                                     0x88C0C0
#define CEditWnd__DrawCaret_x                                      0x87B230
#define CEditWnd__EnsureCaretVisible_x                             0x87D240
#define CEditWnd__GetCaretPt_x                                     0x87CCF0
#define CEditWnd__GetCharIndexPt_x                                 0x87C260
#define CEditWnd__GetDisplayString_x                               0x87B3E0
#define CEditWnd__GetHorzOffset_x                                  0x87B720
#define CEditWnd__GetLineForPrintableChar_x                        0x87BD20
#define CEditWnd__GetSelStartPt_x                                  0x87C520
#define CEditWnd__GetSTMLSafeText_x                                0x87B8C0
#define CEditWnd__PointFromPrintableChar_x                         0x87BE10
#define CEditWnd__ReplaceSelection_x                               0x87CD30
#define CEditWnd__SelectableCharFromPoint_x                        0x87BF90
#define CEditWnd__SetEditable_x                                    0x87B890
#define CEditWnd__SetWindowText_x                                  0x87DD00

// CEverQuest
#define CEverQuest__DoPercentConvert_x                             0x51B600
#define CEverQuest__ClickedPlayer_x                                0x5190A0
#define CEverQuest__CreateTargetIndicator_x                        0x519D00
#define CEverQuest__DeleteTargetIndicator_x                        0x519D80
#define CEverQuest__DoTellWindow_x                                 0x51DA00
#define CEverQuest__OutputTextToLog_x                              0x5130C0
#define CEverQuest__DropHeldItemOnGround_x                         0x526290
#define CEverQuest__dsp_chat_x                                     0x51F1A0
#define CEverQuest__trimName_x                                     0x5172D0
#define CEverQuest__Emote_x                                        0x51F400
#define CEverQuest__EnterZone_x                                    0x531F90
#define CEverQuest__GetBodyTypeDesc_x                              0x515660
#define CEverQuest__GetClassDesc_x                                 0x514DC0
#define CEverQuest__GetClassThreeLetterCode_x                      0x5153C0
#define CEverQuest__GetDeityDesc_x                                 0x515F70
#define CEverQuest__GetLangDesc_x                                  0x5159F0
#define CEverQuest__GetRaceDesc_x                                  0x515CE0
#define CEverQuest__InterpretCmd_x                                 0x51FCE0
#define CEverQuest__LeftClickedOnPlayer_x                          0x536310
#define CEverQuest__LMouseUp_x                                     0x5384D0
#define CEverQuest__RightClickedOnPlayer_x                         0x536D80
#define CEverQuest__RMouseUp_x                                     0x537D30
#define CEverQuest__SetGameState_x                                 0x519750
#define CEverQuest__UPCNotificationFlush_x                         0x520C70
#define CEverQuest__IssuePetCommand_x                              0x5209B0
#define CEverQuest__ReportSuccessfulHit_x                          0x52EE40

// CGaugeWnd
#define CGaugeWnd__CalcFillRect_x                                  0x66F7A0
#define CGaugeWnd__CalcLinesFillRect_x                             0x66F800
#define CGaugeWnd__Draw_x                                          0x66FC30

// CGuild
#define CGuild__FindMemberByName_x                                 0x423D70
#define CGuild__GetGuildName_x                                     0x425670
#define CGuild__GetGuildIndex_x                                    0x425860

// CHotButtonWnd
#define CHotButtonWnd__DoHotButton_x                               0x68C8B0

//CHotButton
#define CHotButton__SetButtonSize_x                                0x550120
#define CHotButton__SetCheck_x                                     0x54FBE0

// CInvSlotMgr
#define CInvSlotMgr__MoveItem_x                                    0x698D80
#define CInvSlotMgr__SelectSlot_x                                  0x697F00

// CInvSLot
#define CInvSlot__HandleRButtonUp_x                                0x697250
#define CInvSlot__SliderComplete_x                                 0x695270
#define CInvSlot__GetItemBase_x                                    0x694780
#define CInvSlot__UpdateItem_x                                     0x694D20
//#define CInvSlot__CharacterPreviewWindow_x                       0x69735C
// CInvSlotWnd
#define CInvSlotWnd__CInvSlotWnd_x                                 0x69AC10
#define CInvSlotWnd__HandleLButtonUp_x                             0x69A5D0

// CItemDisplayWnd
#define CItemDisplayWnd__SetItem_x                                 0x6AE100
#define CItemDisplayWnd__UpdateStrings_x                           0x69AE30
#define CItemDisplayWnd__InsertAugmentRequest_x                    0x6A8190
#define CItemDisplayWnd__RemoveAugmentRequest_x                    0x6A86B0
//#define CItemDisplayWnd__RequestConvertItem_x                      0x0

// CSpellDisplayWnd
#define CSpellDisplayWnd__SetSpell_x                               0x75F060
#define CSpellDisplayWnd__UpdateStrings_x                          0x75EAC0
#define CSpellDisplayWnd__vftable_x                                0x9FB100

// CWndDisplayManager
#define CWndDisplayManager__FindWindow_x                           0x796420

// CItemDisplayManager
#define CItemDisplayManager__CreateWindowInstance_x                0x6A1AE0

// CSpellDisplayManager
#define CSpellDisplayManager__ShowSpell_x                          0x75FD80

// CKeyRingWnd
//#define CKeyRingWnd__ExecuteRightClick_x                           0x0

// CLabel
#define CLabel__UpdateText_x                                       0x6B4D40

// CListWnd
#define CListWnd__CListWnd_x                                       0x857C00
#define CListWnd__dCListWnd_x                                      0x858C80
#define CListWnd__vftable_x                                        0xA1927C   // RTTI-verified
#define CListWnd__AddColumn_x                                      0x858C20
#define CListWnd__AddColumn1_x                                     0x858400
#define CListWnd__AddLine_x                                        0x857EB0
#define CListWnd__AddString_x                                      0x8580C0
#define CListWnd__CalculateFirstVisibleLine_x                      0x854F70
#define CListWnd__CalculateVSBRange_x                              0x8570B0
#define CListWnd__ClearSel_x                                       0x854270
#define CListWnd__ClearAllSel_x                                    0x854220
#define CListWnd__CloseAndUpdateEditWindow_x                       0x857A90
#define CListWnd__Compare_x                                        0x855A80
#define CListWnd__Draw_x                                           0x856D10
#define CListWnd__DrawColumnSeparators_x                           0x856BA0
#define CListWnd__DrawHeader_x                                     0x854530
#define CListWnd__DrawItem_x                                       0x8560D0
#define CListWnd__DrawLine_x                                       0x856840
#define CListWnd__DrawSeparator_x                                  0x856C40
#define CListWnd__EnableLine_x                                     0x854440
#define CListWnd__EnsureVisible_x                                  0x8550D0
#define CListWnd__ExtendSel_x                                      0x855FE0
#define CListWnd__GetColumnTooltip_x                               0x853C40
#define CListWnd__GetColumnMinWidth_x                              0x853D70
#define CListWnd__GetColumnWidth_x                                 0x853B60
#define CListWnd__GetCurSel_x                                      0x853430
#define CListWnd__GetItemAtPoint_x                                 0x855370
#define CListWnd__GetItemAtPoint1_x                                0x8553E0
#define CListWnd__GetItemData_x                                    0x853680
#define CListWnd__GetItemHeight_x                                  0x854BC0
#define CListWnd__GetItemIcon_x                                    0x853860
#define CListWnd__GetItemRect_x                                    0x8551B0
#define CListWnd__GetItemText_x                                    0x853710
#define CListWnd__GetSelList_x                                     0x8582F0
#define CListWnd__GetSeparatorRect_x                               0x8559A0
#define CListWnd__InsertLine_x                                     0x858290
#define CListWnd__RemoveLine_x                                     0x8582A0
#define CListWnd__SetColors_x                                      0x853490
#define CListWnd__SetColumnJustification_x                         0x853F10
#define CListWnd__SetColumnLabel_x                                 0x8544D0
#define CListWnd__SetColumnWidth_x                                 0x853BF0
#define CListWnd__SetCurSel_x                                      0x853470
#define CListWnd__SetItemColor_x                                   0x857770
#define CListWnd__SetItemData_x                                    0x8542D0
#define CListWnd__SetItemText_x                                    0x8575B0
#define CListWnd__ShiftColumnSeparator_x                           0x857440
#define CListWnd__Sort_x                                           0x858D80
#define CListWnd__ToggleSel_x                                      0x8541A0
#define CListWnd__SetColumnsSizable_x                              0x855F20
#define CListWnd__SetItemWnd_x                                     0x857630
#define CListWnd__GetItemWnd_x                                     0x8538B0
#define CListWnd__SetItemIcon_x                                    0x857540
#define CListWnd__CalculateCustomWindowPositions_x                 0x8571E0
#define CListWnd__SetVScrollPos_x                                  0x855C00

// CMapViewWnd
#define CMapViewWnd__CMapViewWnd_x                                 0x6CFB70

// MapViewMap
#define MapViewMap__MapViewMap_x                                   0x6C7E60
#define MapViewMap__dMapViewMap_x                                  0x6CC580
#define MapViewMap__vftable_x                                      0x9EE580
#define MapViewMap__Clear_x                                        0x6C7FD0
#define MapViewMap__SetZoom_x                                      0x6C7C40
#define MapViewMap__HandleLButtonDown_x                            0x6C9430
#define MapViewMap__GetWorldCoordinates_x                          0x6C88A0

// CMerchantWnd
#define CMerchantWnd__DisplayBuyOrSellPrice_x                      0x6EF750
#define CMerchantWnd__PurchasePageHandler__RequestGetItem_x        0x6F5BE0
#define CMerchantWnd__PurchasePageHandler__RequestPutItem_x        0x6F6650
#define CMerchantWnd__MerchantPageHandler__SelectBuySellSlot_x     0x6EEC30
#define CMerchantWnd__SelectBuySellSlot_x                          0x6F31A0
#define CMerchantWnd__PurchasePageHandler__UpdateList_x            0x6F0BB0

// CPacketScrambler
#define CPacketScrambler__ntoh_x                                   0x7D03B0
#define CPacketScrambler__hton_x                                   0x7D03C0

// CSidlManagerBase
#define CSidlManagerBase__CreateTabWnd_x                           0x86C160
#define CSidlManagerBase__CreatePageWnd_x                          0x86BDB0
#define CSidlManagerBase__FindButtonDrawTemplate_x                 0x86CF40
#define CSidlManagerBase__FindButtonDrawTemplate1_x                0x86CED0
#define CSidlManagerBase__FindAnimation1_x                         0x86E010
#define CSidlManagerBase__FindScreenPieceTemplate_x                0x870040
#define CSidlManagerBase__FindScreenPieceTemplate1_x               0x870120
#define CSidlManagerBase__CreateLabel_x                            0x7556F0
#define CSidlManagerBase__CreateXWndFromTemplate_x                 0x86E5E0
#define CSidlManagerBase__CreateXWndFromTemplate1_x                0x870400
#define CSidlManagerBase__CreateXWnd_x                             0x870440

// CSidlManager
#define CSidlManager__CreateHotButtonWnd_x                         0x755DE0
#define CSidlManager__CreateXWnd_x                                 0x755F20

// CSidlScreenWnd
#define CSidlScreenWnd__CalculateHSBRange_x                        0x61EE40
#define CSidlScreenWnd__CalculateVSBRange_x                        0x61ED60
#define CSidlScreenWnd__ConvertToRes_x                             0x88BFA0
#define CSidlScreenWnd__CreateChildrenFromSidl_x                   0x85DCF0
#define CSidlScreenWnd__CSidlScreenWnd1_x                          0x85F070
#define CSidlScreenWnd__CSidlScreenWnd2_x                          0x85F120
#define CSidlScreenWnd__dCSidlScreenWnd_x                          0x85E600
#define CSidlScreenWnd__DrawSidlPiece_x                            0x85D7D0
#define CSidlScreenWnd__EnableIniStorage_x                         0x85CF00
#define CSidlScreenWnd__GetChildItem_x                             0x85CFD0
#define CSidlScreenWnd__GetSidlPiece_x                             0x85D9C0
#define CSidlScreenWnd__HandleLButtonUp_x                          0x863A30
#define CSidlScreenWnd__Init1_x                                    0x85ED50
#define CSidlScreenWnd__LoadIniInfo_x                              0x85DDA0
#define CSidlScreenWnd__LoadIniListWnd_x                           0x85D0C0
#define CSidlScreenWnd__LoadSidlScreen_x                           0x85E850
#define CSidlScreenWnd__m_layoutCopy_x                             0x15D38E0
#define CSidlScreenWnd__StoreIniInfo_x                             0x85CA70
#define CSidlScreenWnd__StoreIniVis_x                              0x85CE90
#define CSidlScreenWnd__vftable_x                                  0xA1982C   // template vtable (see line ~720 for instance WndNotification)
#define CSidlScreenWnd__WndNotification_x                          0x85EAD0

// CSkillMgr   a.k.a. SkillManager
#define CSkillMgr__IsAvailable_x                                   0x5BAE40
#define CSkillMgr__GetSkillCap_x                                   0x5BAEC0
#define CSkillMgr__GetNameToken_x                                  0x5BAAB0
#define CSkillMgr__IsActivatedSkill_x                              0x5BABD0
#define CSkillMgr__IsCombatSkill_x                                 0x5BAB10
#define CSkillMgr__GetSkillTimerDuration_x                         0x5BAD40
#define CSkillMgr__GetSkillLastUsed_x                              0x5BACA0

// CSliderWnd
#define CSliderWnd__GetValue_x                                     0x895FE0
#define CSliderWnd__SetValue_x                                     0x8961B0
#define CSliderWnd__SetNumTicks_x                                  0x896930

// CSpellBookWnd
#define CSpellBookWnd__MemorizeSet_x                               0x75CAC0

// CStmlWnd
#define CStmlWnd__AppendSTML_x                                     0x886720
#define CStmlWnd__CalculateHSBRange_x                              0x87EE80
#define CStmlWnd__CalculateVSBRange_x                              0x87EF10
#define CStmlWnd__CanBreakAtCharacter_x                            0x87F090
#define CStmlWnd__FastForwardToEndOfTag_x                          0x87F970
#define CStmlWnd__ForceParseNow_x                                  0x887070
#define CStmlWnd__GetNextTagPiece_x                                0x87F870
#define CStmlWnd__GetVisibleText_x                                 0x8803C0
#define CStmlWnd__InitializeWindowVariables_x                      0x8825A0
#define CStmlWnd__MakeStmlColorTag_x                               0x87E0D0
#define CStmlWnd__MakeWndNotificationTag_x                         0x87E130
#define CStmlWnd__SetSTMLText_x                                    0x883E10
#define CStmlWnd__StripFirstSTMLLines_x                            0x8863C0
#define CStmlWnd__UpdateHistoryString_x                            0x881300

// CTabWnd
#define CTabWnd__Draw_x                                            0x88D870
#define CTabWnd__DrawCurrentPage_x                                 0x88D0E0
#define CTabWnd__DrawTab_x                                         0x88CEA0
#define CTabWnd__GetCurrentPage_x                                  0x88D4B0
#define CTabWnd__GetCurrentTabIndex_x                              0x88CB20
#define CTabWnd__GetPageFromTabIndex_x                             0x88CDF0
#define CTabWnd__GetPageInnerRect_x                                0x88CB90
#define CTabWnd__GetTabInnerRect_x                                 0x88CD90
#define CTabWnd__GetTabRect_x                                      0x88CC60
#define CTabWnd__InsertPage_x                                      0x88DB60
#define CTabWnd__RemovePage_x                                      0x88D650
#define CTabWnd__SetPage_x                                         0x88D4E0
#define CTabWnd__SetPageRect_x                                     0x88D7B0
#define CTabWnd__UpdatePage_x                                      0x88DB00

// CPageWnd
//#define CPageWnd__FlashTab_x                                       0x0
#define CPageWnd__SetTabText_x                                     0x89B9A0

// CTextOverlay
#define CTextOverlay__DisplayText_x                                0x41F840
#define CBroadcast__Get_x                                          0x41F3A0


// CTextureFont
#define CTextureFont__DrawWrappedText_x                            0x889B70
#define CTextureFont__DrawWrappedText1_x                           0x889950
#define CTextureFont__DrawWrappedText2_x                           0x889A50
#define CTextureFont__GetTextExtent_x                              0x8898D0
#define CTextureFont__GetHeight_x                                  0x889910

// CXMLDataManager
#define CXMLDataManager__GetXMLData_x                              0x8929A0

// CXMLSOMDocumentBase
#define CXMLSOMDocumentBase__XMLRead_x                             0x851540

// CXStr
#define CXStr__gFreeLists_x                                        0xB618A0
#define CXStr__gCXStrAccess_x                                      0x15D35E4

// CXWnd
#define CXWnd__BringToTop_x                                        0x862C50
#define CXWnd__ClrFocus_x                                          0x862820
#define CXWnd__Destroy_x                                           0x869760
#define CXWnd__DoAllDrawing_x                                      0x868A10
#define CXWnd__DrawChildren_x                                      0x868BA0
#define CXWnd__DrawColoredRect_x                                   0x863010
#define CXWnd__DrawTooltip_x                                       0x8632C0
#define CXWnd__DrawTooltipAtPoint_x                                0x8679B0
#define CXWnd__GetBorderFrame_x                                    0x863690
#define CXWnd__GetChildItem_x                                      0x868330
#define CXWnd__GetChildWndAt_x                                     0x866980
#define CXWnd__GetClientClipRect_x                                 0x8634D0
#define CXWnd__GetRelativeRect_x                                   0x8637C0
#define CXWnd__GetScreenClipRect_x                                 0x8680B0
#define CXWnd__GetScreenRect_x                                     0x8638D0
#define CXWnd__GetTooltipRect_x                                    0x863150
#define CXWnd__IsActive_x                                          0x8760A0
#define CXWnd__IsDescendantOf_x                                    0x863640
#define CXWnd__IsReallyVisible_x                                   0x866960
#define CXWnd__IsType_x                                            0x868E10
#define CXWnd__Minimize_x                                          0x8666D0
#define CXWnd__ProcessTransition_x                                 0x862C00
#define CXWnd__Refade_x                                            0x8629D0
#define CXWnd__Resize_x                                            0x863990
#define CXWnd__Right_x                                             0x867EB0
#define CXWnd__SetFocus_x                                          0x865250
#define CXWnd__SetFont_x                                           0x862870
#define CXWnd__SetKeyTooltip_x                                     0x863C00
#define CXWnd__SetMouseOver_x                                      0x8683F0
#define CXWnd__SetParent_x                                         0x869710
#define CXWnd__StartFade_x                                         0x862CA0
#define CXWnd__vftable_x                                           0xA19C74
#define CXWnd__CXWnd_x                                             0x869AC0
#define CXWnd__dCXWnd_x                                            0x869E50

// CScreenPieceTemplate
#define CScreenPieceTemplate__IsType_x                             0x88F1E0

// CXWndManager
#define CXWndManager__DrawCursor_x                                 0x8779C0
#define CXWndManager__DrawWindows_x                                0x877600
#define CXWndManager__GetKeyboardFlags_x                           0x875BD0
#define CXWndManager__HandleKeyboardMsg_x                          0x876530
#define CXWndManager__RemoveWnd_x                                  0x876230
#define CXWndManager__RemovePendingDeletionWnd_x                   0x876C80

// CDBStr
#define CDBStr__GetString_x                                        0x4866C0

// ProfileManager
#define ProfileManager__GetCurrentProfile_x                        0x7DB210

// CCharacterListWnd
#define CCharacterListWnd__SelectCharacter_x                       0x42FEC0
#define CCharacterListWnd__EnterWorld_x                            0x42FD20
#define CCharacterListWnd__Quit_x                                  0x42EA50
#define CCharacterListWnd__UpdateList_x                            0x430760

// ItemClient
#define ItemBase__CanGemFitInSlot_x                                0x7BCDB0
#define ItemBase__CreateItemTagString_x                            0x7BBC50
#define ItemBase__GetImageNum_x                                    0x7B2FB0
#define ItemBase__GetItemValue_x                                   0x7BA920
#define ItemBase__IsEmpty_x                                        0x7B22A0
//#define ItemBase__IsKeyRingItem_x                                  0x0
#define ItemBase__ValueSellMerchant_x                              0x7BCF80
#define ItemClient__CanDrop_x                                      0x554E50
#define ItemClient__CanGoInBag_x                                   0x554FD0
#define ItemClient__CreateItemClient_x                             0x5559A0
#define ItemClient__dItemClient_x                                  0x5552F0

// EQ_LoadingS
#define EQ_LoadingS__SetProgressBar_x                              0x4A59A0
#define EQ_LoadingS__Array_x                                       0xAC6330

// PcClient
#define PcBase__GetAlternateAbilityId_x                            0x7C4460
#define PcBase__GetCombatAbility_x                                 0x7C44F0
#define PcBase__GetCombatAbilityTimer_x                            0x7C45A0
#define PcBase__GetItemContainedRealEstateIds_x                    0x7D0160
#define PcBase__GetNonArchivedOwnedRealEstates_x                   0x7C95F0
#define PcClient__AlertInventoryChanged_x                          0x577C80
#define PcClient__GetConLevel_x                                    0x577CB0
#define PcClient__HasLoreItem_x                                    0x57AE30
#define PcZoneClient__GetItemRecastTimer_x                         0x580600
#define PcZoneClient__RemoveMyAffect_x                             0x57FA50

// EQGroundItemListManager
#define EQGroundItem__EQGroundItem_x                               0x4DBBD0  // was: EQItemList__EQItemList
#define EQGroundItemListManager__Add_x                             0x4DBB10  // was: EQItemList__add_item
#define EQGroundItemListManager__Delete_x                          0x4DBFD0  // was: EQItemList__delete_item
#define EQGroundItemListManager__Clear_x                           0x4DBED0  // was: EQItemList__FreeItemList
#define EQGroundItemListManager__Instance_x                        0x4DC1B0

// EQPlacedItemManager
#define EQPlacedItemManager__Add_x                                 0x506030  // was:  EQItemList__add_object

// CTribute
#define CTribute__GetActiveFavorCost_x                             0x485970

// PlayerClient
#define PlayerBase__CanSee_x                                       0x8D0520
#define PlayerBase__CanSee1_x                                      0x8D05E0
#define PlayerBase__GetVisibilityLineSegment_x                     0x8D0300
#define PlayerBase__HasProperty_x                                  0x8CFDC0
#define PlayerBase__IsTargetable_x                                 0x8D0090
#define PlayerClient__ChangeBoneStringSprite_x                     0x58BE90
#define PlayerClient__dPlayerClient_x                              0x590A60
#define PlayerClient__GetPcClient_x                                0x58C9A0
#define PlayerClient__PlayerClient_x                               0x590880
#define PlayerClient__SetNameSpriteState_x                         0x58E2D0
#define PlayerClient__SetNameSpriteTint_x                          0x58BF00
#define PlayerZoneClient__ChangeHeight_x                           0x5A4EC0
#define PlayerZoneClient__DoAttack_x                               0x5A5000
#define PlayerZoneClient__GetLevel_x                               0x59FC90
#define PlayerZoneClient__IsValidTeleport_x                        0x506A20

// PlayerManagerClient
#define PlayerManagerClient__GetSpawnByID_x                        0x5996E0
#define PlayerManagerClient__GetSpawnByName_x                      0x599B40
#define PlayerManagerClient__GetPlayerFromPartialName_x            0x599710
#define PlayerManagerClient__CreatePlayer_x                        0x599BD0
#define PlayerManagerBase__PrepForDestroyPlayer_x                  0x8CF790
#define PlayerManagerBase__DestroyAllPlayers_x                     0x8CF800

// KeyPressHandler
#define pinstKeypressHandler_x                                     0xE639B0
#define KeypressHandler__AttachAltKeyToEqCommand_x                 0x55AFA0
#define KeypressHandler__AttachKeyToEqCommand_x                    0x55AFE0
#define KeypressHandler__ClearCommandStateArray_x                  0x55AB80
#define KeypressHandler__HandleKeyDown_x                           0x5594E0
#define KeypressHandler__HandleKeyUp_x                             0x559800
#define KeypressHandler__SaveKeymapping_x                          0x55AC50

// PlayerPointManager
#define PlayerPointManager__GetAltCurrency_x                       0x7D5350

// StringTable
#define StringTable__getString_x                                   0x7D0660

// PcZoneClient
#define PcZoneClient__doCombatAbility_x                            0x5808C0
#define PcZoneClient__DestroyHeldItemOrMoney_x                     0x5862F0
#define PcZoneClient__GetPcSkillLimit_x                            0x584BE0
#define PcZoneClient__RemovePetEffect_x                            0x580CA0
#define PcZoneClient__HasAlternateAbility_x                        0x582670
#define PcZoneClient__CanEquipItem_x                               0x583550
#define PcZoneClient__GetItemByID_x                                0x585A10
#define PcZoneClient__RemoveBuffEffect_x                           0x57FCE0
#define PcZoneClient__BandolierSwap_x                              0x585AA0

// Doors
#define EQSwitch__UseSwitch_x                                      0x511D70

// IconCache
#define IconCache__GetIcon_x                                       0x665A60

// CContainerMgr
#define CContainerMgr__OpenContainer_x                             0x65CAE0
#define CContainerMgr__CloseContainer_x                            0x65D2F0
#define CContainerMgr__OpenExperimentContainer_x                   0x65CAB0

// CQuantityWnd
#define CQuantityWnd__Open_x                                       0x725B00

// CHelpWnd
#define CHelpWnd__SetFile_x                                        0x54E670

// CLootWnd
#define CLootWnd__LootAll_x                                        0x6BDBA0
#define CLootWnd__RequestLootSlot_x                                0x6BE3A0

// EQ_Spell
#define EQ_Spell__GetSpellLevelNeeded_x                            0x4AF700
#define EQ_Spell__SpellAffects_x                                   0x4AF770
#define EQ_Spell__SpellAffectBase_x                                0x4AF7A0
#define EQ_Spell__IsSPAStacking_x                                  0x4ACE40
#define EQ_Spell__IsSPAIgnoredByStacking_x                         0x4ACA70
//#define EQ_Spell__IsDegeneratingLevelMod_x                         0x0
#define __IsResEffectSpell_x                                       0x432590

// EQSpellStrings
//#define EQSpellStrings__GetString_x                                0x0

// CTargetWnd
#define CTargetWnd__GetBuffCaster_x                                0x769FD0
#define CTargetWnd__WndNotification_x                              0x769AC0
#define CTargetWnd__RefreshTargetBuffs_x                           0x76A1B0
#define CTargetWnd__HandleBuffRemoveRequest_x                      0x7693F0

// CTaskWnd
#define CTaskWnd__UpdateTaskTimers_x                               0x76EF90
#define CTaskWnd__ConfirmAbandonTask_x                             0x76FD50

// CTaskManager
#define CTaskManager__GetEntry_x                                   0x47FFD0
#define CTaskManager__HandleMessage_x                              0x482080
#define CTaskManager__GetTaskStatus_x                              0x481100
#define CTaskManager__GetElementDescription_x                      0x4800C0

// EqSoundManager
#define EqSoundManager__WavePlay_x                                 0x4A9140
#define EqSoundManager__PlayScriptMp3_x                            0x4AA7D0
#define EqSoundManager__SoundAssistPlay_x                          0x5BFBD0
#define EqSoundManager__WaveInstancePlay_x                         0x5BF9A0

// CCombatSkillsSelectWnd
#define CCombatSkillsSelectWnd__ShouldDisplayThisSkill_x           0x4772A0

// CTextureAnimation
#define CTextureAnimation__Draw_x                                  0x87A4A0
#define CTextureAnimation__SetCurCell_x                            0x87A860

// FreeTargetTracker
#define FreeTargetTracker__CastSpell_x                             0x54C010

// CharacterBase
#define CharacterBase__GetItemByGlobalIndex_x                      0x7E9AE0
#define CharacterBase__GetItemByGlobalIndex1_x                     0x7E9B40
#define CharacterBase__IsExpansionFlag_x                           0x4106C0

// CCastSpellWnd
#define CCastSpellWnd__ForgetMemorizedSpell_x                      0x647D50
#define CCastSpellWnd__IsBardSongPlaying_x                         0x646EE0
#define CCastSpellWnd__RefreshSpellGemButtons_x                    0x647EA0

// messages
#define msg_spell_worn_off_x                                       0x4B79F0
#define msg_new_text_x                                             0x4C0080
#define __msgTokenTextParam_x                                      0x4BDBE0
#define msgTokenText_x                                             0x4BDD00

// SpellManager
#define Spellmanager__LoadTextSpells_x                             0x5C1C30
#define SpellManager__GetSpellByGroupAndRank_x                     0x5C1AE0

// CCollisionInfoTargetVisibility
#define CCollisionInfoTargetVisibility__CCollisionInfoTargetVisibility_x 0x8D4570

// CLargeDialogWnd
#define CLargeDialogWnd__Open_x                                    0x6B4EE0

// CCursorAttachment
#define CCursorAttachment__AttachToCursor_x                        0x661820
#define CCursorAttachment__IsOkToActivate_x                        0x660F30
#define CCursorAttachment__RemoveAttachment_x                      0x660F30

// CEQSuiteTextureLoader
#define CEQSuiteTextureLoader__CreateTexture_x                     0x88AEE0
#define CEQSuiteTextureLoader__GetDefaultUIPath_x                  0x88A8A0
#define CEQSuiteTextureLoader__GetTexture_x                        0x88AA20

// CFindItemWnd
//#define CFindItemWnd__WndNotification_x                            0x0
//#define CFindItemWnd__Update_x                                     0x0
//#define CFindItemWnd__PickupSelectedItem_x                         0x0

// LootFiltersManager
//#define LootFiltersManager__AddItemLootFilter_x                    0x0
//#define LootFiltersManager__GetItemFilterData_x                    0x0
//#define LootFiltersManager__RemoveItemLootFilter_x                 0x0
//#define LootFiltersManager__SetItemLootFilter_x                    0x0

// COptionsWnd
#define COptionsWnd__FillChatFilterList_x                          0x700C80

// CResolutionHandler
#define CResolutionHandler__UpdateResolution_x                     0x8D9C50
#define CResolutionHandler__GetWindowedStyle_x                     0x5B9420

// CColorPickerWnd
#define CColorPickerWnd__Open_x                                    0x659AF0

// CGroupWnd
#define CGroupWnd__WndNotification_x                               0x67C620
#define CGroupWnd__UpdateDisplay_x                                 0x67B470

// ItemBase
#define ItemBase__IsLore_x                                         0x7B4B70
//#define ItemBase__IsLoreEquipped_x                                 0x0

#define MultipleItemMoveManager__ProcessMove_x                     0x572300

// EQPlacedItemManager
#define EQPlacedItemManager__Instance_x                            0x5061E0
#define EQPlacedItemManager__GetItemByGuid_x                       0x5060C0
#define EQPlacedItemManager__GetItemByRealEstateAndRealEstateItemIds_x 0x506120

// RealEstateManagerClient
#define RealEstateManagerClient__Instance_x                        0x5B3540

// RealEstate Window Constructors (Ghidra confirmed: each DAT = FUN() assignment in FUN_004973F0)
#define RealEstateItemsWnd__RealEstateItemsWnd_x                   0x733E00
#define RealEstateManageWnd__RealEstateManageWnd_x                 0x746620
#define RealEstateNeighborhoodWnd__RealEstateNeighborhoodWnd_x     0x749560
#define RealEstatePlotSearchWnd__RealEstatePlotSearchWnd_x         0x74B470
#define RealEstatePurchaseWnd__RealEstatePurchaseWnd_x             0x74CA70
#define RealEstateLayoutDetailsWnd__RealEstateLayoutDetailsWnd_x   0x5AC730

// Master function that constructs ALL RealEstate windows at startup
#define __CreateAllRealEstateWindows_x                             0x4973F0

// Unidentified windows found in same creation function (FUN_004973F0) - needs further analysis
//#define pinstUnknownWnd_D1FCA4_x                                   0xD1FCA4  // DAT_00d1fca4 = FUN_006f4e10()
//#define pinstUnknownWnd_D1FCD0_x                                   0xD1FCD0  // DAT_00d1fcd0 = FUN_00673730()

// FactionManagerClient
//#define FactionManagerClient__Instance_x                           0x0
//#define FactionManagerClient__HandleFactionMessage_x               0x0
//#define FactionManagerClient__GetFactionStanding_x                 0x0
//#define FactionManagerClient__GetMaxFaction_x                      0x0
//#define FactionManagerClient__GetMinFaction_x                      0x0

// AggroMeterManagerClient
#define AggroMeterManagerClient__Instance_x                        0x4110E0

// ClientSOIManager
#define ClientSOIManager__GetSingleton_x                           0x466CC0

// MercenaryAlternateAdvancementManagerClient
//#define MercenaryAlternateAdvancementManagerClient__Instance_x     0x0
//#define MercenaryAlternateAdvancementManagerClient__BuyAbility_x   0x0
//#define CAltAbilityData__GetMercCurrentRank_x                      0x0
//#define CAltAbilityData__GetMercMaxRank_x                          0x0

// CTargetManager
#define CTargetManager__Get_x                                      0x5C6380

// KeyCombo
#define KeyCombo__GetTextDescription_x                             0x88B690

// CUnSerializeBuffer
#define CUnSerializeBuffer__GetString_x                            0x4048D0

// ZoneGuideManagerClient
#define ZoneGuideManagerClient__Instance_x                         0x600B10

// AchievementManager
#define AchievementManager__Instance_x                             0x404060

// FreeToPlayClient
#define FreeToPlayClient__Instance_x                               0x7DBA30
#define FreeToPlayClient__RestrictionInfo_x                        0xA0AA28

// UdpConnection
#define UdpConnection__GetStats_x                                  0x8C2FD0
#define UdpConnection__OnRoutePacket_x                             0x8C5070
#define UdpConnection__Send_x                                      0x8C51F0

// WorldAuthenticationHandler
#define WorldAuthenticationHandler__OnRoutePacket_x                0x5629C0

// ============================================================================
// Spell Scribing (stock client flow). All addresses are file VAs (base 0x400000).
// ============================================================================

// CSpellBookWnd::StartSpellScribe(int bookSlot)
//   __thiscall, ecx = CSpellBookWnd*,  arg1 = destination book slot (0-719)
//   Reads CURSOR item (slot 0x21 = 33) internally - scroll MUST be on cursor.
//   Checks item type == 0x14 (scroll), runs class check, checks if spell
//   already in book via GetBookSlot, then triggers native scribe animation.
//   FinishScribing fires when timer completes and writes spell to SpellBook[].
//#define CSpellBookWnd__StartSpellScribe_x                       0x75DDF0

// EQ_Character::GetBookSlot(int spellID)
//   __thiscall, ecx = CharacterZoneClient + 0x2DC8
//   arg1 = spellID to search for in the spell book
//   Loops all 720 slots (0x2D0), reads SpellBook[] at struct offset 0x2520.
//   Returns slot index if spell found, otherwise runs all 720 iterations.
//#define EQ_Character__GetBookSlot_x                             0x449CD0

// CharacterZoneClient__CanUseItem_x (0x44C430) already defined above ^
// This is the class check called by StartSpellScribe at 0x75DF6A.
// Returns al=1 if character class can use the scroll, al=0 if blocked.

// ============================================================================
// StartSpellScribe internal body reference (file VAs):
//   0x75DDF0  entry point (SEH prologue: push -1, push handler, sub esp,0x218)
//   0x75DE12  mov ebp,ecx         -- save CSpellBookWnd* (this)
//   0x75DE28  cmp [ebp+0x234],-1  -- already scribing guard
//   0x75DE30  cmp [ebp+0x240],-1  -- second state guard
//   0x75DE3E  push 0x21           -- cursor slot (HARD-CODED -- scroll must be here)
//   0x75DE4E  call 0x42DEC0       -- GetItemObjFromSlot(0x21,&out) -> PCONTENTS
//   0x75DF47  call 0x43AFFA0      -- GetItemType(pItem) -> 0x14 for scrolls
//   0x75DF4C  cmp eax,0x14        -- scroll type check
//   0x75DF4F  jne <exit>          -- bail if not a scroll
//   0x75DF64  add ecx,0x2DC8      -- CharacterZoneClient from CharInfo global
//   0x75DF6A  call 0x44C430       -- CanUseItem class/level check
//   0x75DF71  je <exit>           -- skip if class cannot use
//   0x75DF7D  push 4 / call 0x43AFFF0 -- GetScrollSpellID(pItem,4) -> spellID
//   0x75DF88  push eax            -- push spellID arg
//   0x75DF89  add ecx,0x2DC8      -- ecx for GetBookSlot this
//   0x75DF8F  call 0x449CD0       -- GetBookSlot(spellID)
//   0x75DF94  return site from GetBookSlot
//
// Internal helpers (file VAs):
//   0x42DEC0  GetItemObjFromSlot(slotID, &outPtr)
//   0x43AFFA0 GetItemType(pContents) -> 0x14 = scroll
//   0x43AFFF0 GetScrollSpellID(pContents, 4) -> spellID
// ============================================================================

// ============================================================
// Window Constructor Addresses
// Source: SIDL screen name string found inside each constructor (Ghidra)
// Pattern: ClassName__ClassName_x  (constructor function)
// Note: SIDL name used as class name - may have C prefix in actual EQLib (e.g. CLootWnd vs LootWnd)
// ============================================================
#define AAWindow__AAWindow_x                                       0x6093E0
#define AchievementsWnd__AchievementsWnd_x                         0x60F9D0
#define AdventureLeaderboardWnd__AdventureLeaderboardWnd_x         0x616EC0
#define AdventureRequestWnd__AdventureRequestWnd_x                 0x617F10
#define AdventureStatsWnd__AdventureStatsWnd_x                     0x619DA0
#define AdvancedDisplayOptionsWindow__AdvancedDisplayOptionsWindow_x 0x6167A0
#define AggroMeterWnd__AggroMeterWnd_x                             0x4117A0
#define AlarmWnd__AlarmWnd_x                                       0x61A300
#define AlertHistoryWnd__AlertHistoryWnd_x                         0x61A9E0
#define AlertStackWnd__AlertStackWnd_x                             0x61B690
#define AlertWnd__AlertWnd_x                                       0x61C030
#define AltStorageWnd__AltStorageWnd_x                             0x61D5E0
#define AudioTriggersWindow__AudioTriggersWindow_x                 0x41C0E0
#define AuraWindow__AuraWindow_x                                   0x620CD0
#define BandolierWnd__BandolierWnd_x                               0x626E70
#define BarterMerchantWnd__BarterMerchantWnd_x                     0x629860
#define BarterSearchWnd__BarterSearchWnd_x                         0x62C000
#define BarterWnd__BarterWnd_x                                     0x62F9F0
#define BazaarConfirmationWnd__BazaarConfirmationWnd_x             0x41CD20
#define BazaarWnd__BazaarWnd_x                                     0x63A8F0
#define BigBankWnd__BigBankWnd_x                                   0x627C60
#define BlockedBuffWnd__BlockedBuffWnd_x                           0x63DC90
#define BlockedPetBuffWnd__BlockedPetBuffWnd_x                     0x41EAC0
#define BodyTintWnd__BodyTintWnd_x                                 0x63E160
#define BookWindow__BookWindow_x                                   0x63FDD0
#define BreathWindow__BreathWindow_x                               0x640E40
#define CastingWindow__CastingWindow_x                             0x646270
#define CastSpellWnd__CastSpellWnd_x                               0x648310
#define ClaimWnd__ClaimWnd_x                                       0x6588D0
#define ColorPickerWnd__ColorPickerWnd_x                           0x6591E0
#define CombatAbilityWnd__CombatAbilityWnd_x                       0x65B3C0
#define CombatSkillSelectWnd__CombatSkillSelectWnd_x               0x4771D0
#define CompassWindow__CompassWindow_x                             0x65BF20
#define DynamicZoneWnd__DynamicZoneWnd_x                           0x663810
#define EditLabelWnd__EditLabelWnd_x                               0x664C70
#define EQMainWnd__EQMainWnd_x                                     0x665B50
#define ExtendedTargetWnd__ExtendedTargetWnd_x                     0x54B800
#define FeedbackWindow__FeedbackWindow_x                           0x667600
#define FellowshipWnd__FellowshipWnd_x                             0x6689F0
#define FileSelectionWnd__FileSelectionWnd_x                       0x66A890
#define FindLocationWnd__FindLocationWnd_x                         0x66C560
#define FriendsWindow__FriendsWindow_x                             0x66DFF0
#define GemsGameWnd__GemsGameWnd_x                                 0x672BD0
#define GiveWnd__GiveWnd_x                                         0x673730
#define GroupMentorWindow__GroupMentorWindow_x                     0x6746E0
#define GroupSearchFiltersWnd__GroupSearchFiltersWnd_x             0x675E60
#define GroupSearchWnd__GroupSearchWnd_x                           0x679620
#define GroupWindow__GroupWindow_x                                 0x67BC00
#define GuildBankWnd__GuildBankWnd_x                               0x67CE30
#define GuildCreationWnd__GuildCreationWnd_x                       0x67F8B0
#define GuildManagementWnd__GuildManagementWnd_x                   0x688210
#define HeritageSelectionWnd__HeritageSelectionWnd_x               0x689B80
#define HelpWindow__HelpWindow_x                                   0x54E8A0
#define IconSelectionWnd__IconSelectionWnd_x                       0x68EA30
#define InspectWnd__InspectWnd_x                                   0x68F840
#define InventoryWindow__InventoryWindow_x                         0x6909C0
#define ItemExpTransferWnd__ItemExpTransferWnd_x                   0x6AF430
#define JournalCatWnd__JournalCatWnd_x                             0x6B0F80
#define JournalNPCWnd__JournalNPCWnd_x                             0x6B4780
#define LayoutCopyWindow__LayoutCopyWindow_x                       0x6B5EC0
#define LeadershipWindow__LeadershipWindow_x                       0x6B7060
#define LFGuildWnd__LFGuildWnd_x                                   0x6BBD10
#define LoadskinWnd__LoadskinWnd_x                                 0x6BC050
#define LootWnd__LootWnd_x                                         0x6BD780
#define MailCompositionWindow__MailCompositionWindow_x             0x6C0D50
#define MailWindow__MailWindow_x                                   0x6C2C50
#define MapToolbarWnd__MapToolbarWnd_x                             0x6C7410
#define MerchantWnd__MerchantWnd_x                                 0x6F4E10
#define MIZoneSelectWnd__MIZoneSelectWnd_x                         0x6F7A00
#define MusicPlayerWnd__MusicPlayerWnd_x                           0x6F82C0
#define NameChangeMercWnd__NameChangeMercWnd_x                     0x6F9AB0
#define NameChangePetWnd__NameChangePetWnd_x                       0x6FA630
#define NameChangeWnd__NameChangeWnd_x                             0x6FB1B0
#define NewPointMerchantWnd__NewPointMerchantWnd_x                 0x6FBFC0
#define NoteWindow__NoteWindow_x                                   0x6FF2F0
#define ObjectPreviewWnd__ObjectPreviewWnd_x                       0x7005A0
#define OptionsWindow__OptionsWindow_x                             0x70AFF0
#define PetInfoWindow__PetInfoWindow_x                             0x711280
#define PetitionQWindow__PetitionQWindow_x                         0x711A80
#define PlayerCustomizationWnd__PlayerCustomizationWnd_x           0x716B80
#define PlayerNotesWindow__PlayerNotesWindow_x                     0x717960
#define PlayerWindow__PlayerWindow_x                               0x719690
#define PointMerchantWnd__PointMerchantWnd_x                       0x71A2C0
#define PotionBeltWnd__PotionBeltWnd_x                             0x71F910
#define ProgressionSelectionWnd__ProgressionSelectionWnd_x         0x721870
#define PvpLeaderboardWnd__PvpLeaderboardWnd_x                     0x7227A0
#define PvPStatsWnd__PvPStatsWnd_x                                 0x723E20
#define QuantityWnd__QuantityWnd_x                                 0x7255A0
#define RaceChangeWnd__RaceChangeWnd_x                             0x7276F0
#define RaidOptionsWindow__RaidOptionsWindow_x                     0x728120
#define RaidWindow__RaidWindow_x                                   0x72ADF0
#define RespawnWnd__RespawnWnd_x                                   0x74E560
#define RewardSelectionWnd__RewardSelectionWnd_x                   0x74F5B0
#define SelectorWindow__SelectorWindow_x                           0x7516D0
#define SendMoneyWnd__SendMoneyWnd_x                               0x752A50
#define ServerListWnd__ServerListWnd_x                             0x7545D0
#define ShortDurationBuffWindow__ShortDurationBuffWindow_x         0x642F10
#define SkillsSelectWindow__SkillsSelectWindow_x                   0x756660
#define SkillsWindow__SkillsWindow_x                               0x757070
#define SocialEditWnd__SocialEditWnd_x                             0x7588A0
#define SoulmarkWnd__SoulmarkWnd_x                                 0x759C40
#define SpellBookWnd__SpellBookWnd_x                               0x75D680
#define TargetOfTargetWindow__TargetOfTargetWindow_x               0x768C50
#define TargetWindow__TargetWindow_x                               0x76A590
#define TaskSelectWnd__TaskSelectWnd_x                             0x76C1C0
#define TaskTemplateSelectWnd__TaskTemplateSelectWnd_x             0x76E040
#define TaskWnd__TaskWnd_x                                         0x770940
#define TextEntryWnd__TextEntryWnd_x                               0x774D00
#define TipWindow__TipWindow_x                                     0x776AB0
#define TitleWnd__TitleWnd_x                                       0x7775A0
#define TrackingWnd__TrackingWnd_x                                 0x779350
#define TradeWnd__TradeWnd_x                                       0x77FF50
#define TradeskillWnd__TradeskillWnd_x                             0x77CE20
#define TrainWindow__TrainWindow_x                                 0x780D20
#define TributeBenefitWnd__TributeBenefitWnd_x                     0x783A00
#define TributeMasterWnd__TributeMasterWnd_x                       0x787140
#define TributeTrophyWnd__TributeTrophyWnd_x                       0x7881D0
#define VideoModesWindow__VideoModesWindow_x                       0x78A6C0
#define VoiceMacroWnd__VoiceMacroWnd_x                             0x5F19B0
#define VoteResultsWnd__VoteResultsWnd_x                           0x793880
#define VoteWnd__VoteWnd_x                                         0x795B30
#define ZoneGuideWnd__ZoneGuideWnd_x                               0x6030E0
#define ZonePathWnd__ZonePathWnd_x                                 0x6073C0


/////
// Section 5: RoF2 Client Opcodes
// Only non-zero (known) opcode values included.
// These are the wire-protocol opcode numbers for RoF2 client.
/////

// --- Adventure packets: ---
#define OP_AdventureData                                     0x7171
#define OP_AdventureDetails                                  0x5648
#define OP_AdventureFinish                                   0x400f
#define OP_AdventureInfo                                     0x4c54
#define OP_AdventureInfoRequest                              0x3cb0
#define OP_AdventureLeaderboardReply                         0x7f79
#define OP_AdventureLeaderboardRequest                       0x7093
#define OP_AdventureMerchantPurchase                         0x5b72
#define OP_AdventureMerchantRequest                          0x6922
#define OP_AdventureMerchantResponse                         0x3e47
#define OP_AdventureMerchantSell                             0x2f9b
#define OP_AdventurePointsUpdate                             0x65c3
#define OP_AdventureRequest                                  0x2c6c
#define OP_AdventureStatsReply                               0x2370
#define OP_AdventureStatsRequest                             0x5a62
#define OP_AdventureUpdate                                   0x1b01
#define OP_LeaveAdventure                                    0x5d18

// --- bazaar trader stuff: ---
#define OP_Barter                                            0x243a
#define OP_BazaarSearch                                      0x39d6
#define OP_BecomeTrader                                      0x61b3
#define OP_BuyerItems                                        0x1a6a
#define OP_Trader                                            0x4ef5
#define OP_TraderBulkSend                                    0x6a96
#define OP_TraderDelItem                                     0x5829
#define OP_TraderShop                                        0x31df

// --- Button-push commands ---
#define OP_Bind_Wound                                        0x0386
#define OP_CombatAbility                                     0x3eba
#define OP_Disarm                                            0x5ec8
#define OP_DisarmTraps                                       0x78bf
#define OP_FeignDeath                                        0x52fa
#define OP_Fishing                                           0x1e2a
#define OP_InstillDoubt                                      0x640e
#define OP_LDoNInspect                                       0x0438
#define OP_LDoNOpen                                          0x3d5c
#define OP_LDoNPickLock                                      0x9be3
#define OP_Mend                                              0x0ecf
#define OP_PickPocket                                        0x39e8
#define OP_SenseTraps                                        0x02af
#define OP_Sneak                                             0x5d55
#define OP_Taunt                                             0x2703

// --- Character Select Related: ---
#define OP_ApproveName                                       0x56a2
#define OP_CharacterCreate                                   0x6bbf
#define OP_CharacterCreateRequest                            0x6773
#define OP_DeleteCharacter                                   0x1808
#define OP_FloatListThing                                    0x46c6
#define OP_MOTD                                              0x0c22
#define OP_RandomNameGenerator                               0x5954
#define OP_SendMaxCharacters                                 0x5475
#define OP_SendMembership                                    0x7acc
#define OP_SendMembershipDetails                             0x057b
#define OP_SetChatServer                                     0x1bc5
#define OP_SetChatServer2                                    0x7eec
#define OP_WorldComplete                                     0x4493
#define OP_WorldUnknown001                                   0x2301
#define OP_ZoneServerInfo                                    0x4c44

// --- discovered opcodes not yet used: ---
#define OP_PlayMP3                                           0x5770

// --- Ex. Before Right Click Effect happens from items ---
#define OP_ItemAdvancedLoreText                              0x023b
#define OP_ItemVerifyReply                                   0x097b
#define OP_ItemVerifyRequest                                 0x189c

// --- Expeditions ---
#define OP_DzAddPlayer                                       0x4701
#define OP_DzChooseZone                                      0x0b7d
#define OP_DzChooseZoneReply                                 0x4de1
#define OP_DzCompass                                         0x3e0e
#define OP_DzExpeditionEndsWarning                           0x5189
#define OP_DzExpeditionInfo                                  0x9119
#define OP_DzExpeditionInvite                                0x7f4b
#define OP_DzExpeditionInviteResponse                        0x1950
#define OP_DzExpeditionLockoutTimers                         0x205f
#define OP_DzListTimers                                      0x7b68
#define OP_DzMakeLeader                                      0x543d
#define OP_DzMemberList                                      0x5ae4
#define OP_DzMemberListName                                  0x32f0
#define OP_DzMemberListStatus                                0x12f5
#define OP_DzPlayerList                                      0x14c6
#define OP_DzQuit                                            0xb2e3
#define OP_DzRemovePlayer                                    0x1abc
#define OP_DzSetLeaderName                                   0x3de9
#define OP_DzSwapPlayer                                      0x405b

// --- GM/Guide Opcodes ---
#define OP_GMApproval                                        0x6db5
#define OP_GMBecomeNPC                                       0x3ae1
#define OP_GMDelCorpse                                       0x607e
#define OP_GMEmoteWorld                                      0x458e
#define OP_GMEmoteZone                                       0x1cfd
#define OP_GMFind                                            0x4a8f
#define OP_GMGoto                                            0x7d8e
#define OP_GMHideMe                                          0x2fab
#define OP_GMKick                                            0x26a7
#define OP_GMKill                                            0x51d3
#define OP_GMLastName                                        0x46ce
#define OP_GMNameChange                                      0x035f
#define OP_GMSearchCorpse                                    0x357c
#define OP_GMServers                                         0x08c1
#define OP_GMSummon                                          0x486f
#define OP_GMToggle                                          0x2097
#define OP_GMZoneRequest                                     0x62ac
#define OP_GMZoneRequest2                                    0x7e1a

// --- Group Opcodes ---
#define OP_CancelInvite                                      0x2a50
#define OP_DoGroupLeadershipAbility                          0x6eae
#define OP_GroupAcknowledge                                  0x7323
#define OP_GroupDelete                                       0x0f6c
#define OP_GroupDisband                                      0x4c10
#define OP_GroupDisbandOther                                 0x74da
#define OP_GroupDisbandYou                                   0x1ae5
#define OP_GroupFollow                                       0x1649
#define OP_GroupFollow2                                      0x2060
#define OP_GroupInvite                                       0x6110
#define OP_GroupInvite2                                      0x32c2
#define OP_GroupLeaderChange                                 0x21b4
#define OP_GroupLeadershipAAUpdate                           0x02cf
#define OP_GroupMakeLeader                                   0x4229
#define OP_GroupMentor                                       0x3342
#define OP_GroupRoles                                        0x70e2
#define OP_GroupUpdate                                       0x3abb
#define OP_GroupUpdateB                                      0x6194
#define OP_InspectBuffs                                      0x486c

// --- Guild Opcodes ---
#define OP_GetGuildMOTD                                      0x36e0
#define OP_GetGuildMOTDReply                                 0x4f1f
#define OP_GuildBank                                         0x5134
#define OP_GuildBankItemList                                 0x7850
#define OP_GuildCreate                                       0x76d9
#define OP_GuildDelete                                       0x3708
#define OP_GuildDeleteGuild                                  0x6dab
#define OP_GuildDemote                                       0x2d4e
#define OP_GuildInvite                                       0x7099
#define OP_GuildInviteAccept                                 0x7053
#define OP_GuildLeader                                       0x7e09
#define OP_GuildManageBanker                                 0x3f35
#define OP_GuildMemberAdd                                    0x2925
#define OP_GuildMemberDelete                                 0x3141
#define OP_GuildMemberDetails                                0x69b9
#define OP_GuildMemberLevel                                  0x1bd3
#define OP_GuildMemberPublicNote                             0x01f9
#define OP_GuildMemberRankAltBanker                          0x0b9c
#define OP_GuildMemberRename                                 0x3b26
#define OP_GuildMemberUpdate                                 0x69b9
#define OP_GuildOpenGuildWindow                              0x0276
#define OP_GuildPeace                                        0x67e3
#define OP_GuildPromote                                      0x6a98
#define OP_GuildPublicNote                                   0x5053
#define OP_GuildRemove                                       0x1444
#define OP_GuildRenameGuild                                  0x61db
#define OP_GuildStatus                                       0x7326
#define OP_GuildUpdate                                       0x2958
#define OP_GuildWar                                          0x1ffb
#define OP_LFGuild                                           0x3165
#define OP_SetGuildMOTD                                      0x0b0b
#define OP_SetGuildRank                                      0x0b9c

// --- LFG/LFP Opcodes ---
#define OP_LFGCommand                                        0x6060
#define OP_LFGGetMatchesRequest                              0x0340
#define OP_LFGGetMatchesResponse                             0x5048
#define OP_LFPCommand                                        0x49a9
#define OP_LFPGetMatchesRequest                              0x4d7d
#define OP_LFPGetMatchesResponse                             0x22c6

// --- Looting ---
#define OP_EndLootRequest                                    0x30f7
#define OP_LootComplete                                      0x55c4
#define OP_LootItem                                          0x4dc9
#define OP_LootRequest                                       0x0adf

// --- Mercenary Opcodes ---
#define OP_MercenaryCommand                                  0x27f2
#define OP_MercenaryDataRequest                              0x11c1
#define OP_MercenaryDataResponse                             0x72ce
#define OP_MercenaryDataUpdate                               0x61a4
#define OP_MercenaryDataUpdateRequest                        0x7b89
#define OP_MercenaryDismiss                                  0x6e83
#define OP_MercenaryHire                                     0x7169
#define OP_MercenarySuspendRequest                           0x4407
#define OP_MercenarySuspendResponse                          0x6f03
#define OP_MercenaryTimer                                    0x0763
#define OP_MercenaryTimerRequest                             0x31e4
#define OP_MercenaryUnknown1                                 0x5d26
#define OP_MercenaryUnsuspendResponse                        0x27a0

// --- merchant stuff ---
#define OP_ShopDeleteParcel                                  0x47f1
#define OP_ShopDelItem                                       0x724f
#define OP_ShopEnd                                           0x30a8
#define OP_ShopEndConfirm                                    0x3196
#define OP_ShopParcelIcon                                    0x46f0
#define OP_ShopPlayerBuy                                     0x0ddd
#define OP_ShopPlayerSell                                    0x791b
#define OP_ShopRequest                                       0x4fed
#define OP_ShopRetrieveParcel                                0x7013
#define OP_ShopSendParcel                                    0x3a5d

// --- Misc Opcodes ---
#define OP_AAAction                                          0x424e
#define OP_Action                                            0x744c
#define OP_AddNimbusEffect                                   0xc693
#define OP_AggroMeterLockTarget                              0x1643
#define OP_AggroMeterTargetInfo                              0x16bc
#define OP_AggroMeterUpdate                                  0x1781
#define OP_AltCurrency                                       0x6b6d
#define OP_AltCurrencyMerchantReply                          0x27a2
#define OP_AltCurrencyMerchantRequest                        0x5409
#define OP_AltCurrencyPurchase                               0x3788
#define OP_AltCurrencyReclaim                                0x0339
#define OP_AltCurrencySell                                   0x40b6
#define OP_AltCurrencySellSelection                          0x532a
#define OP_Animation                                         0x7177
#define OP_ApplyPoison                                       0x31e6
#define OP_Assist                                            0x4478
#define OP_AssistGroup                                       0x27f8
#define OP_AugmentInfo                                       0x0afb
#define OP_AugmentItem                                       0x661b
#define OP_AutoAttack                                        0x109d
#define OP_AutoAttack2                                       0x3526
#define OP_AutoFire                                          0x241e
#define OP_Bandolier                                         0x7677
#define OP_BankerChange                                      0x791e
#define OP_Begging                                           0x6703
#define OP_BeginCast                                         0x318f
#define OP_BoardBoat                                         0x4211
#define OP_BookButton                                        0x6146
#define OP_Buff                                              0x659c
#define OP_BuffCreate                                        0x3377
#define OP_BuffRemoveRequest                                 0x64f2
#define OP_Bug                                               0x73f4
#define OP_CameraEffect                                      0x127f
#define OP_Camp                                              0x28ec
#define OP_CancelSneakHide                                   0x0927
#define OP_CashReward                                        0x5f7a
#define OP_CastSpell                                         0x1287
#define OP_ChannelMessage                                    0x2b2d
#define OP_Charm                                             0x5d92
#define OP_ClearNPCMarks                                     0x2003
#define OP_ClearSurname                                      0x3fb0
#define OP_ClickDoor                                         0x3a8f
#define OP_ColoredText                                       0x43af
#define OP_ConfirmDelete                                     0x43a3
#define OP_Consent                                           0x1fd1
#define OP_ConsentDeny                                       0x7a45
#define OP_ConsentResponse                                   0x384a
#define OP_Consider                                          0x742b
#define OP_ConsiderCorpse                                    0x5204
#define OP_Consume                                           0x4b70
#define OP_ControlBoat                                       0x0ae7
#define OP_CorpseDrag                                        0x0904
#define OP_CorpseDrop                                        0x7037
#define OP_CrystalCountUpdate                                0x467f
#define OP_CrystalCreate                                     0x7aee
#define OP_CrystalReclaim                                    0x2439
#define OP_CustomTitles                                      0x100e
#define OP_Damage                                            0x6f15
#define OP_Death                                             0x6517
#define OP_DelegateAbility                                   0x76b8
#define OP_DeleteCharge                                      0x01b8
#define OP_DeleteItem                                        0x18ad
#define OP_DeleteSpawn                                       0x7280
#define OP_DeleteSpell                                       0x3358
#define OP_DenyResponse                                      0x2382
#define OP_DisciplineTimer                                   0x6989
#define OP_DisciplineUpdate                                  0x759e
#define OP_DuelAccept                                        0x68d3
#define OP_DuelDecline                                       0x6a46
#define OP_Dye                                               0x23b9
#define OP_Emote                                             0x373b
#define OP_EnvDamage                                         0x51fd
#define OP_FaceChange                                        0x5578
#define OP_Feedback                                          0x5602
#define OP_FindPersonReply                                   0x7e58
#define OP_FindPersonRequest                                 0x5cea
#define OP_Fling                                             0x6f80
#define OP_Forage                                            0x5306
#define OP_FormattedMessage                                  0x1024
#define OP_FriendsWho                                        0x3956
#define OP_GMEndTraining                                     0x4d6b
#define OP_GMTraining                                        0x1966
#define OP_GMTrainSkill                                      0x2a85
#define OP_GMTrainSkillConfirm                               0x4b64
#define OP_Hide                                              0x67fe
#define OP_HideCorpse                                        0x49e1
#define OP_Illusion                                          0x312a
#define OP_IncreaseStats                                     0x70a3
#define OP_InspectAnswer                                     0x71ac
#define OP_InspectMessageUpdate                              0x4d25
#define OP_InspectRequest                                    0x57bc
#define OP_InterruptCast                                     0x048c
#define OP_ItemLinkClick                                     0x4cef
#define OP_ItemLinkResponse                                  0x70c0
#define OP_ItemPacket                                        0x368e
#define OP_ItemPreview                                       0x6b5c
#define OP_ItemPreviewRequest                                0x7f80
#define OP_ItemRecastDelay                                   0x15a9
#define OP_ItemViewUnknown                                   0x465b
#define OP_Jump                                              0x31f4
#define OP_KeyRing                                           0x1219
#define OP_KickPlayers                                       0x6770
#define OP_LDoNButton                                        0x5327
#define OP_LeadershipExpToggle                               0x74bd
#define OP_LeadershipExpUpdate                               0x2797
#define OP_LeaveBoat                                         0x7617
#define OP_LevelAppearance                                   0x3bc9
#define OP_LinkedReuse                                       0x1619
#define OP_LoadSpellSet                                      0x261d
#define OP_Logout                                            0x4ac6
#define OP_MarkNPC                                           0x1fb5
#define OP_Marquee                                           0x502e
#define OP_MemorizeSpell                                     0x217c
#define OP_MobHealth                                         0x37b1
#define OP_MobRename                                         0x2c57
#define OP_MobUpdate                                         0x2c84
#define OP_MoneyOnCorpse                                     0x5f44
#define OP_MoveCoin                                          0x0bcf
#define OP_MoveDoor                                          0x08e8
#define OP_MoveItem                                          0x32ee
#define OP_MoveMultipleItems                                 0x5623
#define OP_NewSpawn                                          0x6097
#define OP_NPCMoveUpdate                                     0x5892
#define OP_OnLevelMessage                                    0x575b
#define OP_PetBuffWindow                                     0x5882
#define OP_PetCommands                                       0x0159
#define OP_PetCommandState                                   0x1dc8
#define OP_PetHoTT                                           0x794a
#define OP_Petition                                          0x3de3
#define OP_PlayerStateAdd                                    0x559a
#define OP_PlayerStateRemove                                 0x2d25
#define OP_PopupResponse                                     0x08a6
#define OP_PotionBelt                                        0x1a3e
#define OP_PurchaseLeadershipAA                              0x6c55
#define OP_PVPLeaderBoardDetailsReply                        0x25b7
#define OP_PVPLeaderBoardDetailsRequest                      0x3707
#define OP_PVPLeaderBoardReply                               0x071f
#define OP_PVPLeaderBoardRequest                             0x04aa
#define OP_PVPStats                                          0x4b15
#define OP_QueryUCSServerStatus                              0x398f
#define OP_RandomReply                                       0x106b
#define OP_RandomReq                                         0x7b10
#define OP_ReadBook                                          0x72df
#define OP_RemoveAllDoors                                    0x700c
#define OP_RemoveNimbusEffect                                0x7b1e
#define OP_Report                                            0x6f14
#define OP_RequestDuel                                       0x3af1
#define OP_ResetAA                                           0x1669
#define OP_RespawnWindow                                     0x0ecb
#define OP_RestState                                         0x000f
#define OP_Rewind                                            0x1745
#define OP_RezzAnswer                                        0x701c
#define OP_RezzComplete                                      0x760d
#define OP_RezzRequest                                       0x3c21
#define OP_Sacrifice                                         0x1821
#define OP_SafeFallSuccess                                   0x2219
#define OP_Save                                              0x4a39
#define OP_SaveOnZoneReq                                     0x600d
#define OP_SendFindableNPCs                                  0x4613
#define OP_SenseHeading                                      0x260a
#define OP_SetFace                                           0x1af3
#define OP_SetGroupTarget                                    0x2814
#define OP_SetRunMode                                        0x009f
#define OP_SetStartCity                                      0x6326
#define OP_Shielding                                         0x52e5
#define OP_SimpleMessage                                     0x213f
#define OP_SkillUpdate                                       0x04c
#define OP_SomeItemPacketMaybe                               0x747c
#define OP_Sound                                             0x1a30
#define OP_SpellEffect                                       0x5936
#define OP_Split                                             0x3a54
#define OP_Stun                                              0x36a4
#define OP_Surname                                           0x0423
#define OP_SwapSpell                                         0x0efa
#define OP_TargetBuffs                                       0x4f4b
#define OP_TargetCommand                                     0x58e2
#define OP_TargetHoTT                                        0x0272
#define OP_TargetMouse                                       0x075d
#define OP_TestBuff                                          0x7cb8
#define OP_Track                                             0x17e5
#define OP_TrackTarget                                       0x695e
#define OP_TrackUnknown                                      0x4577
#define OP_Translocate                                       0x6580
#define OP_UnderWorld                                        0x2eb3
#define OP_Untargetable                                      0x053c
#define OP_UpdateLeadershipAA                                0x0026
#define OP_VetClaimReply                                     0x16d4
#define OP_VetClaimRequest                                   0x1126
#define OP_VetRewardsAvaliable                               0x590e
#define OP_VoiceMacroIn                                      0x17fd
#define OP_VoiceMacroOut                                     0x409a
#define OP_WeaponEquip1                                      0x34a7
#define OP_Weblink                                           0x6f4b
#define OP_WhoAllRequest                                     0x674b
#define OP_WhoAllResponse                                    0x578c
#define OP_XTargetAutoAddHaters                              0x672f
#define OP_XTargetOpen                                       0x61df
#define OP_XTargetOpenResponse                               0x3ef8
#define OP_XTargetRequest                                    0x3763
#define OP_XTargetResponse                                   0x4d59
#define OP_YellForHelp                                       0x4e56
#define OP_ZonePlayerToBind                                  0x08d8

// --- New Opcodes ---
#define OP_EnduranceUpdate                                   0x5f42
#define OP_ManaUpdate                                        0x3791
#define OP_MobEnduranceUpdate                                0x1c81
#define OP_MobManaUpdate                                     0x2404

// --- pc/npc trading ---
#define OP_CancelTrade                                       0x354c
#define OP_FinishTrade                                       0x3993
#define OP_MoneyUpdate                                       0x640c
#define OP_TradeAcceptClick                                  0x69e2
#define OP_TradeBusy                                         0x5505
#define OP_TradeCoins                                        0x4206
#define OP_TradeMoneyUpdate                                  0x68c2
#define OP_TradeRequest                                      0x77b5
#define OP_TradeRequestAck                                   0x14bf

// --- Raid Opcodes ---
#define OP_MarkRaidNPC                                       0x5a58
#define OP_RaidClearNPCMarks                                 0x20d3
#define OP_RaidDelegateAbility                               0x2b33
#define OP_RaidInvite                                        0x55ac
#define OP_RaidUpdate                                        0x3973

// --- Reasons for Disconnect: ---
#define OP_WorldClientReady                                  0x23c1
#define OP_ZoneUnavail                                       0x4cb4

// --- Required to fully log in ---
#define OP_AAExpUpdate                                       0x7d14
#define OP_BlockedBuffs                                      0x3033
#define OP_ChangeSize                                        0x4707
#define OP_CharInventory                                     0x5ca6
#define OP_ClearAA                                           0x0f50
#define OP_ClearBlockedBuffs                                 0x34cb
#define OP_ClearLeadershipAbilities                          0x6da5
#define OP_ClientReady                                       0x345d
#define OP_ClientUpdate                                      0x7dfc
#define OP_ExpUpdate                                         0x20ed
#define OP_GroundSpawn                                       0x6fca
#define OP_GuildMemberList                                   0x12a6
#define OP_GuildMOTD                                         0x3e13
#define OP_HPUpdate                                          0x2828
#define OP_ManaChange                                        0x5467
#define OP_RemoveBlockedBuffs                                0x0de7
#define OP_ReqClientSpawn                                    0x35fa
#define OP_RequestGuildTributes                              0x4d5e
#define OP_RespondAA                                         0x7a27
#define OP_SendAAStats                                       0x43c8
#define OP_SendAATable                                       0x66b5
#define OP_SendExpZonein                                     0x5f8e
#define OP_SendTributes                                      0x729b
#define OP_SendZonepoints                                    0x69a4
#define OP_SetServerFilter                                   0x444d
#define OP_SpawnAppearance                                   0x0971
#define OP_SpawnDoor                                         0x7291
#define OP_SpecialMesg                                       0x0083
#define OP_TGB                                               0x0876
#define OP_TributeInfo                                       0x4254
#define OP_TributeTimer                                      0x073d
#define OP_TributeUpdate                                     0x5961
#define OP_UpdateAA                                          0x66f0
#define OP_WearChange                                        0x7994
#define OP_Weather                                           0x661e
#define OP_WorldObjectsSent                                  0x5ae2

// --- Required to reach Char Select: ---
#define OP_ApproveWorld                                      0x7499
#define OP_EnterWorld                                        0x578f
#define OP_ExpansionInfo                                     0x590d
#define OP_GuildsList                                        0x507a
#define OP_LogServer                                         0x7ceb
#define OP_PostEnterWorld                                    0x6259
#define OP_SendCharInfo                                      0x00d2
#define OP_SendLoginInfo                                     0x7a09
#define OP_World_Client_CRC1                                 0x0f13
#define OP_World_Client_CRC2                                 0x4b8d
#define OP_World_Client_CRC3                                 0x298d

// --- Sent after canceling trade or after closing tradeskill object ---
#define OP_FinishWindow                                      0x7349
#define OP_FinishWindow2                                     0x40ef

// --- Shared Tasks ---
#define OP_SharedTaskAcceptNew                               0x6646
#define OP_SharedTaskAddPlayer                               0x36e8
#define OP_SharedTaskInvite                                  0x3444
#define OP_SharedTaskInviteResponse                          0x7582
#define OP_SharedTaskMakeLeader                              0x37f2
#define OP_SharedTaskMemberChange                            0x0119
#define OP_SharedTaskMemberList                              0x1e7d
#define OP_SharedTaskPlayerList                              0x5727
#define OP_SharedTaskQuit                                    0x322e
#define OP_SharedTaskRemovePlayer                            0x4865
#define OP_SharedTaskSelectWindow                            0x48a2
#define OP_TaskTimers                                        0x2b0f

// --- Task packets ---
#define OP_AcceptNewTask                                     0x0a23
#define OP_AvaliableTask                                     0x36e8
#define OP_CancelTask                                        0x39f0
#define OP_CompletedTasks                                    0x4eba
#define OP_TaskActivity                                      0x08d3
#define OP_TaskActivityComplete                              0x5e19
#define OP_TaskDescription                                   0x3714
#define OP_TaskHistoryReply                                  0x3d05
#define OP_TaskHistoryRequest                                0x5f1c
#define OP_TaskRequestTimer                                  0x7a48
#define OP_TaskSelectWindow                                  0x705b

// --- This can be fixed with a client patch. ---
#define OP_ChangePetName                                     0x5dab
#define OP_InvokeChangePetName                               0x4506
#define OP_InvokeChangePetNameImmediate                      0x046d
#define OP_InvokeNameChangeImmediate                         0x4fe2
#define OP_InvokeNameChangeLazy                              0x2f2e

// --- Title opcodes ---
#define OP_NewTitlesAvailable                                0x0d32
#define OP_RequestTitles                                     0x6344
#define OP_SendTitleList                                     0x2d08
#define OP_SetTitle                                          0x6527
#define OP_SetTitleReply                                     0x4c21

// --- tradeskill stuff: ---
#define OP_ClearObject                                       0x7a11
#define OP_ClickObject                                       0x4aa1
#define OP_ClickObjectAction                                 0x0c1e
#define OP_RecipeAutoCombine                                 0x40d7
#define OP_RecipeDetails                                     0x6e02
#define OP_RecipeReply                                       0x1db6
#define OP_RecipesFavorite                                   0x71b1
#define OP_RecipesSearch                                     0x6290
#define OP_TradeSkillCombine                                 0x579a

// --- Tribute Packets: ---
#define OP_GuildModifyBenefits                               0x34cd
#define OP_GuildOptInOut                                     0x0727
#define OP_GuildSaveActiveTributes                           0x5f3f
#define OP_GuildSelectTribute                                0x649f
#define OP_GuildSendActiveTributes                           0x3771
#define OP_GuildTributeDonateItem                            0x5bcb
#define OP_GuildTributeDonatePlat                            0x43d2
#define OP_GuildTributeFavorAndTimer                         0x7fe0
#define OP_GuildTributeToggleReply                           0x20ae
#define OP_GuildTributeToggleReq                             0x7745
#define OP_OpenGuildTributeMaster                            0x49ea
#define OP_OpenTributeMaster                                 0x7666
#define OP_SelectTribute                                     0x79fc
#define OP_SendGuildTributes                                 0x1877
#define OP_TributeItem                                       0x4f3e
#define OP_TributeMoney                                      0x58fb
#define OP_TributePointUpdate                                0x5300
#define OP_TributeToggle                                     0x241d

// --- Zone in opcodes ---
#define OP_AckPacket                                         0x471d
#define OP_LevelUpdate                                       0x1eec
#define OP_NewZone                                           0x1795
#define OP_PlayerProfile                                     0x6506
#define OP_ReqNewZone                                        0x7887
#define OP_RequestClientZoneChange                           0x3fcf
#define OP_Stamina                                           0x2a79
#define OP_TimeOfDay                                         0x5070
#define OP_ZoneChange                                        0x2d18
#define OP_ZoneEntry                                         0x5089
#define OP_ZoneSpawns                                        0x5237


/////
// Section 6: Class Vtable Addresses (from MSVC RTTI scan)
// Source: EQRTTIScan.java - embedded type info in eqgame.exe
// WndNotification is at vtable[8] (calibrated from CXWnd vs CSidlScreenWnd comparison)
/////

// --- Base UI / Window Framework (RTTI template vtables - 0xA1xxxx range) ---
// NOTE: These are the SIDL XML template/parameter class vtables.
//       Actual game window instance vtables are in the 0x9Exxxx range (see confirmed ones below).
#define CXWnd__WndNotification_x                                     0x867210  // template vtable[8]
// CSidlScreenWnd__WndNotification_x already defined above as 0x85EAD0 (instance vtable)

// --- Confirmed Game Window Instance Vtables (0x9Exxxx range, from .rdata backtrack) ---
// Method: searched .rdata for known WndNotification addr, walked back 8*4 bytes to vtable[0]
#define CBankWnd__vftable_x                                          0x9E2540
#define CBarterSearchWnd__vftable_x                                  0x9E29E0
#define CBarterWnd__vftable_x                                        0x9E2F30
#define CBuffWindow__vftable_x                                       0x9E4728
#define CChatWindow__vftable_x                                       0x9E5D70
#define CGroupWnd__vftable_x                                         0x9E9B10
// CSidlScreenWnd instance vtable = 0x9E4568 (game window objects)
// NOTE: CSidlScreenWnd__vftable_x above (0xA1982C) is the RTTI/template vtable used by SIDL parser
//       The actual screen window instances use 0x9E4568 (same WndNotification 0x85EAD0 at vtable[8])
#define CTargetWnd__vftable_x                                        0x9FB858
#define CCheckBoxWnd__vftable_x                                      0xA1DB4C
#define CComboWnd__vftable_x                                         0xA19DDC
#define CContextMenu__vftable_x                                      0xA195C4
#define CContextMenuManagerBase__vftable_x                           0xA19A5C
#define CEditBaseWnd__vftable_x                                      0xA1A3A4
#define CEditWnd__vftable_x                                          0xA1A524
#define CFinderWnd__vftable_x                                        0xA1D8B4
#define CHorizontalLayoutWnd__vftable_x                              0xA1B964
#define CLabelWnd__vftable_x                                         0xA1AE9C
#define CLayoutWnd__vftable_x                                        0xA1B63C
#define CPageWnd__vftable_x                                          0xA1C0AC
#define CRadioGroup__vftable_x                                       0xA19820
#define CSliderWnd__vftable_x                                        0xA1B2B4
#define CSidlManagerBase__vftable_x                                  0xA1A2AC
#define CStmlWnd__vftable_x                                          0xA1A824
#define CTabWnd__vftable_x                                           0xA1AD2C
#define CTemplateContainerWnd__vftable_x                             0xA1C22C
#define CTileLayoutWnd__vftable_x                                    0xA1B7C4
#define CTreeViewWnd__vftable_x                                      0xA19414
#define CVerticalLayoutWnd__vftable_x                                0xA1BAEC
#define CXMLData__vftable_x                                          0xA19FB0
#define CXMLDataManager__vftable_x                                   0xA1A118
#define CXMLSymbolTable__vftable_x                                   0xA1DD0C
#define CXWndManager__vftable_x                                      0xA1A338

// --- Player / Character Hierarchy ---
#define CharacterBase__vftable_x                                     0xA0E94C   // RTTI-verified
#define PcBase__vftable_x                                            0xA04EDC   // RTTI-verified
#define PcProfile__vftable_x                                         0xA10C74   // RTTI-verified
#define BaseProfile__vftable_x                                       0xA0FE68
#define PlayerBase__vftable_x                                        0xA1E9C4
#define CPlayerCustomizationManager__vftable_x                       0xA1E360

// --- CEverQuest Base ---
#define CEverQuestBase__vftable_x                                    0xA1EB78
#define EQAreaBase__vftable_x                                        0xA1EB70

// --- Item System ---
#define ItemBase__vftable_x                                          0xA02310   // RTTI-verified

// --- World / Zone Data ---
#define EQWorldData__vftable_x                                       0xA0AB68
#define EQZoneInfo__vftable_x                                        0xA0AB8C
#define ZoneGuideManagerBase__vftable_x                              0xA10548

// --- Social / Guild / Group ---
#define CGroupBase__vftable_x                                        0xA0A0FC
#define CGroupMemberBase__vftable_x                                  0xA0A0DC
#define GuildBase__vftable_x                                         0xA0A714   // RTTI-verified
#define SocialManagerBase__vftable_x                                 0xA1004C

// --- Real Estate (Player Housing) ---
#define RealEstate__vftable_x                                        0xA1052C

// --- Reward / Poll System ---
#define PendingReward__vftable_x                                     0xA0FF98
#define PendingRewardList__vftable_x                                 0xA0FFF0   // RTTI-verified
#define Poll__vftable_x                                              0xA103FC
#define PollAnswer__vftable_x                                        0xA10270
#define PollResult__vftable_x                                        0xA10290
#define PointSystemBase__vftable_x                                   0xA10218

// --- Achievements ---
#define AchievementCategory__vftable_x                               0xA0222C
#define AchievementComponent__vftable_x                              0xA02220

// --- Network Stack (UDP) ---
#define UdpConnection__vftable_x                                     0xA1E0B8
#define UdpConnectionHandler__vftable_x                              0xA0FE8C
#define UdpDriver__vftable_x                                         0xA1E0D0
#define UdpManager__vftable_x                                        0xA1E1F0
#define UdpManagerThread__vftable_x                                  0xA1E1A0
#define UdpPlatformDriver__vftable_x                                 0xA1E104
#define UdpPlatformThreadObject__vftable_x                           0xA1E168
#define UdpRefCount__vftable_x                                       0xA1DE54
#define UdpGuardedRefCount__vftable_x                                0xA1DE6C
#define LogicalPacket__vftable_x                                     0xA1E2C0
#define SimpleLogicalPacket__vftable_x                               0xA1E2EC
#define PooledLogicalPacket__vftable_x                               0xA1E318

// --- Network Stack (TCP) ---
#define TcpConnection__vftable_x                                     0xA27B84
#define TcpConnectionDriver__vftable_x                               0xA27BDC
#define TcpConnectionPlatformDriver__vftable_x                       0xA27BF8
#define TcpDriver__vftable_x                                         0xA27CE4
#define TcpManager__vftable_x                                        0xA27BA4
#define TcpPlatformDriver__vftable_x                                 0xA27CAC
#define UniversalChatProxy__vftable_x                                0xA0FEA8   // RTTI-verified

// --- Collision / Physics ---
#define CCollisionInfoBase__vftable_x                                0xA1E99C
#define CCollisionInfoCapsuleVisibility__vftable_x                   0xA1ED94
#define CCollisionInfoLineSegment__vftable_x                         0xA1EAD0   // RTTI-verified
#define CCollisionInfoLineSegmentVisibility__vftable_x               0xA1EDBC
#define CCollisionInfoSphere__vftable_x                              0xA1ED6C   // RTTI-verified
#define CCollisionInfoTargetVisibility__vftable_x                    0xA1EDE4
#define PhysicsManagerBase__vftable_x                                0xA1ED4C

// --- Spline / Path System ---
#define Spline__vftable_x                                            0xA1EE14
#define SplineActionBase__vftable_x                                  0xA1F08C
#define SplineCommon__vftable_x                                      0xA1EE7C
#define SplineControlPoint__vftable_x                                0xA1EE0C
#define SplineManagerBase__vftable_x                                 0xA1EB8C
#define SplineMoverBase__vftable_x                                   0xA1EE1C   // RTTI-verified
#define RideBase__vftable_x                                          0xA1EE38
#define TriggerBase__vftable_x                                       0xA1EE40

// --- Cryptography ---
#define Cipher__vftable_x                                            0xA482CC
#define CipherAES__vftable_x                                         0xA482FC
#define SymmetricKey__vftable_x                                      0xA48258

// --- Threading ---
#define Thread__vftable_x                                            0xA4DA70
#define JobQueueWorkerThread__vftable_x                              0xA4DA38


/////
// Section 7: Window Vtable Master Reference
// Source: Pattern scan of .rdata for CSidlScreenWnd-derived classes
// Method: vtable[0-7] shared pattern + unique vtable[8] (WndNotification)
// Total found: 167 vtables -> 113 unique window classes
/////

// ============================================================
// SHARED virtual function addresses (vtable[0]-[7] + [31]-[34])
// These entries are IDENTICAL across all CSidlScreenWnd-derived windows
// ============================================================
#define CSidlScreenWnd__vfunc0_x                                     0x863670  // destructor (vtable[0])
#define CSidlScreenWnd__vfunc1_x                                     0x863B70  // scalar destructor (vtable[1])
#define CSidlScreenWnd__vfunc2_x                                     0x8633E0  // vtable[2]
#define CSidlScreenWnd__vfunc3_x                                     0x863400  // vtable[3]
#define CSidlScreenWnd__vfunc4_x                                     0x863410  // vtable[4]
#define CSidlScreenWnd__vfunc5_x                                     0x863430  // vtable[5]
#define CSidlScreenWnd__vfunc6_x                                     0x863450  // vtable[6]
#define CSidlScreenWnd__vfunc7_x                                     0x863460  // vtable[7]
// vtable[8] = WndNotification (class-specific, see per-class defines below)
#define CSidlScreenWnd__vfunc31_x                                    0x8629F0  // vtable[31] - shared
#define CSidlScreenWnd__vfunc32_x                                    0x411F40  // vtable[32] - shared
#define CSidlScreenWnd__vfunc33_x                                    0x864220  // vtable[33] - shared
#define CSidlScreenWnd__vfunc34_x                                    0x864460  // vtable[34] - shared (last entry)

// ============================================================
// CONFIRMED window vtables + WndNotification (from known addresses)
// ============================================================
#define CBankWnd__vftable_x.PadRight(60) 0x9E2540
#define CBankWnd__WndNotification_x.PadRight(60) 0x6293C0

#define CBarterSearchWnd__vftable_x.PadRight(60) 0x9E29E0
#define CBarterSearchWnd__WndNotification_x.PadRight(60) 0x62D810

#define CBarterWnd__vftable_x.PadRight(60) 0x9E2F30
#define CBarterWnd__WndNotification_x.PadRight(60) 0x631A10

#define CBuffWindow__vftable_x.PadRight(60) 0x9E4728
#define CBuffWindow__WndNotification_x.PadRight(60) 0x641810

#define CChatWindow__vftable_x.PadRight(60) 0x9E5D70
#define CChatWindow__WndNotification_x.PadRight(60) 0x656640

#define CGroupWnd__vftable_x.PadRight(60) 0x9E9B10
#define CGroupWnd__WndNotification_x.PadRight(60) 0x67C620

#define CSidlScreenWnd__vftable_x.PadRight(60) 0x9E4568
#define CSidlScreenWnd__WndNotification_x.PadRight(60) 0x85EAD0

#define CTargetWnd__vftable_x.PadRight(60) 0x9FB858
#define CTargetWnd__WndNotification_x.PadRight(60) 0x769AC0


// ============================================================
// APPROXIMATE window vtables (nearest constructor address match)
// Confidence varies by dist value - lower dist = more confident
// VERIFY against Ghidra decompile before using in production hooks
// Format: WndNotify=0xXXXX is the class's WndNotification function
// ============================================================
// [MED dist=0x16A0] AAWindow  WndNotify=0x60AA80
#define AAWindow__vftable_x                                          0x9DE470
#define AAWindow__WndNotification_x                                  0x60AA80

// [MED dist=0xB60] AchievementsWnd  WndNotify=0x60EE70
#define AchievementsWnd__vftable_x                                   0x9DEAF8
#define AchievementsWnd__WndNotification_x                           0x60EE70

// [MED dist=0x1650] AdvancedDisplayOptionsWindow  WndNotify=0x615150
#define AdvancedDisplayOptionsWindow__vftable_x                      0x9DFBA0
#define AdvancedDisplayOptionsWindow__WndNotification_x              0x615150

// [MED dist=0x680] AdventureLeaderboardWnd  WndNotify=0x617540
#define AdventureLeaderboardWnd__vftable_x                           0x9DFE88
#define AdventureLeaderboardWnd__WndNotification_x                   0x617540

// [MED dist=0x560] AdventureRequestWnd  WndNotify=0x618470
#define AdventureRequestWnd__vftable_x                               0x9E01B0
#define AdventureRequestWnd__WndNotification_x                       0x618470

// [HIGH dist=0x430] AggroMeterWnd  WndNotify=0x411BD0
#define AggroMeterWnd__vftable_x                                     0x9C4108
#define AggroMeterWnd__WndNotification_x                             0x411BD0

// [HIGH dist=0x240] AlarmWnd  WndNotify=0x61A540
#define AlarmWnd__vftable_x                                          0x9E05B8
#define AlarmWnd__WndNotification_x                                  0x61A540

// [HIGH dist=0x130] AlertStackWnd  WndNotify=0x61B560
#define AlertStackWnd__vftable_x                                     0x9E0948
#define AlertStackWnd__WndNotification_x                             0x61B560

// [HIGH dist=0x340] AlertWnd  WndNotify=0x61C370
#define AlertWnd__vftable_x                                          0x9E0B60
#define AlertWnd__WndNotification_x                                  0x61C370

// [MED dist=0x7C0] AltStorageWnd  WndNotify=0x61DDA0
#define AltStorageWnd__vftable_x                                     0x9E0D30
#define AltStorageWnd__WndNotification_x                             0x61DDA0

// [HIGH dist=0x310] AuraWindow  WndNotify=0x620FE0
#define AuraWindow__vftable_x                                        0x9E1380
#define AuraWindow__WndNotification_x                                0x620FE0

// [HIGH dist=0x4F0] BandolierWnd  WndNotify=0x626980
#define BandolierWnd__vftable_x                                      0x9E2218
#define BandolierWnd__WndNotification_x                              0x626980

// [MED dist=0xD30] BarterMerchantWnd  WndNotify=0x62A590
#define BarterMerchantWnd__vftable_x                                 0x9E2728
#define BarterMerchantWnd__WndNotification_x                         0x62A590

// [MED dist=0x560] BazaarConfirmationWnd  WndNotify=0x41C7C0
#define BazaarConfirmationWnd__vftable_x                             0x9C4850
#define BazaarConfirmationWnd__WndNotification_x                     0x41C7C0

// [MED dist=0xD40] BazaarWnd  WndNotify=0x639BB0
#define BazaarWnd__vftable_x                                         0x9E3CF0
#define BazaarWnd__WndNotification_x                                 0x639BB0

// [MED dist=0x920] BlockedBuffWnd  WndNotify=0x63D370
#define BlockedBuffWnd__vftable_x                                    0x9E3E88
#define BlockedBuffWnd__WndNotification_x                            0x63D370

// [MED dist=0x9F0] BlockedPetBuffWnd  WndNotify=0x41E0D0
#define BlockedPetBuffWnd__vftable_x                                 0x9C4C88
#define BlockedPetBuffWnd__WndNotification_x                         0x41E0D0

// [HIGH dist=0x200] BookWindow  WndNotify=0x63FBD0
#define BookWindow__vftable_x                                        0x9E41B8
#define BookWindow__WndNotification_x                                0x63FBD0

// [MED dist=0x910] CastingWindow  WndNotify=0x645960
#define CastingWindow__vftable_x                                     0x9E5048
#define CastingWindow__WndNotification_x                             0x645960

// [MED dist=0x9F0] CastSpellWnd  WndNotify=0x648D00
#define CastSpellWnd__vftable_x                                      0x9E54D0
#define CastSpellWnd__WndNotification_x                              0x648D00

// [MED dist=0x1110] CInvSlotWnd  WndNotify=0x699B00
#define CInvSlotWnd__vftable_x                                       0x9EB988
#define CInvSlotWnd__WndNotification_x                               0x699B00

// [HIGH dist=0x3D0] ClaimWnd  WndNotify=0x658500
#define ClaimWnd__vftable_x                                          0x9E5F08
#define ClaimWnd__WndNotification_x                                  0x658500

// [HIGH dist=0x30] CListWnd  WndNotify=0x857BD0

// [MED dist=0x14F0] CMapViewWnd  WndNotify=0x6D1060
#define CMapViewWnd__vftable_x                                       0x9EE9A8
#define CMapViewWnd__WndNotification_x                               0x6D1060

// [HIGH dist=0x4D0] ColorPickerWnd  WndNotify=0x6596B0
#define ColorPickerWnd__vftable_x                                    0x9E6218
#define ColorPickerWnd__WndNotification_x                            0x6596B0

// [MED dist=0x10E0] CombatAbilityWnd  WndNotify=0x65A2E0
#define CombatAbilityWnd__vftable_x                                  0x9E6418
#define CombatAbilityWnd__WndNotification_x                          0x65A2E0

// [MED dist=0x8C0] CombatSkillSelectWnd  WndNotify=0x477A90
#define CombatSkillSelectWnd__vftable_x                              0x9C66D8
#define CombatSkillSelectWnd__WndNotification_x                      0x477A90

// [HIGH dist=0x60] DynamicZoneWnd  WndNotify=0x6637B0
#define DynamicZoneWnd__vftable_x                                    0x9E6E60
#define DynamicZoneWnd__WndNotification_x                            0x6637B0

// [HIGH dist=0x4D0] EditLabelWnd  WndNotify=0x665140
#define EditLabelWnd__vftable_x                                      0x9E73B0
#define EditLabelWnd__WndNotification_x                              0x665140

// [HIGH dist=0x380] EQMainWnd  WndNotify=0x665ED0
#define EQMainWnd__vftable_x                                         0x9E75B8
#define EQMainWnd__WndNotification_x                                 0x665ED0

// [MED dist=0xA00] ExtendedTargetWnd  WndNotify=0x54AE00
#define ExtendedTargetWnd__vftable_x                                 0x9D3628
#define ExtendedTargetWnd__WndNotification_x                         0x54AE00

// [HIGH dist=0x230] FeedbackWindow  WndNotify=0x667830
#define FeedbackWindow__vftable_x                                    0x9E7AA8
#define FeedbackWindow__WndNotification_x                            0x667830

// [MED dist=0xA50] FellowshipWnd  WndNotify=0x669440
#define FellowshipWnd__vftable_x                                     0x9E7C50
#define FellowshipWnd__WndNotification_x                             0x669440

// [MED dist=0x9D0] FileSelectionWnd  WndNotify=0x66B260
#define FileSelectionWnd__vftable_x                                  0x9E8240
#define FileSelectionWnd__WndNotification_x                          0x66B260

// [MED dist=0x870] FriendsWindow  WndNotify=0x66D780
#define FriendsWindow__vftable_x                                     0x9E8450
#define FriendsWindow__WndNotification_x                             0x66D780

// [MED dist=0x640] GemsGameWnd  WndNotify=0x672590
#define GemsGameWnd__vftable_x                                       0x9E89C8
#define GemsGameWnd__WndNotification_x                               0x672590

// [MED dist=0x790] GiveWnd  WndNotify=0x673EC0
#define GiveWnd__vftable_x                                           0x9E8D28
#define GiveWnd__WndNotification_x                                   0x673EC0

// [HIGH dist=0x2A0] GroupMentorWindow  WndNotify=0x674980
#define GroupMentorWindow__vftable_x                                 0x9E8F00
#define GroupMentorWindow__WndNotification_x                         0x674980

// [HIGH dist=0x170] GroupSearchFiltersWnd  WndNotify=0x675FD0
#define GroupSearchFiltersWnd__vftable_x                             0x9E92E8
#define GroupSearchFiltersWnd__WndNotification_x                     0x675FD0

// [MED dist=0x11B0] GroupWindow  WndNotify=0x67AA50
#define GroupWindow__vftable_x                                       0x9E9490
#define GroupWindow__WndNotification_x                               0x67AA50

// [HIGH dist=0x270] GuildCreationWnd  WndNotify=0x67FB20
#define GuildCreationWnd__vftable_x                                  0x9E9F88
#define GuildCreationWnd__WndNotification_x                          0x67FB20

// [MED dist=0x1590] GuildManagementWnd  WndNotify=0x686C80
#define GuildManagementWnd__vftable_x                                0x9EA268
#define GuildManagementWnd__WndNotification_x                        0x686C80

// [HIGH dist=0x160] HelpWindow  WndNotify=0x54E740
#define HelpWindow__vftable_x                                        0x9D3BE8
#define HelpWindow__WndNotification_x                                0x54E740

// [HIGH dist=0x3B0] HeritageSelectionWnd  WndNotify=0x689F30
#define HeritageSelectionWnd__vftable_x                              0x9EA838
#define HeritageSelectionWnd__WndNotification_x                      0x689F30

// [MED dist=0x6A0] IconSelectionWnd  WndNotify=0x68F0D0
#define IconSelectionWnd__vftable_x                                  0x9EB038
#define IconSelectionWnd__WndNotification_x                          0x68F0D0

// [HIGH dist=0xC0] InspectWnd  WndNotify=0x68F780
#define InspectWnd__vftable_x                                        0x9EB1E8
#define InspectWnd__WndNotification_x                                0x68F780

// [LOW dist=0x2AD0] InventoryWindow  WndNotify=0x693490
#define InventoryWindow__vftable_x                                   0x9EB388
#define InventoryWindow__WndNotification_x                           0x693490

// [MED dist=0x6B0] JournalCatWnd  WndNotify=0x6B1630
#define JournalCatWnd__vftable_x                                     0x9EC3F8
#define JournalCatWnd__WndNotification_x                             0x6B1630

// [MED dist=0x650] JournalNPCWnd  WndNotify=0x6B4130
#define JournalNPCWnd__vftable_x                                     0x9EC5B8
#define JournalNPCWnd__WndNotification_x                             0x6B4130

// [HIGH dist=0x3D0] LayoutCopyWindow  WndNotify=0x6B5AF0
#define LayoutCopyWindow__vftable_x                                  0x9ECC48
#define LayoutCopyWindow__WndNotification_x                          0x6B5AF0

// [MED dist=0xB90] LeadershipWindow  WndNotify=0x6B7BF0
#define LeadershipWindow__vftable_x                                  0x9ECE10
#define LeadershipWindow__WndNotification_x                          0x6B7BF0

// [HIGH dist=0x1E0] LFGuildWnd  WndNotify=0x6BBB30
#define LFGuildWnd__vftable_x                                        0x9ED488
#define LFGuildWnd__WndNotification_x                                0x6BBB30

// [HIGH dist=0x3B0] LoadskinWnd  WndNotify=0x6BC400
#define LoadskinWnd__vftable_x                                       0x9ED648
#define LoadskinWnd__WndNotification_x                               0x6BC400

// [MED dist=0xA00] LootWnd  WndNotify=0x6BE180
#define LootWnd__vftable_x                                           0x9ED7F8
#define LootWnd__WndNotification_x                                   0x6BE180

// [MED dist=0x930] MapToolbarWnd  WndNotify=0x6C6AE0
#define MapToolbarWnd__vftable_x                                     0x9EE448
#define MapToolbarWnd__WndNotification_x                             0x6C6AE0

// [MED dist=0x1380] MerchantWnd  WndNotify=0x6F3A90
#define MerchantWnd__vftable_x                                       0x9F2620
#define MerchantWnd__WndNotification_x                               0x6F3A90

// [HIGH dist=0x340] MusicPlayerWnd  WndNotify=0x6F7F80
#define MusicPlayerWnd__vftable_x                                    0x9F27D8
#define MusicPlayerWnd__WndNotification_x                            0x6F7F80

// [HIGH dist=0x2C0] NameChangeMercWnd  WndNotify=0x6F9D70
#define NameChangeMercWnd__vftable_x                                 0x9F2B98
#define NameChangeMercWnd__WndNotification_x                         0x6F9D70

// [HIGH dist=0x2C0] NameChangePetWnd  WndNotify=0x6FA8F0
#define NameChangePetWnd__vftable_x                                  0x9F2E00
#define NameChangePetWnd__WndNotification_x                          0x6FA8F0

// [HIGH dist=0x2C0] NameChangeWnd  WndNotify=0x6FB470
#define NameChangeWnd__vftable_x                                     0x9F3048
#define NameChangeWnd__WndNotification_x                             0x6FB470

// [MED dist=0x880] NoteWindow  WndNotify=0x6FEA70
#define NoteWindow__vftable_x                                        0x9F3378
#define NoteWindow__WndNotification_x                                0x6FEA70

// [MED dist=0xA80] PetInfoWindow  WndNotify=0x710800
#define PetInfoWindow__vftable_x                                     0x9F5010
#define PetInfoWindow__WndNotification_x                             0x710800

// [MED dist=0x1C70] PetitionQWindow  WndNotify=0x7136F0
#define PetitionQWindow__vftable_x                                   0x9F53D8
#define PetitionQWindow__WndNotification_x                           0x7136F0

// [MED dist=0xC50] PlayerCustomizationWnd  WndNotify=0x715F30
#define PlayerCustomizationWnd__vftable_x                            0x9F57B0
#define PlayerCustomizationWnd__WndNotification_x                    0x715F30

// [HIGH dist=0x440] PlayerNotesWindow  WndNotify=0x717520
#define PlayerNotesWindow__vftable_x                                 0x9F5988
#define PlayerNotesWindow__WndNotification_x                         0x717520

// [MED dist=0x6A0] PlayerWindow  WndNotify=0x718FF0
#define PlayerWindow__vftable_x                                      0x9F5DC8
#define PlayerWindow__WndNotification_x                              0x718FF0

// [HIGH dist=0x390] PotionBeltWnd  WndNotify=0x71F580
#define PotionBeltWnd__vftable_x                                     0x9F6438
#define PotionBeltWnd__WndNotification_x                             0x71F580

// [HIGH dist=0x490] PvpLeaderboardWnd  WndNotify=0x722310
#define PvpLeaderboardWnd__vftable_x                                 0x9F6638
#define PvpLeaderboardWnd__WndNotification_x                         0x722310

// [HIGH dist=0x240] PvPStatsWnd  WndNotify=0x723BE0
#define PvPStatsWnd__vftable_x                                       0x9F6F20
#define PvPStatsWnd__WndNotification_x                               0x723BE0

// [MED dist=0x7C0] QuantityWnd  WndNotify=0x725D60
#define QuantityWnd__vftable_x                                       0x9F71A0
#define QuantityWnd__WndNotification_x                               0x725D60

// [HIGH dist=0x3D0] RaceChangeWnd  WndNotify=0x727AC0
#define RaceChangeWnd__vftable_x                                     0x9F7340
#define RaceChangeWnd__WndNotification_x                             0x727AC0

// [HIGH dist=0x240] RaidOptionsWindow  WndNotify=0x727EE0
#define RaidOptionsWindow__vftable_x                                 0x9F76C8
#define RaidOptionsWindow__WndNotification_x                         0x727EE0

// [LOW dist=0x2390] RaidWindow  WndNotify=0x72D180
#define RaidWindow__vftable_x                                        0x9F7B70
#define RaidWindow__WndNotification_x                                0x72D180

// [LOW dist=0x8E50] RealEstateItemsWnd  WndNotify=0x73CC50
#define RealEstateItemsWnd__vftable_x                                0x9F8348
#define RealEstateItemsWnd__WndNotification_x                        0x73CC50

// [MED dist=0x910] RealEstateLayoutDetailsWnd  WndNotify=0x5AD040
#define RealEstateLayoutDetailsWnd__vftable_x                        0x9D8E50
#define RealEstateLayoutDetailsWnd__WndNotification_x                0x5AD040

// [MED dist=0x1590] RealEstateManageWnd  WndNotify=0x745090
#define RealEstateManageWnd__vftable_x                               0x9F8668
#define RealEstateManageWnd__WndNotification_x                       0x745090

// [MED dist=0xE40] RealEstateNeighborhoodWnd  WndNotify=0x74A3A0
#define RealEstateNeighborhoodWnd__vftable_x                         0x9F8C90
#define RealEstateNeighborhoodWnd__WndNotification_x                 0x74A3A0

// [MED dist=0x960] RealEstatePlotSearchWnd  WndNotify=0x74C110 (RealEstatePurchaseWnd WndNotification is 0x74D500)
#define RealEstatePurchaseWnd__vftable_x                             0x9F8F10
#define RealEstatePurchaseWnd__WndNotification_x                     0x74D500

// [HIGH dist=0x250] RespawnWnd  WndNotify=0x74E7B0
#define RespawnWnd__vftable_x                                        0x9F9420
#define RespawnWnd__WndNotification_x                                0x74E7B0

// [HIGH dist=0xD0] SelectorWindow  WndNotify=0x751600
#define SelectorWindow__vftable_x                                    0x9F9B68
#define SelectorWindow__WndNotification_x                            0x751600

// [MED dist=0x5B0] SendMoneyWnd  WndNotify=0x753000
#define SendMoneyWnd__vftable_x                                      0x9F9D40
#define SendMoneyWnd__WndNotification_x                              0x753000

// [MED dist=0x8D0] ServerListWnd  WndNotify=0x753D00
#define ServerListWnd__vftable_x                                     0x9FA128
#define ServerListWnd__WndNotification_x                             0x753D00

// [MED dist=0x15E0] ShortDurationBuffWindow  WndNotify=0x6444F0
#define ShortDurationBuffWindow__vftable_x                           0x9E4988
#define ShortDurationBuffWindow__WndNotification_x                   0x6444F0

// [HIGH dist=0x170] SkillsSelectWindow  WndNotify=0x7567D0
#define SkillsSelectWindow__vftable_x                                0x9FA488
#define SkillsSelectWindow__WndNotification_x                        0x7567D0

// [HIGH dist=0x310] SkillsWindow  WndNotify=0x757380
#define SkillsWindow__vftable_x                                      0x9FA6D8
#define SkillsWindow__WndNotification_x                              0x757380

// [HIGH dist=0x370] SocialEditWnd  WndNotify=0x758C10
#define SocialEditWnd__vftable_x                                     0x9FA8D0
#define SocialEditWnd__WndNotification_x                             0x758C10

// [HIGH dist=0x400] SoulmarkWnd  WndNotify=0x759840
#define SoulmarkWnd__vftable_x                                       0x9FAB08
#define SoulmarkWnd__WndNotification_x                               0x759840

// [MED dist=0xB60] TaskSelectWnd  WndNotify=0x76B660
#define TaskSelectWnd__vftable_x                                     0x9FBAF8
#define TaskSelectWnd__WndNotification_x                             0x76B660

// [MED dist=0x9E0] TaskTemplateSelectWnd  WndNotify=0x76EA20
#define TaskTemplateSelectWnd__vftable_x                             0x9FBCC0
#define TaskTemplateSelectWnd__WndNotification_x                     0x76EA20

// [MED dist=0xD70] TaskWnd  WndNotify=0x7716B0
#define TaskWnd__vftable_x                                           0x9FC1C0
#define TaskWnd__WndNotification_x                                   0x7716B0

// [HIGH dist=0x370] TextEntryWnd  WndNotify=0x775070
#define TextEntryWnd__vftable_x                                      0x9FCB00
#define TextEntryWnd__WndNotification_x                              0x775070

// [MED dist=0x540] TipWindow  WndNotify=0x776FF0
#define TipWindow__vftable_x                                         0x9FCE40
#define TipWindow__WndNotification_x                                 0x776FF0

// [MED dist=0xB10] TitleWnd  WndNotify=0x7780B0
#define TitleWnd__vftable_x                                          0x9FD040
#define TitleWnd__WndNotification_x                                  0x7780B0

// [VERIFIED 2026-07-21] TrackingWnd -- real vptr written by the ctor (FUN_00779350:
// `*this = &PTR_009FD408`), NOT the 0x9FD470 the approx scan guessed. Slot 34 =
// WndNotification 0x779B00, slot 49 = OnProcessFrame 0x77A7F0. Full system: Section 42.
#define TrackingWnd__vftable_x                                       0x9FD408
#define TrackingWnd__WndNotification_x                               0x779B00
#define TrackingWnd__OnProcessFrame_x                                0x77A7F0

// [HIGH dist=0xC0] TradeWnd  WndNotify=0x77FE90
#define TradeWnd__vftable_x                                          0x9FD998
#define TradeWnd__WndNotification_x                                  0x77FE90

// [MED dist=0x10B0] TributeBenefitWnd  WndNotify=0x784AB0
#define TributeBenefitWnd__vftable_x                                 0x9FDD10
#define TributeBenefitWnd__WndNotification_x                         0x784AB0

// [MED dist=0x940] TributeMasterWnd  WndNotify=0x786800
#define TributeMasterWnd__vftable_x                                  0x9FE070
#define TributeMasterWnd__WndNotification_x                          0x786800

// [MED dist=0xFA0] TributeTrophyWnd  WndNotify=0x789170
#define TributeTrophyWnd__vftable_x                                  0x9FE3E8
#define TributeTrophyWnd__WndNotification_x                          0x789170

// [HIGH dist=0x350] VideoModesWindow  WndNotify=0x78A370
#define VideoModesWindow__vftable_x                                  0x9FE7D8
#define VideoModesWindow__WndNotification_x                          0x78A370

// [HIGH dist=0x330] VoiceMacroWnd  WndNotify=0x5F1CE0
#define VoiceMacroWnd__vftable_x                                     0x9DBE48
#define VoiceMacroWnd__WndNotification_x                             0x5F1CE0

// [HIGH dist=0x2B0] VoteResultsWnd  WndNotify=0x793B30
#define VoteResultsWnd__vftable_x                                    0x9FFE00
#define VoteResultsWnd__WndNotification_x                            0x793B30

// [MED dist=0xCA0] VoteWnd  WndNotify=0x794E90
#define VoteWnd__vftable_x                                           0xA00138
#define VoteWnd__WndNotification_x                                   0x794E90

// [HIGH dist=0x90] ZonePathWnd  WndNotify=0x607330
#define ZonePathWnd__vftable_x                                       0x9DE2D0
#define ZonePathWnd__WndNotification_x                               0x607330





/////
// Section 8: Deep Scan Results - Network, Rendering, Chat, Struct Offsets
// Source: EQDeepScan.java - pattern analysis and XREF tracing
/////

// ============================================================
// CEverQuest object addresses (confirmed by MOV [pinstCEverQuest_x] in FUN_005fee00)
// ============================================================
// FUN_005fee00 initializes pinstCEverQuest_x (0xE67CCC) and many CEverQuest members
// This is CEverQuest::CEverQuest() constructor or top-level init function
#define CEverQuest__CEverQuest_x                                     0x5FEE00

// ============================================================
// PlayerZoneClient / Spawn Struct Member Offsets
// Source: FLD [ESI+offset] patterns in PlayerZoneClient::ChangeHeight (0x5A4EC0)
// These are byte offsets from the start of the PlayerZoneClient/EQPlayer struct
// ============================================================
#define PlayerZoneClient__SpeedMultiplier_offset                     0x13C   // FLD [ESI+0x13C] in ChangeHeight
#define PlayerZoneClient__Height_offset                              0x31C   // FLD [ESI+0x31C] in ChangeHeight

// ============================================================
// UI Color Table
// Source: FUN_0051ce00 stores 18 sequential 4-byte color values to 0xE15E50
// This is the CONCOLOR / UI chat color lookup table
// ============================================================
#define g_UIColorTable_x                                             0xE15E50  // 18 DWORD entries
#define UIColor_Table_Func_x                                         0x51CE00  // initializer

// ============================================================
// UdpConnection Virtual Functions (vtable @ 0xA1E0B8)
// vfunc3 (0x54CB90) is in main game code range = EQ's UDP send wrapper
// ============================================================
#define UdpConnection__vfunc0_x                                      0x8CC4E0  // destructor
#define UdpConnection__vfunc1_x                                      0x8CC500  // scalar destructor
#define UdpConnection__vfunc2_x                                      0x8C2A80  // vtable[2]
#define UdpConnection__vfunc3_x                                      0x54CB90  // vtable[3] - EQ game code wrapper (SEND?)
#define UdpConnection__vfunc4_x                                      0x8C4500  // vtable[4]
#define UdpConnection__vfunc6_x                                      0x8C6EE0  // vtable[6]
#define UdpConnection__vfunc7_x                                      0x8DC76B  // vtable[7] - null stub
#define UdpConnection__vfunc8_x                                      0x8DC76B  // vtable[8] - null stub
#define UdpConnection__vfunc9_x                                      0x8DC76B  // vtable[9] - null stub

// ============================================================
// TcpConnection Virtual Functions (vtable @ 0xA27B84)
// ============================================================
#define TcpConnection__vfunc0_x                                      0x8FEAF0  // destructor
#define TcpConnection__vfunc1_x                                      0x8FD690  // vtable[1]

// ============================================================
// CResolutionHandlerBase vtable (from RTTI scan)
// Rendering subsystem base - may lead to CRender methods
// ============================================================
#define CResolutionHandlerBase__vftable_x                            0xA1F2E0

// ============================================================
// CEverQuest::dsp_chat callee chain (functions called by dsp_chat @ 0x51F1A0)
// Tracing this chain reveals the full chat display pipeline
// ============================================================
// 0x860130 - CChatManager or display buffer function
// 0x805B20 - string formatting / buffer function
// 0x8061C0 - string processing function
// 0x664AB0 - game-code chat function (CChatWindow method?)
// 0x809B30 - string/formatting utility
// 0x8CF360 - UI/rendering function
// 0x51B600 - CEverQuest::DoPercentConvert (already defined)
// 0x4ED580 - unknown game function
// 0x8DBB3B - runtime lib function
// 0x5130C0 - CEverQuest::OutputTextToLog (already defined)
// 0x805C20 - string function
// 0x4ED110 - unknown game function
// 0x650F90 - CChatWindow chat-add function?
#define dsp_chat__callee_ChatMgr_x                                   0x860130  // likely CChatManager display
#define dsp_chat__callee_GameChat_x                                  0x664AB0  // game-code chat processor
#define dsp_chat__callee_ChatWindow_x                                0x650F90  // CChatWindow method (chat-add)
#define dsp_chat__callee_Unknown1_x                                  0x4ED580  // unknown game function
#define dsp_chat__callee_Unknown2_x                                  0x4ED110  // unknown game function

// ============================================================
// Key unknown globals found via MOV [global] scan
// These are frequently-written global variables of interest
// ============================================================
// 0xDD2644 - written by CharacterZoneClient::CastSpell and spawn code - likely a shared state ptr
// 0xDD2648 - written by packet handlers and BarterMerchantWnd - likely a session/packet ptr
// 0xDE0878 - written at startup, possibly renderer or audio state
// 0xE67880 - written by many major functions (FUN_004c3250, FUN_005fee00) = CEverQuest object?
// 0xE64488, 0xE641F8 - written by FUN_0049b200 near pinstSpawnManager - spawn system data
// 0xE64748 - written by spawn init code - spawn-related pointer
//
// Candidate: CEverQuest object base (the OBJECT at the pointer pinstCEverQuest_x points to):
#define pCEverQuestObject_x                                          0xE67880  // CEverQuest* dereferenced value (unconfirmed)
//
// Candidate: Important CharacterZoneClient-related globals:
#define gCastState_x                                                 0xDD2644  // cast state / shared ptr (unconfirmed)
#define gPacketState_x                                               0xDD2648  // packet/session state (unconfirmed)


/////
// Section 9: String Anchors, Command Table, Widget Names, Serialization Keys
// Source: Raw PE string scan (42,022 strings) + discovered_offsets.md
// These are READ-ONLY DATA (.rdata) and DATA (.data) section addresses
// USAGE:
//   - Command strings: anchor for hooking the /cmd dispatch table
//   - Widget name strings: pass to CXWnd::GetChildByID() to find child controls
//   - Serialization strings: match against packet/save data field names
//   - Debug strings: anchor for finding functions via XREF in Ghidra
//   - RTTI descriptors: confirm class identity at runtime
/////

// ============================================================
// RealEstate System - Slash Command Strings (.rdata)
// These are the literal strings used by CEverQuest::InterpretCmd dispatch
// ============================================================
#define s_cmd_realestate_x                                           0x9CB800  // "/realestate"
#define s_cmd_leaverealestate_x                                      0x9CBE80  // "/leaverealestate"
#define s_token_realestate_x                                         0x9CEF3C  // "realestate" (token)

// ============================================================
// RealEstate System - Key Bind Command Name Strings (.rdata)
// Pass to keybind registry to find/hook key bindings
// ============================================================
#define s_CMD_REAL_ESTATE_LEAVE_x                                    0x9C1A30  // "CMD_REAL_ESTATE_LEAVE"
#define s_CMD_REAL_ESTATE_HELP_WINDOW_x                              0x9C1CF4  // "CMD_REAL_ESTATE_HELP_WINDOW"
#define s_CMD_REAL_ESTATE_PLOT_SEARCH_WINDOW_x                       0x9C1D10  // "CMD_REAL_ESTATE_PLOT_SEARCH_WINDOW"
#define s_CMD_REAL_ESTATE_TROPHIES_WINDOW_x                          0x9C1D34  // "CMD_REAL_ESTATE_TROPHIES_WINDOW"
#define s_CMD_REAL_ESTATE_MANAGE_WINDOW_x                            0x9C1D54  // "CMD_REAL_ESTATE_MANAGE_WINDOW"
#define s_CMD_REAL_ESTATE_ITEMS_WINDOW_x                             0x9C1D74  // "CMD_REAL_ESTATE_ITEMS_WINDOW"
#define s_CMD_REAL_ESTATE_ITEM_PLACEMENT_ROTATE_90_x                 0x9C1A48  // "CMD_REAL_ESTATE_ITEM_PLACEMENT_ROTATE_ITEM_WHEEL_ROTATION_90_DEGREES"
#define s_CMD_REAL_ESTATE_ITEM_PLACEMENT_MODE_FREE_x                 0x9C1C04  // "CMD_REAL_ESTATE_ITEM_PLACEMENT_SET_MODE_FREE"
#define s_CMD_REAL_ESTATE_ITEM_PLACEMENT_MODE_CURSOR_x               0x9C1C34  // "CMD_REAL_ESTATE_ITEM_PLACEMENT_SET_MODE_CURSOR"
#define s_CMD_REAL_ESTATE_ITEM_PLACEMENT_MODE_WALL_x                 0x9C1C64  // "CMD_REAL_ESTATE_ITEM_PLACEMENT_SET_MODE_WALL"
#define s_CMD_REAL_ESTATE_ITEM_PLACEMENT_MODE_COLLISION_x            0x9C1C94  // "CMD_REAL_ESTATE_ITEM_PLACEMENT_SET_MODE_COLLISION"
#define s_CMD_REAL_ESTATE_ITEM_PLACEMENT_TOGGLE_MODE_x               0x9C1CC8  // "CMD_REAL_ESTATE_ITEM_PLACEMENT_TOGGLE_MODE"

// ============================================================
// RealEstate System - Serialization Field Name Strings (.rdata)
// These appear in packet data and save files - match against incoming data
// ============================================================
#define s_field_OwnedItemRealEstatesList_x                           0xA02624  // "OwnedItemRealEstatesList="
#define s_field_OwnedItemRealEstatesCount_x                          0xA02640  // "OwnedItemRealEstatesCount=%d"
#define s_field_OwnedRealEstatesList_x                               0xA02660  // "OwnedRealEstatesList="
#define s_field_OwnedRealEstatesCount_x                              0xA02678  // "OwnedRealEstatesCount=%d"
#define s_field_RealEstateZoneId_x                                   0xA026C8  // "RealEstateZoneId=%d"
#define s_field_RealEstateLocation_x                                 0x9D9178  // "RealEstateLocation"
#define s_field_RealEstateName_x                                     0x9D918B  // "RealEstateName"
#define s_field_RealEstateSaveFile_x                                 0x9D91C0  // "%s-RealEstate.txt" (per-char save)

// ============================================================
// RealEstate System - Data File Name Strings (.rdata)
// Client reads these files from the EQ data directory at startup
// ============================================================
#define s_file_RealEstateDefs_x                                      0xA100B8  // "RealEstateDefs.txt"
#define s_file_RealEstateDefRel_x                                    0xA100CC  // "RealEstateDefRel.txt"
#define s_file_RealEstateErrors_x                                    0xA101A0  // "RealEstateErrors.txt"
#define s_file_RealEstateCosts_x                                     0xA10418  // "RealEstateCosts.txt"
#define s_file_RealEstate_x                                          0xA1042C  // "RealEstate.txt"
#define s_file_RealEstateItemGroups_x                                0xA10464  // "RealEstateItemGroups.txt"

// ============================================================
// RealEstate Items Window - UI Widget Name Strings (.rdata)
// Pass these to CXWnd::GetChildByID() to find specific child controls
// Used for reading/writing housing UI state
// ============================================================
#define s_widget_HOUSE_x                                             0x9F7D60  // "HOUSE" (tab name)
#define s_widget_House_x                                             0x9F7D68  // "House"
#define s_widget_HouseStored_x                                       0x9F7DA8  // "House - Stored"
#define s_widget_HousePlaced_x                                       0x9F7DB8  // "House - Placed"
#define s_widget_YardAndHouse_x                                      0x9F7E28  // "Yard and House"
#define s_widget_REIW_LayoutLoadHouse_Btn_x                          0x9F7F0C  // "REIW_LayoutLoadHouse_Btn"
#define s_widget_REIW_LayoutSaveHouse_Btn_x                          0x9F7F58  // "REIW_LayoutSaveHouse_Btn"
#define s_widget_REIW_ShowHouseButton_x                              0x9F8178  // "REIW_ShowHouseButton"
#define s_widget_REIW_HouseNonRealEstateItemsCount_x                 0x9F824C  // "REIW_HouseNonRealEstateItemsCount"
#define s_widget_REIW_HouseItemsCount_x                              0x9F8270  // "REIW_HouseItemsCount"
#define s_widget_REIW_YardNonRealEstateItemsCount_x                  0x9F82A0  // "REIW_YardNonRealEstateItemsCount"
#define s_widget_REIW_MovingCrateNonRealEstateItemsCount_x           0x9F8208  // "REIW_MovingCrateNonRealEstateItemsCount"
#define s_widget_House_label_x                                       0x9F8498  // " House" (list entry label)
#define s_widget_AddHouse_x                                          0x9F84E8  // "Add House"
#define s_widget_RemoveHouse_x                                       0x9F84F4  // "Remove House"
#define s_widget_PlayerHouse_x                                       0xA10098  // "Player House"
#define s_widget_FittingHouses_x                                     0x9C975C  // "Fitting houses into backpacks"

// ============================================================
// RealEstate System - Debug / Assert String Anchors (.rdata)
// These are embedded assertion strings that identify exact functions
// Use as XREF anchors in Ghidra to find the containing function
// ============================================================
// "RealEstateManagerClient.cpp:1763 RealEstateManagerClient::SetRequestPending()"
// -> XREF from this string leads directly to RealEstateManagerClient::SetRequestPending()
#define s_dbg_RealEstateMgr_SetRequestPending_x                      0x9D7D90
// -> To find the function: getFunctionContaining(getXRefsTo(0x9D7D90)[0])

// ============================================================
// RealEstate System - Window Class Registration Strings (.data)
// These are the class name strings used during window factory registration
// The .data addresses (0xB5xxxx) are where these strings live at runtime
// ============================================================
#define s_wndclass_RealEstateItemsWnd_x                              0xB51DA4  // "RealEstateItemsWnd"
#define s_wndclass_RealEstateManageWnd_x                             0xB51DFC  // "RealEstateManageWnd"
#define s_wndclass_RealEstateNeighborhoodWnd_x                       0xB51E54  // "RealEstateNeighborhoodWnd"
#define s_wndclass_RealEstatePurchaseWnd_x                           0xB51EAC  // "RealEstatePurchaseWnd"
#define s_wndclass_RealEstatePlotSearchWnd_x                         0xB51F04  // "RealEstatePlotSearchWnd"
#define s_wndclass_RealEstateOverviewWnd_x                           0xB51FB4  // "RealEstateOverviewWnd"
#define s_wndclass_RealEstateLayoutDetailsWnd_x                      0xB520BC  // "RealEstateLayoutDetailsWnd"

// ============================================================
// RealEstate System - HTML Template Strings (.data)
// Used by the window system to load the SIDL XML/HTML for each window
// ============================================================
#define s_html_RealEstateManage_x                                    0xB51DCC  // "ctxrealestatemanage.html"
#define s_html_RealEstateNeighborhood_x                              0xB51E24  // "ctxrealestateneighborhood.html"
#define s_html_RealEstatePurchase_x                                  0xB51E7C  // "ctxrealestatepurchase.html"
#define s_html_RealEstatePlotSearch_x                                0xB51ED4  // "ctxrealestateplotsearch.html"
#define s_html_RealEstateOverview_x                                  0xB51F84  // "ctxrealestateoverview.html"
#define s_html_RealEstateLayout_x                                    0xB5208C  // "ctxrealestatelayout.html"

// ============================================================
// RealEstate System - .rdata Window Name Strings
// Used by CSidlManager to look up screen templates by name
// ============================================================
#define s_sidl_RealEstateLayoutDetailsWnd_x                          0x9D8F64  // "RealEstateLayoutDetailsWnd"
#define s_sidl_RealEstateItemsWnd_x                                  0x9F845C  // "RealEstateItemsWnd"
#define s_sidl_RealEstateManageWnd_x                                 0x9F8AA4  // "RealEstateManageWnd"
#define s_sidl_RealEstatePurchaseWnd_x                               0x9F937C  // "RealEstatePurchaseWnd"
#define s_sidl_RealEstateNeighborhoodWnd_x                           0x9F8DA4  // "RealEstateNeighborhoodWnd"
#define s_sidl_RealEstatePlotSearchWnd_x                             0x9F9024  // "RealEstatePlotSearchWnd"

// ============================================================
// RealEstate System - RTTI Type Descriptor (.data)
// C++ RTTI mangled name - confirms class exists and vtable is valid
// ============================================================
#define s_rtti_RealEstate_x                                          0xB61794  // ".?AVRealEstate@@" - RTTI type descriptor

// ============================================================
// UI Label Strings - General Real Estate Labels
// ============================================================
#define s_label_RealEstate_x                                         0x9CA488  // "Real Estate" (UI label)


/////
// Section 10: Comprehensive Decompiler Analysis Results
// Source: EQComprehensiveScan.java - Ghidra AST analysis of key functions
// Generated: 2026-07-05
/////

// ============================================================
// CSpawnManager Init (CONFIRMED from pattern scan)
// MOV [pinstSpawnManager_x], EAX at 0x5361BA inside this function
// ============================================================
#define CSpawnManager__Init_x                                        0x5338A0

// ============================================================
// Struct Member Offsets - CharacterZoneClient
// Source: AST PTRSUB/PTRADD analysis of Cur_HP (0x449E00) and Cur_Mana (0x4442E0)
// These are byte offsets accessed within those functions
// 0x2E4 from Cur_HP is the most likely HP value field (largest unique offset)
// 0x6C  from Cur_Mana is the likely mana value field
// ============================================================
#define CharacterZoneClient__offset_0x8                              0x8    // accessed in Cur_HP
#define CharacterZoneClient__offset_0xD                              0xD    // accessed in Cur_HP / Cur_Mana
#define CharacterZoneClient__offset_0x24                             0x24   // accessed in Cur_HP / Cur_Mana (flags?)
#define CharacterZoneClient__HP_field_offset                         0x2E4  // [BEST CANDIDATE] HP value (from Cur_HP AST)
#define CharacterZoneClient__Mana_field_offset                       0x6C   // [BEST CANDIDATE] Mana value (from Cur_Mana AST)

// ============================================================
// Struct Member Offsets - PlayerZoneClient (GetLevel + ChangeHeight + DoAttack)
// Source: AST PTRSUB analysis of GetLevel (0x59FC90) and ChangeHeight (0x5A4EC0)
// ============================================================
#define PlayerZoneClient__Level_offset                               0x125  // Level byte field (from GetLevel AST)
#define PlayerZoneClient__Level_offset2                              0x250  // secondary level reference
#define PlayerZoneClient__offset_0x2CC                               0x2CC  // accessed in GetLevel
// Position-related offsets (from ChangeHeight AST):
#define PlayerZoneClient__offset_0x3C                                0x3C   // possible Y or Z float coord
#define PlayerZoneClient__offset_0x48                                0x48   // possible X or Y float coord
// 0x13C and 0x31C already defined in Section 8 (SpeedMultiplier, Height)
#define PlayerZoneClient__offset_0x101C                              0x101C // large offset accessed in ChangeHeight

// ============================================================
// Struct Member Offsets - PlayerZoneClient Combat (DoAttack 0x5A5000)
// Many combat-related fields accessed
// ============================================================
#define PlayerZoneClient__CombatField_0x29                           0x29   // possibly "in combat" flag
#define PlayerZoneClient__CombatField_0x44                           0x44   // possibly target spawn ID
#define PlayerZoneClient__CombatField_0x62                           0x62   // combat byte flag
#define PlayerZoneClient__CombatField_0x87                           0x87   // combat byte
#define PlayerZoneClient__CombatField_0x8E                           0x8E   // combat byte
#define PlayerZoneClient__CombatField_0xA2                           0xA2   // combat field
#define PlayerZoneClient__CombatField_0xB3                           0xB3   // combat byte
#define PlayerZoneClient__CombatField_0xC8                           0xC8   // combat dword
#define PlayerZoneClient__CombatField_0xD0                           0xD0   // combat dword
#define PlayerZoneClient__CombatField_0x182                          0x182  // combat field
#define PlayerZoneClient__CombatField_0x188                          0x188  // combat field
#define PlayerZoneClient__CombatField_0x1D4                          0x1D4  // combat dword
#define PlayerZoneClient__CombatField_0x251                          0x251  // combat byte (level?)

// ============================================================
// Struct Member Offsets - CEverQuest IssuePetCommand (0x5209B0)
// ============================================================
#define CEverQuest__PetCmd_offset_0x44                               0x44   // pet-related field (target?)
#define CEverQuest__PetCmd_offset_0x148                              0x148  // pet command field
#define CEverQuest__PetCmd_offset_0x168                              0x168  // pet command field
#define CEverQuest__PetCmd_offset_0x38C                              0x38C  // pet command field

// ============================================================
// Struct Member Offsets - CGroupWnd WndNotification (0x67C620)
// Group member data is stored at 0x220-0x230 range in the window struct
// 0x3A4-0x3A7 are likely bool flags for each of 4 group member slots
// ============================================================
#define CGroupWnd__GroupMemberArray_offset                           0x220  // GroupMember[0] start
#define CGroupWnd__GroupMemberArray_end_offset                       0x230  // GroupMember array end (6 entries * 4 bytes)
#define CGroupWnd__GroupFlag_0x3A4                                   0x3A4  // group member flag byte
#define CGroupWnd__GroupFlag_0x3A5                                   0x3A5  // group member flag byte
#define CGroupWnd__GroupFlag_0x3A6                                   0x3A6  // group member flag byte
#define CGroupWnd__GroupFlag_0x3A7                                   0x3A7  // group member flag byte

// ============================================================
// Struct Member Offsets - CTargetWnd WndNotification (0x769AC0)
// Target entity data offsets within CTargetWnd
// ============================================================
#define CTargetWnd__offset_0x5C                                      0x5C   // target field
#define CTargetWnd__offset_0x94                                      0x94   // target field (HP%?)
#define CTargetWnd__offset_0x98                                      0x98   // target field
#define CTargetWnd__offset_0xDC                                      0xDC   // target field
#define CTargetWnd__offset_0x174                                     0x174  // target field
#define CTargetWnd__offset_0x210                                     0x210  // target field
#define CTargetWnd__offset_0x85C                                     0x85C  // target field (spawn struct?)
#define CTargetWnd__offset_0x860                                     0x860  // target field
#define CTargetWnd__offset_0x864                                     0x864  // target field
#define CTargetWnd__offset_0x868                                     0x868  // target field

// ============================================================
// CEverQuest Constructor Key Callees (0x5FEE00)
// These are the major subsystems initialized during game startup
// ============================================================
#define CEverQuest__ctor_Network_init_x                              0x8C4B60  // UDP/network subsystem init
#define CEverQuest__ctor_Audio_init_x                                0x8C7490  // audio subsystem init
#define CEverQuest__ctor_SpawnMgr_init_x                             0x5338A0  // CSpawnManager ctor (same as above)
#define CEverQuest__ctor_FontEnum_x                                  0x5FA2D0  // font enumeration
#define CEverQuest__ctor_Init2_x                                     0x5FB440  // secondary init (stores E67xxx globals)
#define CEverQuest__ctor_Init3_x                                     0x5FC850  // tertiary init (E67B28-E67B38)
#define CEverQuest__ctor_Init4_x                                     0x5FC340  // quaternary init
#define CEverQuest__ctor_UI_init_x                                   0x808E50  // UI subsystem init (called from ctor)
#define CEverQuest__ctor_Game_init_x                                 0x4209E0  // game subsystem init
// NOTE: DirectInput8Create is called at 0x79987C

// ============================================================
// Chat System - Internal Functions (from dsp_chat call tree)
// ============================================================
#define CChatManager__Display_x                                      0x860130  // CChatManager display function
#define CChatManager__GetChatManager_x                               0x8051A0  // likely CChatManager::GetInstance
#define CChatManager__FormatAndSend_x                                0x806580  // format + send to chat
#define CChatManager__RenderLine_x                                   0x806FE0  // render one chat line
#define CChatManager__GetLineColor_x                                 0x805F40  // get line color
#define CChatManager__Scrollback_x                                   0x806190  // scrollback handler
#define CChatManager__ChatCallback_x                                 0x807BF0  // chat input callback
#define CChatWindow__AddGameChat_x                                   0x4FEEE0  // game-code chat add (from 0x860130)
// dsp_chat internal buffer size = 0x800 (2048 bytes)
#define CEverQuest__dsp_chat_bufsize                                 0x800

// ============================================================
// Slash Command String Addresses (.rdata)
// These are the exact string bytes for each registered slash command
// Use as anchors to find the command dispatch table via XREF
// ============================================================
#define s_cmd_camp_x                                                 0x9C5BD6  // "/camp"
#define s_cmd_who_x                                                  0x9C63C3  // "/who"
#define s_cmd_say_x                                                  0x9CBA6E  // "/say"
#define s_cmd_shout_x                                                0x9CC74D  // "/shout"
#define s_cmd_tell_x                                                 0x9CBA1E  // "/tell"
#define s_cmd_sit_x                                                  0x9C42E5  // "/sit"
#define s_cmd_stand_x                                                0x9CBF5D  // "/stand"
#define s_cmd_duck_x                                                 0x9CC2A9  // "/duck"
#define s_cmd_target_x                                               0x9C3FB8  // "/target"
#define s_cmd_assist_x                                               0x9CB94D  // "/assist"
#define s_cmd_consider_x                                             0x9CC859  // "/consider"
#define s_cmd_cast_x                                                 0x9C3409  // "/cast"

// ============================================================
// Global String Tables Found
// ============================================================
#define RaceNameTable_x                                              0x9C7498  // "Human","Barbarian"... race name cluster
#define SpellData_x                                                  0xA0ABC4  // "Burn","Frost"... spell category strings

// ============================================================
// Direct3D Hook Offsets (IDirect3DDevice9 vtable)
// d3d9.dll is loaded dynamically at runtime - use these vtable slot offsets
// to hook rendering by patching the device vtable after creation
// ============================================================
// IDirect3DDevice9 vtable layout (each slot = 4 bytes on x86)
#define IDirect3DDevice9__Reset_vtable_offset                        0x40   // slot 16 * 4
#define IDirect3DDevice9__Present_vtable_offset                      0x44   // slot 17 * 4  <- hook here for render overlay
#define IDirect3DDevice9__EndScene_vtable_offset                     0xA8   // slot 42 * 4  <- hook here for draw overlay

// ============================================================
// Spawn System - Functions Accessing pinstLocalPlayer
// (Candidates for GetItem, inventory, and stat accessors)
// Short functions (< 0x30 bytes) are likely simple accessors
// ============================================================
// Key functions that access pinstLocalPlayer_x directly:
#define FUN_accessing_LocalPlayer_0x423F00_x                         0x423F00
#define FUN_accessing_LocalPlayer_0x424120_x                         0x424120
#define FUN_accessing_LocalPlayer_0x425D30_x                         0x425D30
#define FUN_accessing_LocalPlayer_0x42F180_x                         0x42F180
#define FUN_accessing_LocalPlayer_0x42F9F0_x                         0x42F9F0
#define FUN_accessing_LocalPlayer_0x42FEC0_x                         0x42FEC0
#define FUN_accessing_LocalPlayer_0x4348A0_x                         0x4348A0
#define FUN_accessing_LocalPlayer_0x439700_x                         0x439700
#define FUN_accessing_LocalPlayer_0x47C430_x                         0x47C430
#define FUN_accessing_LocalPlayer_0x47D410_x                         0x47D410
#define FUN_accessing_LocalPlayer_0x4B4850_x                         0x4B4850
#define FUN_accessing_LocalPlayer_0x4BD5F0_x                         0x4BD5F0

// ============================================================
// Indexed Global Tables (potential spell/item/zone arrays)
// Source: MOV EAX, [ESI*4+global] pattern scan
// ============================================================
#define g_IndexedTable_DE0D64_x                                      0xDE0D64  // indexed in FUN_0049ca10 (window factory area)
#define g_IndexedTable_F73854_x                                      0xF73854  // indexed in FUN_004d7230 (large init)
#define g_IndexedTable_E15E78_x                                      0xE15E78  // indexed in FUN_00659c80 (UI/spell effect?)
#define g_IndexedTable_DCF148_x                                      0xDCF148  // indexed in FUN_004eeca0


// ============================================================
// Section 11: Client Slash Commands Table Handlers
// Base Address: 0xACD864 (28 bytes per entry)
// Structure layout: ID (4B), NamePtr (4B), DescPtr (4B), HandlerPtr (4B)...
// ============================================================
#define g_CommandTable_x                                             0xACD864
#define g_CommandTable_EntrySize                                     28

#define cmd_tell_handler_x                                 0x004E1DC0  // Command: /tell (ID: 13974)
#define cmd_msg_handler_x                                  0x004E1DC0  // Command: /msg (ID: 13975)
#define cmd_send_handler_x                                 0x004E1DC0  // Command: /send (ID: 13976)
#define cmd_ttell_handler_x                                0x004E1E00  // Command: /ttell (ID: 13977)
#define cmd_gsay_handler_x                                 0x004E1C60  // Command: /gsay (ID: 13978)
#define cmd_shout_handler_x                                0x004E1C90  // Command: /shout (ID: 13979)
#define cmd_auction_handler_x                              0x004E1D60  // Command: /auction (ID: 13980)
#define cmd_ooc_handler_x                                  0x004E1D90  // Command: /ooc (ID: 13981)
#define cmd_reply_handler_x                                0x004E2080  // Command: /reply (ID: 13982)
#define cmd_channel_handler_x                              0x004F2AE0  // Command: /channel (ID: 13983)
#define cmd_guildsay_handler_x                             0x004DF5F0  // Command: /guildsay (ID: 13984)
#define cmd_fsay_handler_x                                 0x004DF610  // Command: /fsay (ID: 4587)
#define cmd_facepick_handler_x                             0x004EBC80  // Command: /facepick (ID: 14200)
#define cmd_guildinvite_handler_x                          0x004DF4F0  // Command: /guildinvite (ID: 13985)
#define cmd_guildremove_handler_x                          0x004DF580  // Command: /guildremove (ID: 13986)
#define cmd_guilddelete_handler_x                          0x004DF5A0  // Command: /guilddelete (ID: 13987)
#define cmd_guildstatus_handler_x                          0x004DF5B0  // Command: /guildstatus (ID: 13988)
#define cmd_guildleader_handler_x                          0x004DF5D0  // Command: /guildleader (ID: 13989)
#define cmd_guildwar_handler_x                             0x004DF440  // Command: /guildwar (ID: 13990)
#define cmd_guildpeace_handler_x                           0x004DF4B0  // Command: /guildpeace (ID: 13991)
#define cmd_guildmotd_handler_x                            0x004F1310  // Command: /guildmotd (ID: 14191)
#define cmd_getguildmotd_handler_x                         0x004F14D0  // Command: /getguildmotd (ID: 5120)
#define cmd_guildcreate_handler_x                          0x004E3350  // Command: /guildcreate (ID: 4294967295)
#define cmd_guildpromote_handler_x                         0x004DF500  // Command: /guildpromote (ID: 4294967295)
#define cmd_guilddemote_handler_x                          0x004DF510  // Command: /guilddemote (ID: 4294967295)
#define cmd_pet_handler_x                                  0x004E2150  // Command: /pet (ID: 13992)
#define cmd_petition_handler_x                             0x004E2CE0  // Command: /petition (ID: 13993)
#define cmd_survey_handler_x                               0x00478B70  // Command: /survey (ID: 13993)
#define cmd_private_handler_x                              0x004E5080  // Command: /private (ID: 13994)
#define cmd_find_handler_x                                 0x004F4250  // Command: /find (ID: 13995)
#define cmd_goto_handler_x                                 0x004F5780  // Command: /goto (ID: 13996)
#define cmd_becomenpc_handler_x                            0x004F6C30  // Command: /becomenpc (ID: 13997)
#define cmd_searchcorpse_handler_x                         0x004E50C0  // Command: /searchcorpse (ID: 13998)
#define cmd_yell_handler_x                                 0x004E1CC0  // Command: /yell (ID: 13999)
#define cmd_motd_handler_x                                 0x004F1290  // Command: /motd (ID: 14000)
#define cmd_corpse_handler_x                               0x00478F40  // Command: /corpse (ID: 14001)
#define cmd_corpsedrag_handler_x                           0x00478F60  // Command: /corpsedrag (ID: 4068)
#define cmd_corpsedrop_handler_x                           0x004781B0  // Command: /corpsedrop (ID: 4069)
#define cmd_me_handler_x                                   0x004DFC60  // Command: /me (ID: 14002)
#define cmd_location_handler_x                             0x004F5B00  // Command: /location (ID: 14003)
#define cmd_rewind_handler_x                               0x004E6D00  // Command: /rewind (ID: 4294967295)
#define cmd_melody_handler_x                               0x004F5BF0  // Command: /melody (ID: 4085)
#define cmd_adriver_handler_x                              0x004E0270  // Command: /adriver (ID: 14004)
#define cmd_sdriver_handler_x                              0x004E0270  // Command: /sdriver (ID: 14005)
#define cmd_chat_handler_x                                 0x004F2AD0  // Command: /chat (ID: 14006)
#define cmd_join_handler_x                                 0x004EBE70  // Command: /join (ID: 14007)
#define cmd_leave_handler_x                                0x004EBF10  // Command: /leave (ID: 14008)
#define cmd_leaveall_handler_x                             0x004EBF80  // Command: /leaveall (ID: 14009)
#define cmd_list_handler_x                                 0x004EBFA0  // Command: /list (ID: 14010)
#define cmd_announce_handler_x                             0x004EC010  // Command: /announce (ID: 14011)
#define cmd_1_handler_x                                    0x004EC260  // Command: /1 (ID: 14012)
#define cmd_2_handler_x                                    0x004EC270  // Command: /2 (ID: 14013)
#define cmd_3_handler_x                                    0x004EC280  // Command: /3 (ID: 14014)
#define cmd_4_handler_x                                    0x004EC290  // Command: /4 (ID: 14015)
#define cmd_5_handler_x                                    0x004EC2A0  // Command: /5 (ID: 14016)
#define cmd_6_handler_x                                    0x004EC2B0  // Command: /6 (ID: 14017)
#define cmd_7_handler_x                                    0x004EC2C0  // Command: /7 (ID: 14018)
#define cmd_8_handler_x                                    0x004EC2D0  // Command: /8 (ID: 14019)
#define cmd_9_handler_x                                    0x004EC2E0  // Command: /9 (ID: 14020)
#define cmd_10_handler_x                                   0x004EC2F0  // Command: /10 (ID: 14021)
#define cmd_autojoin_handler_x                             0x004EBDE0  // Command: /autojoin (ID: 14022)
#define cmd_keys_handler_x                                 0x004E4FF0  // Command: /keys (ID: 7522)
#define cmd_mp3_handler_x                                  0x004F5340  // Command: /mp3 (ID: 7530)
#define cmd_map_handler_x                                  0x004E5960  // Command: /map (ID: 15042)
#define cmd_claim_handler_x                                0x004E5CD0  // Command: /claim (ID: 3346)
#define cmd_indicator_handler_x                            0x005C5530  // Command: /indicator (ID: 4058)
#define cmd_guidehelp_handler_x                            0x004E4F70  // Command: /guidehelp (ID: 3554)
#define cmd_stopcast_handler_x                             0x004E8970  // Command: /stopcast (ID: 3557)
#define cmd_netstats_handler_x                             0x004E6A70  // Command: /netstats (ID: 4526)
#define cmd_bandolier_handler_x                            0x004DECD0  // Command: /bandolier (ID: 9305)
#define cmd_potionbelt_handler_x                           0x004DEBE0  // Command: /potionbelt (ID: 9355)
#define cmd_veteranReward_handler_x                        0x004E5C90  // Command: /veteranReward (ID: 4294967295)
#define cmd_stopdisc_handler_x                             0x00478CB0  // Command: /stopdisc (ID: 4294967295)
#define cmd_useitem_handler_x                              0x00479200  // Command: /useitem (ID: 4294967295)
#define cmd_testbuffme_handler_x                           0x004DE310  // Command: /testbuffme (ID: 4294967295)
#define cmd_beta_handler_x                                 0x004E5CF0  // Command: /beta (ID: 4294967295)
#define cmd_betabuff_handler_x                             0x004E5F00  // Command: /betabuff (ID: 4294967295)
#define cmd_deny_handler_x                                 0x004E7480  // Command: /deny (ID: 9506)
#define cmd_emoteworld_handler_x                           0x004E8100  // Command: /emoteworld (ID: 5857)
#define cmd_system_handler_x                               0x00478360  // Command: /system (ID: 4097)
#define cmd_url_handler_x                                  0x00478520  // Command: /url (ID: 4099)
#define cmd_www_handler_x                                  0x00478420  // Command: /www (ID: 4098)
#define cmd_spellscribe_handler_x                          0x004F7B90  // Command: /spellscribe (ID: 9085)
#define cmd_smile_handler_x                                0x004DFE30  // Command: /smile (ID: 14023)
#define cmd_hail_handler_x                                 0x004F3010  // Command: /hail (ID: 14024)
#define cmd_frown_handler_x                                0x004DFE70  // Command: /frown (ID: 14025)
#define cmd_laugh_handler_x                                0x004DFEF0  // Command: /laugh (ID: 14026)
#define cmd_cry_handler_x                                  0x004DFF30  // Command: /cry (ID: 14027)
#define cmd_chuckle_handler_x                              0x004DFF70  // Command: /chuckle (ID: 14028)
#define cmd_hug_handler_x                                  0x004DFFB0  // Command: /hug (ID: 14029)
#define cmd_poke_handler_x                                 0x004DFFF0  // Command: /poke (ID: 14030)
#define cmd_bow_handler_x                                  0x004E0030  // Command: /bow (ID: 14031)
#define cmd_bounce_handler_x                               0x004E0070  // Command: /bounce (ID: 14032)
#define cmd_panic_handler_x                                0x004E00B0  // Command: /panic (ID: 14033)
#define cmd_nod_handler_x                                  0x004E00F0  // Command: /nod (ID: 14034)
#define cmd_rofl_handler_x                                 0x004E0130  // Command: /rofl (ID: 14035)
#define cmd_wave_handler_x                                 0x004E0170  // Command: /wave (ID: 14036)
#define cmd_cheer_handler_x                                0x004E01F0  // Command: /cheer (ID: 14037)
#define cmd_gesture_handler_x                              0x004E0280  // Command: /gesture (ID: 14038)
#define cmd_rude_handler_x                                 0x004E0280  // Command: /rude (ID: 14039)
#define cmd_flipoff_handler_x                              0x004E0280  // Command: /flipoff (ID: 14040)
#define cmd_bird_handler_x                                 0x004E0280  // Command: /bird (ID: 14041)
#define cmd_finger_handler_x                               0x004E0280  // Command: /finger (ID: 14042)
#define cmd_agree_handler_x                                0x004E0300  // Command: /agree (ID: 14043)
#define cmd_amaze_handler_x                                0x004E0340  // Command: /amaze (ID: 14044)
#define cmd_apologize_handler_x                            0x004E0380  // Command: /apologize (ID: 14045)
#define cmd_applaud_handler_x                              0x004E03C0  // Command: /applaud (ID: 14046)
#define cmd_plead_handler_x                                0x004E0400  // Command: /plead (ID: 14047)
#define cmd_bite_handler_x                                 0x004E0440  // Command: /bite (ID: 14048)
#define cmd_bleed_handler_x                                0x004E0480  // Command: /bleed (ID: 14049)
#define cmd_blink_handler_x                                0x004E04C0  // Command: /blink (ID: 14050)
#define cmd_blush_handler_x                                0x004E0500  // Command: /blush (ID: 14051)
#define cmd_boggle_handler_x                               0x004E0540  // Command: /boggle (ID: 14052)
#define cmd_bonk_handler_x                                 0x004E0580  // Command: /bonk (ID: 14053)
#define cmd_bored_handler_x                                0x004E05C0  // Command: /bored (ID: 14054)
#define cmd_brb_handler_x                                  0x004E0600  // Command: /brb (ID: 14055)
#define cmd_burp_handler_x                                 0x004E0640  // Command: /burp (ID: 14056)
#define cmd_bye_handler_x                                  0x004E0680  // Command: /bye (ID: 14057)
#define cmd_cackle_handler_x                               0x004E06C0  // Command: /cackle (ID: 14058)
#define cmd_calm_handler_x                                 0x004E0700  // Command: /calm (ID: 14059)
#define cmd_clap_handler_x                                 0x004E0740  // Command: /clap (ID: 14060)
#define cmd_comfort_handler_x                              0x004E0780  // Command: /comfort (ID: 14061)
#define cmd_congratulate_handler_x                         0x004E07C0  // Command: /congratulate (ID: 14062)
#define cmd_cough_handler_x                                0x004E0800  // Command: /cough (ID: 14063)
#define cmd_cringe_handler_x                               0x004E0840  // Command: /cringe (ID: 14064)
#define cmd_curious_handler_x                              0x004E0880  // Command: /curious (ID: 14065)
#define cmd_dance_handler_x                                0x004E08C0  // Command: /dance (ID: 14066)
#define cmd_drool_handler_x                                0x004E0900  // Command: /drool (ID: 14067)
#define cmd_duck_handler_x                                 0x004E0940  // Command: /duck (ID: 14068)
#define cmd_eye_handler_x                                  0x004E0980  // Command: /eye (ID: 14069)
#define cmd_gasp_handler_x                                 0x004E09C0  // Command: /gasp (ID: 14070)
#define cmd_giggle_handler_x                               0x004E0A00  // Command: /giggle (ID: 14071)
#define cmd_glare_handler_x                                0x004E0A40  // Command: /glare (ID: 14072)
#define cmd_grin_handler_x                                 0x004E0A80  // Command: /grin (ID: 14073)
#define cmd_groan_handler_x                                0x004E0AC0  // Command: /groan (ID: 14074)
#define cmd_grovel_handler_x                               0x004E0B00  // Command: /grovel (ID: 14075)
#define cmd_happy_handler_x                                0x004E0B40  // Command: /happy (ID: 14076)
#define cmd_hungry_handler_x                               0x004E0B80  // Command: /hungry (ID: 14077)
#define cmd_introduce_handler_x                            0x004E0BC0  // Command: /introduce (ID: 14078)
#define cmd_jk_handler_x                                   0x004E0C00  // Command: /jk (ID: 14079)
#define cmd_kneel_handler_x                                0x004E0C40  // Command: /kneel (ID: 14080)
#define cmd_lost_handler_x                                 0x004E0C80  // Command: /lost (ID: 14081)
#define cmd_massage_handler_x                              0x004E0CC0  // Command: /massage (ID: 14082)
#define cmd_moan_handler_x                                 0x004E0D00  // Command: /moan (ID: 14083)
#define cmd_mourn_handler_x                                0x004E0D40  // Command: /mourn (ID: 14084)
#define cmd_peer_handler_x                                 0x004E0D80  // Command: /peer (ID: 14085)
#define cmd_point_handler_x                                0x004E0DC0  // Command: /point (ID: 14086)
#define cmd_ponder_handler_x                               0x004E0E00  // Command: /ponder (ID: 14087)
#define cmd_puzzle_handler_x                               0x004E0E40  // Command: /puzzle (ID: 14088)
#define cmd_raise_handler_x                                0x004E0E80  // Command: /raise (ID: 14089)
#define cmd_ready_handler_x                                0x004E0EC0  // Command: /ready (ID: 14090)
#define cmd_roar_handler_x                                 0x004E0F00  // Command: /roar (ID: 14091)
#define cmd_salute_handler_x                               0x004E0F40  // Command: /salute (ID: 14092)
#define cmd_shiver_handler_x                               0x004E0F80  // Command: /shiver (ID: 14093)
#define cmd_shrug_handler_x                                0x004E0FC0  // Command: /shrug (ID: 14094)
#define cmd_sigh_handler_x                                 0x004E1000  // Command: /sigh (ID: 14095)
#define cmd_smirk_handler_x                                0x004E1040  // Command: /smirk (ID: 14096)
#define cmd_snarl_handler_x                                0x004E1080  // Command: /snarl (ID: 14097)
#define cmd_snicker_handler_x                              0x004E10C0  // Command: /snicker (ID: 14098)
#define cmd_stare_handler_x                                0x004E1100  // Command: /stare (ID: 14099)
#define cmd_tap_handler_x                                  0x004E1140  // Command: /tap (ID: 14100)
#define cmd_tease_handler_x                                0x004E1180  // Command: /tease (ID: 14101)
#define cmd_thank_handler_x                                0x004E11C0  // Command: /thank (ID: 14102)
#define cmd_thirsty_handler_x                              0x004E1200  // Command: /thirsty (ID: 14103)
#define cmd_veto_handler_x                                 0x004E1240  // Command: /veto (ID: 14104)
#define cmd_welcome_handler_x                              0x004E1280  // Command: /welcome (ID: 14105)
#define cmd_whine_handler_x                                0x004E12C0  // Command: /whine (ID: 14106)
#define cmd_whistle_handler_x                              0x004E1300  // Command: /whistle (ID: 14107)
#define cmd_yawn_handler_x                                 0x004E1340  // Command: /yawn (ID: 14108)
#define cmd_smack_handler_x                                0x004E1380  // Command: /smack (ID: 14110)
#define cmd_pat_handler_x                                  0x004E13C0  // Command: /pat (ID: 14111)
#define cmd_fidget_handler_x                               0x004E1400  // Command: /fidget (ID: 14112)
#define cmd_flex_handler_x                                 0x004E1500  // Command: /flex (ID: 14113)
#define cmd_nudge_handler_x                                0x004E1440  // Command: /nudge (ID: 14114)
#define cmd_purr_handler_x                                 0x004E1480  // Command: /purr (ID: 14115)
#define cmd_kiss_handler_x                                 0x004E14C0  // Command: /kiss (ID: 14116)
#define cmd_time_handler_x                                 0x004DF630  // Command: /time (ID: 14117)
#define cmd_discipline_handler_x                           0x004F1F00  // Command: /discipline (ID: 14118)
#define cmd_random_handler_x                               0x004DF6C0  // Command: /random (ID: 14120)
#define cmd_summon_handler_x                               0x004F3680  // Command: /summon (ID: 14121)
#define cmd_broadcast_handler_x                            0x004DEE80  // Command: /broadcast (ID: 14122)
#define cmd_lastname_handler_x                             0x004F02C0  // Command: /lastname (ID: 14125)
#define cmd_zone_handler_x                                 0x004F08F0  // Command: /zone (ID: 14126)
#define cmd_kick_handler_x                                 0x004F05E0  // Command: /kick (ID: 14127)
#define cmd_kill_handler_x                                 0x004F0CA0  // Command: /kill (ID: 14128)
#define cmd_delcorpse_handler_x                            0x004DEF90  // Command: /delcorpse (ID: 14129)
#define cmd_hideme_handler_x                               0x004FBB40  // Command: /hideme (ID: 14131)
#define cmd_servers_handler_x                              0x004DEEB0  // Command: /servers (ID: 14132)
#define cmd_height_handler_x                               0x004F0FA0  // Command: /height (ID: 14133)
#define cmd_exit_handler_x                                 0x004E64C0  // Command: /exit (ID: 14134)
#define cmd_loot_handler_x                                 0x004E7880  // Command: /loot (ID: 14135)
#define cmd_selfkill_handler_x                             0x004DEF50  // Command: /selfkill (ID: 6177)
#define cmd_roleplay_handler_x                             0x004E83D0  // Command: /roleplay (ID: 14136)
#define cmd_follow_handler_x                               0x004F5EF0  // Command: /follow (ID: 14137)
#define cmd_report_handler_x                               0x004EFF50  // Command: /report (ID: 14138)
#define cmd_vreport_handler_x                              0x004FB700  // Command: /vreport (ID: 14138)
#define cmd_surname_handler_x                              0x004EFE20  // Command: /surname (ID: 14139)
#define cmd_log_handler_x                                  0x004FC230  // Command: /log (ID: 14141)
#define cmd_secondslog_handler_x                           0x004E8350  // Command: /secondslog (ID: 14141)
#define cmd_toggletell_handler_x                           0x004E61D0  // Command: /toggletell (ID: 14142)
#define cmd_inquire_handler_x                              0x004E6310  // Command: /inquire (ID: 14143)
#define cmd_praise_handler_x                               0x004E6340  // Command: /praise (ID: 14144)
#define cmd_warn_handler_x                                 0x004E6370  // Command: /warn (ID: 14145)
#define cmd_servertransfer_handler_x                       0x004E82D0  // Command: /servertransfer (ID: 14150)
#define cmd_hidecorpses_handler_x                          0x004E78C0  // Command: /hidecorpses (ID: 14151)
#define cmd_reverb_handler_x                               0x004E7B10  // Command: /reverb (ID: 14153)
#define cmd_fastdrop_handler_x                             0x004E5200  // Command: /fastdrop (ID: 14154)
#define cmd_trackplayers_handler_x                         0x004E5410  // Command: /trackplayers (ID: 14155)
#define cmd_trackpets_handler_x                            0x004E5550  // Command: /trackpets (ID: 4111)
#define cmd_trackmercs_handler_x                           0x004E5640  // Command: /trackmercs (ID: 4115)
#define cmd_clickthrough_handler_x                         0x004E5730  // Command: /clickthrough (ID: 4116)
#define cmd_stoptracking_handler_x                         0x004E5770  // Command: /stoptracking (ID: 6777)
#define cmd_help_handler_x                                 0x004F4520  // Command: /help (ID: 14156)
#define cmd_lfgroup_handler_x                              0x004E52E0  // Command: /lfgroup (ID: 14157)
#define cmd_lfguild_handler_x                              0x004E53F0  // Command: /lfguild (ID: 14157)
#define cmd_approval_handler_x                             0x004F4780  // Command: /approval (ID: 14158)
#define cmd_sit_handler_x                                  0x004E8620  // Command: /sit (ID: 14159)
#define cmd_stand_handler_x                                0x004E8670  // Command: /stand (ID: 14160)
#define cmd_book_handler_x                                 0x004F7230  // Command: /book (ID: 14161)
#define cmd_cast_handler_x                                 0x004E8900  // Command: /cast (ID: 14162)
#define cmd_doability_handler_x                            0x004E8AA0  // Command: /doability (ID: 14163)
#define cmd_whotarget_handler_x                            0x004E8B60  // Command: /whotarget (ID: 14164)
#define cmd_language_handler_x                             0x004FC420  // Command: /language (ID: 14165)
#define cmd_target_handler_x                               0x004E9790  // Command: /target (ID: 14166)
#define cmd_camp_handler_x                                 0x004E8B80  // Command: /camp (ID: 14167)
#define cmd_attack_handler_x                               0x004E8C70  // Command: /attack (ID: 14168)
#define cmd_invite_handler_x                               0x004E8C10  // Command: /invite (ID: 14169)
#define cmd_disband_handler_x                              0x004E8C50  // Command: /disband (ID: 14170)
#define cmd_note_handler_x                                 0x004F5E50  // Command: /note (ID: 14171)
#define cmd_usercolor_handler_x                            0x004E9B60  // Command: /usercolor (ID: 14173)
#define cmd_resetwindows_handler_x                         0x004EAE80  // Command: /resetwindows (ID: 14174)
#define cmd_mcicontrol_handler_x                           0x004EAE90  // Command: /mcicontrol (ID: 14175)
#define cmd_popmail_handler_x                              0x004EB0E0  // Command: /popmail (ID: 14176)
#define cmd_dynamiclights_handler_x                        0x004E98F0  // Command: /dynamiclights (ID: 14178)
#define cmd_chatfontsize_handler_x                         0x004E9A10  // Command: /chatfontsize (ID: 14179)
#define cmd_lootnodrop_handler_x                           0x004F2700  // Command: /lootnodrop (ID: 14180)
#define cmd_leaverealestate_handler_x                      0x004DF640  // Command: /leaverealestate (ID: 6865)
#define cmd_reversesound_handler_x                         0x004E81E0  // Command: /reversesound (ID: 14181)
#define cmd_emotezone_handler_x                            0x004E8020  // Command: /emotezone (ID: 14182)
#define cmd_invwinlabels_handler_x                         0x004E9A90  // Command: /invwinlabels (ID: 14183)
#define cmd_tracksort_handler_x                            0x004F4930  // Command: /tracksort (ID: 14184)
#define cmd_trackfilter_handler_x                          0x004F4CD0  // Command: /trackfilter (ID: 14185)
#define cmd_mousespeed_handler_x                           0x004F5130  // Command: /mousespeed (ID: 14186)
#define cmd_gems_handler_x                                 0x004E61A0  // Command: /gems (ID: 14187)
#define cmd_toggleinspect_handler_x                        0x004E7F20  // Command: /toggleinspect (ID: 8510)
#define cmd_inspect_handler_x                              0x004E7B20  // Command: /inspect (ID: 14188)
#define cmd_acompare_handler_x                             0x004E7C40  // Command: /acompare (ID: 5202)
#define cmd_aclearcompare_handler_x                        0x004E7CE0  // Command: /aclearcompare (ID: 5206)
#define cmd_open_handler_x                                 0x004E7CF0  // Command: /open (ID: 8513)
#define cmd_reveal_handler_x                               0x004E7E30  // Command: /reveal (ID: 9156)
#define cmd_barter_handler_x                               0x004E6F30  // Command: /barter (ID: 6061)
#define cmd_buyer_handler_x                                0x004E6F60  // Command: /buyer (ID: 6062)
#define cmd_showspelleffects_handler_x                     0x004E5780  // Command: /showspelleffects (ID: 14190)
#define cmd_combatmusic_handler_x                          0x004E5880  // Command: /combatmusic (ID: 7526)
#define cmd_mixahead_handler_x                             0x004F5230  // Command: /mixahead (ID: 7527)
#define cmd_charinfo_handler_x                             0x004DF330  // Command: /charinfo (ID: 14192)
#define cmd_safelock_handler_x                             0x004EAD80  // Command: /safelock (ID: 14193)
#define cmd_alternateadv_handler_x                         0x004F73E0  // Command: /alternateadv (ID: 14195)
#define cmd_targetgroupbuff_handler_x                      0x004EA820  // Command: /targetgroupbuff (ID: 14196)
#define cmd_tgb_handler_x                                  0x004EA820  // Command: /tgb (ID: 14197)
#define cmd_hotbutton_handler_x                            0x004F79B0  // Command: /hotbutton (ID: 14198)
#define cmd_particledensity_handler_x                      0x004EA740  // Command: /particledensity (ID: 5366)
#define cmd_dismount_handler_x                             0x004EAC50  // Command: /dismount (ID: 14201)
#define cmd_rtarget_handler_x                              0x004EAD00  // Command: /rtarget (ID: 14202)
#define cmd_showgrass_handler_x                            0x004E6DA0  // Command: /showgrass (ID: 14203)
#define cmd_dopropertinting_handler_x                      0x004EBCD0  // Command: /dopropertinting (ID: 14207)
#define cmd_makeleader_handler_x                           0x004EF340  // Command: /makeleader (ID: 5043)
#define cmd_fullscreen_handler_x                           0x00478140  // Command: /fullscreen (ID: 4294967295)
#define cmd_viewport_handler_x                             0x004EB380  // Command: /viewport (ID: 14210)
#define cmd_clearchat_handler_x                            0x004E99E0  // Command: /clearchat (ID: 3127)
#define cmd_clearallchat_handler_x                         0x004E9A00  // Command: /clearallchat (ID: 6864)
#define cmd_bazaar_handler_x                               0x004EAB10  // Command: /bazaar (ID: 4501)
#define cmd_trader_handler_x                               0x004EAB40  // Command: /trader (ID: 4502)
#define cmd_bugreport_handler_x                            0x004E7310  // Command: /bugreport (ID: 13952)
#define cmd_loadskin_handler_x                             0x004F31E0  // Command: /loadskin (ID: 3123)
#define cmd_copylayout_handler_x                           0x004F33D0  // Command: /copylayout (ID: 6333)
#define cmd_memspellset_handler_x                          0x004F7100  // Command: /memspellset (ID: 3341)
#define cmd_autoinventory_handler_x                        0x004FE8C0  // Command: /autoinventory (ID: 9079)
#define cmd_adventure_handler_x                            0x004DE0F0  // Command: /adventure (ID: 5148)
#define cmd_dzhelp_handler_x                               0x004EC890  // Command: /dzhelp (ID: 3550)
#define cmd_dzlisttimers_handler_x                         0x004EC750  // Command: /dzlisttimers (ID: 3548)
#define cmd_dzaddplayer_handler_x                          0x004EC300  // Command: /dzaddplayer (ID: 3543)
#define cmd_dzremoveplayer_handler_x                       0x004EC3E0  // Command: /dzremoveplayer (ID: 3544)
#define cmd_dzswapplayer_handler_x                         0x004EC4C0  // Command: /dzswapplayer (ID: 3545)
#define cmd_dzmakeleader_handler_x                         0x004EC5D0  // Command: /dzmakeleader (ID: 3546)
#define cmd_dzplayerlist_handler_x                         0x004EC6B0  // Command: /dzplayerlist (ID: 3547)
#define cmd_dzquit_handler_x                               0x004EC7F0  // Command: /dzquit (ID: 3549)
#define cmd_eqplayersupdate_handler_x                      0x004DF250  // Command: /eqplayersupdate (ID: 3661)
#define cmd_eqplayersguildupdate_handler_x                 0x004DF190  // Command: /eqplayersguildupdate (ID: 3658)
#define cmd_taskaddplayer_handler_x                        0x004EC970  // Command: /taskaddplayer (ID: 8903)
#define cmd_taskremoveplayer_handler_x                     0x004ECA60  // Command: /taskremoveplayer (ID: 8904)
#define cmd_taskplayerlist_handler_x                       0x004ECB50  // Command: /taskplayerlist (ID: 8906)
#define cmd_taskmakeleader_handler_x                       0x004ECC00  // Command: /taskmakeleader (ID: 8950)
#define cmd_tasktimers_handler_x                           0x004ECE50  // Command: /tasktimers (ID: 8988)
#define cmd_taskquit_handler_x                             0x004ECCF0  // Command: /taskquit (ID: 8907)
#define cmd_taskhelp_handler_x                             0x004ECDA0  // Command: /taskhelp (ID: 8949)
#define cmd_gmarknpc_handler_x                             0x004E8690  // Command: /gmarknpc (ID: 8813)
#define cmd_rmarknpc_handler_x                             0x004E8730  // Command: /rmarknpc (ID: 8814)
#define cmd_marknpc_handler_x                              0x004E8690  // Command: /marknpc (ID: 8815)
#define cmd_clearmarks_handler_x                           0x004E87D0  // Command: /clearmarks (ID: 8816)
#define cmd_findpc_handler_x                               0x004E8800  // Command: /findpc (ID: 8817)
#define cmd_inspectbuffs_handler_x                         0x004E88A0  // Command: /inspectbuffs (ID: 8813)
#define cmd_webupdate_handler_x                            0x004DE1E0  // Command: /webupdate (ID: 14209)
#define cmd_bodytint_handler_x                             0x004EBCA0  // Command: /bodytint (ID: 4294967295)
#define cmd_raidwindow_handler_x                           0x004DE390  // Command: /raidwindow (ID: 4294967295)
#define cmd_makeraidleader_handler_x                       0x004DE3E0  // Command: /makeraidleader (ID: 5089)
#define cmd_raidinvite_handler_x                           0x004DE470  // Command: /raidinvite (ID: 5090)
#define cmd_raiddecline_handler_x                          0x004DE4C0  // Command: /raiddecline (ID: 5091)
#define cmd_raidaccept_handler_x                           0x004DE4D0  // Command: /raidaccept (ID: 5092)
#define cmd_raiddisband_handler_x                          0x004DE510  // Command: /raiddisband (ID: 5093)
#define cmd_setloottype_handler_x                          0x004DE520  // Command: /setloottype (ID: 5097)
#define cmd_addraidlooter_handler_x                        0x004DE530  // Command: /addraidlooter (ID: 5098)
#define cmd_removeraidlooter_handler_x                     0x004DE540  // Command: /removeraidlooter (ID: 5099)
#define cmd_rsay_handler_x                                 0x004DE550  // Command: /rsay (ID: 5110)
#define cmd_summoncorpse_handler_x                         0x004F39B0  // Command: /summoncorpse (ID: 4294967295)
#define cmd_sheathe_handler_x                              0x004DE600  // Command: /sheathe (ID: 4294967295)
#define cmd_fontface_handler_x                             0x004DE560  // Command: /fontface (ID: 3377)
#define cmd_aggressive_handler_x                           0x004DE6F0  // Command: /aggressive (ID: 4294967295)
#define cmd_outputfile_handler_x                           0x004FEBD0  // Command: /outputfile (ID: 3486)
#define cmd_vplay_handler_x                                0x004DE7E0  // Command: /vplay (ID: 6013)
#define cmd_vtell_handler_x                                0x004EF7B0  // Command: /vtell (ID: 6014)
#define cmd_vgroup_handler_x                               0x004EFA20  // Command: /vgroup (ID: 6015)
#define cmd_vraid_handler_x                                0x004EFC20  // Command: /vraid (ID: 6016)
#define cmd_shadows_handler_x                              0x004DE880  // Command: /shadows (ID: 7000)
#define cmd_autofire_handler_x                             0x004FB9B0  // Command: /autofire (ID: 392)
#define cmd_setstartcity_handler_x                         0x004DEAC0  // Command: /setstartcity (ID: 4294967295)
#define cmd_changename_handler_x                           0x004F04A0  // Command: /changename (ID: 5969)
#define cmd_resetaas_handler_x                             0x004782E0  // Command: /resetaas (ID: 4294967295)
#define cmd_resetCreationServer_handler_x                  0x004ECEE0  // Command: /resetCreationServer (ID: 4294967295)
#define cmd_paperdoll_handler_x                            0x004ECFF0  // Command: /paperdoll (ID: 4294967295)
#define cmd_blockspell_handler_x                           0x004FC730  // Command: /blockspell (ID: 4294967295)
#define cmd_uptime_handler_x                               0x004785B0  // Command: /uptime (ID: 4101)
#define cmd_decaycorpse_handler_x                          0x00479140  // Command: /decaycorpse (ID: 4294967295)
#define cmd_testcopy_handler_x                             0x00478970  // Command: /testcopy (ID: 9188)
#define cmd_voicechat_handler_x                            0x004789C0  // Command: /voicechat (ID: 4294967295)
#define cmd_mercwindows_handler_x                          0x004E2D20  // Command: /mercwindows (ID: 4294967295)
#define cmd_stance_handler_x                               0x004E3230  // Command: /stance (ID: 4294967295)
#define cmd_mercassist_handler_x                           0x004E3290  // Command: /mercassist (ID: 4294967295)
#define cmd_mercswitch_handler_x                           0x00478AC0  // Command: /mercswitch (ID: 4294967295)
#define cmd_xtarget_handler_x                              0x004E4780  // Command: /xtarget (ID: 35100)
#define cmd_aggrometer_handler_x                           0x004F3B20  // Command: /aggrometer (ID: 13335)
#define cmd_aggrolock_handler_x                            0x004F3C10  // Command: /aggrolock (ID: 4294967295)
#define cmd_changepetname_handler_x                        0x004F8B40  // Command: /changepetname (ID: 5969)
#define cmd_changemercname_handler_x                       0x004F8C90  // Command: /changemercname (ID: 5969)
#define cmd_kickplayers_handler_x                          0x004F8DE0  // Command: /kickplayers (ID: 4294967295)
#define cmd_alarm_handler_x                                0x004E3500  // Command: /alarm (ID: 4294967295)
#define cmd_tweet_handler_x                                0x00478910  // Command: /tweet (ID: 4294967295)
#define cmd_twitter_handler_x                              0x00478910  // Command: /twitter (ID: 4294967295)
#define cmd_facebook_handler_x                             0x00478930  // Command: /facebook (ID: 4294967295)
#define cmd_fb_handler_x                                   0x00478930  // Command: /fb (ID: 4294967295)
#define cmd_share_handler_x                                0x00478950  // Command: /share (ID: 4294967295)
#define cmd_legendsofnorrath_handler_x                     0x004E2D00  // Command: /legendsofnorrath (ID: 668)
#define cmd_tcg_handler_x                                  0x004E2D00  // Command: /tcg (ID: 669)
#define cmd_grouproles_handler_x                           0x004F8360  // Command: /grouproles (ID: 4294967295)
#define cmd_groupleader_handler_x                          0x004EF690  // Command: /groupleader (ID: 4294967295)
#define cmd_marketplace_handler_x                          0x004ED930  // Command: /marketplace (ID: 4294967295)
#define cmd_merclog_handler_x                              0x00478A30  // Command: /merclog (ID: 4294967295)
#define cmd_comparetaskraid_handler_x                      0x004ED960  // Command: /comparetaskraid (ID: 4294967295)
#define cmd_compareexpedraid_handler_x                     0x004EDA40  // Command: /compareexpedraid (ID: 4294967295)
#define cmd_welcomescreen_handler_x                        0x004FD4A0  // Command: /welcomescreen (ID: 4294967295)
#define cmd_achievements_handler_x                         0x004EDB00  // Command: /achievements (ID: 4294967295)
#define cmd_racechange_handler_x                           0x004E4E40  // Command: /racechange (ID: 4294967295)
#define cmd_realestate_handler_x                           0x004E4E60  // Command: /realestate (ID: 4294967295)
#define cmd_stopsocial_handler_x                           0x004E8A80  // Command: /stopsocial (ID: 4294967295)
#define cmd_offlinemode_handler_x                          0x004E6660  // Command: /offlinemode (ID: 4294967295)
#define cmd_quit_handler_x                                 0x004E63A0  // Command: /quit (ID: 4294967295)

// ============================================================
// Section 12: Unresolved window-singleton globals
// ============================================================
// The 132 'Duplicate/Alternate pinst' comments and 13 redundant
// pinstUnknown defines formerly here were removed: each duplicated a
// real pinst<Window>_x defined elsewhere (their 'new=' guesses were
// misaligned/incorrect). Only these 12 globals remain -- unique
// addresses not defined elsewhere, singleton-pointer candidates known
// by their writing constructor but with the window class unresolved.
// ============================================================
#define pinstUnknown_00F70F30_x                    0x00F70F30   // ctor=0x00697D00
#define pinstUnknown_00E64AE8_x                    0x00E64AE8   // ctor=0x00654C80
#define pinstUnknown_00E64798_x                    0x00E64798   // ctor=0x005F74F0
#define pinstUnknown_00D1FC44_x                    0x00D1FC44   // ctor=0x006BF010
#define pinstUnknown_00D1FC7C_x                    0x00D1FC7C   // ctor=0x006BF010
#define pinstUnknown_00E64488_x                    0x00E64488   // ctor=0x00460C70
#define pinstUnknown_00E63780_x                    0x00E63780   // ctor=0x005AA330
#define pinstUnknown_00E63620_x                    0x00E63620   // ctor=0x0054C950
#define pinstUnknown_00B79010_x                    0x00B79010   // ctor=0x005485D0
#define pinstUnknown_00B79100_x                    0x00B79100   // ctor=0x004109E0
#define pinstUnknown_00E63E18_x                    0x00E63E18   // ctor=0x004132A0
// This global is pinstCMarketplaceWnd, NOT AvaZoneWnd. Proof is the
// construction site, which stores the ctor's return straight into this global:
//     0049692E  cmp   dword ptr [0x00D1FD08], 0
//     00496937  push  0x684                     ; sizeof(CMarketplaceWnd)
//     0049693C  call  0x008DBB3B                ; operator new
//     00496958  call  0x006DDB90                ; CMarketplaceWnd::CMarketplaceWnd
//     00496976  mov   dword ptr [0x00D1FD08], eax
// and 0x006DDB90 is the function that binds the SIDL name "MarketplaceWnd" (0x006DDBAE)
// plus all 219 "MKPW_*" child controls. AvaZoneWnd is a DIFFERENT global -- see
// pinstAvaZoneWnd_x below.
#define pinstUnknown_00D1FD08_x                    0x00D1FD08   // = CMarketplaceWnd, ctor=0x006DDB90, size 0x684
#define pinstCMarketplaceWnd_x                     0x00D1FD08   // preferred name for the above
// The real AvaZoneWnd (GM zone picker). Its ctor 0x00625870 binds "AvaZoneWnd"
// (string 0x009E216C), and 0x0049A9C6 stores it here right after that call.
#define pinstAvaZoneWnd_x                          0x00D1FD40   // ctor=0x00625870 [verified 2026-08-04]

// ============================================================
// Section 13: Struct Member Offsets (decompiled)
// These represent secondary field accesses in PlayerZoneClient & CharacterZoneClient
// ============================================================
#define CharacterZoneClient__Cur_HP_field_off_8                      0x8
#define CharacterZoneClient__Cur_HP_field_off_d                      0xD
#define CharacterZoneClient__Cur_HP_field_off_24                     0x24
#define CharacterZoneClient__Cur_Mana_field_off_1                    0x1
#define CharacterZoneClient__Cur_Mana_field_off_4                    0x4
#define CharacterZoneClient__Cur_Mana_field_off_d                    0xD
#define CharacterZoneClient__Cur_Mana_field_off_24                   0x24
#define PlayerZoneClient__Level_field_off_250                        0x250
#define PlayerZoneClient__Level_field_off_2cc                        0x2CC
#define PlayerZoneClient__Position_fields_off_3c                     0x3C
#define PlayerZoneClient__Position_fields_off_48                     0x48
#define PlayerZoneClient__Position_fields_off_13c                    0x13C
#define PlayerZoneClient__Position_fields_off_31c                    0x31C
#define PlayerZoneClient__Position_fields_off_101c                   0x101C
#define PlayerZoneClient__Combat_fields_off_29                       0x29
#define PlayerZoneClient__Combat_fields_off_44                       0x44
#define PlayerZoneClient__Combat_fields_off_62                       0x62
#define PlayerZoneClient__Combat_fields_off_87                       0x87
#define PlayerZoneClient__Combat_fields_off_8e                       0x8E
#define PlayerZoneClient__Combat_fields_off_a2                       0xA2
#define PlayerZoneClient__Combat_fields_off_a4                       0xA4
#define PlayerZoneClient__Combat_fields_off_b3                       0xB3
#define PlayerZoneClient__Combat_fields_off_c8                       0xC8
#define PlayerZoneClient__Combat_fields_off_d0                       0xD0
#define PlayerZoneClient__Combat_fields_off_d7                       0xD7
#define PlayerZoneClient__Combat_fields_off_d9                       0xD9
#define PlayerZoneClient__Combat_fields_off_da                       0xDA
#define PlayerZoneClient__Combat_fields_off_182                      0x182
#define PlayerZoneClient__Combat_fields_off_188                      0x188
#define PlayerZoneClient__Combat_fields_off_18c                      0x18C
#define PlayerZoneClient__Combat_fields_off_1d4                      0x1D4

// CEverQuest Pet Commands member offsets
#define CEverQuest__PetCmd_field_off_8                               0x8
#define CEverQuest__PetCmd_field_off_44                              0x44
#define CEverQuest__PetCmd_field_off_148                             0x148
#define CEverQuest__PetCmd_field_off_168                             0x168
#define CEverQuest__PetCmd_field_off_38C                             0x38C

// CGroupWnd member offsets
#define CGroupWnd__Group_field_off_15                                0x15
#define CGroupWnd__Group_field_off_24                                0x24
#define CGroupWnd__Group_field_off_94                                0x94

// CTargetWnd member offsets
#define CTargetWnd__Target_field_off_1                               0x1
#define CTargetWnd__Target_field_off_4                               0x4
#define CTargetWnd__Target_field_off_8                               0x8
#define CTargetWnd__Target_field_off_C                               0xC
#define CTargetWnd__Target_field_off_5C                              0x5C
#define CTargetWnd__Target_field_off_94                              0x94
#define CTargetWnd__Target_field_off_98                              0x98
#define CTargetWnd__Target_field_off_DC                              0xDC
#define CTargetWnd__Target_field_off_174                             0x174
#define CTargetWnd__Target_field_off_210                             0x210
#define CTargetWnd__Target_field_off_26B                             0x26B
#define CTargetWnd__Target_field_off_3B8                             0x3B8
#define CTargetWnd__Target_field_off_4E3                             0x4E3
#define CTargetWnd__Target_field_off_53C                             0x53C
#define CTargetWnd__Target_field_off_85C                             0x85C
#define CTargetWnd__Target_field_off_860                             0x860
#define CTargetWnd__Target_field_off_864                             0x864
#define CTargetWnd__Target_field_off_868                             0x868



// ============================================================
// Section 14: CEverQuest Game State & Initialization Offsets
// Discovered by decompiling CEverQuest::SetGameState (0x519750) and EnterZone (0x531F90)
// ============================================================
#define CEverQuest__GameState_offset                                 0x5C8  // Current game state (1=Zoning, 2=CharSelect, 5=InGame)
#define CEverQuest__WorldInitError_offset                            0x5C4  // Set to 1 on world initialization error



// ============================================================

// ============================================================

// ============================================================

// ============================================================
// Section 15: Approximate Window Vtables
// These vtables are matched based on nearest constructor address proximity (dist < 0x80000).
// Only contains vtable offsets not already defined in earlier sections (keeping alternative offsets).
// Duplicate window names or names already defined in Section 9/12 are suffixed with their address to prevent collisions.
// ============================================================

// ~AchievementsWnd (dist=0x1070 from ctor) WndNotify=0x610A40
#define AchievementsWnd_9DF0E0                       0x9DF0E0

// ~AdvancedDisplayOptionsWindow (dist=0x1AD0 from ctor) WndNotify=0x614CD0
#define AdvancedDisplayOptionsWindow_9DF6E8          0x9DF6E8

// ~AlarmWnd (dist=0x290 from ctor) WndNotify=0x61A070
#define AlarmWnd_9E0350                              0x9E0350

// ~AlertStackWnd (dist=0x500 from ctor) WndNotify=0x61B190
#define AlertStackWnd_9E0780                         0x9E0780

// ~AltStorageWnd (dist=0x14E0 from ctor) WndNotify=0x61EAC0
#define AltStorageWnd_9E1088                         0x9E1088

// ~AuraWindow (dist=0x2A30 from ctor) WndNotify=0x623700
#define AuraWindow_9E1D48                            0x9E1D48

// ~BandolierWnd (dist=0x1110 from ctor) WndNotify=0x625D60
#define BandolierWnd_9E2058                          0x9E2058

// ~BazaarConfirmationWnd (dist=0x980 from ctor) WndNotify=0x41D6A0
#define BazaarConfirmationWnd_9C4AB8                 0x9C4AB8

// ~BazaarWnd (dist=0x4F60 from ctor) WndNotify=0x635990
#define BazaarWnd_9E3A08                             0x9E3A08

// ~BlockedPetBuffWnd (dist=0x12560 from ctor) WndNotify=0x431020
#define BlockedPetBuffWnd_9C5478                     0x9C5478

// ~CastingWindow (dist=0x1640 from ctor) WndNotify=0x644C30
#define CastingWindow_9E4EC0                         0x9E4EC0

// ~CastSpellWnd (dist=0x6E40 from ctor) WndNotify=0x64F150
#define CastSpellWnd_9E5970                          0x9E5970

// ~CCollisionInfoTargetVisibility (dist=0x19180 from ctor) WndNotify=0x8BB3F0
#define CCollisionInfoTargetVisibility_A1D91C        0xA1D91C

// ~CListWnd (dist=0x30 from ctor) WndNotify=0x857BD0
#define CListWnd_A1962C                              0xA1962C

// ~CListWnd (dist=0x30 from ctor) WndNotify=0x857BD0
#define CListWnd_A192E4                              0xA192E4

// ~CListWnd (dist=0x3950 from ctor) WndNotify=0x85B550
#define CListWnd_A1947C                              0xA1947C

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_9EC7F0                                 0x9EC7F0

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_A1DBB4                                 0xA1DBB4

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_9E8810                                 0x9E8810

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_9FCCA0                                 0x9FCCA0

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_A1AD94                                 0xA1AD94

// ~CXWnd (dist=0x7C40 from ctor) WndNotify=0x861E80
#define CXWnd_A19AC4                                 0xA19AC4

// ~CXWnd (dist=0x1210 from ctor) WndNotify=0x86ACD0
#define CXWnd_A19E44                                 0xA19E44

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_A1AF04                                 0xA1AF04

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_9FB3B8                                 0x9FB3B8

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_A19CDC                                 0xA19CDC

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_A1B484                                 0xA1B484

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_A1C294                                 0xA1C294

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_9D6A78                                 0x9D6A78

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_9DE988                                 0x9DE988

// ~CXWnd (dist=0x7C40 from ctor) WndNotify=0x861E80
#define CXWnd_9E6B08                                 0x9E6B08

// ~CXWnd (dist=0x17F0 from ctor) WndNotify=0x8682D0
#define CXWnd_9E0EF0                                 0x9E0EF0

// ~DynamicZoneWnd (dist=0x3730 from ctor) WndNotify=0x6600E0
#define DynamicZoneWnd_9E67C0                        0x9E67C0

// ~DynamicZoneWnd (dist=0x2920 from ctor) WndNotify=0x660EF0
#define DynamicZoneWnd_9E6C88                        0x9E6C88

// ~EditLabelWnd (dist=0x880 from ctor) WndNotify=0x6643F0
#define EditLabelWnd_9E6FE0                          0x9E6FE0

// ~ExtendedTargetWnd (dist=0xDB0 from ctor) WndNotify=0x54C5B0
#define ExtendedTargetWnd_9D3940                     0x9D3940

// ~FriendsWindow (dist=0xE30 from ctor) WndNotify=0x66EE20
#define FriendsWindow_9E8680                         0x9E8680

// ~GuildCreationWnd (dist=0xB30 from ctor) WndNotify=0x67ED80
#define GuildCreationWnd_9E9CA8                      0x9E9CA8

// ~HelpWindow (dist=0x6140 from ctor) WndNotify=0x5549E0
#define HelpWindow_9D4010                            0x9D4010

// ~HelpWindow (dist=0x4E10 from ctor) WndNotify=0x5536B0
#define HelpWindow_9D3DD0                            0x9D3DD0

// ~HeritageSelectionWnd (dist=0x2610 from ctor) WndNotify=0x68C190
#define HeritageSelectionWnd_9EA9F8                  0x9EA9F8

// ~JournalCatWnd (dist=0xD50 from ctor) WndNotify=0x6B0230
#define JournalCatWnd_9EC200                         0x9EC200

// ~LayoutCopyWindow (dist=0x830 from ctor) WndNotify=0x6B5690
#define LayoutCopyWindow_9EC970                      0x9EC970

// ~MusicPlayerWnd (dist=0xB80 from ctor) WndNotify=0x6F8E40
#define MusicPlayerWnd_9F29C8                        0x9F29C8

// ~PotionBeltWnd (dist=0x25C0 from ctor) WndNotify=0x71D350
#define PotionBeltWnd_9F6090                         0x9F6090

// ~PotionBeltWnd (dist=0x1A60 from ctor) WndNotify=0x71DEB0
#define PotionBeltWnd_9F6280                         0x9F6280

// ~PvpLeaderboardWnd (dist=0x5F0 from ctor) WndNotify=0x722D90
#define PvpLeaderboardWnd_9F6B78                     0x9F6B78

// ~RealEstateLayoutDetailsWnd (dist=0x16C50 from ctor) WndNotify=0x5C3380
#define RealEstateLayoutDetailsWnd_9D9548            0x9D9548

// ~RealEstatePurchaseWnd (dist=0xA90 from ctor) WndNotify=0x74D500
#define RealEstatePurchaseWnd_9F9268                 0x9F9268

// ~SelectorWindow (dist=0x770 from ctor) WndNotify=0x750F60
#define SelectorWindow_9F9620                        0x9F9620

// ~ServerListWnd (dist=0xB50 from ctor) WndNotify=0x755120
#define ServerListWnd_9FA2C8                         0x9FA2C8

// ~SoulmarkWnd (dist=0x1320 from ctor) WndNotify=0x75AF60
#define SoulmarkWnd_9FAD20                           0x9FAD20

// ~TextEntryWnd (dist=0x2000 from ctor) WndNotify=0x772D00
#define TextEntryWnd_9FC7A8                          0x9FC7A8

// ~TradeWnd (dist=0x1760 from ctor) WndNotify=0x77E7F0
#define TradeWnd_9FD6A8                              0x9FD6A8

// ~TributeBenefitWnd (dist=0x13F0 from ctor) WndNotify=0x782610
#define TributeBenefitWnd_9FDB78                     0x9FDB78

// ~ZonePathWnd (dist=0x1140 from ctor) WndNotify=0x606280
#define ZonePathWnd_9DDD50                           0x9DDD50





// ============================================================

// ============================================================
// Section 17: Graphics, Cameras & UI Layouts (Decompiled)
// Found by decompiling EnterZone (0x531F90) and SetGameState (0x519750)
// ============================================================
// CCamera viewports / structures passed to Camera functions
#define pCameraViewport_9D1B48_x                                     0x9D1B48
#define pCameraViewport_9C5898_x                                     0x9C5898
#define pCameraViewport_9C4EC4_x                                     0x9C4EC4
#define pCameraViewport_9D1B44_x                                     0x9D1B44
#define pCameraViewport_9C735C_x                                     0x9C735C

// Camera & Active session globals
#define pinstActiveCamera_x                                          0xDE0D80  // Active CCamera pointer
#define g_ZoneInitStatus_xDDF704_x                                   0xDDF704  // Initialization status byte
#define pinstActiveTradeSession_xE635B0_x                            0xE635B0  // Loot / trade session pointer

// Map & XML Engine functions
#define MapViewMap__SaveEx_x                                         0x6CB880  // Saves active viewport coordinates
#define TcpLibrary__TcpManager_x                                     0x82E530  // Connection constructor / allocator
#define CXMLSOMDocumentBase__SetSchema_x                             0x84B5E0  // XML schema validator initialization
#define CEverQuest__LootCorpse_x                                     0x536720  // Initiates looting of a corpse target
#define CCastSpellWnd__CastSpell_x                                   0x6488F0  // Initiates spell casting for gem index (0-11)
#define PlayerZoneClient__StandUp_x                                  0x58FF30  // Stands player up (e.g. before casting)
#define CSidlManager__GetWindowBySidlName_x                          0x5B9A10  // Resolves any window pointer by its SIDL XML name

// Game state transition & UI layout management functions
#define CEverQuest__ResetUI_x                                        0x4DB920
#define CEverQuest__ActivateUILayout_x                               0x4DB8E0
#define CEverQuest__DeactivateUILayout_x                             0x4DB910
#define CEverQuest__TransitionUI_x                                   0x4DB8F0


// ============================================================
// Section 18: System & Client Engine Vtables (RTTI scan)
// Mapped from type descriptors and CompleteObjectLocators in the binary
// ============================================================
#define _PackFileData__vftable_x                                     0xA020A8
#define _PackFileDataRawFile__vftable_x                              0xA020B0
#define AchievementSubComponentCountData__vftable_x                  0xA02414
#define AddedToSplineTrigger__vftable_x                              0xA1EF74
#define AsyncAddressResult_TcpLibrary__vftable_x                     0xA27D38
#define BaseReq__vftable_x                                           0xA09F50
#define BaseRequirementCheckData__vftable_x                          0xA0A0A0
#define CButtonTemplate__vftable_x                                   0xA1B068
#define CClickStickInfo__vftable_x                                   0xA1DD14
#define CComboboxTemplate__vftable_x                                 0xA1B110
#define CControlTemplate__vftable_x                                  0xA1B060
#define CEditboxTemplate__vftable_x                                  0xA1B088
#define CFinderBoxTemplate__vftable_x                                0xA1B0D0
#define CGaugeTemplate__vftable_x                                    0xA1B070
#define ChangeFacingAction__vftable_x                                0xA1EEBC
#define CHorizontalLayoutBoxTemplate__vftable_x                      0xA1B0C0
#define CHotButtonTemplate__vftable_x                                0xA1B118
#define CHtmlComponentTemplate__vftable_x                            0xA1B0F0
#define CInvSlotTemplate__vftable_x                                  0xA1B080
#define CipherAES_Crypto__vftable_x                                  0xA56220
#define CLabelTemplate__vftable_x                                    0xA1B098
#define CLayoutBoxTemplate__vftable_x                                0xA1B0B8
#define CLayoutHorizontalTemplate__vftable_x                         0xA1B01C
#define CLayoutStrategy__vftable_x                                   0xA1DA3C
#define CLayoutStrategyHorizontal__vftable_x                         0xA1DA54
#define CLayoutStrategyTemplate__vftable_x                           0xA1B00C
#define CLayoutStrategyVertical__vftable_x                           0xA1DA48
#define CLayoutVerticalTemplate__vftable_x                           0xA1B014
#define CListboxTemplate__vftable_x                                  0xA1B100
#define ControllerFactory__vftable_x                                 0xA1AD04
#define ControllerManager__vftable_x                                 0xA1AD20
#define CPageTemplate__vftable_x                                     0xA1B0A8
#define CParam__vftable_x                                            0xA19F78
#define CParamButton__vftable_x                                      0xA1A274
#define CParamButtonDrawTemplate__vftable_x                          0xA1A084
#define CParamClass__vftable_x                                       0xA19F94
#define CParamCombobox__vftable_x                                    0xA1CDC8
#define CParamControl__vftable_x                                     0xA1A258
#define CParamEditbox__vftable_x                                     0xA1CD0C
#define CParamFinderBox__vftable_x                                   0xA1CE98
#define CParamFrame__vftable_x                                       0xA1A194
#define CParamFrameTemplate__vftable_x                               0xA1A0A0
#define CParamGauge__vftable_x                                       0xA1CC78
#define CParamGaugeDrawTemplate__vftable_x                           0xA1C75C
#define CParamHorizontalLayoutBox__vftable_x                         0xA1CE38
#define CParamHotButton__vftable_x                                   0xA1CF54
#define CParamHtmlComponent__vftable_x                               0xA1CCCC
#define CParamInvSlot__vftable_x                                     0xA1CCE8
#define CParamLabel__vftable_x                                       0xA1CD54
#define CParamLayoutBox__vftable_x                                   0xA1CAF8
#define CParamLayoutHorizontal__vftable_x                            0xA1C950
#define CParamLayoutStrategy__vftable_x                              0xA1C8CC
#define CParamLayoutVertical__vftable_x                              0xA1C924
#define CParamListbox__vftable_x                                     0xA1CADC
#define CParamListboxColumn__vftable_x                               0xA1CAC0
#define CParamNamedTemplatePiece__vftable_x                          0xA1C674
#define CParamPage__vftable_x                                        0xA1CDEC
#define CParamPoint__vftable_x                                       0xA1A04C
#define CParamRGB__vftable_x                                         0xA1A0BC
#define CParamScreen__vftable_x                                      0xA1A290
#define CParamScreenPiece__vftable_x                                 0xA1A220
#define CParamScreens__vftable_x                                     0xA1CA9C
#define CParamScrollbarDrawTemplate__vftable_x                       0xA1A1CC
#define CParamSize__vftable_x                                        0xA1A068
#define CParamSlider__vftable_x                                      0xA1CD30
#define CParamSliderDrawTemplate__vftable_x                          0xA1A204
#define CParamSpellGem__vftable_x                                    0xA1CCA0
#define CParamSpellGemDrawTemplate__vftable_x                        0xA1C7C0
#define CParamStaticAnimation__vftable_x                             0xA1A30C
#define CParamStaticFrame__vftable_x                                 0xA1CBD8
#define CParamStaticHeader__vftable_x                                0xA1CC10
#define CParamStaticScreenPiece__vftable_x                           0xA1A23C
#define CParamStaticText__vftable_x                                  0xA1CBB0
#define CParamStaticTintedBlendAnimation__vftable_x                  0xA1CB94
#define CParamSTMLbox__vftable_x                                     0xA1CD70
#define CParamSuiteDefaults__vftable_x                               0xA1C9C0
#define CParamTabBox__vftable_x                                      0xA1CE10
#define CParamTemplateAssoc__vftable_x                               0xA1C658
#define CParamTemplateContainer__vftable_x                           0xA1CF00
#define CParamTemplateScreen__vftable_x                              0xA1CC3C
#define CParamTextureInfo__vftable_x                                 0xA1A178
#define CParamTileLayoutBox__vftable_x                               0xA1CEC0
#define CParamTopLevelWindowList__vftable_x                          0xA1CF38
#define CParamTreeView__vftable_x                                    0xA1CD94
#define CParamUi2DAnimation__vftable_x                               0xA1A1B0
#define CParamVerticalLayoutBox__vftable_x                           0xA1CE68
#define CParamWindowDrawTemplate__vftable_x                          0xA1A1E8
#define CPropertySetOrderedSort__vftable_x                           0xA19408
#define CriticalSection_EQUtil__vftable_x                            0xA0220C
#define CrossControlPointTrigger__vftable_x                          0xA1EEEC
#define CrossDistanceTrigger__vftable_x                              0xA1EF30
#define CScreenPieceTemplate__vftable_x                              0xA1B030
#define CScreenTemplate__vftable_x                                   0xA1B0E0
#define CSliderTemplate__vftable_x                                   0xA1B090
#define CSpellGemTemplate__vftable_x                                 0xA1B078
#define CStaticAnimationTemplate__vftable_x                          0xA1B040
#define CStaticFrameTemplate__vftable_x                              0xA1B050
#define CStaticHeaderTemplate__vftable_x                             0xA1B058
#define CStaticScreenPieceTemplate__vftable_x                        0xA1B038
#define CStaticTextTemplate__vftable_x                               0xA1B048
#define CStaticTintedBlendAnimationTemplate__vftable_x               0xA1B0E8
#define CSTMLboxTemplate__vftable_x                                  0xA1B0A0
#define CTabBoxTemplate__vftable_x                                   0xA1B0B0
#define CTemplateContainerTemplate__vftable_x                        0xA1B0F8
#define CTextureAnimation__vftable_x                                 0xA1A0D8
#define CTextureFont__vftable_x                                      0xA1A330
#define CTileLayoutBoxTemplate__vftable_x                            0xA1B0D8
#define CTreeViewTemplate__vftable_x                                 0xA1B108
#define CVerticalLayoutBoxTemplate__vftable_x                        0xA1B0C8
#define CXMLDataPtr__vftable_x                                       0xA1A040
#define CXMLParamManager__vftable_x                                  0xA1A148
#define CXMLSOMDocument__vftable_x                                   0xA1A110
#define CXMLSOMDocumentBase__vftable_x                               0xA18808
#define EjectRiderAction__vftable_x                                  0xA1EEA4
#define EntropySourceHandler_Crypto__vftable_x                       0xA48364
#define FacePointAction__vftable_x                                   0xA1EED4
#define FinderItem__vftable_x                                        0xA1D89C
#define FunctionObject_Uram__vftable_x                               0xA17DAC
#define HttpConnectionHandlerAdapter_HttpLibrary__vftable_x          0xA27FD0
#define HttpHandler_Uram__vftable_x                                  0xA17E28
#define ItemBase__vftable1_x                                          0xA022FC   // secondary vtable, subobject +0x8 (RTTI)
#define LogHandler_SslLibrary__vftable_x                             0xA17D68
#define ObjectReportHandlerAdapter_Uram__vftable_x                   0xA13A4C
#define OsEntropy_Crypto__vftable_x                                  0xA4837C
#define PausedTrigger__vftable_x                                     0xA1EFFC
#define PcBase__vftable1_x                                            0xA04E44   // secondary vtable, subobject +0x2DCC (RTTI)
#define PlayerPointManager__vftable_x                                0xA0A0D4
#define PollResults__vftable_x                                       0xA10410
#define PollVote__vftable_x                                          0xA10234
#define RemovedFromSplineTrigger__vftable_x                          0xA1EFB8
#define ReportHandler_Smart__vftable_x                               0xA1346C
#define ReportHandlerAdapter_SslLibrary__vftable_x                   0xA282EC
#define ReqAAs__vftable_x                                            0xA09FE0
#define ReqBodyType__vftable_x                                       0xA09FD8
#define ReqCharacterType__vftable_x                                  0xA0A038
#define ReqClass__vftable_x                                          0xA09FB0
#define ReqCreationDate__vftable_x                                   0xA0A020
#define ReqCreationServer__vftable_x                                 0xA09F80
#define ReqCurrentProfile__vftable_x                                 0xA09FD0
#define ReqExpansion__vftable_x                                      0xA09FE8
#define ReqGameFeature__vftable_x                                    0xA0A098
#define ReqGuildStatus__vftable_x                                    0xA0A018
#define ReqHasMemorizedSpell__vftable_x                              0xA0A060
#define ReqHasScribedSpell__vftable_x                                0xA0A068
#define ReqHasTask__vftable_x                                        0xA0A058
#define ReqHasWornItemClassInSlot__vftable_x                         0xA0A070
#define ReqIsFastRegen__vftable_x                                    0xA09F88
#define ReqLeadershipAbilities__vftable_x                            0xA09F78
#define ReqLevelRange__vftable_x                                     0xA09FA0
#define ReqMaxLevel__vftable_x                                       0xA09F60
#define ReqMaxMembershipLevel__vftable_x                             0xA0A080
#define ReqMaxRealLevel__vftable_x                                   0xA09F70
#define ReqMaxSkill__vftable_x                                       0xA09FF8
#define ReqMembershipLevelRange__vftable_x                           0xA0A088
#define ReqMinLevel__vftable_x                                       0xA09F58
#define ReqMinMembershipLevel__vftable_x                             0xA0A078
#define ReqMinRealLevel__vftable_x                                   0xA09F68
#define ReqMinSkill__vftable_x                                       0xA09FF0
#define ReqNamedBit__vftable_x                                       0xA0A010
#define ReqNamedBitRange__vftable_x                                  0xA0A028
#define ReqRace__vftable_x                                           0xA09FC0
#define ReqRawFaction__vftable_x                                     0xA0A030
#define ReqRealClass__vftable_x                                      0xA09FB8
#define ReqRealLevelRange__vftable_x                                 0xA09FA8
#define ReqRealRace__vftable_x                                       0xA09FC8
#define ReqRestrictedGameFeatures__vftable_x                         0xA0A090
#define ReqSex__vftable_x                                            0xA09F98
#define ReqSkillRange__vftable_x                                     0xA0A000
#define ReqStartCity__vftable_x                                      0xA0A008
#define ReqTaskCompleted__vftable_x                                  0xA0A040
#define ReqTextureType__vftable_x                                    0xA0A048
#define RequirementAssociationManager__vftable_x                     0xA0A0C0
#define RequirementManager__vftable_x                                0xA0A0B8
#define ReqZoneId__vftable_x                                         0xA0A050
#define RewardAa__vftable_x                                          0xA0FF14
#define RewardAaPoints__vftable_x                                    0xA0FF38
#define RewardBase__vftable_x                                        0xA0FEC4
#define RewardGenericPoints__vftable_x                               0xA0FF88
#define RewardItem__vftable_x                                        0xA0FFBC
#define RewardMoney__vftable_x                                       0xA0FEE8
#define RewardSet__vftable_x                                         0xA0FFE8
#define RunScriptAction__vftable_x                                   0xA1EE8C
#define ShareBase__vftable_x                                         0xA02020
#define SharedStartingPlayerData__vftable_x                          0xA0FFF8
#define SharedStartingPlayerManager__vftable_x                       0xA10020
#define SingleAchievementAndComponentsInfo__vftable_x                0xA023BC
#define SingleAchievementAndComponentsInfoWithCounts__vftable_x      0xA023E4
#define SslConnectionFilterImpl_SslLibrary__vftable_x                0xA290C0
#define SymmetricKey_Crypto__vftable_x                               0xA5618C
#define Sync_EQUtil__vftable_x                                       0xA021F8
#define TcpConnectionHandler_TcpLibrary__vftable_x                   0xA17D54
#define TcpConnectionPlatformDriver_TcpLibrary__vftable_x            0xA55BD8
#define TcpPlatformDriver_TcpLibrary__vftable_x                      0xA55D5C
#define TiXmlBase__vftable_x                                         0xA506F4
#define TiXmlDocument__vftable_x                                     0xA13C44
#define type_info__vftable_x                                         0xA1F6C0
#define UdpGuardedRefCount_UdpLibrary__vftable_x                     0xA53D20
#define UdpPlatformDriver_UdpLibrary__vftable_x                      0xA53F7C
#define UnpausedTrigger__vftable_x                                   0xA1F040
#define UploaderLogHandlerAdapter_Uram__vftable_x                    0xA17D98
#define UtilsReportHandlerAdapter_SslLibrary__vftable_x              0xA13488
#define VeBaseReferenceCount__vftable_x                              0xA022C4
#define XwndControllerFactory_ControllerFactory__vftable_x           0xA1AD18


// ============================================================
// Section 19: Advanced Platform, Network & Graphics Subsystems
// Discovered via recursive decompiler call-tree trace and billing/auth strings
// ============================================================
#define ios_base__eofbitset_x                                        0x414E80
#define EQ_Affect__AddEQAffectArrayToBuffer_x                        0x7E4160
#define ItemBaseContainer__MoveItemCount_x                           0x7E6230
#define BitStream__ReadUInt_x                                        0x80D380
#define BitStream__ReadInt_x                                         0x80D460
#define CUITextureInfo__DrawQMarkFailed_x                            0x865A60
#define CTextureAnimation__Draw_x                                    0x896CF0


// ============================================================
// Section 19: Advanced Platform, Network & Graphics Subsystems
// Discovered via recursive decompiler call-tree trace and billing/auth strings
// ============================================================
#define CRaid__AddRaidMember_FindOpenIndexreturn_1_x                 0x47E4E0
#define Failedtoopen_sforwritinginMapViewMap__SaveEx_x               0x6CC710
#define CRaidWnd__SetPlayerClassColor_playernotfound__s_x            0x72A5E0
#define CRaidWnd__ChangeRaidGroupLeader_newleadernotfoundgroup__d_newleader_s_x 0x72CF90
#define SslLibrary__MakeDirectoryDefault_CreateDirectoryfailed_path__s_x 0x814CF0
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814D50
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814D80
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814DB0
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814DE0
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814E10
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814E40
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814E70
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814EA0
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814ED0
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814F00
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814F30
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814F60
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814F90
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814FC0
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x814FF0
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x815020
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x815050
#define SslLibrary__CalcHash_CryptGetHashParamfailed_x               0x815080
#define SslLibrary__MakeDirectoryDefault_CreateDirectoryfailed_path__s_x 0x8152D0
#define SslLibrary__ConvertCertificateStore_Failedtowritedataforcert__stofile__s_x 0x8156C0
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x816290
#define SslLibrary__GetFuncByName_GetProcAddressfailed_proc__s_x     0x8168D0
#define AccountPaymentInfoRequest__Validate_failedwithinvalidsessionId_x 0x816D20
#define AccountPaymentInfoResponse__Validate_missingstatus_x         0x816E20
#define FundWalletRequest__Validate_failedwithnullamountandnullsku_x 0x816F60
#define FundWalletResponse__Validate_missingstatus_x                 0x817150
#define AddPaymentSourceRequest__Validate_failedwithinvalidzip_x     0x817240
#define AddPaymentSourceResponse__Validate_missingstatus_x           0x8174E0
#define CreditCardTypesRequest__Validate_failedwithinvalidsessionId_x 0x8175A0
#define CreditCardTypesResponse__Validate_missingstatus_x            0x8176A0
#define PreviewOrderRequest__Validate_failedwithinvalidgameCode_x    0x817760
#define PreviewOrderResponse__Validate_missingstatus_x               0x8178E0
#define PreviewOrderRequest__Validate_failedwithinvalidgameCode_x    0x8179D0
#define PurchaseOrderResponse__Validate_missingstatus_x              0x817B70
#define RedeemCodeRequest__Validate_failedwithinvalidgameCode_x      0x817C30
#define RedeemCodeResponse__Validate_missingstatus_x                 0x817D80
#define CountryCodesRequest__Validate_failedwithinvalidlocale_x      0x817E40
#define CountryCodesResponse__Validate_missingstatus_x               0x817F40
#define UpdateCreditCardRequest__Validate_failedwithinvalidzip_x     0x818030
#define UpdateCreditCardResponse__Validate_missingstatus_x           0x818250
#define DeletePaymentSourceRequest__Validate_failedwithinvalidpaymentSourceId_x 0x818310
#define DeletePaymentSourceResponse__Validate_missingstatus_x        0x818430
#define CasKeepAliveRequest__Validate_failedwithinvalidsessionId_x   0x8184F0
#define CasKeepAliveResponse__Validate_missingstatus_x               0x8185F0
#define CasGetSessionTicketRequest__Validate_failedwithinvalidsessionId_x 0x818670
#define IsAffiliatedWithPartnerRequest__Validate_nopartnerClientIdwasprovided_x 0x818780
#define IsAffiliatedWithPartnerResponse__Validate_missingstatus_x    0x818900
#define FinalizeThirdPartyOrderTransactionRequest__Validate_failedwithinvalidsessionId_x 0x8189C0
#define FinalizeThirdPartyOrderTransactionResponse__Validate_missingstatus_x 0x818AE0
#define FinalizeThirdPartyOrderTransactionRequest__Validate_failedwithinvaliduserAgent_x 0x818BA0
#define CasKeepAliveResponse__Validate_missingstatus_x               0x820900
#define FundWalletResponse__Parse_failedwithinvalidstatus_x          0x824880
#define AddPaymentSourceResponse__Parse_failedwithinvalidstatus_x    0x824CB0
#define UpdateCreditCardResponse__Parse_failedwithinvalidstatus_x    0x826320
#define DeletePaymentSourceResponse__Parse_failedwithinvalidstatus_x 0x826690
#define IsAffiliatedWithPartnerResponse__Parse_failedwithinvalidstatus_x 0x826A90
#define FinalizeThirdPartyOrderTransactionResponse__Parse_failedwithinvalidstatus_x 0x826E60
#define CreditCardTypesResponse__Parse_failedwithinvalidstatus_x     0x827150
#define PurchaseOrderResponse__Parse_failedwithinvalidstatus_x       0x827750
#define CountryCodesResponse__Parse_failedwithinvalidstatus_x        0x827C80
#define PreviewOrderResponse__Parse_failedwithinvalidstatus_x        0x828000
#define AccountPaymentInfoResponse__Parse_failedwithinvalidstatus_x  0x828E90
#define RedeemCodeResponse__Parse_failedwithinvalidstatus_x          0x829340
#define HttpHandler__OnTerminated_disconnectReason__s_d_WSAGetLastError__d_x 0x82DDC0
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x840B40
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x840C20
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x840D00
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x843650
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x843730
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x843810
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x8438F0
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x8439D0
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x843AB0
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x843B90
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x843C70
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x843D50
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x843E30
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x844390
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x844D60
#define FunctionObjectT__GiveTime_cancellingtrackingId__uwithmsecElapsedMax__d__msecElapsed__d_x 0x844E40
#define File__Writefailure__x                                        0x8FC5D0
#define File__Openfailure__s_x                                       0x8FCEA0
#define SslConnectionFilterImpl__HandleError__s_d_x                  0x906680
#define SslConnectionFilterImpl__HandleError__s_d_x                  0x906720
#define SslConnectionFilterImpl__DoConnect_SslConnectionFilter__cStatusConnected__status__d_x 0x906870
#define SslConnectionFilterImpl__Send_notconnected_x                 0x906A50
#define SslConnectionImpl__DoHostCheck_failedtomatchhost__sandsubject__s_x 0x906B40
#define SslConnectionFilterImpl__DoAuthenticate_err__d_x             0x906CD0
#define SslConnectionFilterImpl__ConfigAsServer_nullsslcertificatefileand_orkeyfile_x 0x906E40
#define SslLibrary__VerifyCallbackfailed_err__d_s_s_depth__d_x       0x906FA0
#define SslConnectionFilterImpl__ConfigAsClient_BIO_newfailed_x      0x9071D0
#define SslConnectionFilterImpl__HandleError__s_d_s_x                0x9073F0
#define SslConnectionFilterImpl__ReportSSLErrors_err__s_u_x          0x9074A0
#define SslConnectionFilterImpl__AttachServer_alreadyconnected_x     0x9075A0
#define SslConnectionFilterImpl__AttachClient_invalidhost_x          0x907DE0
#define SslConnectionFilterImpl__ConfigServer_nullhandler_x          0x908000
#define SslConnectionFilterImpl__ConfigClient_nullhandler_x          0x908320


// ============================================================
// Section 20: Window Class Virtual Methods (Vtable offsets)
// Extracted by backtracking from confirmed virtual function tables in .rdata
// Index [8] = WndNotification, Index [23] = Draw
// ============================================================
#define CBankWnd__Draw_x                                             0x627850
#define CBarterWnd__Draw_x                                           0x62EFC0
#define CBarterSearchWnd__Draw_x                                     0x62BC40
#define CBuffWindow__Draw_x                                          0x6415F0
#define CChatWindow__Draw_x                                          0x6558A0
#define CGroupWnd__Draw_x                                            0x67C940
#define CTargetWnd__Draw_x                                           0x76AFB0
#define CSidlScreenWnd__Draw_x                                       0x640DB0


// ============================================================
// Section 21: Mapped Game Structure Offsets (Decompiled)
// Semantically resolved from decompiled player/actor initializers
// ============================================================
#define PlayerClient__Floor_Z_offset                                 0x28   // Floor Z height coordinate (float)
#define PlayerClient__X_Coordinate_offset                            0x68   // X-coordinate position (float)
#define PlayerClient__EquipmentArray_offset                          0x2E8  // Visible equipment slot items array (int[13])
#define Spawn__ActorDefName_offset                                   0xEBA  // Actor Definition Name string (e.g. ELF_ACTORDEF)

// ============================================================
// Section 24: Player Data Set (PDS) Offsets
// ============================================================
#define PlayerProfile__PDSMaxMana_offset                         0x578
#define PlayerProfile__PDSNoBuffMaxMana_offset                   0x5C0
#define PlayerProfile__PDSNoBuffStamina_offset                   0x590
#define PlayerProfile__PDSNoBuffAgility_offset                   0x5A0
#define PlayerProfile__PDSNoBuffWisdom_offset                    0x5A4

// ============================================================
// Section 25: Inventory and UI Classes (Decompiled)
// ============================================================
#define CParamInvSlot__Constructor_x                                 0x8A99B0
#define CInvSlotTemplate__Constructor_x                              0x88FE50

// ============================================================
// Section 26: Spell, Buff, and Targeting Classes (Decompiled)
// ============================================================
#define CSpellGemTemplate__Constructor_x                             0x88FD20
#define CParamSpellGemDrawTemplate__Constructor_x                    0x8A1700
#define CParamSpellGem__Constructor_x                                0x8A97D0
#define CCollisionInfoTargetVisibility__Constructor_x                0x8D4570
#define ReqHasMemorizedSpell__Constructor_x                          0x7D1E30
#define ReqHasScribedSpell__Constructor_x                            0x7D1E70


// ============================================================
// Section 27: Zone and Networking Classes (Decompiled)
// ============================================================
#define ReqCreationServer__Constructor_x                             0x7D1040
#define ReqZoneId__Constructor_x                                     0x7D1DB0
#define EQZoneInfo__Constructor_1_x                                  0x7DC290
#define EQZoneInfo__Constructor_2_x                                  0x7DC9E0
#define ZoneGuideManagerBase__Constructor_x                          0x7FEC80
#define LogicalPacket__Constructor_x                                 0x8CE150
#define SimpleLogicalPacket__Constructor_x                           0x8CE170
#define PooledLogicalPacket__Constructor_x                           0x8CE200

// ============================================================
// Section 28: Items, Skills, and Miscellaneous Classes (Decompiled)
// ============================================================
#define ItemBase__Constructor_1_x                                    0x555CA0
#define ItemBase__Constructor_2_x                                    0x7BB4B0
#define ItemBase__Constructor_3_x                                    0x7BCB00
#define FinderItem__Constructor_x                                    0x8BAED0
#define RewardItem__Constructor_x                                    0x7F2610
#define ReqMinSkill__Constructor_x                                   0x7D1930
#define ReqMaxSkill__Constructor_x                                   0x7D1990
#define ReqSkillRange__Constructor_x                                 0x7D19F0
#define ReqHasWornItemClassInSlot__Constructor_x                     0x7D1EB0

// ============================================================
// Section 29: Combat, Melee, and Defensive UI Classes
// ============================================================
#define CombatSkillSelectWnd__Constructor_x                          0x4771D0
#define CombatAbilityWnd__Constructor_x                              0x65B3C0
#define CMemoryBlock__Constructor_x                                  0x84D970
#define PlayerProfile__PDSAttackValue_offset                     0x584
#define PlayerProfile__PDSArmorValue_offset                      0x588
#define PlayerProfile__PDSNoBuffAttackValue_offset               0x5CC
#define PlayerProfile__PDSNoBuffArmorValue_offset                0x5D0

// ============================================================
// Section 30: Guild, Texture, and Animation Classes
// ============================================================
#define GuildBase__Constructor_1_x                                   0x7DAB00
#define GuildBase__Constructor_2_x                                   0x7DABF0
#define ReqGuildStatus__Constructor_x                                0x7D1B10
#define ReqTextureType__Constructor_x                                0x7D1D70
#define CTextureAnimation__Constructor_x                             0x87A9A0
#define CTextureFont__Constructor_x                                  0x889880
#define CStaticAnimationTemplate__Constructor_x                      0x88F390
#define CStaticTintedBlendAnimationTemplate__Constructor_x           0x890B50
#define CParamUi2DAnimation__Constructor_x                          0x8A8010
#define CParamStaticAnimation__Constructor_x                         0x8A8B40
#define CParamStaticTintedBlendAnimation__Constructor_x              0x8A8BD0

// ============================================================
// Section 31: Tasks and Engine Base Classes
// ============================================================
#define CEverQuestBase__Constructor_1_x                              0x532640
#define CEverQuestBase__Constructor_2_x                              0x542510
