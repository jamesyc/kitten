/** \file
 * Interface to standard PC keyboards.
 *
 * Nothing is done with the keys in this handler.
 *
 * It is in device class "late", which is done after most every other
 * device has been setup.
 */
#include <lwk/driver.h>
#include <lwk/signal.h>
#include <lwk/interrupt.h>
#include <arch/proto.h>
#include <arch/io.h>
#include <arch/idt_vectors.h>


/**
 * Handle a keyboard interrupt.
 */
static irqreturn_t
do_keyboard_interrupt(
	int			vector,
	void *			unused
)
{
	const uint8_t KB_STATUS_PORT = 0x64;
	const uint8_t KB_DATA_PORT   = 0x60;
	const uint8_t KB_OUTPUT_FULL = 0x01;

	uint8_t status = inb(KB_STATUS_PORT);

	if ((status & KB_OUTPUT_FULL) == 0)
		return IRQ_NONE;

	uint8_t key = inb(KB_DATA_PORT);
	printk("Keyboard Interrupt: status=%u, key=%u\n", status, key);

	return IRQ_HANDLED;
}


/**
 * Register handlers for standard PC devices.
 */
int
keyboard_init( void )
{
	irq_request(
		IRQ1_VECTOR,
		&do_keyboard_interrupt,
		0,
		"keyboard",
		NULL
	);

	return 0;
}


DRIVER_INIT( "late", keyboard_init );
