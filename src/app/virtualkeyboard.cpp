#include "virtualkeyboard.hpp"
#include "screen.hpp"
#include "../rmioc/screen.hpp"
#include "common.h"
#include <iostream>
#include <tuple>


int mapQTToX11Key(int key) {
    switch (key) {
        case 0x01000003:    return 0xff08;      // Backspace
        case 0x01000001:    return 0xff09;      // Tab
        case 0x01000004:    return 0xff0d;      // Enter/Return
        case 0x01000000:    return 0xff1b;      // Escape
        case 0x01000007:    return 0xffff;      // Delete
        case 0x01000012:    return 0xff51;      // LEFT
        case 0x01000013:    return 0xff52;      // UP
        case 0x01000014:    return 0xff53;      // RIGHT
        case 0x01000015:    return 0xff54;      // DOWN
        case 0x01000010:    return 0xff50;      // HOME
        case 0x01000011:    return 0xff57;      // END
        case 0x01000016:    return 0xff55;      // PGUP
        case 0x01000017:    return 0xff56;      // PGDOWN
        case 0x01000020:    return 0xffe1;      // Shift
        case 0x01000021:    return 0xffe3;      // Control
        case 0x01000023:    return 0xffe9;      // Alt
    }

    // Mask the pure ascii:
    return key & 0xFF;
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
            this->send_virtual_key_press(mapQTToX11Key(keyCode), type == INPUT_VKB_PRESS);
        }
    }
}

} // namespace app
