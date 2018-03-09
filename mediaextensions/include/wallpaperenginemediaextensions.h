
#pragma once

#include "iwallpaperenginemediaextensions.h"

class WallpaperEngineMediaExtensions : public IWallpaperEngineMediaExtensions
{
public:
	virtual ~WallpaperEngineMediaExtensions();

	virtual void Init();
	virtual void Destroy();

	virtual SoundBufferHandle CreateSoundBuffer(const char *name, uint8_t *data, uint32_t sizeInBytes);
	virtual void DestroySoundBuffer(SoundBufferHandle handle);

	virtual SoundHandle CreateSound(SoundBufferHandle bufferHandle);
	virtual void DestroySound(SoundHandle handle);

	virtual float GetDuration(SoundHandle handle);
	virtual bool IsPlaying(SoundHandle handle);
	virtual void Play(SoundHandle handle, bool loop);
	virtual void Pause(SoundHandle handle);
	virtual void Stop(SoundHandle handle);
	virtual void SetVolume(SoundHandle handle, float volume);

private:
	static std::atomic<int> mpgInitCount;

	struct MediaSoundBuffer
	{
		uint32_t referenceCount;
		uint8_t *data;
		uint32_t sizeInBytes;
	};
	std::unordered_map<std::string, MediaSoundBuffer*> soundBuffers;
	std::mutex soundBufferMutex;
};
