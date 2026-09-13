# NEC LAVIE N15 Keyboard Backlight Driver
Linux support for the keyboard backlight on the NEC LAVIE N15 `PC-N1585EAL-YC`.

## Features
- Exposes the keyboard backlight as `/sys/class/leds/nec::kbd_backlight`
- Handles the keyboard backlight key to change brightness.

## Building
```bash
make
#load
sudo insmod ./build/nec_kbd_backlight.ko
#unload
sudo insmod ./build/nec_kbd_backlight.ko
```
