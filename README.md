# PyraOS
### A new OS designed to be dependent on the Pyra coding language.

* Grub Configuration: iso/boot/grub/grub.cfg

* Linker Configuration: os/linker.ld

* ASM Boot File: os/boot.asm

* Kernal Entrypoint (source): src/main.pyra

* Generated Kernal Entrypoint: build/pyra_kernel.generated.cpp

* Final ISO: build/pyraos.iso

 Build Scripts:
 
  * PowerShell: build-iso.ps1

  * Bash: build-iso ( Recommended )

---------------------------------------------------------------------------------------------------------------------------------

Note: To install the required packages on Debian/Ubuntu Based systems, use these commands:
* sudo apt update
* sudo apt install nasm xorriso grub-pc-bin qemu-system-x86 build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
