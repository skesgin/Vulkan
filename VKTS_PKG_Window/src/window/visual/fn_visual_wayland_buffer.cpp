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

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <memory>
#include <sys/mman.h>
#include <vkts/window/vkts_window.hpp>

namespace vkts
{

static int _waylandCreateAnonymousFile(int32_t size)
{
    static const std::string fileName = "/weston-shared-XXXXXX";

    const std::string path = getenv("XDG_RUNTIME_DIR");
    if (path.empty())
    {
    	return -1;
    }

    std::string name = path + fileName;
    char* cName = const_cast<char*>(name.data());

    int fd = mkstemp(cName);
    if (fd < 0)
    {
    	return -1;
    }

    if (unlink(cName) == -1)
    {
    	close(fd);

    	return -1;
    }

    int flags = fcntl(fd, F_GETFD);
    if (flags == -1)
    {
    	close(fd);

    	return -1;
    }
    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
    {
    	close(fd);

    	return -1;
    }
    if (ftruncate(fd, size) < 0)
    {
    	close(fd);

    	return -1;
    }

    return fd;
}

struct wl_buffer* VKTS_APIENTRY _waylandCreateBuffer(struct wl_shm* shm, int32_t width, int32_t height)
{
    int32_t stride = width * 4;
    int32_t size = stride * height;

    auto fd = _waylandCreateAnonymousFile(size);
    if (fd < 0)
    {
    	return nullptr;
    }

    //

    void* data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
    {
    	close(fd);

    	return nullptr;
    }

    memset(data, 255, size);

    munmap(data, size);

    //

    struct wl_shm_pool* pool = wl_shm_create_pool(shm, fd, size);
    if (!pool)
    {
    	close(fd);

    	return nullptr;
    }

    struct wl_buffer* buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);

    wl_shm_pool_destroy(pool);

    close(fd);

    return buffer;
}

}
