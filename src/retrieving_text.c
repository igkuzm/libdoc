/**
 * File              : retrieving_text.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 26.05.2024
 * Last Modified Date: 27.05.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "../include/libdoc/retrieving_text.h"

void get_char_for_cp(cfb_doc_t *doc, CP cp,
		void *user_data,
		int (*callback)(void *user_data, int ch)		
		)
{
	struct PlcPcd *PlcPcd = &(doc->clx.Pcdt->PlcPcd);

/* The Clx contains a Pcdt, and the Pcdt contains a PlcPcd.
 * Find the largest i such that PlcPcd.aCp[i] ≤ cp. As with
 * all Plcs, the elements of PlcPcd.aCp are sorted in
 * ascending order.  Recall from the definition of a Plc
 * that the aCp array has one more element than the aPcd
 * array.  Thus, if the last element of PlcPcd.aCp is less
 * than or equal to cp, cp is outside the range of valid
 * character positions in this document
 */
		int i = 0;
		while (PlcPcd->aCp[i] <= cp)
			i++;
		i--;	

/*
 * PlcPcd.aPcd[i] is a Pcd. Pcd.fc is an FcCompressed that
 * specifies the location in the WordDocument Stream of the
 * text at character position PlcPcd.aCp[i].
 */
	struct FcCompressed fc = PlcPcd->aPcd[i].fc;	
	if (FcCompressed(fc)){
/*
 * If FcCompressed.fCompressed is 1, the character at
 * position cp is an 8-bit ANSI character at offset
 * (FcCompressed.fc / 2) + (cp - PlcPcd.aCp[i]) in the
 * WordDocument Stream, unless it is one of the special
 * values in the table defined in the description of
 * FcCompressed.fc. This is 
 * to say that the text at character position PlcPcd.aCP[i]
 * begins at offset FcCompressed.fc / 2 in the WordDocument
 * Stream and each character occupies one byte.
*/			
		//ANSI
		DWORD off = (FcValue(fc) / 2) + (cp - PlcPcd->aCp[i]);
		fseek(doc->WordDocument, off, SEEK_SET);	
		int ch;
		fread(&ch, 1, 1, doc->WordDocument);			
		// check special chars
		int sch = FcCompressedSpecialChar_get(ch);
		if (sch)
			callback(user_data, sch);
		else
			callback(user_data, ch);

	} else {
/*
 * If FcCompressed.fCompressed is zero, the character at
 * position cp is a 16-bit Unicode character at offset
 * FcCompressed.fc + 2(cp - PlcPcd.aCp[i]) in the
 * WordDocument Stream. This is to say that the text at
 * character position PlcPcd.aCP[i] begins at offset
 * FcCompressed.fc in the 
 * WordDocument Stream and each character occupies two
 * bytes.
*/			
		//UNICODE 16
		DWORD off = FcValue(fc) + 2*(cp - PlcPcd->aCp[i]);
		fseek(doc->WordDocument, off, SEEK_SET);	
		WORD u;
		fread(&u, 2, 1, doc->WordDocument);
		if (doc->biteOrder){
			u = bo_16_sw(u);
		}
		char utf8[4]={0};
		//sprintf(utf8, "0x%x ", u);
		_utf16_to_utf8(&u, 1, utf8);
		if (doc->biteOrder){
				u = bo_16_sw(u);
			}
			if (u < 0x010){
				// first byte in uint16 is 00
				if (u > 0x1f && u < 0x7f) {
					//simple ANSI
					int ch;
					fread(&ch, 1, 1,
							doc->WordDocument);			
					callback(user_data, ch);
				} else {
					//this is a mark

		/* Following symbols below 32 are allowed inside paragraph:
			 0x0002 - footnote mark
			 0x0007 - table separator (converted to tabmode)
			 0x0009 - Horizontal tab ( printed as is)
			 0x000B - hard return
			 0x000C - page break
			 0x000D - return - marks an end of paragraph
			 0x001E - IS2 for some reason means short defis in Word.
			 0x001F - soft hyphen in Word
			 0x0013 - start embedded hyperlink
			 0x0014 - separate hyperlink URL from text
			 0x0015 - end embedded hyperlink
			 */

					switch (u) {
						case 0x0D: callback(user_data, '\n'); break;
						case 0x07: callback(user_data, '\n'); break;
						case 0x1E: callback(user_data, '-' ); break;
						case 0x09: callback(user_data, '\t'); break;
						case 0x13: callback(user_data, ' ' ); break;
						case 0x15: callback(user_data, ' ' ); break;
						case 0x0C: callback(user_data, u)   ; break;
						case 0x1F: callback(user_data, 0xAD); break;
						case 0x0B: callback(user_data, 0x0A); break;
						case 0x08: case 0x01: callback(user_data, ' '); break;
						default: break;
					}
				}
			} else if (u != 0xfeff) {
				char utf8[4]={0};
				_utf16_to_utf8(&u, 1, utf8);
				for (i = 0; i < 4; ++i) {
					callback(user_data, utf8[i]);
				}
			}
	}
}
