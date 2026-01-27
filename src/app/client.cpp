#include "client.hpp"
#include "../log.hpp"
#include "../rmioc/device.hpp"
#include "qtfb-client.h"
#include <algorithm>
#include <bitset>
#include <cerrno>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <vector>
#include <poll.h>
#include <rfb/rfbclient.h>
#include <unistd.h>
#include <thread>
// IWYU pragma: no_include <type_traits>

/** Custom log printer for the VNC client library.  */
// NOLINTNEXTLINE(cert-dcl50-cpp): Need to use a vararg function for C compat
void vnc_client_log(const char* format, ...)
{
    va_list args;

    // ↓ Use of C library
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-no-array-decay)
    va_start(args, format);

    // Use a copy of args to compute required buffer size because vsnprintf
    // consumes the va_list.
    va_list args_copy;
    va_copy(args_copy, args);

    // NOLINTNEXTLINE(hicpp-no-array-decay): Use of C library
    ssize_t buffer_size = vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    std::vector<char> buffer(buffer_size + 1);

    // NOLINTNEXTLINE(hicpp-no-array-decay): Use of C library
    vsnprintf(buffer.data(), buffer.size(), format, args);

    vnsee::log::print("VNC message") << buffer.data();

    // NOLINTNEXTLINE(hicpp-no-array-decay): Use of C library
    va_end(args);
}

namespace app
{

using namespace std::placeholders;

client::client(const char* ip, int port, const char* password, rmioc::device& device)
: vnc_client(rfbGetClient(0, 0, 0))
{
    if (device.get_screen() == nullptr)
    {
        throw std::runtime_error{"Missing screen device"};
    }

    auto& screen_device = *device.get_screen();

    // Initialize the member screen_handler (avoid shadowing a local variable).
    this->screen_handler = std::make_unique<screen>(screen_device, vnc_client);

    auto virtualkeyboard_callback = [this](int keyCode, bool down)
    {
        this->send_virtual_key_press(keyCode, down);
    };

    // Initialize the member virtualkeyboard_handler (avoid shadowing a local variable).
    this->virtualkeyboard_handler = std::make_unique<virtualkeyboard>(*this->screen_handler, virtualkeyboard_callback);

    // Initialize the member touch_handler (avoid shadowing a local variable).
    this->buttons_handler = std::make_unique<buttons>(screen_device);

    auto button_callback = [this](int x, int y, MouseButton button)
    {
        this->send_button_press(x, y, button);
    };

    // Initialize the member touch_handler (avoid shadowing a local variable).
    this->touch_handler = std::make_unique<touch>(*this->screen_handler, button_callback);

    rfbClientLog = vnc_client_log;
    rfbClientErr = vnc_client_log;

    // ↓ Use of C library
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory,cppcoreguidelines-no-malloc)
    free(this->vnc_client->serverHost);
    this->vnc_client->serverHost = strdup(ip);
    this->vnc_client->serverPort = port;

    // If password is not empty, set a GetPassword function to return it.
    if(strcmp(password, "") != 0)
    {
        // Seems to need a static variable to work.
        static char* staticPassword = nullptr;
        // Will be freed by GetPassword after use, no need to free anything manually.
        staticPassword = strdup(password);

        this->vnc_client->GetPassword = [](rfbClient*) -> char*
        {
            return staticPassword;
        };
    }

    if (rfbInitClient(this->vnc_client, nullptr, nullptr) == 0)
    {
        throw std::runtime_error{"Failed to initialize VNC connection"};
    }

    // create a pointer to the screen device and capture it by value in the lambda.
    // detach the thread so its destructor won't call std::terminate.
    {
        auto* device_ptr = &device;
        auto* screen_ptr = this->screen_handler.get();
        auto* touch_ptr = this->touch_handler.get();
        auto* virtualkeyboard_ptr = this->virtualkeyboard_handler.get();
        auto* buttons_ptr = this->buttons_handler.get();

        std::thread([device_ptr, screen_ptr, touch_ptr, virtualkeyboard_ptr, buttons_ptr]() {
            qtfb::ServerMessage externalMessage;
            while (true) {
                device_ptr->get_screen()->get_connection().pollServerPacket(externalMessage);

                if (externalMessage.userInput.inputType == INPUT_TOUCH_PRESS || externalMessage.userInput.inputType == INPUT_TOUCH_UPDATE || externalMessage.userInput.inputType == INPUT_TOUCH_RELEASE || externalMessage.userInput.inputType == INPUT_PEN_PRESS || externalMessage.userInput.inputType == INPUT_PEN_UPDATE || externalMessage.userInput.inputType == INPUT_PEN_RELEASE) {
                    touch_ptr->handle_event(
                        externalMessage.userInput.inputType,
                        externalMessage.userInput.x,
                        externalMessage.userInput.y
                    );
                }

                if (externalMessage.userInput.inputType == INPUT_VKB_PRESS || externalMessage.userInput.inputType == INPUT_VKB_RELEASE) {
                    virtualkeyboard_ptr->handle_event(
                        externalMessage.userInput.inputType,
                        externalMessage.userInput.x
                    );
                }

                if (externalMessage.userInput.inputType == INPUT_BTN_PRESS || externalMessage.userInput.inputType == INPUT_BTN_RELEASE) {
                    buttons_ptr->handle_event(
                        externalMessage.userInput.inputType,
                        externalMessage.userInput.x
                    );
                }
            }
        }).detach();
    }

    this->poll_vnc = this->polled_fds.size();
    this->polled_fds.push_back(pollfd{
        /* fd = */ this->vnc_client->sock,
        /* events = */ POLLIN,
        /* revents = */ 0
    });
}

client::~client()
{
    rfbClientCleanup(this->vnc_client);
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto client::event_loop() -> bool
{
    // Maximum time to wait before timeout in the next poll
    long timeout = -1;

    // Flag used for quitting the event loop
    bool quit = false;

    auto handle_status = [&timeout, &quit](const event_loop_status& st)
    {
        if (st.quit)
        {
            quit = true;
        }
        else
        {
            if (timeout == -1)
            {
                timeout = st.timeout;
            }
            else if (st.timeout != -1)
            {
                timeout = std::min(timeout, st.timeout);
            }
        }
    };

    // Wait for events from the VNC server or from device inputs
    while (!quit)
    {
        while (poll(
                    this->polled_fds.data(),
                    this->polled_fds.size(),
                    static_cast<int>(timeout)) == -1)
        {
            if (errno != EAGAIN)
            {
                throw std::system_error(
                    errno,
                    std::generic_category(),
                    "(client::event_loop) Wait for message"
                );
            }
        }

        timeout = -1;

        // NOLINTNEXTLINE(hicpp-signed-bitwise): Use of C library
        if ((polled_fds[this->poll_vnc].revents & POLLIN) != 0)
        {
            if (HandleRFBServerMessage(this->vnc_client) == 0)
            {
                return false;
            }
        }

        handle_status(this->screen_handler->event_loop());
    }

    return true;
}

void client::send_button_press(
    int x, int y,
    MouseButton button
)
{
    auto button_flag = static_cast<std::uint8_t>(button);
    constexpr auto bits = 8 * sizeof(button_flag);

    vnsee::log::print("Button press")
        << x << 'x' << y << " (button mask: "
        << std::setfill('0') << std::setw(bits)
        << std::bitset<bits>(button_flag) << ")\n";

    SendPointerEvent(this->vnc_client, x, y, button_flag);
}

void client::send_virtual_key_press(
    int keyCode, bool down
)
{
    vnsee::log::print("Virtual key press")
        << keyCode << "\n";

    SendKeyEvent(this->vnc_client, keyCode, down);
}

} // namespace app
