#ifndef APP_TOUCH_HPP
#define APP_TOUCH_HPP

#include "event_loop.hpp"
#include <chrono>

namespace rmioc
{
    class screen;
    class touch;
}

namespace app
{

class screen;
class touch
{
public:
    touch(
        app::screen& screen,
        MouseCallback send_button_press
    );

    /**
     * Process events from the touchscreen.
     *
     * @param inhibit True to discard any event from the touchscreen.
     */
    void handle_event(int type, int x, int y);

private:
    /** reMarkable screen. */
    app::screen& screen;

    /** Callback for sending mouse events. */
    MouseCallback send_button_press;

    /**
     * Called when the touch point position changes.
     *
     * @param x New X position of the touch point.
     * @param y New Y position of the touch point.
     */
    void on_update(int x, int y);

    /** Called when all touch points are removed from the screen. */
    void on_end();

    /** Current state of the touch interaction. */
    enum class TouchState
    {
        /** No touch point is active. */
        Inactive,

        /** Touch points are active but did not move enough to scroll. */
        Tap,

        /** Touch points are active and scrolling horizontally. */
        ScrollX,

        /** Touch points are active and scrolling vertically. */
        ScrollY,
    } state = TouchState::Inactive;

    /** Starting time of the current touch interaction. */
    std::chrono::steady_clock::time_point touch_start{};

    /** Current X position of the touch interaction, if not inactive. */
    int x = 0;

    /** Current Y position of the touch interaction, if not inactive. */
    int y = 0;

    /** Initial X position of the touch interaction, if not inactive. */
    int x_last = 0;

    /** Initial Y position of the touch interaction, if not inactive. */
    int y_last = 0;

    /**
     * Total number of horizontal scroll events that were sent in this
     * interaction, positive for scrolling right and negative for
     * scrolling left.
     */
    int x_scroll_events = 0;

    /**
     * Total number of vertical scroll events that were sent in this
     * interaction, positive for scrolling down and negative for
     * scrolling up.
     */
    int y_scroll_events = 0;
};

} // namespace app

#endif // APP_TOUCH_HPP
