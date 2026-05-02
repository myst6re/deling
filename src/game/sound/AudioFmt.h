#pragma once

#include <QIODevice>
#include "Sound.h"

constexpr int WAVE_FORMAT_ADPCM = 2;

class AudioFmt
{
public:
    AudioFmt(QIODevice *io);
    bool readHeader(uint16_t &soundCount);
    bool readSoundFormat(Sound &sound);
    bool readAll(QList<Sound> &sounds);
private:
    QIODevice *_io;
};
