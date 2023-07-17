#include "config.h"
#include "bindings.h"

UsbkToSunk USBK_TO_SUNK{};

UsbkToSunk::UsbkToSunk() {
  for (const auto &binding : DV_BINDINGS)
    dv[binding.usbkModifier] = binding.sunkMake;

  for (const auto &binding : SEL_BINDINGS)
    sel[binding.usbkSelector] = binding.sunkMake;

  for (const auto &binding : DV_SEL_BINDINGS)
    special[binding.usbkSelector] = binding.sunkMake;
}
