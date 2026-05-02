#pragma once

#include <QtWidgets>
#include <QMediaPlayer>

class SoundPlayer : public QPushButton {
    Q_OBJECT
public:
    SoundPlayer(QWidget *parent = nullptr);
public slots:
    bool togglePlay(quint32 sfxGameId);
    bool play(quint32 sfxGameId);
    void stop();
    void updateButtonText(bool isPlaying);
private:
    QBuffer _buffer;
    QMediaPlayer _mediaPlayer;
};
