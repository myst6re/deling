#pragma once

#include <QtCore>
#include "Sound.h"

class Sfx {
public:
    Sfx(const QString &audioFmtFileName, const QString &audioDatFileName);
    bool open();
    void close();
    QByteArray saveWav(quint32 sfxGameId);
    inline const Sound &sound(quint32 sfxGameId) const {
        return _sounds.at(sfxGameId);
    }
private:
    QFile _audioFmtFile, _audioDatFile;
    QList<Sound> _sounds;
};
