# RAPTOR: Call of the Shadows 1.2

[![RAPTOR: Call Of The Shadow](https://github.com/whoozle/raptor-call-of-the-shadows/actions/workflows/actions.yml/badge.svg)](https://github.com/whoozle/raptor-call-of-the-shadows/actions/workflows/actions.yml)

Port of RAPTOR: Call Of The Shadows to modern OSes using SDL2/SDL2_mixer

This port is based on the original DOS source code found at
https://github.com/skynettx/dosraptor

I tried to leave as much original code as possible, replacing only DOS or x86 specific routines.

Currently, only Linux/MacOSX supported.

# Building

```
mkdir build
cmake -B build
make -C build
```

# Game files

You will need original game files, you can buy original game at steam

https://store.steampowered.com/app/358360/Raptor_Call_of_the_Shadows_1994_Classic_Edition/
https://store.steampowered.com/app/336060/Raptor_Call_of_The_Shadows__2015_Edition/

# Additional features

Pressing `Ctrl+F` toggles fullscreen.

You can tweak `setup.ini` found in the binary directory of the game

A few additional options are available in this port:

- `Control=0` - set it to 2 to play with game pad.
- `Fullscreen=0` - set it to 1 to start game in fullscreen by default.
- `Zoom=4` - window zoom factor, original resolution is 320x200, so zoom=4 gives you 1280x800
- `GodMode=0` - set it to 1 to start in god mode.
- `SkipCredits=0` - set it to 1 to skip initial credits screen.
- `FrameRate=70` - adjusts game frame rate, larger value makes game run faster.
- `VSync=0` - Vsync, keep it off unless you know what are you doing, original game runs at 70Hz.
- `TaiwanVersion=0` - set it to 1 and see Taiwanese version of this great game.

# Music on MacOSX

To play original music you will need a sound font in sf2 format.

Free sound font could be downloaded from:
https://member.keymusician.com/Member/FluidR3_GM/index.html

Unpack it to the directory with your game files, then adjust setup.ini as needed. 

# Gamepad support

Unfortunately it's not found anywhere in options, however you can turn it on by setting `Control=0` to `Control=2`.
It's possible to remap buttons, but not axes/hats at the moment.
Buttons are in the `[JoyStick]` section of setup.ini.

# Cheats

You can turn on GodMode in original game by setting environment variable `SHOST` to `CASTLE`.
Another method would be to set `GodModel=0` to 1 in `setup.ini`. (Specific to this port).

I wouldn't recommend cheating though, the game is way more interesting without it.
One unexpected side effect of god mode is if you press ESC and say NO, the game counts it as a success.
