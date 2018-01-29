# FACS pcie memory map

[Switch to the English version](./pcie_memory_map.md)

一个PF支持`2个BAR`，BAR0保留，供SHELL使用；BAR1供用户逻辑使用。
  - BAR0为32-bit BAR，大小为`64MiB`(0x0~0x3FF-FFFF)。
  - BAR5为32-bit BAR，大小为`16MiB`(0x0~0xFF-FFFF),和用户逻辑之间的接口为AXI-L 32bit。 

