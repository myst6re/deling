# Deling

[![Build Status Linux/OSX](https://travis-ci.org/myst6re/deling.svg?branch=develop)](https://travis-ci.org/myst6re/deling)
[![Build status Win32](https://ci.appveyor.com/api/projects/status/74gm0u6ya9nqxw9u/branch/develop?svg=true)](https://ci.appveyor.com/project/myst6re/deling/branch/develop)

![Deling](images/deling.png)

Final Fantasy VIII field archive editor.

## Building

### Requirements

 - Qt 4.8 or Qt 5
 - GLUT
 - zlib
 - lz4

On Windows, you can either use mingw32 (g++) or msvc to compile.
You need liblz4.dll to run Deling on Windows.

### With Qt Creator

Open `Deling.pro` with Qt Creator IDE.
Please refer to the official documentation of Qt Creator
if you have troubles to compile and run Deling.

### Command line

#### Windows

Use the command prompt given in your Qt installation
to set your environment (`%QTDIR%/bin/qtvars.bat` for Qt 4
and `%QTDIR%/%compiler%/bin/qtenv2.bat` for Qt 5).
If you want to compile with msvc, run `vcvarsall.bat`
from your Microsoft Visual Studio installation
(eg: `C:\Program Files\Microsoft Visual Studio 11.0\VC\vcvarsall.bat`.

Then run:

~~~sh
mkdir build
cd build
qmake /path/to/Deling.pro
# For mingw32
mingw32-make
# For msvc
jom
~~~

#### Linux

Set your environment:

 - [Qt 4 only] `QTDIR` must contains the directory of your Qt installation
 - `PATH` should contains the path to the bin directory of your Qt installation (`$QTDIR/bin`)
 - [Qt 4 only] Your compiler should be in the `PATH`

Then run:

~~~sh
mkdir build
cd build
qmake /path/to/Deling.pro
make
~~~

## How to contribute

Pull requests are welcome on [GitHub](https://github.com/myst6re/deling).
Ensure that you have commited in `develop` branch, otherwise I will not accept your
request.
