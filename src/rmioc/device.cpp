#include "device.hpp"
#include "screen.hpp"
#include <fcntl.h>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>
#include <iostream>

namespace fs = std::filesystem;

namespace rmioc
{

device::device(
    std::unique_ptr<screen>&& screen_device
)
: screen_device(std::move(screen_device))
{}

auto device::detect(device_request request) -> device
{
    std::unique_ptr<screen> screen_device;
    screen_device = std::make_unique<screen>();

    return device(
        std::move(screen_device)
    );
}

auto device::get_screen() -> screen*
{
    return this->screen_device.get();
}

} // namespace rmioc
