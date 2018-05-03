
#include "pch.h"

#include "mediaextensionspublic.h"
#include "wallpaperenginemediaextensions.h"

using namespace std;

MEDIA_EXTENSIONS_VERSION_EXPORT;
MEDIA_EXTENSIONS_FACTORY
{
#ifdef _DEBUG
	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return new WallpaperEngineMediaExtensions();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		break;
	}
	return TRUE;
}
