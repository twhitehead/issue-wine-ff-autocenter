# Summary

This is a test program to verify correct operation of auto centering of force feedback devices under Windows and
Wine. A pre-compiled version is available in the `bin` folder.

The program will cycle through all you force feedback devices popping up dialogues titled by the device
that is being tested. The dialogues purposes are, in order,

* report the initial value of `DIPROP_AUTOCENTER`,
* pause after acquiring the device without setting `DIPROP_AUTOCENTER` so you can test your device,
* pause after acquiring the device with `DIPROP_AUTOCENTER` to `DIPROPAUTOCENTER_OFF` so you can test your device,
  and
* pause after acquiring the device with `DIPROP_AUTOCENTER` to `DIPROPAUTOCENTER_ON` so you can test your device.

# Upstream breakage

I hacked together this program as I believe [merge request
8605](https://gitlab.winehq.org/wine/wine/-/merge_requests/8605) (which was merged) broke `DIPROP_AUTOCENTER`
support in almost all cases. Others disagree. The problem is none of us have a bunch of devices to test. With this
program people can test their devices and report back so we know what is the case

My expectation is that, for devices that udev HIDRAW doesn't work (e.g., so udev input or SDL are the only option)
* all non-HID driver devices for which the kernel driver supported auto center (e.g., the I-FORCE) are now broken
  with respect to `DIPROP_AUTOCENTER` for all input methods

with respect to devices where udev HIDRAW wouldn't work with auto center, but udev input or SDL would (e.g., devices
that that don't turn auto center on on reset and off on stop all effects)
* all HID non-pidff driver devices for which the kernel driver supports auto center (e.g., the Logitech WingMan Force)
  are now broken with respect to `DIPROP_AUTOCENTER` for all input methods

and with respect to devices where udev HIDRAW does work with auto center (e.g., devices that do turn auto center on
on reset and off on stop all effects
* all HID pdiff driver devices for which the kernel driver supported auto center (e.g., the USB SideWinder 2) are
  broken with respect to `DIPROP_AUTOCENTER` for all input methods but udev HIDRAW

# Correct operation

* when `DIPROP_AUTOCENTER` is not set by the program, the device should exhibit which ever behaviour is the default
  upon being acquired (the default can be obtained by querying the `DIPROP_AUTOCENTER` property),
* when `DIPROP_AUTOCENTER` is set to `DIPROPAUTOCENTER_OFF`, the device should not exhibit auto centering behaviour
  upon being acquired, and
* when `DIPROP_AUTOCENTER` is set to `DIPROPAUTOCENTER_ON`, the device should exhibit auto centering
  behaviour upon being acquired.

The initial value of `DIPROP_AUTOCENTER` should also agree between Windows and Wine. This is required as some
programs may assume the Windows initial value instead of checking and setting it. These programs will be broken
if Wine has a different initial value.

# Verifying your device

Wine has three different ways of getting input. For each of these (see below for how to choose which one)
run the program and

* note the default shown in the first dialogue box,
* verify auto center is acting accordingly when the second dialogue box is up,
* verify auto center is off when the third dialogue box is up, and
* verify auto center is on when the fourth dialogue box is up, and

Note that the merge request I believe broke everything was merged on 2026-08-06, so you need to test with a wine
version newer than that in order to determine if it has indeed broken your device (or fixed it).

If you have Windows as well, run the program under Windows and check that the default shown in the first
dialogue box matches what is shown when ran under Wine. Please report any wrong behaviour or discrepancies
between Windows and Wine for the default setting.

# Wine bus

Wine can get device input from three sources in Linux: SDL, udev input, and udev HIDRAW. To fully test your
device you need control which of these are enabled.

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
