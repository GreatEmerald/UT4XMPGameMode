# UT4XMPGameMode
The XMP gametype for Unreal Tournament 4

To use, clone this repository into the UnrealTournament/UnrealTournament/Plugins directory, with the name "XMPGameMode":

```bash
cd UnrealTournament/UnrealTournament/Plugins
git clone https://github.com/GreatEmerald/UT4XMPGameMode.git XMPGameMode
```

Alternatively, clone it anywhere you want, and make a symlink (this makes is easier to avoid issues with repositories living inside repositories):

```bash
git clone https://github.com/GreatEmerald/UT4XMPGameMode.git
ln -s UT4XMPGameMode UnrealTournament/UnrealTournament/Plugins/XMPGameMode
```

Then just run your UT4 build script and the XMPGameMode will get picked up and built automatically. Run it from the in-game interface, selecting "XMPGameMode" game type.

If you find a bug, you can file it on GitHub in the Issues pane, or note it on the release thread on the UT4 forums.
