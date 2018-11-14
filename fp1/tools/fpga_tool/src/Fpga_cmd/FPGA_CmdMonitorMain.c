/*-
 *   BSD LICENSE
 *
 *   Copyright(c)  2017-2018 Huawei Technologies Co., Ltd. All rights reserved.
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

#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <getopt.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

#include "securec.h"
#include "FPGA_CmdCommon.h"
#include "FPGA_CmdMonitorMain.h"
#include "FPGA_CmdParse.h"

#ifdef    __cplusplus
extern "C"{
#endif


FPGA_CMD_PARA   g_strFpgaModule = { 0 };

COMMAND_PROC_FUNC g_pafnFpgaCmdList[CMD_PARSE_END] =
{
    [CMD_HFI_LOAD] = FPGA_MonitorLoadHfi,
    [CMD_HFI_CLEAR] = FPGA_MonitorClearHfi,
    [CMD_IMAGE_INQUIRE] = FPGA_MonitorInquireFpgaImageInfo,
    [CMD_RESOURSE_INQUIRE] = FPGA_MonitorDisplayDevice,
    [CMD_STATUS_INQUIRE] = FPGA_MonitorAlmMsgs,
    [CMD_LED_STATUS_INQUIRE] = FPGA_MonitorInquireLEDStatus,
    [CMD_TOOL_VERSION] = NULL,
};
UINT32 g_ulparseParaFlag = 0;

typedef struct {
    UINT32 ulRegBase;           /* 寄存器基址 32位 */ 
    UINT32 RegIdx:9;           /* 寄存器列表最长为 512*/
    UINT32 Bit:6;               /* 告警寄存器bit 最大为63 */
    UINT32 Lvl:3;               /* 级别最多为 8 */
    UINT32 ucAlmCode:10;         /* 告警码偏移 最多为 1024 */
    UINT32 ucNoAlmValue:1;
    UINT32 Reserve:3;           /* 预留 2 bit: 最大值 4 */
}FPGA_ALM_REG_INFO;

typedef struct {
    UINT8 ucBitVal;
    UINT32 ulAlmCodeBase;
    FPGA_ALM_REG_INFO* pstrRegInfo;
}FPGA_ALM_RST;

enum {
    FPGA_LOGIC_NO_TYPE = 0,
    FPGA_LOGIC_OCL_TYPE,
    FPGA_LOGIC_DPDK_TYPE
};

enum {
    FPGA_ALM_LOGGING = 0,
    FPGA_ALM_SHIELDING = 1,
    FPGA_ALM_ALL = 2,
    FPGA_ALM_MALFUNC = 3,
    FPGA_ALM_END = 8
};

enum {
    FPGA_SF_ALM = 0,
    FPGA_SF_STATS,
    FPGA_SF_CONFIG,
    FPGA_SF_MAX = 16
};

#define REG_CLKM_ERR                   0
#define REG_PARRLT_ERR                 1
#define REG_RSPBD_ERR                  2
#define REG_DMAE_0ERR                  3     
#define REG_DMAE_1ERR                   4
#define REG_BDQM_ERR                    5
#define REG_MULQM_ERR0                  6
#define REG_TXQM_STATUS_HISTORY         7
#define REG_PRTY_ERR                    8
#define REG_TXM_ERR                     9   
#define REG_TXM_STATUS_HISTORY          10
#define RXM_REQ_BUSY                    11
#define REG_BDQM_ERR_3RD                12
#define REG_TRM_FF_ERR                  13
#define REG_HPI_TXFF_ERR                14
#define REG_BAR_ACCESS_ERR              15
#define AXI_FIFO_STATUS                 16
#define AXI_ADDR_ERR                    17
#define CM_STATUS                       18
#define DC_STATUS                       19
#define MAC_STATUS                      20
#define SYS_FIFO_STAT                   21
#define SYS_FLAG                        22
#define TX_FIFO_STAT1                   23
#define TX_FIFO_STAT2                   24
#define BUS_FIFO_STAT1                  25
#define DMA_SOP_EOP_CHK_ERROR           26
#define USR_FIFO_STATUS                 27
#define RX_LEN_UNVLD_CNT                28
#define RX_STAT_UNVLD_CNT               29
#define RX_TAG_UNVLD_CNT                30
#define PLD_TIMEOUT_CNT                 31

#define SDX_REG_SYS_ERR                    0
#define SDX_REG_DDR0_ECC_STATUS            1
#define SDX_REG_DDR1_ECC_STATUS            2
#define SDX_FIREWALL_CTRL_STATUS           3     
#define SDX_FIREWALL_USR_CTRL_STATUS      4
#define SDX_FIREWALL_USR_DATA_STATUS      5

#define NORMAL_0                            0
#define NORMAL_1                            1
#define REG_BIT(X)                         X
#define RESERVE                            0


static FPGA_ALM_REG_INFO DPDKAlmRegInfoTbl[]=
{
    {0x44080, REG_PARRLT_ERR, REG_BIT(7),  FPGA_ALM_LOGGING,  40, NORMAL_0, RESERVE},
    {0x44080, REG_PARRLT_ERR, REG_BIT(6),  FPGA_ALM_LOGGING,  41, NORMAL_0, RESERVE},
    {0x44080, REG_PARRLT_ERR, REG_BIT(4),  FPGA_ALM_LOGGING,  43, NORMAL_0, RESERVE},
    {0x44080, REG_PARRLT_ERR, REG_BIT(3),  FPGA_ALM_LOGGING,  44, NORMAL_0, RESERVE},
    {0x44080, REG_PARRLT_ERR, REG_BIT(2),  FPGA_ALM_LOGGING,  45, NORMAL_0, RESERVE},
    {0x44080, REG_PARRLT_ERR, REG_BIT(1),  FPGA_ALM_LOGGING,  46, NORMAL_0, RESERVE},
    {0x44080, REG_PARRLT_ERR, REG_BIT(0),  FPGA_ALM_LOGGING,  47, NORMAL_0, RESERVE},
    {0x44081, REG_RSPBD_ERR, REG_BIT(8),  FPGA_ALM_LOGGING,  71, NORMAL_0, RESERVE},
    {0x44081, REG_RSPBD_ERR, REG_BIT(7),  FPGA_ALM_LOGGING,  72, NORMAL_0, RESERVE},
    {0x44081, REG_RSPBD_ERR, REG_BIT(6),  FPGA_ALM_LOGGING,  73, NORMAL_0, RESERVE},
    {0x44081, REG_RSPBD_ERR, REG_BIT(5),  FPGA_ALM_LOGGING,  74, NORMAL_0, RESERVE},
    {0x44081, REG_RSPBD_ERR, REG_BIT(4),  FPGA_ALM_LOGGING,  75, NORMAL_0, RESERVE},
    {0x44081, REG_RSPBD_ERR, REG_BIT(3),  FPGA_ALM_LOGGING,  76, NORMAL_0, RESERVE},
    {0x44081, REG_RSPBD_ERR, REG_BIT(2),  FPGA_ALM_LOGGING,  77, NORMAL_0, RESERVE},
    {0x40080, REG_DMAE_0ERR, REG_BIT(30), FPGA_ALM_LOGGING,  81, NORMAL_0, RESERVE},
    {0x40080, REG_DMAE_0ERR, REG_BIT(26), FPGA_ALM_LOGGING,  85, NORMAL_0, RESERVE},
    {0x41080, REG_BDQM_ERR, REG_BIT(5),  FPGA_ALM_LOGGING,  121, NORMAL_0, RESERVE},
    {0x41080, REG_BDQM_ERR, REG_BIT(3),  FPGA_ALM_LOGGING,  123, NORMAL_0, RESERVE},
    {0x41081, REG_MULQM_ERR0, REG_BIT(3),  FPGA_ALM_LOGGING,  127, NORMAL_0, RESERVE},
    {0x41081, REG_MULQM_ERR0, REG_BIT(2),  FPGA_ALM_LOGGING,  128, NORMAL_0, RESERVE},
    {0x41081, REG_MULQM_ERR0, REG_BIT(1),  FPGA_ALM_LOGGING,  129, NORMAL_0, RESERVE},
    {0x41081, REG_MULQM_ERR0, REG_BIT(0),  FPGA_ALM_LOGGING,  130, NORMAL_0, RESERVE},
    {0x42081, REG_TXM_ERR, REG_BIT(11), FPGA_ALM_LOGGING,  138, NORMAL_0, RESERVE},
    {0x42081, REG_TXM_ERR, REG_BIT(10), FPGA_ALM_LOGGING,  139, NORMAL_0, RESERVE},
    {0x42081, REG_TXM_ERR, REG_BIT(9),  FPGA_ALM_LOGGING,  140, NORMAL_0, RESERVE},
    {0x00B080, REG_TRM_FF_ERR,  REG_BIT(6),  FPGA_ALM_LOGGING,  220, NORMAL_0, RESERVE},
    {0x17048, AXI_FIFO_STATUS,  REG_BIT(30), FPGA_ALM_LOGGING,  302, NORMAL_0, RESERVE},
    {0x17048, AXI_FIFO_STATUS,  REG_BIT(22), FPGA_ALM_LOGGING,  310, NORMAL_0, RESERVE},
    {0x17048, AXI_FIFO_STATUS,  REG_BIT(14), FPGA_ALM_LOGGING,  318, NORMAL_0, RESERVE},
    {0x17048, AXI_FIFO_STATUS,  REG_BIT(6),  FPGA_ALM_LOGGING,  326, NORMAL_0, RESERVE},
    {0x1704c, AXI_ADDR_ERR,  REG_BIT(9),  FPGA_ALM_LOGGING,  355, NORMAL_0, RESERVE},
    {0x1704c, AXI_ADDR_ERR,  REG_BIT(8),  FPGA_ALM_LOGGING,  356, NORMAL_0, RESERVE},
    {0x17120, CM_STATUS, REG_BIT(14), FPGA_ALM_LOGGING,  382, NORMAL_0, RESERVE},
    {0x17120, CM_STATUS, REG_BIT(13), FPGA_ALM_LOGGING,  383, NORMAL_0, RESERVE},
    {0x17120, CM_STATUS, REG_BIT(6),  FPGA_ALM_LOGGING,  390, NORMAL_0, RESERVE},
    {0x17124, DC_STATUS, REG_BIT(22), FPGA_ALM_LOGGING,  406, NORMAL_0, RESERVE},
    {0x17124, DC_STATUS, REG_BIT(7),  FPGA_ALM_LOGGING,  421, NORMAL_0, RESERVE},
    {0x17124, DC_STATUS, REG_BIT(6),  FPGA_ALM_LOGGING,  422, NORMAL_0, RESERVE},
    {0x17148, MAC_STATUS, REG_BIT(30), FPGA_ALM_LOGGING,  430, NORMAL_0, RESERVE},
    {0x17148, MAC_STATUS, REG_BIT(22), FPGA_ALM_LOGGING,  438, NORMAL_0, RESERVE},
    {0x17148, MAC_STATUS, REG_BIT(7),  FPGA_ALM_LOGGING,  453, NORMAL_0, RESERVE},
    {0x17148, MAC_STATUS, REG_BIT(6),  FPGA_ALM_LOGGING,  454, NORMAL_0, RESERVE},
    {0x004080, SYS_FIFO_STAT,  REG_BIT(14), FPGA_ALM_LOGGING,  461, NORMAL_0, RESERVE},
    {0x004080, SYS_FIFO_STAT,  REG_BIT(13), FPGA_ALM_LOGGING,  462, NORMAL_0, RESERVE},
    {0x004080, SYS_FIFO_STAT,  REG_BIT(12), FPGA_ALM_LOGGING,  463, NORMAL_0, RESERVE},
    {0x004080, SYS_FIFO_STAT,  REG_BIT(6),  FPGA_ALM_LOGGING,  464, NORMAL_0, RESERVE},
    {0x004080, SYS_FIFO_STAT,  REG_BIT(5),  FPGA_ALM_LOGGING,  465, NORMAL_0, RESERVE},
    {0x004080, SYS_FIFO_STAT,  REG_BIT(4),  FPGA_ALM_LOGGING,  466, NORMAL_0, RESERVE},
    {0x004011, SYS_FLAG,  REG_BIT(8),  FPGA_ALM_LOGGING,  467, NORMAL_0, RESERVE},
    {0x004011, SYS_FLAG,  REG_BIT(0),  FPGA_ALM_LOGGING,  468, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(30), FPGA_ALM_LOGGING,  469, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(29), FPGA_ALM_LOGGING,  470, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(28), FPGA_ALM_LOGGING,  471, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(22), FPGA_ALM_LOGGING,  472, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(21), FPGA_ALM_LOGGING,  473, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(20), FPGA_ALM_LOGGING,  474, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(14), FPGA_ALM_LOGGING,  475, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(13), FPGA_ALM_LOGGING,  476, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(12), FPGA_ALM_LOGGING,  477, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(6),  FPGA_ALM_LOGGING,  478, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(5),  FPGA_ALM_LOGGING,  479, NORMAL_0, RESERVE},
    {0x004040, TX_FIFO_STAT1, REG_BIT(4),  FPGA_ALM_LOGGING,  480, NORMAL_0, RESERVE},
    {0x004041, TX_FIFO_STAT2, REG_BIT(22), FPGA_ALM_LOGGING,  481, NORMAL_0, RESERVE},
    {0x004041, TX_FIFO_STAT2, REG_BIT(21), FPGA_ALM_LOGGING,  482, NORMAL_0, RESERVE},
    {0x004041, TX_FIFO_STAT2, REG_BIT(20), FPGA_ALM_LOGGING,  483, NORMAL_0, RESERVE},
    {0x004041, TX_FIFO_STAT2, REG_BIT(14), FPGA_ALM_LOGGING,  484, NORMAL_0, RESERVE},
    {0x004041, TX_FIFO_STAT2, REG_BIT(13), FPGA_ALM_LOGGING,  485, NORMAL_0, RESERVE},
    {0x004041, TX_FIFO_STAT2, REG_BIT(12), FPGA_ALM_LOGGING,  486, NORMAL_0, RESERVE},
    {0x004041, TX_FIFO_STAT2, REG_BIT(6),  FPGA_ALM_LOGGING,  487, NORMAL_0, RESERVE},
    {0x004041, TX_FIFO_STAT2, REG_BIT(5),  FPGA_ALM_LOGGING,  488, NORMAL_0, RESERVE},
    {0x004041, TX_FIFO_STAT2, REG_BIT(4),  FPGA_ALM_LOGGING,  489, NORMAL_0, RESERVE},
    {0x004042, BUS_FIFO_STAT1, REG_BIT(30), FPGA_ALM_LOGGING,  490, NORMAL_0, RESERVE},
    {0x004042, BUS_FIFO_STAT1, REG_BIT(29), FPGA_ALM_LOGGING,  491, NORMAL_0, RESERVE},
    {0x004042, BUS_FIFO_STAT1, REG_BIT(28), FPGA_ALM_LOGGING,  492, NORMAL_0, RESERVE},
    {0x004042, BUS_FIFO_STAT1, REG_BIT(14), FPGA_ALM_LOGGING,  493, NORMAL_0, RESERVE},
    {0x004042, BUS_FIFO_STAT1, REG_BIT(13), FPGA_ALM_LOGGING,  494, NORMAL_0, RESERVE},
    {0x004042, BUS_FIFO_STAT1, REG_BIT(12), FPGA_ALM_LOGGING,  495, NORMAL_0, RESERVE},
    {0x004042, BUS_FIFO_STAT1, REG_BIT(6),  FPGA_ALM_LOGGING,  496, NORMAL_0, RESERVE},
    {0x004042, BUS_FIFO_STAT1, REG_BIT(5),  FPGA_ALM_LOGGING,  497, NORMAL_0, RESERVE},
    {0x004042, BUS_FIFO_STAT1, REG_BIT(4),  FPGA_ALM_LOGGING,  498, NORMAL_0, RESERVE},
    {0x004044, DMA_SOP_EOP_CHK_ERROR, REG_BIT(24), FPGA_ALM_LOGGING,  499, NORMAL_0, RESERVE},
    {0x00404a, USR_FIFO_STATUS, REG_BIT(14), FPGA_ALM_LOGGING,  501, NORMAL_0, RESERVE},
    {0x00404a, USR_FIFO_STATUS, REG_BIT(13), FPGA_ALM_LOGGING,  502, NORMAL_0, RESERVE},
    {0x00404a, USR_FIFO_STATUS, REG_BIT(12), FPGA_ALM_LOGGING,  503, NORMAL_0, RESERVE},
    {0x00404a, USR_FIFO_STATUS, REG_BIT(6),  FPGA_ALM_LOGGING,  504, NORMAL_0, RESERVE},
    {0x00404a, USR_FIFO_STATUS, REG_BIT(5),  FPGA_ALM_LOGGING,  505, NORMAL_0, RESERVE},
    {0x00404a, USR_FIFO_STATUS, REG_BIT(4),  FPGA_ALM_LOGGING,  506, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(15), FPGA_ALM_LOGGING,  507, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(14), FPGA_ALM_LOGGING,  508, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(13), FPGA_ALM_LOGGING,  509, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(12), FPGA_ALM_LOGGING,  510, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(11), FPGA_ALM_LOGGING,  511, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(10), FPGA_ALM_LOGGING,  512, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(15), FPGA_ALM_LOGGING,  513, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(14), FPGA_ALM_LOGGING,  514, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(13), FPGA_ALM_LOGGING,  515, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(12), FPGA_ALM_LOGGING,  516, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(11), FPGA_ALM_LOGGING,  517, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(10), FPGA_ALM_LOGGING,  518, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(9),  FPGA_ALM_LOGGING,  519, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(8),  FPGA_ALM_LOGGING,  520, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(7),  FPGA_ALM_LOGGING,  521, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(6),  FPGA_ALM_LOGGING,  522, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(5),  FPGA_ALM_LOGGING,  523, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(4),  FPGA_ALM_LOGGING,  524, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(3),  FPGA_ALM_LOGGING,  525, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(2),  FPGA_ALM_LOGGING,  526, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(1),  FPGA_ALM_LOGGING,  527, NORMAL_0, RESERVE},
    {0x004081, RX_LEN_UNVLD_CNT, REG_BIT(0),  FPGA_ALM_LOGGING,  528, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(15), FPGA_ALM_LOGGING,  529, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(14), FPGA_ALM_LOGGING,  530, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(13), FPGA_ALM_LOGGING,  531, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(12), FPGA_ALM_LOGGING,  532, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(11), FPGA_ALM_LOGGING,  533, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(10), FPGA_ALM_LOGGING,  534, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(9),  FPGA_ALM_LOGGING,  535, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(8),  FPGA_ALM_LOGGING,  536, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(7),  FPGA_ALM_LOGGING,  537, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(6),  FPGA_ALM_LOGGING,  538, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(5),  FPGA_ALM_LOGGING,  539, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(4),  FPGA_ALM_LOGGING,  540, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(3),  FPGA_ALM_LOGGING,  541, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(2),  FPGA_ALM_LOGGING,  542, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(1),  FPGA_ALM_LOGGING,  543, NORMAL_0, RESERVE},
    {0x004082, RX_STAT_UNVLD_CNT, REG_BIT(0),  FPGA_ALM_LOGGING,  544, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(15), FPGA_ALM_LOGGING,  545, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(14), FPGA_ALM_LOGGING,  546, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(13), FPGA_ALM_LOGGING,  547, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(12), FPGA_ALM_LOGGING,  548, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(11), FPGA_ALM_LOGGING,  549, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(10), FPGA_ALM_LOGGING,  550, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(9),  FPGA_ALM_LOGGING,  551, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(8),  FPGA_ALM_LOGGING,  552, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(7),  FPGA_ALM_LOGGING,  553, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(6),  FPGA_ALM_LOGGING,  554, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(5),  FPGA_ALM_LOGGING,  555, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(4),  FPGA_ALM_LOGGING,  556, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(3),  FPGA_ALM_LOGGING,  557, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(2),  FPGA_ALM_LOGGING,  558, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(1),  FPGA_ALM_LOGGING,  559, NORMAL_0, RESERVE},
    {0x004083, RX_TAG_UNVLD_CNT, REG_BIT(0),  FPGA_ALM_LOGGING,  560, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(15), FPGA_ALM_LOGGING,  561, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(14), FPGA_ALM_LOGGING,  562, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(13), FPGA_ALM_LOGGING,  563, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(12), FPGA_ALM_LOGGING,  564, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(11), FPGA_ALM_LOGGING,  565, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(10), FPGA_ALM_LOGGING,  566, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(9),  FPGA_ALM_LOGGING,  567, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(8),  FPGA_ALM_LOGGING,  568, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(7),  FPGA_ALM_LOGGING,  569, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(6),  FPGA_ALM_LOGGING,  570, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(5),  FPGA_ALM_LOGGING,  571, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(4),  FPGA_ALM_LOGGING,  572, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(3),  FPGA_ALM_LOGGING,  573, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(2),  FPGA_ALM_LOGGING,  574, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(1),  FPGA_ALM_LOGGING,  575, NORMAL_0, RESERVE},
    {0x004088, PLD_TIMEOUT_CNT, REG_BIT(0),  FPGA_ALM_LOGGING,  576, NORMAL_0, RESERVE}
    
};

static FPGA_ALM_REG_INFO SDAccelAlmRegInfoTbl[]=
{
    {0x1000000, SDX_REG_DDR0_ECC_STATUS, REG_BIT(1),  FPGA_ALM_LOGGING, 1, NORMAL_0, RESERVE},
    {0x1000000, SDX_REG_DDR0_ECC_STATUS, REG_BIT(0),  FPGA_ALM_LOGGING, 2, NORMAL_0, RESERVE},
    {0x1010000, SDX_REG_DDR1_ECC_STATUS, REG_BIT(1),  FPGA_ALM_LOGGING, 3, NORMAL_0, RESERVE},
    {0x1010000, SDX_REG_DDR1_ECC_STATUS, REG_BIT(0),  FPGA_ALM_LOGGING, 4, NORMAL_0, RESERVE},
    {0xd0000,  SDX_FIREWALL_CTRL_STATUS,     REG_BIT(20), FPGA_ALM_LOGGING, 8, NORMAL_0, RESERVE},
    {0xd0000,  SDX_FIREWALL_CTRL_STATUS,     REG_BIT(19), FPGA_ALM_LOGGING, 9, NORMAL_0, RESERVE},
    {0xd0000,  SDX_FIREWALL_CTRL_STATUS,     REG_BIT(18), FPGA_ALM_LOGGING, 10, NORMAL_0, RESERVE},
    {0xd0000,  SDX_FIREWALL_CTRL_STATUS,     REG_BIT(17), FPGA_ALM_LOGGING, 11, NORMAL_0, RESERVE},
    {0xd0000,  SDX_FIREWALL_CTRL_STATUS,     REG_BIT(16), FPGA_ALM_LOGGING, 12, NORMAL_0, RESERVE},
    {0xd0000,  SDX_FIREWALL_CTRL_STATUS,     REG_BIT(4),  FPGA_ALM_LOGGING, 13, NORMAL_0, RESERVE},
    {0xd0000,  SDX_FIREWALL_CTRL_STATUS,     REG_BIT(3),  FPGA_ALM_LOGGING, 14, NORMAL_0, RESERVE},
    {0xd0000,  SDX_FIREWALL_CTRL_STATUS,     REG_BIT(2),  FPGA_ALM_LOGGING, 15, NORMAL_0, RESERVE},
    {0xd0000,  SDX_FIREWALL_CTRL_STATUS,     REG_BIT(1),  FPGA_ALM_LOGGING, 16, NORMAL_0, RESERVE},
    {0xd0000,  SDX_FIREWALL_CTRL_STATUS,     REG_BIT(0),  FPGA_ALM_LOGGING, 17, NORMAL_0, RESERVE},
    {0xe0000,  SDX_FIREWALL_USR_CTRL_STATUS, REG_BIT(20), FPGA_ALM_LOGGING, 18, NORMAL_0, RESERVE},
    {0xe0000,  SDX_FIREWALL_USR_CTRL_STATUS, REG_BIT(19), FPGA_ALM_LOGGING, 19, NORMAL_0, RESERVE},
    {0xe0000,  SDX_FIREWALL_USR_CTRL_STATUS, REG_BIT(18), FPGA_ALM_LOGGING, 20, NORMAL_0, RESERVE},
    {0xe0000,  SDX_FIREWALL_USR_CTRL_STATUS, REG_BIT(17), FPGA_ALM_LOGGING, 21, NORMAL_0, RESERVE},
    {0xe0000,  SDX_FIREWALL_USR_CTRL_STATUS, REG_BIT(16), FPGA_ALM_LOGGING, 22, NORMAL_0, RESERVE},
    {0xe0000,  SDX_FIREWALL_USR_CTRL_STATUS, REG_BIT(4),  FPGA_ALM_LOGGING, 23, NORMAL_0, RESERVE},
    {0xe0000,  SDX_FIREWALL_USR_CTRL_STATUS, REG_BIT(3),  FPGA_ALM_LOGGING, 24, NORMAL_0, RESERVE},
    {0xe0000,  SDX_FIREWALL_USR_CTRL_STATUS, REG_BIT(2),  FPGA_ALM_LOGGING, 25, NORMAL_0, RESERVE},
    {0xe0000,  SDX_FIREWALL_USR_CTRL_STATUS, REG_BIT(1),  FPGA_ALM_LOGGING, 26, NORMAL_0, RESERVE},
    {0xe0000,  SDX_FIREWALL_USR_CTRL_STATUS, REG_BIT(0),  FPGA_ALM_LOGGING, 27, NORMAL_0, RESERVE},
    {0xf0000,  SDX_FIREWALL_USR_DATA_STATUS, REG_BIT(20), FPGA_ALM_LOGGING, 28, NORMAL_0, RESERVE},
    {0xf0000,  SDX_FIREWALL_USR_DATA_STATUS, REG_BIT(19), FPGA_ALM_LOGGING, 29, NORMAL_0, RESERVE},
    {0xf0000,  SDX_FIREWALL_USR_DATA_STATUS, REG_BIT(18), FPGA_ALM_LOGGING, 30, NORMAL_0, RESERVE},
    {0xf0000,  SDX_FIREWALL_USR_DATA_STATUS, REG_BIT(17), FPGA_ALM_LOGGING, 31, NORMAL_0, RESERVE},
    {0xf0000,  SDX_FIREWALL_USR_DATA_STATUS, REG_BIT(16), FPGA_ALM_LOGGING, 32, NORMAL_0, RESERVE},
    {0xf0000,  SDX_FIREWALL_USR_DATA_STATUS, REG_BIT(4),  FPGA_ALM_LOGGING, 33, NORMAL_0, RESERVE},
    {0xf0000,  SDX_FIREWALL_USR_DATA_STATUS, REG_BIT(3),  FPGA_ALM_LOGGING, 34, NORMAL_0, RESERVE},
    {0xf0000,  SDX_FIREWALL_USR_DATA_STATUS, REG_BIT(2),  FPGA_ALM_LOGGING, 35, NORMAL_0, RESERVE},
    {0xf0000,  SDX_FIREWALL_USR_DATA_STATUS, REG_BIT(1),  FPGA_ALM_LOGGING, 36, NORMAL_0, RESERVE},
    {0xf0000,  SDX_FIREWALL_USR_DATA_STATUS, REG_BIT(0),  FPGA_ALM_LOGGING, 37, NORMAL_0, RESERVE}
};

#define LEN_BIT_8 (8)
#define TBL_LEN(a) (sizeof(a)/sizeof(a[0]))
/*******************************************************************************
Function     : FPGA_MonitorInitModule
Description  : Global variable initialize
Input        : None
Output       : None
Return       : 0:sucess other:fail  
*******************************************************************************/
UINT32 FPGA_MonitorInitModule( void )
{
    INT32 lRet = ( INT32 )SDKRTN_MONITOR_ERROR_BASE;

    lRet = memset_s( &g_strFpgaModule, sizeof( g_strFpgaModule ), 0, sizeof (g_strFpgaModule ) );
    if ( OK != lRet )
    {
        return SDKRTN_MONITOR_MEMSET_ERROR;
    }
    g_strFpgaModule.ulOpcode= INIT_VALUE;
    g_strFpgaModule.ulSlotIndex= INIT_VALUE;
    g_strFpgaModule.bShowInfo= false;

    return SDKRTN_MONITOR_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MonitorExecuteCmd
Description  : Excute the cmd
Input        : None
Output       : None
Return       : 0:sucess other:fail    
*******************************************************************************/
UINT32 FPGA_MonitorExecuteCmd( void )
{
    if ( g_strFpgaModule.ulOpcode >= CMD_PARSE_END )
    {
        LOG_DEBUG( "Invalid cmd %d", g_strFpgaModule.ulOpcode );
        return SDKRTN_MONITOR_OPCODE_ERROR;
    }

    if ( g_pafnFpgaCmdList[g_strFpgaModule.ulOpcode] == NULL )
    {
        printf( "Opcode func is null.\r\n" );
        return SDKRTN_MONITOR_OPCODE_FUNC_ERROR;
    }

    return g_pafnFpgaCmdList[g_strFpgaModule.ulOpcode](  );
}
 /*******************************************************************************
 Function     : FPGA_DoMonitorDisplayAlmInfo
 Description  : Subroutine of Display the FPGA alarm information
 Input        : UINT8 *aucAlmBitStrm , UINT32 ulAlmLen, UINT8 ucAlmLevel
 Output       : FPGA_ALM_RST* astrRst, UINT32* ulAlmCnt
 Return       : 0:sucess other:fail  
 *******************************************************************************/
UINT32 FPGA_DoMonitorDisplayAlmInfo( UINT8 *aucAlmBitStrm , UINT32 ulAlmLen, UINT8 ucAlmLevel, FPGA_ALM_RST* astrRst, UINT32* pulAlmCnt)
{
    UINT32 i = 0;
    UINT8 ucType = FPGA_LOGIC_NO_TYPE;
    FPGA_ALM_REG_INFO* AlmRegInfos[] = {SDAccelAlmRegInfoTbl, DPDKAlmRegInfoTbl};
    UINT32 AlmTblLen[] = {TBL_LEN(SDAccelAlmRegInfoTbl), TBL_LEN(DPDKAlmRegInfoTbl)};
    UINT32 ulAlmCodeBases[]={SDX_ALM_CODE_BASE, BASIC_ALM_CODE_BASE};

    if ( (aucAlmBitStrm == NULL) || (astrRst == NULL) || (pulAlmCnt == NULL))
    {
        LOG_ERROR( "FPGA_DoMonitorDisplayAlmInfo parameter array is NULL");
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    /* 从msgbox得到逻辑类型: 迭代获得前LEN_BIT_8的值 */
    ucType = aucAlmBitStrm[LOGIC_TYPE_BIT - 1];

    if ( ucType > 1)
    {
        LOG_ERROR( "FPGA_DoMonitorDisplayAlmInfo ucType bit(8bit) is invalid");
        return SDKRTN_MONITOR_INPUT_ERROR;
    }


    if ( ulAlmLen < ( AlmTblLen[ucType] + LOGIC_TYPE_BIT ) )
    {
        LOG_ERROR( "FPGA_DoMonitorDisplayAlmInfo ulAlmLen is too small");
        return SDKRTN_MONITOR_INPUT_ERROR;
    }    

    *pulAlmCnt = 0;

    /*结果录入: 利用union记录整个寄存器值 */
    for ( i = 0; i < AlmTblLen[ucType]; i++ )
    {
        if ( aucAlmBitStrm[ (i + LOGIC_TYPE_BIT) - 1] == AlmRegInfos[ucType][i].ucNoAlmValue)
        {
            continue;
        }

        /* 只有符合告警等级， 并且和默认值不同，才会记录，用来回显 */
        if (( AlmRegInfos[ucType][i].Lvl == ucAlmLevel ) || ( AlmRegInfos[ucType][i].Lvl == FPGA_ALM_ALL ))
        {
        
            /* 记录告警值 */
            astrRst[*pulAlmCnt].pstrRegInfo= &( AlmRegInfos[ucType][i] );
            /* 关联告警值对应的寄存器基地址 */
            astrRst[*pulAlmCnt].ucBitVal= aucAlmBitStrm[(i + LOGIC_TYPE_BIT - 1)];
            astrRst[*pulAlmCnt].ulAlmCodeBase = ulAlmCodeBases[ucType];

            *pulAlmCnt += 1;
        }      
    }

    fprintf (stdout, "%u alarm was found.\r\n", *pulAlmCnt);

    return SDKRTN_MONITOR_SUCCESS;
}

 /*******************************************************************************
 Function     : FPGA_MonitorDisplayAlmInfo
 Description  : Display the FPGA alarm information
 Input        : UINT32 ulSlotIndex, UINT8 AlmLevel, UINT8 Almtype, UINT8 *acAlmInfo, UINT32 ulAlmLen
 Output       : UINT8 *acAlmInfo
 Return       : 0:sucess other:fail       
 *******************************************************************************/
UINT32 FPGA_MonitorDisplayAlmInfo( UINT32 ulSlotIndex, UINT8 ucAlmLevel, UINT8 ucAlmtype, UINT8 *aucAlmBitStrm , UINT32 ulAlmLen)
{

    UINT32 i = 0;
    UINT32 ulAlmCnt = 0;
    FPGA_ALM_RST astrRst[MAX_MBOX_BIT_LEN] ={0};
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;
    
    if ( ulSlotIndex >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_MonitorDisplayAlmInfo slot is out of range %d", ulSlotIndex );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    if ( ulAlmLen > MAX_MBOX_BIT_LEN )
    {
        LOG_ERROR( "FPGA_MonitorDisplayAlmInfo ulAlmLen is larger than max:%u", MAX_MBOX_BIT_LEN);
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    if ( ucAlmtype >= FPGA_SF_MAX )
    {
        LOG_ERROR( "FPGA_MonitorDisplayAlmInfo ulAlmLen is larger than max:%u", MAX_MBOX_BIT_LEN );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }
    
    /* 子过程: 便于LLT对输出结果的检查 */
    ulRet = FPGA_DoMonitorDisplayAlmInfo(aucAlmBitStrm, ulAlmLen, ucAlmLevel, astrRst, &ulAlmCnt);

    if ( SDKRTN_MONITOR_SUCCESS!= ulRet)
    {
        LOG_ERROR( "FPGA_MonitorDisplayAlmInfo is failed");
        return ulRet;
    }
    
    /* 打印结果 */
    for ( i = 0; i < ulAlmCnt; i++ )
    {
        fprintf(stdout, 
            "Alarm (Error Code %4u): bit %2u of Register (Address: 0x%032x, BitValue:%d \r\n",
            astrRst[i].pstrRegInfo->ucAlmCode + astrRst[i].ulAlmCodeBase, astrRst[i].pstrRegInfo->Bit, 
            astrRst[i].pstrRegInfo->ulRegBase, astrRst[i].ucBitVal);
    }    
    
    return SDKRTN_MONITOR_SUCCESS;
}

 /*******************************************************************************
 Function     : FPGA_MonitorExecuteCmd
 Description  : Display the vf information
 Input        : UINT32 ulSlotId, FpgaResourceMap *pstVfInfo
 Output       : None
 Return       : 0:sucess other:fail       
 *******************************************************************************/
 UINT32 FPGA_MonitorDisplayVfInfo( UINT32 ulSlotId, FpgaResourceMap *pstVfInfo )
{
    if ( ulSlotId >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_MonitorDisplayVfInfo slot is out of range %d", ulSlotId );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    if ( NULL == pstVfInfo )
    {
        LOG_ERROR( "FPGA_MonitorDisplayVfInfo pstVfInfo is null" );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    if ( g_strFpgaModule.bShowInfo )
    {
        printf(" ----------------FPGA Information------------------\n");       
        printf("     Type\t\t\t%s\n","Fpga Device");
        printf("     Slot\t\t\t%u\n",ulSlotId);
        printf("     VendorId\t\t\t0x%04x\n",pstVfInfo->usVendorId);
        printf("     DeviceId\t\t\t0x%04x\n",pstVfInfo->usDeviceId);
        printf("     DBDF\t\t\t%04x:%02x:%02x.%d\n",pstVfInfo->usDomain, pstVfInfo->ucBus,
        pstVfInfo->ucDev, pstVfInfo->ucFunc);
        printf(" --------------------------------------------------\n");
    }

    return SDKRTN_MONITOR_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MonitorDisplayDevice
Description  : Scan all the device and print the info to the stdout
Input        : None
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MonitorDisplayDevice( void )
{
    INT32 i = 0;
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;
    FpgaResourceMap strFpgaInfo[FPGA_SLOT_MAX]= { { 0 } };

    /* Scan all VF of this VM */
    ulRet = FPGA_PciScanAllSlots( strFpgaInfo, sizeof_array( strFpgaInfo ) );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_PciScanAllSlots failed %d", ulRet );
        return ulRet;
    }

    for ( i = 0; i < ( int ) sizeof_array( strFpgaInfo ); i++ )
    {
        if ( 0 == strFpgaInfo[i].usVendorId )
        {
            continue;
        }

        /* Display vf information */
        ulRet = FPGA_MonitorDisplayVfInfo(i, &strFpgaInfo[i]);
        if ( SDKRTN_MONITOR_SUCCESS != ulRet )
        {
            LOG_ERROR( "Display VF Devices failed %d", ulRet );
            return ulRet;
        }
    }

    return ulRet;
}

/*******************************************************************************
 Function     : FPGA_MonitorDisplayImgInfo
 Description  : Print the inquired info
 Input        : UINT32 ulSlotId, FpgaResourceMap *pstVfInfo, FPGA_IMG_INFO *pstrImgInfo
 Output       : None
 Return       : 0:sucess other:fail   
 *******************************************************************************/
 UINT32 FPGA_MonitorDisplayImgInfo( UINT32 ulSlotId, FpgaResourceMap *pstVfInfo, FPGA_IMG_INFO *pstrImgInfo )
{
    UINT32 ulFpgaOpsStatus;
    UINT32 ulFpgaLoadErr;

    INT8 *pfpgaPrStatusList[FPGA_PR_STATUS_END] =
    {
        [FPGA_PR_STATUS_NOT_PROGRAMMED] = "NOT_PROGRAMMED",
        [FPGA_PR_STATUS_PROGRAMMED] = "PROGRAMMED",
        [FPGA_PR_STATUS_EXCEPTION] = "EXCEPTION",
        [FPGA_PR_STATUS_PROGRAMMING] = "PROGRAMMING",
    };
    
    INT8 *pfpgaOpsStatusList[FPGA_OPS_STATUS_END] =
    {
        [FPGA_OPS_STATUS_INITIALIZED] = "INITIALIZED",
        [FPGA_OPS_STATUS_SUCCESS] = "SUCCESS",
        [FPGA_OPS_STATUS_FAILURE] = "FAILURE",
        [FPGA_OPS_STATUS_PROCESSING] = "PROCESSING",
    };

    INT8* pfpgaLoadErrNameList[FPGA_CLEAR_ERROR_END] = {NULL};
    
    pfpgaLoadErrNameList[FPGA_LOAD_OK] = "OK";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_LOCK_BUSY] = "GET_LOCK_BUSY";
    pfpgaLoadErrNameList[FPGA_LOAD_WRITE_DB_ERR] = "WRITE_DB_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_HOSTID_ERR] = "GET_HOSTID_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_NOVA_CFG_ERR] = "GET_NOVA_CFG_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_TOKEN_MATCH_ERR] = "TOKEN_MATCH_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_SERVICEID_ERR] = "GET_SERVICEID_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_NOVAAPI_ERR] = "GET_NOVAAPI_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_UUID_ERR] = "GET_UUID_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_INVALID_AEIID] = "INVALID_AEI_ID";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_IMAGEPARA_ERR] = "GET_IMAGEPARA_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_AEI_CHECK_ERR] = "AEI_CHECK_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_AEIFILE_ERR] = "GET_AEIFILE_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_WR_MAILBOX_ERR] = "WR_MAILBOX_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_PROGRAM_PARA_ERR] = "PROGRAM_PARA_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_PROGRAM_ICAP_ERR] = "PROGRAM_ICAP_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_DDR_CHECK_ERR] = "DDR_CHECK_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_FPGA_DISABLE_ERR ] = "FPGA_DISABLE_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_PUSH_QUEUE_ERR ] = "PUSH_QUEUE_ERR";
    pfpgaLoadErrNameList[FPGA_OTHER_EXCEPTION_ERR ] = "FPGA_EXCEPTION_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_GET_LOCK_BUSY] = "CLEAR_GET_LOCK_BUSY";
    pfpgaLoadErrNameList[FPGA_CLEAR_WRITE_DB_ERR] = "CLEAR_WRITE_DB_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_GET_BLANK_FILE_ERR] = "CLEAR_GET_BLANK_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_WR_MAILBOX_ERR] = "CLEAR_WR_MAILBOX_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_PROGRAM_PARA_ERR] = "CLEAR_PROGRAM_PARA_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_PROGRAM_ICAP_ERR] = "CLEAR_PROGRAM_ICAP_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_DDR_CHECK_ERR] = "CLEAR_DDR_CHECK_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_FPGA_DISABLE_ERR] = "CLEAR_FPGA_DISABLE_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_NOT_SUPPORT_ERR] = "CLEAR_NOT_SUPPORT_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_PUSH_QUEUE_ERR ] = "CLEAR_PUSH_QUEUE_ERR";

    if ( ulSlotId >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_MonitorDisplayVfInfo slot is out of range %d", ulSlotId );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    if ( NULL == pstVfInfo )
    {
        LOG_ERROR( "FPGA_MonitorDisplayImgInfo pstVfInfo is null" );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    if ( NULL == pstrImgInfo )
    {
        LOG_ERROR( "FPGA_MonitorDisplayImgInfo pstrImgInfo is null" );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    /* This is the command operation status */
    ulFpgaOpsStatus = ((pstrImgInfo->ulCmdOpsStatus) & FPGA_OPS_STATUS_MASK) >> FPGA_OPS_STATUS_SHIFT;
    /* This is the command operation error code */
    ulFpgaLoadErr = (pstrImgInfo->ulCmdOpsStatus) & FPGA_LOAD_ERROR_MASK;

    if( pstrImgInfo->ulFpgaPrStatus >= FPGA_PR_STATUS_END )
    {
        LOG_ERROR( "FPGA PR status Code Out Of Range %d", pstrImgInfo->ulFpgaPrStatus );
        return SDKRTN_MONITOR_PR_STATUS_ERROR;
    }

     if( ulFpgaOpsStatus >= FPGA_OPS_STATUS_END )
    {
        LOG_ERROR( "FPGA Cmd Ops Status Code Out Of Range %d", pstrImgInfo->ulCmdOpsStatus);
        return SDKRTN_MONITOR_CMD_OPS_ERROR;
    }

    if((( ulFpgaLoadErr >=  FPGA_LOAD_ERROR_END ) && ( ulFpgaLoadErr <  FPGA_OTHER_EXCEPTION_ERR )) ||
        (( ulFpgaLoadErr >=  FPGA_OTHER_ERROR_END ) && ( ulFpgaLoadErr <  FPGA_CLEAR_GET_LOCK_BUSY )) ||
        ( ulFpgaLoadErr >= FPGA_CLEAR_ERROR_END ))
    {
        LOG_ERROR( "FPGA Cmd Ops Status Code Out Of Range %d", pstrImgInfo->ulCmdOpsStatus);
        return SDKRTN_MONITOR_LOAD_ERRNAME_ERROR;
    }
     
    printf(" -------------Image Information--------------------\n");
    printf("     Type\t\t\t%s\n","Fpga Device");
    printf("     Slot\t\t\t%u\n",ulSlotId);
    printf("     VendorId\t\t\t0x%04x\n",pstVfInfo->usVendorId);
    printf("     DeviceId\t\t\t0x%04x\n",pstVfInfo->usDeviceId);
    printf("     DBDF\t\t\t%04x:%02x:%02x.%d\n",pstVfInfo->usDomain, pstVfInfo->ucBus,
        pstVfInfo->ucDev, pstVfInfo->ucFunc);
    printf("     AEI ID\t\t\t%s\n",pstrImgInfo->acHfid);
    printf("     Shell ID\t\t\t%08x\n", pstrImgInfo->ulShellID);
    printf("     FPGA PR status\t\t%s\n",pfpgaPrStatusList[pstrImgInfo->ulFpgaPrStatus]);     /* the current status of FPGA PR region */
    printf("     Load/ClearOpsStatus\t%s\n",pfpgaOpsStatusList[ulFpgaOpsStatus]);            /* the result status of Load/clear command operation */
    if(FPGA_OPS_STATUS_FAILURE == ulFpgaOpsStatus)
    {
        printf("     Load/ClearOpsError\t\t%s\n",pfpgaLoadErrNameList[ulFpgaLoadErr]);         /* the error reason of Load/clear command operation */    
    }
    printf(" --------------------------------------------------\n");    

    return SDKRTN_MONITOR_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MonitorAlmMsgs(void)
Description  : The entrance function of collect fpga alarm messages and display  
Input        : None
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MonitorAlmMsgs(void)
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;

    UINT8 * acAlmStream = NULL;

    acAlmStream = (UINT8* ) calloc (MAX_MBOX_BIT_LEN ,  sizeof(UINT8));    
    if (NULL == acAlmStream)
    {
        LOG_ERROR( "FPGA_MonitorAlmMsgs calloc failed\r\n");
        return SDKRTN_MONITOR_MALLOC_ERROR;
    }

    ulRet = FPGA_MgmtQueryAlmMsgs( g_strFpgaModule.ulSlotIndex,  
        g_strFpgaModule.SF_TYPE_FIRST_BYTE,  g_strFpgaModule.SF_LVL_FIRST_BYTE,
        acAlmStream,  MAX_MBOX_BIT_LEN);
    
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtQueryAlmMsgs failed ulRet = 0x%x\r\n", ulRet );
        free(acAlmStream);
        acAlmStream = NULL;
        return ulRet;
    } 

    ulRet = FPGA_MonitorDisplayAlmInfo( g_strFpgaModule.ulSlotIndex,  
        g_strFpgaModule.SF_TYPE_FIRST_BYTE,  g_strFpgaModule.SF_LVL_FIRST_BYTE,
        acAlmStream,  MAX_MBOX_BIT_LEN);
    
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_MonitorDisplayAlmInfo failed ulRet = 0x%x\r\n", ulRet );
    }

    free(acAlmStream);
    acAlmStream = NULL;
    
    return ulRet;

}

/*******************************************************************************
Function     : FPGA_MonitorClearHfi
Description  : The entrance function of clearing image   
Input        : None
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MonitorClearHfi(void)
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;
    ulRet = FPGA_MgmtClearHfiImage( g_strFpgaModule.ulSlotIndex );
    return ulRet;
}

/*******************************************************************************
Function     : FPGA_MonitorLoadHfi
Description  : The entrance function of loading image   
Input        : None
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MonitorLoadHfi(void)
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;
    ulRet = FPGA_MgmtLoadHfiImage( g_strFpgaModule.ulSlotIndex, g_strFpgaModule.acHfiId );
    return ulRet;
}

/*******************************************************************************
Function     : FPGA_MonitorDisplayFpgaImageInfo
Description  : The entrance function of displaying image information 
Input        : UINT32 ulSlotIndex, FPGA_IMG_INFO *pstrImgInfo
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MonitorDisplayFpgaImageInfo( UINT32 ulSlotIndex, FPGA_IMG_INFO *pstrImgInfo )
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;
    FpgaResourceMap strFpgaInfo[FPGA_SLOT_MAX]= { { 0 } };

    if ( NULL == pstrImgInfo )
    {
        LOG_ERROR( "FPGA_MonitorDisplayFpgaImageInfo pstrImgInfo is null" );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    /* Scan all VF of this VM */
    ulRet = FPGA_PciScanAllSlots( strFpgaInfo, sizeof_array( strFpgaInfo ) );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_PciScanAllSlots failed %d", ulRet );
        return ulRet;
    }

    ulRet = FPGA_MonitorDisplayImgInfo(ulSlotIndex, &strFpgaInfo[ulSlotIndex], pstrImgInfo);
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "Display image info failed %d", ulRet );
        return ulRet;
    }

    return ulRet;
}

/*******************************************************************************
Function     : FPGA_MonitorInquireFpgaImageInfo
Description  : Inquire the information of fpga image
Input        : None
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MonitorInquireFpgaImageInfo(void)
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;
    FPGA_IMG_INFO pstrImgInfo = { 0 };

    ulRet = FPGA_MgmtInquireFpgaImageInfo( g_strFpgaModule.ulSlotIndex, &pstrImgInfo );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtInquireFpgaImageInfo failed ulRet = 0x%x\r\n", ulRet );
        return ulRet;
    }

    ulRet = FPGA_MonitorDisplayFpgaImageInfo( g_strFpgaModule.ulSlotIndex, &pstrImgInfo );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_MonitorDisplayFpgaImageInfo failed ulRet = 0x%x\r\n", ulRet );
        return ulRet;
    }
    return ulRet;
}
/*******************************************************************************
Function     : FPGA_MonitorInquireLEDStatus
Description  : Inquire the status of virtual led
Input        : None
Output       : None
Return       : 0:sucess other:fail  
*******************************************************************************/
UINT32 FPGA_MonitorInquireLEDStatus(void)
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;

    ulRet = FPGA_MgmtInquireLEDStatus( g_strFpgaModule.ulSlotIndex );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_MonitorInquireLEDStatus failed ulRet = 0x%x\r\n", ulRet );
        return ulRet;
    }

    return ulRet;

}
/*******************************************************************************
Function     : main
Description  : The entrance function of tool
Input        : INT32 argc, INT8 *argv[]
Output       : None
Return       : 0:sucess other:fail  
*******************************************************************************/
int main( INT32 argc, INT8 *argv[] )
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;

    /* At least input 2 parameters  */
    if ( argc < FPGA_INPUT_PARAS_NUM_MIN )
    {
        printf( "[***TIPS***] Input parameter number should be 2 at least.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacCommandEntryHelp, sizeof_array( g_pacCommandEntryHelp ) );
        return ( INT32 )SDKRTN_MONITOR_INPUT_ERROR;
    }

    /* Initialize global variables */
    ulRet = FPGA_MonitorInitModule(  );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        printf( "Initialization failed.\r\n"  );
        return ( INT32 )ulRet;
    }

    /* Initialize libfpgamgmt */
    ulRet = FPGA_MgmtInit(  );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        return ( INT32 )ulRet;
    }

    /* Parse command */
    ulRet = FPGA_ParseCommand( argc, argv );
    if ( SDKRTN_PARSE_SUCCESS != ulRet )
    {
        printf( "Parse command failed.\r\n" );
        return ( INT32 )ulRet;
    }
    
    /* it is unnecessary to continue if commands like -V -h or -? are executed and the program will exit */
    if(g_ulparseParaFlag == QUIT_FLAG)
    {
        return ( INT32 )ulRet;
    }

    /* Eccute cmd */
    ulRet = FPGA_MonitorExecuteCmd(  );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        printf( "Execute command failed.\r\n" );
        return ( INT32 )ulRet;
    }

    printf( "Command execution is complete.\r\n" );
    return ( INT32 )ulRet;

}

#ifdef    __cplusplus
}
#endif
