
#include "SoundPlayer.h"
#include "game/sound/Sfx.h"
#include "Data.h"
#include <QAudioOutput>

SoundPlayer::SoundPlayer(QWidget *parent) :
    QPushButton(tr("Play"), parent)
{
    _mediaPlayer.setAudioOutput(new QAudioOutput);
    connect(&_mediaPlayer, SIGNAL(playingChanged(bool)), SLOT(updateButtonText(bool)));
}

bool SoundPlayer::togglePlay(quint32 sfxGameId)
{
    if (_mediaPlayer.isPlaying()) {
        stop();

        return true;
    }

    return play(sfxGameId);
}

bool SoundPlayer::play(quint32 sfxGameId)
{
    stop();
    _mediaPlayer.setSourceDevice(nullptr);
    Sfx *sfx = Data::sounds();

    if (sfx == nullptr) {
        return false;
    }

    QByteArray wavData = sfx->saveWav(sfxGameId);
    if (wavData.isEmpty()) {
        return false;
    }

    _buffer.setData(wavData);
    if (!_buffer.open(QIODevice::ReadOnly)) {
        return false;
    }

    _mediaPlayer.setSourceDevice(&_buffer, QUrl::fromLocalFile("deling-sound-play.wav"));
    _mediaPlayer.setLoops(1);
    _mediaPlayer.play();
    
    return true;
}

void SoundPlayer::stop()
{
    _mediaPlayer.stop();
    _buffer.close();
}

void SoundPlayer::updateButtonText(bool isPlaying)
{
    setText(isPlaying ? tr("Stop") : tr("Play"));
}
