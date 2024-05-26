/**
 * File              : doc.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 26.05.2024
 * Last Modified Date: 26.05.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/libdoc/doc.h"

/* How to read the FIB
 * The Fib structure is located at offset 0 of the
 * WordDocument Stream. Given the variable size of
 * the Fib, the proper way to load it is the following:
 * 1.  Set all bytes of the in-memory version of the Fib
 * being used to 0. It is recommended to use 
 *     the largest version of the Fib structure as the
 *     in-memory version.
 * 2.  Read the entire FibBase, which MUST be present and
 * has fixed size.
 * 3.  Read Fib.csw.
 * 4.  Read the minimum of Fib.csw * 2 bytes and the size,
 * in bytes, of the in-memory version of 
 *     FibRgW97 into FibRgW97.
 * 5.  If the application expects fewer bytes than indicated
 * by Fib.csw, advance by the difference
 *     thereby skipping the unknown portion of FibRgW97.
 * 6.  Read Fib.cslw.
 * 7.  Read the minimum of Fib.cslw * 4 bytes and the size,
 * in bytes, of the in-memory version of
 *     FibRgLw97 into FibRgLw97.
 * 8.  If the application expects fewer bytes than indicated
 * by Fib.cslw, advance by the 
 *     difference thereby skipping the unknown portion of
 *     FibRgLw97.
 * 9.  Read Fib.cbRgFcLcb.
 * 10. Read the minimum of Fib.cbRgFcLcb * 8 bytes and the
 * size, in bytes, of the in-memory 
 *     version of FibRgFcLcb into FibRgFcLcb.
 * 11. If the application expects fewer bytes than indicated
 * by Fib.cbRgFcLcb, advance by the 
 *     difference, thereby skipping the unknown portion of
 *     FibRgFcLcb.
 * 12. Read Fib.cswNew.
 * 13. Read the minimum of Fib.cswNew * 2 bytes and the
 * size, in bytes, of the in-memory version 
 *     of FibRgCswNew into FibRgCswNew.*/
static int _doc_fib_init(Fib *fib, FILE *fp, struct cfb *cfb){
#ifdef DEBUG
	LOG("start _cfb_doc_fib_init\n");
#endif

	fib->base = NULL;
	fib->csw = 0;
	fib->rgW97 = NULL;
	fib->cslw = 0;
	fib->rgLw97 = NULL;
	fib->cbRgFcLcb = 0;
	fib->rgFcLcb = NULL;
	fib->cswNew = 0;
	fib->rgCswNew = NULL;

	//allocate fibbase
#ifdef DEBUG
	LOG("_cfb_doc_fib_init: allocate fibbase\n");
#endif
	
	fib->base = (FibBase *)malloc(32);
	if (!fib->base)
		return DOC_ERR_ALLOC;

	//read fibbase
#ifdef DEBUG
	LOG("_cfb_doc_fib_init: read fibbase\n");
#endif
	
	if (fread(fib->base, 32, 1, fp) != 1){
		free(fib->base);
		return DOC_ERR_FILE;
	}
	if (cfb->biteOrder){
		fib->base->wIdent        = bo_16_sw(fib->base->wIdent);
		fib->base->nFib          = bo_16_sw(fib->base->nFib);
		fib->base->lid           = bo_16_sw(fib->base->lid);
		fib->base->pnNext        = bo_16_sw(fib->base->pnNext);
		fib->base->ABCDEFGHIJKLM = bo_16_sw(fib->base->ABCDEFGHIJKLM);
		fib->base->nFibBack      = bo_16_sw(fib->base->nFibBack);
		fib->base->lKey          = bo_32_sw(fib->base->lKey);
	}
	
	//check wIdent
#ifdef DEBUG
	LOG("_cfb_doc_fib_init: check wIdent: 0x%x\n", fib->base->wIdent);
#endif	
	if (fib->base->wIdent != 0xA5EC){
		free(fib->base);
		return DOC_ERR_HEADER;
	}	

#ifdef DEBUG
	LOG("_cfb_doc_fib_init: read csw\n");
#endif	
	//read Fib.csw
	if (fread(&(fib->csw), 2, 1, fp) != 1){
		free(fib->base);
		return DOC_ERR_FILE;
	}
	if (cfb->biteOrder){
		fib->csw = bo_16_sw(fib->csw);
	}

	//check csw
#ifdef DEBUG
	LOG("_cfb_doc_fib_init: check csw: 0x%x\n", fib->csw);
#endif		
	if (fib->csw != 14) {
		free(fib->base);
		return DOC_ERR_HEADER;
	}

#ifdef DEBUG
	LOG("_cfb_doc_fib_init: allocate FibRgW97\n");
#endif	
	//allocate FibRgW97
	fib->rgW97 = (FibRgW97 *)malloc(28);
	if (!fib->rgW97){
		free(fib->base);
		return DOC_ERR_ALLOC;
	}

	//read FibRgW97
#ifdef DEBUG
	LOG("_cfb_doc_fib_init: read FibRgW97\n");
#endif
	if (fread(fib->rgW97, 28, 1, fp) != 1){
		free(fib->base);
		free(fib->rgW97);
		return DOC_ERR_FILE;
	}
	if (cfb->biteOrder){
		fib->rgW97->lidFE = bo_16_sw(fib->rgW97->lidFE);
	}

#ifdef DEBUG
	LOG("_cfb_doc_fib_init: read Fib.cslw\n");
#endif	
	//read Fib.cslw
	if (fread(&(fib->cslw), 2, 1, fp) != 1){
		free(fib->base);
		free(fib->rgW97);
		return DOC_ERR_FILE;
	}
	if (cfb->biteOrder){
		fib->cslw = bo_16_sw(fib->cslw);
	}

#ifdef DEBUG
	LOG("_cfb_doc_fib_init: check cslw: 0x%x\n", fib->cslw);
#endif	
	//check cslw
	if (fib->cslw != 22) {
		free(fib->base);
		free(fib->rgW97);
		return DOC_ERR_HEADER;
	}	

#ifdef DEBUG
	LOG("_cfb_doc_fib_init: allocate FibRgLw97\n");
#endif	
	//allocate FibRgLw97
	fib->rgLw97 = (FibRgLw97 *)malloc(88);
	if (!fib->rgLw97){
		free(fib->base);
		free(fib->rgW97);
		return DOC_ERR_ALLOC;
	}
	
#ifdef DEBUG
	LOG("_cfb_doc_fib_init: read Fib.FibRgLw97\n");
#endif	
	//read FibRgLw97
	if (fread(fib->rgLw97, 88, 1, fp) != 1){
		free(fib->base);
		free(fib->rgW97);
		free(fib->rgLw97);
		return DOC_ERR_FILE;
	}	
	if (cfb->biteOrder){
		fib->rgLw97->cbMac      = bo_32_sw(fib->rgLw97->cbMac);
		fib->rgLw97->ccpText    = bo_32_sw(fib->rgLw97->ccpText);
		fib->rgLw97->ccpFtn     = bo_32_sw(fib->rgLw97->ccpFtn);
		fib->rgLw97->ccpHdd     = bo_32_sw(fib->rgLw97->ccpHdd);
		fib->rgLw97->ccpAtn     = bo_32_sw(fib->rgLw97->ccpAtn);
		fib->rgLw97->ccpEdn     = bo_32_sw(fib->rgLw97->ccpEdn);
		fib->rgLw97->ccpTxbx    = bo_32_sw(fib->rgLw97->ccpTxbx);
		fib->rgLw97->ccpHdrTxbx = bo_32_sw(fib->rgLw97->ccpHdrTxbx);
	}
	
#ifdef DEBUG
	LOG("_cfb_doc_fib_init: read Fib.cbRgFcLcb\n");
#endif	
	//read Fib.cbRgFcLcb
	if (fread(&(fib->cbRgFcLcb), 2, 1, fp) != 1){
		free(fib->base);
		free(fib->rgW97);
		free(fib->rgLw97);
		return DOC_ERR_FILE;
	}
	if (cfb->biteOrder){
		fib->cbRgFcLcb = bo_16_sw(fib->cbRgFcLcb);
	}
	
#ifdef DEBUG
	LOG("cbRgFcLcb: 0x%x\n", fib->cbRgFcLcb);
#endif	

#ifdef DEBUG
	LOG("_cfb_doc_fib_init: allocate FibRgLw97 with size: %d\n", fib->cbRgFcLcb*8);
#endif	
	//allocate rgFcLcb
	fib->rgFcLcb = (uint32_t *)malloc(fib->cbRgFcLcb*8);
	if (!fib->rgFcLcb){
		free(fib->base);
		free(fib->rgW97);
		free(fib->rgLw97);
		return DOC_ERR_ALLOC;
	}	

#ifdef DEBUG
	LOG("_cfb_doc_fib_init: read Fib.rgFcLcb\n");
#endif	
	//read rgFcLcb
	if (fread(fib->rgFcLcb, 8, fib->cbRgFcLcb, fp) != fib->cbRgFcLcb){
		free(fib->base);
		free(fib->rgW97);
		free(fib->rgLw97);
		free(fib->rgFcLcb);
		return DOC_ERR_FILE;
	}	
	if (cfb->biteOrder){
		int i;
		for (i = 0; i < fib->cbRgFcLcb/4; ++i) {
			fib->rgFcLcb[i] = bo_32_sw(fib->rgFcLcb[i]);	
		}
	}

#ifdef DEBUG
	LOG("_cfb_doc_fib_init: read Fib.cswNew\n");
#endif	
	//read Fib.cswNew
	fread(&(fib->cswNew), 2, 1, fp);

#ifdef DEBUG
	LOG("cswNew: 0x%x\n", fib->cswNew);
#endif	
	if (cfb->biteOrder){
		fib->cswNew = bo_16_sw(fib->cswNew);
	}

	if (fib->cswNew > 0){
#ifdef DEBUG
	LOG("_cfb_doc_fib_init: allocate FibRgCswNew with size: %d\n", fib->cswNew * 2);
#endif		
		//allocate FibRgCswNew
		fib->rgCswNew = (FibRgCswNew *)malloc(fib->cswNew * 2);
		if (!fib->rgFcLcb){
			free(fib->base);
			free(fib->rgW97);
			free(fib->rgLw97);
			free(fib->rgFcLcb);
			return DOC_ERR_ALLOC;
		}	

#ifdef DEBUG
	LOG("_cfb_doc_fib_init: read FibRgCswNew\n");
#endif		
		//read FibRgCswNew
		if (fread(fib->rgCswNew, 2, fib->cswNew, fp) != fib->cswNew){
			free(fib->base);
			free(fib->rgW97);
			free(fib->rgLw97);
			free(fib->rgFcLcb);
			return DOC_ERR_FILE;
		}	
		if (cfb->biteOrder){
			fib->rgCswNew->nFibNew = bo_16_sw(fib->rgCswNew->nFibNew);
			int i;
			for (i = 0; i < 4; ++i) {
				fib->rgCswNew->rgCswNewData[i] = bo_16_sw(fib->rgCswNew->rgCswNewData[i]);
			}
		}
	}
#ifdef DEBUG
	LOG("_cfb_doc_fib_init done\n");
#endif	
	return 0;
};

static FILE *_table_stream(cfb_doc_t *doc, struct cfb *cfb){
	char *table = (char *)"0Table";
	Fib *fib = &doc[0].fib;
	if (FibBaseG(fib[0].base))
		table = (char *)"1Table";
#ifdef DEBUG
	LOG("_table_stream: table: %s\n", table);
#endif	
	return cfb_get_stream(cfb, table);
}

int _plcpcd_init(struct PlcPcd * PlcPcd, uint32_t len, cfb_doc_t *doc){
#ifdef DEBUG
	LOG("start _plcpcd_init\n");
#endif	
	
	int i;

	//get lastCP
	uint32_t lastCp = 
			doc->fib.rgLw97->ccpFtn +
			doc->fib.rgLw97->ccpHdd +
			doc->fib.rgLw97->reserved3 + //Mcr
			doc->fib.rgLw97->ccpAtn +
			doc->fib.rgLw97->ccpEdn +
			doc->fib.rgLw97->ccpTxbx +
			doc->fib.rgLw97->ccpHdrTxbx;
	
	if (lastCp)
		lastCp += 1 + doc->fib.rgLw97->ccpText;
	else
		lastCp = doc->fib.rgLw97->ccpText;

#ifdef DEBUG
	LOG("_plcpcd_init: lastCp: %d\n", lastCp);
#endif	

#ifdef DEBUG
	LOG("_plcpcd_init: allocate aCP\n");
#endif	
	//allocate aCP
	PlcPcd->aCp = (uint32_t *)malloc(4);
	if (!PlcPcd->aCp){
		free(PlcPcd);	
		return -1;
	}

	//read aCP
	i=0;
	uint32_t ch;
	while(fread(&ch, 4, 1, doc->Table) == 1){
		if (doc->biteOrder){
			ch = bo_32_sw(ch);
		}
		PlcPcd->aCp[i] = ch;
#ifdef DEBUG
	LOG("_plcpcd_init: aCp[%d]: %u\n", i, PlcPcd->aCp[i]);
#endif		
		i++;
		if (ch == lastCp)
			break;

		//realloc aCp
#ifdef DEBUG
	LOG("_plcpcd_init: realloc aCP with size: %ld\n", (i+1)*4);
#endif
		void *ptr = realloc(PlcPcd->aCp, (i+1)*4);
		if(!ptr)
			break;
		PlcPcd->aCp = (uint32_t *)ptr;
	}
#ifdef DEBUG
	LOG("_plcpcd_init: number of cp in array: %d\n", i);
#endif	
	//number of cp in array
	PlcPcd->aCPl = i;

	//read PCD - has 64bit
	uint32_t size = len - i*4;
	
	//number of Pcd in array
	PlcPcd->aPcdl = size / 8;
#ifdef DEBUG
	LOG("_plcpcd_init: number of Pcd in array: %d\n", PlcPcd->aPcdl);
#endif	
	
#ifdef DEBUG
	LOG("_plcpcd_init: allocate aPcd with size: %ld\n", size);
#endif
	PlcPcd->aPcd = (struct Pcd *)malloc(size);
	if (!PlcPcd->aPcd){
		free(PlcPcd->aCp);	
		free(PlcPcd);	
		return -1;
	}

	// get Pcd array
	for (i = 0; i < PlcPcd->aPcdl; ++i) {
		uint64_t ch;
		struct Pcd Pcd;
		fread(&Pcd.ABCfR2, 2, 1, doc->Table);
		fread(&Pcd.fc.fc, 4, 1, doc->Table);
		fread(&Pcd.prm, 2, 1, doc->Table);
		if (doc->biteOrder){
			Pcd.ABCfR2 = bo_16_sw(Pcd.ABCfR2);
			Pcd.fc.fc = bo_32_sw(Pcd.fc.fc);
			Pcd.prm = bo_16_sw(Pcd.prm);
		}

#ifdef DEBUG
		LOG("_plcpcd_init: PlcPcd->aPcd[%d]: ABCfR2: 0x%x, FC: %d, PRM: 0x%x\n", i, Pcd.ABCfR2, Pcd.fc.fc, Pcd.prm);
#endif	
		PlcPcd->aPcd[i] = Pcd;
	}

	
#ifdef DEBUG
	LOG("_plcpcd_init done\n");
#endif	

	return 0;
}

/* init CLX structure */
static int _clx_init(cfb_doc_t *doc)
{

#ifdef DEBUG
	LOG("start _clx_init\n");
#endif

	//get CLX
	//All versions of the FIB contain exactly one FibRgFcLcb97 
	FibRgFcLcb97 *rgFcLcb97 = (FibRgFcLcb97 *)(doc->fib.rgFcLcb);
	
	//FibRgFcLcb97.fcClx specifies the offset in the Table Stream of a Clx
	uint32_t fcClx = rgFcLcb97->fcClx;
	if (doc->biteOrder)
		fcClx = bo_32_sw(fcClx);
#ifdef DEBUG
	LOG("fcClx: %d\n", fcClx);
#endif
	
	//FibRgFcLcb97.lcbClx specifies the size, in bytes, of that Clx
	uint32_t lcbClx = rgFcLcb97->lcbClx;
	if (doc->biteOrder)
		lcbClx = bo_32_sw(lcbClx);
#ifdef DEBUG
	LOG("lcbClx: %d\n", lcbClx);
#endif	

	struct Clx *clx = &doc->clx;
	
	//get clx
	uint8_t ch;
	fseek(doc->Table, fcClx, SEEK_SET);
	fread(&ch, 1, 1, doc->Table);
#ifdef DEBUG
	LOG("_clx_init: first bite of CLX: 0x%x\n", ch);
#endif

	if (ch == 0x01){ //we have RgPrc (Prc array)
#ifdef DEBUG
	LOG("_clx_init: we have RgPrc (Prc array)\n");
#endif		
		//allocate RgPrc
#ifdef DEBUG
	LOG("_clx_init: allocate RgPrc\n");
#endif
		clx->RgPrc = (struct Prc *)malloc(sizeof(struct Prc));
		if (!clx->RgPrc)
			return DOC_ERR_ALLOC;
		
		int16_t cbGrpprl; //the first 2 bite of PrcData - signed integer
		fread(&cbGrpprl, 2, 1, doc->Table);
		if (doc->biteOrder){
			cbGrpprl = bo_16_sw(cbGrpprl);
		}
#ifdef DEBUG
	LOG("_clx_init: the first 2 bite of PrcData is cbGrpprl: 0x%x\n", cbGrpprl);
#endif		
		if (cbGrpprl > 0x3FA2) //error
			return DOC_ERR_FILE;		
		//allocate RgPrc->data 
#ifdef DEBUG
	LOG("_clx_init: allocate RgPrc->data\n");
#endif
		clx->RgPrc->data = (struct PrcData *)malloc(sizeof(struct PrcData));
		if (!clx->RgPrc->data)
			return DOC_ERR_ALLOC;

		clx->RgPrc->data->cbGrpprl = cbGrpprl;

		//allocate GrpPrl
#ifdef DEBUG
	LOG("_clx_init: allocate GrpPrl with size: 0x%x\n", cbGrpprl);
#endif		
		clx->RgPrc->data->GrpPrl = (struct Prl *)malloc(cbGrpprl);
		if (!clx->RgPrc->data->GrpPrl)
			return DOC_ERR_ALLOC;
		
		//read GrpPrl
#ifdef DEBUG
	LOG("_clx_init: read GrpPrl\n");
#endif		
		fread(clx->RgPrc->data->GrpPrl, cbGrpprl, 1, doc->Table);
		/* TODO:  parse GrpPrl + byteOrder */

		//read ch again
#ifdef DEBUG
	LOG("_clx_init: again first bite of CLX: 0x%x\n", ch);
#endif		
		fread(&ch, 1, 1, doc->Table);
	}	

	//get PlcPcd
#ifdef DEBUG
	LOG("_clx_init: allocate PlcPcd\n");
#endif
	clx->Pcdt = (struct Pcdt *)malloc(sizeof(struct Pcdt));
	if (!clx->Pcdt)
		return DOC_ERR_ALLOC;	

	//read Pcdt->clxt - this must be 0x02
	clx->Pcdt->clxt = ch;
#ifdef DEBUG
	LOG("_clx_init: Pcdt->clxt: 0x%x\n", clx->Pcdt->clxt);
#endif	
	if (clx->Pcdt->clxt != 0x02) { //some error
		return DOC_ERR_FILE;		
	}

	//read lcb;
	fread(&(clx->Pcdt->lcb), 4, 1, doc->Table);	
	if (doc->biteOrder){
		clx->Pcdt->lcb = bo_32_sw(clx->Pcdt->lcb);
	}
#ifdef DEBUG
	LOG("_clx_init: Pcdt->lcb: %d\n", clx->Pcdt->lcb);
#endif	

	//get PlcPcd
	_plcpcd_init(&(clx->Pcdt->PlcPcd), clx->Pcdt->lcb, doc);
	
#ifdef DEBUG
	LOG("_clx_init: aCP: %d, PCD: %d\n", clx->Pcdt->PlcPcd.aCPl, clx->Pcdt->PlcPcd.aPcdl);
#endif		

#ifdef DEBUG
	LOG("_clx_init done\n");
#endif
	
	return 0;
}

static int _doc_plcBtePapx_init(cfb_doc_t *doc){
	FibRgFcLcb97 *fibRgFcLcb97 = 
		(FibRgFcLcb97 *)(doc->fib.rgFcLcb);
	doc->plcbtePapx = 
			plcbtePapx_get(
					doc->Table, 
					fibRgFcLcb97->fcPlcfBtePapx,
					fibRgFcLcb97->lcbPlcfBtePapx, 
					&doc->plcbtePapxNaFc); 
	if (!doc->plcbtePapx){
		ERR("can't read PlcBtePapx");
		return -1;
	}
	return 0;
}

int doc_read(cfb_doc_t *doc, struct cfb *cfb){
#ifdef DEBUG
	LOG("start cfb_doc_init\n");
#endif

	memset(doc, 0, sizeof(cfb_doc_t));
	
	int ret = 0;
	//get byte order
	doc->biteOrder = cfb->biteOrder;
	
	//get WordDocument
	FILE *fp = cfb_get_stream(cfb, (char*)"WordDocument");
	if (!fp)	
		return DOC_ERR_FILE;
	fseek(fp, 0, SEEK_SET);
	doc->WordDocument = fp;

	//init FIB
	_doc_fib_init(&(doc->fib), doc->WordDocument, cfb);

	//get table
	doc->Table = _table_stream(doc, cfb);
	if (!doc->Table){
		//printf("Can't get Table stream\n"); 
		return DOC_ERR_FILE;
	}

	//Read the Clx from the Table Stream
	ret = _clx_init(doc);
	if (ret)
		return ret;	

	// get PlcBtePapx
	ret = _doc_plcBtePapx_init(doc);
	if (ret)
		return ret;	

#ifdef DEBUG
	LOG("cfb_doc_init done\n");
#endif	
	return 0;
}

void doc_close(cfb_doc_t *doc)
{
/* TODO: free memory and close streams */
}
