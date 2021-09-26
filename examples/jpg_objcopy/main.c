#include <stdio.h>
#include <stdint.h>

extern uint8_t image_end[];
extern uint8_t image_start[];

int main(int argc, char *argv[])
{
	size_t len = image_end - image_start;

	printf("start:\t%p\n", image_start);
	printf("end:\t%p\n", image_end);
	printf("size:\t%#010x\n", len);
	return 0;
}
