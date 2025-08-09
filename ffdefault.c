#include <stdio.h>

#include <windef.h>
#include <winbase.h>
#include <winuser.h>

#include <dinput.h>


char message[100];
FILE* logfile;
IDirectInput8 *dinput;
HWND hwindow;


BOOL CALLBACK enum_devices_callback(const DIDEVICEINSTANCE *instance, void *context) {
  HRESULT hr;
  int yesno;
  IDirectInputDevice8A *device;
  DIPROPDWORD autocenter = {
    .diph = {
      .dwSize = sizeof(DIPROPDWORD),
      .dwHeaderSize = sizeof(DIPROPHEADER),
      .dwHow = DIPH_DEVICE,
    },
  };

  // Log device
  sprintf_s(message, sizeof(message)/sizeof(message[0]),
            "FFDEFAULT: Product name: %s\n", instance->tszProductName);
  if (logfile) fputs(message, logfile);
  OutputDebugString(message);

  // Required setup stuff
  hr = IDirectInput8_CreateDevice(dinput, &instance->guidInstance, &device, 0);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to create direct input device interface: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_SetDataFormat(device, &c_dfDIJoystick);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to set direct input device data format: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_SetCooperativeLevel(device, hwindow, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to set direct input device cooperative level: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    return DIENUM_CONTINUE;
  }

  // Display default value
  hr = IDirectInputDevice8_GetProperty(device, DIPROP_AUTOCENTER, &autocenter.diph);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to query direct input device property: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  sprintf_s(message, sizeof(message)/sizeof(message[0]),
            "FFDEFAULT: Autocenter initial value: %s\n",
            autocenter.dwData == DIPROPAUTOCENTER_ON ? "on" :
            autocenter.dwData == DIPROPAUTOCENTER_OFF ? "off" :
            "other");
  if (logfile) fputs(message, logfile);
  OutputDebugString(message);
  sprintf_s(message, sizeof(message)/sizeof(message[0]),
            "Autocenter initial value: %s\n",
            autocenter.dwData == DIPROPAUTOCENTER_ON ? "on" :
            autocenter.dwData == DIPROPAUTOCENTER_OFF ? "off" :
            "other");
  MessageBox(hwindow, message, instance->tszProductName, MB_OK);

  // Acquire with default value
  hr = IDirectInputDevice8_Acquire(device);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to acquire direct input device: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  sprintf_s(message, sizeof(message)/sizeof(message[0]),
            "Acquired without setting autocentering.\n\nIs your device autocentering?");
  yesno = MessageBox(hwindow, message, instance->tszProductName, MB_YESNO);
  sprintf_s(message, sizeof(message)/sizeof(message[0]),
            "FFDEFAULT: Acquired with default: %s\n", yesno == IDYES ? "autocentering" : "not autocentering");
  if (logfile) fputs(message, logfile);
  OutputDebugString(message);

  hr = IDirectInputDevice8_Unacquire(device);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to unacquire direct input device: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  // Acquire with DIPROPAUTOCENTER_OFF
  autocenter.dwData = DIPROPAUTOCENTER_OFF;
  hr = IDirectInputDevice8_SetProperty(device, DIPROP_AUTOCENTER, &autocenter.diph);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to set direct input device property: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_Acquire(device);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to acquire direct input device: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  sprintf_s(message, sizeof(message)/sizeof(message[0]),
            "Acquired after setting autocentering off.\n\nIs your device autocentering?");
  yesno = MessageBox(hwindow, message, instance->tszProductName, MB_YESNO);
  sprintf_s(message, sizeof(message)/sizeof(message[0]),
            "FFDEFAULT: Acquired with off: %s\n", yesno == IDYES ? "autocentering" : "not autocentering");
  if (logfile) fputs(message, logfile);
  OutputDebugString(message);

  hr = IDirectInputDevice8_Unacquire(device);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to unacquire direct input device: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  // Acquire with DIPROPAUTOCENTER_ON
  autocenter.dwData = DIPROPAUTOCENTER_ON;
  hr = IDirectInputDevice8_SetProperty(device, DIPROP_AUTOCENTER, &autocenter.diph);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to set direct input device property: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_Acquire(device);
  if (FAILED(hr)) {
    printf("  Unable to acquire direct input device: hr %#lx\n", hr);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  sprintf_s(message, sizeof(message)/sizeof(message[0]),
            "Acquired after setting autocentering on.\n\nIs your device autocentering?");
  yesno = MessageBox(hwindow, message, instance->tszProductName, MB_YESNO);
  sprintf_s(message, sizeof(message)/sizeof(message[0]),
            "FFDEFAULT: Acquired with on: %s\n", yesno == IDYES ? "autocentering" : "not autocentering");
  if (logfile) fputs(message, logfile);
  OutputDebugString(message);

  hr = IDirectInputDevice8_Unacquire(device);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to unacquire direct input device: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  // Release and done
  IDirectInputDevice_Release(device);
  return DIENUM_CONTINUE;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {
  HRESULT hr;

  // Create window so we have a window handle for dinput
  hwindow = CreateWindow("STATIC", TEXT("FF Default"), WS_OVERLAPPED | WS_CAPTION,
                         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, \
                         0, 0, hInstance, 0);
  if (!hwindow) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to create a window: error %#lu\n", GetLastError());
    OutputDebugString(message);
    return 1;
  }

  // Open and note log file
  logfile = fopen("ffdefault.log", "a");
  fputs("FFDEFAULT: ==========\n", logfile);
  if (logfile) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Please sumbit log file: %s\n", "ffdefault.log");
    MessageBox(hwindow, message, "Send log file", MB_OK);
  }

  // Query and log system characteristics
  if (MessageBox(hwindow, "Are you running under Windows?\n", "Identify your OS", MB_YESNO) == IDYES) {
    fputs("FFDEFAULT: Windows\n", logfile);
  }
  else {
    if (MessageBox(hwindow, "Is you wine 10.2 or older (if unsure, say no)?\n", "Identify Wine version", MB_YESNO) == IDYES) {
      fputs("FFDEFAULT: Wine version: <= 10.2\n", logfile);
    }
    else if (MessageBox(hwindow, "Is your wine 10.3 or newer (if unsure, say no)?\n", "Identify Wine version", MB_YESNO) == IDYES) {
      fputs("FFDEFAULT: Wine version: >= 10.3\n", logfile);
    }
    else {
      fputs("FFDEFAULT: Wine version: unsure\n", logfile);
    }

    if (MessageBox(hwindow, "Are you using the SDL backend under wine (if unsure, say no)?\n", "Identify Wine backend", MB_YESNO) == IDYES) {
      fputs("FFDEFAULT: Wine backend: SDL\n", logfile);
    }
    else if (MessageBox(hwindow, "Are you using the udev input backend under wine (if unsure, say no)?\n", "Identify Wine backend", MB_YESNO) == IDYES) {
      fputs("FFDEFAULT: Wine backend: udev input\n", logfile);
    }
    else if (MessageBox(hwindow, "Are you using the udev hidraw backend under wine (if unsure, say no)?\n", "Identify Wine backend", MB_YESNO) == IDYES) {
      fputs("FFDEFAULT: Wine backend: udev hidraw\n", logfile);
    }
    else {
      fputs("FFDEFAULT: Wine backend: unsure\n", logfile);
    }
  }

  // Acquire direct input interface
  hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, &IID_IDirectInput8, (void**)&dinput, 0);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to create direct input interface: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    return 1;
  }

  // Enumerate FF devices
  hr = IDirectInput8_EnumDevices(dinput, DI8DEVCLASS_ALL, enum_devices_callback, 0, DIEDFL_FORCEFEEDBACK);
  if (FAILED(hr)) {
    sprintf_s(message, sizeof(message)/sizeof(message[0]),
              "Unable to enumerate direct input force feedback devices: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, "Error", MB_OK);
    return 1;
  }

  // Cleanup
  IDirectInput_Release(dinput);

  if (logfile)
    fclose(logfile);

  return 0;
}
