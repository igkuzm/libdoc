/**
 * File              : direct_paragraph_formatting.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 26.05.2024
 * Last Modified Date: 26.05.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
*/

#include "../include/libdoc/direct_paragraph_formatting.h"

/* 2.4.6.1 Direct Paragraph Formatting
 * This section explains how to find the properties applied
 * directly (as opposed to through a style, for
 * example) to a paragraph, given a character position cp
 * within it. The properties are found as an array
 * of Prl elements. */
void direct_paragraph_formatting(
		cfb_doc_t *doc, CP cp,
		struct PapxFkp *papxFkp, int k,
		uint32_t of, struct Pcd *pcd)
{
/* 1. Follow the algorithm from Determining Paragraph
 * Boundaries for finding the character position of
 * the last character in the paragraph to completion. From
 * step 5, remember the PapxFkp and k.
 * From step 4, remember the offset in the WordDocument
 * Stream at which PapxFkp was read. Let
 * this offset be called of. From step 2 remember the Pcd.
 * If the algorithm from Determining
 * Paragraph Boundaries specifies that cp is invalid, leave
 * the algorithm. */

/* 2. Find a BxPap at PapxFkp.rgbx[k]. Find a PapxInFkp at
 * offset of + 2*BxPap.bOffset */
	struct BxPap *bxPap = (struct BxPap *)(papxFkp->rgbx[k]);
	uint8_t offset = of + 2 * bxPap->bOffset;
	void *p = &((papxFkp->rgbx)[offset]);
	struct PapxInFkp *papxInFkp = (struct PapxInFkp *)p;

/* 3. Find a GrpprlAndIstd in the PapxInFkp from step 2.
 * The offset and size of the GrpprlAndIstd
 * is instructed by the first byte of the PapxInFkp, as
 * detailed at PapxInFkp. */
	struct GrpPrlAndIstd *grpPrlAndIstd;
	int len = 1;
	
	if (papxInFkp->cb){
		grpPrlAndIstd = (struct GrpPrlAndIstd *)papxInFkp->grpprlInPapx;
	} else {
		// cb is 0
		uint8_t cb_ = papxInFkp->grpprlInPapx[0];
		if (cb_ < 1) {
			// error
			ERR("cb' MUST be at least 1");
			return;
		}
		void *p = &(papxInFkp->grpprlInPapx[1]);
		grpPrlAndIstd = (struct GrpPrlAndIstd *)p;
		len = 2 * cb_;
	}

/* 4. Find the grpprl within the GrpprlAndIstd. This is an
 * array of Prl elements that specifies the
 * direct properties of this paragraph. */

	int i;
	for (i = 0; i < len; ++i) {
		struct Prl *prl = grpPrlAndIstd->grpprl[i];
	}

/* 5. Finally Pcd.Prm specifies further property
 * modifications that apply to this paragraph. If Pcd.Prm
 * is a Prm0 and the Sprm specified within Prm0 modifies a
 * paragraph property, append to the
 * array of Prl elements from the previous step a single Prl
 * made of the Sprm and value in Prm0. if
 * Pcd.Prm is a Prm1, append to the array of Prl elements
 * from the previous step any Sprm
 * structures that modify paragraph properties within the
 * array of Prl elements specified by Prm1. */
}
