#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>

#include "libbch.h"

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

void parse_bch_decode_err(int err_code)
{
	if (err_code == -EBADMSG)
		printf("libbch decode failed: %d\n", err_code);
	else if (err_code == -EINVAL)
		printf("bhclib decode invalid parameters: %d\n", err_code);
	else
		printf("bhclib decode unknown error: %d\n", err_code);
}

int simple_test(void)
{
	struct libbch *libbch;
	uint8_t *data;
	uint8_t *ecc;
	int errcnt;

	libbch = libbch_init(512, 8);

	libbch_dump(libbch);

	data = malloc(libbch->data_len);
	memset(data, 0xFF, libbch->data_len);

	ecc = malloc(libbch->ecc_len);
	memset(ecc, 0x0, libbch->ecc_len);

	libbch_encode(libbch, data, ecc);
	//dump_data(data, libbch->data_len);
	//dump_data(ecc, libbch->ecc_len);

	data[0] = 0xF0;
	ecc[0] = ecc[0] ^ 0xF0;

	errcnt = libbch_decode(libbch, data, ecc);
	if (errcnt > 0) {
		libbch_dump_errloc(libbch);
	} else if (errcnt < 0) {
		parse_bch_decode_err(errcnt);
		goto has_errors;
	}

	libbch_correct_all(libbch, data, ecc);

	errcnt = libbch_decode(libbch, data, ecc);
	if (errcnt > 0) {
		libbch_dump_errloc(libbch);
	} else if (errcnt < 0) {
		parse_bch_decode_err(errcnt);
		goto has_errors;
	}

	return 0;

has_errors:
	libbch_free(libbch);
	return errcnt;
}

void init_random_bch_data(struct libbch *libbch, uint8_t *data, uint8_t *ecc)
{
	int i;
	uint32_t *data32 = (uint32_t*)data;

	for (i = 0; i < (libbch->data_len / 4); i++)
		data32[i] = rand();

	memset(ecc, 0x0, libbch->ecc_len);
	libbch_encode(libbch, data, ecc);
}

void flip_random_bits(struct libbch *libbch, uint8_t *data, uint8_t *ecc)
{
	int i;
	unsigned int data_bits = libbch->data_len * 8;
	unsigned int ecc_bits = libbch->ecc_len * 8;
	unsigned int total_bits = data_bits + ecc_bits;
	unsigned int flip_bits = (rand() % libbch->ecc_cap) + 1;
	unsigned int err_bit;
	unsigned int err_byte;

	for (i = 0; i < flip_bits; i++) {
		err_bit = rand() % total_bits;
		err_byte = err_bit / 8;

		if (err_bit < data_bits) {
			data[err_byte] ^= 1 << (err_bit % 8);
		} else {
			ecc[err_byte - libbch->data_len] ^= 1 << (err_bit % 8);
		}
	}
}

int libbch_verify_random(void)
{
	struct libbch *libbch;
	uint8_t *data;
	uint8_t *ecc;
	uint8_t *data_golden;
	uint8_t *ecc_golden;
	int errcnt;

	libbch = libbch_init(64, 8);
	libbch_dump(libbch);
	data = malloc(libbch->data_len);
	ecc = malloc(libbch->ecc_len);
	data_golden = malloc(libbch->data_len);
	ecc_golden = malloc(libbch->ecc_len);

	while (1) {
		init_random_bch_data(libbch, data, ecc);
		memcpy(data_golden, data, libbch->data_len);
		memcpy(ecc_golden, ecc, libbch->ecc_len);

		flip_random_bits(libbch, data, ecc);

		errcnt = libbch_decode(libbch, data, ecc);
		if (errcnt > 0) {
			//libbch_dump_errloc(libbch);
		} else if (errcnt < 0) {
			parse_bch_decode_err(errcnt);
			goto has_errors;
		}

		libbch_correct_all(libbch, data, ecc);

		if (memcmp(data, data_golden, libbch->data_len)) {
			printf("verify: data memcmp failed\n");
			break;
		}
		if (memcmp(ecc, ecc_golden, libbch->ecc_len)) {
			printf("verify: ecc memcmp failed\n");
			break;
		}

		errcnt = libbch_decode(libbch, data, ecc);
		if (errcnt != 0) {
			printf("verify: decode failed\n");
			break;
		}

		printf(".");
	}

	return 0;

has_errors:
	libbch_free(libbch);
	return errcnt;
}

int main(int argc, const char *argv[])
{
	srand(time(NULL));
	//simple_test();
	libbch_verify_random();

	return 0;
}
