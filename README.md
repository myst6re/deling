# Deling

[![Build Status Linux/OSX](https://travis-ci.org/myst6re/deling.svg?branch=develop)](https://travis-ci.org/myst6re/deling)
[![Build status Win32](https://ci.appveyor.com/api/projects/status/74gm0u6ya9nqxw9u/branch/develop?svg=true)](https://ci.appveyor.com/project/myst6re/deling/branch/develop)

![Deling](images/deling.png)

Final Fantasy VIII field archive editor.

## Building

If you are a developer and you want to build this software, follow the
instructions below.

### Requirements

 - CMake
 - Qt 6.5+
 - LZ4

### Windows

#### Required: Qt

0) Download the online installer from https://www.qt.io/download-qt-installer ( remember to click the Download button )
1) Install Qt with these items checked:
   - **Packages categories:** `Latest supported releases`
   - **Components:** Uncheck everything and pick `Qt/Qt 6.*.*/MSVC 20?? 64-bit`
   - [Optional] To open the project with Qt Creator (easier), check `Developer and Designer Tools/Qt Creator` and `Developer and Designer Tools/CMake`

#### Required: MSVC Compiler

0) Download the the latest [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) installer (2019 or 2022 depending of what you checked when installing Qt)
1) Run the installer, check "Desktop developement C++", or per component, at least:
   - `Compiler/MSVC C++ x64/86` (last version)
   - [Optional] To open the project with Visual Studio or Visual Studio Code, check `Compiler/CMake C++ Tools for Windows`, `Dev/Base C++ features`, `Dev/IntelliCode`

#### Optional: Qt Creator (easier)

Open the project (`File` > `Open File or Project...` > `CMakeLists.txt`) and setup Qt. Select Debug or Release build and then run the project (`CTRL + R`).

To deploy the project with Qt dependencies you can add an extra "cmake install" step in "Run settings" of the project.

#### Optional: Visual Studio

Open this repository **as a folder** and click the build button.

To execute the project you need Qt dependencies where the exe is created. You can do (replace `{QT_DIR}` by where Qt is installed):
```sh
{QT_DIR}/*/msvc2019_64/bin/windeployqt --no-quick-import -xml --translations en {build_dir}
```

#### Optional: Visual Studio Code

1) Download and install the latest [Visual Studio Code](https://code.visualstudio.com/) release
2) Install the following extensions:
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools
3) Open this repository as a folder in Visual Studio code
4) Choose as build profile in the status bar `CMake: [Release]` ( or one of the aforementioned profiles )
5) Click the button on the status bar `Build`

To execute the project, follow the instructions from "Optional: Visual Studio" section.

### macOS

In order to continue please make sure you have `brew` installed. See https://brew.sh/ for instructions.

#### Install dependencies
```sh
$ brew install cmake
$ brew install qt6
```
Close and reopen your terminal.

#### Build

> **Please note**: You can choose other build profiles if you want. See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html for more information.

```sh
$ mkdir -p .dist/build .dist/install
$ cmake -S . -B .dist/build -DCMAKE_INSTALL_PREFIX=.dist/install -DCMAKE_BUILD_TYPE=Debug
$ cmake --build .dist/build --config Debug
```

### Linux

#### Install dependencies

##### Debian/Ubuntu
```sh
$ apt-get install build-essential cmake qt6-default qttools6-dev
```

##### Arch Linux
```sh
$ pacman -S --needed base-devel cmake qt6
```

#### Build

> **Please note**: You can choose other build profiles if you want. See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html for more information.

```sh
$ mkdir -p .dist/build .dist/install
$ cmake -S . -B .dist/build -DCMAKE_INSTALL_PREFIX=.dist/install -DCMAKE_BUILD_TYPE=Debug
$ cmake --build .dist/build --config Debug
```

## How to contribute

Pull requests are welcome on [GitHub](https://github.com/myst6re/deling).
Ensure that you have commited in `develop` branch, otherwise I will not accept your
request.
