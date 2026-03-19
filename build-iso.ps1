param(
    [string]$CrossPrefix = 'i686-elf-',
    [string]$PyraCli = 'C:\Users\bashy\Documents\GitHub\WsGt\Pyra\__main__.py',
    [string]$PythonExe = 'python'
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $root

$nasm = 'nasm'
$gxx = "${CrossPrefix}g++"
$ld = "${CrossPrefix}ld"
$grub = 'grub-mkrescue'
$qemu = 'qemu-system-i386'

New-Item -ItemType Directory -Force -Path build, 'iso\boot' | Out-Null

& $PythonExe $PyraCli "$root\src\main.pyra" --target cpp --bare --freestanding --entrypoint pyra_kernel_entry -o "$root\build\pyra_kernel.generated.cpp"
Write-Host 'Generated freestanding kernel source at build/pyra_kernel.generated.cpp'

& $nasm -f elf32 os/boot.asm -o build/boot.o
& $gxx -ffreestanding -m32 -fno-exceptions -fno-rtti -c build/pyra_kernel.generated.cpp -o build/pyra_kernel.o
& $ld -T os/linker.ld -o build/pyraos.bin build/boot.o build/pyra_kernel.o
Copy-Item build/pyraos.bin iso/boot/pyraos.bin -Force
& $grub -o build/pyraos.iso iso
Write-Host 'Built build/pyraos.iso'
Write-Host 'To run it:'
Write-Host "  $qemu -cdrom build/pyraos.iso"
