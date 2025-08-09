# Summary

This is a test program to test the operation of auto centering of force feedback devices under Windows and
Wine. A pre-compiled version is available in the `bin` folder.

The program will query you about your system and then cycle throuh your force feedback devices popping up
dialogues titled by the device that

* report if autocenter is intial set on or not (i.e., the initial value of `DIPROP_AUTOCENTER`),
* ask if autocenter is on after acquiring the device without setting autocentering (i.e. `DIPROP_AUTOCENTER` left at initial value),
* ask if autocenter is on after acquiring the device after setting autocentering off (i.e., `DIPROP_AUTOCENTER` set to `DIPROPAUTOCENTER_OFF`), and
* ask if autocenter is on after acquiring the device after setting autocentering on (i.e., `DIPROP_AUTOCENTER` set to `DIPROPAUTOCENTER_ON`).

A `ffdefault.log` file will be created in the directory the program is ran with the results.

Please run this under Windows and Wine and send me the `ffdefault.log` files (open a pull request to add it, open
an issue to report it, or just email it to my twhitehead gmail email address) along with any other details you feel
may be pertinent.

# Correct operation

A device is working 100% correctly with respect to `DIPROP_AUTOCENTER` will

* have the same intial autocentering setting under Windows and Wine,
* autocenter or not for the first test according to the initial value,
* not autocenter for the second test as it was explicitly turned off, and
* autocenter for the third test as it was explicitly turned on.

Agreement on the initial value is important as some programs may assume the Windows initial value instead of
checking and setting it. These programs will be broken if Wine has a different initial value.

I expect some devices will be broken with respect to the initial value as the initial value for Wine is on for all
devices (as of 2025-08-05) because that is what it was for the one device I had to test when I implemented
it. Reporting your device will allow me to set the correct initial value for your device too.

# Wine backend

Wine can get device input from three sources in Linux: SDL, udev input, and udev HIDRAW. To fully test your
device you need to control which of these are enabled.

## SDL

To force SDL you need to edit your _system.reg_ file in your _~/.wine_ prefix and set the following keys
```
[System\\ControlSet001\\Services\\winebus]
"DisableHidraw"=dword:00000001
"DisableInput"=dword:00000001
"Enable SDL"=dword:00000001
```

If you set `WINEDEBUG=+hid` before running the program you should see some output like the following
that indicates your device is being accessed via SDL (note the `sdl_add_device` and `is_hidraw 0')
```
00bc:trace:hid:sdl_add_device Making up serial number for Microsoft SideWinder Force Feedback 2 Joystick: 030043cf5e0400001b00000000010000.1
...
00bc:trace:hid:bus_main_thread creating non-hidraw device 045e:001b with usages 0001:0004
00bc:trace:hid:bus_create_hid_device desc {vid 045e, pid 001b, version 0100, input -1, uid 00000000, is_gamepad 0, is_hidraw 0, is_bluetooth 0}, unix_device 0x7e5bc630
```

## udev input

To force udev input you need to edit your _system.reg_ file in your _~/.wine_ prefix and set the following keys
```
[System\\ControlSet001\\Services\\winebus]
"DisableHidraw"=dword:00000001
"DisableInput"=dword:00000000
"Enable SDL"=dword:00000000
```
Wine also need read/write access to the _/dev/input/event*_ file for your device. You can tell which is the correct
device file by running
```
ls -l /sys/class/input
```
and looking for your device's VID:PID. You can also find it by look through the udev data by running
```
udevadm info --query=all /dev/input/event*
```

If you set `WINEDEBUG=+hid` before running the program you should see some output like the following that indicates
your device is being accessed via udev input (note the `udev_add_device`, that it is _/dev/input/event*_, and
`is_hidraw 0')
```
00c0:trace:hid:udev_add_device udev "/dev/input/event30" syspath /sys/devices/pci0000:00/0000:00:14.0/usb3/3-11/3-11:1.0/0003:045E:001B.02B0/input/input1990/event30
...
00c0:trace:hid:bus_main_thread creating non-hidraw device 045e:001b with usages 0001:0005
00c0:trace:hid:bus_create_hid_device desc {vid 045e, pid 001b, version 0100, input 0, uid 00000000, is_gamepad 0, is_hidraw 0, is_bluetooth 0}, unix_device 0x7e50bcb0
```

## Udev HIDRAW

To force udev HIDRAW you need to edit your _system.reg_ file in your _~/.wine_ prefix and set the following keys
```
[System\\ControlSet001\\Services\\winebus]
"DisableHidraw"=dword:00000000
"DisableInput"=dword:00000001
"Enable SDL"=dword:00000000
```
Wine also need read/write access to the _/dev/hidraw*_ file for your device. You can tell which is the correct
device file by running
```
ls -l /sys/class/hidraw
```
and looking for your device's VID:PID. You can also find it by look through the udev data by running
```
udevadm info --query=all /dev/input/event*
```

If you set `WINEDEBUG=+hid` before running the program you should see a line like

If you set `WINEDEBUG=+hid` before running the program you should see some output like the following that indicates
your device is being accessed via udev HIDRAW (note the `udev_add_device`, that it is _/dev/hidraw/*_, and
`is_hidraw 1')
```
00bc:trace:hid:udev_add_device udev "/dev/hidraw13" syspath /sys/devices/pci0000:00/0000:00:14.0/usb3/3-11/3-11:1.0/0003:045E:001B.02B1/hidraw/hidraw13
...
00bc:trace:hid:bus_main_thread creating hidraw device 045e:001b with usages 0001:0004
00bc:trace:hid:bus_create_hid_device desc {vid 045e, pid 001b, version 0a00, input 0, uid 00000000, is_gamepad 0, is_hidraw 1, is_bluetooth 0}, unix_device 0x7e503200
```

# Upstream issue

I hacked together this program due to [merge request
8605](https://gitlab.winehq.org/wine/wine/-/merge_requests/8605). This merge request (which was merged) removed
autocenter support from the udev input and SDL backends. I believe this was done to force autocenter off on the
merge request's author's device. His arguement was that it was irreparably broken.

I believe it was just a wrong default value for his device and would like to restore autocenter support from the
udev input and SDL backends. As I only have access to one force feedback device to test though I cannot verify
what the states of all the devices out there are.

That is where you come in. By testing your device and reporting back, I can learn what the actual states of
all the devices are (is it irreparibly broken or not) and hopefully restore autocenter support to the wine
with whatever fixes are required to make it work for everyone's devices.

Thanks in advance for helping!

# Devices tested

This is a summary of devices tested so far. Run the program under Windows and Wine (ideally all backends). Send me
the `ffdefault.log` files (pull request, issue, email to my twhitehead gmail email address) along with any other
details you feel may be pertinent and I'll add your device.

## Wine 10.2 or older (before [4911](https://gitlab.winehq.org/wine/wine/-/merge_requests/8605))

| Device | PID:VID | Backend | Windows Initial | Wine Initial | Test Initial | Test OFF | Test ON | Overall |
| -- | -- | -- | -- | -- | -- | -- | -- | -- |
| MS SideWinder FF 2 Joystick | 045e:001b | SDL | ON | ON | ON | OFF | ON | PASS |
| MS SideWinder FF 2 Joystick | 045e:001b | udev input | ON | ON | ON | OFF | ON | PASS |
| MS SideWinder FF 2 Joystick | 045e:001b | udev hidraw | ON | ON | ON | OFF | ON | PASS |

## Wine 10.3 or newer (after [4911](https://gitlab.winehq.org/wine/wine/-/merge_requests/8605))

| Device | PID:VID | Backend | Windows Initial | Wine Initial | Test Initial | Test OFF | Test ON | Overall |
| -- | -- | -- | -- | -- | -- | -- | -- | -- |
| MS SideWinder FF 2 Joystick | 045e:001b | SDL | ON | ON | ON | ON | ON | FAIL |
| MS SideWinder FF 2 Joystick | 045e:001b | udev input | ON | ON | ON | ON | ON | FAIL |
| MS SideWinder FF 2 Joystick | 045e:001b | udev hidraw | ON | ON | ON | OFF | ON | PASS |
