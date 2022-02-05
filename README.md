# usairb

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

To build the project, run `make`. The dependencies are `make` and `gcc`.
