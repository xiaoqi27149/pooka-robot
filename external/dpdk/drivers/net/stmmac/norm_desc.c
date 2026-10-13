/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2022 Fuzhou Rockchip Electronics Co., Ltd
 */

#include "common.h"
#include "descs_com.h"

static int ndesc_get_tx_status(struct rte_eth_stats *x,
			       struct dma_desc *p)
{
	unsigned int tdes0 = le32_to_cpu(p->des0);
	int ret = tx_done;

	/* Get tx owner first */
	if (unlikely(tdes0 & TDES0_OWN))
		return tx_dma_own;

	if (unlikely(tdes0 & TDES0_ERROR_SUMMARY))
		ret = tx_err;

	if (unlikely(tdes0 & TDES0_DEFERRED))
		ret = tx_err;

	if (ret == tx_err)
		x->ierrors++;

	return ret;
}

static int ndesc_get_tx_len(struct dma_desc *p)
{
	return (le32_to_cpu(p->des1) & RDES1_BUFFER1_SIZE_MASK);
}

/* This function verifies if each incoming frame has some errors
 * and, if required, updates the multicast statistics.
 * In case of success, it returns good_frame because the GMAC device
 * is supposed to be able to compute the csum in HW. */
static int ndesc_get_rx_status(struct rte_eth_stats *x,
			       struct dma_desc *p, unsigned int *len,
			       int rx_coe_type)
{
	unsigned int rdes0 = le32_to_cpu(p->des0);
	unsigned int csum = 0;
	int ret = good_frame;

	if (unlikely(rdes0 & RDES0_OWN))
		return dma_own;

	if (unlikely(rdes0 & RDES0_ERROR_SUMMARY))
		ret = discard_frame;

	if (unlikely(rdes0 & RDES0_DRIBBLING))
		ret = discard_frame;

	if (unlikely(rdes0 & RDES0_LENGTH_ERROR))
		ret = discard_frame;

	if (unlikely(rdes0 & RDES0_MII_ERROR))
		ret = discard_frame;

	if (ret == discard_frame)
		x->oerrors++;

	/* The type-1 checksum offload engines append the checksum at
	 * the end of frame and the two bytes of checksum are added in
	 * the length.
	 * Adjust for that in the framelen for type-1 checksum offload
	 * engines
	 */
	if (rx_coe_type == STMMAC_RX_COE_TYPE1)
		csum = 2;

	*len = (((rdes0 & RDES0_FRAME_LEN_MASK)
			>> RDES0_FRAME_LEN_SHIFT) -
		csum);

	return ret;
}

static void ndesc_init_rx_desc(struct dma_desc *p, int disable_rx_ic, int mode,
			       int end, int bfsize)
{
	int bfsize1;

	p->des0 |= cpu_to_le32(RDES0_OWN);

	bfsize1 = min(bfsize, BUF_SIZE_2KiB - 1);
	p->des1 |= cpu_to_le32(bfsize1 & RDES1_BUFFER1_SIZE_MASK);

	if (mode == STMMAC_CHAIN_MODE)
		ndesc_rx_set_on_chain(p, end);
	else
		ndesc_rx_set_on_ring(p, end, bfsize);

	p->des1 |= cpu_to_le32(RDES1_DISABLE_IC);
}

static void ndesc_init_tx_desc(struct dma_desc *p, int mode, int end)
{
	p->des0 &= cpu_to_le32(~TDES0_OWN);
	if (mode == STMMAC_CHAIN_MODE)
		ndesc_tx_set_on_chain(p);
	else
		ndesc_end_tx_desc_on_ring(p, end);
}

static int ndesc_get_tx_owner(struct dma_desc *p)
{
	return (le32_to_cpu(p->des0) & TDES0_OWN) >> 31;
}

static void ndesc_set_tx_owner(struct dma_desc *p)
{
	p->des0 |= cpu_to_le32(TDES0_OWN);
}

static void ndesc_set_rx_owner(struct dma_desc *p, int disable_rx_ic)
{
	p->des0 = cpu_to_le32(RDES0_OWN);
}

static int ndesc_get_tx_ls(struct dma_desc *p)
{
	return (le32_to_cpu(p->des1) & TDES1_LAST_SEGMENT) >> 30;
}

static void ndesc_release_tx_desc(struct dma_desc *p, int mode)
{
	int ter = (le32_to_cpu(p->des1) & TDES1_END_RING) >> 25;

	memset(p, 0, offsetof(struct dma_desc, des2));
	if (mode == STMMAC_CHAIN_MODE)
		ndesc_tx_set_on_chain(p);
	else
		ndesc_end_tx_desc_on_ring(p, ter);
}

static void ndesc_prepare_tx_desc(struct dma_desc *p, int is_fs, int len,
				  bool csum_flag, int mode, bool tx_own,
				  bool ls, unsigned int tot_pkt_len,
				  bool last_desc)
{
	unsigned int tdes1;// = le32_to_cpu(p->des1);

	if (is_fs)
		tdes1 |= TDES1_FIRST_SEGMENT;
	else
		tdes1 &= ~TDES1_FIRST_SEGMENT;

	if (likely(csum_flag))
		tdes1 |= (TX_CIC_FULL) << TDES1_CHECKSUM_INSERTION_SHIFT;
	else
		tdes1 &= ~(TX_CIC_FULL << TDES1_CHECKSUM_INSERTION_SHIFT);

	if (ls)
		tdes1 |= TDES1_LAST_SEGMENT;

	if (last_desc)
		tdes1 |= cpu_to_le32(TDES1_END_RING);

	p->des1 = cpu_to_le32(tdes1);

	if (mode == STMMAC_CHAIN_MODE)
		norm_set_tx_desc_len_on_chain(p, len);
	else
		norm_set_tx_desc_len_on_ring(p, len);

	if (tx_own)
		p->des0 = cpu_to_le32(TDES0_OWN);
}

static void ndesc_set_tx_ic(struct dma_desc *p)
{
	p->des1 |= cpu_to_le32(TDES1_INTERRUPT);
}

static int ndesc_get_rx_frame_len(struct dma_desc *p, int rx_coe_type)
{
	unsigned int csum = 0;

	/* The type-1 checksum offload engines append the checksum at
	 * the end of frame and the two bytes of checksum are added in
	 * the length.
	 * Adjust for that in the framelen for type-1 checksum offload
	 * engines
	 */
	if (rx_coe_type == STMMAC_RX_COE_TYPE1)
		csum = 2;

	return (((le32_to_cpu(p->des0) & RDES0_FRAME_LEN_MASK)
				>> RDES0_FRAME_LEN_SHIFT) -
		csum);

}

static void ndesc_enable_tx_timestamp(struct dma_desc *p)
{
	p->des1 |= cpu_to_le32(TDES1_TIME_STAMP_ENABLE);
}

static int ndesc_get_tx_timestamp_status(struct dma_desc *p)
{
	return (le32_to_cpu(p->des0) & TDES0_TIME_STAMP_STATUS) >> 17;
}

static void ndesc_get_timestamp(void *desc, u32 ats, u64 *ts)
{
	struct dma_desc *p = (struct dma_desc *)desc;
	u64 ns;

	ns = le32_to_cpu(p->des2);
	/* convert high/sec time stamp value to nanosecond */
	ns += le32_to_cpu(p->des3) * 1000000000ULL;

	*ts = ns;
}

static int ndesc_get_rx_timestamp_status(void *desc, void *next_desc, u32 ats)
{
	struct dma_desc *p = (struct dma_desc *)desc;

	if ((le32_to_cpu(p->des2) == 0xffffffff) &&
	    (le32_to_cpu(p->des3) == 0xffffffff))
		/* timestamp is corrupted, hence don't store it */
		return 0;
	else
		return 1;
}

static void ndesc_display_ring(void *head, unsigned int size, bool rx,
			       dma_addr_t dma_rx_phy, unsigned int desc_size)
{
	struct dma_desc *p = (struct dma_desc *)head;
	dma_addr_t dma_addr;
	int i;

	STMMAC_PMD_INFO("%s descriptor ring:\n", rx ? "RX" : "TX");

	if (desc_size == sizeof(struct dma_desc)) {
		struct dma_desc *p = (struct dma_desc *)head;

		for (i = 0; i < size; i++) {
			dma_addr = dma_rx_phy + i * sizeof(*p);
			STMMAC_PMD_INFO("%03d [%pad]: 0x%x 0x%x 0x%x 0x%x\n",
				i, &dma_addr,
				rte_le_to_cpu_32(p->des0), rte_le_to_cpu_32(p->des1),
				rte_le_to_cpu_32(p->des2), rte_le_to_cpu_32(p->des3));
			p++;
		}
	}

	STMMAC_PMD_INFO("\n");
}

static void ndesc_get_addr(struct dma_desc *p, unsigned int *addr)
{
	*addr = le32_to_cpu(p->des2);
}

static void ndesc_set_addr(struct dma_desc *p, dma_addr_t addr)
{
	p->des2 = cpu_to_le32(addr);
}

static void ndesc_clear(struct dma_desc *p)
{
	p->des2 = 0;
}

const struct stmmac_desc_ops ndesc_ops = {
	.tx_status = ndesc_get_tx_status,
	.rx_status = ndesc_get_rx_status,
	.get_tx_len = ndesc_get_tx_len,
	.init_rx_desc = ndesc_init_rx_desc,
	.init_tx_desc = ndesc_init_tx_desc,
	.get_tx_owner = ndesc_get_tx_owner,
	.release_tx_desc = ndesc_release_tx_desc,
	.prepare_tx_desc = ndesc_prepare_tx_desc,
	.set_tx_ic = ndesc_set_tx_ic,
	.get_tx_ls = ndesc_get_tx_ls,
	.set_tx_owner = ndesc_set_tx_owner,
	.set_rx_owner = ndesc_set_rx_owner,
	.get_rx_frame_len = ndesc_get_rx_frame_len,
	.enable_tx_timestamp = ndesc_enable_tx_timestamp,
	.get_tx_timestamp_status = ndesc_get_tx_timestamp_status,
	.get_timestamp = ndesc_get_timestamp,
	.get_rx_timestamp_status = ndesc_get_rx_timestamp_status,
	.display_ring = ndesc_display_ring,
	.get_addr = ndesc_get_addr,
	.set_addr = ndesc_set_addr,
	.clear = ndesc_clear,
};
