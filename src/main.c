#include <fcntl.h>
#include <libudev.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
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

  regex_t REGEX_BUSID;
  int regex_busid_comp_error_code =
      regcomp(&REGEX_BUSID, "^busid=([0-9.-]+)#.*", REG_EXTENDED);
  if (regex_busid_comp_error_code) {
    char regex_busid_error[100];
    regerror(regex_busid_comp_error_code, &REGEX_BUSID, regex_busid_error,
             sizeof(regex_busid_error));
    fprintf(stderr, "regcomp for REGEX_BUSID returned error: %s\n",
            regex_busid_error);
    exit(EXIT_FAILURE);
  }
  int REGEX_BUSID_MAX_GROUPS = 2;

  while (1) {
    struct udev_device *device = udev_monitor_receive_device(udev_monitor);

    if (!device) {
      fprintf(stderr, "udev_monitor_receive_device returned NULL\n");
      exit(EXIT_FAILURE);
    }

    const char *device_type = udev_device_get_devtype(device);
    if (!device_type || strcmp(device_type, "usb_device") != 0) {
      udev_device_unref(device);
      continue;
    }

    const char *device_action = udev_device_get_action(device);
    if (!device_action || strcmp(device_action, "add") != 0) {
      udev_device_unref(device);
      continue;
    }

    const char *device_v = udev_device_get_property_value(device, "ID_VENDOR");
    const char *device_vid =
        udev_device_get_property_value(device, "ID_VENDOR_ID");
    const char *device_p = udev_device_get_property_value(device, "ID_MODEL");
    const char *device_pid =
        udev_device_get_property_value(device, "ID_MODEL_ID");

    if (device_vid == NULL || device_pid == NULL)
      continue;

    char device_vid_pid[10];
    sprintf(device_vid_pid, "%s:%s", device_vid, device_pid);

    printf("%s vid:pid %s %s %s\n", device_action, device_vid_pid, device_v,
           device_p);

    FILE *usbip_list_stdout = popen("usbip list --parsable --local", "r");

    char usbip_list_line[80];
    while (!feof(usbip_list_stdout)) {
      fgets(usbip_list_line, sizeof(usbip_list_line), usbip_list_stdout);
      if (strstr(usbip_list_line, device_vid_pid)) {
        regmatch_t matches[REGEX_BUSID_MAX_GROUPS];

        if (regexec(&REGEX_BUSID, usbip_list_line, REGEX_BUSID_MAX_GROUPS,
                    matches, 0) == 0) {
          regmatch_t match = matches[1]; // Zero is the entire string.

          char busid[20];
          int length = match.rm_eo - match.rm_so;
          memcpy(busid, usbip_list_line + match.rm_so, length);
          busid[length] = 0;

          char usbip_bind_command[50];
          sprintf(usbip_bind_command, "usbip bind --busid=%s", busid);
          popen(usbip_bind_command, "r");
        };
      }
    }

    udev_device_unref(device);
  }

  exit(EXIT_SUCCESS);
}
