#include <stdio.h>
#include <stdint.h>

extern uint8_t _binary_pic_jpg_end[];
extern uint8_t _binary_pic_jpg_start[];

int main(int argc, char *argv[])
{
	size_t len = _binary_pic_jpg_end - _binary_pic_jpg_start;

	printf("start:\t%p\n", _binary_pic_jpg_start);
	printf("end:\t%p\n", _binary_pic_jpg_end);
	printf("size:\t%lu\n", len);
	return 0;
}
