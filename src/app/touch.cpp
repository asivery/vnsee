#include "touch.hpp"
#include "screen.hpp"
#include "../rmioc/screen.hpp"
#include <cstdlib>
#include <functional>
#include <map>
#include <utility>
#include <iostream>
// IWYU pragma: no_include <ratio>
// IWYU pragma: no_include <type_traits>

namespace app
{

touch::touch(
    app::screen& screen,
    MouseCallback send_button_press
)
: screen(screen)
, send_button_press(std::move(send_button_press))
{}

void touch::handle_event(int type, int x, int y)
{
    if (type == INPUT_TOUCH_PRESS || type == INPUT_PEN_PRESS)
    {
        this->screen.set_repaint_mode(screen::repaint_modes::fast);
    }

    this->send_button_press(
        x, y,
        MouseButton::Left
    );

    if (type == INPUT_TOUCH_RELEASE || type == INPUT_PEN_RELEASE) {
        this->send_button_press(
            x, y,
            MouseButton::None
        );

        this->screen.set_repaint_mode(screen::repaint_modes::standard);
        this->screen.repaint();
    }
}

} // namespace app
