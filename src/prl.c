/**
 * File              : prl.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 26.05.2024
 * Last Modified Date: 26.05.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/libdoc/doc.h"
#include <cstdint>

// parse prl - return len of grpPrlAndIstd
int parse_papxInFkp(struct PapxInFkp *papxInFkp, 
		struct GrpPrlAndIstd **grpPrlAndIstd)
{
	void *p = (void *)papxInFkp->grpprlInPapx;
	if (papxInFkp->cb){
		// If cb is nonzero, grpprlInPapx is
		// GrpPrlAndIstd.
		*grpPrlAndIstd = 
			(struct GrpPrlAndIstd *)(papxInFkp->grpprlInPapx);
		return 1;
	} else {
		//If this value is 0, the size is
		// specified by the first byte of
		// grpprlInPapx.
		uint8_t *cb_ = &(((uint8_t *)p)[0]);
		// If this value is not 0,
		// the grpprlInPapx is 2×cb-1 bytes long.
		void    *pp  = &(((uint8_t *)p)[1]);
	}
}
