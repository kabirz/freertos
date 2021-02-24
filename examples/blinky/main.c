#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <FreeRTOS.h>
#include <queue.h>

/* Set STM32 to 72 MHz. */
static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

	/* Enable GPIOB and GPIOC clock. */
	rcc_periph_clock_enable(RCC_GPIOB);
}

static void gpio_setup(void)
{

	/* Set GPIO7 (in GPIO port B) to 'output push-pull'. */
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO7);
}


int main(void)
{
	QueueHandle_t queue;
	clock_setup();
	gpio_setup();

	queue = xQueueCreate(2, sizeof(uint32_t));

	while (1) {
		gpio_toggle(GPIOB, GPIO7);	/* LED on/off */

		for (int i = 0; i < 1000000; i++)	/* Wait a bit. */
			__asm__ __volatile__ ("nop");
	}

	return 0;
}
