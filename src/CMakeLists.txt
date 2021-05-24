###############################################################################
## Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
## Copyright (C) 2020 Julian Xhokaxhiu <https://julianxhokaxhiu.com>
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
###############################################################################

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Autodetect the current Qt MSVC toolkit installed
# Original source: https://stackoverflow.com/a/47723189 + custom patch to reflect current VS year based on the toolkit
set(QT_AUTODETECT True)

# Force deprecated to be errors (before Qt 5.15)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

# use Qt_DIR approach so you can find Qt after cmake has been invoked
if(DEFINED ENV{_CMAKE_QT_DIR})
    set(QT_PATH $ENV{_CMAKE_QT_DIR})
    set(QT_AUTODETECT False)
# Attempt autodetection on Windows. Unix environments should have Qt within the already known paths by the compiler
elseif(MSVC)
    # look for user-registry pointing to qtcreator
    get_filename_component(QT_BIN [HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{89c4e30e-3a1a-47c7-80d7-013ba3e25e57};InstallLocation] ABSOLUTE CACHE)

    file(GLOB QT_VERSIONS "${QT_BIN}/6.*")
    list(SORT QT_VERSIONS)

    # assume the latest version will be last alphabetically
    list(REVERSE QT_VERSIONS)

    list(GET QT_VERSIONS 0 QT_VERSION)

    # fix any double slashes which seem to be common
    string(REPLACE "//" "/"  QT_VERSION "${QT_VERSION}")

    if(MSVC_TOOLSET_VERSION MATCHES 142)
        SET(QT_MSVC "2019")
    elseif(MSVC_TOOLSET_VERSION MATCHES 141)
        SET(QT_MSVC "2017")
    endif()

    # check for 64-bit os
    # may need to be removed for older compilers as it wasn't always offered
    if(CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE MATCHES x64)
        set(QT_MSVC "${QT_MSVC}_64")
    endif()
    set(QT_PATH "${QT_VERSION}/msvc${QT_MSVC}")

    set(QT_AUTODETECT False)
elseif(APPLE AND EXISTS /usr/local/opt/qt6)
    # Homebrew installs Qt5 (up to at least 5.9.1) in
    # /usr/local/qt6, ensure it can be found by CMake since
    # it is not in the default /usr/local prefix.
    list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt6")
endif()

# Print the autodetected Qt path
if(NOT QT_AUTODETECT)
    message("-- Qt found: ${QT_PATH}")
    set(QT_DIR "${QT_PATH}/lib/cmake/Qt6/")
    set(Qt6_DIR "${QT_PATH}/lib/cmake/Qt6/")
    set(Qt6Test_DIR "${QT_PATH}/lib/cmake/Qt6Test")
    set(Qt6Gui_DIR "${QT_PATH}/lib/cmake/Qt6Gui/")
    set(Qt6Widgets_DIR "${QT_PATH}/lib/cmake/Qt6Widgets/")
    set(Qt6WidgetsTools_DIR "${QT_PATH}/lib/cmake/Qt6WidgetsTools/")
    set(Qt6CoreTools_DIR "${QT_PATH}/lib/cmake/Qt6CoreTools/")
    set(Qt6GuiTools_DIR "${QT_PATH}/lib/cmake/Qt6GuiTools/")
    set(Qt6LinguistTools_DIR "${QT_PATH}/lib/cmake/Qt6LinguistTools/")
    set(Qt6OpenGL_DIR "${QT_PATH}/lib/cmake/Qt6OpenGL/")
    set(Qt6OpenGLWidgets_DIR "${QT_PATH}/lib/cmake/Qt6OpenGLWidgets/")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(RELEASE_NAME "Deling")
set(RELEASE_VERSION "1.8.2")
set(GUI_TARGET "${RELEASE_NAME}")

if(APPLE)
    set(CMAKE_MACOSX_BUNDLE ON)
    set(MACOSX_BUNDLE_BUNDLE_NAME "${RELEASE_NAME}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${RELEASE_VERSION}")
    set(MACOSX_BUNDLE_ICON_FILE "${RELEASE_NAME}.icns")
endif()

find_package(LZ4 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(QT NAMES Qt6 COMPONENTS Widgets LinguistTools OpenGL OpenGLWidgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools OpenGL OpenGLWidgets REQUIRED)

file(GLOB_RECURSE source_files "*.cpp")
# Translations
file(GLOB_RECURSE translation_files "${RELEASE_NAME}_*.ts")

qt_add_executable(${GUI_TARGET}
    WIN32 ${source_files} ${QM_FILES} qt/${RELEASE_NAME}.qrc qt/${RELEASE_NAME}.rc
)

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${translation_files})

target_include_directories(${GUI_TARGET} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(${GUI_TARGET} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    Qt${QT_VERSION_MAJOR}::GuiPrivate
    LZ4::LZ4
    ZLIB::ZLIB
)
if(MSVC)
    target_compile_options(${GUI_TARGET}
        PRIVATE /D_CRT_SECURE_NO_WARNINGS
        PRIVATE /DNOMINMAX
        PRIVATE /EHsc
        PRIVATE /w
    )
    target_link_options(${GUI_TARGET}
        PRIVATE /PDBALTPATH:${RELEASE_NAME}.pdb
    )
endif()

include(GNUInstallDirs)

# Retrieve the absolute path to qmake and then use that path to find the binaries
get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(LCONVERT_EXECUTABLE lconvert HINTS "${_qt_bin_dir}")
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")

if(APPLE)
    # Executable
    install(TARGETS ${GUI_TARGET} BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR})
else()
    # Executable
    install(TARGETS ${GUI_TARGET} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# Translations
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_BINDIR})
# License
install(FILES ${CMAKE_SOURCE_DIR}/COPYING.TXT DESTINATION ${CMAKE_INSTALL_BINDIR})
# Deploy Qt dependencies on Windows
if(WIN32)
    # Lz4 dll
    install(FILES ${CMAKE_INSTALL_PREFIX}/vendor/bin/lz4.dll DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Zlib dll
    install(FILES ${CMAKE_INSTALL_PREFIX}/vendor/bin/zlib.dll DESTINATION ${CMAKE_INSTALL_BINDIR})

    # PDB ( Debug mode only )
    install(FILES $<TARGET_PDB_FILE:${GUI_TARGET}> DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)

    add_custom_command(TARGET ${GUI_TARGET} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E env PATH="${_qt_bin_dir}"
            "${WINDEPLOYQT_EXECUTABLE} --force --release --dir \"${CMAKE_INSTALL_FULL_BINDIR}\" --no-quick-import --no-translations --no-webkit2 --no-angle --no-svg --no-webkit --no-opengl-sw $<TARGET_FILE:${GUI_TARGET}>"
        COMMENT "Running windeployqt..."
    )
endif()
# Deploy DMG on macOS
if(APPLE)
    add_custom_command(TARGET ${GUI_TARGET} POST_BUILD
        COMMAND "${MACDEPLOYQT_EXECUTABLE}"
                "$<TARGET_FILE_DIR:${GUI_TARGET}>/../.."
                -always-overwrite
                -libpath=${CMAKE_INSTALL_PREFIX}/vendor/lib
        COMMENT "Running macdeployqt..."
    )
endif()

mark_as_advanced(WINDEPLOYQT_EXECUTABLE MACDEPLOYQT_EXECUTABLE)
