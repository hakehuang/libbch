#ifndef _LIBBCH_H_
#define _LIBBCH_H_

#include "bch.h"

struct libbch
{
	/* lengthes */
	unsigned int data_len;
	unsigned int ecc_len;
	unsigned int ecc_cap;
	unsigned int error_count;

	/* private */
	struct bch_control *bch_ctrl;
	unsigned int *errloc;
};


void libbch_free(struct libbch *libbch);

struct libbch* libbch_init(unsigned int data_len, unsigned int ecc_cap);

void libbch_encode(struct libbch *libbch, const uint8_t *data, uint8_t *ecc);

int libbch_decode(struct libbch *libbch, const uint8_t *data, uint8_t *ecc);

void libbch_correct_all(struct libbch *libbch, uint8_t *data, uint8_t *ecc);

void libbch_dump(struct libbch *libbch);

void libbch_dump_errloc(struct libbch *libbch);

#endif /* _LIBBCH_H_ */
