#include "virtualkeyboard.hpp"
#include "screen.hpp"
#include "../rmioc/screen.hpp"
#include "common.h"
#include <iostream>
#include <tuple>


int mapAsciiToX11Key(int ascii) {
    // All ASCII printable characters
    if (ascii >= ' ' && ascii <= '~') {
        return ascii;
    }

    // All ASCII printable extended characters
    if (ascii >= 0x00a0 && ascii <= 0x00ff) {
        return ascii;
    }

    // ASCII unprintable characters
    switch (ascii) {
        case 8:                 return 0xff08;     // Backspace
        case 9:                 return 0xff09;     // Tab
        case 13:                return 0xff0d;     // Enter/Return
        case 27:                return 0xff1b;     // Escape
        case 127:               return 0xffff;     // Delete
        case INPUT_VKB_LEFT:    return 0xff51;
        case INPUT_VKB_UP:      return 0xff52;
        case INPUT_VKB_RIGHT:   return 0xff53;
        case INPUT_VKB_DOWN:    return 0xff54;
        case INPUT_VKB_HOME:    return 0xff50;
        case INPUT_VKB_END:     return 0xff57;
        case INPUT_VKB_PGUP:    return 0xff55;
        case INPUT_VKB_PGDOWN:  return 0xff56;
    }

    return 0;
}

int remapModifierKey(int key) {
    switch (key) {
        case INPUT_VKB_SHIFTMOD: return 0xffe1;
        case INPUT_VKB_CTRLMOD: return 0xffe3;
        case INPUT_VKB_ALTMOD: return 0xffe9;
    }
    return 0;
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
        if (keyCode != 0x0000) {
            if((keyCode & 0xFFFF) == 0) {
                // Modifier key being pressed down
                this->send_virtual_key_press(remapModifierKey(keyCode), type == INPUT_VKB_PRESS);
            } else {
                this->send_virtual_key_press(mapAsciiToX11Key(keyCode & 0xFFFF), type == INPUT_VKB_PRESS);
            }
        }
    }
}

} // namespace app
