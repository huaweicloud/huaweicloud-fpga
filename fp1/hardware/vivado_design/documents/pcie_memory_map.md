# FACS PCIe Memory Mapping

[切换到中文版](./pcie_memory_map_cn.md)

Each PF supports `two BARs`. BAR0 is reserved for shell, and BAR1 is used by user logic.
  - BAR0 is 32-bit, and the size is `64 Mbytes` (0x0–0x3FF-FFFF).
  - BAR1 is 32-bit, and the size is `16Mbytes` (0x0–0xFF-FFFF). The 32-bit AXI-Lite interface is between BAR1 and user logic.


