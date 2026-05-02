#pragma once

#include <QtCore>

struct WaveFormatEx {
    uint16_t wFormatTag;
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;
    uint16_t cbSize;
};

class Sound
{
public:
    Sound();
    void setDataInfos(uint32_t offset, uint32_t length);
    void setBufferCursor(uint32_t read, uint32_t write);
    void setIsLooping(bool isLooping);
    void setWaveFormat(const WaveFormatEx &format, const QByteArray &adpcmData);
    inline uint32_t dataOffset() const {
        return _dataOffset;
    }
    inline uint32_t dataLength() const {
        return _dataLength;
    }
    inline uint32_t bufferReadCursor() const {
        return _bufferReadCursor;
    }
    inline uint32_t bufferWriteCursor() const {
        return _bufferWriteCursor;
    }
    inline const QByteArray &adpcmData() const {
        return _adpcmData;
    }
    inline const WaveFormatEx &format() const {
        return _format;
    }
    inline bool isLooping() const {
        return _isLooping;
    }
    inline bool isValid() const {
        return _dataLength != 0;
    }
    QByteArray toWav(QIODevice *audioDat) const;
private:
    uint32_t _dataOffset;
    uint32_t _dataLength;
    uint32_t _bufferReadCursor;
    uint32_t _bufferWriteCursor;
    QByteArray _adpcmData;
    WaveFormatEx _format;
    bool _isLooping;
};
