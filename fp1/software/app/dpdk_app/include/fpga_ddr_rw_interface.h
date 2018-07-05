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

#ifndef	_FPGA_DDR_RW_INTERFACE_H_
#define	_FPGA_DDR_RW_INTERFACE_H_

typedef struct _rw_ddr_data_ 
{
	unsigned long long fpga_ddr_wr_addr;
    unsigned long long fpga_ddr_rd_addr;
	unsigned long long cpu_vir_src_addr;
	unsigned long long cpu_vir_dst_addr;
	unsigned int length;
} rw_ddr_data;

typedef void (*Callbackfunc)( unsigned int thread_id, unsigned int slot_id, rw_ddr_data rw_data, int rw_flag); 

/*module init
*including: dpdk env init, memory manager init, global resource init.
*/
int fddr_access_mode_init(Callbackfunc callback);
/*module uninit*/
int fddr_access_mode_uninit();
/*thread id alloc interface
*thread id as unique tag to send msg and recv result.
*/
int alloc_thread_id(unsigned int *thread_id);
/*thread id free interface*/
int free_thread_id(unsigned int thread_id);
/*read data from fpga ddr interface*/
int read_data_from_fddr (unsigned int thread_id, unsigned int slot_id, rw_ddr_data rw_data);
/*write data to fpga ddr interface*/
int write_data_to_fddr(unsigned int thread_id, unsigned int slot_id, rw_ddr_data rw_data);
/*process data with fpga interface*/
int process_data_with_fpga(unsigned int thread_id, unsigned int slot_id, rw_ddr_data rw_data);
/*read register interface*/
int read_register(unsigned int slot_id, unsigned int addr, unsigned int *value);
/*write register interface*/
int write_register(unsigned int slot_id ,unsigned int addr, unsigned int value);

void* memory_manager_alloc_bulk(unsigned int buff_size);
int memory_manager_free_bulk( void* buff_vaddr);
void info_collect_mem_manager(void);

#endif
