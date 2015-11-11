/*
 * Driver for keys on GPIO lines capable of generating interrupts.
 *
 * Copyright 2005 Phil Blundell
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/jack_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>

#ifdef CONFIG_JACK_MGR
#include <linux/sec_jack.h>

static int mehmet_jack_key_registered = 0;

extern int mehmet_convert_jack_remote_key(int state);
extern int mehmet_jack_get_type(void);

struct jack_button_data {
	struct jack_keys_button *button;
	struct input_dev *input;
	struct timer_list timer;
	struct work_struct work;
	bool disabled;
};

struct jack_keys_drvdata {
	struct input_dev *input;
	struct mutex disable_lock;
	unsigned int n_buttons;
	struct jack_button_data data[0];
};

/*
 * SYSFS interface for enabling/disabling keys and switches:
 *
 * There are 4 attributes under /sys/devices/platform/gpio-keys/
 *	keys [ro]              - bitmap of keys (EV_KEY) which can be
 *	                         disabled
 *	switches [ro]          - bitmap of switches (EV_SW) which can be
 *	                         disabled
 *	disabled_keys [rw]     - bitmap of keys currently disabled
 *	disabled_switches [rw] - bitmap of switches currently disabled
 *
 * Userland can change these values and hence disable event generation
 * for each key (or switch). Disabling a key means its interrupt line
 * is disabled.
 *
 * For example, if we have following switches set up as gpio-keys:
 *	SW_DOCK = 5
 *	SW_CAMERA_LENS_COVER = 9
 *	SW_KEYPAD_SLIDE = 10
 *	SW_FRONT_PROXIMITY = 11
 * This is read from switches:
 *	11-9,5
 * Next we want to disable proximity (11) and dock (5), we write:
 *	11,5
 * to file disabled_switches. Now proximity and dock IRQs are disabled.
 * This can be verified by reading the file disabled_switches:
 *	11,5
 * If we now want to enable proximity (11) switch we write:
 *	5
 * to disabled_switches.
 *
 * We can disable only those keys which don't allow sharing the irq.
 */

/**
 * get_n_events_by_type() - returns maximum number of events per @type
 * @type: type of button (%EV_KEY, %EV_SW)
 *
 * Return value of this function can be used to allocate bitmap
 * large enough to hold all bits for given type.
 */
static inline int get_n_events_by_type(int type)
{
	BUG_ON(type != EV_SW && type != EV_KEY);

	return (type == EV_KEY) ? KEY_CNT : SW_CNT;
}

/**
 * gpio_keys_disable_button() - disables given GPIO button
 * @bdata: button data for button to be disabled
 *
 * Disables button pointed by @bdata. This is done by masking
 * IRQ line. After this function is called, button won't generate
 * input events anymore. Note that one can only disable buttons
 * that don't share IRQs.
 *
 * Make sure that @bdata->disable_lock is locked when entering
 * this function to avoid races when concurrent threads are
 * disabling buttons at the same time.
 */
static void jack_keys_disable_button(struct jack_button_data *bdata)
{
	if (!bdata->disabled) {
		/*
		 * Disable IRQ and possible debouncing timer.
		 */
		disable_irq(gpio_to_irq(bdata->button->gpio));
		if (bdata->button->debounce_interval)
			del_timer_sync(&bdata->timer);

		bdata->disabled = true;
	}
}

/**
 * gpio_keys_enable_button() - enables given GPIO button
 * @bdata: button data for button to be disabled
 *
 * Enables given button pointed by @bdata.
 *
 * Make sure that @bdata->disable_lock is locked when entering
 * this function to avoid races with concurrent threads trying
 * to enable the same button at the same time.
 */
static void jack_keys_enable_button(struct jack_button_data *bdata)
{
	if (bdata->disabled) {
		enable_irq(gpio_to_irq(bdata->button->gpio));
		bdata->disabled = false;
	}
}

/**
 * gpio_keys_attr_show_helper() - fill in stringified bitmap of buttons
 * @ddata: pointer to drvdata
 * @buf: buffer where stringified bitmap is written
 * @type: button type (%EV_KEY, %EV_SW)
 * @only_disabled: does caller want only those buttons that are
 *                 currently disabled or all buttons that can be
 *                 disabled
 *
 * This function writes buttons that can be disabled to @buf. If
 * @only_disabled is true, then @buf contains only those buttons
 * that are currently disabled. Returns 0 on success or negative
 * errno on failure.
 */
static ssize_t jack_keys_attr_show_helper(struct jack_keys_drvdata *ddata,
					  char *buf, unsigned int type,
					  bool only_disabled)
{
	int n_events = get_n_events_by_type(type);
	unsigned long *bits;
	ssize_t ret;
	int i;

	bits = kcalloc(BITS_TO_LONGS(n_events), sizeof(*bits), GFP_KERNEL);
	if (!bits)
		return -ENOMEM;

	for (i = 0; i < ddata->n_buttons; i++) {
		struct jack_button_data *bdata = &ddata->data[i];

		if (bdata->button->type != type)
			continue;

		if (only_disabled && !bdata->disabled)
			continue;

		__set_bit(bdata->button->code, bits);
    if(bdata->button->code == KEY_MEDIA)
    {
      __set_bit(KEY_VOLUMEUP, bits);
      __set_bit(KEY_VOLUMEDOWN, bits);
    }
	}

	ret = bitmap_scnlistprintf(buf, PAGE_SIZE - 2, bits, n_events);
	buf[ret++] = '\n';
	buf[ret] = '\0';

	kfree(bits);

	return ret;
}

/**
 * gpio_keys_attr_store_helper() - enable/disable buttons based on given bitmap
 * @ddata: pointer to drvdata
 * @buf: buffer from userspace that contains stringified bitmap
 * @type: button type (%EV_KEY, %EV_SW)
 *
 * This function parses stringified bitmap from @buf and disables/enables
 * GPIO buttons accordinly. Returns 0 on success and negative error
 * on failure.
 */
static ssize_t jack_keys_attr_store_helper(struct jack_keys_drvdata *ddata,
					   const char *buf, unsigned int type)
{
	int n_events = get_n_events_by_type(type);
	unsigned long *bits;
	ssize_t error;
	int i;

	bits = kcalloc(BITS_TO_LONGS(n_events), sizeof(*bits), GFP_KERNEL);
	if (!bits)
		return -ENOMEM;

	error = bitmap_parselist(buf, bits, n_events);
	if (error)
		goto out;

	/* First validate */
	for (i = 0; i < ddata->n_buttons; i++) {
		struct jack_button_data *bdata = &ddata->data[i];

		if (bdata->button->type != type)
			continue;

		if (test_bit(bdata->button->code, bits) &&
		    !bdata->button->can_disable) {
			error = -EINVAL;
			goto out;
		}
	}

	mutex_lock(&ddata->disable_lock);

	for (i = 0; i < ddata->n_buttons; i++) {
		struct jack_button_data *bdata = &ddata->data[i];

		if (bdata->button->type != type)
			continue;

		if (test_bit(bdata->button->code, bits))
			jack_keys_disable_button(bdata);
		else
			jack_keys_enable_button(bdata);
	}

	mutex_unlock(&ddata->disable_lock);

out:
	kfree(bits);
	return error;
}

#define ATTR_SHOW_FN(name, type, only_disabled)				\
static ssize_t jack_keys_show_##name(struct device *dev,		\
				     struct device_attribute *attr,	\
				     char *buf)				\
{									\
	struct platform_device *pdev = to_platform_device(dev);		\
	struct jack_keys_drvdata *ddata = platform_get_drvdata(pdev);	\
									\
	return jack_keys_attr_show_helper(ddata, buf,			\
					  type, only_disabled);		\
}

ATTR_SHOW_FN(keys, EV_KEY, false);
ATTR_SHOW_FN(switches, EV_SW, false);
ATTR_SHOW_FN(disabled_keys, EV_KEY, true);
ATTR_SHOW_FN(disabled_switches, EV_SW, true);

/*
 * ATTRIBUTES:
 *
 * /sys/devices/platform/gpio-keys/keys [ro]
 * /sys/devices/platform/gpio-keys/switches [ro]
 */
static DEVICE_ATTR(keys, S_IRUGO, jack_keys_show_keys, NULL);
static DEVICE_ATTR(switches, S_IRUGO, jack_keys_show_switches, NULL);

#define ATTR_STORE_FN(name, type)					\
static ssize_t jack_keys_store_##name(struct device *dev,		\
				      struct device_attribute *attr,	\
				      const char *buf,			\
				      size_t count)			\
{									\
	struct platform_device *pdev = to_platform_device(dev);		\
	struct jack_keys_drvdata *ddata = platform_get_drvdata(pdev);	\
	ssize_t error;							\
									\
	error = jack_keys_attr_store_helper(ddata, buf, type);		\
	if (error)							\
		return error;						\
									\
	return count;							\
}

ATTR_STORE_FN(disabled_keys, EV_KEY);
ATTR_STORE_FN(disabled_switches, EV_SW);

/*
 * ATTRIBUTES:
 *
 * /sys/devices/platform/gpio-keys/disabled_keys [rw]
 * /sys/devices/platform/gpio-keys/disables_switches [rw]
 */
static DEVICE_ATTR(disabled_keys, S_IWUSR | S_IRUGO,
		   jack_keys_show_disabled_keys,
		   jack_keys_store_disabled_keys);
static DEVICE_ATTR(disabled_switches, S_IWUSR | S_IRUGO,
		   jack_keys_show_disabled_switches,
		   jack_keys_store_disabled_switches);

static struct attribute *jack_keys_attrs[] = {
	&dev_attr_keys.attr,
	&dev_attr_switches.attr,
	&dev_attr_disabled_keys.attr,
	&dev_attr_disabled_switches.attr,
	NULL,
};

static struct attribute_group jack_keys_attr_group = {
	.attrs = jack_keys_attrs,
};

static void jack_keys_report_event(struct jack_button_data *bdata)
{
	struct jack_keys_button *button = bdata->button;
	struct input_dev *input = bdata->input;
	unsigned int type = button->type ?: EV_KEY;
	int state = (gpio_get_value(button->gpio) ? 1 : 0) ^ button->active_low;
  int key_code = KEY_MAX;

  printk("### %s : type=%d, key=%d, state=%d ###\n",__func__,type,button->code,state);
  
  if( ( type == EV_KEY ) && ( button->code == KEY_MEDIA ) && ( mehmet_jack_get_type() == SEC_HEADSET_4POLE ) )
  {
    key_code = mehmet_convert_jack_remote_key( state );

    printk("### %s : converted_key=%d ###\n",__func__,key_code);

    if( key_code == KEY_MAX )
      return;

    printk("### %s : report key. type=%d, key_code=%d, state=%d ###\n",__func__,type,key_code,state);

  	input_event(input, type, key_code, !!state);
  	input_sync(input);

    return;
  }

  printk("### %s : report key. type=%d, button->code=%d, state=%d ###\n",__func__,type,button->code,state);

	input_event(input, type, button->code, !!state);
	input_sync(input);
}

static void jack_keys_work_func(struct work_struct *work)
{
	struct jack_button_data *bdata =
		container_of(work, struct jack_button_data, work);

	jack_keys_report_event(bdata);
}

static void jack_keys_timer(unsigned long _data)
{
	struct jack_button_data *data = (struct jack_button_data *)_data;

	schedule_work(&data->work);
}

static irqreturn_t jack_keys_isr(int irq, void *dev_id)
{
	struct jack_button_data *bdata = dev_id;
	struct jack_keys_button *button = bdata->button;

	BUG_ON(irq != gpio_to_irq(button->gpio));

	if (button->debounce_interval)
		mod_timer(&bdata->timer,
			jiffies + msecs_to_jiffies(button->debounce_interval));
	else
		schedule_work(&bdata->work);

	return IRQ_HANDLED;
}

static int __devinit jack_keys_setup_key(struct platform_device *pdev,
					 struct jack_button_data *bdata,
					 struct jack_keys_button *button)
{
	char *desc = button->desc ? button->desc : "jack_keys";
	struct device *dev = &pdev->dev;
	unsigned long irqflags;
	int irq, error;

	setup_timer(&bdata->timer, jack_keys_timer, (unsigned long)bdata);
	INIT_WORK(&bdata->work, jack_keys_work_func);

	error = gpio_request(button->gpio, desc);
	if (error < 0) {
		dev_err(dev, "failed to request GPIO %d, error %d\n",
			button->gpio, error);
		goto fail2;
	}

	error = gpio_direction_input(button->gpio);
	if (error < 0) {
		dev_err(dev, "failed to configure"
			" direction for GPIO %d, error %d\n",
			button->gpio, error);
		goto fail3;
	}

	irq = gpio_to_irq(button->gpio);

	if (irq < 0) {
		error = irq;
		dev_err(dev, "Unable to get irq number for GPIO %d, error %d\n",
			button->gpio, error);
		goto fail3;
	}

	// pulling up all keys
	s3c_gpio_setpull(button->gpio, S3C_GPIO_PULL_UP);

	irqflags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;
	/*
	 * If platform has specified that the button can be disabled,
	 * we don't want it to share the interrupt line.
	 */
	if (!button->can_disable)
		irqflags |= IRQF_SHARED;

	error = request_irq(irq, jack_keys_isr, irqflags, desc, bdata);
	if (error) {
		dev_err(dev, "Unable to claim irq %d; error %d\n",
			irq, error);
		goto fail3;
	}

	return 0;

fail3:
	gpio_free(button->gpio);
fail2:
	return error;
}

static int __devinit jack_keys_probe(struct platform_device *pdev)
{
	struct jack_keys_platform_data *pdata = pdev->dev.platform_data;
	struct jack_keys_drvdata *ddata;
	struct device *dev = &pdev->dev;
	struct input_dev *input;
	int i, error;
	int wakeup = 0;

	ddata = kzalloc(sizeof(struct jack_keys_drvdata) +
			pdata->nbuttons * sizeof(struct jack_button_data),
			GFP_KERNEL);
	input = input_allocate_device();
	if (!ddata || !input) {
		dev_err(dev, "failed to allocate state\n");
		error = -ENOMEM;
		goto fail1;
	}

	ddata->input = input;
	ddata->n_buttons = pdata->nbuttons;
	mutex_init(&ddata->disable_lock);

	platform_set_drvdata(pdev, ddata);

	input->name = pdev->name;
	input->phys = "jack-keys/input0";
	input->dev.parent = &pdev->dev;

	input->id.bustype = BUS_HOST;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0100;

	/* Enable auto repeat feature of Linux input subsystem */
	if (pdata->rep)
		__set_bit(EV_REP, input->evbit);

	for (i = 0; i < pdata->nbuttons; i++) {
		struct jack_keys_button *button = &pdata->buttons[i];
		struct jack_button_data *bdata = &ddata->data[i];
		unsigned int type = button->type ?: EV_KEY;

		bdata->input = input;
		bdata->button = button;

		error = jack_keys_setup_key(pdev, bdata, button);
		if (error)
			goto fail2;

		if (button->wakeup)
			wakeup = 1;

		input_set_capability(input, type, button->code);
    if(button->code == KEY_MEDIA)
    {
      input_set_capability(input, type, KEY_VOLUMEUP);
      input_set_capability(input, type, KEY_VOLUMEDOWN);
    }
	}

	error = sysfs_create_group(&pdev->dev.kobj, &jack_keys_attr_group);
	if (error) {
		dev_err(dev, "Unable to export keys/switches, error: %d\n",
			error);
		goto fail2;
	}

	error = input_register_device(input);
	if (error) {
		dev_err(dev, "Unable to register input device, error: %d\n",
			error);
		goto fail3;
	}

	/* get current state of buttons */
	for (i = 0; i < pdata->nbuttons; i++)
		jack_keys_report_event(&ddata->data[i]);
	input_sync(input);

	device_init_wakeup(&pdev->dev, wakeup);

	return 0;

 fail3:
	sysfs_remove_group(&pdev->dev.kobj, &jack_keys_attr_group);
 fail2:
	while (--i >= 0) {
		free_irq(gpio_to_irq(pdata->buttons[i].gpio), &ddata->data[i]);
		if (pdata->buttons[i].debounce_interval)
			del_timer_sync(&ddata->data[i].timer);
		cancel_work_sync(&ddata->data[i].work);
		gpio_free(pdata->buttons[i].gpio);
	}

	platform_set_drvdata(pdev, NULL);
 fail1:
	input_free_device(input);
	kfree(ddata);

	return error;
}

static int __devexit jack_keys_remove(struct platform_device *pdev)
{
	struct jack_keys_platform_data *pdata = pdev->dev.platform_data;
	struct jack_keys_drvdata *ddata = platform_get_drvdata(pdev);
	struct input_dev *input = ddata->input;
	int i;

	sysfs_remove_group(&pdev->dev.kobj, &jack_keys_attr_group);

	device_init_wakeup(&pdev->dev, 0);

	for (i = 0; i < pdata->nbuttons; i++) {
		int irq = gpio_to_irq(pdata->buttons[i].gpio);
		free_irq(irq, &ddata->data[i]);
		if (pdata->buttons[i].debounce_interval)
			del_timer_sync(&ddata->data[i].timer);
		cancel_work_sync(&ddata->data[i].work);
		gpio_free(pdata->buttons[i].gpio);
	}

	input_unregister_device(input);

	return 0;
}

#ifdef CONFIG_PM
static int jack_keys_suspend(struct device *dev)
{
#if 0
	struct platform_device *pdev = to_platform_device(dev);
	struct jack_keys_platform_data *pdata = pdev->dev.platform_data;
	int i;

	if (device_may_wakeup(&pdev->dev)) {
		for (i = 0; i < pdata->nbuttons; i++) {
			struct jack_keys_button *button = &pdata->buttons[i];
			if (button->wakeup) {
				int irq = gpio_to_irq(button->gpio);
				enable_irq_wake(irq);
			}
		}
	}
#endif // 0

	return 0;
}

static int jack_keys_resume(struct device *dev)
{
#if 0
	struct platform_device *pdev = to_platform_device(dev);
	struct jack_keys_drvdata *ddata = platform_get_drvdata(pdev);
	struct jack_keys_platform_data *pdata = pdev->dev.platform_data;
	int i;

	for (i = 0; i < pdata->nbuttons; i++) {

		struct jack_keys_button *button = &pdata->buttons[i];
		if (button->wakeup && device_may_wakeup(&pdev->dev)) {
			int irq = gpio_to_irq(button->gpio);
			disable_irq_wake(irq);
		}

		jack_keys_report_event(&ddata->data[i]);
	}
	input_sync(ddata->input);
#endif // 0

	return 0;
}

static const struct dev_pm_ops jack_keys_pm_ops = {
	.suspend	= jack_keys_suspend,
	.resume		= jack_keys_resume,
};
#endif

static struct platform_driver jack_keys_device_driver = {
	.probe		= jack_keys_probe,
	.remove		= __devexit_p(jack_keys_remove),
	.driver		= {
		.name	= "jack-keys",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &jack_keys_pm_ops,
#endif
	}
};

void mehmet_enable_media_key(void)
{
  if(!mehmet_jack_key_registered)
  {
    platform_driver_register(&jack_keys_device_driver);
    mehmet_jack_key_registered = 1;
  }
}
void mehmet_disable_media_key(void)
{
  if(mehmet_jack_key_registered)
  {
    platform_driver_unregister(&jack_keys_device_driver);
    mehmet_jack_key_registered = 0;
  }
}

static int __init jack_keys_init(void)
{
#if 0
  return platform_driver_register(&jack_keys_device_driver);
#endif // 0
  return 0;
}

static void __exit jack_keys_exit(void)
{
#if 0
  platform_driver_unregister(&jack_keys_device_driver);
#endif // 0
}

module_init(jack_keys_init);
module_exit(jack_keys_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phil Blundell <pb@handhelds.org>");
MODULE_DESCRIPTION("Keyboard driver for Earjack");
MODULE_ALIAS("platform:jack-keys");

#endif /* CONFIG_JACK_MGR */

