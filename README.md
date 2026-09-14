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
