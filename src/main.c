#include <fcntl.h>
#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
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

      printf("Received an event from a USB device:\n");
      printf("   Node: %s\n", udev_device_get_devnode(device));
      printf("   Action: %s\n", udev_device_get_action(device));

      udev_device_unref(device);
    } else {
      fprintf(stderr, "udev_monitor_receive_device returned NULL\n");
    }
  }

  exit(EXIT_SUCCESS);
}
