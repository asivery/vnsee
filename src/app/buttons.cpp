#include "buttons.hpp"
#include "../rmioc/screen.hpp"
#include <linux/input-event-codes.h>

namespace app
{

buttons::buttons(
    screen_provider_t& screen_provider
)
: screen_provider(screen_provider)
{}

void buttons::handle_event(int type, int buttonCode)
{
    if (type == INPUT_BTN_PRESS && buttonCode == KEY_POWER)
    {
        exit(0);
    }

    rmioc::screen* screen_device;

    if (type == INPUT_BTN_PRESS && buttonCode == KEY_HOME && (screen_device = screen_provider.fetch()))
    {
        // Full screen refresh when pressing home
        screen_device->update();
    }
}

} // namespace app
