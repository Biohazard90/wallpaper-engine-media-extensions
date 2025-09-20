
#pragma once

#include "iwallpaperenginemediaextensions.h"

class WallpaperEngineMediaExtensions : public IWallpaperEngineMediaExtensions
{
public:
	virtual ~WallpaperEngineMediaExtensions();

	virtual void Init() override;
	virtual void Destroy() override;

	virtual SoundBufferHandle CreateSoundBuffer(const char *name, const uint8_t *data, uint32_t sizeInBytes) override;
	virtual void DestroySoundBuffer(SoundBufferHandle handle) override;

	virtual SoundHandle CreateSound(SoundBufferHandle bufferHandle) override;
	virtual void DestroySound(SoundHandle handle) override;

	virtual float GetDuration(SoundHandle handle) override;
	virtual bool IsPlaying(SoundHandle handle) override;
	virtual bool IsPaused(SoundHandle handle) override;
	virtual bool IsStopped(SoundHandle handle) override;
	virtual void Play(SoundHandle handle, bool loop) override;
	virtual void Pause(SoundHandle handle) override;
	virtual void Stop(SoundHandle handle) override;
	virtual void SetVolume(SoundHandle handle, float volume) override;

	virtual int GetChannelCount(SoundHandle handle) override;
	virtual void SetAttenuation(SoundHandle handle, float attenuation) override;
	virtual void SetMinimumDistance(SoundHandle handle, float minDistance) override;
	virtual void SetPosition(SoundHandle handle, float *xyz) override;
	virtual void SetPositions(const uint32_t count, SoundHandle *handles, float *xyz) override;

private:
	static std::atomic<int> mpgInitCount;

	struct MediaSoundBuffer
	{
		uint32_t referenceCount;
		uint8_t *data;
		uint32_t sizeInBytes;
	};
	std::unordered_map<std::string, MediaSoundBuffer*> soundBuffers;
};
