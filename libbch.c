#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "libbch.h"

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

void libbch_free(struct libbch *libbch)
{
	free(libbch->errloc);
	free(libbch->bch_ctrl);
	free(libbch);
}

struct libbch* libbch_init(unsigned int data_len, unsigned int ecc_cap)
{
	unsigned int m;
	struct libbch *libbch;

	if ((ecc_cap >= 32) || (ecc_cap <= 0)) {
		printf("Invalid ecc_cap: %u\n", ecc_cap);
		return NULL;
	}
	
	if (data_len <= 0) {
		printf("Invalid data_len: %u\n", data_len);
		return NULL;
	}

	m = find_m(data_len, ecc_cap);

	/* init libbch and bch_control */
	libbch = (struct libbch*)malloc(sizeof(*libbch));
	libbch->bch_ctrl = init_bch(m, ecc_cap, 0);
	
	/* init lengthes */
	libbch->data_len = data_len;
	libbch->ecc_len = libbch->bch_ctrl->ecc_bytes;
	libbch->ecc_cap = libbch->bch_ctrl->t;

	/* init buffers */
	libbch->errloc = (unsigned int*)
			 malloc(libbch->ecc_cap * sizeof(*libbch->errloc));

	return libbch;
}

void libbch_encode(struct libbch *libbch, const uint8_t *data, uint8_t *ecc)
{
	encode_bch(libbch->bch_ctrl, data, libbch->data_len, ecc);
}

int libbch_decode(struct libbch *libbch, const uint8_t *data, uint8_t *ecc)
{
	int errcnt = decode_bch(libbch->bch_ctrl, data, libbch->data_len, ecc,
				NULL, NULL, libbch->errloc);

	if (errcnt >= 0)
		libbch->error_count = errcnt;
	else if (errcnt == -EBADMSG)
		libbch->error_count = libbch->ecc_cap + 1;

	return errcnt;
}

void libbch_correct_all(struct libbch *libbch, uint8_t *data, uint8_t *ecc)
{
	int i;
	unsigned int *errloc = libbch->errloc;

	for (i = 0; i < libbch->error_count; i++) {
		unsigned int err_byte = errloc[i] / 8;

		if (err_byte < libbch->data_len) {
			/* bit error in data area */
			data[err_byte] ^= 1 << (errloc[i] % 8);
		} else if (err_byte < (libbch->data_len + libbch->ecc_len)) {
			/* bit error in ecc area */
			ecc[err_byte - libbch->data_len] ^= 1 << (errloc[i] % 8);
		}
	}
}

void libbch_dump(struct libbch *libbch)
{
	printf("libbch:\n");
	printf("  m = %u\n", libbch->bch_ctrl->m);
	printf("  data_len = %u\n", libbch->data_len);
	printf("  ecc_len = %u\n", libbch->ecc_len);
	printf("  ecc_cap = %u\n", libbch->ecc_cap);
}

void libbch_dump_errloc(struct libbch *libbch)
{
	int i;
	unsigned int errloc_byte;

	printf("libbch errloc:\n  ");
	for (i = 0; i < libbch->error_count; i++) {
		errloc_byte = libbch->errloc[i] / 8;
		if (errloc_byte < libbch->data_len) {
			printf("%u ", errloc_byte);
		} else {
			printf("%u+%u ", libbch->data_len,
					 errloc_byte - libbch->data_len);
		}
	}
	printf("\n");
}
