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
// HousingNpc.h -- placed CREATURES in player housing  (Section 48)
// ============================================================================
// Merchants, mounts, pets and plushies placed inside a house or guild hall.
// Split out of Housing.h (Section 43) because it turned out not to be a
// housing-rendering problem at all: none of these things is a placed object in
// the rendering sense. They are ordinary NPC spawns that happen to carry a
// real-estate key. Section 43 stays the authority on the placed-OBJECT path
// (furniture, paintings, structures); this file owns everything creature.
//
//
// SUMMARY:
//
//   A creature model CANNOT be rendered as a placed housing object. The bag
//   model that appears on a failed placement is not the item's model -- it is
//   the client's hardcoded not-found fallback. Placed creatures must be ordinary
//   NPC spawns.
//
// Evidence base: static analysis of stock RoF2 eqgame.exe (image base 0x400000
// -- REBASE for the ASLR'd running client).
// ============================================================================

// ============================================================================
// 48.1  *** THE BAG IS NOT A MODEL -- IT IS THE NOT-FOUND FALLBACK ***
//       AND CREATURE MODELS CAN NEVER RENDER AS PLACED OBJECTS
// ============================================================================
// A placed item that renders as a bag means ONE thing: the actordef name did
// not resolve. The bag is not the item's art -- it is hardcoded in the actor
// factory as the first fallback. Read a bag as "name lookup missed".
//
// ---------------------------------------------------------------------------
// THE FACTORY, 0x0048B710  (called from the furniture branch at 0x00504E13)
// ---------------------------------------------------------------------------
// It copies the tag (0x40 cap), then decides whether to append the suffix:
//
//   0048b757  <strlen loop over the tag>
//   0048b760  cmp eax, 0x0A            ; strlen(tag) >= 10 ?
//   0048b763  jae 0x48b7a4             ;   yes -> do NOT append
//   0048b769  push 0x9C7BB0            ; "PLAYER_1"
//   0048b76f  call 0x8DD05F            ; strcmp(tag, "PLAYER_1")
//   0048b779  je  0x48b7a4             ;   equal -> do NOT append
//   0048b788  <copies the 10 bytes at 0x9C5708 == "_ACTORDEF" onto the end>
//
// So the rule is exactly: append "_ACTORDEF" iff strlen(tag) < 10 AND the tag
// is not the literal "PLAYER_1".
//
// Then the lookup, three attempts against manager [0x015D46A0] vtbl+0x08:
//
//   0048b7b3  attempt 1: the tag as built
//   0048b7da  attempt 2: "IT63_ACTORDEF"   (0x9C7BA0)   <-- ***THE BAG***
//   0048b820  attempt 3: "BLOB_ACTORDEF"   (0x9C7B90)
//   0048b7d0  all three missed -> return 0, nothing renders
//
//
// "HUM" is 3 chars and not "PLAYER_1", so the factory builds "HUM_ACTORDEF" --
// a name the client knows (it strcmps against that exact literal at
// 0x0058DD69) -- yet the lookup misses. Manager 0x015D46A0 holds OBJECT
// actordefs only; creature/character defs live in a different manager and are
// not reachable from the placed-item factory.
//
// *** A CREATURE MODEL CANNOT BE RENDERED AS A PLACED HOUSING OBJECT. ***
//
// ---------------------------------------------------------------------------
// THE ONLY REMAINING PATH: A REAL SPAWN
// ---------------------------------------------------------------------------
// The NPC branch at 0x00504B60 does not build a creature from the tag when it
// can avoid it -- it ATTACHES AN EXISTING ONE:
//
//   00504b6e  mov eax,[esi+0x30]       ; a spawn id
//   00504b7c  call 0x005996E0          ; spawn-manager lookup by id
//
// EQPlacedItem+0x30 is a SPAWN ID. The tag-driven create at 0x00504C12 is
// only the fallback when +0x30 is 0, and it resolves a 3-char code through the
// registry singleton at [0x00DCFCE0] (getter 0x0050F070, lookup 0x0050A370,
// 2000-entry reserve) which does NOT contain bare race codes, so a bare race
// code produces an invisible NPC.
//
// So placed creatures are real spawns that the placed record points AT.
//
// Named for grepping:
#define EQPlacedItem__BuildActor_x                                  0x00504B60
#define EQPlacedItem__BuildActor_FurnitureBranch_x                  0x00504DCB
#define EQPlacedItem__BuildActor_NpcCreateFromTag_x                 0x00504C12
#define EQPlacedItem__SPAWNID_OFS                                   0x30
#define ActorFactory__CreateFromTag_x                               0x0048B710
#define ActorFactory__FallbackIT63_x                                0x0048B7DA
#define ActorFactory__FallbackBLOB_x                                0x0048B820
#define SpawnManager__FindBySpawnID_x                               0x005996E0
#define SpawnManager__CreateSpawn_x                                 0x00599BD0
#define RaceCodeRegistry__GetInstance_x                             0x0050F070
#define RaceCodeRegistry__Lookup_x                                  0x0050A370
// ============================================================================
