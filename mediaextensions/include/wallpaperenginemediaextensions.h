
#pragma once

#include "iwallpaperenginemediaextensions.h"

class WallpaperEngineMediaExtensions : public IWallpaperEngineMediaExtensions
{
public:
	virtual ~WallpaperEngineMediaExtensions();

	virtual void Init();
	virtual void Destroy();

	virtual void *CreateSound(const char *name, uint8_t *data, uint32_t sizeInBytes);
	virtual void DestroySound(void *handle);
	virtual void Play(void *handle, bool loop);
	virtual void Pause(void *handle);
	virtual void Stop(void *handle);
	virtual void SetVolume(void *handle, float volume);

private:
	static std::atomic<int> mpgInitCount;

	struct MediaSoundBuffer
	{
		uint32_t referenceCount;
		//sf::SoundBuffer buffer;
		uint8_t *data;
		uint32_t sizeInBytes;
	};
	std::unordered_map<std::string, MediaSoundBuffer*> soundBuffers;
	std::mutex soundBufferMutex;
};
