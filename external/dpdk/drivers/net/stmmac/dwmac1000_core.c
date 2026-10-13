/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2023 Fuzhou Rockchip Electronics Co., Ltd
 */

#include "stmmac_pmd_logs.h"
#include "stmmac.h"
//#include "stmmac_pcs.h"
#include "dwmac1000.h"

static void dwmac1000_core_init(struct mac_device_info *hw,
				struct stmmac_private *priv)
{
	void __iomem *ioaddr_v = hw->pcsr;
	u32 value = readl(ioaddr_v + GMAC_CONTROL);
	int mtu = priv->mtu;

	/* Configure GMAC core */
	value |= GMAC_CORE_INIT;

	/* Clear ACS bit because Ethernet switch tagging formats such as
	 * Broadcom tags can look like invalid LLC/SNAP packets and cause the
	 * hardware to truncate packets on reception.
	 */
	if (!priv->plat->enh_desc)
		value &= ~GMAC_CONTROL_ACS;

	if (mtu > 1500)
		value |= GMAC_CONTROL_2K;
	if (mtu > 2000)
		value |= GMAC_CONTROL_JE;

	if (hw->ps) {
		value |= GMAC_CONTROL_TE;

		value &= ~hw->link.speed_mask;
		switch (hw->ps) {
		case SPEED_1000:
			value |= hw->link.speed1000;
			break;
		case SPEED_100:
			value |= hw->link.speed100;
			break;
		case SPEED_10:
			value |= hw->link.speed10;
			break;
		}
	}

	writel(value, ioaddr_v + GMAC_CONTROL);

	/* Mask GMAC interrupts */
	value = GMAC_INT_DEFAULT_MASK;

	if (hw->pcs)
		value &= ~GMAC_INT_DISABLE_PCS;

	writel(value, ioaddr_v + GMAC_INT_MASK);

#ifdef STMMAC_VLAN_TAG_USED
	/* Tag detection without filtering */
	writel(0x0, ioaddr_v + GMAC_VLAN_TAG);
#endif
}

static int dwmac1000_rx_ipc_enable(struct mac_device_info *hw)
{
	void __iomem *ioaddr_v = hw->pcsr;
	u32 value = readl(ioaddr_v + GMAC_CONTROL);

	if (hw->rx_csum)
		value |= GMAC_CONTROL_IPC;
	else
		value &= ~GMAC_CONTROL_IPC;

	writel(value, ioaddr_v + GMAC_CONTROL);

	value = readl(ioaddr_v + GMAC_CONTROL);

	return !!(value & GMAC_CONTROL_IPC);
}

static void dwmac1000_dump_regs(struct mac_device_info *hw, u32 *reg_space)
{
	void __iomem *ioaddr_v = hw->pcsr;
	int i;

	for (i = 0; i < 55; i++)
		reg_space[i] = readl(ioaddr_v + i * 4);
}

static void dwmac1000_set_umac_addr(struct mac_device_info *hw,
				    unsigned char *addr,
				    unsigned int reg_n)
{
	void __iomem *ioaddr_v = hw->pcsr;
	stmmac_set_mac_addr(ioaddr_v, addr, GMAC_ADDR_HIGH(reg_n),
			    GMAC_ADDR_LOW(reg_n));
}

static void dwmac1000_get_umac_addr(struct mac_device_info *hw,
				    unsigned char *addr,
				    unsigned int reg_n)
{
	void __iomem *ioaddr_v = hw->pcsr;
	stmmac_get_mac_addr(ioaddr_v, addr, GMAC_ADDR_HIGH(reg_n),
			    GMAC_ADDR_LOW(reg_n));
}

static void dwmac1000_flow_ctrl(struct mac_device_info *hw, unsigned int duplex,
				unsigned int fc, unsigned int pause_time,
				u32 tx_cnt)
{
	void __iomem *ioaddr_v = hw->pcsr;
	/* Set flow such that DZPQ in Mac Register 6 is 0,
	 * and unicast pause detect is enabled.
	 */
	unsigned int flow = GMAC_FLOW_CTRL_UP;

	if (fc & FLOW_RX) {
		STMMAC_PMD_INFO("\tReceive Flow-Control ON\n");
		flow |= GMAC_FLOW_CTRL_RFE;
	}
	if (fc & FLOW_TX) {
		STMMAC_PMD_INFO("\tTransmit Flow-Control ON\n");
		flow |= GMAC_FLOW_CTRL_TFE;
	}

	if (duplex) {
		STMMAC_PMD_INFO("\tduplex mode: PAUSE %d\n", pause_time);
		flow |= (pause_time << GMAC_FLOW_CTRL_PT_SHIFT);
	}

	writel(flow, ioaddr_v + GMAC_FLOW_CTRL);
}

static void dwmac1000_pmt(struct mac_device_info *hw, unsigned long mode)
{
	void __iomem *ioaddr_v = hw->pcsr;
	unsigned int pmt = 0;

	if (mode & WAKE_MAGIC) {
		STMMAC_PMD_INFO("GMAC: WOL Magic frame\n");
		pmt |= power_down | magic_pkt_en;
	}
	if (mode & WAKE_UCAST) {
		STMMAC_PMD_INFO("GMAC: WOL on global unicast\n");
		pmt |= power_down | global_unicast | wake_up_frame_en;
	}

	writel(pmt, ioaddr_v + GMAC_PMT);
}

/* RGMII or SMII interface */
static void dwmac1000_rgsmii(void __iomem *ioaddr_v, struct stmmac_extra_stats *x)
{
	u32 status;

	status = readl(ioaddr_v + GMAC_RGSMIIIS);
	x->irq_rgmii_n++;

	/* Check the link status */
	if (status & GMAC_RGSMIIIS_LNKSTS) {
		int speed_value;

		x->pcs_link = 1;

		speed_value = ((status & GMAC_RGSMIIIS_SPEED) >>
			       GMAC_RGSMIIIS_SPEED_SHIFT);
		if (speed_value == GMAC_RGSMIIIS_SPEED_125)
			x->pcs_speed = SPEED_1000;
		else if (speed_value == GMAC_RGSMIIIS_SPEED_25)
			x->pcs_speed = SPEED_100;
		else
			x->pcs_speed = SPEED_10;

		x->pcs_duplex = (status & GMAC_RGSMIIIS_LNKMOD_MASK);

		STMMAC_PMD_INFO("Link is Up - %d/%s\n", (int)x->pcs_speed,
			x->pcs_duplex ? "Full" : "Half");
	} else {
		x->pcs_link = 0;
		STMMAC_PMD_INFO("Link is Down\n");
	}
}

static int dwmac1000_irq_status(struct mac_device_info *hw,
				struct stmmac_extra_stats *x)
{
	void __iomem *ioaddr_v = hw->pcsr;
	u32 intr_status = readl(ioaddr_v + GMAC_INT_STATUS);
	u32 intr_mask = readl(ioaddr_v + GMAC_INT_MASK);
	int ret = 0;

	/* Discard masked bits */
	intr_status &= ~intr_mask;

	/* Not used events (e.g. MMC interrupts) are not handled. */
	if ((intr_status & GMAC_INT_STATUS_MMCTIS))
		x->mmc_tx_irq_n++;
	if (unlikely(intr_status & GMAC_INT_STATUS_MMCRIS))
		x->mmc_rx_irq_n++;
	if (unlikely(intr_status & GMAC_INT_STATUS_MMCCSUM))
		x->mmc_rx_csum_offload_irq_n++;
	if (unlikely(intr_status & GMAC_INT_DISABLE_PMT)) {
		/* clear the PMT bits 5 and 6 by reading the PMT status reg */
		readl(ioaddr_v + GMAC_PMT);
		x->irq_receive_pmt_irq_n++;
	}

	/* MAC tx/rx EEE LPI entry/exit interrupts */
	if (intr_status & GMAC_INT_STATUS_LPIIS) {
		/* Clean LPI interrupt by reading the Reg 12 */
		ret = readl(ioaddr_v + LPI_CTRL_STATUS);

		if (ret & LPI_CTRL_STATUS_TLPIEN)
			x->irq_tx_path_in_lpi_mode_n++;
		if (ret & LPI_CTRL_STATUS_TLPIEX)
			x->irq_tx_path_exit_lpi_mode_n++;
		if (ret & LPI_CTRL_STATUS_RLPIEN)
			x->irq_rx_path_in_lpi_mode_n++;
		if (ret & LPI_CTRL_STATUS_RLPIEX)
			x->irq_rx_path_exit_lpi_mode_n++;
	}

	if (intr_status & PCS_RGSMIIIS_IRQ)
		dwmac1000_rgsmii(ioaddr_v, x);

	return ret;
}

static void dwmac1000_set_eee_mode(struct mac_device_info *hw,
				   bool en_tx_lpi_clockgating)
{
	void __iomem *ioaddr_v = hw->pcsr;
	u32 value;

	/*TODO - en_tx_lpi_clockgating treatment */

	/* Enable the link status receive on RGMII, SGMII ore SMII
	 * receive path and instruct the transmit to enter in LPI
	 * state.
	 */
	value = readl(ioaddr_v + LPI_CTRL_STATUS);
	value |= LPI_CTRL_STATUS_LPIEN | LPI_CTRL_STATUS_LPITXA;
	writel(value, ioaddr_v + LPI_CTRL_STATUS);
}

static void dwmac1000_reset_eee_mode(struct mac_device_info *hw)
{
	void __iomem *ioaddr_v = hw->pcsr;
	u32 value;

	value = readl(ioaddr_v + LPI_CTRL_STATUS);
	value &= ~(LPI_CTRL_STATUS_LPIEN | LPI_CTRL_STATUS_LPITXA);
	writel(value, ioaddr_v + LPI_CTRL_STATUS);
}

static void dwmac1000_set_eee_pls(struct mac_device_info *hw, int link)
{
	void __iomem *ioaddr_v = hw->pcsr;
	u32 value;

	value = readl(ioaddr_v + LPI_CTRL_STATUS);

	if (link)
		value |= LPI_CTRL_STATUS_PLS;
	else
		value &= ~LPI_CTRL_STATUS_PLS;

	writel(value, ioaddr_v + LPI_CTRL_STATUS);
}

static void dwmac1000_set_eee_timer(struct mac_device_info *hw, int ls, int tw)
{
	void __iomem *ioaddr_v = hw->pcsr;
	int value = ((tw & 0xffff)) | ((ls & 0x7ff) << 16);

	/* Program the timers in the LPI timer control register:
	 * LS: minimum time (ms) for which the link
	 *  status from PHY should be ok before transmitting
	 *  the LPI pattern.
	 * TW: minimum time (us) for which the core waits
	 *  after it has stopped transmitting the LPI pattern.
	 */
	writel(value, ioaddr_v + LPI_TIMER_CTRL);
}

static void dwmac1000_debug(void __iomem *ioaddr_v, struct stmmac_extra_stats *x,
			    u32 rx_queues, u32 tx_queues)
{
	u32 value = readl(ioaddr_v + GMAC_DEBUG);

	if (value & GMAC_DEBUG_TXSTSFSTS)
		x->mtl_tx_status_fifo_full++;
	if (value & GMAC_DEBUG_TXFSTS)
		x->mtl_tx_fifo_not_empty++;
	if (value & GMAC_DEBUG_TWCSTS)
		x->mmtl_fifo_ctrl++;
	if (value & GMAC_DEBUG_TRCSTS_MASK) {
		u32 trcsts = (value & GMAC_DEBUG_TRCSTS_MASK)
			     >> GMAC_DEBUG_TRCSTS_SHIFT;
		if (trcsts == GMAC_DEBUG_TRCSTS_WRITE)
			x->mtl_tx_fifo_read_ctrl_write++;
		else if (trcsts == GMAC_DEBUG_TRCSTS_TXW)
			x->mtl_tx_fifo_read_ctrl_wait++;
		else if (trcsts == GMAC_DEBUG_TRCSTS_READ)
			x->mtl_tx_fifo_read_ctrl_read++;
		else
			x->mtl_tx_fifo_read_ctrl_idle++;
	}
	if (value & GMAC_DEBUG_TXPAUSED)
		x->mac_tx_in_pause++;
	if (value & GMAC_DEBUG_TFCSTS_MASK) {
		u32 tfcsts = (value & GMAC_DEBUG_TFCSTS_MASK)
			      >> GMAC_DEBUG_TFCSTS_SHIFT;

		if (tfcsts == GMAC_DEBUG_TFCSTS_XFER)
			x->mac_tx_frame_ctrl_xfer++;
		else if (tfcsts == GMAC_DEBUG_TFCSTS_GEN_PAUSE)
			x->mac_tx_frame_ctrl_pause++;
		else if (tfcsts == GMAC_DEBUG_TFCSTS_WAIT)
			x->mac_tx_frame_ctrl_wait++;
		else
			x->mac_tx_frame_ctrl_idle++;
	}
	if (value & GMAC_DEBUG_TPESTS)
		x->mac_gmii_tx_proto_engine++;
	if (value & GMAC_DEBUG_RXFSTS_MASK) {
		u32 rxfsts = (value & GMAC_DEBUG_RXFSTS_MASK)
			     >> GMAC_DEBUG_RRCSTS_SHIFT;

		if (rxfsts == GMAC_DEBUG_RXFSTS_FULL)
			x->mtl_rx_fifo_fill_level_full++;
		else if (rxfsts == GMAC_DEBUG_RXFSTS_AT)
			x->mtl_rx_fifo_fill_above_thresh++;
		else if (rxfsts == GMAC_DEBUG_RXFSTS_BT)
			x->mtl_rx_fifo_fill_below_thresh++;
		else
			x->mtl_rx_fifo_fill_level_empty++;
	}
	if (value & GMAC_DEBUG_RRCSTS_MASK) {
		u32 rrcsts = (value & GMAC_DEBUG_RRCSTS_MASK) >>
			     GMAC_DEBUG_RRCSTS_SHIFT;

		if (rrcsts == GMAC_DEBUG_RRCSTS_FLUSH)
			x->mtl_rx_fifo_read_ctrl_flush++;
		else if (rrcsts == GMAC_DEBUG_RRCSTS_RSTAT)
			x->mtl_rx_fifo_read_ctrl_read_data++;
		else if (rrcsts == GMAC_DEBUG_RRCSTS_RDATA)
			x->mtl_rx_fifo_read_ctrl_status++;
		else
			x->mtl_rx_fifo_read_ctrl_idle++;
	}
	if (value & GMAC_DEBUG_RWCSTS)
		x->mtl_rx_fifo_ctrl_active++;
	if (value & GMAC_DEBUG_RFCFCSTS_MASK)
		x->mac_rx_frame_ctrl_fifo = (value & GMAC_DEBUG_RFCFCSTS_MASK)
					    >> GMAC_DEBUG_RFCFCSTS_SHIFT;
	if (value & GMAC_DEBUG_RPESTS)
		x->mac_gmii_rx_proto_engine++;
}

const struct stmmac_ops dwmac1000_ops = {
	.core_init = dwmac1000_core_init,
	.set_mac = stmmac_set_mac,
	.rx_ipc = dwmac1000_rx_ipc_enable,
	.dump_regs = dwmac1000_dump_regs,
	.host_irq_status = dwmac1000_irq_status,
	.flow_ctrl = dwmac1000_flow_ctrl,
	.pmt = dwmac1000_pmt,
	.set_umac_addr = dwmac1000_set_umac_addr,
	.get_umac_addr = dwmac1000_get_umac_addr,
	.set_eee_mode = dwmac1000_set_eee_mode,
	.reset_eee_mode = dwmac1000_reset_eee_mode,
	.set_eee_timer = dwmac1000_set_eee_timer,
	.set_eee_pls = dwmac1000_set_eee_pls,
	.debug = dwmac1000_debug,
};

int dwmac1000_setup(struct stmmac_private *priv)
{
	struct mac_device_info *mac = priv->hw;

	STMMAC_PMD_LOG(INFO,  "\tDWMAC1000\n");

	//priv->dev->priv_flags |= IFF_UNICAST_FLT;
	mac->pcsr = priv->ioaddr_v;
	//mac->multicast_filter_bins = priv->plat->multicast_filter_bins;
	//mac->unicast_filter_entries = priv->plat->unicast_filter_entries;
	//mac->mcast_bits_log2 = 0;

	//if (mac->multicast_filter_bins)
	//	mac->mcast_bits_log2 = ilog2(mac->multicast_filter_bins);

	mac->link.duplex = GMAC_CONTROL_DM;
	mac->link.speed10 = GMAC_CONTROL_PS;
	mac->link.speed100 = GMAC_CONTROL_PS | GMAC_CONTROL_FES;
	mac->link.speed1000 = 0;
	mac->link.speed_mask = GMAC_CONTROL_PS | GMAC_CONTROL_FES;
	//mac->mii.addr = GMAC_MII_ADDR;
	//mac->mii.data = GMAC_MII_DATA;
	//mac->mii.addr_shift = 11;
	//mac->mii.addr_mask = 0x0000F800;
	//mac->mii.reg_shift = 6;
	//mac->mii.reg_mask = 0x000007C0;
	//mac->mii.clk_csr_shift = 2;
	//mac->mii.clk_csr_mask = GENMASK(5, 2);

	return 0;
}
