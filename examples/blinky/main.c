#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <FreeRTOS.h>
#include <queue.h>

extern int printf(const char *format, ...);
static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
}

static void gpio_setup(void)
{
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

void led_task(void *arg)
{
	(void)arg;
	while (1) {
		gpio_toggle(GPIOB, GPIO8);
		gpio_toggle(GPIOC, GPIO13);
		vTaskDelay(100);
	}
}

void uart_task(void *arg)
{
	int count = (int)arg;
	vTaskDelay(1000);
	while (count >= 0) {
		printf("%04d: uart enable!\n", count);
		vTaskDelay(200);
		count--;
	}
	printf("task: %s end!\n", __FUNCTION__);
	vTaskDelete(NULL);
}

int main(void)
{
	clock_setup();
	gpio_setup();
	
	xTaskCreate(led_task, "led_task", 1000, NULL, 4, NULL);
	xTaskCreate(uart_task, "uart_task", 1000, (void *)100, 4, NULL);
	vTaskStartScheduler();

	return 0;
}
