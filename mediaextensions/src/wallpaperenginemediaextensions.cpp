
#include "pch.h"

#include "wallpaperenginemediaextensions.h"

#include "mp3soundstream.h"
#include "mpg123/mpg123.h"

using namespace std;

atomic<int> WallpaperEngineMediaExtensions::mpgInitCount = 0;

namespace
{
	struct SoundInstance
	{
		sf::SoundStream *sound;
		string key;
	};
}

WallpaperEngineMediaExtensions::~WallpaperEngineMediaExtensions()
{
	WINASSERT(soundBuffers.empty());
}

void WallpaperEngineMediaExtensions::Init()
{
	if (mpgInitCount.fetch_add(1) == 0)
	{
		mpg123_init();
	}
}

void WallpaperEngineMediaExtensions::Destroy()
{
	if (mpgInitCount.fetch_sub(1) == 1)
	{
		mpg123_exit();
		delete this;
	}
}

void *WallpaperEngineMediaExtensions::CreateSound(const char *name, uint8_t *data, uint32_t sizeInBytes)
{
	WINASSERT(name != nullptr);

	MediaSoundBuffer *buffer = nullptr;

	{
		unique_lock<mutex> lock(soundBufferMutex);
		auto itr = soundBuffers.find(name);
		if (itr != soundBuffers.end())
		{
			++itr->second->referenceCount;
			buffer = itr->second;
		}
		else if (data != nullptr && sizeInBytes > 0)
		{
			buffer = new MediaSoundBuffer();
			buffer->referenceCount = 1;
			buffer->data = new uint8_t[sizeInBytes];
			memcpy(buffer->data, data, sizeInBytes);
			buffer->sizeInBytes = sizeInBytes;
			soundBuffers[name] = buffer;
		}
	}

	if (buffer != nullptr)
	{
		sf::SoundStream *validSoundStream = nullptr;
		sf::Music *music = new sf::Music();

		if (music->openFromMemory(buffer->data, buffer->sizeInBytes))
		{
			validSoundStream = music;
		}
		else
		{
			delete music;
			// Try loading as mp3

			sf::Mp3SoundStream *mp3 = new sf::Mp3SoundStream();
			if (mp3->openFromMemory(buffer->data, buffer->sizeInBytes))
			{
				validSoundStream = mp3;
			}
			else
			{
				delete mp3;
			}
		}

		// Unable to open memory as audio file
		if (validSoundStream == nullptr)
			return nullptr;

		SoundInstance *soundInstance = new SoundInstance();
		soundInstance->key = name;
		soundInstance->sound = validSoundStream;
		return soundInstance;
	}
	else
	{
		return nullptr;
	}
}

void WallpaperEngineMediaExtensions::DestroySound(void *handle)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	unique_lock<mutex> lock(soundBufferMutex);
	auto itr = soundBuffers.find(soundInstance->key);

	if (soundInstance->sound->getStatus() != sf::Sound::Stopped)
		soundInstance->sound->stop();
	delete soundInstance->sound;
	delete soundInstance;

	WINASSERT(itr != soundBuffers.end());
	if (itr != soundBuffers.end())
	{
		if (--itr->second->referenceCount == 0)
		{
			delete[] itr->second->data;
			delete itr->second;
			soundBuffers.erase(itr);
		}
	}
}

void WallpaperEngineMediaExtensions::Play(void *handle, bool loop)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	soundInstance->sound->setLoop(loop);
	soundInstance->sound->play();
}

void WallpaperEngineMediaExtensions::Pause(void *handle)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	soundInstance->sound->pause();
}

void WallpaperEngineMediaExtensions::Stop(void *handle)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	soundInstance->sound->stop();
}

void WallpaperEngineMediaExtensions::SetVolume(void *handle, float volume)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	soundInstance->sound->setVolume(100.0f * volume);
}
