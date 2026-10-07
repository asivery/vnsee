#ifndef APP_BUTTONS_HPP
#define APP_BUTTONS_HPP

#include "event_loop.hpp"
#include "dep_provider.hpp"
#include "../rmioc/device.hpp"
#include <optional>

namespace rmioc
{
    class screen;
}

namespace app
{

class buttons
{
public:
    typedef provider<rmioc::screen, std::optional<rmioc::screen_request_parameters> > screen_provider_t;

    buttons(
        screen_provider_t& screen_provider
    );

    /**
     * Process input from the physical buttons.
     *
     * @param inhibit True to discard any event from the buttons.
     */
    void handle_event(int type, int buttonCode);

private:
    /** reMarkable screen device provider. */
    screen_provider_t& screen_provider;
};

} // namespace app

#endif // APP_TOUCH_HPP
