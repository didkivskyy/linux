/*
 * Copyright 2009 Texas Instruments.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __ARCH_ARM_FH_SPI_H
#define __ARCH_ARM_FH_SPI_H

#include <linux/io.h>
#include <linux/scatterlist.h>

#define SPI_MASTER_CONTROLLER_MAX_SLAVE		(2)
#define SPI_TRANSFER_USE_DMA			(0x77888877)

struct fh_spi_cs {
	u32 GPIO_Pin;
	char *name;
};

struct fh_spi_chip {
	u8 poll_mode;	/* 0 for contoller polling mode */
	u8 type;	/* SPI/SSP/Micrwire */
	u8 enable_dma;
	void *cs_control;
	/* void (*cs_control)(u32 command);*/
};

struct fh_spi_platform_data {
	u32 apb_clock_in;
	u32 fifo_len;
	u32 slave_max_num;
	struct fh_spi_cs cs_data[SPI_MASTER_CONTROLLER_MAX_SLAVE];
	/*below is dma transfer needed*/
	u32 dma_transfer_enable;
	u32 rx_handshake_num;
	u32 tx_handshake_num;
	u32 bus_no;
	char *clk_name;
	u32 rx_dma_channel;
	u32 tx_dma_channel;
#define	RX_ONLY_MODE		0x30
#define	TX_RX_MODE			0x31
	u32 spidma_xfer_mode;
	/* add support wire width*/
	u32 ctl_wire_support;
	u32 max_speed_support;
	u32 data_reg_offset;
#define INC_SUPPORT	0x55
	u32 data_increase_support;
	u32 data_field_size;
#define SWAP_SUPPORT 0x55
	u32 swap_support;
	void (*plat_init)(struct fh_spi_platform_data *plat_data);
#define SPI_DMA_PROTCTL_ENABLE	0x55
	u32 dma_protctl_enable;
	u32 dma_protctl_data;

#define SPI_DMA_MASTER_SEL_ENABLE	0x55
	u32 dma_master_sel_enable;
	u32 dma_master_ctl_sel;
	u32 dma_master_mem_sel;
};

#endif	/* __ARCH_ARM_FH_SPI_H */