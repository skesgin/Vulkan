/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef VKTS_CORE_HPP_
#define VKTS_CORE_HPP_

/**
 *
 * C++11 Libraries.
 *
 */

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cinttypes>
#include <cmath>
#include <condition_variable>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

/**
 *
 * GLM Library.
 * https://www.opengl.org/sdk/libs/GLM/
 *
 */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 *
 * Vulkan Library.
 * https://www.khronos.org/vulkan
 *
 */

#include <vulkan/vulkan.h>

/**
 *
 * VKTS Start.
 *
 */

/**
 * OS platform.
 */

#include <vkts/fn_os_platform.hpp>

/**
 * API platform.
 */

#include <vkts/fn_api_platform.hpp>

//

/**
 * Defines.
 */

#define VKTS_MAX_LOG_CHARS 2048

/**
 * Types.
 */

typedef enum VkTsSearch_
{
    VKTS_SEARCH_ABSOLUTE = 0, VKTS_SEARCH_RELATVE = 1
} VkTsSearch;

/**
 * Base.
 */

#include <vkts/core/base/ICloneable.hpp>
#include <vkts/core/base/IDestroyable.hpp>
#include <vkts/core/base/IResetable.hpp>
#include <vkts/core/base/IUpdateable.hpp>
#include <vkts/core/base/List.hpp>
#include <vkts/core/base/SmartPointerList.hpp>
#include <vkts/core/base/SmartPointerVector.hpp>
#include <vkts/core/base/ThreadsafeQueue.hpp>
#include <vkts/core/base/Vector.hpp>

#include <vkts/core/base/Map.hpp>
#include <vkts/core/base/SmartPointerMap.hpp>

/**
 * Log.
 */

#include <vkts/core/log/fn_log.hpp>

/**
 * Time.
 */

#include <vkts/core/time/fn_time.hpp>

/**
 * Processor.
 */

#include <vkts/core/processor/fn_processor.hpp>

/**
 * Profile.
 */

#include <vkts/core/profile/fn_profile.hpp>

/**
 * Binary buffer.
 */

#include <vkts/core/binary_buffer/IBinaryBuffer.hpp>

#include <vkts/core/binary_buffer/fn_binary_buffer.hpp>

/**
 * Text buffer.
 */

#include <vkts/core/text_buffer/ITextBuffer.hpp>

#include <vkts/core/text_buffer/fn_text_buffer.hpp>

/**
 * File.
 */

#include <vkts/core/file/fn_file.hpp>

#endif /* VKTS_CORE_HPP_ */
