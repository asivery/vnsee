#ifndef APP_VIRTUALKEYBOARD_HPP
#define APP_VIRTUALKEYBOARD_HPP

#include "event_loop.hpp"

namespace rmioc
{
    class screen;
    class virtualkeyboard;
}

namespace app
{

class screen;
class virtualkeyboard
{
public:
    virtualkeyboard(
        app::screen& screen,
        VirtualKeyboardCallback send_virtual_key_press
    );

    /**
     * Process input from the software keyboard.
     */
    void handle_event(int type, int keyCode);

private:
    app::screen& screen;

    VirtualKeyboardCallback send_virtual_key_press;
};

} // namespace app

#endif // APP_KEYBOARD_HPP
