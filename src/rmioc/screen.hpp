#ifndef RMIOC_SCREEN_HPP
#define RMIOC_SCREEN_HPP

#include "qtfb-client.h"
#include <cstdint>

namespace mxcfb
{
    struct update_data;
}

namespace rmioc
{

/**
 * Information about the location of a RGB component in a packed pixel.
 */
struct component_format
{
    /** Offset of the first bit of the component. */
    unsigned short offset;

    /** Number of contiguous bits used to represent the component. */
    unsigned short length;

    /** Maximum value. */
    std::uint32_t max() const;
}; // struct component_format

class screen
{
public:
    /**
     * Connect to the qtfb server.
     */
    screen();

    void update(
        int x, int y, int w, int h,
        int mode = REFRESH_MODE_UI,
        bool wait = false);

    void update(
        int mode = REFRESH_MODE_UI,
        bool wait = true);

    std::uint8_t* get_data();

    int get_xres() const;
    int get_yres() const;

    unsigned short get_bits_per_pixel() const;
    component_format get_red_format() const;
    component_format get_green_format() const;
    component_format get_blue_format() const;

    auto get_connection() -> qtfb::ClientConnection&;

private:
    /** File descriptor for the shared memory framebuffer. */
    int framebuf_fd = -1;

    /** Identifier of the framebuffer message queue. */
    int msgqueue_id = -1;

    /** Pointer to the memory-mapped framebuffer. */
    std::uint8_t* framebuf_ptr = nullptr;

    /* qtfb client connection kept as a member so the connection object
       outlives the constructor (was previously a local variable). */
    qtfb::ClientConnection conn;
}; // class screen

} // namespace rmioc

#endif // RMIOC_screen_HPP
