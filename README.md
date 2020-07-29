# HM-Studio
This is a program to edit HM games.
HM Studio is a tool to edit Harvest Moon Games. Currently, it supports:
* Harvest Moon - Friends of Mineral Town (American Version)
* Harvest Moon - More Friends of Mineral Town (American Version)
* Harvest Moon -  DS (American Version)
The support for HM FoMT and DS are not 100%

## To Do List

### For the next Release

- [ ] Make ScriptEditorFrame inherit from EditorFrame
- [ ] Insert Items from dump.
- [ ] Default ROM option.
- [ ] Open ROM with default emulator.
- [ ] Open Project Folder.
- [ ] Show also original size, line and col in status bar in script editor.
- [ ] Disable the spellchecker in the original script.
- [ ] Allow select dictionary or disable spellchecker.
- [ ] Move Script To.
- [ ] Performance update in GraphicsEditor drawing.
- [ ] Nicer GUI for MainFrame

### For future releases

* Convert solution to CMake
* Support opening random ROM with a text file configuration.
* See the face and name of who is talking in a script (Reverse-Engineering).
* Change the stamina/fatigue values of fridge itens (Reverse-Engineering).
* Delete and re-write all code in GraphicsEditor, to make it load all graphics automatically (Reverse-Engineering).

Things marked with (Reverse-Engineering) needs research to be implemented. If you know reverse-engineering, please, feel free to try.

## Overview

### MainScreen

![MainFrame](https://i.imgur.com/BlAWLmd.png)

1. Open the ROM with the default emulator (Unimplemented)
1. Create a backup copy of the ROM (Unimplemented)
1. Select current working ROM
1. Always open HM Studio with the current ROM selection (Unimplemented)
1. Open one of the editors with the current ROM selection

### Script Editor

![ScriptEditor](https://i.imgur.com/CdJ9Rr1.png)

1. Go to the previous script. Show an error message if the script was deleted or has no text.
1. Go to the following script. Same as 1.
1. Go to a script by it's number.
1. Save the script file.
1. Insert the script. If the script is larger than the old, it will try to find space in the ROM.
1. Check the code of all script in the ROM.
1. Save the string. Does not affect the script file.
1. Go to the previous string. Do nothing if the first string.
1. Go to the following string. Do nothing if the last string.
1. The index of the current string and the string count.
1. The size of the translated text.
1. The line where the cursor is of translated text.
1. The size of the current selection in translated text.
1. The cursor column in the translated text.
