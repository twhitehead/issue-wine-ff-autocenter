#include <stdio.h>

#include <windef.h>
#include <winbase.h>
#include <winuser.h>

#include <dinput.h>


IDirectInput8 *dinput;
HWND hwindow;


BOOL CALLBACK enum_devices_callback(const DIDEVICEINSTANCEA *instance, void *context) {
  HRESULT hr;
  IDirectInputDevice8A *device;
  DIPROPDWORD autocenter = {
    .diph = {
      .dwSize = sizeof(DIPROPDWORD),
      .dwHeaderSize = sizeof(DIPROPHEADER),
      .dwHow = DIPH_DEVICE,
    },
  };

  printf("%s\n", instance->tszProductName);

  // Required setup stuff
  hr = IDirectInput8_CreateDevice(dinput, &instance->guidInstance, &device, 0);
  if (FAILED(hr)) {
    printf("  Unable to create direct input device interface: hr %#lx\n", hr);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_SetDataFormat(device, &c_dfDIJoystick);
  if (FAILED(hr)) {
    printf("  Unable to set direct input device data format: hr %#lx\n", hr);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_SetCooperativeLevel(device, hwindow, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
  if (FAILED(hr)) {
    printf("  Unable to set direct input device cooperative level: hr %#lx\n", hr);
    return DIENUM_CONTINUE;
  }

  // Aquire with default value
  hr = IDirectInputDevice8_GetProperty(device, DIPROP_AUTOCENTER, &autocenter.diph);
  if (FAILED(hr)) {
    printf("  Unable to query direct input device property: hr %#lx\n", hr);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }
  printf("  DIPROP_AUTOCENTER initial value: %s\n",
         autocenter.dwData == DIPROPAUTOCENTER_ON ? "DIPROPAUTOCENTER_ON" :
         autocenter.dwData == DIPROPAUTOCENTER_OFF ? "DIPROPAUTOCENTER_OFF" :
         "unknown value");

  hr = IDirectInputDevice8_Acquire(device);
  if (FAILED(hr)) {
    printf("  Unable to aquire direct input device: hr %#lx\n", hr);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  printf("  Aquired without first setting DIPROP_AUTOCENTER (you have 10s to play with the device)...\n");
  Sleep(10000);
  
  hr = IDirectInputDevice8_Unacquire(device);
  if (FAILED(hr)) {
    printf("  Unable to unaquire direct input device: hr %#lx\n", hr);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  // Aquire with DIPROPAUTOCENTER_OFF
  autocenter.dwData = DIPROPAUTOCENTER_OFF;
  hr = IDirectInputDevice8_SetProperty(device, DIPROP_AUTOCENTER, &autocenter.diph);
  if (FAILED(hr)) {
    printf("  Unable to set direct input device property: hr %#lx\n", hr);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_Acquire(device);
  if (FAILED(hr)) {
    printf("  Unable to aquire direct input device: hr %#lx\n", hr);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  printf("  Aquired after setting DIPROP_AUTOCENTER to DIPROPAUTOCENTER_OFF (you have 10s to play with the device)...\n");
  Sleep(10000);
  
  hr = IDirectInputDevice8_Unacquire(device);
  if (FAILED(hr)) {
    printf("  Unable to unaquire direct input device: hr %#lx\n", hr);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }
    
  // Aquire with DIPROPAUTOCENTER_ON
  autocenter.dwData = DIPROPAUTOCENTER_ON;
  hr = IDirectInputDevice8_SetProperty(device, DIPROP_AUTOCENTER, &autocenter.diph);
  if (FAILED(hr)) {
    printf("  Unable to set direct input device property: hr %#lx\n", hr);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  hr = IDirectInputDevice8_Acquire(device);
  if (FAILED(hr)) {
    printf("  Unable to aquire direct input device: hr %#lx\n", hr);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }

  printf("  Aquired after setting DIPROP_AUTOCENTER to DIPROPAUTOCENTER_ON (you have 10s to play with the device)...\n");
  Sleep(10000);
  
  hr = IDirectInputDevice8_Unacquire(device);
  if (FAILED(hr)) {
    printf("  Unable to unaquire direct input device: hr %#lx\n", hr);
    IDirectInputDevice_Release(device);
    return DIENUM_CONTINUE;
  }
    
  // Release and done
  IDirectInputDevice_Release(device);
  return DIENUM_CONTINUE;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {
  HRESULT hr;

  hwindow = CreateWindow("STATIC", TEXT("FF Default"), WS_OVERLAPPED | WS_CAPTION,
                         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, \
                         0, 0, hInstance, 0);
  if (!hwindow) {
    printf("Unable to create a window: error %#lu\n", GetLastError());
    return 1;
  }
  
  hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, &IID_IDirectInput8, (void**)&dinput, 0);
  if (FAILED(hr)) {
    printf("Unable to create direct input interface: hr %#lx\n", hr);
    return 1;
  }

  hr = IDirectInput8_EnumDevices(dinput, DI8DEVCLASS_ALL, enum_devices_callback, 0, DIEDFL_FORCEFEEDBACK);
  if (FAILED(hr)) {
    printf("Unable to enumerate direct input force feedback devices: hr %#lx\n", hr);
    return 1;
  }

  IDirectInput_Release(dinput);
  return 0;
}
