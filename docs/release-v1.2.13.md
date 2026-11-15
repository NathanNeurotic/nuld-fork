Apps from your home server, OPL's ART folder, and square covers for PS1 games and apps.

## New

- **Apps from the home server.** The catalog now also lists the server's `APPS/` folder, in OPL's layout (a loose
  `.elf`, or a folder with `title.cfg` holding `title=` and `boot=`). Pressing X downloads the ELF to
  `nuld/red.elf` on the USB and runs it. Only the ELF is downloaded: an app that reads other files from its own
  folder still has to be on the USB. Needs the server's `nuld_catalog.py` from this zip's `server/`; restart it with
  `systemctl --user restart nuld-catalog`.
- **OPL's ART folder (#16).** Covers are read straight from `ART/`, with OPL's names: `ART/SLUS_209.46_COV.jpg` or
  `.png` for games, `ART/<ELF file>_COV.png` for apps (for example `ART/OPNPS2LD.ELF_COV.png`). The launcher looks
  in the `ART/` folder of its own drive and of the game's drive. It converts each image once into `covers/`, and
  does not download a cover for that game from the internet. Transparent PNGs are drawn over NULD's background.
- **Square covers for PS1 games and apps (#27).** PS1 covers are no longer squashed into the DVD case shape: they
  are drawn square, like a CD case, and so are app covers. PS1 covers downloaded by earlier versions are downloaded
  again in the new shape. `tools/covers.py --square` makes square covers by hand; app covers made earlier with
  `covers.py` have to be made again with `--square`.
- **Games on a udpfs server reconnect after a lost link.** Before, once udpfs gave up on the server, every later
  read failed and the game crashed at its next load. Now it finds the server again (for up to about a minute) and
  retries the read. Not tested on a console yet.

## Updating

With the network cable in, START at the update notice. Otherwise copy `launcher.elf` **and** `neutrino/` from the
zip over the ones on your drive.
