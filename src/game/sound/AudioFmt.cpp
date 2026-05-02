#include "AudioFmt.h"

#include <QDebug>

struct Format {
	uint32_t dataLength;
	uint32_t dataOffset;
	uint8_t bufferFlags;
	uint8_t field9, fieldA, fieldB;
	uint32_t bufferReadCursor;
	uint32_t bufferWriteCursor;
};

AudioFmt::AudioFmt(QIODevice *io) : _io(io) {}

bool AudioFmt::readHeader(uint16_t &soundCount)
{
	return _io->read((char *)&soundCount, sizeof(uint16_t)) == sizeof(uint16_t);
}

bool AudioFmt::readSoundFormat(Sound &sound)
{
	Format format = Format();
	if (! _io->read((char *)&format, sizeof(Format))) {
		return false;
	}
	
	sound.setDataInfos(format.dataOffset, format.dataLength);
	sound.setBufferCursor(format.bufferReadCursor, format.bufferWriteCursor);

	if (format.bufferFlags > 1) {
		qWarning() << "AudioFmt::readSoundFormat" << "Unknown dsound buffer flag" << format.bufferFlags;
	}
	
	sound.setIsLooping(format.bufferFlags == 1);
	
	WaveFormatEx wavFormat = WaveFormatEx();
	if (! _io->read((char *)&wavFormat, 18)) {
		return false;
	}
	
	QByteArray extraData;
	
	if (wavFormat.cbSize > 0) {
		if (wavFormat.wFormatTag == WAVE_FORMAT_ADPCM) {
			extraData = _io->read(wavFormat.cbSize);
		} else {
			qWarning() << "AudioFmt::readSoundFormat" << "cbSize if not 0 with format" << wavFormat.wFormatTag;
		}
	}

	sound.setWaveFormat(wavFormat, extraData);

	return true;
}

bool AudioFmt::readAll(QList<Sound> &sounds)
{
	uint16_t soundCount;
	
	if (! readHeader(soundCount)) {
		return false;
	}
	
	for (int i = 0; i < soundCount + 1; ++i) {
		Sound sound;
		if (! readSoundFormat(sound)) {
			return false;
		}
		
		sounds.append(sound);
	}
	
	return true;
}
