
#pragma once

class IWallpaperEngineMediaExtensions
{
public:
	virtual ~IWallpaperEngineMediaExtensions() {}

	virtual void Init() = 0;
	virtual void Destroy() = 0;

	virtual void *CreateSound(const char *name, uint8_t *data, uint32_t sizeInBytes) = 0;
	virtual void DestroySound(void *handle) = 0;
	virtual void Play(void *handle, bool loop) = 0;
	virtual void Pause(void *handle) = 0;
	virtual void Stop(void *handle) = 0;
	virtual void SetVolume(void *handle, float volume) = 0;
};
