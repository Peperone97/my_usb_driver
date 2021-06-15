#include <linux/module.h>
#include <linux/usb.h>
#include <linux/kernel.h>

#define USB_VENDOR_ID	(0x45e)
#define USB_PRODUCT_ID	(0x7b9)

MODULE_LICENSE("Dual BSD/GPL");

//#define VENDOR_ID	0x045e
//#define KEYBOARD_ID	0x07b9

int called = 0;

static int my_driver_probe( struct usb_interface *interface, const struct usb_device_id *id ){
	struct usb_host_interface *iface;
	int i;

	called++;
	//printk(KERN_DEBUG "Device connected\n");

	iface = interface->cur_altsetting;
	for( i = 0; i < iface->desc.bNumEndpoints; i++ ){
		printk(KERN_DEBUG "Endpoint adress: 0x%x\n", iface->endpoint[i].desc.bEndpointAddress);
	}

	return 0;
}

static void my_driver_disconnect( struct usb_interface *interface ){
	printk(KERN_DEBUG "Device disconnected\n");
}

static struct usb_device_id my_driver_table [] ={
	{ USB_DEVICE( USB_VENDOR_ID, USB_PRODUCT_ID ) },
	{ USB_DEVICE( 0x03f0, 0x1c41 ) }, // my hp mouse
	{ USB_DEVICE( 0x1a86, 0x7529 ) },
	{ } // terminating entry
};
MODULE_DEVICE_TABLE( usb, my_driver_table );

static struct usb_driver my_driver = {
	.name		= "my_driver",
	.probe		= my_driver_probe,
	.disconnect	= my_driver_disconnect,
	.id_table	= my_driver_table,
};

static int __init my_init( void ){
	int res;

	res = usb_register( &my_driver );
	printk(KERN_DEBUG "My driver init, result of usb_register: %d\n", res);

	return res;
}

static void __exit my_exit( void ){
	usb_deregister( &my_driver );
	printk(KERN_DEBUG "My driver exit: %d\n", called);
}

module_init( my_init );
module_exit( my_exit );
