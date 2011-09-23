/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * This file is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify this file without charge, but are not authorized to
 * license or distribute it to anyone else except as part of a product
 * or program developed by the user.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * This file is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY THIS FILE
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even
 * if Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

/*
 * Defines and structures used only within the driver, isasg.c
 */

#pragma	ident	"@(#)isasg_def.h 1.8	94/06/21 SMI"

#ifndef	_ISASG_DEF_H
#define	_ISASG_DEF_H

#ifdef	__cplusplus
extern "C" {
#endif

#if	defined(_KERNEL) || defined(_KMEMUSER)

/*
 * Driver compile options
 */

/*
 * This driver does not reset the SCSI bus. Instead we just give up
 * and complain if the target hangs. If the bus is really stuck, one
 * of the Sun drivers (e.g. sd) will reset it. If you decide that
 * your device is critical and you should in fact reset the bus in
 * this driver, turn on this define below:
#define	SCSI_BUS_RESET
 */

/*
 * Local definitions, for clarity of code
 */
#define	ISASG_MUTEX(t)	(&((t)->targ_devp)->sd_mutex)
#define	ROUTE(t)	(&((t)->targ_devp)->sd_address)

#define	SCBP(pkt)	((struct scsi_status *)(pkt)->pkt_scbp)
#define	SCBP_C(pkt)	((*(pkt)->pkt_scbp) & STATUS_MASK)
#define	CDBP(pkt)	((union scsi_cdb *)(pkt)->pkt_cdbp)

#define	ISASG_CE_DEBUG1	((1 << 8) | CE_CONT)
#define	ISASG_CE_DEBUG2	((2 << 8) | CE_CONT)
#define	ISASG_CE_DEBUG3	((3 << 8) | CE_CONT)
#define	ISASG_CE_DEBUG4	((4 << 8) | CE_CONT)
#define	ISASG_LOG		if (isasg_debug) isasg_log
#define	ISASG_DEBUG_ENTER	if (isasg_debug) debug_enter


/*
 * Private info for scsi targets.
 *
 * Pointed to by the un_private pointer
 * of one of the SCSI_DEVICE structures.
 */
struct scsi_target {
	struct scsi_pkt	*targ_rqs;	/* ptr to request sense command pkt */
	struct scsi_pkt	*targ_pkt;	/* ptr to current command pkt */
	struct	buf	*targ_sbufp;	/* for use in special io */
	kcondvar_t	targ_sbuf_cv;	/* conditional variable for sbufp */
	kcondvar_t	targ_pkt_cv;	/* conditional variable for pkt */
	int		targ_sbuf_busy;	/* Wait Variable */
	int		targ_pkt_busy;	/* Wait Variable */
	int		targ_retry_ct;	/* retry count */
	u_int		targ_state;	/* current state */
	u_int		targ_arq;	/* ARQ mode on this tgt */
	struct scsi_device *targ_devp; /* back pointer to SCSI_DEVICE */
	struct buf	*targ_rqbp;	/* buf for Request Sense packet */
};

struct isasg_private {
	struct buf 	*priv_bp;	/* bp associated with this packet */
	/*
	 * To handle partial DMA mappings, target may need several
	 * SCSI commands to satisfy packet.  Keep track of remaining
	 * data in this packet in the following two fields.
	 */
	u_int		priv_amt;	/* bytes requested in this chunk */
	u_int		priv_amtdone;	/* bytes done so far in current pkt */
};


/*
 * Driver states
 */
#define	ISASG_STATE_NIL		0
#define	ISASG_STATE_CLOSED	1
#define	ISASG_STATE_OPEN		2

/*
 * Parameters
 */

#define	ISASG_IO_TIME	30	/* default command timeout, 30sec */

/*
 * 5 seconds is what we'll wait if we get a Busy Status back
 */
#define	ISASG_BSY_TIMEOUT		(drv_usectohz(5 * 1000000))

/*
 * Number of times we'll retry a normal operation.
 *
 * This includes retries due to transport failure
 * (need to distinguish between Target and Transport failure)
 */
#define	ISASG_RETRY_COUNT		30

/*
 * isasg_callback action codes
 */
#define	COMMAND_DONE		0
#define	COMMAND_DONE_ERROR	1
#define	QUE_COMMAND		2
#define	QUE_SENSE		3
#define	JUST_RETURN		4
#define	CONTINUE_PKT		5

/*
 * Special pkt flag just for this driver.
 * NB: Other pkt_flags defines are in scsi_pkt.h.
 */
#define	FLAG_SENSING    0x0400  /* Running request sense for failed pkt */

#endif	/* defined(_KERNEL) || defined(_KMEMUSER) */

/*
 * Ioctl commands
 */
#define	ISASGIOC		('S' << 8)
#define	ISASGIOC_READY	(ISASGIOC|0)	/* Send a Test Unit Ready command */
#define	ISASGIOC_ERRLEV	(ISASGIOC|1)	/* Set Error Reporting level */

#ifdef	__cplusplus
}
#endif

#endif	/* _ISASG_DEF_H */
