#ifndef RMIOC_DEVICE_HPP
#define RMIOC_DEVICE_HPP

#include "flags.hpp"
#include "screen.hpp"
#include "qtfb-client.h"
#include <boost/preprocessor/arithmetic/limits/dec_256.hpp>
#include <boost/preprocessor/arithmetic/limits/inc_256.hpp>
#include <boost/preprocessor/control/expr_iif.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/detail/limits/auto_rec_256.hpp>
#include <boost/preprocessor/logical/limits/bool_256.hpp>
#include <boost/preprocessor/repetition/detail/limits/for_256.hpp>
#include <boost/preprocessor/repetition/for.hpp>
#include <boost/preprocessor/seq/limits/elem_256.hpp>
#include <boost/preprocessor/seq/limits/size_256.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/limits/to_seq_64.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/variadic/limits/elem_64.hpp>
#include <memory>

namespace rmioc
{

/** Devices that can be opened. */
RMIOC_FLAGS_DEFINE(
    device_request,
    buttons, touch, pen, screen, virtualkeyboard
);

/**
 * Access input and output devices.
 *
 * This class enables access to the buttons, touch and pen inputs and
 * screen output while abstracting differences between rM1 and rM2.
 */
class device
{
public:
    /**
     * Automatically detect the current device type and open the requested
     * input and output devices.
     *
     * @param request Devices to open.
     */
    static device detect(device_request request);

    /** Access the screen device, if possible. */
    screen* get_screen();

private:
    device(
        std::unique_ptr<screen>&& screen_device
    );

    std::unique_ptr<screen> screen_device;
};

} // namespace

#endif // RMIOC_DEVICE_HPP
