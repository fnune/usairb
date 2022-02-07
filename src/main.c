#include <fcntl.h>
#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connection.h"
#include "external/libusbip/libusbip/libusbip.h"

int USAIRB_PORT = 3240;

int main(void) {
  int server_socket = usairb_connect(USAIRB_PORT);
  int client_socket = usairb_accept(server_socket);

  struct libusbip_connection_info usbip_connection_info = {
      .ctx = LIBUSBIP_CTX_SERVER,
      .server_sock = server_socket,
      .client_sock = client_socket,
  };

  libusbip_error_t usbip_init_error = libusbip_init(&usbip_connection_info);
  if (usbip_init_error) {
    fprintf(stderr, "libusbip_init returned error code %i\n", usbip_init_error);
    exit(EXIT_FAILURE);
  }

  // TODO: move udev stuff to a different file.
  struct udev *udev = udev_new();
  if (!udev) {
    fprintf(stderr, "udev_new returned NULL\n");
    exit(EXIT_FAILURE);
  }

  struct udev_monitor *udev_monitor =
      udev_monitor_new_from_netlink(udev, "udev");
  if (!udev_monitor) {
    fprintf(stderr, "udev_monitor_new_from_netlink returned NULL\n");
    exit(EXIT_FAILURE);
  }

  int enable_result = udev_monitor_enable_receiving(udev_monitor);
  if (enable_result < 0) {
    fprintf(stderr,
            "udev_monitor_enable_receiving returned negative error code %i\n",
            enable_result);
    exit(EXIT_FAILURE);
  };

  // udev_monitor_receive_device is non-blocking. To make it blocking,
  // get the monitor file descriptor and unset its O_NONBLOCK flag.
  int udev_monitor_fd = udev_monitor_get_fd(udev_monitor);
  int udev_monitor_fd_flags = fcntl(udev_monitor_fd, F_GETFL);
  fcntl(udev_monitor_fd, F_SETFL, udev_monitor_fd_flags & ~O_NONBLOCK);

  while (1) {
    struct udev_device *device = udev_monitor_receive_device(udev_monitor);

    if (device) {
      const char *device_type = udev_device_get_devtype(device);
      if (!device_type || strcmp(device_type, "usb_device") != 0) {
        udev_device_unref(device);
        continue;
      }

      const char *device_action = udev_device_get_action(device);
      if (!device_action || (strcmp(device_action, "add") != 0 &&
                             strcmp(device_action, "remove") != 0)) {
        udev_device_unref(device);
        continue;
      }

      printf("%s %s vid:%s pid:%s\n", udev_device_get_action(device),
             udev_device_get_devnode(device),
             udev_device_get_property_value(device, "ID_VENDOR_ID"),
             udev_device_get_property_value(device, "ID_MODEL_ID"));

      udev_device_unref(device);
    } else {
      fprintf(stderr, "udev_monitor_receive_device returned NULL\n");
    }
  }

  exit(EXIT_SUCCESS);
}
