# usairb

> :warning: Note that `usairb` is currently a learning project, and that the
> features described here do not yet work. I keep [a development log][devlog]
> on my blog.

[devlog]: https://fnune.com/devlog/usairb/2022/02/05/listening-to-devices-with-libudev-usairb-devlog-1/

The goal of [`usairb`][usairb-repo] (Universal Serial Air-Bus) is to transform
any embedded Linux device with access to the Internet into a multiplexing
transmitter for USB hubs: connect gadgets to it and use them remotely from your
desktop.

To achieve this, `usairb` uses [USB/IP][usbip]. USB/IP follows a server-client
architecture where the server or host is the device broadcasting its USB
gadgets, and the client can connect to them. USB/IP is available as a native
Kernel module on Linux for the host, and has multi-platform client programs.

While all planned features of `usairb` are achievable using just USB/IP,
`usairb` aims to provide a no-frills experience, potentially offering both a
client graphical user interface as well as very simple interface for the host
device.

[usairb-repo]: https://github.com/fnune/usairb
[usbip]: https://wiki.archlinux.org/title/USB/IP

## Development

To build the project, run `make`.

Dependencies:

- `make`, `gcc`, probably other base utilities,
- `usbip`

### Recommended utilities

- `usbutils` provides `lsusb`, which will be useful to debug `usairb`.
- [`bear`](https://github.com/rizsotto/Bear) will allow you to run `make
  compile_commands.json`, which makes the Clang LSP recognize items in your
  project.
