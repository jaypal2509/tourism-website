# Linux I2C Client Driver + Yocto Integration

A portfolio project demonstrating Linux kernel I2C client-driver structure, Device Tree integration, and Yocto external-kernel-module packaging.

## What this project demonstrates

- Linux I2C client-driver probe/remove lifecycle
- Device Tree `compatible` matching and I2C address configuration
- SMBus byte-data register read/write helpers
- Sysfs attributes for controlled register access
- Out-of-tree kernel module build flow
- Yocto `module` class packaging and autoload configuration

## Project layout

```text
.
├── driver/
│   ├── jp_i2c_demo.c
│   └── Makefile
├── dts/
│   └── jp-i2c-demo.dts
└── yocto/
    └── recipes-kernel/
        └── jp-i2c-demo/
            ├── jp-i2c-demo.bb
            └── files/
                ├── jp_i2c_demo.c
                └── Makefile
```

## Build on a Linux target

```bash
make KDIR=/lib/modules/$(uname -r)/build
sudo insmod jp_i2c_demo.ko
```

The driver expects a device matching `jaypal,i2c-demo`. The sample Device Tree node uses I2C address `0x2a`.

## Device Tree

See `dts/jp-i2c-demo.dts` for a minimal example. Adapt the I2C controller node, address, and compatible string to the actual target hardware.

## Sysfs interface

Once the driver is bound, the sample exposes:

- `info` - reports the bound I2C adapter/address
- `reg_read` - reads a register using SMBus byte-data access
- `reg_write` - writes a register/value pair

Example:

```bash
cat /sys/bus/i2c/devices/1-002a/info
cat /sys/bus/i2c/devices/1-002a/reg_read
printf '0x10 0x55' | sudo tee /sys/bus/i2c/devices/1-002a/reg_write
```

## Yocto integration

The recipe under `yocto/recipes-kernel/jp-i2c-demo/` uses Yocto's `module` class to build and package the external kernel module. Add the recipe directory to a custom layer and include `jp-i2c-demo` in the image.

```bitbake
IMAGE_INSTALL:append = " jp-i2c-demo"
```

## Notes

This is a portfolio/reference implementation. The register map and hardware behavior are intentionally generic; hardware validation requires an I2C device whose register interface matches the sample access pattern.
