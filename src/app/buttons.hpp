#ifndef APP_BUTTONS_HPP
#define APP_BUTTONS_HPP

#include "event_loop.hpp"

namespace rmioc
{
    class screen;
}

namespace app
{

class buttons
{
public:
    buttons(
        rmioc::screen& screen_device
    );

    /**
     * Process input from the physical buttons.
     *
     * @param inhibit True to discard any event from the buttons.
     */
    void handle_event(int type, int buttonCode);

private:
    /** reMarkable screen device. */
    rmioc::screen& screen_device;
};

} // namespace app

#endif // APP_TOUCH_HPP
