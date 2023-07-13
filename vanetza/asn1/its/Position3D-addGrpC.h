/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "AddGrpC"
 * 	found in "build.asn1/iso/ISO19091.asn"
 * 	`asn1c -fcompound-names -fincludes-quoted -no-gen-example -R`
 */

#ifndef	_Position3D_addGrpC_H_
#define	_Position3D_addGrpC_H_


#include "asn_application.h"

/* Including external dependencies */
#include "Altitude.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Position3D-addGrpC */
typedef struct Position3D_addGrpC {
	Altitude_t	 altitude;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Position3D_addGrpC_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Position3D_addGrpC;
extern asn_SEQUENCE_specifics_t asn_SPC_Position3D_addGrpC_specs_1;
extern asn_TYPE_member_t asn_MBR_Position3D_addGrpC_1[1];

#ifdef __cplusplus
}
#endif

#endif	/* _Position3D_addGrpC_H_ */
#include "asn_internal.h"
