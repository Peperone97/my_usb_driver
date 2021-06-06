#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <libusb-1.0/libusb.h>

/*
1c 00 c0 97 69 a5 89 e5 ff ff 00 00 00 00 1b 00
00 01 00 09 00 00 02 62 00 00 00 00 21 09 00 03
00 00 5a 00 00 1f 00 00 00 08 0f 03 00 00 00 00
00 00 ff 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 fb 00
*/

void parseInput( char *str, int len ){
	char msg[] = {  0x1c, 0x00, 0xc0, 0x97, 0x69, 0xa5, 0x89, 0xe5, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00,
			0x00, 0x01, 0x00, 0x09, 0x00, 0x00, 0x02, 0x62, 0x00, 0x00, 0x00, 0x00, 0x21, 0x09, 0x00, 0x03,
			0x00, 0x00, 0x5a, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x08, 0x0f, 0x03, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfb, 0x00
	};
	int i;
	/* A = 4  - Z = 29 */
	for( i = 0; i < len; i++ ){
		if( str[i] >= 4 && str[i] <= 29 ){
			putchar( str[i]-4+'A' );
		}else if( str[i] == 44 ){
			putchar(' ');
		}else if( str[i] == 40 ){
			putchar('\n');
		}
	}
	putchar('\n');
	/*for( i = 0; i < read; i++ ){
		printf( "%d ", str[i] );
	}
	putchar('\n');*/
}

void keyboardPrv( struct libusb_device_handle *device_handle ){
	char *str;
	int read;
	printf("Start reading\n");
	int err = libusb_bulk_transfer( device_handle, 0x81, str, 256, &read, 2000 );
	if( err == 0 || err == LIBUSB_ERROR_TIMEOUT ){
		printf( "read: %d\n", read );
		parseInput( str, read );
	}else{
		printf( "Error: %s\n", libusb_strerror( err ) );
	}
}

int main( int argc, char **argv ){
	const int razer_vendor_id = 0x1532, keyboard = 0x23f;
	char *str = malloc( sizeof(char) * 256 );

	if( libusb_init( 0 ) ){
		printf( "Warning: libusb init error\n" );
		return 1;
	}
	struct libusb_device **devices;
	int len = libusb_get_device_list( NULL, &devices );
	if( len < 0 ){
		printf( "Warning: device detecting error\n" );
		return 2;
	}

	//analyze USB devices
	int i;
	for( i = 0; i < len; i++ ){
		int n_port = libusb_get_port_number( devices[i] );
		if( n_port ){ //the port number is 0 if is not available
			struct libusb_device_descriptor *desc;
			if( !libusb_get_device_descriptor( devices[i], desc ) ){
				if( desc->idVendor == razer_vendor_id ){
					printf( "Port number: %d\tBus number: %d", n_port, libusb_get_bus_number( devices[i] ) );
					printf("\tProduct number: %d\n", desc->idProduct);
					
					struct libusb_device_handle *dev_handle;
					int err;
					if( (err = libusb_open( devices[i], &dev_handle )) ){
						printf( "Open error: %s\n", libusb_strerror( err ) );
					}else{
						libusb_get_string_descriptor_ascii( dev_handle, desc->iProduct, str, 256 );
						printf( "\t%s\n", str );

						if( desc->idProduct == keyboard ){
							struct libusb_config_descriptor *conf;
							libusb_get_active_config_descriptor( devices[i], &conf );
							printf("%d\n", conf->bNumInterfaces );
							while(1){
								libusb_detach_kernel_driver( dev_handle, desc->iSerialNumber );
								if( ( err = libusb_claim_interface( dev_handle, 0 ) ) != 0 ){
									printf( "Error: %d %s\n", err, libusb_strerror( err ) );
								}
								keyboardPrv( dev_handle );
								libusb_release_interface( dev_handle, 0 );
								libusb_attach_kernel_driver( dev_handle, desc->iSerialNumber );
								sleep(5);
							}
							libusb_free_config_descriptor( conf );
						}

						libusb_close( dev_handle );
					}
					
				}
			}else{
				printf( "\tDescription error\n");
			}
		}
	}

	free(str);
	libusb_free_device_list( devices, len );
	libusb_exit( NULL );

	return 0;
}
