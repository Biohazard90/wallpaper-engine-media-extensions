
#include "pch.h"

#include "wallpaperenginemediaextensions.h"

using namespace std;

namespace
{
	std::mutex soundBufferMutex;
	struct SoundInstance
	{
		//enum SoundInstanceFlags
		//{
		//	IS_SPATIALIZATION_ACTIVE = (1 << 0),
		//};

		sf::SoundStream *sound;
		float duration;
		sf::Music::Status status;
		//uint32_t flags = 0;
	};
}

WallpaperEngineMediaExtensions::~WallpaperEngineMediaExtensions()
{
	WINASSERT(soundBuffers.empty());
}

void WallpaperEngineMediaExtensions::Init()
{
}

void WallpaperEngineMediaExtensions::Destroy()
{
	delete this;
}

IWallpaperEngineMediaExtensions::SoundBufferHandle WallpaperEngineMediaExtensions::CreateSoundBuffer(const char *name, const uint8_t *data, uint32_t sizeInBytes)
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
	unique_lock<mutex> lock(soundBufferMutex);
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
	unique_lock<mutex> lock(soundBufferMutex);
	sf::SoundStream *validSoundStream = nullptr;
	float duration = 0.0f;
	sf::Music *music = new sf::Music();

	auto result = music->openFromMemory(buffer->data, buffer->sizeInBytes);
	if (result == sf::Music::OPEN_SUCCESS)
	{
		validSoundStream = music;
		duration = music->getDuration().asSeconds();
	}
	else if (result == sf::Music::OPEN_NO_AUDIO_DEVICE)
	{
		// SFML cannot init any audio devices, just continue without device so
		// the client will reinitialize this stream when an audio device is connected
		delete music;
	}
	else
	{
		// SFML ready, but data seems to be invalid or unsupported
		delete music;
		return nullptr;
	}

	SoundInstance *soundInstance = new SoundInstance();
	soundInstance->status = sf::Music::Stopped;
	soundInstance->sound = validSoundStream;
	soundInstance->duration = duration;
	return soundInstance;
}

void WallpaperEngineMediaExtensions::DestroySound(SoundHandle handle)
{
	WINASSERT(handle != nullptr);
	unique_lock<mutex> lock(soundBufferMutex);
	SoundInstance *soundInstance = (SoundInstance*)handle;

	//if (soundInstance->sound->getStatus() != sf::Sound::Stopped)
	if (soundInstance->status != sf::Sound::Stopped && soundInstance->sound != nullptr)
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
	unique_lock<mutex> lock(soundBufferMutex);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	if (soundInstance->sound != nullptr)
	{
		soundInstance->sound->setLoop(loop);
		soundInstance->sound->play();
	}
	soundInstance->status = sf::Music::Playing;
}

void WallpaperEngineMediaExtensions::Pause(void *handle)
{
	WINASSERT(handle != nullptr);
	unique_lock<mutex> lock(soundBufferMutex);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	if (soundInstance->sound != nullptr)
	{
		soundInstance->sound->pause();
	}
	soundInstance->status = sf::Music::Paused;
}

void WallpaperEngineMediaExtensions::Stop(void *handle)
{
	WINASSERT(handle != nullptr);
	unique_lock<mutex> lock(soundBufferMutex);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	//if (soundInstance->status != sf::SoundSource::Stopped)
	if (soundInstance->sound != nullptr)
	{
		soundInstance->sound->stop();
	}
	soundInstance->status = sf::Music::Stopped;
}

void WallpaperEngineMediaExtensions::SetVolume(void *handle, float volume)
{
	WINASSERT(handle != nullptr);
	unique_lock<mutex> lock(soundBufferMutex);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	if (soundInstance->sound != nullptr)
	{
		soundInstance->sound->setVolume(100.0f * volume);
	}
}

int WallpaperEngineMediaExtensions::GetChannelCount(SoundHandle handle)
{
	WINASSERT(handle != nullptr);
	unique_lock<mutex> lock(soundBufferMutex);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	return soundInstance->sound ? soundInstance->sound->getChannelCount() : 0;
}

void WallpaperEngineMediaExtensions::SetAttenuation(SoundHandle handle, float attenuation)
{
	WINASSERT(handle != nullptr);
	unique_lock<mutex> lock(soundBufferMutex);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	if (soundInstance->sound != nullptr)
	{
		soundInstance->sound->setAttenuation(attenuation);
	}
}

void WallpaperEngineMediaExtensions::SetMinimumDistance(SoundHandle handle, float minDistance)
{
	WINASSERT(handle != nullptr);
	unique_lock<mutex> lock(soundBufferMutex);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	if (soundInstance->sound != nullptr)
	{
		soundInstance->sound->setMinDistance(minDistance);
	}
}

void WallpaperEngineMediaExtensions::SetPosition(SoundHandle handle, float *xyz)
{
	WINASSERT(handle != nullptr);
	unique_lock<mutex> lock(soundBufferMutex);
	SoundInstance *soundInstance = (SoundInstance*)handle;
	if (soundInstance->sound != nullptr)
	{
		//if ((soundInstance->flags & SoundInstance::IS_SPATIALIZATION_ACTIVE) == 0)
		//{
		//	soundInstance->flags |= SoundInstance::IS_SPATIALIZATION_ACTIVE;
		//	soundInstance->sound->setRelativeToListener(
		//}
		soundInstance->sound->setPosition(xyz[0], xyz[1], xyz[2]);
	}
}

void WallpaperEngineMediaExtensions::SetPositions(const uint32_t count, SoundHandle *handles, float *xyz)
{
	WINASSERT(handles != nullptr);
	unique_lock<mutex> lock(soundBufferMutex);
	for (uint32_t i = 0; i < count; ++i)
	{
		SoundInstance *soundInstance = (SoundInstance*)handles[i];
		if (soundInstance->sound != nullptr)
		{
			soundInstance->sound->setPosition(xyz[0], xyz[1], xyz[2]);
		}
	}
}
