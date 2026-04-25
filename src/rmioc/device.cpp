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

auto device::detect(device_request request, std::optional<screen_request_parameters> screen_params) -> device
{
    std::unique_ptr<screen> screen_device;
    std::optional<std::tuple<uint16_t, uint16_t>> customResolution = {};
    if(screen_params) {
        customResolution = std::make_tuple((uint16_t)screen_params.value().width, (uint16_t)screen_params.value().height);
    }
    screen_device = std::make_unique<screen>(customResolution);

    return device(
        std::move(screen_device)
    );
}

auto device::get_screen() -> screen*
{
    return this->screen_device.get();
}

} // namespace rmioc
