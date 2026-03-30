#include "virtualkeyboard.hpp"
#include "screen.hpp"
#include "../rmioc/screen.hpp"
#include <iostream>
#include <tuple>

const bool isShifted(int ascii) {
    if (ascii >= '!' && ascii <= '&') return true;
    if (ascii >= '(' && ascii <= '+') return true;
    if (ascii >= ':' && ascii <= ':') return true;
    if (ascii >= '<' && ascii <= '<') return true;
    if (ascii >= '>' && ascii <= 'Z') return true;
    if (ascii >= '^' && ascii <= '_') return true;
    if (ascii >= '{' && ascii <= '~') return true;

    if (ascii >= 0xA2 && ascii <= 0xA3) return true; // ¢ to £
    if (ascii >= 0xA6 && ascii <= 0xA8) return true; // ¦ to ¨
    if (ascii >= 0xAF && ascii <= 0xB0) return true; // ¯ to °
    if (ascii >= 0xB9 && ascii <= 0xB0) return true; // ¹ to °
    if (ascii >= 0xC0 && ascii <= 0xD6) return true; // À to Ö
    if (ascii >= 0xD8 && ascii <= 0xDE) return true; // Ø to Þ

    return false;
}

std::tuple<int, bool> mapAsciiToX11Key(int ascii) {
    const bool shifted = isShifted(ascii);

    // All ASCII printable characters
    if (ascii >= ' ' && ascii <= '~') {
        return {ascii, shifted};
    }

    // All ASCII printable extended characters
    if (ascii >= 0x00a0 && ascii <= 0x00ff) {
        return {ascii, shifted};
    }

    // ASCII unprintable characters
    switch (ascii) {
        case 8:   return {0xff08, false};     // Backspace
        case 9:   return {0xff09, false};     // Tab
        case 13:  return {0xff0d, false};     // Enter/Return
        case 27:  return {0xff1b, false};     // Escape
        case 127: return {0xffff, false};     // Delete
    }

    return {0x0000, false};
}

namespace app
{

virtualkeyboard::virtualkeyboard(
    app::screen& screen,
    VirtualKeyboardCallback send_virtual_key_press
)
: screen(screen)
, send_virtual_key_press(std::move(send_virtual_key_press))
{}

void virtualkeyboard::handle_event(int type, int keyCode)
{
    if (keyCode == 0xf001) {
        if (type == INPUT_VKB_PRESS) {
            this->screen.set_repaint_mode(screen::repaint_modes::fast);
        }
    } else if (keyCode == 0xf000) {
        if (type == INPUT_VKB_PRESS) {
            this->screen.set_repaint_mode(screen::repaint_modes::standard);
            this->screen.repaint();
        }
    } else {
        auto [keySym, isShifted] = mapAsciiToX11Key(keyCode);

        if (keySym != 0x0000) {
            if (isShifted) {
                this->send_virtual_key_press(0xffe1, type == INPUT_VKB_PRESS);
            }

            this->send_virtual_key_press(keySym, type == INPUT_VKB_PRESS);
        }
    }
}

} // namespace app
