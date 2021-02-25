#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdbool.h>
#include <errno.h>

static bool _usart_enabled;
static void usart_setup(void)
{
	/* Setup GPIO pin GPIO_USART1_RE_TX on GPIO port B for transmit. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX);

	/* Finally enable the USART. */
	usart_enable(USART1);
	_usart_enabled = true;
}

void _exit(int status)
{
	(void)status;
	while(1){}
}

int _write(int file, char *ptr, int len)
{
	int i = -1;

	if (file == 1) {
		if (!_usart_enabled) usart_setup();

		for (i = 0; i < len; i++)
			usart_send_blocking(USART1, ptr[i]);
		return i;
	}

	errno = EIO;
	return i;
}
