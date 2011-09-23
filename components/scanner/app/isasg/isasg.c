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
 * isasg.c - Simple SCSI Target driver; a template character SCSA target
 *	   driver for Solaris 2.x.
 *
 * This file is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify this file without charge, but are not authorized to
 * license or distribute it to anyone else except as part of a product
 * or program developed by the user.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
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
 *
 * ------------------------------------------------------------------
 *
 * This driver is intended as an example of programming a SCSA
 * target driver for Solaris 2.X; it is not intended for any particular
 * device.
 *
 * Areas where you may need to change this code or add your own to
 * deal with your specific device are marked "Note".
 * Other warnings are marked "WARNING"
 *
 * To compile:
 *	% cc -D_KERNEL -c isasg.c
 *	% ld -r isasg.o -o isasg
 *
 * To install:
 * 1. Copy the module (isasg) and config file (isasg.conf) into /usr/kernel/drv
 * 2. Add an entry to /etc/devlink.tab, of the form:
 *	"type=sample_driver;name=isasg;minor=character        risasg\A1"
 *    This will cause devlinks(1M) to create link(s) to /devices with
 *    names of the form "/dev/risasgX" where X is the SCSI target number.
 * 3. Run add_drv(1M).
 *
 * Setting variables
 *	Variables can be explicitly set from the /etc/system file, by
 *	adding an entry of the form
 *		"set isasg:<variable name>=<value>"
 *	Alternatively, you can use adb to set variables and debug as
 *	follows:
 *		# adb -kw /dev/ksyms /dev/mem
 *	The /etc/system file is read only once at boot time, if you change
 *	it you must reboot for the change to take effect.
 */

#pragma	ident	"@(#)isasg.c 1.14	95/12/01 SMI"

/*
 * Includes, Declarations and Local Data
 */

#include <sys/scsi/scsi.h>
#include <sys/modctl.h>
#include <sys/stat.h>
#include <sys/ddi.h>
#include <sys/sunddi.h>
#include "isasg_def.h"

/*
 * Local Function Prototypes
 */
static int isasg_open(dev_t *dev_p, int flag, int otyp, cred_t *cred_p);
static int isasg_close(dev_t dev, int flag, int otyp, cred_t *cred_p);
static int isasg_read(dev_t dev, struct uio *uio, cred_t *cred_p);
static int isasg_write(dev_t dev, struct uio *uio, cred_t *cred_p);
static int isasg_ioctl(dev_t, int, int, int, cred_t *, int *);

static int isasg_strategy(struct buf *bp);
static int isasg_ioctl_cmd(dev_t, struct uscsi_cmd *, enum uio_seg, enum uio_seg);

static int isasg_unit_ready(dev_t dev);
static void isasg_done(struct scsi_target *targ, struct buf *bp);
static void isasg_restart(caddr_t arg);
static void isasg_callback(struct scsi_pkt *pkt);
static struct scsi_pkt *isasg_make_cmd(struct scsi_target *targ, struct buf *bp,
    struct scsi_pkt *pktp);
static void isasg_fill_cdb(struct scsi_pkt *pkt, struct scsi_target *targ,
    struct buf *bp, long flags);
static int isasg_handle_incomplete(struct scsi_target *targ);
static int isasg_handle_arq(struct scsi_pkt *pktp, struct scsi_target *targ,
    struct buf *bp);
static int isasg_handle_sense(struct scsi_target *targ, struct buf *bp);
static int isasg_check_error(struct scsi_target *targ, struct buf *bp);
static void isasg_log(struct scsi_device *devp, int level, const char *fmt, ...);
static void hex_print(char *msg, char *cp, int len);

/*
 * Local Static Data
 */
static int isasg_io_time		= ISASG_IO_TIME;
static int isasg_retry_count	= ISASG_RETRY_COUNT;
static void *isasg_state;

/*
 * Errors at or above this level will be reported
 */
static int isasg_error_reporting = SCSI_ERR_RETRYABLE;

/*
 * Enable the driver debugging code if DEBUG is defined (DEBUG also enables
 * other debugging code, e.g. ASSERT statements).
 */
#ifdef	DEBUG
#define	ISASG_DEBUG
#endif	DEBUG


/*
 * Debug message control
 * Debug Levels:
 *	0 = no messages
 *	1 = Errors
 *	2 = Subroutine calls & control flow
 *	3 = I/O Data (verbose!)
 * Can be set with adb or in the /etc/system file with
 * "set isasg:isasg_debug=<value>"
 * turn on diagnostics if DEBUG is defined (DEBUG also enables
 * other debugging code, e.g. ASSERT statements).
 */

#ifdef	ISASG_DEBUG
static int isasg_debug = 3;
#else
static int isasg_debug = 0;
#endif /* ISASG_DEBUG */

/*
 * Array of commands supported by the device, suitable for
 * scsi_errmsg(9f)
 * Note: Add or remove commands here as appropriate for
 *	 your device.
 */
static struct scsi_key_strings isasg_cmds[] = {
	0x00, "test unit ready",
	0x01, "rezero/rewind",
	0x03, "request sense",
	0x04, "format",
	0x05, "read block limits",
	0x07, "reassign",
	0x08, "read",
	0x0a, "write",
	0x0b, "seek",
	0x0f, "read reverse",
	0x10, "write file mark",
	0x12, "inquiry",
	0x13, "verify",
	0x14, "recover buffered data",
	0x15, "mode select",
	0x16, "reserve",
	0x17, "release",
	0x18, "copy",
	0x19, "erase tape",
	0x1a, "mode sense",
	0x1b, "start/stop/load",
	0x1e, "door lock",
	0x37, "read defect data",
	-1, NULL,
};

/*
 * -----------------------------------------------------------------------------
 *	Module Loading/Unloading and Autoconfiguration Routines
 */

/*
 * Device driver ops vector - cb_ops(9s) structure
 * Device switch table fields (equivalent to the old 4.x cdevsw and bdevsw).
 * Unsupported entry points (e.g. for xxprint() and xxdump()) are set to nodev,
 * except for the poll routine, which is set to nochpoll(), a routine that
 * returns ENXIO.
 * Note	This uses ddi_prop_op for the prop_op(9e) routine. If your device
 *	has its own properties, you should implement a isasg_prop_op() routine
 *	to manage them.
 */
static struct cb_ops isasg_cb_ops = {
	isasg_open,		/* b/c open */
	isasg_close,		/* b/c close */
	isasg_strategy,		/* b strategy */
	(int (*)(unsigned long, char *))nodev,			/* b print */
	(int (*)(unsigned long, char *, long, int))nodev,			/* b dump */
	isasg_read,		/* c read */
	isasg_write,		/* c write */
	isasg_ioctl,		/* c ioctl */
	(int (*)(unsigned long, void **, void **, ddi_devmap_cmd_t, long, unsigned, unsigned, struct cred*))nodev,			/* c devmap */
	(int (*)(unsigned long, long, int))nodev,			/* c mmap */
	(int (*)(unsigned long, long, struct as*, char**, long, unsigned, unsigned, unsigned, struct cred *))nodev,			/* c segmap */
	nochpoll,		/* c poll */
	ddi_prop_op,		/* cb_prop_op */
	0,			/* streamtab  */
	D_MP | D_NEW		/* Driver compatibility flag */

};


/*
 * dev_ops(9S) structure, defined in sys/devops.h.
 * Device Operations table, for autoconfiguration
 * Note	If you replace the isasg_detach entry here with "nulldev", it
 *	implies that the detach is always successful. We need a real detach to
 *	free the sense packet and the unit structure. If you don't want the
 *	driver to ever be unloaded, replace the isasg_detach entry with "nodev"
 *	(which always fails).
 */
static int isasg_info(dev_info_t *dip, ddi_info_cmd_t infocmd, void *arg,
		void **result);
static int isasg_probe(dev_info_t *dip);
static int isasg_attach(dev_info_t *dip, ddi_attach_cmd_t cmd);
static int isasg_detach(dev_info_t *dip, ddi_detach_cmd_t cmd);

static struct dev_ops isasg_ops = {
	DEVO_REV,		/* devo_rev, */
	0,			/* refcnt  */
	isasg_info,		/* info */
	(int (*)(void **))nulldev,		/* identify */
	isasg_probe,		/* probe */
	isasg_attach,		/* attach */
	isasg_detach,		/* detach */
	(int (*)(void**,ddi_reset_cmd_t))nodev,			/* reset */
	&isasg_cb_ops,		/* driver operations */
	(struct bus_ops *)0	/* bus operations */
};

/*
 * Module Loading and Unloading
 * See modctl.h for external structure definitions.
 */

static struct modldrv modldrv = {
	&mod_driverops,			/* Type of module (driver). */
	"Isabel SCSI Generic Driver, Version 1.0",  /* Description */
	&isasg_ops,			/* driver ops */
};

static struct modlinkage modlinkage = {
	MODREV_1, (void *)&modldrv, NULL
};

/*
 * Tell the system that we depend on the general scsi support routines,
 * i.e the scsi "misc" module must be loaded
 */
char _depends_on[] = "misc/scsi";

/*
 * Module Installation
 * Install the driver and "pre-allocate" space for INIT_UNITS units,
 * i.e. instances of the driver.
 */
int
_init(void)
{
	int e;

	if ((e = ddi_soft_state_init(&isasg_state, sizeof (struct scsi_target),
								0)) != 0) {
		ISASG_LOG(0, ISASG_CE_DEBUG2,
			"_init, ddi_soft_state_init failed: 0x%x\n", e);
		return (e);
	}

	if ((e = mod_install(&modlinkage)) != 0) {
		ISASG_LOG(0, ISASG_CE_DEBUG2,
				"_init, mod_install failed: 0x%x\n", e);
		ddi_soft_state_fini(&isasg_state);
	}

	ISASG_LOG(0, ISASG_CE_DEBUG2, "_init succeeded\n");
	return (e);
}


/*
 * Module removal
 */
int
_fini(void)
{
	int e;

	if ((e = mod_remove(&modlinkage)) != 0) {
		ISASG_LOG(0, ISASG_CE_DEBUG1, "_fini mod_remove failed, 0x%x\n", e);
		return (e);
	}

	ddi_soft_state_fini(&isasg_state);

	ISASG_LOG(0, ISASG_CE_DEBUG2, "_fini succeeded\n");
	return (e);
}

/*
 * Return module info
 */
int
_info(struct modinfo *modinfop)
{
	ISASG_LOG(0, ISASG_CE_DEBUG2, "_info\n");
	return (mod_info(&modlinkage, modinfop));
}

/*
 * Autoconfiguration Routines
 */

/*
 * probe(9e) - Check that we're talking to the right device on the SCSI bus.
 * Calls scsi_probe(9f) to see if there's a device at our target id. If
 * there is, scsi_probe will fill in the sd_inq struct (in the devp
 * scsi device struct) with the inquiry data. Validate the data here,
 * allocate a request sense packet, and start filling in the private data
 * structure.
 * Note: Probe should have no side-effects. Just check that we have the right
 *	 device, don't set up any data or initialize the device here. Also,
 *	 it's a Sun convention to probe quietly; send messages to the log
 *	 file only, not to the console.
 * Note: The host adapter driver sets up the scsi_device structure and puts
 *	 it into the dev_info structure with ddi_set_driver_private().
 * Note: No need to allow for probing/attaching in the open() routine because
 *	 of the loadability - the first reference to the device will auto-load
 *	 it, i.e. will call this routine.
 */
#define	VIDSZ		8	/* Vendor Id length in Inquiry Data */
#define	PIDSZ		16	/* Product Id length in Inquiry Data */

static int
isasg_probe(dev_info_t *dip)
{
	register struct scsi_device *devp;
	int i;
	int	err, rval = DDI_PROBE_FAILURE;
	int	tgt, lun;
	char	vpid[VIDSZ+PIDSZ+1];
	static int alreadyInst = 0;
	char *hpchar;

	if( alreadyInst )
	  return rval;

	devp = (struct scsi_device *)ddi_get_driver_private(dip);
	tgt = ddi_getprop(DDI_DEV_T_ANY, dip, DDI_PROP_DONTPASS, "target", -1);
	lun = ddi_getprop(DDI_DEV_T_ANY, dip, DDI_PROP_DONTPASS, "lun", -1);

	ISASG_LOG(devp, ISASG_CE_DEBUG2, "isasg_probe: target %d, lun %d\n",
	    tgt, lun);

	/*
	 * Call the routine scsi_probe to do some of the dirty work.
	 * This routine uses the SCSI Inquiry command to  test for the
	 * presence of the device. If it's successful, it will fill in
	 * the sd_inq field in the scsi_device structure.
	 */
	switch (err = scsi_probe(devp, NULL_FUNC)) {
	case SCSIPROBE_NORESP:
		isasg_log(devp, CE_CONT, "No response from target %d, lun %d\n",
		    tgt, lun);
		rval = DDI_PROBE_FAILURE;
		break;

	case SCSIPROBE_NONCCS:
	case SCSIPROBE_NOMEM:
	case SCSIPROBE_FAILURE:
	default:
		ISASG_LOG(devp, ISASG_CE_DEBUG1,
		    "isasg_probe: scsi_slave failed, 0x%x\n", err);
		rval = DDI_PROBE_FAILURE;
		break;

	case SCSIPROBE_EXISTS:
		/*
		 * Inquiry succeeded, devp->sd_inq is now filled in
		 * Note: Check inq_dtype, inq_vid, inq_pid and any other
		 *	 fields to make sure the target/unit is what's
		 *	 expected (sd_inq is a struct scsi_inquiry,
		 *	 defined in scsi/generic/inquiry.h).
		 * Note: Put device-specific checking into the appropriate
		 *	 case statement, and delete the rest.
		 * Note: The DTYPE_* defines are from <scsi/generic/inquiry.h>,
		 *	 this is the full list as of "now", check it for new
		 *	 types.
		 */
		switch (devp->sd_inq->inq_dtype) {
		case DTYPE_PROCESSOR:
			/*
			 * Print what was found on the console. For your
			 * device, you should send the 'found' message to
			 * the system log file only, by inserting an
			 * exclamation point, "!", as the first character of
			 * the message - see cmn_err(9f).
			 */
			ISASG_LOG(devp, ISASG_CE_DEBUG1,
			    "found %s device at tgt%d, lun%d\n",
			    scsi_dname((int)devp->sd_inq->inq_dtype), tgt, lun);
			bcopy((caddr_t)devp->sd_inq->inq_vid, (caddr_t)vpid,
			    VIDSZ);
			vpid[VIDSZ] = '\0';
			hpchar = strchr( vpid, 'H');
			if( hpchar == NULL) {
			  rval = DDI_PROBE_FAILURE;
			  break;
			}
			hpchar[2] = '\0';
			if( strcmp( hpchar, "HP")) {
			  rval = DDI_PROBE_FAILURE;
			  break;
			}
			bcopy((caddr_t)devp->sd_inq->inq_pid,
			    (caddr_t)vpid+VIDSZ, PIDSZ);
			(vpid+VIDSZ)[6] = '\0';

			if( strcmp( vpid+VIDSZ, "C2520A")) {
			  rval = DDI_PROBE_FAILURE;
			  break;
			}
			ISASG_LOG(devp, ISASG_CE_DEBUG1, "Vendor/Product ID = %s %s\n",
			    vpid, vpid+VIDSZ);
			ISASG_LOG(devp, ISASG_CE_DEBUG1, "Installing isabel scanner driver\n");
			rval = DDI_PROBE_SUCCESS;
			alreadyInst = 1;
			break;
		case DTYPE_OPTICAL:
		case DTYPE_DIRECT:
		case DTYPE_SEQUENTIAL:
		case DTYPE_PRINTER:
		case DTYPE_WORM:
		case DTYPE_RODIRECT:
		case DTYPE_SCANNER:
		case DTYPE_CHANGER:
		case DTYPE_COMM:
			rval = DDI_PROBE_FAILURE;
			break;
		case DTYPE_NOTPRESENT:
			rval = DDI_PROBE_FAILURE;
			break;

		default:
			rval = DDI_PROBE_FAILURE;
			break;
		}
	}
	scsi_unprobe(devp);	/* it's OK to call this if scsi_probe failed */
	return (rval);
}



/*
 * Attach
 *	- create minor device nodes
 *	- initialize per-instance mutex's & condition variables
 *	- device-specific initialization (e.g. read disk label)
 */
/*ARGSUSED*/
static int
isasg_attach(dev_info_t *dip, ddi_attach_cmd_t cmd)
{
	int			instance;
	struct	scsi_pkt	*rqpkt;
	struct scsi_target	*targ;
	struct scsi_device	*devp;
	struct buf		*bp;

	if (cmd != DDI_ATTACH) {
		ISASG_LOG(0, ISASG_CE_DEBUG1, "isasg_attach: unsupported cmd 0x%x\n",
		    cmd);
		return (DDI_FAILURE);
	}

	devp = (struct scsi_device *)ddi_get_driver_private(dip);
	instance = ddi_get_instance(dip);
	ISASG_LOG(devp, ISASG_CE_DEBUG2, "isasg_attach: instance %d\n", instance);

	/*
	 * Re-probe the device to get the Inquiry data; it's used
	 * elsewhere in the driver. The Inquiry data was validated in
	 * isasg_probe so there's no need to look at it again here.
	 */
	if (scsi_probe(devp, NULL_FUNC) != SCSIPROBE_EXISTS) {
		ISASG_LOG(0, ISASG_CE_DEBUG1, "isasg_attach: re-probe failed\n");
		scsi_unprobe(devp);
		return (DDI_FAILURE);
	}

	if (ddi_soft_state_zalloc(isasg_state, instance) != DDI_SUCCESS) {
		scsi_unprobe(devp);
		return (DDI_FAILURE);
	}

	targ = (struct scsi_target *)ddi_get_soft_state(isasg_state, instance);
	devp->sd_private = (char *)(opaque_t)targ;

	targ->targ_sbufp = getrbuf(KM_NOSLEEP);
	if (targ->targ_sbufp == NULL) {
		goto error;
	}

	targ->targ_devp = devp;

	/*
	 * Set auto-rqsense, per-target; record whether it's allowed
	 * in targ_arq
	 */
	targ->targ_arq = (scsi_ifsetcap(ROUTE(targ),
	    "auto-rqsense", 1, 1) == 1) ? 1 : 0;
	ISASG_LOG(devp, ISASG_CE_DEBUG2, "isasg_attach: Auto Sensing %s\n",
	    targ->targ_arq ? "enabled" : "disabled");

	if (!targ->targ_arq) {
		/*
		 * Allocate a Request Sense packet
		 */
		bp = scsi_alloc_consistent_buf(&devp->sd_address, NULL,
		    SENSE_LENGTH, B_READ, NULL_FUNC, NULL);
		if (!bp) {
			goto error;
		}

		rqpkt = scsi_init_pkt(&devp->sd_address, NULL, bp, CDB_GROUP0,
		    targ->targ_arq ? sizeof (struct scsi_arq_status) : 1,
		    0, PKT_CONSISTENT, NULL_FUNC, NULL);
		if (!rqpkt) {
			goto error;
		}
		devp->sd_sense = (struct scsi_extended_sense *)bp->b_un.b_addr;

		makecom_g0(rqpkt, devp, FLAG_NOPARITY, SCMD_REQUEST_SENSE, 0,
		    SENSE_LENGTH);

		rqpkt->pkt_comp = (void (*)())isasg_callback;
		rqpkt->pkt_time = isasg_io_time;
		rqpkt->pkt_flags |= FLAG_SENSING;
		targ->targ_rqs = rqpkt;
		targ->targ_rqbp = bp;
	}

	/*
	 * Create the minor node(s), see the man page
	 * for ddi_create_minor_node(9f).
	 * The 2nd parameter is the minor node name; drvconfig(1M)
	 * appends it to the /devices entry, after the colon.
	 * The 4th parameter ('instance') is the actual minor number,
	 * put into the /devices entry's inode and passed to the driver.
	 * The 5th parameter ("sample_driver") is the node type; it's used
	 * by devlinks to match an entry in /etc/devlink.tab to create
	 * the link from /dev to /devices. The #defines are in <sys/sunddi.h>.
	 */
	if (ddi_create_minor_node(dip, "character", S_IFCHR, instance,
	    "sample_driver", 0) == DDI_FAILURE) {
		ISASG_LOG(0, ISASG_CE_DEBUG1, "Create Minor Failed\n");
		goto error;
	}

	/*
	 * Initialize the condition variables.
	 * Note: We don't need to initialize the mutex (ISASG_MUTEX)
	 *	 because it's actually devp->sd_mutex (in the struct
	 *	 scsi_device) and is initialized by our parent, the
	 *	 host adapter driver.
	 * Note: We don't really need targ_sbuf_cv, we could just wait
	 *	 for targ_pkt_cv instead, but it's clearer this way.
	 */
	cv_init(&targ->targ_sbuf_cv, "targ_sbuf_cv", CV_DRIVER, NULL);
	cv_init(&targ->targ_pkt_cv, "targ_pkt_cv", CV_DRIVER, NULL);

	/*
	 * Note: Do any other pre-open target initialization here,
	 *	 e.g. read/verify the disk label for a fixed-disk drive.
	 */

	ddi_report_dev(dip);
	ISASG_LOG(devp, ISASG_CE_DEBUG2, "Attached isasg driver\n");
	targ->targ_state = ISASG_STATE_CLOSED;
	return (DDI_SUCCESS);

error:
	if (bp) {
		scsi_free_consistent_buf(bp);
	}
	if (rqpkt) {
		scsi_destroy_pkt(rqpkt);
	}
	if (targ->targ_sbufp) {
		freerbuf(targ->targ_sbufp);
	}
	ddi_soft_state_free(isasg_state, instance);
	devp->sd_private = (char *)(opaque_t)0;
	devp->sd_sense = (struct scsi_extended_sense *)0;
	scsi_unprobe(devp);
	return (DDI_FAILURE);
}


/*
 * Detach
 * Free resources allocated in isasg_attach
 * Note: If you implement a timeout routine in this driver, cancel it
 *	 here. Note that scsi_init_pkt is called with SLEEP_FUNC, so it
 *	 will wait for resources if none are available. Changing it to
 *	 a callback constitutes a timeout; however this detach routine
 *	 will be called only if the driver is not open, and there should be
 *	 no outstanding scsi_init_pkt's if we're closed.
 */
/*ARGSUSED*/
static int
isasg_detach(dev_info_t *dip, ddi_detach_cmd_t cmd)
{
	int			instance;
	register struct scsi_device	*devp;
	register struct scsi_target	*targ;


	if (cmd != DDI_DETACH) {
		ISASG_LOG(0, ISASG_CE_DEBUG1, "isasg_detach: bad cmd 0x%x\n", cmd);
		return (DDI_FAILURE);
	}

	devp = (struct scsi_device *)ddi_get_driver_private(dip);
	instance = ddi_get_instance(dip);
	ISASG_LOG(devp, ISASG_CE_DEBUG2, "isasg_detach: unit %d\n", instance);

	if ((targ = (struct scsi_target *)ddi_get_soft_state(isasg_state, instance)) == NULL) {
		ISASG_LOG(devp, CE_WARN, "No Target Struct for isasg%d\n",
		    instance);
		return (DDI_SUCCESS);
	}

	/*
	 * Note: Do unit-specific detaching here; e.g. shut down the device
	 */

	/*
	 * Remove other data structures allocated in isasg_attach()
	 */
	cv_destroy(&targ->targ_sbuf_cv);
	cv_destroy(&targ->targ_pkt_cv);
	if (targ->targ_rqbp) {
		scsi_free_consistent_buf(targ->targ_rqbp);
	}
	if (targ->targ_rqs) {
		scsi_destroy_pkt(targ->targ_rqs);
	}
	if (targ->targ_sbufp) {
		freerbuf(targ->targ_sbufp);
	}
	ddi_soft_state_free(isasg_state, instance);
	devp->sd_private = (char *)(opaque_t)0;
	devp->sd_sense = (struct scsi_extended_sense *)0;
	ddi_remove_minor_node(dip, NULL);
	scsi_unprobe(devp);
	return (DDI_SUCCESS);
}


/*
 * -----------------------------------------------------------------------------
 * Entry Points
 */


/*
 * open(9e)
 * Called for each open(2) call on the device.
 * Make sure the device is present and correct. Do any initialization that's
 * needed (start it up, load media, etc).
 * Note: Credp can be used to restrict access to root, by calling drv_priv(9f);
 *	 see also cred(9s).
 *	 Flag shows the access mode (read/write). Check it if the device is
 *	 read or write only, or has modes where this is relevant.
 *	 Otyp is an open type flag, see open.h.
 *
 * WARNING: Unfortunately there is a bug in SunOS 5.0 that affects driver
 * open(). The file's reference count is incremented before the driver's
 * open is called. This means that if another process closes (last close)
 * the device while we're in the open routine, the close routine will not
 * be called. If the open then fails (e.g. it's an exclusive open device),
 * the file's ref count is decremented again, but the device is now in an
 * inconsistent state - the driver thinks it's open (close was never called),
 * but it's really closed. If it is an exclusive open device, it's now
 * unusable. This is a particular problem if you want to block in the open
 * routine until someone closes the device - close will never be called!
 */
/*ARGSUSED*/
static int
isasg_open(dev_t *dev_p, int flag, int otyp, cred_t *cred_p)
{
	register dev_t			dev = *dev_p;
	register struct scsi_target	*targ;

	ISASG_LOG(0, ISASG_CE_DEBUG2, "isasg_open\n");

	/*
	 * Test the open type flag
	 */
	if (otyp != OTYP_CHR) {
		ISASG_LOG(0, ISASG_CE_DEBUG1, "Unsupported open type %d\n", otyp);
		return (EINVAL);
	}

	if ((targ = (struct scsi_target *)ddi_get_soft_state(isasg_state, getminor(dev))) == NULL) {
		return (ENXIO);		/* invalid minor number */
	}

	/*
	 * This is an exclusive open device; fail if it's not closed.
	 * Otherwise, set the state to open - this will lock out any
	 * other access attempts. We need the mutex here because the
	 * state must not change between the test for closed and the
	 * set to open.
	 * Note	With an exclusive open device, we know that no one else
	 *	will access our data structures so strictly mutexes are
	 *	not required. However since this is an example driver, and
	 *	you may want shared access, the rest of the driver is
	 *	coded with mutexes.
	 * Note	If you need shared access, it's more complex. See bst.c
	 */
	/* ISASG_MUTEX(targ) returns sd_mutex in the scsi_device struct */
	mutex_enter(ISASG_MUTEX(targ));		/* LOCK the targ data struct */
	if (targ->targ_state != ISASG_STATE_CLOSED) {
		mutex_exit(ISASG_MUTEX(targ));
		return (EBUSY);
	}
	targ->targ_state = ISASG_STATE_OPEN;	/* lock out other accesses */
	mutex_exit(ISASG_MUTEX(targ));		/* UNLOCK targ data struct */

	/*
	 * Test to make sure unit still is powered on and is ready
	 * by sending the SCSI Test Unit Ready command.
	 *
	 * If this is, for instance, a CD-ROM, we may get an error on the
	 * first TUR if the disk has never been accessed (a Check Condition
	 * with extended sense data to tell us why) so do one isasg_unit_ready
	 * and ignore the result.
	 */
	(void) isasg_unit_ready(dev);

	if (isasg_unit_ready(dev) == 0) {
		ISASG_LOG(0, ISASG_CE_DEBUG1, "isasg%d_open: not ready\n",
		    getminor(dev));
		targ->targ_state = ISASG_STATE_CLOSED;
		return (EIO);
	}

	/*
	 * Do any other initalization work here, e.g. send
	 * Mode Sense/Select commands to get the target in the
	 * right state.
	 */

	return (0);

}


/*
 * close(9e)
 * Called on final close only, i.e. the last close(2) call.
 * Shut down the device, and mark it closed in the unit structure.
 */
/*ARGSUSED*/
static int
isasg_close(dev_t dev, int flag, int otyp, cred_t *cred_p)
{
	register struct scsi_target	*targ;

	ISASG_LOG(0, ISASG_CE_DEBUG2, "isasg_close\n");

	if ((targ = (struct scsi_target *)ddi_get_soft_state(isasg_state, getminor(dev))) == NULL) {
		return (ENXIO);
	}

	/*
	 * Note: Close processing here, eg rewind if it's a tape;
	 *	 mark offline if removable media, etc.
	 * WARNING: Since this is an exclusive open device, other
	 *	accesses will be locked out until the state is set
	 *	to closed. If you make the device shareable, you
	 *	need to cope with the open routine being called while
	 *	we're in the close routine, and vice-versa. You could
	 *	serialize open/close calls with a semaphore or with
	 *	a condition variable. See bst.c.
	 */

	targ->targ_state = ISASG_STATE_CLOSED;
	return (0);
}


/*
 * Device Configuration Routine
 * link instance number (unit) with dev_info structure
 */
/*ARGSUSED*/
static int
isasg_info(dev_info_t *dip, ddi_info_cmd_t infocmd, void *arg, void **result)
{
	dev_t			dev;
	struct scsi_target	*targ;
	int			instance, error;

	ISASG_LOG(0, ISASG_CE_DEBUG2, "isasg_info\n");

	switch (infocmd) {
	case DDI_INFO_DEVT2DEVINFO:
		dev = (dev_t)arg;
		instance = getminor(dev);
		if ((targ = (struct scsi_target *)ddi_get_soft_state(isasg_state, instance)) == NULL)
			return (DDI_FAILURE);
		*result = (void *) targ->targ_devp->sd_dev;
		error = DDI_SUCCESS;
		break;
	case DDI_INFO_DEVT2INSTANCE:
		*result = (void *)getminor((dev_t)arg);
		error = DDI_SUCCESS;
		break;
	default:
		error = DDI_FAILURE;
	}
	return (error);
}


/*
 * Character (raw) read and write routines, called via read(2) and
 * write(2). These routines perform "raw" (i.e. unbuffered) i/o.
 * Just and call strategy via physio. Physio(9f) will take care of
 * address mapping and locking, and will split the transfer if ncessary,
 * based on minphys, possibly calling the strategy routine multiple times.
 */
/* ARGSUSED2 */
static int
isasg_read(dev_t dev, struct uio *uio, cred_t *cred_p)
{
	ISASG_LOG(0, ISASG_CE_DEBUG2, "Read\n");
	return (physio(isasg_strategy, (struct buf *)0, dev, B_READ,
	    minphys, uio));
}

/* ARGSUSED2 */
static int
isasg_write(dev_t dev, struct uio *uio, cred_t *cred_p)
{
	ISASG_LOG(0, ISASG_CE_DEBUG2, "Write\n");
	return (physio(isasg_strategy, (struct buf *)0, dev, B_WRITE,
	    minphys, uio));
}


/*
 * strategy
 * Main routine for commands to the device. since this is a character
 * device, this routine is called only from the read/write routines above,
 * and isasg_ioctl_cmd for the pass through ioctl (USCSICMD).
 * The cv_wait prevents this routine from being called simultaneously by
 * two threads.
 */
static int
isasg_strategy(struct buf *bp)
{
	register struct scsi_target	*targ;

	if (!(targ = (struct scsi_target *)ddi_get_soft_state(isasg_state, getminor(bp->b_edev)))) {
		bp->b_resid = bp->b_bcount;
		bioerror(bp, ENXIO);
		biodone(bp);
		return (0);
	}

	ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "isasg_strategy\n");

	/*
	 * Wait for the current request to finish. We can safely release
	 * the mutex once we have the pkt, because anyone else calling
	 * strategy will wait here until we release it with a cv_signal.
	 */
	mutex_enter(ISASG_MUTEX(targ));
	while (targ->targ_pkt_busy) {
		cv_wait(&targ->targ_pkt_cv, ISASG_MUTEX(targ));
	}
	targ->targ_pkt_busy =  1;		/* mark busy */
	mutex_exit(ISASG_MUTEX(targ));

	if (((targ->targ_pkt = isasg_make_cmd(targ, bp,
	    (struct scsi_pkt *)NULL))) == NULL) {
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1,
		    "Unable to create SCSI command\n");
		bp->b_resid = bp->b_bcount;
		bioerror(bp, EIO);
		biodone(bp);
		return (0);
	}

	bp->b_resid = 0;

	if (scsi_transport(targ->targ_pkt) != TRAN_ACCEPT) {
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1,
		    "Command transport failed\n");
		bp->b_resid = bp->b_bcount;
		bioerror(bp, EIO);
		biodone(bp);
	}

	return (0);
}


/*
 * ioctl calls.
 * Ioctls are device specific. Three are provided here as examples:
 * ISASGIOC_READY: Send a Test Unit Ready command to the device. This fills
 *		 in the uscsi_cmd structure in the unit struct. The actual
 *		 cdb to be sent to the device is created here; the union
 *		 scsi_cdb is defined in /usr/include/sys/scsi/impl/commands.h.
 * ISASG_ERRLEV:	Change the error reporting level.
 * USCSICMD:	Pass through. Send the user-supplied command to the device. Very
 *		little checking is done - it's left up to the caller to supply
 *		a valid cdb.
 */
/* ARGSUSED3 */
static int
isasg_ioctl(dev_t dev, int cmd, int arg, int flag, cred_t *cred_p, int *rval_p)
{
	register int			err = 0;
	auto struct scsi_target		*targ;
	auto struct uscsi_cmd		scmd;
	auto char			cmdblk[CDB_GROUP0];

	if ((targ = (struct scsi_target *)ddi_get_soft_state(isasg_state, getminor(dev))) == NULL) {
		return (ENXIO);		/* invalid minor number */
	}

	ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "isasg_ioctl: cmd = 0x%x\n", cmd);

	bzero((caddr_t)&scmd, sizeof (struct uscsi_cmd));

	switch (cmd) {
	case ISASGIOC_READY:		/* Send a Test Unit Ready command */
		scmd.uscsi_bufaddr = 0;
		scmd.uscsi_buflen = 0;
		bzero(cmdblk, CDB_GROUP0);
		cmdblk[0] = (char)SCMD_TEST_UNIT_READY;
		scmd.uscsi_flags = USCSI_DIAGNOSE|USCSI_SILENT|USCSI_WRITE;
		scmd.uscsi_cdb = cmdblk;
		scmd.uscsi_cdblen = CDB_GROUP0;

		err = isasg_ioctl_cmd(dev, &scmd, UIO_SYSSPACE, UIO_SYSSPACE);
		break;

	case ISASGIOC_ERRLEV:		/* Set the error reporting level */
		if (copyin((caddr_t)arg, (caddr_t)&isasg_error_reporting,
		    sizeof (int))) {
			return (EFAULT);
		}
		break;

	case USCSICMD:
		/*
		* Check root permissions
		*/
		if (drv_priv(cred_p) != 0) {
			return (EPERM);
		}

		/*
		* Run a generic ucsi.h command.
		*/
		if (copyin((caddr_t)arg, (caddr_t)&scmd, sizeof (scmd))) {
			return (EFAULT);
		}

		err = isasg_ioctl_cmd(dev, &scmd, UIO_USERSPACE, UIO_USERSPACE);
		if (copyout((caddr_t)&scmd, (caddr_t)arg, sizeof (scmd))) {
			err = EFAULT;
		}
		break;

	default:
		err = ENOTTY;
		break;
	}
	return (err);
}


/*
 * Run a command for user (from isasg_ioctl) or from someone else in the driver.
 *
 * cdbspace is for address space of cdb; dataspace is for address space
 * of the buffer - user or kernel.
 */
static int
isasg_ioctl_cmd(dev_t dev, struct uscsi_cmd *scmd,
    enum uio_seg cdbspace, enum uio_seg dataspace)
{
	caddr_t			cdb, user_cdbp;
	register int			err, rw;
	register struct buf		*bp;
	register struct scsi_target	*targ;

	if ((targ = (struct scsi_target *)ddi_get_soft_state(isasg_state, getminor(dev))) == NULL) {
		return (ENXIO);
	}

	ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "isasg_ioctl_cmd\n");

	/*
	 * The uscsi structure itself is already in kernel space (copied
	 * in by isasg_ioctl, or declared there by our caller); but
	 * we need to copy in the cdb here.
	 */
	cdb = (char *)kmem_zalloc((size_t)scmd->uscsi_cdblen, KM_SLEEP);
	if (cdbspace == UIO_SYSSPACE) {
		bcopy(scmd->uscsi_cdb, cdb, scmd->uscsi_cdblen);
	} else if (copyin(scmd->uscsi_cdb, cdb, (u_int) scmd->uscsi_cdblen)) {
		kmem_free(cdb, (size_t)scmd->uscsi_cdblen);
		return (EFAULT);
	}

	/*
	 * The cdb pointer in the structure passed by the user is pointing
	 * to user space. We've just copied the cdb into a local buffer,
	 * so point uscsi_cdb to it now. We'll restore hte user's pointer
	 * at the end.
	 */
	user_cdbp = scmd->uscsi_cdb;	/* save the user's pointer */
	scmd->uscsi_cdb = cdb;		/* point to the local cdb buffer */
	rw = (scmd->uscsi_flags & USCSI_READ) ? B_READ : B_WRITE;

	/*
	 * Get the 'special command' buffer.
	 * First lock the targ struct; if the buffer's busy, wait for
	 * it to become free. Once we get the buffer, mark it busy to
	 * lock out other requests for it.
	 * Note	cv_wait will release the mutex, allowing other parts
	 *	of the driver to acquire it.
	 * Note	Once we have the special buffer, we can safely release
	 *	the mutex; the buffer is now busy and another thread will
	 *	block in the cv_wait until we release it. All the code
	 *	from here until we unset the busy flag is non-reentrant,
	 *	including the physio/strategy/start/callback/done thread.
	 */
	mutex_enter(ISASG_MUTEX(targ));
	while (targ->targ_sbuf_busy) {
		cv_wait(&targ->targ_sbuf_cv, ISASG_MUTEX(targ));
	}
	targ->targ_sbuf_busy = 1;
	mutex_exit(ISASG_MUTEX(targ));

	bp = targ->targ_sbufp;

	if (scmd->uscsi_buflen) {
		/*
		 * We're sending/receiving data; create a uio structure and
		 * call physio to do the right things.
		 */
		auto struct 	iovec	aiov;
		auto struct 	uio	auio;
		register struct uio	*uio = &auio;

		bzero((caddr_t)&auio, sizeof (struct uio));
		bzero((caddr_t)&aiov, sizeof (struct iovec));
		aiov.iov_base = scmd->uscsi_bufaddr;
		aiov.iov_len = scmd->uscsi_buflen;
		uio->uio_iov = &aiov;

		uio->uio_iovcnt = 1;
		uio->uio_resid = scmd->uscsi_buflen;
		uio->uio_segflg = dataspace;
		uio->uio_offset = 0;
		uio->uio_fmode = 0;

		/*
		 * Let physio do the rest...
		 */
		bp->b_private = (void *) scmd;
		err = physio(isasg_strategy, bp, dev, rw, minphys, uio);
	} else {
		/*
		 * No data transfer, we can call isasg_strategy directly
		 */
		bp->b_private = (void *) scmd;
		bp->b_flags = B_BUSY | rw;
		bp->b_edev = dev;
		bp->b_bcount = bp->b_blkno = 0;
		(void) isasg_strategy(bp);
		err = biowait(bp);
	}

	/*
	 * get the status block, if any, and
	 * release any resources that we had.
	 */
	scmd->uscsi_status = 0;
	scmd->uscsi_status = SCBP_C(targ->targ_pkt);

	/*
	 * Lock the targ struct. Clearing the 'busy' flag and signalling
	 * must be made atomic.
	 */
	mutex_enter(ISASG_MUTEX(targ));		/* LOCK the targ struct */
	if (targ->targ_pkt != NULL)
		scsi_destroy_pkt(targ->targ_pkt);
	targ->targ_sbuf_busy = 0;
	cv_signal(&targ->targ_sbuf_cv);		/* release the special buffer */
	targ->targ_pkt_busy = 0;
	cv_signal(&targ->targ_pkt_cv);		/* release the packet */
	mutex_exit(ISASG_MUTEX(targ));		/* UNLOCK the targ struct */

	kmem_free(scmd->uscsi_cdb, (size_t)scmd->uscsi_cdblen);
	scmd->uscsi_cdb = user_cdbp;	/* restore the user's pointer */
	return (err);
}


/*
 * Check to see if the unit will respond to a Test Unit Ready
 * Returns true or false
 */
static int
isasg_unit_ready(dev_t dev)
{
	auto struct uscsi_cmd scmd, *com = &scmd;
	auto char	cmdblk[CDB_GROUP0];
	auto int	err;

	com->uscsi_bufaddr = 0;
	com->uscsi_buflen = 0;
	bzero(cmdblk, CDB_GROUP0);
	cmdblk[0] = (char)SCMD_TEST_UNIT_READY;
	com->uscsi_flags = USCSI_DIAGNOSE|USCSI_SILENT|USCSI_WRITE;
	com->uscsi_cdb = cmdblk;
	com->uscsi_cdblen = CDB_GROUP0;

	if (err = isasg_ioctl_cmd(dev, com, UIO_SYSSPACE, UIO_SYSSPACE)) {
		ISASG_LOG(0, ISASG_CE_DEBUG1, "isasg_unit_ready failed: 0x%x\n", err);
		return (0);
	}
	return (1);
}


/*
 * Done with a command.
 * Start the next command and then call biodone() to tell physio or
 * isasg_ioctl_cmd that this i/o has completed.
 *
 */
static void
isasg_done(struct scsi_target *targ, register struct buf *bp)
{
	ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "isasg_done\n");

	/*
	 * Lock the targ struct; we don't want targ_pkt to change
	 * between taking a copy of it and zeroing it.
	 */
	mutex_enter(ISASG_MUTEX(targ));

	/*
	 * For regular commands (i.e. not using the special buffer),
	 * free resources and clear the current request. Isasg_ioctl_cmd
	 * will do its own freeing for special commands. We need the lock
	 * here because targ_pkt must not change during the operations.
	 */
	if (bp != targ->targ_sbufp) {
		scsi_destroy_pkt(targ->targ_pkt);
		targ->targ_pkt_busy = 0;
		cv_signal(&targ->targ_pkt_cv);
	}

	mutex_exit(ISASG_MUTEX(targ));		/* UNLOCK the targ struct */

	/*
	 * Tell interested parties that the i/o is done
	 */
	biodone(bp);
}


/*
 * Allocate resources for a SCSI command - call scsi_init_pkt to get
 * a packet and allocate DVMA resources, and create the SCSI CDB.
 *
 * Also used to continue an in-progress packet; simply calls scsi_init_pkt
 * to update the DMA resources to the next chunk, and re-creates the
 * SCSI command.
 *
 * Note: There are differences in the DMA subsystem between the SPARC
 *	and x86 platforms. For example, on the x86 platform, DMA uses
 *	physical rather than virtual addresses as a result the DMA buffer
 *	may not be contiguous.
 *
 *	Because of these differences, the system may not be able to fully
 *	satisfy a DMA allocation request from the target driver with resources
 *	attached to one packet. As a solution, the scsi_init_pkt() routine
 *	should be called with the PKT_DMA _PARTIAL flag to allow the system
 *	to break up the DMA request. If a single DMA allocation cannot be
 *	done, the HBA driver sets the pkt_resid in the scsi_pkt structure
 *	to the number of bytes of DMA resources it was able to  allocate.
 *	The target driver needs to modify the SCSI command it places into
 *	the packet for transport to request the proper amount of data specified
 *	in pkt_resid. When the packet's completion routine is called, the
 *	process is repeated again.
 *
 */
static struct scsi_pkt *
isasg_make_cmd(struct scsi_target *targ, struct buf *bp, struct scsi_pkt *pkt)
{
	struct uscsi_cmd *scmd = (struct uscsi_cmd *)bp->b_private;
	int		flags, tval, blkcnt;

	int			pktalloc = 0;
	struct isasg_private	*isasgprivp;

	ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2,
	    "isasg_make_cmd: block %d, count=%d\n", bp->b_blkno, bp->b_bcount);

	/*
	 * If called with pkt == NULL, this is first time, and we
	 * must allocate and initialize the packet
	 */
	if (pkt == (struct scsi_pkt *)NULL)
		pktalloc = 1;

	flags = 0;
	blkcnt = bp->b_bcount >> DEV_BSHIFT;

	if (bp != targ->targ_sbufp) {
		/*
		 * Allocate a packet.
		 * ROUTE(targ) is shorthand for sd_address
		 * Note that if auto request sensing is enabled, we
		 * specify the status length to hold sense data if needed
		 */
		if (pktalloc) {
			pkt = scsi_init_pkt(ROUTE(targ),
			    (struct scsi_pkt *)NULL,
			    bp,
			    (bp->b_blkno + blkcnt > 0x1FFFFF || blkcnt > 0xFF) ?
				CDB_GROUP1 : CDB_GROUP0,
			    targ->targ_arq ?
			    sizeof (struct scsi_arq_status) : 1,
			    sizeof (struct isasg_private), PKT_DMA_PARTIAL,
			    SLEEP_FUNC, 0);
			if (pkt == (struct scsi_pkt *)0) {
				return (NULL);
			}
		} else {
			/*
			 * Here we're simply calling scsi_init_pkt() for the
			 * next section of DMA resource for the current
			 * I/O request (in bp); we needn't worry about
			 * the associated lengths, or possible failure,
			 * since scsi_init_pkt() is just moving the DMA win
			 */
			pkt = scsi_init_pkt(ROUTE(targ), pkt, bp, 0,
			    targ->targ_arq ?
			    sizeof (struct scsi_arq_status) : 1,
			    0, PKT_DMA_PARTIAL, NULL_FUNC, 0);
		}

		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG3,
		    "isasg_make_cmd: pkt_resid: %d\n", pkt->pkt_resid);

		isasgprivp = (struct isasg_private *)pkt->pkt_private;

		if (pktalloc) {
			isasgprivp->priv_bp = bp;
			isasgprivp->priv_amtdone = 0;
			isasgprivp->priv_amt = bp->b_bcount - pkt->pkt_resid;
		}

		isasg_fill_cdb(pkt, targ, bp, flags);
		pkt->pkt_flags = flags;
		tval = isasg_io_time;
	} else {
		/*
		 * All special command come through isasg_ioctl_cmd, which
		 * uses the uscsi interface. Just need to get the CDB
		 * from scmd and plug it in. Still call makecom_g0 because
		 * it fills in some of the pkt field for us. Its cdb
		 * manipulations will be overwritten by the bcopy.
		 */
		if (scmd->uscsi_flags & USCSI_SILENT)
			flags |= FLAG_SILENT;
		if (scmd->uscsi_flags & USCSI_DIAGNOSE)
			flags |= FLAG_DIAGNOSE;
		if (scmd->uscsi_flags & USCSI_ISOLATE)
			flags |= FLAG_ISOLATE;
		if (scmd->uscsi_flags & USCSI_NODISCON)
			flags |= FLAG_NODISCON;
		if (scmd->uscsi_flags & USCSI_NOPARITY)
			flags |= FLAG_NOPARITY;

		if (pktalloc) {
			pkt = scsi_init_pkt(ROUTE(targ),
			    (struct scsi_pkt *)NULL,
			    bp->b_bcount ? bp : 0,
			    scmd->uscsi_cdblen,
			    targ->targ_arq ?
			    sizeof (struct scsi_arq_status) : 1,
			    sizeof (struct isasg_private), PKT_DMA_PARTIAL,
			    SLEEP_FUNC, 0);
			if (!pkt) {
				return (NULL);
			}
		} else {
			pkt = scsi_init_pkt(ROUTE(targ), pkt,
			    bp->b_bcount ? bp : 0, 0,
			    targ->targ_arq ?
			    sizeof (struct scsi_arq_status) : 1,
			    0, PKT_DMA_PARTIAL, NULL_FUNC, 0);
		}

		isasgprivp = (struct isasg_private *)pkt->pkt_private;
		if (pktalloc) {
			isasgprivp->priv_bp = bp;
			isasgprivp->priv_amtdone = 0;
			isasgprivp->priv_amt = bp->b_bcount - pkt->pkt_resid;
		}

		makecom_g0(pkt, targ->targ_devp, flags, scmd->uscsi_cdb[0],
		    0, 0);
		bcopy(scmd->uscsi_cdb, (caddr_t)pkt->pkt_cdbp,
		    scmd->uscsi_cdblen);
		tval = scmd->uscsi_timeout;
	}

	pkt->pkt_comp = (void (*)())isasg_callback;
	pkt->pkt_time = tval;

#ifdef ISASG_DEBUG
	if (isasg_debug > 2) {
		hex_print("isasg_make_cmd, CDB", (caddr_t)pkt->pkt_cdbp, 6);
	}
#endif ISASG_DEBUG

	return (pkt);
}


/*
 * Stuff CDB with SCSI command for current I/O request
 */

static void
isasg_fill_cdb(struct scsi_pkt *pkt, struct scsi_target *targ, struct buf *bp,
    long flags)
{
	struct isasg_private *isasgprivp;
	u_int blkno, len;
	u_int com;
	int use_g1_cmd = 0;

	if ((scsi_options & SCSI_OPTIONS_DR) == 0)
		flags |= FLAG_NODISCON;

	isasgprivp = (struct isasg_private *)(pkt->pkt_private);

	blkno = bp->b_blkno + (isasgprivp->priv_amtdone >> DEV_BSHIFT);
	/* use result of scsi_init_pkt() as I/O length */
	len = isasgprivp->priv_amt; /* pkt->pkt_resid; */

	ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG3,
	    "isasg_fill_cdb: blkno %x len %x\n", blkno, len);
	/*
	 * Note: We use the group 0 Read/Write commands here
	 *	unless the starting block number or blocks
	 *	requested is too large.
	 *	If your device needs different commands for normal
	 *	data transfer (e.g. Send/Receive, read/write buffer),
	 *	change it here.
	 */

	if ((blkno + (len >> DEV_BSHIFT) > 0x1FFFFF) ||
	    ((len >> DEV_BSHIFT) > 0xFF) ||
#ifdef	_DDICT
	    0 ) {
#else
	    CDB_GROUPID(((union scsi_cdb *)(pkt)->pkt_cdbp)->scc_cmd)) {
#endif

		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG3,
		    "isasg_make_cmd: using g1 command\n");
		com = (bp->b_flags & B_READ) ? SCMD_READ_G1 : SCMD_WRITE_G1;
		use_g1_cmd = 1;
	} else {
		com = (bp->b_flags & B_READ) ? SCMD_READ : SCMD_WRITE;
	}

	/*
	 * Note: The CDB creation differs for sequential and
	 *	 direct access devices. We do both here, but
	 *	 you should need only one. For sequential access,
	 *	 we assume a fixed 512 byte block size. If you
	 *	 have a variable block size device, ask for the
	 *	 actual number of bytes wanted, and change the last
	 *	 parameter, 'fixbit' (1) to zero.
	 */
	if (targ->targ_devp->sd_inq->inq_dtype == DTYPE_DIRECT ||
	    targ->targ_devp->sd_inq->inq_dtype == DTYPE_RODIRECT) {
		if (use_g1_cmd)
			makecom_g1(pkt, targ->targ_devp, flags, com,
			    blkno, len >> DEV_BSHIFT);
		else
			makecom_g0(pkt, targ->targ_devp, flags, com,
			    blkno, len >> DEV_BSHIFT);
	} else {
		makecom_g0_s(pkt, targ->targ_devp, flags, com,
		    len >> DEV_BSHIFT,
		    1);		/* 'fixbit': fixed block size */
	}
}


/*
 * -----------------------------------------------------------------------------
 * Interrupt Service Routines
 */

/*
 * Restart a command - the device was either busy or not ready
 */
static void
isasg_restart(caddr_t arg)
{
	struct scsi_target	*targ = (struct scsi_target *)arg;
	struct scsi_pkt		*pkt;
	struct buf		*bp;

	ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "isasg_restart\n");

	/*
	 * No need to lock the targ structure because any other threads
	 * will wait in isasg_strategy for the packet.
	 */
	pkt = targ->targ_pkt;
	bp = (struct buf *)pkt->pkt_private;

	if (bp) {
		struct scsi_pkt *pkt = targ->targ_pkt;

		if (pkt->pkt_flags & FLAG_SENSING) {
			pkt = targ->targ_rqs;
		}

		if (scsi_transport(pkt) != TRAN_ACCEPT) {
			bp->b_resid = bp->b_bcount;
			bioerror(bp, ENXIO);
			isasg_done(targ, bp);
		}
	}
}


/*
 * Command completion processing, called by the host adapter driver
 * when it's done with the command.
 * No need for mutexes in this routine - there's only one active command
 * at a time, anyone else wanting to send a command will wait in strategy
 * until we call isasg_done.
 */
static void
isasg_callback(struct scsi_pkt *pkt)
{
	struct scsi_target	*targ;
	struct buf		*bp;
	int			action;
	struct isasg_private	*isasgprivp;

	isasgprivp = (struct isasg_private *)pkt->pkt_private;
	bp = isasgprivp->priv_bp;
	targ = (struct scsi_target *)ddi_get_soft_state(isasg_state, getminor(bp->b_edev));

	ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2,
	"isasg_callback: pkt_reason = 0x%x, pkt_flags = 0x%x, pkt_state = 0x%x\n",
	    pkt->pkt_reason, pkt->pkt_flags, pkt->pkt_state);

	mutex_enter(ISASG_MUTEX(targ));

	if (pkt->pkt_reason != CMD_CMPLT) {
		/*
		 * The command did not complete. Retry if possible.
		 */
		action = isasg_handle_incomplete(targ);
	} else if (targ->targ_arq && pkt->pkt_state & STATE_ARQ_DONE) {
		/*
		 * The auto-rqsense happened, and the packet has a
		 * filled-in scsi_arq_status structure, pointed to by
		 * pkt_scbp.
		 */
		action = isasg_handle_arq(pkt, targ, bp);
	} else if (pkt->pkt_flags & FLAG_SENSING) {
		/*
		 * We were running a REQUEST SENSE. Decode the
		 * sense data and decide what to do next.
		 */
		pkt = targ->targ_pkt;	/* get the pkt for the orig command */
		pkt->pkt_flags &= ~FLAG_SENSING;
		action = isasg_handle_sense(targ, bp);
	} else {
		/*
		 * Command completed and we're not getting sense. Check
		 * for errors and decide what to do next.
		 */
		action = isasg_check_error(targ, bp);
	}
	mutex_exit(ISASG_MUTEX(targ));

	switch (action) {
	case QUE_SENSE:
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "Getting Sense\n");
		pkt->pkt_flags |= FLAG_SENSING;
		targ->targ_rqs->pkt_private = (opaque_t)bp;
		bzero((caddr_t)targ->targ_devp->sd_sense, SENSE_LENGTH);
		if (scsi_transport(targ->targ_rqs) == TRAN_ACCEPT) {
			break;
		}
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1,
		    "Request Sense transport failed\n");
		/*FALLTHROUGH*/
	case COMMAND_DONE_ERROR:
		bp->b_resid = bp->b_bcount;
		bioerror(bp, ENXIO);
		/*FALLTHROUGH*/
	case COMMAND_DONE:
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "Command Done\n");
		isasg_done(targ, bp);
		break;

	case QUE_COMMAND:
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "Retrying Command\n");
		if (scsi_transport(targ->targ_pkt) != TRAN_ACCEPT) {
			ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1,
			    "Retry transport failed\n");
			bp->b_resid = bp->b_bcount;
			bioerror(bp, ENXIO);
			isasg_done(targ, bp);
			return;
		}
		break;
	case JUST_RETURN:
		break;

	case CONTINUE_PKT:
		/*
		 * Back from a chunk of a split-up bp.  Do next chunk or
		 * finish up.
		 */
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "Continuing packet\n");
		(void) isasg_make_cmd(targ, bp, pkt);
		if (scsi_transport(pkt) != TRAN_ACCEPT) {
			ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1,
			    "Command transport failedn");
			bp->b_resid = bp->b_bcount;
			bioerror(bp, EIO);
			biodone(bp);
		}
	}

}


/*
 * Incomplete command handling. Figure out what to do based on
 * how far the command did get.
 */
static int
isasg_handle_incomplete(struct scsi_target *targ)
{
	register int	rval = COMMAND_DONE_ERROR;
	register struct scsi_pkt	*pkt = targ->targ_pkt;

	if (!targ->targ_arq && pkt->pkt_flags & FLAG_SENSING) {
		pkt = targ->targ_rqs;
	}

	/*
	 * The target may still be running the  command,
	 * so try and reset it, to get it into a known state.
	 * Note: This is forcible, there may be a more polite
	 *	 method for your device.
	 */
	if ((pkt->pkt_statistics &
	    (STAT_BUS_RESET|STAT_DEV_RESET|STAT_ABORTED)) == 0) {
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1, "Aborting Command\n");
		if (!(scsi_abort(ROUTE(targ), (struct scsi_pkt *)0))) {
			ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1,
			    "Resetting Target\n");
			if (!(scsi_reset(ROUTE(targ), RESET_TARGET))) {
				ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1,
				    "Resetting SCSI Bus\n");
				if (!scsi_reset(ROUTE(targ), RESET_ALL)) {
					isasg_log(targ->targ_devp, CE_WARN,
					    "SCSI bus reset failed\n");
				}
				return (COMMAND_DONE_ERROR);
			}
		}
	}

	/*
	 * If we were running a request sense, try it again if possible.
	 * Some devices can handle retries, others will not.
	 */
	if (pkt->pkt_flags & FLAG_SENSING) {
		if (targ->targ_retry_ct++ < isasg_retry_count) {
			rval = QUE_SENSE;
		}
	} else if (targ->targ_retry_ct++ < isasg_retry_count) {
		rval = QUE_COMMAND;
	} else {
		rval = COMMAND_DONE_ERROR;
	}

	ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1, "Cmd incomplete, %s\n",
	    (rval == COMMAND_DONE_ERROR) ? "giving up" : "retrying");

	return (rval);
}


/*
 * Decode sense data
 */
static int
isasg_handle_sense(struct scsi_target *targ, struct buf *bp)
{
	struct scsi_pkt	*rqpkt = targ->targ_rqs;
	register	int rval = COMMAND_DONE_ERROR;
	int		level, amt;


	if (SCBP(rqpkt)->sts_busy) {
		if (targ->targ_retry_ct++ < isasg_retry_count) {
			(void) timeout((void(*)())isasg_restart, (caddr_t)targ,
			    ISASG_BSY_TIMEOUT);
			rval = JUST_RETURN;
		}
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1, "Target Busy, %s\n",
		    (rval == JUST_RETURN) ? "restarting" : "giving up");
		return (rval);
	}

	if (SCBP(rqpkt)->sts_chk) {
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2,
		    "Check Condition on Request Sense!\n");
		return (rval);
	}

	amt = SENSE_LENGTH - rqpkt->pkt_resid;
	if ((rqpkt->pkt_state & STATE_XFERRED_DATA) == 0 || amt == 0) {
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "no sense data\n");
		return (rval);
	}

	/*
	 * Now, check to see whether we got enough sense data to make any
	 * sense out if it (heh-heh).
	 */
	if (amt < SUN_MIN_SENSE_LENGTH) {
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2,
		    "not enough sense data\n");
		return (rval);
	}

	if (isasg_debug > 2) {
		hex_print("isasg Sense Data", (caddr_t)targ->targ_devp->sd_sense,
		    SENSE_LENGTH);
	}

	/*
	 * Decode the sense data
	 * Note: We only looking at the sense key here. Most devices
	 *	 have unique additional sense codes & qualifiers, so
	 *	 it's often more useful to look at them instead.
	 *
	 */
	switch (targ->targ_devp->sd_sense->es_key) {
	case KEY_NOT_READY:
		/*
		 * If we get a not-ready indication, wait a bit and
		 * try it again, unless this is a special command with
		 * the 'fail on error' (FLAG_DIAGNOSE) option set.
		 */
		if ((bp == targ->targ_sbufp) &&
		    (targ->targ_pkt->pkt_flags & FLAG_DIAGNOSE)) {
			rval = COMMAND_DONE_ERROR;
			level = SCSI_ERR_FATAL;
		} else if (targ->targ_retry_ct++ < isasg_retry_count) {
			(void) timeout((void (*)())isasg_restart, (caddr_t)targ,
			    ISASG_BSY_TIMEOUT);
			rval = JUST_RETURN;
			level = SCSI_ERR_RETRYABLE;
		} else {
			rval = COMMAND_DONE_ERROR;
			level = SCSI_ERR_FATAL;
		}
		break;

	case KEY_ABORTED_COMMAND:
	case KEY_UNIT_ATTENTION:
		rval = QUE_COMMAND;
		level = SCSI_ERR_INFO;
		break;
	case KEY_RECOVERABLE_ERROR:
	case KEY_NO_SENSE:
		rval = COMMAND_DONE;
		level = SCSI_ERR_RECOVERED;
		break;
	case KEY_HARDWARE_ERROR:
	case KEY_MEDIUM_ERROR:
	case KEY_MISCOMPARE:
	case KEY_VOLUME_OVERFLOW:
	case KEY_WRITE_PROTECT:
	case KEY_BLANK_CHECK:
	case KEY_ILLEGAL_REQUEST:
	default:
		rval = COMMAND_DONE_ERROR;
		level = SCSI_ERR_FATAL;
		break;
	}

	/*
	 * If this was for a special command, check the options
	 */
	if (bp == targ->targ_sbufp) {
		if ((rval == QUE_COMMAND) &&
		    (targ->targ_pkt->pkt_flags & FLAG_DIAGNOSE)) {
			rval = COMMAND_DONE_ERROR;
		}
		if (((targ->targ_pkt->pkt_flags & FLAG_SILENT) == 0) ||
		    isasg_debug) {
			scsi_errmsg(targ->targ_devp, targ->targ_pkt, "isasg",
			    level, bp->b_blkno, 0, isasg_cmds,
			    targ->targ_devp->sd_sense);
		}
	} else if ((level >= isasg_error_reporting) || isasg_debug) {
		scsi_errmsg(targ->targ_devp, targ->targ_pkt, "isasg",
		    level, bp->b_blkno, 0, isasg_cmds, targ->targ_devp->sd_sense);
	}

	return (rval);
}


#define	ARQP(pktp) 	((struct scsi_arq_status *)((pktp)->pkt_scbp))

/*
 * Decode auto-rqsense data
 */
static int
isasg_handle_arq(struct scsi_pkt *pktp, struct scsi_target *targ, struct buf *bp)
{
	int	level, amt, rval = COMMAND_DONE_ERROR;

	ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "Auto Request Sense done\n");

	if (ARQP(pktp)->sts_rqpkt_status.sts_chk) {
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2,
		    "Check Condition on Auto Request Sense!\n");
		return (rval);
	}

	amt = SENSE_LENGTH - ARQP(pktp)->sts_rqpkt_resid;
	if ((ARQP(pktp)->sts_rqpkt_state & STATE_XFERRED_DATA) == 0 ||
	    amt == 0) {
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "no auto sense data\n");
		return (rval);
	}

	/*
	 * Stuff the sense data pointer into sd_sense
	 */
	targ->targ_devp->sd_sense = &(ARQP(pktp)->sts_sensedata);

	/*
	 * Now, check to see whether we got enough sense data to make any
	 * sense out if it (heh-heh).
	 */
	if (amt < SUN_MIN_SENSE_LENGTH) {
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2,
		    "not enough auto sense data\n");
		return (rval);
	}

	if (isasg_debug > 2) {
		hex_print("isasg Auto Sense Data",
		    (caddr_t)&(ARQP(pktp)->sts_sensedata), SENSE_LENGTH);
	}

	/*
	 * Decode the sense data
	 * Note: I'm only looking at the sense key here. Most devices
	 *	 have unique additional sense codes & qualifiers, so
	 *	 it's often more useful to look at them instead.
	 *
	 */
	switch (ARQP(pktp)->sts_sensedata.es_key) {
	case KEY_NOT_READY:
		/*
		 * If we get a not-ready indication, wait a bit and
		 * try it again, unless this is a special command with
		 * the 'fail on error' (FLAG_DIAGNOSE) option set.
		 */
		if ((bp == targ->targ_sbufp) &&
		    (targ->targ_pkt->pkt_flags & FLAG_DIAGNOSE)) {
			rval = COMMAND_DONE_ERROR;
			level = SCSI_ERR_FATAL;
		} else if (targ->targ_retry_ct++ < isasg_retry_count) {
			(void) timeout((void (*)())isasg_restart, (caddr_t)targ,
			    ISASG_BSY_TIMEOUT);
			rval = JUST_RETURN;
			level = SCSI_ERR_RETRYABLE;
		} else {
			rval = COMMAND_DONE_ERROR;
			level = SCSI_ERR_FATAL;
		}
		break;

	case KEY_ABORTED_COMMAND:
	case KEY_UNIT_ATTENTION:
		rval = QUE_COMMAND;
		level = SCSI_ERR_INFO;
		break;
	case KEY_RECOVERABLE_ERROR:
	case KEY_NO_SENSE:
		rval = COMMAND_DONE;
		level = SCSI_ERR_RECOVERED;
		break;
	case KEY_HARDWARE_ERROR:
	case KEY_MEDIUM_ERROR:
	case KEY_MISCOMPARE:
	case KEY_VOLUME_OVERFLOW:
	case KEY_WRITE_PROTECT:
	case KEY_BLANK_CHECK:
	case KEY_ILLEGAL_REQUEST:
	default:
		rval = COMMAND_DONE_ERROR;
		level = SCSI_ERR_FATAL;
		break;
	}

	/*
	 * If this was for a special command, check the options
	 */
	if (bp == targ->targ_sbufp) {
		if ((rval == QUE_COMMAND) &&
		    (targ->targ_pkt->pkt_flags & FLAG_DIAGNOSE)) {
			rval = COMMAND_DONE_ERROR;
		}
		if (((targ->targ_pkt->pkt_flags & FLAG_SILENT) == 0) ||
		    isasg_debug) {
			scsi_errmsg(targ->targ_devp, targ->targ_pkt, "isasg",
			    level, bp->b_blkno, 0, isasg_cmds,
			    targ->targ_devp->sd_sense);
		}
	} else if ((level >= isasg_error_reporting) || isasg_debug) {
		scsi_errmsg(targ->targ_devp, targ->targ_pkt, "isasg",
		    level, bp->b_blkno, 0, isasg_cmds, targ->targ_devp->sd_sense);
	}

	return (rval);
}


/*
 * Command completion routine. Check the returned status of the
 * command
 */
static int
isasg_check_error(struct scsi_target *targ, struct buf *bp)
{
	struct scsi_pkt	*pkt = targ->targ_pkt;
	int		action;
	struct isasg_private *isasgprivp = (struct isasg_private *)(pkt->pkt_private);

	if (SCBP(pkt)->sts_busy) {
		/*
		 * Target was busy. If we're not out of retries, call
		 * timeout to restart in a bit; otherwise give up and
		 * reset the target. If the fail on error flag is
		 * set, give up immediately.
		 */
		int tval = (pkt->pkt_flags & FLAG_DIAGNOSE) ? 0
							    : ISASG_BSY_TIMEOUT;

		if (SCBP(pkt)->sts_is) {
			/*
			 * Implicit assumption here is that a device
			 * will only be reserved long enough to
			 * permit a single i/o operation to complete.
			 */
			tval = isasg_io_time * drv_usectohz(1000000);
		}

		if ((int)targ->targ_retry_ct++ < isasg_retry_count) {
			if (tval) {
				(void) timeout((void (*)())isasg_restart, (caddr_t)targ,
				    tval);
				action = JUST_RETURN;
				ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2,
				    "Target busy, retrying\n");
			} else {
				action = COMMAND_DONE_ERROR;
				ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2,
				    "Target busy, no retries\n");
			}
		} else {
			/*
			 * WARNING: See the warning in isasg_handle_incomplete
			 */
			ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1,
			    "Resetting Target\n");
			if (!scsi_reset(ROUTE(targ), RESET_TARGET)) {
#ifdef SCSI_BUS_RESET
				ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1,
				    "Resetting SCSI Bus\n");
				if (!scsi_reset(ROUTE(targ), RESET_ALL)) {
					isasg_log(targ->targ_devp, CE_WARN,
					    "SCSI bus reset failed\n");
				}
#else SCSI_BUS_RESET
				isasg_log(targ->targ_devp, CE_WARN,
				    "Reset Target failed\n");
#endif SCSI_BUS_RESET
			}
			action = COMMAND_DONE_ERROR;
		}
	} else if (SCBP(pkt)->sts_chk) {
		action = QUE_SENSE;	/* check condition - get sense */
		if (targ->targ_arq) {
			ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG1,
			    "Check Condition with Auto Sense enabled!\n");
		} else {
			ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2,
			    "Check Condition\n");
		}
	} else {
		targ->targ_retry_ct = 0;
		ISASG_LOG(targ->targ_devp, ISASG_CE_DEBUG2, "Command Complete\n");
		/*
		 * pkt_resid will reflect, at this point, a residual
		 * of how many bytes were not transferred; a non-zero
		 * pkt_resid is an error.
		 */
		if (pkt->pkt_resid) {
			action = COMMAND_DONE_ERROR;
			bp->b_resid += pkt->pkt_resid;
		} else {
			isasgprivp->priv_amtdone += isasgprivp->priv_amt;
			if (isasgprivp->priv_amtdone < bp->b_bcount)
				action = CONTINUE_PKT;
			else
				action = COMMAND_DONE;
		}
	}
	return (action);
}


/*
 * -----------------------------------------------------------------------------
 *	Error Message Data and Routines
 */

/*
 * Log a message to the console and/or syslog with cmn_err
 */
/*ARGSUSED*/
static void
isasg_log(struct scsi_device *devp, int level, const char *fmt, ...)
{
#if 0
	auto char name[16];
	auto char buf[256];
	va_list ap;

	if (devp) {
		(void) sprintf(name, "%s%d", ddi_get_name(devp->sd_dev),
		    ddi_get_instance(devp->sd_dev));
	} else {
		(void) sprintf(name, "isasg");
	}

	va_start(ap, fmt);
	(void) vsprintf(buf, fmt, ap);
	va_end(ap);

	switch (level) {
	case CE_CONT:
	case CE_NOTE:
	case CE_WARN:
	case CE_PANIC:
		cmn_err(level, "%s:\t%s", name, buf);
		break;

	case ISASG_CE_DEBUG4: if (isasg_debug < 4) break;
	/*FALLTHROUGH*/
	case ISASG_CE_DEBUG3: if (isasg_debug < 3) break;
	/*FALLTHROUGH*/
	case ISASG_CE_DEBUG2: if (isasg_debug < 2) break;
	/*FALLTHROUGH*/
	case ISASG_CE_DEBUG1:
	/*FALLTHROUGH*/
	default:
		cmn_err(CE_CONT, "^%s:\t%s", name, buf);
		break;
	}
#endif
}

/*
 * Print a readable error message
 * Note: This uses the arrays 'isasg_cmds' and 'isasg_errors' defined in
 *	 this file.
 */

#define	BUFSZ	256

/*
 * Print a buffer readably
 */
static void
hex_print(char *msg, char *cp, int len)
{
	int i = 0, j;
	char	buf[BUFSZ];

	bzero(buf, BUFSZ);
	for (i = 0; i < len; i++) {
		/*
		 * make sure there's room for longest %x (i.e., 8 for
		 * a negative number) plus space (1) plus zero (1)
		 */
		if ((j = strlen(buf)) >= (BUFSZ - 10)) {
			buf[BUFSZ-2] = '>';
			buf[BUFSZ-1] = 0;
			break;		/* cp too long, give up */
		}
		(void) sprintf(&buf[j], "%x ", cp[i]);
	}
	cmn_err(CE_CONT, "^%s: %s\n", msg, buf);
}
