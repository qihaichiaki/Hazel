#pragma once

/**
 * 预编译头文件, 加快编译速度
 */

#include <iostream>
#include <memory>
#include <functional>
#include <utility>
#include <algorithm>

#include <array>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Hazel/Core/log.h"
#include "Hazel/Debug/instrumentor.h"

#ifdef _WIN32
    #include <Windows.h>
#endif