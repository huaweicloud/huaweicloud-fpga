基于vivado工具的SDK配置及编译
===========================
[配置环境](#a)

[编译DPDK](#b)

[编译Example](#c)

<a name="a"></a>
配置环境
------------
```
cd /home/fp1/
source setup.sh
```

<a name="b"></a>
编译DPDK
------------
### 说明
该步骤仅编译DPDK，编译出的PMD驱动提供给用户应用使用。

### 编译方法
```
cd /home/fp1/software/userspace/dpdk_src  
chmod +x build_dpdk.sh  
sh build_dpdk.sh 
```

<a name="c"></a>
编译Example
------------
### 说明
编译Example应用时依赖于DPDK，因此会自动编译DPDK。

### 编译方法

```
cd /home/fp1/software/app/dpdk_app/
chmod +x build_dpdk_app.sh 
sh build_dpdk_app.sh
```

编译成功后，在该目录下的`bin/`目录下会生成二进制的可执行文件。



