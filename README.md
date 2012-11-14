L4D2 Recording Helpers
======================

This client plugin is simply a set of useful modifications and functions to help making recording from demos easier and more feature filled.

Features
==
- Re-enables many hidden CVars on L4D by removing DEVELOPMENTONLY flags from them. 
- Re-enables a native thirdperson(shoulder) mode in gamemodes with player-controlled zombies.


Usage:
==
Place the files in your l4d2 addons folder and launch the game with -insecure in order to load it.

It may be useful to make a shortcut to l4d2 that launches with -insecure, so you don't have to edit your launch parameters all the time.
e.g. make a shortcut to "C:\Program Files\Steam\steam.exe" -applaunch 550 -insecure

Changelog:
==
0.4:
- Fixed thirdperson patches not working after recent L4D2 Updates
- Initial release of L4D1 build (DEVONLY flag removal only)

0.3:
- Fixed a crash that would occur upon entering thirdperson mode

0.2:
- Added some patches to enable thirdpersonshoulder usage in gamemodes with player controlled zombies.

0.1:
- Initial release
- Unhides DEVONLY CVars

License:
==
Most of the code is GPL. All HL2SDK code is copyright Valve Corporation.