# Legends of Norrath: home screen panels and the dead link buttons

Researched 2026-10-06. Static RE only: Ghidra + capstone on `LegendsOfNorrath.dll` (image base 0x10000000),
plus the forms extracted from `resources.rcc`. Nothing was edited, built, copied or run.

Our client copy of the DLL differs from the stock copy in 29 bytes only, all at file offset ~0x62B8D9 (the
gateway host literal our launcher already writes). Every address below is therefore the same in both copies.

**Correction to `research/lon_leaderboard_button.md`:** that note says the Leaderboard button goes "straight to
`QDesktopServices::openUrl`, not through EverQuest's URL callbacks". That is wrong. LoN registers its own handler
for the `http`, `https` and `help` schemes (section 1.2). So every one of these buttons ends in
eqgame's URL callback slot 0, and **our dinput8 `OpenUrl` (nms_lon_link.cpp) already drops every URL it does not
map.** That is why the buttons "do nothing" today. It also means the buttons can be re-pointed in our own DLL,
with no LoN byte patch and no string-length limit.

---

## 1. The five buttons

### 1.1 Where they are

None of them is on the home screen. They are menu items in the navigator's pop-up menus, built in the EQNavigator
init `0x10472400` (EQNavigator vtable `0x10649714`, slot +0xC8). The constructor `0x102CA6A0` is called
unconditionally from the main-window init `0x10460F10` (`0x104612D3`), then init runs via vt+0xC8 (`0x104612ED`).

Each item is a `QWidgetAction` whose default widget is a `WAMapScreenButton` (a `QPushButton` with title and
description labels, ctor `0x10463B30`). It is made by `0x104718C0`, which also connects `clicked()` to
`trigger()`. The action pointer is stored in the navigator:

| Button | Menu | Action stored at | Signal -> slot | Slot body |
|---|---|---|---|---|
| Forums | Community (+0x40) | nav **+0x7C** | `goToForums()` | EQNavigator vt+0x118 -> **`0x102C8D60`** |
| Leaderboards | Community (+0x40) | nav **+0x80** | `goToLeaderBoards()` | EQNavigator vt+0x11C -> **`0x102C8E80`** |
| Website | Community (+0x40) | nav **+0x84** | `goToWebSite()` | EQNavigator vt+0x114 -> **`0x102C8CE0`** |
| Review EULA | Misc (+0x44) | nav **+0x8C** | `startEULAInterface()` | **`0x10470C30`** |
| Help | Misc (+0x44) | nav **+0x90** | `goToHelp()` | **`0x1046FC70`** |

The slot mapping is proven. EQNavigator's `qt_metacall` (`0x104D4110`) sends its own slots 0/1/2 to vt+0x114/+0x118/+0x11C.
The base WANavigator `qt_metacall` (`0x104E0E90`) handles the rest. Its case 0x0D is `0x10470C30` and case 0x18 is
`0x1046FC70`. The case numbers follow the moc names at `0x106B4378` (startEULAInterface) and `0x106B4463` (goToHelp).

The Community and Misc nav buttons call vt+0x138 (`0x1046F900`) through `0x1046F990` and `0x1046F9B0`.
**`0x1046F900` first calls vt+0x134 (`0x104712C0`, "update actions") and only then calls `QMenu::popup`.**

### 1.2 What each one does when clicked

| Button | What happens |
|---|---|
| Website | `QDesktopServices::openUrl("http://legendsofnorrath.station.sony.com/")`. The literal is at `0x10649BE8` (41 chars). |
| Forums | `openUrl("http://forums.station.sony.com/lon/index.php")`. The literal is at `0x10649C14` (44 chars). It is the same literal in all three language branches (`FUN_104AEB40` 2/3/other). |
| Leaderboards | `openUrl("http://legendsofnorrath.station.sony.com/d/ratings/")`. The literal is at `0x10649C44` (51 chars). |
| Help | `openUrl("help://index")` (`0x1046FC70`). The registered `help` handler `WABrowserControl::goToHelp` (`0x103A7EF0`) builds `"%1/help/%2.vm"`. `%1` is the browser control's +8 base (`http://legendsofnorrath.station.sony.com`, set by `0x103A7420` from `0x1062B884`) or its +0xC base (`...-test...`, `0x1062B854`). It picks between them on `FUN_1045DA90() != 5`. Which branch is live is UNVERIFIED. `%2` is the authority `index`. French uses `%1/fr/help/index.vm` and German uses `%1/de/help/index.vm`. The result is passed to `goToUrl`. |
| Review EULA | Opens or raises the `eulaDialog` (`WAEULADialog`, ctor `0x103F5B30`, form `:/forms/dialogs/eula.ui`). Mode byte +0x4C is 0 on this path, so the setup at `0x103F5C20` **hides Accept and Decline and shows only Close.** The text, version and date come from the account singleton (`FUN_10068600`). They are only filled by a server `NetworkCommand_EULA` (0x1C8), which world never sends. So the dialog is blank ("Version: 0", a 1970 date). It is harmless, just empty. |

**The URL path (proven).** `WABrowserControl` (ctor `0x103A70A0`, created at app init via `0x103A72C0` from
`FUN_101E2BE0`) calls `QDesktopServices::setUrlHandler` for `store`, `help`, `guild`, `http` and `https`
(`0x103A70A0` body). Qt4's `openUrl` consults that registry first. So `openUrl(http://...)` goes to `goToUrl(QUrl)`
(`0x103A7610`), then to `0x103A7320`, then to `(*DAT_1078C628)[0](url)`, or to `[1](url, post)` when there is post
data. `DAT_1078C628` is set to `app+0x34` in `0x101E2CB6`. According to our DLL's existing comment ("the card game
copies the table in Initialize", nms_lon_link.cpp), that is the copy of eqgame's table at `0x00F8034C`. I did not
re-trace that copy here. Our `ReplaceUrlCallbacks` sets slot 0 to `OpenUrl`, which drops every URL it does not
map.

### 1.3 What can re-enable a disabled action

- `0x104712C0` (vt+0x134) re-enables **Help (+0x90)** and **EULA (+0x8C)** every time it runs while connected
  (`FUN_10068520() != 0`), and disables them while disconnected. It never touches +0x7C, +0x80 or +0x84.
  It runs on every Community/Misc menu open (`0x1046F908`) and on custom event 0x12 (`0x10471070`, eventFilter).
- `0x1046F8D0` (updateButtonAccessibility, vt+0x124) only touches the card-store button (+0x2C).
- No other `QAction::setEnabled`/`setVisible` call in the navigator range touches the five slots. I checked every
  `call [setEnabled/setVisible]` site in `0x1046E000..0x10473000`.

So a one-time disable sticks for Forums, Leaderboards and Website, but Help and EULA must be disabled again after
every `0x104712C0` run.

### 1.4 Greying them: the safe way

**There is ready-made code in LoN.** `0x10471220` is `__stdcall void disableAction(QAction*)` and `0x10471270` is
`enableAction(QAction*)`. Each calls `QAction::setEnabled(b)`, then `QMetaObject::cast` to `QWidgetAction`, then
`defaultWidget()->setEnabled(b)`, and ends with `ret 4`. Neither touches ECX on entry. They have no direct callers:
`0x104712C0` has the same sequence inlined. A disabled `QAction` does not fire `triggered()`. The default widget
being disabled also stops `clicked()`. `:/style/navigation.qss` already styles it:
`QPushButton:disabled { background: url(:/images/navigation/menu_item_disabled.png); }`, so it really looks grey.

**Recommended patch (P8, same pattern as P1-P7):** replace the EQNavigator vtable slot at **RVA 0x649848**
(vt `0x10649714` + 0x134, original value **`0x104712C0`**) with our wrapper:

```cpp
// __thiscall, no stack args -> __fastcall(ecx, edx) returns with plain RET: matches.
void __fastcall NavUpdateActions(void *nav, void *)
{
    g_origUpdateActions(nav);                       // 0x104712C0 via a __fastcall typedef
    BYTE *n = (BYTE *) nav;
    auto set = [&](DWORD off, bool on) {
        void *act = *(void **)(n + off);
        if (act) (on ? g_enableAction : g_disableAction)(act);   // 0x10471270 / 0x10471220, __stdcall(void*)
    };
    set(0x7C, LinkOn(kForums));
    set(0x80, LinkOn(kLeaderboards));
    set(0x84, LinkOn(kWebsite));
    if (!LinkOn(kEula)) set(0x8C, false);           // ON: keep stock connected/disconnected logic
    if (!LinkOn(kHelp)) set(0x90, false);
}
```

All-or-none byte checks, as in `PvPHostPulse`:
- the slot holds `0x104712C0 - 0x10000000 + base`
- the helper bytes at RVA `0x471220` = `56 8B 74 24 08 6A 00 8B CE FF 15` and at RVA `0x471270` = `56 8B 74 24 08 6A 01 8B CE FF 15`
- the update-actions bytes at RVA `0x4712C0` (take its first 8 to 12 bytes from the image when building)

The vtable is in `.rdata`, so use `PAGE_READWRITE` as `PvPWriteSlot` does. Patching after the navigator exists is fine:
objects hold a pointer to the vtable, and the vtable entry is read on each call.

First-paint timing: the menus are hidden until clicked, and every open calls vt+0x134 before `popup`
(`0x1046F900`), so the items are already grey the first time anyone sees them. Turning a flag on or off at runtime
takes effect on the next menu open.

Alternative without the LoN helpers: call the QtGui4 exports directly through `GetProcAddress`
(`?setEnabled@QAction@@QAEX_N@Z`, `?defaultWidget@QWidgetAction@@QBEPAVQWidget@@XZ`,
`?setEnabled@QWidget@@QAEX_N@Z`). Both ways work. The helpers need fewer moving parts.

**Not recommended:**
- `QAction::setVisible(false)`. It hides the item instead of greying it. The design calls for grey.
- Editing `resources.rcc`. The `.ui` and `.qss` files are XOR 0x73 obfuscated inside an 80 MB rcc. The navigator
  items are built in code, not in a `.ui`, so a resource edit cannot disable them. It also could not be switched.

### 1.5 The home screen "View More..." button

`viewLeaderBoardsButton` in `map.ui` maps to `WAMapScreen` moc slot 8 (`0x10461740`), which calls
`navigator->vt+0x11C`. That is **the same `goToLeaderBoards` (`0x102C8E80`)**. Disabling the Leaderboards action
does not grey this button. Its URL does go through the same `OpenUrl` mapping (1.6), so with the switch OFF it does
nothing, as it does today.

Optional greying: hook `WAMapScreen` vt+0xF8 (**RVA 0x68EDCC**, original **`0x10462FB0`**, the screen setup;
`__thiscall(this, QWidget *root)`, `ret 4`). After the original runs, find `"viewLeaderBoardsButton"` with QtCore4's
`qt_qFindChild_helper` and `QWidget::setEnabled(false)`. This needs Qt ABI calls from our DLL (QString build). It
works, but there is more to get wrong. The simple fallback is to leave it enabled and inert.

### 1.6 Re-pointing the URLs (no LoN patch, no length limit)

Do it in `OpenUrlPost` in nms_lon_link.cpp. The incoming string is `QUrl::toString().toLatin1()`:

| Incoming URL contains | Button | Map to |
|---|---|---|
| `legendsofnorrath` + `/d/ratings/` | Leaderboards, View More | configured leaderboard URL |
| `forums.station.sony.com/lon/` | Forums | configured forums URL |
| exactly `http://legendsofnorrath.station.sony.com/` | Website | configured website URL |
| `station.sony.com` + `/help/` + `.vm` | Help | configured help URL |
| `csgPurchaseFlow` / `loginWithoutTicket` | store | dropped (the store button does nothing; the receiving server must provide its own way to sell packs) |
| anything else (support FAQ `0x1069EEB0`, registration `0x1068E280`, links in MOTD/news text) | | drop, or open only when it starts with an allowed prefix (see the security note) |

Open the URL with `ShellExecuteA(nullptr, "open", url, ...)`. **Security:** these URLs come from the server.
Only pass a string that begins with `https://` (or `http://`) and contains no quotes or control characters. Never
pass `file:`, a path, or a bare command to `ShellExecute`.

Fallback, if a literal patch is ever wanted instead: the three `.rdata` literals can be overwritten in place, like
`PatchGatewayHost`. The new URL must not exceed the old length: Website ≤ 41, Forums ≤ 44, Leaderboards ≤ 51
characters plus NUL, zero-filled. The Help base `0x1062B884` is shared by every `WABrowserControl` URL, so do not
patch it. The `OpenUrl` mapping makes all of this unnecessary.

### 1.7 The ON/OFF switch: how the DLL learns it

**(a) Server rule via OP_NMSLoNSession (0x141C): recommended.**
`NMSLoNSession_Struct` (`common/eq_packet_structs.h:1696`, 172 bytes, static_assert) already has
`uint32 reserved` at offset **168**. Zone sets it to 0 (`zone/nms_lon_session.cpp:76`). The DLL ignores it
(`OnSession` in nms_lon_link.cpp reads token/host/port only and accepts `size >= 172`).

- Bitmask in `reserved`, **bit set = ON**: 1 Help, 2 EULA, 4 Forums, 8 Leaderboards (and View More), 16 Website.
  An old server sends 0, which means all OFF, which is the intended default. An old DLL ignores the field.
- URLs: append a tail after the 172 bytes. For example `uint32 magic 'NMSU'; char help[128], forums[128],
  leaderboard[128], website[128];` (688 bytes total). Old DLLs read only the first 172 bytes (`size >= 172` check),
  so this is safe. Keep the existing `static_assert(172)` for the head struct and add a v2 struct with its own
  assert. An empty URL forces that button OFF.
- Rules (zone reads them in `SendLoNSession`): `Custom:LoNLinkButtons` (int, default 0) and
  `Custom:LoNHelpUrl` / `LoNForumsUrl` / `LoNLeaderboardUrl` / `LoNWebsiteUrl` (string, default ""). Change them with
  `#rules setdb` (from a zone that uses the default ruleset), then `#reload rules global`. The DLL picks them up on the
  next zone-in (the packet is re-sent on every zone-in), and the menus refresh on the next open.
- Opcode and conf already exist. Only the struct, the zone sender and the DLL reader change.

**Peeking a LoN message in the relay** is possible, but it is worse. It would need a fake LoN class world invents
(an unknown class id jams the stream, `research/lon_pvp_build_spec.md` 4.1) or parsing real frames. Not recommended.

**(b) Client ini.** `GetPrivateProfileString` is already used in the DLL (chat_tabs_window.cpp:1537). Good for a
local override while testing, but every player would need the file edited. Use it only as an override:
`[LoN] LinkButtons=-1` (-1 = follow the server).

**(c) Both.** Server is the source, the ini can override. Recommended: (a), with (b) as a dev override.

---

## 2. The home screen panels (WAMapScreen, `:/forms/screens/map.ui`)

The screen is `WAMapScreen`, vtable `0x1068ECD4`. Setup is vt+0xF8 `0x10462FB0`. eventFilter is vt+0x14
`0x10463650` (custom event type 0x3E9: id 0xAE runs the leaderboard refill `0x10462C20`, id 0xAF runs the news
refill `0x10462960`). MOTD setter is vt+0xC8 `0x10461E10`. Form widgets: `newsLabel` "Getting Started" plus
`newsText` (QTextBrowser, `openExternalLinks=true`), `motdLabel` "Message of the Day" plus `motdText`
(QTextBrowser, `openExternalLinks=true`), and `leaderBoardsLabel` plus `leaderBoardsTreeWidget` plus
`viewLeaderBoardsButton` "View More...". No panel is filled from HTTP, a local file or the `.ui` (newsText's `.ui`
HTML is an empty paragraph and is cleared anyway). **All three are filled only by server messages, which world does
not send today.** That is why they are blank.

Setup `0x10462FB0` reads cached data:
- `newsText` and `motdText` get `:/style/chat.css` as their default stylesheet.
- `motdText.setHtml(mainwindow.map<int,QString>(+0x5C)[1])` (`0x104631AB..0x104631C9`: `FUN_10460380`, then map
  `operator[]` `0x102FB6E0` with key 1).
- `0x10462960` (news), then `0x10462C20` (leaderboard).

So data sent before the home screen appears shows when it opens. Data sent while it is open refreshes through the
events. Live refresh assumes this screen's eventFilter is installed on the poster object, which I did not trace.
UNVERIFIED, but that is the only consumer of events 0xAE and 0xAF.

General wire rules (`research/lon_trade_lobby_spec.md` section 0):
- `U()` = varint; `S()` = `U(len)` + bytes + NUL.
- Class header = `U(cls) U(ver)` once per level. A class id mismatch makes the read fail (`0x10176CA0` logs
  "Wanted classID %d, but got %d" from `0x1061D3C4`).
- Read failure = the LoN stream jams for good (`0x101740E0`, `research/lon_pvp_build_spec.md` 4.1).
- Int list = `U(n)` + n×`U`. String list = `U(n)` + n×`S` (`0x1000A2C0` / `0x1000A340`).
- Execute is vtable slot 15 (+0x3C), as our `kPvPSlotSetup` comment says.
- Strings over 0x1400000 bytes: "Invalid string length", read fails (`0x10097A00`). Text is decoded as UTF-8
  (`QString::fromUtf8`, import `0x105EFCB0`).

### 2.1 Getting Started = `NetworkCommand_News` 0x1C9, version 1, **1 level**

- vtable `0x1061C2C4`. GetClassID `0x10170F70` returns 0x1C9. Version `0x10170FC0` returns 1.
- Read `0x101712B0`: one header (vt+0x10), then string list +0x04, string list +0x14, int list +0x24, then the end
  marker (vt+0x14). There is no NetworkCommand base read, so it is 1 level.
- The client never sends it (only factory `0x10171210` and clone `0x10171080` construct it).
- Execute `0x101713E0`: for i < count(titles), it builds `{title, body, key}` (0x3C-byte entries via
  `0x10171330`). It assigns them to singleton `0x1078BE78` +0xE8 (`0x1006C8A0` -> `0x1006C360`, which **replaces**,
  not appends). Then it posts event 0xAF (`0x10171576`).
- Display `0x10462960`: copies the list, sorts it by key **descending** (`0x104627D0` with comparator `0x104615A0`:
  `a.key > b.key`), clears `newsText`, then for each entry
  `insertHtml("<b>" + title + "</b><br>" + body + "<p>")` (`0x10631354`, `0x1068EDF8`). Then it moves the cursor to
  the start.
- **It is rich text** (a Qt4 HTML subset, with `chat.css` as the default style). Links open through `openUrl`, which
  goes to our `OpenUrl` (1.6). So to make a link work, allow its prefix there. Titles and bodies are inserted as raw
  HTML: world must HTML-escape any text it did not write itself. There is no length cap beyond the 20 MB string limit.
  Keep it to a few KB.

```
Header(0x1C9,1,1)                      29 1C 01
U(n)  S(title_1) .. S(title_n)
U(n)  S(body_1)  .. S(body_n)          // MUST be >= n entries
U(n)  U(key_1)   .. U(key_n)           // MUST be >= n entries; larger key is shown first
```
Example, one item ("Welcome", "Hi", key 1): `29 1C 01 01 07 57 65 6C 63 6F 6D 65 00 01 02 48 69 00 01 01`

### 2.2 Leaderboards = `NetworkCommand_LeaderBoardData` 0x1CA, version 1, **1 level**

- vtable `0x1061C1CC`. GetClassID `0x1016FE90` returns 0x1CA. Version `0x1016FEE0` returns 1. Never sent by the
  client.
- Read `0x10170260`, **wire order**:

| # | Field | Type |
|---|---|---|
| 1 | +0x34 | int list (group B ids) |
| 2 | +0x44 | string list (group B names) |
| 3 | +0x54 | int list (group B ratings) |
| 4 | +0x04 | int list (group A ids) |
| 5 | +0x14 | string list (group A names) |
| 6 | +0x24 | int list (group A ratings) |

- Execute `0x101703C0`: group A becomes 0x24-byte entries `{int id @+0, string name @+4, int rating @+0x20}`
  (`0x10170310`) and goes to singleton +0xF8 (`0x1006C910`). Group B goes to singleton +0x108 (`0x1006C960`).
  Both are replaced, not appended. Then event 0xAE is posted (`0x10170617`).
- Display `0x10462C20`: headers "Rank" (`0x1068AA70`), "Rating", "Name" (`0x1068AF00`). Column 0 width is 0x23.
  It shows **group A only** (singleton +0xF8 via `0x1006C300`). Group B (+0x108 via `0x1006C330`) is copied but
  never displayed: the only reader of +0x108 is this function, and it ignores it.
  - Column 0 = row index 1..n, computed by the client.
  - Column 1 = `QString::number(rating)`.
  - Column 2 = `fromUtf8(name)`, **plain text** (`QTreeWidgetItem::setText`, no HTML).
  - The id is not shown.
  - There is **no row cap**: every entry becomes a row, and the tree scrolls. Rows appear in the order sent, so
    world sorts them.
- **Which rating it shows is up to world.** The client has no casual or tournament notion here. Recommendation: a
  rule `Custom:LoNHomeBoard` (0 = `rating_casual`, 1 = `rating_tournament`, default 0) and `Custom:LoNHomeBoardRows`
  (default 10). The default of 10 is my suggestion; Sony's count is unknown.
- **"View More..."** = navigator vt+0x11C (1.5). It opens the leaderboard URL. It does not request more rows.

```
Header(0x1CA,1,1)                      2A 1C 01
U(0) U(0) U(0)                         // group B empty (safe: loop guards on a null begin pointer)
U(n) U(account_id_1) .. U(account_id_n)
U(n) S(char_name_1)  .. S(char_name_n) // MUST equal n
U(n) U(rating_1)     .. U(rating_n)    // MUST equal n
```
Query: `SELECT account_id, char_name, rating_casual FROM lon_pvp_stats WHERE ranked = 1 AND hidden = 0
ORDER BY rating_casual DESC, games DESC LIMIT {rows}` (index `board_casual (hidden, ranked, rating_casual)`
exists). Ratings are signed; `U()` handles negatives.
Example, one row (account 5, "Bob", rating 1500): `2A 1C 01 00 00 00 01 05 01 03 42 6F 62 00 01 2C 5D`
(1500 = 0x5DC, so the bytes are `2C 5D`).

### 2.3 Message of the Day = `LoginCommand_DisplayMOTD` 0x135, version 1, **3 levels**

- vtable `0x10617E3C`. GetClassID `0x10157190` returns 0x135. Version `0x101571E0` returns 1.
- Read `0x10157460`: leaf header, then `0x10157000` (LoginCommand base = header + `0x1016C670` NetworkCommand base =
  header), so 3 headers. Then `S(+0x04)`, `U(+0x20)`, `U(+0x24)`, `S(+0x28)`.
- Execute `0x101572A0`: no state gate. Posts event 0x54 with args (str +4, int +0x20, int +0x24, str +0x28).
- Main-window eventFilter `0x104603B0`, case 0x54 (`0x104606B6..0x1046070B`): key = arg 1 (+0x20) if it is an int,
  else 0. text = `fromUtf8(arg 3)` (+0x28). Then `0x10460300`: `map(+0x5C)[key] = text`, then
  `currentScreen(+0x14)->vt+0xC8(text, key)`.
  - On the **home screen**, vt+0xC8 = `0x10461E10`: `motdText.setHtml(text)`. The key is ignored, so any key
    overwrites the box.
  - On **any other screen**, vt+0xC8 = `0x104A6340` (WAScreen base; also Login, Lobby, Trade, Game and others):
    it **pops up a `WAMOTDDialog`** named `motdDialog_<key>` (`:/forms/dialogs/motd.ui`, 400×122, chat-style
    `:/style/prompt.css`), or reuses the existing one.
  - The home screen's own setup always reads key **1** (`0x104631A8`).
- `+0x04` and `+0x24` are unused on this path. Send "" and 0.
- Rich text: yes (`setHtml`), and links open through `OpenUrl`.

```
Header(0x135,1,3)                      25 13 01 25 13 01 25 13 01
S("")                                  00 00
U(1)                                   01          // key: 1 = the home screen box
U(0)                                   00
S(html)
```
Example ("Hello"): `25 13 01 25 13 01 25 13 01 00 00 01 00 05 48 65 6C 6C 6F 00`

**Timing caveat (UNVERIFIED which screen is current):** sent right after `LoginReply` (`lon_service.cpp:1580`),
the current screen is probably still the login screen. The player would then get a MOTD pop-up as well as the home
box (the box reads `map[1]` when the home screen is set up). If no pop-up is wanted, send it later. Which client
message reliably follows the switch to the home screen is not traced. Test it once in game. A pop-up with another key
(for example 2) is a usable broadcast channel, but on the home screen it also overwrites the box, because vt+0xC8
ignores the key.

### 2.4 Where world should send them

In `HandleLogin` after `LoginReply(s, true, 0)` (`lon_service.cpp:1580`), next to `SendAccount` and the others:
- `SendHomeNews(s)`: titles, bodies and keys from a small table or from rules.
- `SendHomeBoard(s)`
- `SendMotd(s)`: see the timing caveat.

Refresh the board to every logged-in session after a rated result (where `lon_pvp_stats` is updated). That is cheap,
and event 0xAE redraws it live. MOTD source: a rule `Custom:LoNMOTD` (string, HTML) or a DB row. Getting Started
source: a table `lon_home_news (id, sort_key, title, body_html, enabled)` (schema migration) or 1 to 3 rules.

---

## 3. Every assert, abort or throw on these paths

| Trigger | Where | Effect | Avoided by |
|---|---|---|---|
| News: bodies or keys list shorter than titles | `0x101713E0`, three `_invalid_parameter_noinfo` checks | VC8 CRT invalid parameter: process terminates. That no custom handler is installed is UNVERIFIED. **EverQuest dies.** | equal counts |
| LeaderBoardData: names or ratings shorter than ids (either group) | `0x101703C0`, `_invalid_parameter_noinfo` ×3 per group | same | equal counts |
| Wrong class id in any header level | `0x10176CA0` -> "Wanted classID" log, read fails | stream jams (`0x101740E0`) | exact `Header(cls,1,levels)` |
| Short or missing field, bad varint | every read above | stream jams | exact layouts above |
| String > 0x1400000 | `0x10097A00` "Invalid string length" | stream jams | keep text small |
| `wa_assert` (`FUN_10097BA0` -> `_CxxThrowException`) | `"clone"` in `0x10170080` (LeaderBoardData clone dynamic_cast); `"WABrowserControl::sInstance == 0"` `0x103A710F` (double create) | EQ exits | not reachable from the wire or from the button paths |
| MOTD event with no current screen (+0x14 null) | `0x1046033E` (`mov edx,[ecx]`) | null dereference. Reachability UNVERIFIED (a screen exists after login). | send after login only |
| `NetworkCommand_EULA` 0x1C8 sent by world | execute `0x1016CF60` posts 0xAD. `0x10475150` then opens the mode-1 EULA dialog with Accept and Decline. **Decline (`0x103F60F0`) closes the main window (LoN exits).** | not an assert, but LoN closes | **do not send EULA** (leave "Review EULA" OFF or blank) |
| Help, Forums, Leaderboards, Website, EULA clicks | `0x1046FC70`, `0x102C8D60`, `0x102C8E80`, `0x102C8CE0`, `0x10470C30`, `0x103A7EF0`, `0x103A7610`, `0x103A7320` | none: no assert on these paths | n/a |
| `0x10462960` news sort guard `_invalid_parameter_noinfo` (end < begin) | `0x10462960` | cannot happen (own copy) | n/a |
| Our P8 vtable or helper patch on an unknown build | n/a | do nothing (all-or-none byte check, log once) | byte checks in 1.4 |

---

## 4. Build plan

1. **DLL: URL map** (nms_lon_link.cpp `OpenUrlPost`): match the five Sony URLs (1.6), look up the
   per-button flag and URL, then `ShellExecuteA` only for `http(s)://` strings. Unknown URLs stay dropped. This
   makes Help, Forums, Website, Leaderboards and View More configurable with no LoN patch.
2. **DLL: P8 grey-out**: replace the EQNavigator vt+0x134 slot (RVA 0x649848, orig `0x104712C0`) with the wrapper in
   1.4, using the helpers `0x10471220` and `0x10471270`. Use all-or-none byte checks, installed beside P1-P7 in the
   per-load pulse. Optional P9: WAMapScreen vt+0xF8 (RVA 0x68EDCC, orig `0x10462FB0`) to grey View More.
3. **Switch: zone + DLL**: put the bitmask in `NMSLoNSession_Struct.reserved` (offset 168), plus a v2 URL tail after
   byte 172. Rules `Custom:LoNLinkButtons` and `Custom:LoNHelpUrl`/`LoNForumsUrl`/`LoNLeaderboardUrl`/`LoNWebsiteUrl`.
   Default 0 means all five grey. Optional ini override `[LoN] LinkButtons`.
4. **World: home panels** (lon_service.cpp, after `LoginReply`): News 0x1C9, LeaderBoardData 0x1CA (top N
   `ranked=1 AND hidden=0`, casual by default) and DisplayMOTD 0x135 key 1, byte-exact as in section 2. Rebroadcast
   the board after rated results. New rules and a table for news are needed.
5. **Leave alone:** `NetworkCommand_EULA` (its Decline closes LoN) and the `.rdata` literals.
6. Test steps:
   - The five items are grey in the Community/Misc menus, and stay grey after reopening and after a reconnect.
   - With a flag ON, the item opens the configured page.
   - The home screen shows Getting Started text, the MOTD and the top-N rows.
   - Note whether a MOTD pop-up appears at login.
