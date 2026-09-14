# NEC LAVIE N15 Keyboard Backlight Driver
Linux support for the keyboard backlight on the NEC LAVIE N15 `PC-N1585EAL-YC`.

## Features
- Handles the keyboard backlight key to change brightness.
- Exposes the keyboard backlight as `/sys/class/leds/nec::kbd_backlight`

```bash
brightnessctl -d 'nec::kbd_backlight' set 1
```

## Installation
```bash
makepkg -si
```

## Building
```bash
make
#load
sudo insmod ./build/nec_kbd_backlight.ko
#unload
sudo rmmod ./build/nec_kbd_backlight.ko
```
