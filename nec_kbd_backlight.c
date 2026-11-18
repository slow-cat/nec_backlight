// SPDX-License-Identifier: CC0-1.0 OR GPL-2.0-only
#include <linux/acpi.h>
#include <linux/devm-helpers.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#define NEC_NOTIFY_KBD_BACKLIGHT 0x83

#define NEC_KBD_OFF    0
#define NEC_KBD_DIM    1
#define NEC_KBD_BRIGHT 10
#define NEC_KBD_MAX    10

struct nec_kbd {
	struct device *dev;
	acpi_handle hwui;

	struct led_classdev led;
	struct mutex lock;
	struct work_struct toggle_work;
};

static int nec_get_level_nolock(struct nec_kbd *kbd, unsigned int *level)
{
	unsigned long long value;
	acpi_status status;

	status = acpi_evaluate_integer(kbd->hwui, "GKBS", NULL, &value);
	if (ACPI_FAILURE(status))
		return -EIO;

	if (value > NEC_KBD_MAX)
		return -EIO;

	*level = value;
	return 0;
}

static int nec_set_level_nolock(struct nec_kbd *kbd, unsigned int level)
{
	union acpi_object arg = {
		.type = ACPI_TYPE_INTEGER,
	};
	struct acpi_object_list args = {
		.count = 1,
		.pointer = &arg,
	};
	unsigned long long result;
	acpi_status status;

	if (level > NEC_KBD_MAX)
		return -EINVAL;

	arg.integer.value = level;

	status = acpi_evaluate_integer(kbd->hwui, "SKBS", &args, &result);
	if (ACPI_FAILURE(status))
		return -EIO;

	/* DSDTのSKBSは常にZeroを返す */
	if (result != 0)
		return -EIO;

	return 0;
}

static enum led_brightness nec_led_get(struct led_classdev *led)
{
	struct nec_kbd *kbd = container_of(led, struct nec_kbd, led);
	unsigned int level;
	int ret;

	mutex_lock(&kbd->lock);
	ret = nec_get_level_nolock(kbd, &level);
	mutex_unlock(&kbd->lock);

	if (ret) {
		dev_warn_ratelimited(kbd->dev,
				     "GKBS failed: %d\n", ret);
		return led->brightness;
	}

	return level;
}

static int nec_led_set(struct led_classdev *led,
		       enum led_brightness brightness)
{
	struct nec_kbd *kbd = container_of(led, struct nec_kbd, led);
	int ret;

	mutex_lock(&kbd->lock);
	ret = nec_set_level_nolock(kbd, brightness);
	mutex_unlock(&kbd->lock);

	return ret;
}

static void nec_toggle_work(struct work_struct *work)
{
	struct nec_kbd *kbd =
		container_of(work, struct nec_kbd, toggle_work);
	unsigned int level, next;
	int ret;

	mutex_lock(&kbd->lock);

	ret = nec_get_level_nolock(kbd, &level);
	if (ret)
		goto out;

	/* 0 -> 1 -> 10 と明るさを変更する*/
	switch (level) {
	case NEC_KBD_OFF:
		next = NEC_KBD_DIM;
		break;
	case NEC_KBD_DIM:
		next = NEC_KBD_BRIGHT;
		break;
	default:
		next = NEC_KBD_OFF;
		break;
	}

	ret = nec_set_level_nolock(kbd, next);
	if (!ret)
		WRITE_ONCE(kbd->led.brightness, next);

out:
	mutex_unlock(&kbd->lock);

	if (ret)
		dev_warn(kbd->dev,
			 "keyboard backlight toggle failed: %d\n", ret);
}

static void nec_notify(acpi_handle handle, u32 event, void *context)
{
	struct nec_kbd *kbd = context;

	if (event == NEC_NOTIFY_KBD_BACKLIGHT) {
		schedule_work(&kbd->toggle_work);
		return;
	}

	dev_dbg(kbd->dev, "unknown notify 0x%x\n", event);
}

static int nec_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct nec_kbd *kbd;
	unsigned int level = 0;
	acpi_status status;
	int ret;

	kbd = devm_kzalloc(dev, sizeof(*kbd), GFP_KERNEL);
	if (!kbd)
		return -ENOMEM;

	kbd->dev = dev;
	mutex_init(&kbd->lock);

	/*
	 * DSDT:
	 *   \_SB.HWUI = NCT0031
	 *       GKBS()
	 *       SKBS(value)
	 */
	status = acpi_get_handle(NULL, "\\_SB.HWUI", &kbd->hwui);
	if (ACPI_FAILURE(status)) {
		dev_err(dev, "cannot find \\_SB.HWUI\n");
		return -ENODEV;
	}

	kbd->led.name = "nec::kbd_backlight";
	kbd->led.max_brightness = NEC_KBD_MAX;
	kbd->led.brightness_get = nec_led_get;
	kbd->led.brightness_set_blocking = nec_led_set;

	if (!nec_get_level_nolock(kbd, &level))
		kbd->led.brightness = level;

	ret = devm_led_classdev_register(dev, &kbd->led);
	if (ret)
		return ret;

	/* On detach, remove the notifier before cancelling its work. */
	ret = devm_work_autocancel(dev, &kbd->toggle_work, nec_toggle_work);
	if (ret)
		return ret;

	ret = devm_acpi_install_notify_handler(dev, ACPI_DEVICE_NOTIFY,
					     nec_notify, kbd);
	if (ret)
		return ret;

	dev_info(dev,
		 "NEC keyboard backlight registered, brightness=%u\n",
		 level);

	return 0;
}

static const struct acpi_device_id nec_ids[] = {
	{ "NCT0030", 0 },
	{ }
};
MODULE_DEVICE_TABLE(acpi, nec_ids);

static struct platform_driver nec_driver = {
	.probe = nec_probe,
	.driver = {
		.name = "nec-kbd-backlight",
		.acpi_match_table = nec_ids,
	},
};

module_platform_driver(nec_driver);

MODULE_AUTHOR("moamoa");
MODULE_DESCRIPTION("NEC LAVIE N15 keyboard backlight driver");
MODULE_LICENSE("GPL");
