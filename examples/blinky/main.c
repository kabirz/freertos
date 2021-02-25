#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <FreeRTOS.h>
#include <queue.h>

static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOB);
}

static void gpio_setup(void)
{
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);
}

void led_task(void *arg)
{
	(void)arg;
	while (1) {
		gpio_toggle(GPIOB, GPIO8);
		vTaskDelay(100);
	}
}

void uart_task(void *arg)
{
	int i = 0;
	(void)arg;
	while (1) {
		printf("%04d: uart enable!\n", i);
		vTaskDelay(200);
		i++;
	}
}

int main(void)
{
	TaskHandle_t handle, handle1;
	clock_setup();
	gpio_setup();
	
	xTaskCreate(led_task, "led_task", 120, NULL, 4, &handle);
	xTaskCreate(uart_task, "uart_task", 120, NULL, 4, &handle1);
	vTaskStartScheduler();

	return 0;
}
