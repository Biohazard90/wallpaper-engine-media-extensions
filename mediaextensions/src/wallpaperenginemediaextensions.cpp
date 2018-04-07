
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
		float duration;
		sf::Music::Status status;
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
	}
	delete this;
}

IWallpaperEngineMediaExtensions::SoundBufferHandle WallpaperEngineMediaExtensions::CreateSoundBuffer(const char *name, uint8_t *data, uint32_t sizeInBytes)
{
	unique_lock<mutex> lock(soundBufferMutex);
	auto itr = soundBuffers.find(name);
	if (itr != soundBuffers.end())
	{
		++itr->second->referenceCount;
		return itr->second;
	}
	else if (data != nullptr && sizeInBytes > 0)
	{
		MediaSoundBuffer *buffer = new MediaSoundBuffer();
		buffer->referenceCount = 1;
		buffer->data = new uint8_t[sizeInBytes];
		memcpy(buffer->data, data, sizeInBytes);
		buffer->sizeInBytes = sizeInBytes;
		soundBuffers[name] = buffer;
		return buffer;
	}
	else
	{
		return nullptr;
	}
}

void WallpaperEngineMediaExtensions::DestroySoundBuffer(SoundBufferHandle handle)
{
	MediaSoundBuffer *buffer = (MediaSoundBuffer*)handle;
	if (--buffer->referenceCount == 0)
	{
		for (auto itr : soundBuffers)
		{
			if (itr.second == buffer)
			{
				soundBuffers.erase(itr.first);
				break;
			}
		}

		delete[] buffer->data;
		delete buffer;
	}
}

IWallpaperEngineMediaExtensions::SoundHandle WallpaperEngineMediaExtensions::CreateSound(SoundBufferHandle bufferHandle)
{
	MediaSoundBuffer *buffer = (MediaSoundBuffer*)bufferHandle;
	sf::SoundStream *validSoundStream = nullptr;
	float duration = 0.0f;
	sf::Music *music = new sf::Music();

	if (music->openFromMemory(buffer->data, buffer->sizeInBytes))
	{
		validSoundStream = music;
		duration = music->getDuration().asSeconds();
	}
	else
	{
		delete music;
		// Try loading as mp3

		sf::Mp3SoundStream *mp3 = new sf::Mp3SoundStream();
		if (mp3->openFromMemory(buffer->data, buffer->sizeInBytes))
		{
			validSoundStream = mp3;
			duration = mp3->GetDuration();
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
	soundInstance->status = sf::Music::Stopped;
	soundInstance->sound = validSoundStream;
	soundInstance->duration = duration;
	return soundInstance;
}

void WallpaperEngineMediaExtensions::DestroySound(SoundHandle handle)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	unique_lock<mutex> lock(soundBufferMutex);

	//if (soundInstance->sound->getStatus() != sf::Sound::Stopped)
	if (soundInstance->status != sf::Sound::Stopped)
		soundInstance->sound->stop();
	delete soundInstance->sound;
	delete soundInstance;
}

float WallpaperEngineMediaExtensions::GetDuration(SoundHandle handle)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	return soundInstance->duration;
}

bool WallpaperEngineMediaExtensions::IsPlaying(SoundHandle handle)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	//return soundInstance->sound->getStatus() == sf::SoundSource::Playing;
	return soundInstance->status == sf::SoundSource::Playing;
}

bool WallpaperEngineMediaExtensions::IsPaused(SoundHandle handle)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	//return soundInstance->sound->getStatus() == sf::SoundSource::Paused;
	return soundInstance->status == sf::SoundSource::Paused;
}

bool WallpaperEngineMediaExtensions::IsStopped(SoundHandle handle)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	//return soundInstance->sound->getStatus() == sf::SoundSource::Stopped;
	return soundInstance->status == sf::SoundSource::Stopped;
}

void WallpaperEngineMediaExtensions::Play(void *handle, bool loop)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	soundInstance->sound->setLoop(loop);
	soundInstance->sound->play();
	soundInstance->status = sf::Music::Playing;
}

void WallpaperEngineMediaExtensions::Pause(void *handle)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	soundInstance->sound->pause();
	soundInstance->status = sf::Music::Paused;
}

void WallpaperEngineMediaExtensions::Stop(void *handle)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	soundInstance->sound->stop();
	soundInstance->status = sf::Music::Stopped;
}

void WallpaperEngineMediaExtensions::SetVolume(void *handle, float volume)
{
	WINASSERT(handle != nullptr);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	soundInstance->sound->setVolume(100.0f * volume);
}
