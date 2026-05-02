#include "Sound.h"

Sound::Sound() :
    _dataOffset(0), _dataLength(0), _bufferReadCursor(0), _bufferWriteCursor(0),
    _adpcmData(QByteArray()), _format(WaveFormatEx()), _isLooping(false)
{
}

void Sound::setDataInfos(uint32_t offset, uint32_t length)
{
    _dataOffset = offset;
    _dataLength = length;
}

void Sound::setBufferCursor(uint32_t read, uint32_t write)
{
    _bufferReadCursor = read;
    _bufferWriteCursor = write;
}

void Sound::setIsLooping(bool isLooping)
{
    _isLooping = isLooping;
}

void Sound::setWaveFormat(const WaveFormatEx &format, const QByteArray &adpcmData)
{
    _format = format;
    _adpcmData = adpcmData;
}

QByteArray Sound::toWav(QIODevice *audioDat) const
{
    QByteArray ret;
    
    if (! audioDat->seek(_dataOffset)) {
        return ret;
    }
    QByteArray data = audioDat->read(_dataLength);
    
    if (data.isEmpty()) {
        return ret;
    }

    uint32_t length;
    
    // Credits to Qhimm (from FF8SND)
    ret.append("RIFF", 4);
    length = _dataLength + 36 + _adpcmData.size();
    ret.append((const char *)&length, 4);
    ret.append("WAVEfmt ", 8);
    length = 18 + _adpcmData.size();
    ret.append((const char *)&length, 4);
    ret.append((const char *)&_format, 18);
    ret.append(_adpcmData);
    ret.append("data", 4);
    ret.append((const char *)&_dataLength, 4);
    ret.append(data);
    
    return ret;
}
