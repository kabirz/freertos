#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/crc.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/gpio.h>
#include <FreeRTOS.h>
#include <queue.h>

extern int printf(const char *format, ...);
static void __attribute__((constructor(101))) clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
}

static __attribute__((constructor(102))) void gpio_setup(void)
{
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

void led_task(void *arg __attribute__((unused)))
{
	while (1) {
		gpio_toggle(GPIOB, GPIO8);
		gpio_toggle(GPIOC, GPIO13);
		vTaskDelay(100);
	}
}

void uart_task(void *arg)
{
	int count = (int)arg;

	while (count >= 0) {
		printf("%04d: uart enable!\n", count);
		vTaskDelay(200);
		count--;
	}
	printf("task: %s end!\n", __FUNCTION__);
	vTaskDelete(NULL);
}

void crc_task(void *arg __attribute__((unused)))
{
	uint8_t array[256];
	for (int i = 0; i < 256; ++i) {
		array[i] = i + 0;
	}
	rcc_periph_clock_enable(RCC_CRC);
	crc_reset();
	uint32_t val = crc_calculate_block((uint32_t *)&array, sizeof(array)/4);
	if (val == 0xb7ec66F4) printf("crc test pass, result: 0x%x\n", val);

	crc_reset();
	val = crc_calculate(0x31323334);
	if (val == 0xa695c4aa) printf("crc test pass, result: 0x%x\n", val);
	rcc_periph_clock_disable(RCC_CRC);
	vTaskDelay(2000);
/*  back register test */
#define RTC_BKP_DR(reg)  MMIO16(BACKUP_REGS_BASE + 4 + (4 * (reg)))
	rcc_periph_clock_enable(RCC_BKP);
	if (RTC_BKP_DR(1) == 100) printf("val test pass, val: %d\n", 100);
	pwr_disable_backup_domain_write_protect();
	RTC_BKP_DR(1) = 100;
	pwr_enable_backup_domain_write_protect();
	vTaskDelete(NULL);
}

int main(void)
{
	xTaskCreate(led_task, "led_task", 120, NULL, 4, NULL);
	xTaskCreate(uart_task, "uart_task", 400, (void *)10, 4, NULL);
	xTaskCreate(crc_task, "crc_task", 400, NULL, 4, NULL);
	vTaskStartScheduler();

	return 0;
}
