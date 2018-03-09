
#pragma once

class IWallpaperEngineMediaExtensions
{
public:
	typedef void* SoundBufferHandle;
	typedef void* SoundHandle;

	virtual ~IWallpaperEngineMediaExtensions() {}

	virtual void Init() = 0;
	virtual void Destroy() = 0;

	virtual SoundBufferHandle CreateSoundBuffer(const char *name, uint8_t *data, uint32_t sizeInBytes) = 0;
	virtual void DestroySoundBuffer(SoundBufferHandle handle) = 0;

	virtual SoundHandle CreateSound(SoundBufferHandle bufferHandle) = 0;
	virtual void DestroySound(SoundHandle handle) = 0;

	virtual float GetDuration(SoundHandle handle) = 0;
	virtual bool IsPlaying(SoundHandle handle) = 0;
	virtual void Play(SoundHandle handle, bool loop) = 0;
	virtual void Pause(SoundHandle handle) = 0;
	virtual void Stop(SoundHandle handle) = 0;
	virtual void SetVolume(SoundHandle handle, float volume) = 0;
};
