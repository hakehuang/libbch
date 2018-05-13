#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "bchlib.h"

void dump_data(uint8_t *data, unsigned int len)
{
	int i;

	for (i = 0; i < len; i++) {
		if ((i % 8) == 0)
			printf("\n");
		printf("0x%02X ", data[i]);
	}
	printf("\n");
}

int main(int argc, const char *argv[])
{
	struct bchlib *bchlib;
	uint8_t *data;
	uint8_t *ecc;
	int errcnt;

	bchlib = bchlib_init(512, 8);

	bchlib_dump(bchlib);

	data = malloc(bchlib->data_len);
	memset(data, 0xFF, bchlib->data_len);

	ecc = malloc(bchlib->ecc_len);
	memset(ecc, 0x0, bchlib->ecc_len);

	bchlib_encode(bchlib, data, ecc);
	//dump_data(data, bchlib->data_len);
	//dump_data(ecc, bchlib->ecc_len);

	data[0] = 0;
	errcnt = bchlib_decode(bchlib, data, ecc);

	if(errcnt >= 0) {
		printf("bchlib_decode errcnt = %d\n", errcnt);
		if (errcnt > 0)
			bchlib_dump_errloc(bchlib);
	} else {
		if (errcnt == -EBADMSG)
			printf("bchlib decode failed: %d\n", errcnt);
		else if (errcnt == -EINVAL)
			printf("bhclib invalid parameters: %d\n", errcnt);
		else
			printf("bhclib decode error: %d\n", errcnt);
	}

	bchlib_correct_all(bchlib, data, ecc);

	errcnt = bchlib_decode(bchlib, data, ecc);
	if(errcnt >= 0) {
		printf("bchlib_decode errcnt = %d\n", errcnt);
		if (errcnt > 0)
			bchlib_dump_errloc(bchlib);
	} else {
		if (errcnt == -EBADMSG)
			printf("bchlib decode failed: %d\n", errcnt);
		else if (errcnt == -EINVAL)
			printf("bhclib invalid parameters: %d\n", errcnt);
		else
			printf("bhclib decode error: %d\n", errcnt);
	}

	bchlib_free(bchlib);

	return 0;
}
