// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <chrono>
#include <iostream>
#include <filesystem>
#include <format>
#include <fstream>

#include <assert.h>
#define ASSERT assert
