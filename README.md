# RoF2 client offsets

Reverse-engineering notes and offsets for the EverQuest Rain of Fear 2 (RoF2)
client, `eqgame.exe` build dated May 10 2013.

## Files

- `eqgame.h` - main offset list: functions, globals, opcodes and struct layouts.
- `Achievement.h` - achievement window and data offsets.
- `Chat.h` - chat window and chat channel offsets.
- `Fellowship.h` - fellowship window and data offsets.
- `Housing.h` - player housing: placement, estates, plots and their packets.
- `HousingNpc.h` - housing NPC (placed NPC) offsets.
- `Market.h` - marketplace and Station Cash store notes (static analysis).
- `LICENSE` - GNU General Public License, version 2.

## Addresses

Every address is for that exact build of `eqgame.exe`. They will not match any
other build. Addresses that have not been checked in a running client say so
where they appear.

## License and credit

Most headers are based on MacroQuest code, Copyright (C) 2002-present
MacroQuest Authors, released under the GNU General Public License, version 2.
The copyright notices are kept in each file. This collection is released under
the same license; see `LICENSE`.

## Not included

Nothing here is about any server project, and there is no anti-cheat material.
