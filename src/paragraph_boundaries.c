/**
 * File              : paragraph_boundaries.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 26.05.2024
 * Last Modified Date: 26.05.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/libdoc/doc.h"

/* 2.4.2 Determining Paragraph Boundaries
 * This section specifies how to find the beginning and end 
 * character positions of the paragraph that contains a 
 * given character position. The character at the end 
 * character position of a paragraph MUST be a paragraph 
 * mark, an end-of-section character, a cell mark, or a 
 * TTP mark (See Overview of Tables). Negative character 
 * positions are not valid. */ 

/* To find the character position of the first character 
 * in the paragraph that contains a given character 
 * position cp : */ 
CP first_cp_in_paragraph(cfb_doc_t *doc, CP cp)
{
	CP fcp = CPERROR;
	struct PapxFkp *papxFkp = NULL;

/* 1. Follow the algorithm from Retrieving Text up to and 
 * including step 3 to find i. Also remember the 
 * FibRgFcLcb97 and PlcPcd found in step 1 of Retrieving 
 * Text. If the algorithm from Retrieving Text specifies 
 * that cp is invalid, leave the algorithm. */
	FibRgFcLcb97 *fibRgFcLcb97 = (FibRgFcLcb97 *)(doc->fib.rgFcLcb);
	struct PlcPcd *plcPcd = &(doc->clx.Pcdt->PlcPcd);

	int i = 0;
		while (plcPcd->aCp[i] <= cp)
			i++;
		i--;	

  while(1){
/* 2. Let pcd be PlcPcd.aPcd[i]. */
		struct Pcd *pcd = &(plcPcd->aPcd[i]);

/* 3. Let fcPcd be Pcd.fc.fc. Let fc be 
 * fcPcd + 2(cp – PlcPcd.aCp[i]). If Pcd.fc.fCompressed is 
 * one, set fc to fc / 2, and set fcPcd to fcPcd/2. */
		//uint32_t fcPcd = FcValue(pcd->fc);
		uint32_t fcPcd = FcValue(pcd->fc);
		uint32_t fc = fcPcd + 2*(cp - plcPcd->aCp[i]); 
		if (FcCompressed(pcd->fc)){
			fcPcd /= 2;
			fc /= 2;
		}

/* 4. Read a PlcBtePapx at offset FibRgFcLcb97.fcPlcfBtePapx 
 * in the Table Stream, and of size FibRgFcLcb97.lcbPlcfBtePapx.
 * Let fcLast be the last element of plcbtePapx.aFc. */
	 
		uint32_t fcLast = 
			doc->plcbtePapx->aFc[doc->plcbtePapxNaFc-1];
		uint32_t fcFirst;

/* If fcLast is less than or equal to fc, examine fcPcd. 
 * If fcLast is less than fcPcd, go to step 8. Otherwise, 
 * set fc to fcLast. If Pcd.fc.fCompressed is one, set 
 * fcLast to fcLast / 2. Set fcFirst to fcLast and go 
 * to step 7. */
		if (fcLast <= fc) {
			if (fcLast < fcPcd){
				// goto 8
				goto first_cp_in_paragraph_8;
			} else {
					fc = fcLast;
					if (FcCompressed(pcd->fc))
						fcLast /= 2;
					fcFirst = fcLast;
					// goto 7
					goto first_cp_in_paragraph_7;
			}
		}
/* 5. Find the largest j such that plcbtePapx.aFc[j] ≤ fc.
 * Read a PapxFkp at offset
 * aPnBtePapx[j].pn *512 in the WordDocument Stream. */
		int j;
		for (j=0; doc->plcbtePapx->aFc[j] <= fc; )
			j++;	
		j--;

		papxFkp = papxFkp_get(
				doc->WordDocument, 
				pnFkpPapx_pn(
					doc->plcbtePapx->aPnBtePapx[j]) * 512);

/* 6. Find the largest k such that PapxFkp.rgfc[k] ≤ fc.
 * If the last element of PapxFkp.rgfc is less
 * than or equal to fc, then cp is outside the range of
 * character positions in this document, and is
 * not valid. Let fcFirst be PapxFkp.rgfc[k].*/
		int k;
		for (k=0; papxFkp->rgfc[k] <= fc; )
			k++;	
		k--;
		
		if (papxFkp->rgfc[*papxFkp->cpara] <= fc){
			ERR("last element of PapxFkp.rgfc is less"
					" than or equal to fc: cp is outside the"
					" range of character positions in this document");
			return CPERROR;
		}
		fcFirst = papxFkp->rgfc[k];

first_cp_in_paragraph_7:
/* 7. If fcFirst is greater than fcPcd, then let dfc be
 * (fcFirst – fcPcd). If Pcd.fc.fCompressed is
 * zero, then set dfc to dfc / 2. The first character of
 * the paragraph is at character position
 * PlcPcd.aCp[i] + dfc. Leave the algorithm. */
		if (fcFirst > fcPcd){
			uint32_t dfc = fcFirst - fcPcd;
			if (!FcCompressed(pcd->fc))
				dfc /= 2;
			fcp = plcPcd->aCp[i] + dfc;
			break;
		}

first_cp_in_paragraph_8:
/* 8. If PlcPcd.aCp[i] is 0, then the first character of
 * the paragraph is at character position 0. Leave
 * the algorithm */
		if (plcPcd->aCp[i] == 0){
			fcp = 0;
			break;
		}

/* 9. Set cp to PlcPcd.aCp[i]. Set i to i - 1. 
 * Go to step 2; */
		cp = plcPcd->aCp[i];
		i--;
	
		if (papxFkp)
			free(papxFkp);
		papxFkp = NULL;
	}

	if (papxFkp)
		free(papxFkp);
	return fcp;
}

/* To find the character position of the last character in
 * the paragraph that contains a given character
 * position cp: */
CP last_cp_in_paragraph(cfb_doc_t *doc, CP cp)
{
	CP lcp = CPERROR;
	struct PapxFkp *papxFkp = NULL;

/* 1. Follow the algorithm from Retrieving Text up to and
 * including step 3 to find i. Also remember the
 * FibRgFcLcb97, and PlcPcd found in step 1 of Retrieving
 * Text. If the algorithm from Retrieving
 * Text specifies that cp is invalid, leave the algorithm.
 */
	FibRgFcLcb97 *fibRgFcLcb97 = (FibRgFcLcb97 *)(doc->fib.rgFcLcb);
	struct PlcPcd *plcPcd = &(doc->clx.Pcdt->PlcPcd);
	
	int i;
	for(i=0; plcPcd->aCp[i] <= cp;)
		i++;
	i--;	

	while(1){
/* 2. Let pcd be PlcPcd.aPcd[i]. */
		struct Pcd *pcd = &(plcPcd->aPcd[i]);

/* 3. Let fcPcd be Pcd.fc.fc. Let fc be fcPcd + 2(cp –
 * PlcPcd.aCp[i]). Let fcMac be fcPcd +
 * 2(PlcPcd.aCp[i+1] - PlcPcd.aCp[i]). If Pcd.fc.fCompressed
 * is one, set fc to fc/2, set fcPcd to
 * fcPcd /2 and set fcMac to fcMac/2 */

		uint32_t fcPcd = FcValue(pcd->fc);
		uint32_t fc = fcPcd + 2 * (cp - plcPcd->aCp[i]);
		uint32_t fcMac = fcPcd + 2 * (plcPcd->aCp[i+1] - 
				plcPcd->aCp[i]);
		if (FcCompressed(pcd->fc)){
			fc /= 2;
			fcPcd /= 2;
			fcMac /= 2;
		}

/* 4. Read a PlcBtePapx at offset FibRgFcLcb97.fcPlcfBtePapx
 * in the Table Stream, and of size
 * FibRgFcLcb97.lcbPlcfBtePapx. Then find the largest j such
 * that plcbtePapx.aFc[j] ≤ fc. If the
 * last element of plcbtePapx.aFc is less than or equal to
 * fc, then go to step 7. Read a PapxFkp at
 * offset aPnBtePapx[j].pn *512 in the WordDocument Stream */
		
		int j;
		for (j=0; doc->plcbtePapx->aFc[j] <= fc; )
			j++;	
		j--;
		
		if (doc->plcbtePapx->aFc[doc->plcbtePapxNaFc-1] <= fc){
			// goto 7
			goto last_cp_in_paragraph_7;
		}
		
		papxFkp = papxFkp_get(
				doc->WordDocument, 
				pnFkpPapx_pn(
					pnFkpPapx_pn(
						doc->plcbtePapx->aPnBtePapx[j])) * 512);

/* 5. Find largest k such that PapxFkp.rgfc[k] ≤ fc. If the
 * last element of PapxFkp.rgfc is less than
 * or equal to fc, then cp is outside the range of character
 * positions in this document, and is not
 * valid. Let fcLim be PapxFkp.rgfc[k+1]. */
		int k;
		for (k=0; papxFkp->rgfc[k] <= fc; )
			k++;	
		k--;
		
		if (papxFkp->rgfc[*papxFkp->cpara] <= fc){
			ERR("last element of PapxFkp.rgfc is less"
					" than or equal to fc: cp is outside the"
					" range of character positions in this document");
			return CPERROR;
		}
		uint32_t fcLim = papxFkp->rgfc[k+1];
		
/* 6. If fcLim ≤ fcMac, then let dfc be (fcLim – fcPcd). If
 * Pcd.fc.fCompressed is zero, then set dfc
 * to dfc / 2. The last character of the paragraph is at
 * character position PlcPcd.aCp[i] + dfc – 1.
 * Leave the algorithm. */
		if (fcLim <= fcMac){
			uint32_t dfc = fcLim - fcPcd;
			if (!FcCompressed(pcd->fc)){
				dfc /= 2;
				lcp = plcPcd->aCp[i] + dfc - 1;
				break;
			}
		}
/* 7. Set cp to PlcPcd.aCp[i+1]. Set i to i + 1. 
 * Go to step 2.*/
last_cp_in_paragraph_7:
		cp = plcPcd->aCp[i+1];
		i++;
		if (papxFkp)
			free(papxFkp);
		papxFkp = NULL;
	}

	if (papxFkp)
		free(papxFkp);
	return lcp;
}
