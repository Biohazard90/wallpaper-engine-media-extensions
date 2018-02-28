
#pragma once

#include <algorithm>
#include <assert.h>
#include <atomic>
#include <cstdint>
#include <functional>
#include <math.h>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <windows.h>

#ifdef DEBUG_PC
#include <assert.h>
#define WINASSERT(x) assert(x)
#else // NOT DEBUG_PC
#define WINASSERT(x)
#endif // end DEBUG_PC

#define SFML_STATIC 1

#include "SFML/Audio.hpp"
