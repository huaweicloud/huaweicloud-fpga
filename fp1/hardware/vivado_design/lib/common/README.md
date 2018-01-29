# File Descriptions

[切换到中文版](./README_CN.md)

*  **vivado_design/lib/common** stores the Common Building Blocks (CBBs) provided by Huawei.
*  All CBBs in the **vivado_design/lib/common** belong to the common library, and can be directly invoked by projects in the `fp1/hardware/vivado_design` directory.
*  The CBBs provided by Huawei are distinguished by the file name. For details, see Directory Descriptions.

# Directory Descriptions
* [vivado_design/lib/common](#vivado_design/lib/common_dir)/  
  See the following table for the CBBs and their functions:  

  | Name                     | Description                              |
  | :----------------------- | :--------------------------------------- |
  | asyn_frm_fifo_288x512_sa | 288 x 512 (width x depth) asynchronous frame FIFO |
  | axi_time_out             | Valid & ready timeout detection block of the AXI protocol |
  | axi4                     | HPI-to-AXI4 switch block                 |
  | axil2hpis_adp            | PCIe BAR0/BAR5 AXI-L interface adaptation block |
  | buft32                   | 32-bit tri-state buffer                  |
  | cmd_reg32_inst           | Command register adaptation block        |
  | cnt32_reg_inst           | 32-bit counter access block              |
  | count32                  | 32-bit counter block                     |
  | ddr_ctrl                 | DDRA/DDRB/DDRD netlist                   |
  | err_wc_reg_inst          | Alarm register adaptation block          |
  | hpi2axi4lm_adp           | PCIe user HPI interface adaptation block |
  | if_cbb                   | FIFO library CBB                         |
  | ram_def                  | RAM parameter defining block             |
  | raxi_rc256_fifo.v        | DMA receiving channel AXI interface adaptation block (256 bits) |
  | raxi_rc512_fifo          | DMA receiving channel AXI interface adaptation block (512 bits) |
  | raxi_rq256_fifo          | DMA sending channel AXI interface adaptation block (256 bits) |
  | raxi_rq512_fifo          | DMA sending channel AXI interface adaptation block (512 bits) |
  | ro_reg_inst              | Read-only register adaptation block      |
  | rw_reg_inst              | Read/write register adaptation block     |
  | sdpramb_dclk             | Simple dual-port RAM block (dual-clock)  |
  | sdpramb_sclk             | Simple dual-port RAM block (single-clock) |
  | syn_frm_fifo_540x512b    | 540 x 512 (width x depth) synchronous frame FIFO |
  | ts_addr_reg_inst         | Address inversion register adaptation block |
  | ts_reg_inst              | Data inversion register adaptation block |
  | wc_reg_inst              | Write/clear register adaptation block    |
  | xilinx_sdpramb_dclk      | Xilinx simple dual-port RAM block (dual-clock) |
  | xilinx_sdpramb_sclk      | Xilinx simple dual-port RAM block (single-clock) |

