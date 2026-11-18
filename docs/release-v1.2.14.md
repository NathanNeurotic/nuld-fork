Covers from your home server, the interface in Portuguese, and the PlayStation 2 logo before the games.

## New

- **Covers from the home server (#19).** The catalog serves the server's `ART/` folder, with OPL's names
  (`ART/SLUS_209.46_COV.png` or `.jpg` for games, `ART/<ELF file>_COV.png` for apps). The launcher asks the server
  for the cover of each of its games and apps before downloading one from the internet. Needs the server's
  `nuld_catalog.py` from this zip's `server/`; restart it with `systemctl --user restart nuld-catalog`.
- **Portuguese interface.** In `config.ini` set `[ui] idioma = pt`; the setup wizard of a new drive offers it too.
  The in-game menu and the achievement notices are in Portuguese as well.
- **PlayStation 2 logo before the games.** In `config.ini` set `[juegos] logo = si` and each game starts with the
  PlayStation 2 logo, as from a disc. Off by default. A game from another region than the console (PAL on an NTSC
  console or the reverse) may not start this way: leave it off for those. Not tested on a console yet.

## Updating

With the network cable in, START at the update notice. Otherwise copy `launcher.elf` **and** `neutrino/` from the
zip over the ones on your drive.
