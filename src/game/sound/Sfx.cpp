#include "Sfx.h"
#include "AudioFmt.h"

Sfx::Sfx(const QString &audioFmtFileName, const QString &audioDatFileName) :
    _audioFmtFile(audioFmtFileName), _audioDatFile(audioDatFileName)
{
}

bool Sfx::open()
{
    if (!_audioFmtFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    if (!_audioDatFile.open(QIODevice::ReadOnly)) {
        _audioFmtFile.close();
        return false;
    }

    AudioFmt fmt(&_audioFmtFile);
    return fmt.readAll(_sounds);
}

void Sfx::close()
{
    _audioFmtFile.close();
    _audioDatFile.close();
}

QByteArray Sfx::saveWav(quint32 sfxGameId)
{
    if (sfxGameId >= _sounds.size()) {
        return QByteArray();
    }

    return _sounds.at(sfxGameId).toWav(&_audioDatFile);
}
