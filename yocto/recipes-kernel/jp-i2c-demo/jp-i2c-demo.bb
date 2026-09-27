SUMMARY = "Generic Linux I2C client driver example"
DESCRIPTION = "Out-of-tree I2C client driver example demonstrating Device Tree matching and Yocto module packaging."
LICENSE = "GPL-2.0-only"

inherit module

SRC_URI = " \
    file://jp_i2c_demo.c \
    file://Makefile \
"

S = "${WORKDIR}"

KERNEL_MODULE_AUTOLOAD += "jp_i2c_demo"
