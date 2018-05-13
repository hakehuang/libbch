#include <stdio.h>
#include <stdlib.h>
#include "bchlib.h"

static unsigned int find_m(unsigned int data_len, unsigned int ecc_cap)
{
	unsigned int m;
	unsigned int data_bits = data_len * 8;

	for (m = 5; m <= 15; m++) {
		if(data_bits + (m * ecc_cap) <= (2 << (m - 1)) - 1)
			return m;
	}

	return -1;
}

void bchlib_free(struct bchlib *bchlib)
{
	free(bchlib->errloc);
	free(bchlib->bch_ctrl);
	free(bchlib);
}

struct bchlib* bchlib_init(unsigned int data_len, unsigned int ecc_cap)
{
	unsigned int m;
	struct bchlib *bchlib;

	if ((ecc_cap >= 32) || (ecc_cap <= 0)) {
		printf("Invalid ecc_cap: %u\n", ecc_cap);
		return NULL;
	}
	
	if (data_len <= 0) {
		printf("Invalid data_len: %u\n", data_len);
		return NULL;
	}

	m = find_m(data_len, ecc_cap);
	printf("Found m = %u\n", m);

	/* init bchlib and bch_control */
	bchlib = (struct bchlib*)malloc(sizeof(*bchlib));
	bchlib->bch_ctrl = init_bch(m, ecc_cap, 0);
	
	/* init lengthes */
	bchlib->data_len = data_len;
	bchlib->ecc_len = bchlib->bch_ctrl->ecc_bytes;
	bchlib->ecc_cap = bchlib->bch_ctrl->t;

	/* init buffers */
	bchlib->errloc = (unsigned int*)
			 malloc(bchlib->ecc_cap * sizeof(*bchlib->errloc));

	return bchlib;
}

void bchlib_encode(struct bchlib *bchlib, const uint8_t *data, uint8_t *ecc)
{
	encode_bch(bchlib->bch_ctrl, data, bchlib->data_len, ecc);
}

int bchlib_decode(struct bchlib *bchlib, const uint8_t *data, uint8_t *ecc)
{
	return decode_bch(bchlib->bch_ctrl, data, bchlib->data_len, ecc, NULL,
			  NULL, bchlib->errloc);
}

void bchlib_correct_all(struct bchlib *bchlib, uint8_t *data, uint8_t *ecc)
{
	int i;
	unsigned int *errloc = bchlib->errloc;

	for (i = 0; i < bchlib->ecc_cap; i++) {
		unsigned int err_byte = errloc[i] / 8;

		if (err_byte < bchlib->data_len) {
			/* bit error in data area */
			data[err_byte] ^= 1 << (errloc[i] % 8);
		} else if (err_byte < (bchlib->data_len + bchlib->ecc_len)) {
			/* bit error in ecc area */
			ecc[err_byte - bchlib->data_len] ^= 1 << (errloc[i] % 8);
		}
	}
}

void bchlib_dump(struct bchlib *bchlib)
{
	printf("bchlib:\n");
	printf("  m = %u\n", bchlib->bch_ctrl->m);
	printf("  data_len = %u\n", bchlib->data_len);
	printf("  ecc_len = %u\n", bchlib->ecc_len);
	printf("  ecc_cap = %u\n", bchlib->ecc_cap);
}

void bchlib_dump_errloc(struct bchlib *bchlib)
{
	int i;

	for (i = 0; i < bchlib->ecc_cap; i++)
		printf("%u ", bchlib->errloc[i]);
	printf("\n");
}
