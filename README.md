# PyraOS v0.4.1 Hello World OS

This project now boots from the generated freestanding `.pyra` output.

## Current boot path

The bootable kernel build uses:
- `src/main.pyra` as the kernel entry source
- `use pyra "freestanding"` as the special freestanding helper alias
- `os/boot.asm` as the multiboot bootstrap
- `os/linker.ld` as the linker script
- `iso/boot/grub/grub.cfg` as the GRUB boot entry

`build-iso.ps1` generates:
- `build/pyra_kernel.generated.cpp`

and links that generated file directly into the kernel binary.

## Why the freestanding helper exists

`freestanding` is a special compiler-supported helper library for freestanding
kernel work. It is intentionally separate so OS-specific boot and framebuffer
plumbing does not have to live directly in your app file.

Right now it still expands to raw freestanding C++ internally, but your project
entry can stay clean:

```pyra
use pyra "freestanding"
```

That is a bridge toward a more high-level freestanding Pyra story.
