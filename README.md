# NEC LAVIE N15 Keyboard Backlight Driver
Linux support for the keyboard backlight on the NEC LAVIE N15 `PC-N1585EAL-YC`.

## Features
- Handles the keyboard backlight key to change brightness.
- Exposes the keyboard backlight as `/sys/class/leds/nec::kbd_backlight`

```bash
brightnessctl -d 'nec::kbd_backlight' set 1
```

## Installation
No need to clone this entire repository, but to get only `PKGBUILD`.

Better to build in `/tmp`, as `makepkg` may create many files in the current directory.

```bash
cd /tmp 
curl -o https://raw.githubusercontent.com/slow-cat/nec_backlight/refs/heads/master/PKGBUILD
makepkg -si
```

<!-- ## Building -->
<!-- ```bash -->
<!-- make -->
<!-- #load -->
<!-- sudo insmod ./build/nec_kbd_backlight.ko -->
<!-- #unload -->
<!-- sudo rmmod ./build/nec_kbd_backlight.ko -->
<!-- ``` -->

## Editor setup (clangd)

With the headers for the target kernel installed, run:

```bash
make clangd
```

This builds the module and regenerates `compile_commands.json` from the actual
build commands. Run it again after a kernel update, then restart clangd in your
editor. To target another kernel, use `make clangd KDIR=/path/to/kernel/build`.
