#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/kernel.h>

MODULE_LICENSE("Dual BSD/GPL");

#define VENDOR_ID	0x045e
#define KEYBOARD_ID	0x07b9

static struct usb_device_id my_driver_table [] ={
	{ USB_DEVICE( VENDOR_ID, KEYBOARD_ID ) },
	{ } // terminating entry
};
MODULE_DEVICE_TABLE( usb, my_driver_table );

static int my_driver_probe( struct usb_interface *interface, const struct usb_device_id *id ){
	printk(KERN_ALERT "Device connected\n");
	return 0;
}

static void my_driver_disconnect( struct usb_interface *interface ){
	printk(KERN_ALERT "Device disconnected\n");
}

static struct usb_driver my_driver = {
	.name		= "my_driver",
	.probe		= my_driver_probe,
	.disconnect	= my_driver_disconnect,
	.id_table	= my_driver_table,
};

static int __init my_init( void ){
	printk(KERN_ALERT "Hello world\n");
	return usb_register( &my_driver );
}

static void __exit my_exit( void ){
	usb_deregister( &my_driver );
	printk(KERN_ALERT "Exit\n");
}

module_init( my_init );
module_exit( my_exit );
