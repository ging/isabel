/* Copyright (C) 1992 Eric Young - see COPYING for more details */

#ifndef DES_DEFS
#define DES_DEFS


#define	CRYPTO_KEY	"Copyright (c) 1993,1994 Isabel Development Team (DIT/UPM)"


typedef unsigned char des_cblock[8];
typedef struct des_ks_struct
	{
	union	{
		des_cblock _;
		/* make sure things are correct size on machines with
		 * 8 byte longs */
		unsigned long pad[2];
		} ks;
#define _	ks._
	} des_key_schedule[16];

#define DES_KEY_SZ 	(sizeof(des_cblock))
#define DES_ENCRYPT	1
#define DES_DECRYPT	0

#define DES_CBC_MODE	0
#define DES_PCBC_MODE	1
extern int des_rw_mode;

#define C_Block des_cblock
#define Key_schedule des_key_schedule
#define ENCRYPT DES_ENCRYPT
#define DECRYPT DES_DECRYPT
#define KEY_SZ DES_KEY_SZ
#define string_to_key des_string_to_key
#define read_pw_string des_read_pw_string
#define random_key des_random_key
#define pcbc_encrypt des_pcbc_encrypt
#define set_key des_set_key
#define key_sched des_key_sched
#define cbc_encrypt des_cbc_encrypt
#define cbc_cksum des_cbc_cksum
#define quad_cksum des_quad_cksum

/* For compatibility with the MIT lib - eay 20/05/92 */
typedef struct des_ks_struct bit_64;

#ifndef KERBEROS
int des_read_password();
int des_string_to_key();
int des_read_pw_string();
int des_random_key();
int des_set_key();
int des_key_sched();
int des_ecb_encrypt();
int des_cbc_encrypt();
int des_pcbc_encrypt();
int des_enc_read();
int des_enc_write();
#endif
unsigned long des_cbc_cksum();
unsigned long des_quad_cksum();
char *crypt();

extern int des_check_key; /* default is false */
#endif
