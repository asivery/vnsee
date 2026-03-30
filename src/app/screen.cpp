#include "screen.hpp"
#include "../log.hpp"
#include "../rmioc/screen.hpp"
#include <algorithm>
#include <chrono>
#include <climits>
#include <cstring>
#include <ostream>
#include <stdexcept>
#include <rfb/rfbclient.h>
// IWYU pragma: no_include <type_traits>

namespace chrono = std::chrono;

namespace app
{

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-avoid-magic-numbers)
void* screen::instance_tag = reinterpret_cast<void*>(6803);

screen::screen(rmioc::screen& device, rfbClient* vnc_client)
: device(device)
, vnc_client(vnc_client)
, repaint_mode(repaint_modes::standard)
, standard_repaint_delay(500)
, fast_repaint_delay(50) // 15 FPS
, standard_waveform_mode(REFRESH_MODE_UI)
, fast_waveform_mode(REFRESH_MODE_ANIMATE)
{
    char *env_waveform = std::getenv("VNSEE_WAVEFORM_MODE");

    if (env_waveform != NULL) {
        if (strcmp(env_waveform, "FASTEST") == 0) {
            standard_repaint_delay = chrono::milliseconds(50);
            standard_waveform_mode = REFRESH_MODE_ANIMATE;
        } else if (strcmp(env_waveform, "FAST") == 0) {
            standard_repaint_delay = chrono::milliseconds(200);
            standard_waveform_mode = REFRESH_MODE_FAST;
        } else if (strcmp(env_waveform, "STANDARD") == 0) {
            standard_repaint_delay = chrono::milliseconds(500);
            standard_waveform_mode = REFRESH_MODE_UI;
        } else if (strcmp(env_waveform, "SLOW") == 0) {
            standard_repaint_delay = chrono::milliseconds(1000);
            standard_waveform_mode = REFRESH_MODE_CONTENT;
        }
    }

    rfbClientSetClientData(
        this->vnc_client,
        screen::instance_tag,
        this
    );

    // Ask the server to send pixels in the same format as the screen buffer
    this->vnc_client->format.bitsPerPixel = this->device.get_bits_per_pixel();
    this->vnc_client->format.depth = this->device.get_bits_per_pixel();
    this->vnc_client->format.redShift = this->device.get_red_format().offset;
    this->vnc_client->format.redMax = this->device.get_red_format().max();
    this->vnc_client->format.greenShift = this->device.get_green_format().offset;
    this->vnc_client->format.greenMax = this->device.get_green_format().max();
    this->vnc_client->format.blueShift = this->device.get_blue_format().offset;
    this->vnc_client->format.blueMax = this->device.get_blue_format().max();

    char *env_encoding = std::getenv("VNSEE_ENCODING");
    if (env_encoding != NULL) {
        if (strcmp(env_encoding, "RAW") == 0) {
            this->vnc_client->appData.encodingsString = "raw";
        } else if (strcmp(env_encoding, "COPYRECT") == 0) {
            this->vnc_client->appData.encodingsString = "copyrect";
        } else if (strcmp(env_encoding, "TIGHT") == 0) {
            this->vnc_client->appData.encodingsString = "tight";
            this->vnc_client->appData.compressLevel = 9;
            this->vnc_client->appData.enableJPEG = true;
            this->vnc_client->appData.qualityLevel = 0;
        } else if (strcmp(env_encoding, "HEXTILE") == 0) {
            this->vnc_client->appData.encodingsString = "hextile";
        } else if (strcmp(env_encoding, "ZLIB") == 0) {
            this->vnc_client->appData.encodingsString = "zlib";
            this->vnc_client->appData.compressLevel = 9;
        } else if (strcmp(env_encoding, "ZLIBHEX") == 0) {
            this->vnc_client->appData.encodingsString = "zlibhex";
            this->vnc_client->appData.compressLevel = 9;
        } else if (strcmp(env_encoding, "TRLE") == 0) {
            this->vnc_client->appData.encodingsString = "trle";
        } else if (strcmp(env_encoding, "ZRLE") == 0) {
            this->vnc_client->appData.encodingsString = "zrle";
        } else if (strcmp(env_encoding, "ZYWRLE") == 0) {
            this->vnc_client->appData.encodingsString = "zywrle";
            this->vnc_client->appData.qualityLevel = 0;
        } else if (strcmp(env_encoding, "ULTRA") == 0) {
            this->vnc_client->appData.encodingsString = "ultra";
        } else if (strcmp(env_encoding, "ULTRAZIP") == 0) {
            this->vnc_client->appData.encodingsString = "ultrazip";
        } else if (strcmp(env_encoding, "CORRE") == 0) {
            this->vnc_client->appData.encodingsString = "corre";
        } else if (strcmp(env_encoding, "RRE") == 0) {
            this->vnc_client->appData.encodingsString = "rre";
        }
    } else {
        this->vnc_client->appData.encodingsString = "copyrect";
    }

    this->vnc_client->appData.useRemoteCursor = true;
    this->vnc_client->MallocFrameBuffer = screen::create_framebuf;
    this->vnc_client->GotFrameBufferUpdate = screen::commit_updates;
}

void screen::repaint()
{
    // Clear the has_update flag only in standard repaint mode
    // In fast mode, a clean update will be needed in the future
    if (this->repaint_mode == repaint_modes::standard)
    {
        this->update_info.has_update = false;
    }

    this->last_repaint = chrono::steady_clock::now();

    vnsee::log::print("Screen update")
        << this->update_info.w << 'x' << this->update_info.h << '+'
        << this->update_info.x << '+' << this->update_info.y << '\n';

    this->device.update(
        this->update_info.x, this->update_info.y,
        this->update_info.w, this->update_info.h,
        this->repaint_mode == repaint_modes::standard
            ? standard_waveform_mode
            : fast_waveform_mode
    );
}

auto screen::get_xres() -> int
{
    return this->device.get_xres();
}

auto screen::get_yres() -> int
{
    return this->device.get_yres();
}

void screen::set_repaint_mode(repaint_modes mode)
{
    this->repaint_mode = mode;

    vnsee::log::print("Screen update") << (mode == repaint_modes::standard
        ? "Switched to standard mode\n"
        : "Switched to fast mode\n");
}

auto screen::event_loop() -> event_loop_status
{
    if (this->update_info.has_update)
    {
        auto next_update_time = this->last_repaint + (
            this->repaint_mode == repaint_modes::standard
            ? standard_repaint_delay
            : fast_repaint_delay
        );

        auto now = chrono::steady_clock::now();
        long wait_time = chrono::duration_cast<chrono::milliseconds>(
            next_update_time - now
        ).count();

        if (wait_time <= 0)
        {
            this->repaint();
        }
        else
        {
            // Wait until the next update is due
            return {
                /* quit = */ false,
                /* timeout = */ wait_time
            };
        }
    }

    return {/* quit = */ false, /* timeout = */ -1};
}

auto screen::create_framebuf(rfbClient* vnc_client) -> rfbBool
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    auto* that = reinterpret_cast<screen*>(
        rfbClientGetClientData(
            vnc_client,
            screen::instance_tag
        ));

    int xres = static_cast<int>(that->device.get_xres());
    int yres = static_cast<int>(that->device.get_yres());

    if (vnc_client->width < 0 || vnc_client->height < 0)
    {
        std::stringstream msg;
        msg << "Invalid server resolution ("
            << vnc_client->width << 'x' << vnc_client->height;
        throw std::runtime_error{msg.str()};
    }

    if (vnc_client->width > xres || vnc_client->height > yres)
    {
        std::cerr << "Warning: The server resolution ("
            << vnc_client->width << 'x' << vnc_client->height
            << ") does not fit in the screen ("
            << xres << 'x' << yres << ")\nThe image will be cropped to fit\n";
    }

    vnc_client->frameBuffer = that->device.get_data();

    return TRUE;
}

void screen::commit_updates(rfbClient* vnc_client, int x, int y, int w, int h)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    auto* that = reinterpret_cast<screen*>(
        rfbClientGetClientData(
            vnc_client,
            screen::instance_tag
        ));

    // Register the region as pending update, potentially extending
    // an existing one
    vnsee::log::print("VNC update") << w << 'x' << h << '+' << x << '+' << y << '\n';

    if (that->update_info.has_update)
    {
        // Merge new rectangle with existing one
        int left_x = std::min(x, that->update_info.x);
        int top_y = std::min(y, that->update_info.y);
        int right_x = std::max(x + w,
                that->update_info.x + that->update_info.w);
        int bottom_y = std::max(y + h,
                that->update_info.y + that->update_info.h);

        that->update_info.x = left_x;
        that->update_info.y = top_y;
        that->update_info.w = right_x - left_x;
        that->update_info.h = bottom_y - top_y;
    }
    else
    {
        that->update_info.x = x;
        that->update_info.y = y;
        that->update_info.w = w;
        that->update_info.h = h;
        that->update_info.has_update = true;
    }
}

} // namespace app
