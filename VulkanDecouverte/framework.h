#pragma once

#include "libs/im_gui/imgui.h"
#include "libs/im_gui/imgui_impl_glfw.h"
#include "libs/im_gui/imgui_impl_vulkan.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

#include <iostream>
#include <vector>
#include <optional>
#include <array>
