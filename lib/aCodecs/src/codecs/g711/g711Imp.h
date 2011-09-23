/*
 * This source code is a product of Sun Microsystems, Inc. and is provided
 * for unrestricted use.  Users may copy or modify this source code without
 * charge.
 *
 * SUN SOURCE CODE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING
 * THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun source code is provided with no support and without any obligation on
 * the part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY THIS SOFTWARE
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

/*
 * g711.h
 *
 * u-law, A-law and linear PCM conversions.
 *
 * This file didn't exist originally. It has been created because there were
 * too many warnings in the code saying 'implicit declaration of function...'.
 * Now, we will use the #include directive to use this file and to avoid
 * such warnings.
 *
 * Sergio Fernández Muñoz (Tue Apr 25 11:05:10 EDT 2000)
 * sfernandez@agora-systems.com
 */

#ifndef __G711Imp_H__
#define __G711Imp_H__

unsigned char linear2alaw(short pcm_val);
short alaw2linear(unsigned char a_val);
unsigned char linear2ulaw(short pcm_val);
short ulaw2linear(unsigned char u_val);
unsigned char alaw2ulaw(unsigned char aval);
unsigned char ulaw2alaw(unsigned char uval);

#endif
