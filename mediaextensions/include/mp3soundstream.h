
#pragma once

#include "mpg123/mpg123.h"

namespace sf
{
	// From https://github.com/SFML/SFML/wiki/Source:-MP3-Player (which itself is CC0)
	class Mp3SoundStream : public SoundStream
	{
	public:
		Mp3SoundStream();
		~Mp3SoundStream();

		bool openFromMemory(void* data, size_t sizeInBytes);

	protected:
		bool onGetData(Chunk& data);
		void onSeek(Time timeOffset);

	private:
		mpg123_handle *myHandle;
		size_t myBufferSize;
		unsigned char *myBuffer;
		Mutex myMutex;
	};
}
