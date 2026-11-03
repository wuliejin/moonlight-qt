#pragma once
#include <cstdint>

// Resolve the physical USB parent of Windows' generic XInput HID child.
// Failure leaves the caller's SDL-provided identifiers unchanged.
bool nativeGamepadUsbIds(const char* path, std::uint16_t& vendor, std::uint16_t& product);
