#include "buttons.hpp"
#include "../rmioc/screen.hpp"
#include <linux/input-event-codes.h>

namespace app
{

buttons::buttons(
    rmioc::screen& screen_device
)
: screen_device(screen_device)
{}

void buttons::handle_event(int type, int buttonCode)
{
    if (type == INPUT_BTN_PRESS && buttonCode == KEY_POWER)
    {
        exit(0);
    }

    if (type == INPUT_BTN_PRESS && buttonCode == KEY_HOME)
    {
        // Full screen refresh when pressing home
        this->screen_device.update();
    }
}

} // namespace app
