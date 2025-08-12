#include <stdio.h>

#include <windef.h>
#include <winbase.h>
#include <winuser.h>

#include <dinput.h>


WCHAR message[1024];
WCHAR guidstr[39];
FILE* logfile;
IDirectInput8 *dinput;
HWND hwindow;


BOOL CALLBACK enum_devices_callback(const DIDEVICEINSTANCE *instance, void *context) {
  HRESULT hr;
  DWORD yesno;
  IDirectInputDevice8 *device;
  DIPROPDWORD autocenter = {
    .diph = {
      .dwSize = sizeof(DIPROPDWORD),
      .dwHeaderSize = sizeof(DIPROPHEADER),
      .dwHow = DIPH_DEVICE,
    },
  };

  // Log device
  StringFromGUID2(&instance->guidProduct, guidstr, sizeof(guidstr)/sizeof(guidstr[0]));
  swprintf_s(message, sizeof(message)/sizeof(message[0]),
             L"FFDEFAULT: Product GUID: %s\n"
             L"FFDEFAULT: Product name: %s\n"
             L"FFDEFAULT: Device type (sub/primary): 0x%02x/0x%02x\n"
             L"FFDEFAULT: HID usage (page/usage): 0x%02x/0x%02x\n",
             guidstr,
             instance->tszProductName,
             GET_DIDEVICE_SUBTYPE(instance->dwDevType), GET_DIDEVICE_TYPE(instance->dwDevType),
             instance->wUsagePage, instance->wUsage);
  if (logfile) fputws(message, logfile);
  OutputDebugString(message);

  // Required setup stuff
  hr = IDirectInput8_CreateDevice(dinput, &instance->guidInstance, &device, 0);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to create direct input device interface: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_SetDataFormat(device, &c_dfDIJoystick);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to set direct input device data format: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_SetCooperativeLevel(device, hwindow, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to set direct input device cooperative level: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    return DIENUM_CONTINUE;
  }

  // Display default value
  hr = IDirectInputDevice8_GetProperty(device, DIPROP_AUTOCENTER, &autocenter.diph);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to query direct input device property: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  swprintf_s(message, sizeof(message)/sizeof(message[0]),
             L"FFDEFAULT: Autocenter initial value: %s\n",
             autocenter.dwData == DIPROPAUTOCENTER_ON ? L"on" :
             autocenter.dwData == DIPROPAUTOCENTER_OFF ? L"off" :
             L"other");
  if (logfile) fputws(message, logfile);
  OutputDebugString(message);
  swprintf_s(message, sizeof(message)/sizeof(message[0]),
             L"Autocenter initial value: %s\n",
             autocenter.dwData == DIPROPAUTOCENTER_ON ? L"on" :
             autocenter.dwData == DIPROPAUTOCENTER_OFF ? L"off" :
             L"other");
  MessageBox(hwindow, message, instance->tszProductName, MB_OK);

  // Acquire with default value
  hr = IDirectInputDevice8_Acquire(device);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to acquire direct input device: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  swprintf_s(message, sizeof(message)/sizeof(message[0]),
             L"Acquired without setting autocentering.\n\nIs your device autocentering?");
  yesno = MessageBox(hwindow, message, instance->tszProductName, MB_YESNO);
  swprintf_s(message, sizeof(message)/sizeof(message[0]),
             L"FFDEFAULT: Acquired with default: %s\n",
             yesno == IDYES ? L"autocentering" : L"not autocentering");
  if (logfile) fputws(message, logfile);
  OutputDebugString(message);

  hr = IDirectInputDevice8_Unacquire(device);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to unacquire direct input device: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  // Acquire with DIPROPAUTOCENTER_OFF
  autocenter.dwData = DIPROPAUTOCENTER_OFF;
  hr = IDirectInputDevice8_SetProperty(device, DIPROP_AUTOCENTER, &autocenter.diph);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to set direct input device property: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_Acquire(device);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to acquire direct input device: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  swprintf_s(message, sizeof(message)/sizeof(message[0]),
             L"Acquired after setting autocentering off.\n\nIs your device autocentering?");
  yesno = MessageBox(hwindow, message, instance->tszProductName, MB_YESNO);
  swprintf_s(message, sizeof(message)/sizeof(message[0]),
             L"FFDEFAULT: Acquired with off: %s\n", yesno == IDYES ? "autocentering" : "not autocentering");
  if (logfile) fputws(message, logfile);
  OutputDebugString(message);

  hr = IDirectInputDevice8_Unacquire(device);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to unacquire direct input device: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  // Acquire with DIPROPAUTOCENTER_ON
  autocenter.dwData = DIPROPAUTOCENTER_ON;
  hr = IDirectInputDevice8_SetProperty(device, DIPROP_AUTOCENTER, &autocenter.diph);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to set direct input device property: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_Acquire(device);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to acquire direct input device: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  swprintf_s(message, sizeof(message)/sizeof(message[0]),
             L"Acquired after setting autocentering on.\n\nIs your device autocentering?");
  yesno = MessageBox(hwindow, message, instance->tszProductName, MB_YESNO);
  swprintf_s(message, sizeof(message)/sizeof(message[0]),
             L"FFDEFAULT: Acquired with on: %s\n", yesno == IDYES ? "autocentering" : "not autocentering");
  if (logfile) fputws(message, logfile);
  OutputDebugString(message);

  hr = IDirectInputDevice8_Unacquire(device);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to unacquire direct input device: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  // Release and done
  IDirectInputDevice_Release(device);
  return DIENUM_CONTINUE;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow) {
  HRESULT hr;

  // Create window so we have a window handle for dinput
  hwindow = CreateWindow(L"STATIC", L"FF Default", WS_OVERLAPPED | WS_CAPTION,
                         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, \
                         0, 0, hInstance, 0);
  if (!hwindow) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to create a window: error %#lu\n", GetLastError());
    OutputDebugString(message);
    return 1;
  }

  // Open and note log file
  logfile = _wfopen(L"ffdefault.log", L"a");
  fputws(L"FFDEFAULT: ==========\n", logfile);
  if (logfile) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Please sumbit log file: %s\n", L"ffdefault.log");
    MessageBox(hwindow, message, L"Send log file", MB_OK);
  }

  // Query and log system characteristics
  if (MessageBox(hwindow, L"Are you running under Windows?\n", L"Identify your OS", MB_YESNO) == IDYES) {
    fputws(L"FFDEFAULT: Windows\n", logfile);
  }
  else {
    if (MessageBox(hwindow, L"Is your wine 10.2 or older (if unsure, say no)?\n", L"Identify Wine version", MB_YESNO) == IDYES) {
      fputws(L"FFDEFAULT: Wine version: <= 10.2\n", logfile);
    }
    else if (MessageBox(hwindow, L"Is your wine 10.3 or newer (if unsure, say no)?\n", L"Identify Wine version", MB_YESNO) == IDYES) {
      fputws(L"FFDEFAULT: Wine version: >= 10.3\n", logfile);
    }
    else {
      fputws(L"FFDEFAULT: Wine version: unsure\n", logfile);
    }

    if (MessageBox(hwindow, L"Are you using the SDL backend under wine (if unsure, say no)?\n", L"Identify Wine backend", MB_YESNO) == IDYES) {
      fputws(L"FFDEFAULT: Wine backend: SDL\n", logfile);
    }
    else if (MessageBox(hwindow, L"Are you using the udev input backend under wine (if unsure, say no)?\n", L"Identify Wine backend", MB_YESNO) == IDYES) {
      fputws(L"FFDEFAULT: Wine backend: udev input\n", logfile);
    }
    else if (MessageBox(hwindow, L"Are you using the udev hidraw backend under wine (if unsure, say no)?\n", L"Identify Wine backend", MB_YESNO) == IDYES) {
      fputws(L"FFDEFAULT: Wine backend: udev hidraw\n", logfile);
    }
    else {
      fputws(L"FFDEFAULT: Wine backend: unsure\n", logfile);
    }
  }

  // Acquire direct input interface
  hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, &IID_IDirectInput8, (void**)&dinput, 0);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to create direct input interface: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    return 1;
  }

  // Enumerate FF devices
  hr = IDirectInput8_EnumDevices(dinput, DI8DEVCLASS_ALL, enum_devices_callback, 0, DIEDFL_FORCEFEEDBACK);
  if (FAILED(hr)) {
    swprintf_s(message, sizeof(message)/sizeof(message[0]),
               L"Unable to enumerate direct input force feedback devices: hr %#lx\n", hr);
    OutputDebugString(message);
    MessageBox(hwindow, message, L"Error", MB_OK);
    return 1;
  }

  // Cleanup
  IDirectInput_Release(dinput);

  if (logfile)
    fclose(logfile);

  return 0;
}
