#include "usb.h"
#include "drivers/usb/csud/include/usbd/usbd.h"


void usb_init(void) 
{
	// Initialise USB
	UsbInitialise();
}