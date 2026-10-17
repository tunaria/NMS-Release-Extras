# NMS-Release Extras

Reference material and add-ons for EverQuest emulator work on the Rain of Fear 2 (RoF2) client.

## Contents

### `offsets/` - RoF2 client offsets and reverse-engineering notes
For `eqgame.exe` built May 10 2013. Every address is for that exact build and will not match any
other build; addresses not checked in a running client say so where they appear.

- `eqgame.h` - main offset list: functions, globals, opcodes and struct layouts.
- `Achievement.h` - achievement window and data offsets.
- `Chat.h` - chat window and chat channel offsets.
- `Fellowship.h` - fellowship window and data offsets.
- `Housing.h` - player housing: placement, estates, plots and their packets.
- `HousingNpc.h` - housing NPC (placed NPC) offsets.
- `Market.h` - marketplace and Station Cash store notes (static analysis).

No anti-cheat material is included.

### `LoN_bundle/` - Legends of Norrath card game install bundle
Published exactly as it was received. It revives the Legends of Norrath card game that ships
inside the RoF2 client: server card service, zone session hand-off, database tables, the client
`dinput8.dll` code and research notes. Start with `LoN_bundle/README.md`.

**Card data is not included.** The bundle's `lon_data/` folder (`campaign.dat`, the
`archetypes/*.dat` files and the `decks/*.eqd` starter decks) is the RoF2 client's own data and is
left out of this repository. Copy it from your own client: everything under
`LegendsOfNorrath\data\` in your RoF2 install goes into the server's `lon_data/` folder, as the
bundle's README describes.

## Student submissions

This repository includes work submitted by students. Contributions are made on the terms in
[CONTRIBUTING.md](CONTRIBUTING.md).

## License and credit

The offset headers are based on MacroQuest code, Copyright (C) 2002-present MacroQuest Authors,
released under the GNU General Public License, version 2; the copyright notices are kept in each
file. The repository is released under the same license; see `LICENSE`.
