#ifndef _BCHLIB_H_
#define _BCHLIB_H_

#include "bch.h"

struct bchlib
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


void bchlib_free(struct bchlib *bchlib);

struct bchlib* bchlib_init(unsigned int data_len, unsigned int ecc_cap);

void bchlib_encode(struct bchlib *bchlib, const uint8_t *data, uint8_t *ecc);

int bchlib_decode(struct bchlib *bchlib, const uint8_t *data, uint8_t *ecc);

void bchlib_correct_all(struct bchlib *bchlib, uint8_t *data, uint8_t *ecc);

void bchlib_dump(struct bchlib *bchlib);

void bchlib_dump_errloc(struct bchlib *bchlib);

#endif /* _BCHLIB_H_ */
