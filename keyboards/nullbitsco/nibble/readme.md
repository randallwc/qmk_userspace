# Todo

- [ ] create a bongo cat via firmware
- [ ] create a snail via firmware
- [ ] fix the control button and windows button by using via

# Notes

forked from here

https://github.com/qmk/qmk_firmware/tree/45dc2499dc369ce0ac7a4868a01ffb18fbaf0384/keyboards/nullbitsco/nibble

last via enabled keymaps

https://github.com/qmk/qmk_firmware/tree/8f6d2ac53cdde6d0befc9f86c326f56620dcfcc0/keyboards/nullbitsco/nibble

last user keymaps

https://github.com/qmk/qmk_firmware/tree/e64815511303584787b623c12ac8bab7dccb0034/keyboards/nullbitsco/nibble

my personal keymaps

https://github.com/randallwc/.dotfiles/tree/b6aaa0ceeb9ca44c5c9306896ed24ea2b98a42e3/qmk

# NIBBLE

![NIBBLE](https://nullbits.co/static/img/nibble1.jpg)

A unique, tweakable 65% keyboard kit built by nullbits. [More info at nullbits.co](https://nullbits.co/nibble/)

* Keyboard Maintainer: [Jay Greco](https://github.com/jaygreco)
* Hardware Supported: NIBBLE Rev1, Pro Micro comaptible MCUs.
* Hardware Availability: [nullbits.co](https://nullbits.co/)

Note: If you are seeing issues with MacOS and keyboard hangs after sleep, make sure `NO_USB_STARTUP_CHECK = yes` is set in your rules.mk.

Adds experimental "Remote Keyboard" functionality, which forwards keystrokes from an external macropad, keyboard, or numpad over UART/TRRS, removing the need for an additional USB connection. 

Make example for this keyboard (after setting up your build environment):

    make nullbitsco/nibble:default

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).
