#pragma once

#include <functional>
#include <optional>

#include "2D/Vertex.h"
#include "2D/Shader.h"
#include "3D/Vertex.h"
#include "3D/Shader.h"
#include "3D/PerspectiveTest.h"

#include "RendererInterface.h"

#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#include "Windows/WindowsGDI.h"
#include "Windows/WindowsRSI.h"
#endif

#define VK_USE_PLATFORM_WIN32_KHR
#include "Vulkan/Include/vulkan/vulkan.h"
#include "Vulkan/VulkanRHI.h"

using namespace CK;
