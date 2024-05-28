/**
 * File              : apply_properties.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 28.05.2024
 * Last Modified Date: 29.05.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/libdoc/apply_properties.h"
#include "../include/libdoc/style_properties.h"
#include "../include/libdoc/operands.h"
#include <stdint.h>

static int apply_char_property(cfb_doc_t *doc, struct Prl *prl);
static int apply_paragraph_property(cfb_doc_t *doc, struct Prl *prl);
static int apply_picture_property(cfb_doc_t *doc, struct Prl *prl);
static int apply_section_property(cfb_doc_t *doc, struct Prl *prl);
static int apply_table_property(cfb_doc_t *doc, struct Prl *prl);

int apply_property(cfb_doc_t *doc, struct Prl *prl)
{
	uint8_t sgc = SprmSgc(prl->sprm);
#ifdef DEBUG
	LOG("sgc: 0x%02x", sgc); 
#endif
	switch (sgc) {
		case sgcCha:
			return apply_char_property(doc, prl);
		case sgcPar:
			return apply_paragraph_property(doc, prl);
		case sgcSec:
			return apply_section_property(doc, prl);
		case sgcTab:
			return apply_table_property(doc, prl);
		case sgcPic:
			return apply_picture_property(doc, prl);
		
		default:
			break;
	}

	return 0;
}

int apply_char_property(
		cfb_doc_t *doc, struct Prl *prl)
{
	uint16_t ismpd = SprmIspmd(prl->sprm);
#ifdef DEBUG
	LOG("ismpd: 0x%04x", ismpd); 
#endif

	// set bold
	if (ismpd == sprmCFBold){
		doc->prop.chp.fBold = 
			ToggleOperand(doc, prl->operand[0]); 
		return 0;
	}
	
	// set italic
	if (ismpd == sprmCFItalic){
		doc->prop.chp.fItalic = 
			ToggleOperand(doc, prl->operand[0]); 
		return 0;
	}
	
	// set outline
	if (ismpd == sprmCFOutline){
		doc->prop.chp.fUnderline = 
			ToggleOperand(doc, prl->operand[0]); 
		return 0;
	}

	// background color
	if (ismpd == sprmCHighlight){
		const COLOR *c = Ico(prl->operand[0]);
		if (c){
			int rgb;
			int r = c->red;
			int g = c->green;
			int b = c->blue;
			rgb = (r << 24) + (g << 16) + (b << 8);
			doc->prop.chp.bcolor = rgb; 
		}
	}

#ifdef DEBUG
	LOG("no rule to parse ismpd: 0x%04x", ismpd); 
#endif
	return 1;
}

int apply_paragraph_property(
		cfb_doc_t *doc, struct Prl *prl)
{
	uint16_t ismpd = SprmIspmd(prl->sprm);
#ifdef DEBUG
	LOG("ismpd: 0x%04x", ismpd); 
#endif

	if (ismpd == sprmPIstd){
		uint16_t *istd = (uint16_t *)prl->operand;
#ifdef DEBUG
	LOG("paragraph istd: %d", *istd); 
#endif
		apply_style_properties(doc, *istd);
	}


#ifdef DEBUG
	LOG("no rule to parse ismpd: 0x%04x", ismpd); 
#endif
	return 1;
}

int apply_section_property(
		cfb_doc_t *doc, struct Prl *prl)
{
	uint16_t ismpd = SprmIspmd(prl->sprm);
#ifdef DEBUG
	LOG("ismpd: 0x%04x", ismpd); 
#endif


#ifdef DEBUG
	LOG("no rule to parse ismpd: 0x%04x", ismpd); 
#endif
	return 1;
}

int apply_table_property(
		cfb_doc_t *doc, struct Prl *prl)
{
	uint16_t ismpd = SprmIspmd(prl->sprm);
#ifdef DEBUG
	LOG("ismpd: 0x%04x", ismpd); 
#endif


#ifdef DEBUG
	LOG("no rule to parse ismpd: 0x%04x", ismpd); 
#endif
	return 1;
}

int apply_picture_property(
		cfb_doc_t *doc, struct Prl *prl)
{
	uint16_t ismpd = SprmIspmd(prl->sprm);
#ifdef DEBUG
	LOG("ismpd: 0x%04x", ismpd); 
#endif


#ifdef DEBUG
	LOG("no rule to parse ismpd: 0x%04x", ismpd); 
#endif
	return 1;
}
