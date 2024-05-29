/**
 * File              : style_properties.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 28.05.2024
 * Last Modified Date: 29.05.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/libdoc/style_properties.h"
#include "memread.h"
#include <stdint.h>
#include <stdio.h>

/* 2.4.6.5 Determining Properties of a Style
 * This section specifies an algorithm to determine the set
 * of properties to apply to text, a paragraph, a
 * table, or a list when a particular style is applied to
 * it. Given an istd, one or more arrays of Prl can be
 * derived that express the differences from defaults for
 * this style. Depending on its stk, a style can
 * specify properties for any combination of tables,
 * paragraphs, and characters. */

/* Given an istd: */
void apply_style_properties(cfb_doc_t *doc, USHORT istd)
{
/* 1. Read the FIB from offset zero in the WordDocument
 * Stream. */
	struct Fib *fib = &doc->fib;

/* 2. All versions of the FIB contain exactly one
 * FibRgFcLcb97 though it can be nested in a larger
 * structure. Read a STSH from offset FibRgFcLcb97.fcStshf
 * in the Table Stream with size
 * FibRgFcLcb97.lcbStshf. */
	struct STSH *STSH = doc->STSH;

/* 3. The given istd is a zero-based index into
 * STSH.rglpstd. Read an LPStd at STSH.rglpstd[istd]. */
	struct LPStd *LPStd = 
		LPStd_at_index(STSH->rglpstd, 
				doc->lrglpstd, istd);
	if (!LPStd){
		return;
	}

	if (LPStd->cbStd == 0){
		return;
	}

/* 4. Read the STD structure as LPStd.std, of length
 * LPStd.cbStd bytes. */
	BYTE STD[LPStd->cbStd];
	memcpy(STD, LPStd->STD, LPStd->cbStd);

/* 5. From the STD.stdf.stdfBase obtain istdBase. If
 * istdBase is any value other than 0x0FFF, then
 * this style is based on another style. Recursively apply
 * this algorithm using istdBase as the
 * starting istd to obtain one or more arrays of Prls as the
 * properties for tables, paragraphs and
 * characters from the base style. */
	struct StdfBase *stdfBase = (struct StdfBase *)STD;
	USHORT istdBase = StdfBaseIstdBase(stdfBase);
	if (istdBase != 0x0FFF){
		// recursion 
#ifdef DEBUG
	LOG("parent style: 0x%04x", istdBase);
#endif
		apply_style_properties(doc, istdBase);
	}

/* 6. From the STD.stdf.stdfBase obtain stk. For more
 * information, see the description of the cupx
 * member of StdfBase. Read an STD.grLPUpxSw. Based on the
 * stk, grLPUpxSw contains one of
 * the following structures: StkParaGRLPUPX, StkCharGRLPUPX,
 * StkTableGRLPUPX, StkListGRLPUPX.
 * 7. Each of the preceding structures contains one or more
 * of the following: LPUpxPapx, LPUpxChpx,
 * LPUpxTapx. Each of the latter structures leads to one or
 * more arrays of Prl that specify properties.
 * For more information, see the sections documenting these
 * structures for how to obtain these
 * arrays.
 * 8. For each array obtained in step 7 that specifies
 * properties of a table, paragraph, or characters,
 * append to the beginning of the corresponding array from
 * step 5, if any. The resulting arrays of Prl
 * are the desired output. Leave the algorithm. */
}
