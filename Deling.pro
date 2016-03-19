TEMPLATE = app
!win32 {
    TARGET = deling
}
win32 {
    TARGET = Deling
}

QT += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    CONFIG += c++11
}
lessThan(QT_MAJOR_VERSION, 5) {
    # Compatibility layer
    INCLUDEPATH += compat
    # Enabling c++11
    QMAKE_CXXFLAGS += -std=c++0x
}

# Input
HEADERS += MainWindow.h \
    parameters.h \
    Data.h \
    Config.h \
    FF8Text.h \
    FieldArchive.h \
    LZS.h \
    Search.h \
    BGPreview.h \
    TextPreview.h \
    Field.h \
    WalkmeshGLWidget.h \
    FsArchive.h \
    IsoArchive.h \
    VarManager.h \
    MiscSearch.h \
    FsDialog.h \
    FsWidget.h \
    BGPreview2.h \
    FsPreviewWidget.h \
    FF8Image.h \
    MsdHighlighter.h \
    ConfigDialog.h \
    FF8DiscArchive.h \
    JsmHighlighter.h \
    JsmData.h \
    JsmScripts.h \
    FieldPS.h \
    FieldPC.h \
    FieldArchivePS.h \
    FieldArchivePC.h \
    JsmOpcode.h \
    PlainTextEdit.h \
    widgets/PageWidget.h \
    widgets/BackgroundWidget.h \
    widgets/JsmWidget.h \
    widgets/MsdWidget.h \
    widgets/WalkmeshWidget.h \
    widgets/MiscWidget.h \
    files/MsdFile.h \
    files/JsmFile.h \
    files/CaFile.h \
    files/BackgroundFile.h \
    files/TdwFile.h \
    files/RatFile.h \
    files/MrtFile.h \
    widgets/EncounterWidget.h \
    FieldThread.h \
    files/CharaFile.h \
    CharaModel.h \
    widgets/TdwWidget.h \
    widgets/TdwGrid.h \
    files/TimFile.h \
    widgets/CharaWidget.h \
    files/MchFile.h \
    files/TexFile.h \
    files/TextureFile.h \
    widgets/CharaPreview.h \
    widgets/TdwLetter.h \
    widgets/TdwDisplay.h \
    FF8Font.h \
    widgets/TdwWidget2.h \
    TdwManagerDialog.h \
    widgets/TdwPalette.h \
    files/MskFile.h \
    VertexWidget.h \
    files/InfFile.h \
    files/IdFile.h \
    OrientationWidget.h \
    files/SfxFile.h \
    files/AkaoListFile.h \
    files/File.h \
    widgets/SoundWidget.h \
    HexLineEdit.h \
    files/PvpFile.h \
    files/PmpFile.h \
    files/PmdFile.h \
    Listwidget.h \
    SpecialCharactersDialog.h \
    GZIP.h \
    QTaskBarButton.h \
    ArchiveObserver.h \
    ArchiveObserverProgressDialog.h \
    ArchiveObservers.h \
    ProgressWidget.h
SOURCES += MainWindow.cpp \
    main.cpp \
    Data.cpp \
    Config.cpp \
    FF8Text.cpp \
    FieldArchive.cpp \
    LZS.cpp \
    Search.cpp \
    BGPreview.cpp \
    TextPreview.cpp \
    Field.cpp \
    WalkmeshGLWidget.cpp \
    FsArchive.cpp \
    IsoArchive.cpp \
    VarManager.cpp \
    MiscSearch.cpp \
    FsDialog.cpp \
    FsWidget.cpp \
    BGPreview2.cpp \
    FsPreviewWidget.cpp \
    FF8Image.cpp \
    MsdHighlighter.cpp \
    ConfigDialog.cpp \
    FF8DiscArchive.cpp \
    JsmHighlighter.cpp \
    JsmData.cpp \
    JsmScripts.cpp \
    FieldPS.cpp \
    FieldPC.cpp \
    FieldArchivePS.cpp \
    FieldArchivePC.cpp \
    JsmOpcode.cpp \
    PlainTextEdit.cpp \
    widgets/PageWidget.cpp \
    widgets/BackgroundWidget.cpp \
    widgets/JsmWidget.cpp \
    widgets/MsdWidget.cpp \
    widgets/WalkmeshWidget.cpp \
    widgets/MiscWidget.cpp \
    files/MsdFile.cpp \
    files/JsmFile.cpp \
    files/CaFile.cpp \
    files/BackgroundFile.cpp \
    files/TdwFile.cpp \
    files/RatFile.cpp \
    files/MrtFile.cpp \
    widgets/EncounterWidget.cpp \
    FieldThread.cpp \
    files/CharaFile.cpp \
    CharaModel.cpp \
    widgets/TdwWidget.cpp \
    widgets/TdwGrid.cpp \
    files/TimFile.cpp \
    widgets/CharaWidget.cpp \
    files/MchFile.cpp \
    files/TexFile.cpp \
    files/TextureFile.cpp \
    widgets/CharaPreview.cpp \
    widgets/TdwLetter.cpp \
    widgets/TdwDisplay.cpp \
    FF8Font.cpp \
    widgets/TdwWidget2.cpp \
    TdwManagerDialog.cpp \
    widgets/TdwPalette.cpp \
    files/MskFile.cpp \
    VertexWidget.cpp \
    files/InfFile.cpp \
    files/IdFile.cpp \
    OrientationWidget.cpp \
    files/SfxFile.cpp \
    files/AkaoListFile.cpp \
    files/File.cpp \
    widgets/SoundWidget.cpp \
    HexLineEdit.cpp \
    files/PvpFile.cpp \
    files/PmpFile.cpp \
    files/PmdFile.cpp \
    Listwidget.cpp \
    SpecialCharactersDialog.cpp \
    GZIP.cpp \
    QTaskBarButton.cpp \
    ArchiveObserverProgressDialog.cpp \
    ArchiveObservers.cpp \
    ProgressWidget.cpp

TRANSLATIONS += deling_en.ts \
    deling_ja.ts \
    deling_eu.ts

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

RESOURCES += Deling.qrc
# macx:ICON = images/Deling.icns

# include zlib
!win32 {
    LIBS += -lz
} else {
    exists($$[QT_INSTALL_PREFIX]/include/QtZlib) {
        INCLUDEPATH += $$[QT_INSTALL_PREFIX]/include/QtZlib
    } else {
        INCLUDEPATH += zlib
        greaterThan(QT_MAJOR_VERSION, 4) {
            LIBS += -lz
        }
    }
}

win32 {
    RC_FILE = Deling.rc
    LIBS += -lole32
    # OpenGL features
    LIBS += -lopengl32 -lGlU32
    # QTaskbarButton
    greaterThan(QT_MAJOR_VERSION, 4):qtHaveModule(winextras) {
        QT += winextras
        DEFINES += TASKBAR_BUTTON QTASKBAR_WIN_QT5
    } else {
        TASKBAR_BUTTON {
            DEFINES += TASKBAR_BUTTON
            LIBS += -lole32
            INCLUDEPATH += include
        }
    }
}

OTHER_FILES += Deling.rc \
    README.md \
    compat/QtWidgets
DISTFILES += Deling.desktop

# call lrelease to make the qm files.
system(lrelease Deling.pro)

# only on linux/unix (for package creation and other deploys)
unix:!macx:!symbian {
    LIBS += -lglut -lGLU
}
