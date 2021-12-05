# HM-Studio
This is a program to edit HM games.
HM Studio is a tool to edit Harvest Moon Games. Currently, it supports:
* Harvest Moon - Friends of Mineral Town (American Version)
* Harvest Moon - More Friends of Mineral Town (American Version)
* Harvest Moon -  DS (American Version)
The support for HM FoMT and DS are not 100%

# Overview

### Script Editor

![Script Editor](https://i.imgur.com/hcEUc3Z.png)

## Building

### Linux

```shell
git clone --recurse-submodules https://github.com/Moonslate/HM-Studio.git
cmake CMakeLists.txt
make
```

### Windows

You need an envoirment variable pointing to wxWidgets and (optionally) Hunspell root folder

```shell
git clone --recurse-submodules https://github.com/Moonslate/HM-Studio.git
Open folder with Visual Studio or VS Code
Select a configuration either Release or Debug
Hit Build
```
