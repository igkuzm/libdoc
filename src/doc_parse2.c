/**
 * File              : doc_parse2.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 07.08.2024
 * Last Modified Date: 07.08.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/libdoc/doc.h"
#include "../include/libdoc.h"

int doc_parse(const char *filename, void *user_data,
		int (*styles)(void *user_data, STYLE *s),
		int (*text)(void *user_data, DOC_PART part, ldp_t *p, int ch))
{
#ifdef DEBUG
	LOG("start");
#endif
	int i,   // iterator
			err; // error

	// get CFB
	struct cfb cfb;
	err = cfb_open(&cfb, filename);
	if (err)
		return err;
	
	// Read the DOC Streams
	cfb_doc_t doc;
	err	= doc_read(&doc, &cfb);
	if (err)
		return err;

	doc.prop.data = &doc;
	FibRgFcLcb97 *rgFcLcb97 = (FibRgFcLcb97 *)(doc.fib.rgFcLcb);

	/* MAIN_DOCUMENT */
	CP ccpText; // number of cp in MAIN_DOCUMENT
	
	/* MAIN_DOCUMENT consists of sections */
	int csect; //number of sections
	csect = doc.plcfSedNaCP;  

	// iterate sectors
	for (i = 0; i < csect; ++i) {
		CP sfcp,  // sector first CP
			 slcp;  // sector last CP

		sfcp = doc.plcfSed->aCP[i];
		slcp = doc.plcfSed->aCP[i+1];
		if (!slcp)
			slcp = ccpText;

		LOG("SECT %d, fcp: %d, lcp: %d", i, sfcp, slcp);

		/* SECTION consists of paragraphs */
		/* paragraph CP are in PlcPcd */
		struct PlcPcd *plcPcd = &(doc.clx.Pcdt->PlcPcd);
		int cpar; // number of paragraph in section
		LOG("plcPcd->aCPl: %d", plcPcd->aCPl);
		
		int k;
		for (k = 0; k < 10; ++k) {
			CP pfcp,  // paragraph first CP
				 plcp;  // paragraph last CP
			pfcp = plcPcd->aCp[k];
			plcp = plcPcd->aCp[k+1];
			LOG("\tPAR %d, fcp: %d, lcp: %d", k, pfcp, plcp);
			
		}
	}

#ifdef DEBUG
	LOG("done");
#endif
	return 0;
}
