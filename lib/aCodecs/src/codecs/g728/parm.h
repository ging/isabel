#ifndef PARM_H
#define PARM_H

#define IDIM	5	/* Size of Speech Vector */
#define NFRSZ	20	/* Frame Size */
#define NUPDATE	4	/* Predictor Update Period */

#define LPC	50	/* # of LPC Coeff. in Sinthesys Filter */
#define LPCLG	10	/* # of LPC Coeff. in Gain Predictor */
#define LPCW	10	/* # of LPC Coeff. in Weighting Filter */
#define NONR	35	/* Size of Nonrecursive Part of Synth. Adapter */ 
#define NONRLG	20	/* ------------------------- of Gain Adapter */
#define NONRW	30	/* ------------------------- of Weighting Filter */

#define AGCFAC 	0.99	/* Adaptive Gain Control FACtor */
#define DIMINV	0.2	/* Inverse if IDIM */
#define FAC    	(253.0/256.0)
#define FACGP	(29.0/32.0)
#define GOFF	32	/* Gain (Logarithmic) Offset */
#define KPDELTA	6
#define KPMIN	20	/* Min Pitch Period ( 400 Hz) */
#define KPMAX	140	/* Max Pitch Period (~ 57 Hz) */
#define NCWD	128	/* Shape Codebook Size */
#define NG	8	/* Gain Codebook Size */
#define NPWSZ	100	/* Pitch Predictor Window Size */
#define PPFTH	0.6
#define PPFZCF	0.15
#define SPFPCF	0.75
#define SPFZCF	0.65
#define TAPTH	0.4
#define TILTF	0.15
#define WNCF	(257.0/256.0)
#define WPCF	0.6
#define WZCF	0.9

#define BIG 10.e+30

#define MAX 4095.0
#define MIN (-4095.0)

#endif /*PARM_H*/
