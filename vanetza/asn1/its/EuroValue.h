/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "EfcDsrcApplication"
 * 	found in "build.asn1/iso/ISO14906-0-6.asn"
 * 	`asn1c -fcompound-names -fincludes-quoted -no-gen-example -R`
 */

#ifndef	_EuroValue_H_
#define	_EuroValue_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeEnumerated.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum EuroValue {
	EuroValue_noEntry	= 0,
	EuroValue_euro_1	= 1,
	EuroValue_euro_2	= 2,
	EuroValue_euro_3	= 3,
	EuroValue_euro_4	= 4,
	EuroValue_euro_5	= 5,
	EuroValue_euro_6	= 6,
	EuroValue_reservedForUse1	= 7,
	EuroValue_reservedForUse2	= 8,
	EuroValue_reservedForUse3	= 9,
	EuroValue_reservedForUse4	= 10,
	EuroValue_reservedForUse5	= 11,
	EuroValue_reservedForUse6	= 12,
	EuroValue_reservedForUse7	= 13,
	EuroValue_reservedForUse8	= 14,
	EuroValue_eev	= 15
} e_EuroValue;

/* EuroValue */
typedef long	 EuroValue_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_EuroValue_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_EuroValue;
extern const asn_INTEGER_specifics_t asn_SPC_EuroValue_specs_1;
asn_struct_free_f EuroValue_free;
asn_struct_print_f EuroValue_print;
asn_constr_check_f EuroValue_constraint;
ber_type_decoder_f EuroValue_decode_ber;
der_type_encoder_f EuroValue_encode_der;
xer_type_decoder_f EuroValue_decode_xer;
xer_type_encoder_f EuroValue_encode_xer;
jer_type_encoder_f EuroValue_encode_jer;
oer_type_decoder_f EuroValue_decode_oer;
oer_type_encoder_f EuroValue_encode_oer;
per_type_decoder_f EuroValue_decode_uper;
per_type_encoder_f EuroValue_encode_uper;
per_type_decoder_f EuroValue_decode_aper;
per_type_encoder_f EuroValue_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _EuroValue_H_ */
#include "asn_internal.h"
