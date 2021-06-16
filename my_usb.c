#include <linux/module.h>
#include <linux/usb.h>
#include <linux/kernel.h>

#define USB_VENDOR_ID	(0x45e)
#define USB_PRODUCT_ID	(0x7b9)

MODULE_LICENSE("Dual BSD/GPL");

//#define VENDOR_ID	0x045e
//#define KEYBOARD_ID	0x07b9

struct urb *my_usb_urb = NULL;

static int my_driver_probe( struct usb_interface *interface, const struct usb_device_id *id ){
	struct usb_host_interface *iface;
	int i;

	printk(KERN_DEBUG "Vendor: 0x%x\tProduct: 0x%x\n", id->idVendor, id->idProduct);

	iface = interface->cur_altsetting;
	for( i = 0; i < iface->desc.bNumEndpoints; i++ ){
		printk(KERN_DEBUG "Endpoint adress: 0x%x\tMax pkg size: %d\n", iface->endpoint[i].desc.bEndpointAddress, iface->endpoint[i].desc.wMaxPacketSize);
	}

	my_usb_urb = usb_alloc_urb( 0, GFP_KERNEL );
	if( my_usb_urb == NULL ){
		printk(KERN_ALERT "URB error\n" );
	}

	return 0;
}

static void my_driver_disconnect( struct usb_interface *interface ){
	usb_free_urb( my_usb_urb );
	my_usb_urb = NULL;
	printk(KERN_DEBUG "Device disconnected\n");
}

static struct usb_device_id my_driver_table [] ={
	{ USB_DEVICE( USB_VENDOR_ID, USB_PRODUCT_ID ) }, // my keyboard
	{ USB_DEVICE( 0x03f0, 0x1c41 ) }, // my hp mouse
	{ USB_DEVICE( 0x1a86, 0x7529 ) }, //
	{ USB_DEVICE( 0X22b8, 0x2e82 ) }, // my phone
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
	if( my_usb_urb != NULL ){
		usb_free_urb( my_usb_urb );
	}
	usb_deregister( &my_driver );
	printk(KERN_DEBUG "My driver exit\n");
}

module_init( my_init );
module_exit( my_exit );
