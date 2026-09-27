// SPDX-License-Identifier: GPL-2.0
/*
 * Generic Linux I2C client-driver example.
 * Portfolio/reference implementation for kernel + Device Tree learning.
 */

#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/sysfs.h>

#define JP_I2C_REG_ID 0x00

struct jp_i2c_data {
	struct i2c_client *client;
	struct mutex lock;
};

static int jp_i2c_read_reg(struct jp_i2c_data *data, u8 reg, u8 *value)
{
	int ret;

	mutex_lock(&data->lock);
	ret = i2c_smbus_read_byte_data(data->client, reg);
	mutex_unlock(&data->lock);

	if (ret < 0)
		return ret;

	*value = (u8)ret;
	return 0;
}

static int jp_i2c_write_reg(struct jp_i2c_data *data, u8 reg, u8 value)
{
	int ret;

	mutex_lock(&data->lock);
	ret = i2c_smbus_write_byte_data(data->client, reg, value);
	mutex_unlock(&data->lock);

	return ret;
}

static ssize_t info_show(struct device *dev,
			 struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);

	return sysfs_emit(buf, "driver=jp_i2c_demo adapter=%s address=0x%02x\n",
			  dev_name(&client->adapter->dev), client->addr);
}

static ssize_t reg_read_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	struct jp_i2c_data *data = i2c_get_clientdata(to_i2c_client(dev));
	u8 value;
	int ret;

	ret = jp_i2c_read_reg(data, JP_I2C_REG_ID, &value);
	if (ret < 0)
		return ret;

	return sysfs_emit(buf, "0x%02x\n", value);
}

static ssize_t reg_write_store(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct jp_i2c_data *data = i2c_get_clientdata(to_i2c_client(dev));
	unsigned int reg, value;
	int ret;

	ret = sscanf(buf, "%x %x", &reg, &value);
	if (ret != 2 || reg > 0xff || value > 0xff)
		return -EINVAL;

	ret = jp_i2c_write_reg(data, (u8)reg, (u8)value);
	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR_RO(info);
static DEVICE_ATTR_RO(reg_read);
static DEVICE_ATTR_WO(reg_write);

static struct attribute *jp_i2c_attrs[] = {
	&dev_attr_info.attr,
	&dev_attr_reg_read.attr,
	&dev_attr_reg_write.attr,
	NULL,
};

static const struct attribute_group jp_i2c_attr_group = {
	.attrs = jp_i2c_attrs,
};

static int jp_i2c_demo_probe(struct i2c_client *client)
{
	struct jp_i2c_data *data;
	int ret;

	data = devm_kzalloc(&client->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->client = client;
	mutex_init(&data->lock);
	i2c_set_clientdata(client, data);

	ret = devm_device_add_group(&client->dev, &jp_i2c_attr_group);
	if (ret)
		return ret;

	dev_info(&client->dev, "jp_i2c_demo bound at 0x%02x\n", client->addr);
	return 0;
}

static const struct of_device_id jp_i2c_demo_of_match[] = {
	{ .compatible = "jaypal,i2c-demo" },
	{ }
};
MODULE_DEVICE_TABLE(of, jp_i2c_demo_of_match);

static const struct i2c_device_id jp_i2c_demo_id[] = {
	{ "jp_i2c_demo", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, jp_i2c_demo_id);

static struct i2c_driver jp_i2c_demo_driver = {
	.driver = {
		.name = "jp_i2c_demo",
		.of_match_table = jp_i2c_demo_of_match,
	},
	.probe = jp_i2c_demo_probe,
	.id_table = jp_i2c_demo_id,
};

module_i2c_driver(jp_i2c_demo_driver);

MODULE_AUTHOR("Jaypal Sodhaparmar");
MODULE_DESCRIPTION("Generic Linux I2C client driver example");
MODULE_LICENSE("GPL");
