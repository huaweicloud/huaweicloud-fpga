/*-
 *   BSD LICENSE
 *
 *   Copyright(c)  2017 Huawei Technologies Co., Ltd. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Huawei Technologies Co., Ltd  nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef	_REGS_INFOS_H_
#define	_REGS_INFOS_H_

#if defined __KU115__
#define SPACE_SIZE_EVERY_VF                     (36 * 16 * 1024)
#define SPACE_SIZE_OFFSET_VF_IN_PF              (3UL * 1024UL * 1024UL + 8UL * 16UL * 1024UL)
#define SPACE_SIZE_OFFSET_QUE0_IN_VF            (0)
#elif defined __VU9P__
#define SPACE_SIZE_EVERY_VF                     (1024 * 1024)
#define SPACE_SIZE_OFFSET_VF_IN_PF              (3UL * 1024UL * 1024UL)
#define SPACE_SIZE_OFFSET_QUE0_IN_VF            (8UL * 16UL * 1024UL)
#else
#error  Correct Logic type should be defined
#endif

#define IP_REG_LENGTH_UNIT          				(16UL * 4UL * 1024UL)   
#define	ONE_REG_BYTE_LEN							(4)

/* PF's mailbox registers infos, see by PF */
#define	REG_PF_MAILBOX_BASE							(0x00000000)
#define	REG_OFFSET_PF_MAILBOX_INT_CAUSE				(REG_PF_MAILBOX_BASE + 0x00 * 4)
#define	REG_OFFSET_PF_MAILBOX_INT_MASK				(REG_PF_MAILBOX_BASE + 0x02 * 4)
#define	REG_OFFSET_PF_MAILBOX_INT_ENABLE			(REG_PF_MAILBOX_BASE + 0x03 * 4)
#define	REG_OFFSET_PF_MAILBOX_INT_LOCK				(REG_PF_MAILBOX_BASE + 0x04 * 4)
#define	REG_OFFSET_PF_MAILBOX_PF_CTRL_VF0			(REG_PF_MAILBOX_BASE + 0x20 * 4)
#define	REG_OFFSET_PF_MAILBOX_PF_CTRL_VF1			(REG_PF_MAILBOX_BASE + 0x21 * 4)
#define	REG_OFFSET_PF_MAILBOX_PF_CTRL_VF2			(REG_PF_MAILBOX_BASE + 0x22 * 4)
#define	REG_OFFSET_PF_MAILBOX_PF_CTRL_VF3			(REG_PF_MAILBOX_BASE + 0x23 * 4)


/* VF's mailbox registers infos, see by VF */
#define	REG_VF_MAILBOX_BASE							(0x00000000)
#define	REG_VF_MAILBOX_CTRL							(REG_VF_MAILBOX_BASE + 0x00 * 4)
#define	REG_VF_MAILBOX_INT_MASK						(REG_VF_MAILBOX_BASE + 0x02 * 4)
#define	REG_VF_MAILBOX_MSG0							(REG_VF_MAILBOX_BASE + 0x10 * 4)
#define	REG_VF_MAILBOX_MSG1							(REG_VF_MAILBOX_BASE + 0x11 * 4)
#define	REG_VF_MAILBOX_MSG2							(REG_VF_MAILBOX_BASE + 0x12 * 4)
#define	REG_VF_MAILBOX_MSG3							(REG_VF_MAILBOX_BASE + 0x13 * 4)
#define	REG_VF_MAILBOX_MSG4							(REG_VF_MAILBOX_BASE + 0x14 * 4)
#define	REG_VF_MAILBOX_MSG5							(REG_VF_MAILBOX_BASE + 0x15 * 4)
#define	REG_VF_MAILBOX_MSG6							(REG_VF_MAILBOX_BASE + 0x16 * 4)
#define	REG_VF_MAILBOX_MSG7							(REG_VF_MAILBOX_BASE + 0x17 * 4)
#define	REG_VF_MAILBOX_MSG8							(REG_VF_MAILBOX_BASE + 0x18 * 4)
#define	REG_VF_MAILBOX_MSG9							(REG_VF_MAILBOX_BASE + 0x19 * 4)
#define	REG_VF_MAILBOX_MSG10						(REG_VF_MAILBOX_BASE + 0x1A * 4)
#define	REG_VF_MAILBOX_MSG11						(REG_VF_MAILBOX_BASE + 0x1B * 4)
#define	REG_VF_MAILBOX_MSG12						(REG_VF_MAILBOX_BASE + 0x1C * 4)
#define	REG_VF_MAILBOX_MSG13						(REG_VF_MAILBOX_BASE + 0x1D * 4)
#define	REG_VF_MAILBOX_MSG14						(REG_VF_MAILBOX_BASE + 0x1E * 4)
#define	REG_VF_MAILBOX_MSG15						(REG_VF_MAILBOX_BASE + 0x1F * 4)


/* VF0's mailbox registers infos, see by PF */
#define	REG_VF0_IN_PF_MAILBOX_BASE					(SPACE_SIZE_OFFSET_VF_IN_PF + REG_VF_MAILBOX_BASE)
#define	REG_VF0_IN_PF_MAILBOX_CTRL					(REG_VF0_IN_PF_MAILBOX_BASE + 0x00 * 4)
#define	REG_VF0_IN_PF_MAILBOX_INT_MASK				(REG_VF0_IN_PF_MAILBOX_BASE + 0x02 * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG0					(REG_VF0_IN_PF_MAILBOX_BASE + 0x10 * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG1					(REG_VF0_IN_PF_MAILBOX_BASE + 0x11 * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG2					(REG_VF0_IN_PF_MAILBOX_BASE + 0x12 * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG3					(REG_VF0_IN_PF_MAILBOX_BASE + 0x13 * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG4					(REG_VF0_IN_PF_MAILBOX_BASE + 0x14 * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG5					(REG_VF0_IN_PF_MAILBOX_BASE + 0x15 * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG6					(REG_VF0_IN_PF_MAILBOX_BASE + 0x16 * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG7					(REG_VF0_IN_PF_MAILBOX_BASE + 0x17 * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG8					(REG_VF0_IN_PF_MAILBOX_BASE + 0x18 * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG9					(REG_VF0_IN_PF_MAILBOX_BASE + 0x19 * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG10					(REG_VF0_IN_PF_MAILBOX_BASE + 0x1A * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG11					(REG_VF0_IN_PF_MAILBOX_BASE + 0x1B * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG12					(REG_VF0_IN_PF_MAILBOX_BASE + 0x1C * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG13					(REG_VF0_IN_PF_MAILBOX_BASE + 0x1D * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG14					(REG_VF0_IN_PF_MAILBOX_BASE + 0x1E * 4)
#define	REG_VF0_IN_PF_MAILBOX_MSG15					(REG_VF0_IN_PF_MAILBOX_BASE + 0x1F * 4)

#define REG_PF_BASE_BASE							(0x0000C000)
#define	REG_PF_BASE_VERNUM_REG						(REG_PF_BASE_BASE + 0x00 * 4)
#define	REG_PF_BASE_VERDATEH_REG					(REG_PF_BASE_BASE + 0x01 * 4)
#define	REG_PF_BASE_VERSUB_REG						(REG_PF_BASE_BASE + 0x02 * 4)
#define	REG_PF_BASE_REG_TEST_REG					(REG_PF_BASE_BASE + 0x10 * 4)
#define	REG_PF_BASE_REG_ADDR_TEST_REG				(REG_PF_BASE_BASE + 0x11 * 4)
#define	REG_PF_BASE_REG_CNT_CLR_REG					(REG_PF_BASE_BASE + 0x12 * 4)

#define	REG_PF_RX_ADP_BASE							(0x00028000)
#define	REG_PF_RX_ADP_CFG_ROUTE_REG					(REG_PF_RX_ADP_BASE + 0x1E * 4)

#define	REG_PF_ETH_BASE								(0x00034000)
#define	REG_PF_ETH_LOOP_CFG							(REG_PF_ETH_BASE + 0x01 * 4)
#define REG_PF_ETH_PORT_CFG	   						(REG_PF_ETH_BASE + 0x02 * 4)   /* Port configuration reg */

#define	REG_PF_ISO_BASE								(0x30000)
#define	REG_PF_IOS_ENABLE							(REG_PF_ISO_BASE + 0x00 * 4)

#define	REG_PF_DEMO1_BASE							(0x00248000)
#define	REG_PF_DEMO1_VERSION						(REG_PF_DEMO1_BASE + 0x00 * 4)
#define	REG_PF_DEMO1_ADDER_CFG_WDATA0				(REG_PF_DEMO1_BASE + 0x01 * 4)
#define	REG_PF_DEMO1_ADDER_CFG_WDATA1				(REG_PF_DEMO1_BASE + 0x02 * 4)
#define	REG_PF_DEMO1_SUM_RDATA						(REG_PF_DEMO1_BASE + 0x03 * 4)
#define	REG_PF_OPPOS_DATA							(REG_PF_DEMO1_BASE + 0x04 * 4)



#define	IP_REG_BASE_PF								(SPACE_SIZE_OFFSET_VF_IN_PF + SPACE_SIZE_OFFSET_QUE0_IN_VF)
#define	IP_SA_REG_BASE_PF							(IP_REG_BASE_PF)
#define	IP_INLINE_IN_REG_BASE_PF					(IP_SA_REG_BASE_PF + IP_REG_LENGTH_UNIT)
#define	IP_INLINE_OUT_REG_BASE_PF					(IP_INLINE_IN_REG_BASE_PF + IP_REG_LENGTH_UNIT)

/* SA IP Registers: HPI */
#define	SA_HPI_REG_BASE_PF							(IP_SA_REG_BASE_PF + 0x00000800)
#define SA_HPI_REG_SA_EN_PF                         (SA_HPI_REG_BASE_PF + 0x10C)
/* SA IP Registers: INQ */
#define	SA_INQ_REG_BASE_PF							(IP_SA_REG_BASE_PF + 0x00001000)
#define SA_INQ_REG_WORK_EN_PF                       (SA_INQ_REG_BASE_PF + 0x100)
#define SA_INQ_REG_PHY_FLOW_OP_PF                   (SA_INQ_REG_BASE_PF + 0x130)
/* SA IP Registers: HIU */
#define SA_HIU_REG_BASE_PF                          (IP_SA_REG_BASE_PF + 0x3000)
#define SA_HIU_REG_SIPEN_PF                         (SA_HIU_REG_BASE_PF + 0x104)
/* SA IP Registers: MMU */
#define SA_MMU_REG_BASE_PF                          (IP_SA_REG_BASE_PF + 0x00006000)
#define SA_MMU_REG_WORK_EN_PF                       (SA_MMU_REG_BASE_PF + 0x100)
#define	SA_MMU_REG_INQ_DONE_STATUS_PF				(SA_MMU_REG_BASE_PF + 0x300)
#define	SA_MMU_REG_TCP_DONE_STATUS_PF				(SA_MMU_REG_BASE_PF + 0x304)
#define SA_MMU_REG_PARSE_DONE_STATUS_PF             (SA_MMU_REG_BASE_PF + 0x308)

/* InLine IP Registers: HPI*/
#define	INLINE_HPI_REG_BASE_PF						(IP_INLINE_IN_REG_BASE_PF + 0x00000800)
#define INLINE_HPI_REG_IA_EN_PF                     (INLINE_HPI_REG_BASE_PF + 0x10C)
#define INLINE_HPI_REG_DDR_CAL_INIT_STATUS_PF       (INLINE_HPI_REG_BASE_PF + 0x410)
/* InLine IP Registers: PKM */
#define INLINE_PKM_REG_BASE_PF                      (IP_INLINE_IN_REG_BASE_PF + 0x1000)
#define INLINE_PKM_REG_COS2TOS_EN_PF                (INLINE_PKM_REG_BASE_PF + 0x100)
#define INLINE_PKM_REG_TMF_EN_PF                    (INLINE_PKM_REG_BASE_PF + 0x104)
/* InLine IN Registers: ACL IN*/
#define	INLINE_ACL_IN_REG_BASE_PF					(IP_INLINE_IN_REG_BASE_PF + 0x00002000)
#define	INLINE_ACL_IN_REG_ENABLE_PF					(INLINE_ACL_IN_REG_BASE_PF + 0x100)

/* InLine IP Registers: ACL OUT */
#define INLINE_ACL_OUT_REG_BASE_PF                  (IP_INLINE_IN_REG_BASE_PF + 0x2000)
#define	INLINE_ACL_OUT_REG_ENABLE_PF				(INLINE_ACL_OUT_REG_BASE_PF + 0x108)



/* In Bar1 */
#define	REG_PF_DEMO1_BASE_NEW							(0)
#define	REG_PF_DEMO1_VERSION_NEW						(REG_PF_DEMO1_BASE_NEW + 0x00 * 4)
#define	REG_PF_DEMO1_ADDER_CFG_WDATA0_NEW				(REG_PF_DEMO1_BASE_NEW + 0x02 * 4)
#define	REG_PF_DEMO1_ADDER_CFG_WDATA1_NEW				(REG_PF_DEMO1_BASE_NEW + 0x03 * 4)
#define	REG_PF_DEMO1_SUM_RDATA_NEW						(REG_PF_DEMO1_BASE_NEW + 0x04 * 4)
#define	REG_PF_OPPOS_DATA_NEW							(REG_PF_DEMO1_BASE_NEW + 0x05 * 4)

/* DDR status addr */
#define REG_VF_DEMO1_BASE_ADDR                          (0)
#define REG_VF_DEMO1_DDRA_ADDR                          (REG_VF_DEMO1_BASE_ADDR + 0x3c0052 * 4)
#define REG_VF_DEMO1_DDRB_ADDR                          (REG_VF_DEMO1_BASE_ADDR + 0x3c1052 * 4)
#define REG_VF_DEMO1_DDRC_ADDR                          (REG_VF_DEMO1_BASE_ADDR + 0x17052  * 4)
#define REG_VF_DEMO1_DDRD_ADDR                          (REG_VF_DEMO1_BASE_ADDR + 0x3c2052 * 4)

#endif	// _REGS_INFOS_H_