# SM64DS Programming Interface

This repository contains header files and symbols that allow using custom C++
code in Super Mario 64 DS mods while interfacing it with the game's original code.
The headers mostly consist of functions, structs and variables from the vanilla game,
as well as some inline functions that may be useful when programming.
(Since almost all SM64DS mods have been patched with [SM64DSe](https://github.com/Gota7/SM64DSe-Ultimate),
some things in the headers may not be accurate to the vanilla game.)

Instead of copying these files to your project,
it's recommended to use this repository as a [Git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

## Credits

These header files and symbols are an aggregation of:
- The original [SM64DS ASM Patch Template](https://github.com/Gota7/SM64DSe-Ultimate/tree/869f496a9920d2825362bc00212b5d0c413ce4e5/ASMPatchTemplate/v1)
by [Fiachra1993](https://github.com/Fiachra1993)
(also see [this post](https://kuribo64.net/board/thread.php?id=1758))
- The [SM64DS ASM Patch Template v2](https://github.com/Gota7/SM64DSe-Ultimate/tree/869f496a9920d2825362bc00212b5d0c413ce4e5/ASMPatchTemplate/v2)
by [hayashi-stl](https://github.com/hayashi-stl)
- Modifications and additions to [hayashi-stl](https://github.com/hayashi-stl)'s
headers and symbols by [pants64DS](https://github.com/pants64DS),
[SplattyDS](https://github.com/SplattyDS) and Overblade
  - See [this](https://github.com/pants64DS/Misc-SM64DS-Patches/tree/c67465dd15a2b1997a1eb0c1a02f4110445310f5),
  [this](https://github.com/SplattyDS/DynamicAllocationDecomp/tree/a6935a5f24f0d0e9126079ca67c48804622c735b)
  and [this](https://github.com/pants64DS/SM64DS-ASM-Reference/tree/9c29ec997a41f3c0108043df561a509177eb86ca)
  for their respective versions predating this repository.
