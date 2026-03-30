# Changelog

## v1.1.0

- Ported VNSee to QTFB and renamed it to VNSee-QTFB.
- Removed all of the code that isn't necessary for QTFB.
- Allows setting the encoding mode with `VNSEE_ENCODING`.
- Fixes all known bugs.

## v1.0.0

- Add support for virtual keyboard input at `/dev/input/virtual_keyboard`
- Switch to the fastest refresh mode touch or pen input is detected, and when the virtual keyboard is shown and hidden.
- Add support for VNC compression, and default to Hextile.
- Remove fake scrolling and pass all touch events raw.
- Configure the refresh mode from an environment variable.

## v0.4.1

- Fix support for release 2.9.1 on reMarkable 1.

## v0.4.0

- Add reMarkable 2 support (depends on [remarkable2-framebuffer](https://github.com/ddvk/remarkable2-framebuffer)).
- Tolerate server resolutions differing from 1404x1872 or 1408x1872.
    - The server image is cropped to fit in the screen.
- Add `vnsee-gui` script to start VNSee from a GUI.
    - This is a [simple](https://rmkit.dev/apps/sas) script.

## v0.3.1

- Fix compatibility with TigerVNC.
    - Fill-in the `depth` PixelFormat flag appropriately.

## v0.3.0

- Rename from rmvncclient to VNSee.
- Update libvncserver to 0.9.13.
- Add `--help` flag to show a help message.
- Add `--version` flag to show the current version number.
- Add `--no-buttons`, `--no-pen` and `--no-touch` flags to disable interactions with the respective input devices.
- Improve repaint latency (thanks @asmanur!)
    - Periodically trigger repaints during long update batches instead of waiting for the end of each batch.
    - Repaint continuously in DU mode when using the pen.

## v0.2.0

- Add support for pen input.
    - Do not support pressure and tilt sensitivity yet.
- Allow refreshing the screen manually by using the Home button.
- Allow quitting the app by using the Power button.

## v0.1.0

- Initial release.
