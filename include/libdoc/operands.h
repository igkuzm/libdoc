/**
 * File              : operands.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 28.05.2024
 * Last Modified Date: 29.05.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef OPERANDS_H
#define OPERANDS_H

#include "doc.h"
#include <stdint.h>
#include <stdbool.h>

/* The ToggleOperand structure is an operand to an SPRM
 * whose spra is 0 and whose sgc is 2. It
 * modifies a Boolean character property. */
static bool ToggleOperand(cfb_doc_t *doc, BYTE operand)
{
#ifdef DEBUG
	LOG("operand: 0x%02x", operand); 
#endif
/* value (1 byte): An unsigned integer which MUST be one of
 * the following values.
 * Value Meaning
 * 0x00 The Boolean property is set to 0, which means the
 * property is turned OFF.
 * 0x01 The Boolean property is set to 1, which means the
 * property is turned ON.
 * 0x80 The Boolean property is set to match the value of
 * the property in the current style that is applied to
 * the text.
 * 0x81 The Boolean property is set to the opposite of the
 * value of the property in the current style that is
 * applied to the text. */
	switch (operand) {
		case 0x00: return false;
		case 0x01: return true;
		case 0x80: 
			{
				/* TODO: current style check */
				return false;	
			}
		case 0x81: 
			{
				/* TODO: current style check */
				return true;	
			}
		default:
			ERR("wrong ToggleOperand: 0x%02x", operand);
			break;	
	}
	return false;
}

/* 2.9.119 Ico
 * The Ico structure specifies an entry in the color palette
 * that is listed in the following table. */
static const struct {BYTE b; COLOR c;} IcoValue[] =
{
	{0x00, {0x00, 0x00, 0x00}},
	{0x01, {0x00, 0x00, 0x00}},
	{0x02, {0x00, 0x00, 0xFF}},
	{0x03, {0x00, 0xFF, 0xFF}},
	{0x04, {0x00, 0xFF, 0x00}},
	{0x05, {0xFF, 0x00, 0xFF}},
	{0x06, {0xFF, 0x00, 0x00}},
	{0x07, {0xFF, 0xFF, 0x00}},
	{0x08, {0xFF, 0xFF, 0xFF}},
	{0x09, {0x00, 0x00, 0x80}},
	{0x0A, {0x00, 0x80, 0x80}},
	{0x0B, {0x00, 0x80, 0x00}},
	{0x0C, {0x80, 0x00, 0x80}},
	{0x0D, {0x80, 0x00, 0x80}},
	{0x0E, {0x80, 0x80, 0x00}},
	{0x0F, {0x80, 0x80, 0x80}},
	{0x10, {0xC0, 0xC0, 0xC0}},
};

static const COLOR *Ico(BYTE operand)
{
#ifdef DEBUG
	LOG("operand: 0x%02x", operand); 
#endif
	int i;
	for (i = 0; i < 17; ++i) {
		if (operand == IcoValue[i].b)
			return &(IcoValue[i].c);
	}
	ERR("no color with Ico: 0x%02x", operand);
	return NULL;
}
				
#endif /* ifndef OPERANDS_H */
