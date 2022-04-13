###############################################################################
## Copyright (C) 2009-2022 Arzel Jérôme <myst6re@gmail.com>
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

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0010 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0091 NEW)

set(RELEASE_NAME "Deling")
set(GUI_TARGET "${RELEASE_NAME}")
if(NOT PRERELEASE_STRING)
    set(PRERELEASE_STRING "")
endif()

project("Deling" VERSION 2.0.0 LANGUAGES CXX
    DESCRIPTION "Final Fantasy VIII field archive editor"
    HOMEPAGE_URL "https://github.com/myst6re/deling"
)

# Try to autodetect Qt path if not specified
include(qt.cmake)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_definitions(
    QT_DISABLE_DEPRECATED_BEFORE=0x050F00
    #QT_RESTRICTED_CAST_FROM_ASCII
    MAKOU_REACTOR_NAME="${PROJECT_NAME}"
    MAKOU_REACTOR_DESCRIPTION="${PROJECT_DESCRIPTION}"
    MAKOU_REACTOR_COMPANY="myst6re"
    MAKOU_REACTOR_VERSION="${PROJECT_VERSION}${PRERELEASE_STRING}"
    MAKOU_REACTOR_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    MAKOU_REACTOR_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    MAKOU_REACTOR_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    MAKOU_REACTOR_VERSION_TWEAK=0
)

find_package(QT NAMES Qt6 COMPONENTS Widgets OpenGLWidgets OpenGL REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets OpenGLWidgets OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(lz4 REQUIRED)

set(PROJECT_SOURCES
    "src/ArchiveObserver.h"
    "src/ArchiveObserverProgressDialog.cpp"
    "src/ArchiveObserverProgressDialog.h"
    "src/ArchiveObservers.cpp"
    "src/ArchiveObservers.h"
    "src/BackgroundExporter.cpp"
    "src/BackgroundExporter.h"
    "src/BGPreview.cpp"
    "src/BGPreview.h"
    "src/BGPreview2.cpp"
    "src/BGPreview2.h"
    "src/CharaModel.cpp"
    "src/CharaModel.h"
    "src/Config.cpp"
    "src/Config.h"
    "src/ConfigDialog.cpp"
    "src/ConfigDialog.h"
    "src/Data.cpp"
    "src/Data.h"
    "src/EncounterExporter.cpp"
    "src/EncounterExporter.h"
    "src/FF8DiscArchive.cpp"
    "src/FF8DiscArchive.h"
    "src/FF8Font.cpp"
    "src/FF8Font.h"
    "src/FF8Image.cpp"
    "src/FF8Image.h"
    "src/FF8Text.cpp"
    "src/FF8Text.h"
    "src/Field.cpp"
    "src/Field.h"
    "src/FieldArchive.cpp"
    "src/FieldArchive.h"
    "src/FieldArchivePC.cpp"
    "src/FieldArchivePC.h"
    "src/FieldArchivePS.cpp"
    "src/FieldArchivePS.h"
    "src/FieldPC.cpp"
    "src/FieldPC.h"
    "src/FieldPS.cpp"
    "src/FieldPS.h"
    "src/FieldThread.cpp"
    "src/FieldThread.h"
    "src/files/AkaoListFile.cpp"
    "src/files/AkaoListFile.h"
    "src/files/BackgroundFile.cpp"
    "src/files/BackgroundFile.h"
    "src/files/CaFile.cpp"
    "src/files/CaFile.h"
    "src/files/CharaFile.cpp"
    "src/files/CharaFile.h"
    "src/files/File.cpp"
    "src/files/File.h"
    "src/files/IdFile.cpp"
    "src/files/IdFile.h"
    "src/files/InfFile.cpp"
    "src/files/InfFile.h"
    "src/files/JsmFile.cpp"
    "src/files/JsmFile.h"
    "src/files/MchFile.cpp"
    "src/files/MchFile.h"
    "src/files/MrtFile.cpp"
    "src/files/MrtFile.h"
    "src/files/MsdFile.cpp"
    "src/files/MsdFile.h"
    "src/files/MskFile.cpp"
    "src/files/MskFile.h"
    "src/files/PmdFile.cpp"
    "src/files/PmdFile.h"
    "src/files/PmpFile.cpp"
    "src/files/PmpFile.h"
    "src/files/PvpFile.cpp"
    "src/files/PvpFile.h"
    "src/files/RatFile.cpp"
    "src/files/RatFile.h"
    "src/files/SfxFile.cpp"
    "src/files/SfxFile.h"
    "src/files/TdwFile.cpp"
    "src/files/TdwFile.h"
    "src/files/TexFile.cpp"
    "src/files/TexFile.h"
    "src/files/TextureFile.cpp"
    "src/files/TextureFile.h"
    "src/files/TimFile.cpp"
    "src/files/TimFile.h"
    "src/FsArchive.cpp"
    "src/FsArchive.h"
    "src/FsDialog.cpp"
    "src/FsDialog.h"
    "src/FsPreviewWidget.cpp"
    "src/FsPreviewWidget.h"
    "src/FsWidget.cpp"
    "src/FsWidget.h"
    "src/GZIP.cpp"
    "src/GZIP.h"
    "src/HexLineEdit.cpp"
    "src/HexLineEdit.h"
    "src/IsoArchive.cpp"
    "src/IsoArchive.h"
    "src/JsmData.cpp"
    "src/JsmData.h"
    "src/JsmExpression.cpp"
    "src/JsmExpression.h"
    "src/JsmHighlighter.cpp"
    "src/JsmHighlighter.h"
    "src/JsmOpcode.cpp"
    "src/JsmOpcode.h"
    "src/JsmScripts.cpp"
    "src/JsmScripts.h"
    "src/Listwidget.cpp"
    "src/Listwidget.h"
    "src/LZS.cpp"
    "src/LZS.h"
    "src/main.cpp"
    "src/MainWindow.cpp"
    "src/MainWindow.h"
    "src/MiscSearch.cpp"
    "src/MiscSearch.h"
    "src/MsdHighlighter.cpp"
    "src/MsdHighlighter.h"
    "src/OrientationWidget.cpp"
    "src/OrientationWidget.h"
    "src/parameters.h"
    "src/PlainTextEdit.cpp"
    "src/PlainTextEdit.h"
    "src/PreviewWidget.cpp"
    "src/PreviewWidget.h"
    "src/ProgressWidget.cpp"
    "src/ProgressWidget.h"
    "src/QLZ4.cpp"
    "src/QLZ4.h"
    "src/QTaskBarButton.cpp"
    "src/QTaskBarButton.h"
    "src/Renderer.cpp"
    "src/Renderer.h"
    "src/ScriptExporter.cpp"
    "src/ScriptExporter.h"
    "src/Search.cpp"
    "src/Search.h"
    "src/SearchAll.cpp"
    "src/SearchAll.h"
    "src/SpecialCharactersDialog.cpp"
    "src/SpecialCharactersDialog.h"
    "src/TdwManagerDialog.cpp"
    "src/TdwManagerDialog.h"
    "src/TextPreview.cpp"
    "src/TextPreview.h"
    "src/VarManager.cpp"
    "src/VarManager.h"
    "src/VertexWidget.cpp"
    "src/VertexWidget.h"
    "src/WalkmeshGLWidget.cpp"
    "src/WalkmeshGLWidget.h"
    "src/widgets/AboutDialog.cpp"
    "src/widgets/AboutDialog.h"
    "src/widgets/BackgroundWidget.cpp"
    "src/widgets/BackgroundWidget.h"
    "src/widgets/CharaPreview.cpp"
    "src/widgets/CharaPreview.h"
    "src/widgets/CharaWidget.cpp"
    "src/widgets/CharaWidget.h"
    "src/widgets/EncounterWidget.cpp"
    "src/widgets/EncounterWidget.h"
    "src/widgets/JsmWidget.cpp"
    "src/widgets/JsmWidget.h"
    "src/widgets/MiscWidget.cpp"
    "src/widgets/MiscWidget.h"
    "src/widgets/MsdWidget.cpp"
    "src/widgets/MsdWidget.h"
    "src/widgets/PageWidget.cpp"
    "src/widgets/PageWidget.h"
    "src/widgets/SoundWidget.cpp"
    "src/widgets/SoundWidget.h"
    "src/widgets/TdwDisplay.cpp"
    "src/widgets/TdwDisplay.h"
    "src/widgets/TdwGrid.cpp"
    "src/widgets/TdwGrid.h"
    "src/widgets/TdwLetter.cpp"
    "src/widgets/TdwLetter.h"
    "src/widgets/TdwPalette.cpp"
    "src/widgets/TdwPalette.h"
    "src/widgets/TdwWidget.cpp"
    "src/widgets/TdwWidget.h"
    "src/widgets/TdwWidget2.cpp"
    "src/widgets/TdwWidget2.h"
    "src/widgets/WalkmeshWidget.cpp"
    "src/widgets/WalkmeshWidget.h"
)

if(APPLE)
    list(APPEND PROJECT_SOURCES
        "src/QTaskBarButton.mm"
    )
endif()

set(RESOURCES "src/qt/${RELEASE_NAME}.qrc")

if(APPLE)
    set(ICON_FILE "deploy/macosx/${RELEASE_NAME}.icns")
    set(EXTRA_RESOURCES_GUI ${ICON_FILE})
    set(EXTRA_RESOURCES_CLI "")

    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
elseif (WIN32)
    set(ICON_FILE "src/qt/images/deling.ico")
    set(EXTRA_RESOURCES_GUI "src/qt/${RELEASE_NAME}.rc")
    set(EXTRA_RESOURCES_CLI "${EXTRA_RESOURCES_GUI}")
endif()

qt_add_executable(${GUI_TARGET} MANUAL_FINALIZATION MACOSX_BUNDLE WIN32 ${PROJECT_SOURCES} ${QM_FILES} ${RESOURCES} ${EXTRA_RESOURCES_GUI})
target_include_directories(${GUI_TARGET} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(${GUI_TARGET} PRIVATE
	Qt::OpenGL
	Qt::Widgets
	Qt::OpenGLWidgets
	ZLIB::ZLIB
	lz4::lz4
)

if(${QT_VERSION_MAJOR} EQUAL 6)
	target_compile_definitions(${GUI_TARGET}
		PRIVATE TASKBAR_BUTTON=1
	)
endif()

set_target_properties(${GUI_TARGET} PROPERTIES
	MACOSX_BUNDLE_BUNDLE_NAME "Deling"
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
	MACOSX_BUNDLE_GUI_IDENTIFIER org.myst6re.deling
	MACOSX_BUNDLE_ICON_FILE ${RELEASE_NAME}.icns
)

if(APPLE OR WIN32)
	set_target_properties(${GUI_TARGET} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
endif()

include(GNUInstallDirs)

if(APPLE)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "/Applications" CACHE PATH "..." FORCE)
    endif()
    if(NOT QT_DEPLOY_TMP_DIR)
        set(QT_DEPLOY_TMP_DIR "${CMAKE_BINARY_DIR}" CACHE PATH "Directory to run deployqt scripts")
    endif()
elseif(WIN32)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND ENV{ProgramFiles})
        # Force Program Files for x64 programs (CMake uses x86 by default)
        set(CMAKE_INSTALL_PREFIX "$ENV{ProgramFiles}/${PROJECT_NAME}" CACHE PATH "..." FORCE)
    endif()
    if(NOT QT_DEPLOY_TMP_DIR)
        set(QT_DEPLOY_TMP_DIR "${CMAKE_BINARY_DIR}/win32-deploy" CACHE PATH "Directory to run deployqt scripts")
    endif()
    if(GUI)
        install(TARGETS ${GUI_TARGET} RUNTIME DESTINATION "${QT_DEPLOY_TMP_DIR}")
    endif()
    if(CLI)
        install(TARGETS ${CLI_TARGET} RUNTIME DESTINATION "${QT_DEPLOY_TMP_DIR}")
    endif()
endif()

add_subdirectory(lang)
add_subdirectory(deploy)

if(APPLE)
    if(GUI)
        install(TARGETS ${GUI_TARGET} BUNDLE DESTINATION ".")
    endif()
    if(CLI)
        install(TARGETS ${CLI_TARGET} RUNTIME EXCLUDE_FROM_ALL)
    endif()
elseif(WIN32)
    install(DIRECTORY "${QT_DEPLOY_TMP_DIR}/" DESTINATION ".")
    install(CODE "file(REMOVE_RECURSE \"${QT_DEPLOY_TMP_DIR}\")")
else()
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/qt/images/deling.png DESTINATION share/icons/hicolor/256x256/apps RENAME Makou_Reactor.png)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/deploy/linux/Deling.desktop DESTINATION share/applications)
    if(GUI)
        install(TARGETS ${GUI_TARGET} RUNTIME)
    endif()
    if(CLI)
        install(TARGETS ${CLI_TARGET} RUNTIME)
    endif()
endif()

if(GUI)
    qt_finalize_executable(${GUI_TARGET})
endif()
if(CLI)
    qt_finalize_executable(${CLI_TARGET})
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CPACK_SYSTEM_NAME "macos")
    set(CPACK_GENERATOR "DragNDrop")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TXZ")
endif()

set(CPACK_PACKAGE_NAME makoureactor)
set(CPACK_SOURCE_IGNORE_FILES ".git/*;*.user;*.yml;*.github/*")
set(CPACK_SOURCE_GENERATOR "ZIP;TGZ")

include(CPack)
