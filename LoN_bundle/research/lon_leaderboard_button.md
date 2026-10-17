# Legends of Norrath: the navigator's Leaderboard button

Researched 2026-10-06. Static RE only (Ghidra + a capstone read of `LegendsOfNorrath.dll`, base 0x10000000).
Nothing was patched or built for it.

## Answer

**The Leaderboard button opens a web page in the player's default browser. It is not a built-in screen and
no server message feeds it.** The address is a hard-coded literal; no account property, preference, config
file or server reply changes it.

## Evidence

| What | VA | Notes |
|---|---|---|
| Button / action | `0x10472CC5..0x10472D2D` in the navigator init `0x10472400` | Icon `:/images/navigation/button_image_leaderboards.png` (`0x10694E98`), text "Leaderboards" (`0x1067F6BC`), tooltip "Visit the official leaderboards and check on game stats and ratings." (`0x10694ED0`). Built by `0x104718C0`, stored at navigator `+0x80`, then `connect(action, triggered(), navigator, "1goToLeaderBoards()")` (`0x10472D21`). Created **unconditionally** (no account flag test around it). |
| Slot | EQ navigator vtable slot at `0x10649830` (RVA `0x649830`) -> **`0x102C8E80`** | The EQ navigator vtable (`0x106497DC` holds the init `0x10472400`) overrides the virtual `goToLeaderBoards`. |
| Body | `0x102C8E80` | `QString("http://legendsofnorrath.station.sony.com/d/ratings/")` -> `QUrl` -> **`QDesktopServices::openUrl`**. Nothing else. |
| URL literal | `0x10649C44` (RVA `0x649C44`), 51 characters + NUL | `.rdata`. Its only reference is `0x102C8E98`. |
| Neighbours (same pattern) | slot `0x10649828` -> `0x102C8CE0` (home page `http://legendsofnorrath.station.sony.com/`, `0x10649BE8`); `0x10649C14` (forums) | Same direct `QDesktopServices::openUrl` calls. |

Important: this path does **not** go through EverQuest's URL callbacks (eqgame `0x00F8034C` slots 0/1), which our
dinput8 already replaces (`nms_lon_link.cpp` `ReplaceUrlCallbacks`: URLs it does not map are dropped). So today
the button launches the player's browser at a dead Sony address.

Not related (different features, different data):
- `EventCommand_RequestEventLeaderBoard` / `RespondEventLeaderBoard` and `NetworkCommand_LeaderBoardData`
  feed the **event screen** ("leaderBoardTreeWidget", `ui.waeventscreen.cpp`: rank, name, score, wins,
  losses, "duelistRating") and the **map screen** tree (`ui.wamapscreen.cpp`: rank, name, rating). Those are
  live-event / campaign map features, not the navigator button. Not traced further.

## How to point it at your website later (not done)

Trivially safe, same technique as the gateway host literal (`PatchGatewayHost`, a `.rdata` string written under
`VirtualProtect(PAGE_READWRITE)` right after `LoadLibraryExA` returns, before the button can be clicked):

1. Check the 52 bytes at RVA `0x649C44` equal `http://legendsofnorrath.station.sony.com/d/ratings/\0` (else leave
   it alone: unknown build).
2. Overwrite with a URL of **at most 51 characters** plus NUL, zero-filling the rest, e.g.
   `https://<your-site>/lon/leaderboard`. `QString(const char*)` reads to the NUL, so a shorter string is fine.
3. Data page, not code: `PAGE_READWRITE` is correct here (unlike the P4/P5 code patches).

Until a leaderboard page exists, either leave the button alone (dead Sony link, harmless) or point it at a
placeholder page. A website could read `lon_pvp_stats` / `lon_pvp_match` / `lon_tournament*` to build one:
filter `hidden = 0 AND ranked = 1`, order by `rating_casual` or `rating_tournament`.
