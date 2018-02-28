
#pragma once

#include "iwallpaperenginemediaextensions.h"

#define MEDIA_EXTENSIONS_VERSION "WallpaperEngineMediaExtensions0001"
#define MEDIA_EXTENSIONS_VERSION_EXPORT extern "C" __declspec(dllexport) const char *WallpaperEngineMedaExtensionVersion = MEDIA_EXTENSIONS_VERSION
#define MEDIA_EXTENSIONS_FACTORY extern "C" __declspec(dllexport) IWallpaperEngineMediaExtensions* __cdecl CreateMediaExtensions()
