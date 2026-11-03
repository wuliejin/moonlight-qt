#include "nativegamepadidentity.h"
#ifdef _WIN32
#include <windows.h>
#include <cfgmgr32.h>
#include <devpropdef.h>
#include <cwchar>
#include <string>
#endif

bool nativeGamepadUsbIds(const char* path, std::uint16_t& vendor, std::uint16_t& product) {
#ifdef _WIN32
    if (!path) return false;
    const auto count = MultiByteToWideChar(CP_UTF8, 0, path, -1, nullptr, 0);
    if (count <= 0) return false;
    std::wstring devicePath(count, L'\0');
    if (MultiByteToWideChar(CP_UTF8, 0, path, -1, devicePath.data(), count) != count) return false;
    const DEVPROPKEY instanceKey = {{0x78c34fc8, 0x104a, 0x4aca, {0x9e,0xa4,0x52,0x4d,0x52,0x99,0x6e,0x57}}, 256};
    WCHAR id[MAX_DEVICE_ID_LEN] {};
    ULONG bytes = sizeof(id);
    DEVPROPTYPE type = DEVPROP_TYPE_EMPTY;
    if (CM_Get_Device_Interface_PropertyW(devicePath.c_str(), &instanceKey, &type,
            reinterpret_cast<PBYTE>(id), &bytes, 0) != CR_SUCCESS || type != DEVPROP_TYPE_STRING) return false;
    DEVINST node;
    if (CM_Locate_DevNodeW(&node, id, CM_LOCATE_DEVNODE_NORMAL) != CR_SUCCESS) return false;
    for (unsigned depth = 0; depth < 12; ++depth) {
        if (CM_Get_Device_IDW(node, id, MAX_DEVICE_ID_LEN, 0) != CR_SUCCESS) return false;
        unsigned short realVendor, realProduct;
        if (std::swscanf(id, L"USB\\VID_%4hx&PID_%4hx", &realVendor, &realProduct) == 2 &&
            !(realVendor == 0x045e && realProduct == 0x02ff)) {
            vendor = realVendor;
            product = realProduct;
            return true;
        }
        DEVINST parent;
        if (CM_Get_Parent(&parent, node, 0) != CR_SUCCESS) break;
        node = parent;
    }
#endif
    return false;
}
