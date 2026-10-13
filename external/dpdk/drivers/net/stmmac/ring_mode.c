/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2023 Fuzhou Rockchip Electronics Co., Ltd
 */

#include "common.h"

static void refill_desc3(void *priv_ptr, struct dma_desc *p)
{
	struct stmmac_rx_queue *rx_q = priv_ptr;
	struct stmmac_private *priv = rx_q->private;

	/* Fill DES3 in case of RING mode */
	if (priv->dma_buf_sz == BUF_SIZE_16KiB)
		p->des3 = cpu_to_le32(le32_to_cpu(p->des2) + BUF_SIZE_8KiB);
}

/* In ring mode we need to fill the desc3 because it is used as buffer */
static void init_desc3(struct dma_desc *p)
{
	p->des3 = cpu_to_le32(le32_to_cpu(p->des2) + BUF_SIZE_8KiB);
}

static void clean_desc3(void *priv_ptr, struct dma_desc *p)
{
	p->des3 = 0;
}

static int set_16kib_bfsize(int mtu)
{
	int ret = 0;
	if (unlikely(mtu > BUF_SIZE_8KiB))
		ret = BUF_SIZE_16KiB;
	return ret;
}

const struct stmmac_mode_ops ring_mode_ops = {
	.refill_desc3 = refill_desc3,
	.init_desc3 = init_desc3,
	.clean_desc3 = clean_desc3,
	.set_16kib_bfsize = set_16kib_bfsize,
};
