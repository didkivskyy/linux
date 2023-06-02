/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/otg/linux/drivers/fh_otg_pcd.h $
 * $Revision: #49 $
 * $Date: 2013/05/16 $
 * $Change: 2231774 $
 *
 * Synopsys HS OTG Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */
#ifndef FH_HOST_ONLY
#if !defined(__FH_PCD_H__)
#define __FH_PCD_H__

#include "fh_otg_os_dep.h"
#include "../fh_common_port/usb.h"
#include "fh_otg_cil.h"
#include "fh_otg_pcd_if.h"
struct cfiobject;

/**
 * @file
 *
 * This file contains the structures, constants, and interfaces for
 * the Perpherial Contoller Driver (PCD).
 *
 * The Peripheral Controller Driver (PCD) for Linux will implement the
 * Gadget API, so that the existing Gadget drivers can be used. For
 * the Mass Storage Function driver the File-backed USB Storage Gadget
 * (FBS) driver will be used.  The FBS driver supports the
 * Control-Bulk (CB), Control-Bulk-Interrupt (CBI), and Bulk-Only
 * transports.
 *
 */

/** Invalid DMA Address */
#define FH_DMA_ADDR_INVALID	(~(fh_dma_t)0)

/** Max Transfer size for any EP */
#define DDMA_MAX_TRANSFER_SIZE 65535

/**
 * Get the pointer to the core_if from the pcd pointer.
 */
#define GET_CORE_IF( _pcd ) (_pcd->core_if)

/**
 * States of EP0.
 */
typedef enum ep0_state {
	EP0_DISCONNECT,		/* no host */
	EP0_IDLE,
	EP0_IN_DATA_PHASE,
	EP0_OUT_DATA_PHASE,
	EP0_IN_STATUS_PHASE,
	EP0_OUT_STATUS_PHASE,
	EP0_STALL,
} ep0state_e;

/** Fordward declaration.*/
struct fh_otg_pcd;

/** FH_otg iso request structure.
 *
 */
typedef struct usb_iso_request fh_otg_pcd_iso_request_t;

#ifdef FH_UTE_PER_IO

/**
 * This shall be the exact analogy of the same type structure defined in the
 * usb_gadget.h. Each descriptor contains
 */
struct fh_iso_pkt_desc_port {
	uint32_t offset;
	uint32_t length;	/* expected length */
	uint32_t actual_length;
	uint32_t status;
};

struct fh_iso_xreq_port {
	/** transfer/submission flag */
	uint32_t tr_sub_flags;
	/** Start the request ASAP */
#define FH_EREQ_TF_ASAP		0x00000002
	/** Just enqueue the request w/o initiating a transfer */
#define FH_EREQ_TF_ENQUEUE		0x00000004

	/**
	* count of ISO packets attached to this request - shall
	* not exceed the pio_alloc_pkt_count
	*/
	uint32_t pio_pkt_count;
	/** count of ISO packets allocated for this request */
	uint32_t pio_alloc_pkt_count;
	/** number of ISO packet errors */
	uint32_t error_count;
	/** reserved for future extension */
	uint32_t res;
	/** Will be allocated and freed in the UTE gadget and based on the CFC value */
	struct fh_iso_pkt_desc_port *per_io_frame_descs;
};
#endif
/** FH_otg request structure.
 * This structure is a list of requests.
 */
typedef struct fh_otg_pcd_request {
	void *priv;
	void *buf;
	fh_dma_t dma;
	uint32_t length;
	uint32_t actual;
	unsigned sent_zlp:1;
    /**
     * Used instead of original buffer if
     * it(physical address) is not dword-aligned.
     **/
	uint8_t *dw_align_buf;
	fh_dma_t dw_align_buf_dma;

	 FH_CIRCLEQ_ENTRY(fh_otg_pcd_request) queue_entry;
#ifdef FH_UTE_PER_IO
	struct fh_iso_xreq_port ext_req;
	//void *priv_ereq_nport; /*  */
#endif
} fh_otg_pcd_request_t;

FH_CIRCLEQ_HEAD(req_list, fh_otg_pcd_request);

/**	  PCD EP structure.
 * This structure describes an EP, there is an array of EPs in the PCD
 * structure.
 */
typedef struct fh_otg_pcd_ep {
	/** USB EP Descriptor */
	const usb_endpoint_descriptor_t *desc;

	/** queue of fh_otg_pcd_requests. */
	struct req_list queue;
	unsigned stopped:1;
	unsigned disabling:1;
	unsigned dma:1;
	unsigned queue_sof:1;

#ifdef FH_EN_ISOC
	/** ISOC req handle passed */
	void *iso_req_handle;
#endif				//_EN_ISOC_

	/** FH_otg ep data. */
	fh_ep_t fh_ep;

	/** Pointer to PCD */
	struct fh_otg_pcd *pcd;

	void *priv;
} fh_otg_pcd_ep_t;

/** FH_otg PCD Structure.
 * This structure encapsulates the data for the fh_otg PCD.
 */
struct fh_otg_pcd {
	const struct fh_otg_pcd_function_ops *fops;
	/** The FH otg device pointer */
	struct fh_otg_device *otg_dev;
	/** Core Interface */
	fh_otg_core_if_t *core_if;
	/** State of EP0 */
	ep0state_e ep0state;
	/** EP0 Request is pending */
	unsigned ep0_pending:1;
	/** Indicates when SET CONFIGURATION Request is in process */
	unsigned request_config:1;
	/** The state of the Remote Wakeup Enable. */
	unsigned remote_wakeup_enable:1;
	/** The state of the B-Device HNP Enable. */
	unsigned b_hnp_enable:1;
	/** The state of A-Device HNP Support. */
	unsigned a_hnp_support:1;
	/** The state of the A-Device Alt HNP support. */
	unsigned a_alt_hnp_support:1;
	/** Count of pending Requests */
	unsigned request_pending;

	/** SETUP packet for EP0
	 * This structure is allocated as a DMA buffer on PCD initialization
	 * with enough space for up to 3 setup packets.
	 */
	union {
		usb_device_request_t req;
		uint32_t d32[2];
	} *setup_pkt;

	fh_dma_t setup_pkt_dma_handle;

	/* Additional buffer and flag for CTRL_WR premature case */
	uint8_t *backup_buf;
	unsigned data_terminated;

	/** 2-byte dma buffer used to return status from GET_STATUS */
	uint16_t *status_buf;
	fh_dma_t status_buf_dma_handle;

	/** EP0 */
	fh_otg_pcd_ep_t ep0;

	/** Array of IN EPs. */
	fh_otg_pcd_ep_t in_ep[MAX_EPS_CHANNELS - 1];
	/** Array of OUT EPs. */
	fh_otg_pcd_ep_t out_ep[MAX_EPS_CHANNELS - 1];
	/** number of valid EPs in the above array. */
//        unsigned      num_eps : 4;
	fh_spinlock_t *lock;

	/** Tasklet to defer starting of TEST mode transmissions until
	 *	Status Phase has been completed.
	 */
	fh_tasklet_t *test_mode_tasklet;

	/** Tasklet to delay starting of xfer in DMA mode */
	fh_tasklet_t *start_xfer_tasklet;

	/** The test mode to enter when the tasklet is executed. */
	unsigned test_mode;
	/** The cfi_api structure that implements most of the CFI API
	 * and OTG specific core configuration functionality
	 */
#ifdef FH_UTE_CFI
	struct cfiobject *cfi;
#endif

};

//FIXME this functions should be static, and this prototypes should be removed
extern void fh_otg_request_nuke(fh_otg_pcd_ep_t * ep);
extern void fh_otg_request_done(fh_otg_pcd_ep_t * ep,
				fh_otg_pcd_request_t * req, int32_t status);

void fh_otg_iso_buffer_done(fh_otg_pcd_t * pcd, fh_otg_pcd_ep_t * ep,
			    void *req_handle);
extern void fh_otg_pcd_start_iso_ddma(fh_otg_core_if_t * core_if, 
				fh_otg_pcd_ep_t * ep);

extern void do_test_mode(void *data);
#endif
#endif /* FH_HOST_ONLY */