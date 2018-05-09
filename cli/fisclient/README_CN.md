# fisclient #
[Switch to the English version](README.md)<br/><br/>
**fisclient** 是FIS（FPGA镜像服务）的命令行客户端，集成了用于FPGA镜像管理的命令。

- [1 运行环境要求](#1-运行环境要求)
- [2 安装](#2-安装)
- [3 配置](#3-配置)
- [4 介绍](#4-介绍)
  - [4.1 fis命令](#41-fis命令)
  - [4.2 查询](#42-查询)
  - [4.3 删除](#43-删除)
  - [4.4 关联、解关联和查询关联](#44-关联解关联和查询关联)
- [5 fis命令详解](#5-fis命令详解)
  - [5.1 查看帮助信息](#51-查看帮助信息)
  - [5.2 删除子命令](#52-删除子命令)
  - [5.3 查询子命令](#53-查询子命令)
  - [5.4 关联子命令](#54-关联子命令)
  - [5.5 解关联子命令](#55-解关联子命令)
  - [5.6 查询关联子命令](#56-查询关联子命令)

<a name="operating-environment-requirements"></a>
# 1 运行环境要求 #
**fisclient** 是在以下环境中开发和测试的：

- CentOS 7.3
- Python 2.7

对于其他环境，不保证可用性。

<a name="installation"></a>
# 2 安装 #
在安装之前，用户首先要确保已经以 **root** 身份登录弹性云服务器。

### 步骤1 下载源码包 ###
- 执行 **git clone https://github.com/Huawei/huaweicloud-fpga.git** 命令下载 **fisclient** 源码包。

> 如果用户已经下载了[FPGA开发套件](https://github.com/Huawei/huaweicloud-fpga/blob/master/README_CN.md)，则可以跳过本步骤。<br/>
> 下载 **fisclient** 源码包时，请确保当前目录下没有以 **huaweicloud-fpga** 命名的文件或目录。

### 步骤2 安装fisclient ###
- 执行 **cd huaweicloud-fpga/cli/fisclient** 命令切换到 **FPGA开发套件** 的 **huaweicloud-fpga/cli/fisclient** 目录。

- 执行 **bash install.sh** 命令安装fisclient工具。

<a name="configuration"></a>
# 3 配置 #
用户需要执行 **fis configure** 命令来进行 **fisclient** 工具的配置。<br/>

在配置过程中，用户需要输入以下参数：

| 参数 | 说明 |
| ---- | ---- |
| **Access Key** | 接入键标识AK |
| **Secret Key** | 安全接入键SK |
| **Region ID** | 用户当前所在的区域 |
| **Bucket Name** | 用于存放待注册FPGA镜像的OBS桶 |

> **fisclient** 会自动保存用户上一次的有效配置参数。用户在配置过程中，既可以输入新的配置参数，也可以通过输入单个**回车键**来使用上一次的配置参数。<br/>
> 在完成配置后，用户可以执行 **fis configure --dump** 命令查看当前的配置。

### 步骤1 配置密钥参数 ###
在配置密钥参数前，用户需要参照[创建访问密钥](https://support.huaweicloud.com/clientogw-obs/zh-cn_topic_0045829057.html)中的指导创建并获取访问密钥。<br/>

<pre>
Access key and Secret key are your identifiers for FIS and OBS.
Access Key []: a0Vfz5j9********eltR
Secret Key []: a0vet3Eh********************cIr4meJzYSMe
</pre>

当提示输入 **Access Key** 时，请输入用户的**接入键标识（Access Key ID）**。<br/>
当提示输入 **Secret Key** 时，请输入用户的**安全接入键（Secret Access Key）**。

### 步骤2 配置区域参数 ###
区域参数 **Region ID** 表示用户在哪一个区域中使用FPGA加速云服务器。<br/>

| 区域 | Region ID |
| ---- | ---- |
| **华北-北京一** | **cn-north-1** |
| **华东-上海二** | **cn-east-2** |
| **华南-广州** | **cn-south-1** |

> 请注意，错误的Region ID可能仍会使FPGA镜像注册和查询成功，但会使FPGA镜像加载失败。

<pre>
Choose the Region where you are located.
Available Regions:
  (1) cn-north-1
  (2) cn-east-2
  (3) cn-south-1
Region ID []: 1
</pre>

当提示输入 **Region ID** 时，请输入用户当前所在区域对应的序号。例如，若用户在 **华北-北京一** 使用FPGA加速云服务器，则请输入 **cn-north-1** 对应的序号，即 **1** 。

### 步骤3 配置桶参数 ###
桶参数 **Bucket Name** 表示存放待注册FPGA镜像的OBS桶。

> 如果用户在当前区域中已经拥有了符合条件的OBS桶，**fisclient** 会罗列出这些桶，用户只需要从中选择一个即可。

- 创建新的OBS桶

<pre>
Choose or Create a Bucket for storing the FPGA images to be registered.
Available Bucket(s):
  (1) hello-fpga1
Bucket Name []: hello-fpga2
Bucket "hello-fpga2" created
</pre>

如果用户在当前区域中没有符合条件的OBS桶，或者虽然拥有了符合条件的OBS桶，但仍然希望创建一个新的OBS桶，则当提示输入 **Bucket Name** 时，请输入相应的OBS桶名，**fisclient** 会自动帮用户完成OBS桶的创建。例如，若用户希望创建一个名称为 **hello-fpga2** 的OBS桶，则请输入 **hello-fpga2**。

> 桶的名称空间是OBS中的所有用户共享的，新创建桶的桶名在OBS中必须是唯一的。<br/>
> OBS桶的命名规则:
> - 长度范围为3到63个字符
> - 仅支持小写字母、数字、"-"、"."
> - 禁止以"-"或"."开头及结尾。
> - 禁止两个"."相邻（如"my..bucket"）。
> - 禁止"."和"-"相邻（如"my-.bucket"和"my.-bucket"）
> - 禁止使用IP地址

- 使用已有的OBS桶

<pre>
Choose or Create a Bucket for storing the FPGA images to be registered.
Available Bucket(s):
  (1) hello-fpga1
  (2) hello-fpga2
Bucket Name []: 2
</pre>
如果用户希望使用已有的OBS桶，则当提示输入 **Bucket Name** 时，请输入相应OBS桶的序号。例如，用户在当前区域中拥有 **hello-fpga1** 和 **hello-fpga2** 两个符合条件的OBS桶，并希望使用 **hello-fpga2** 来存放待注册的FPGA镜像，则请输入 **hello-fpga2** 对应的序号，即 **2** 。

> 当用户期望选择或创建的桶名与Available Bucket(s)列表中OBS桶的序号冲突时，用户可以在桶名前添加一个!符号，使用 **!mybucket** 表示期望选择或创建的桶名为 **mybucket**。

### 步骤4 确认并保存 ###
在用户完成所有参数配置后，**fisclient** 会询问用户是否保存新的配置。
<pre>
New settings:
  Access key: a0Vfz5j9********eltR
  Secret Key: a0vet3Eh********************cIr4meJzYSMe
  Region ID: cn-north-1
  Bucket Name: hello-fpga2
Save settings? [Y/n]: 
Configuration saved to "/root/.fiscfg".
</pre>
如果用户选择保存新的配置，请输入 **y** 或者单个**回车键**，**fisclient** 会将这些配置保存到 **~/.fiscfg** 文件。<br/>
如果用户选择不保存新的配置，请输入 **n**。

<a name="introduction"></a>
# 4 介绍 #
<a name="fis-command"></a>
## 4.1 fis命令 ##
用户通过在Linux操作系统的shell中执行 **fis** 命令来进行FPGA镜像的管理。<br/>

fis命令的格式为 **fis &lt;subcommand&gt; &lt;option&gt;**

- 子命令 **&lt;subcommand&gt;** 指定执行的fis命令的功能。
- 选项 **&lt;option&gt;** 特定于子命令，为子命令指定命令参数。

fis命令的详细使用说明请参见[fis命令详解](#fis-command-description)。

<a name="query"></a>
## 4.2 查询 ##
在注册FPGA镜像后，用户可以使用fis查询子命令查询自身拥有的FPGA镜像的信息。在确认FPGA镜像的状态是 **active** 后，用户可以使用相应的FPGA镜像ID执行后续的加载、删除、关联等操作。<br/>

> 注意，用户通过fis查询子命令只能查询到**自身拥有的FPGA镜像**的信息。对于购买的和共享的FPGA镜像，用户需要通过**fis查询关联子命令**来查询相应的信息。例如，用户可以参考[查询共享的FPGA镜像](#querying-the-shared-fpga-image)来了解如何查询FPGA共享镜像的信息。

fis查询子命令以一个表格来呈现FPGA镜像信息，并且支持分页查询功能。更多详细信息请参见[查询子命令](#query-subcommand)。

### 示例 ###
执行以下fis命令查询FPGA镜像：
<pre>
[root@ ~]# fis fpga-image-list
Success: 200 OK
+----------------------------------+---------+--------+-----------+------+---------------------+-------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
| id                               | name    | status | protected | size | createdAt           | description | metadata                                                                                                                                                                                                                                                      | message |
+----------------------------------+---------+--------+-----------+------+---------------------+-------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
| 000000******08b4015e3224afe203c3 | OCL_001 | active | False     | 43   | 2017-09-19 02:27:31 | mmult_01    | {"manifest_format_version": "1", "pci_vendor_id": "0x19e5", "pci_device_id": "0xD512", "pci_subsystem_id": "-", "pci_subsystem_vendor_id": "-", "shell_type": "0x121", "shell_version": "0x0001", "hdk_version": "SDx 2017.1", "date": "2017/09/17_18:37:12"} |         |
+----------------------------------+---------+--------+-----------+------+---------------------+-------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
</pre>
- 用户之前注册的FPGA镜像的ID为 **000000\*\*\*\*\*\*08b4015e3224afe203c3**。
- FPGA镜像的状态为 **active**，表示用户之前的注册操作执行成功。

因此，用户可以使用该FPGA镜像ID进行后续的加载、删除、关联等操作。

<a name="deletion"></a>
## 4.3 删除 ##
删除操作允许FPGA镜像的拥有者执行FPGA镜像的删除操作。当用户不再使用某个注册成功的FPGA镜像，并且希望从FPGA镜像管理模块中删除该FPGA镜像相关的记录时，可以使用fis删除子命令进行FPGA镜像删除操作。此外，如果用户在执行fis查询子命令时发现某个FPGA镜像的状态是 **error** 时，可以使用fis删除子命令删除该FPGA镜像记录。<br/>
如果FPGA镜像已经和某个弹性云服务器镜像关联，FPGA镜像将被置于“保护”状态（FPGA镜像的 **protected** 属性被置为 **True**），不允许被删除。更多详细信息请参见[删除子命令](#deletion-subcommand)。

### 删除确认 ###
默认情况下，fisclient为用户提供删除确认功能，在用户执行删除操作时会提示用户输入 **yes** 或 **no** 以确认是否进行删除操作。

- 如果用户输入 **yes**，则执行当前的删除操作。
- 如果用户输入 **no**，则取消当前的删除操作。

<pre>
[root@ ~]# fis fpga-image-delete --fpga-image-id 4010b39d********015d5ee5c3b00501
Deleted fpga-image cannot be restored! Are you absolutely sure? (yes/no): no
cancel fpga-image-delete
[root@ ~]# fis fpga-image-delete --fpga-image-id 4010b39d********015d5ee5c3b00501
Deleted fpga-image cannot be restored! Are you absolutely sure? (yes/no): yes
Success: 204 No Content
</pre>
如果用户不想在每次执行fis删除子命令时都进行用户确认操作，则可以指定 **--force** 选项，强制执行删除操作。

### 示例 ###
用户在某次执行查询操作时返回的FPGA镜像信息如下所示：
<pre>
+----------------------------------+----------+--------+-----------+------+---------------------+-------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+----------------------------+
| id                               | name     | status | protected | size | createdAt           | description | metadata                                                                                                                                                                                                                                                                                                                                                                                  | message                    |
+----------------------------------+----------+--------+-----------+------+---------------------+-------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+----------------------------+
| ff********5056b2015d5e13608c73c7 | OCL_001  | active | False     | 43   | 2017-09-19 02:27:31 | mmult_01    | {"manifest_format_version": "1", "pci_vendor_id": "0x19e5", "pci_device_id": "0xD512", "pci_subsystem_id": "-", "pci_subsystem_vendor_id": "-", "shell_type": "0x121", "shell_version": "0x0001", "hdk_version": "SDx 2017.1", "date": "2017/09/18_18:27:12"}                                                                                                                             |                            |
| 4010b39c5d4********48e97411005ae | dpdk_002 | error  | False     | 45   | 2017-09-19 16:39:27 | example_02  | {"manifest_format_version": "1", "pci_vendor_id": "0x19e5", "pci_device_id": "0xD503", "pci_subsystem_id": "-", "pci_subsystem_vendor_id": "-", "dcp_hash": "ced75657********60f212a9454f6c5ae33d50f0a248e99dbef638231b26960c", "shell_type": "0x101", "shell_version": "0x0013", "dcp_file_name": "ul_pr_top_routed.dcp", "hdk_version": "Vivado 2017.2", "date": "2017/09/19_13:51:41"} | register fpga image failed |
+----------------------------------+----------+--------+-----------+------+---------------------+-------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+----------------------------+
</pre>
如果用户希望删除不再使用的ID为 **ff\*\*\*\*\*\*\*\*5056b2015d5e13608c73c7** 的FPGA镜像，以及注册失败的ID为 **4010b39c5d4\*\*\*\*\*\*\*\*48e97411005ae** 的FPGA镜像，可以执行如下的fis命令：

- 删除不再使用的FPGA镜像。
<pre>
[root@ ~]# fis fpga-image-delete --fpga-image-id ff********5056b2015d5e13608c73c7 --force
Success: 204 No Content
</pre>

- 删除状态为error的FPGA镜像。
<pre>
[root@ ~]# fis fpga-image-delete --fpga-image-id 4010b39c5d4********48e97411005ae --force
Success: 204 No Content
</pre>

在上述命令中，**--fpga-image-id** 选项指定待删除的FPGA镜像的ID，**--force** 选项指定执行强制删除操作。如果命令的回显信息为 **Success: 204 No Content**，则表示fis删除子命令执行成功。然而，删除子命令执行成功并不代表FPGA镜像删除成功。用户需要执行查询操作，如果查找不到待删除的FPGA镜像的信息，则表示FPGA镜像删除成功。

<a name="association-disassociation-association-query"></a>
## 4.4 关联、解关联和查询关联 ##
通过关联操作，用户可以将已成功注册的FPGA镜像提供给 **位于相同区域** 的其他用户使用，包括如下两种场景：

- 市场镜像场景：将已成功注册的FPGA镜像发布到云市场进行交易。
- 共享镜像场景：将已成功注册的FPGA镜像共享给指定用户。

通过查询关联操作，用户可以查询其他用户提供的FPGA镜像。通过解关联操作，用户可以取消FPGA镜像的共享。<br/>
本小节以共享镜像场景为例来说明关联、解关联和查询关联操作的使用。这些子命令的更多详细信息请参见[关联子命令](#association-subcommand)、[解关联子命令](#disassociation-subcommand)和[查询关联子命令](#association-query-subcommand)。

<a name="sharing-the-fpga-image"></a>
### 共享FPGA镜像 ###
当用户A想要将自己拥有的一个已注册成功的FPGA镜像共享给用户B时，需要完成以下步骤。以下假设用户A想将ID为 **4010b39c5d4\*\*\*\*\*\*\*\*\*\*f2cf8070c7e** 的 **通用型架构** 的FPGA镜像共享给用户B。 

- 步骤1：从 **通用型架构** 的FPGA弹性云服务器创建一个ECS私有镜像，更多详细信息请参见[创建私有镜像](https://support.huaweicloud.com/usermanual-ims/zh-cn_topic_0030713180.html)。

> FPGA镜像共享是通过ECS私有镜像共享实现的。用户在创建ECS私有镜像前，请确保已将FPGA弹性云服务器中的个人文件（例如 **~/.fiscfg** 文件）删除。

- 步骤2：在创建的ECS私有镜像的详情页面中获取镜像ID。以下假设创建的ECS私有镜像的镜像ID为 **404223ca-8\*\*b-4\*\*2-a\*\*e-d187\*\*\*\*61bc**。
- 步骤3：关联待共享的FPGA镜像和创建的ECS私有镜像。

用户A需要登录到FPGA弹性云服务器中，然后执行fis关联子命令来关联FPGA镜像和ECS私有镜像。
<pre>
[root@ ~]# fis fpga-image-relation-create --fpga-image-id 4010b39c5d4**********f2cf8070c7e --image-id 404223ca-8**b-4**2-a**e-d187****61bc
Success: 204 No Content
</pre>
如果命令的回显信息为 **Success: 204 No Content**，则表示关联操作执行成功。

- 步骤4：将创建的ECS私有镜像共享给用户B，更多细节信息请参见[共享私有镜像](https://support.huaweicloud.com/usermanual-ims/zh-cn_topic_0032042419.html)。

> ECS私有镜像在共享之后会变为 **共享镜像**，无法进行关联操作。因此，确保在共享镜像之前进行关联。

<a name="querying-the-shared-fpga-image"></a>
### 查询共享的FPGA镜像 ###
当用户B想要使用用户A共享的FPGA镜像时，需要完成以下步骤。

- 步骤1：接受用户A共享的ECS镜像，更多详细信息请参见[接受共享镜像](https://support.huaweicloud.com/usermanual-ims/zh-cn_topic_0032042420.html)。
- 步骤2：从用户A处获取共享的FPGA镜像的类型。在本示例中，FPGA镜像的类型是 **通用型架构**。
- 步骤3：使用共享的ECS镜像创建一个与共享FPGA镜像相同类型的FPGA弹性云服务器，更多详细信息请参见[购买并登录Linux弹性云服务器](https://support.huaweicloud.com/qs-ecs/zh-cn_topic_0092494193.html)。在本示例中，用户B需要创建一个 **通用型架构** 的FPGA弹性云服务器。
> 确保创建的FPGA弹性云服务器的类型与共享的FPGA镜像的类型相同。

- 步骤4：在共享的ECS镜像的详情页面中获取镜像ID。在本示例中，共享的ECS镜像的镜像ID是 **404223ca-8\*\*b-4\*\*2-a\*\*e-d187\*\*\*\*61bc**。
- 步骤5：使用共享的ECS镜像的镜像ID作为参数来查询用户A共享的FPGA镜像。

用户B需要登录到使用用户A共享的ECS镜像创建的FPGA弹性云服务器中，然后执行fis查询关联子命令（将 **image-id** 参数设置为共享的ECS镜像的镜像ID）来查询用户A共享的FPGA镜像。
<pre>
[root@ ~]# fis fpga-image-relation-list --image-id 404223ca-8**b-4**2-a**e-d187****61bc
Success: 200 OK
+--------------------------------------+----------------------------------+---------+--------+-----------+------+---------------------+-------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
| image_id                             | fpga_image_id                    | name    | status | protected | size | createdAt           | description | metadata                                                                                                                                                                                                                                                       | message |
+--------------------------------------+----------------------------------+---------+--------+-----------+------+---------------------+-------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
| 404223ca-8**b-4**2-a**e-d187****61bc | 4010b39c5d4**********f2cf8070c7e | name123 | active | True      | 39   | 2017-09-19 03:27:31 | desc123     |  {"manifest_format_version": "1", "pci_vendor_id": "0x19e5", "pci_device_id": "0xD512", "pci_subsystem_id": "-", "pci_subsystem_vendor_id": "-", "shell_type": "0x121", "shell_version": "0x0001", "hdk_version": "SDx 2017.1", "date": "2017/09/18_19:27:12"} |         |
+--------------------------------------+----------------------------------+---------+--------+-----------+------+---------------------+-------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
</pre>
上述回显信息表示用户A共享的FPGA镜像的ID是 **4010b39c5d4\*\*\*\*\*\*\*\*\*\*f2cf8070c7e**。用户B可以使用该FPGA镜像ID进行后续的加载操作。

<a name="canceling-the-fpga-image-sharing"></a>
### 取消FPGA镜像共享 ###
当用户A想取消给用户B的FPGA镜像共享时，需要完成以下步骤。

- 步骤1：取消给用户B的ECS镜像共享，更多详细信息请参见[取消共享镜像的共享](https://support.huaweicloud.com/usermanual-ims/zh-cn_topic_0032087324.html)。
- 步骤2：解关联共享的FPGA镜像和ECS镜像。

用户A需要登录到FPGA弹性云服务器中，然后执行fis解关联子命令来解关联FPGA镜像和ECS镜像。
<pre>
[root@ ~]# fis fpga-image-relation-delete --fpga-image-id 4010b39c5d4**********f2cf8070c7e --image-id 404223ca-8**b-4**2-a**e-d187****61bc
Success: 204 No Content
</pre>
如果命令的回显信息为 **Success: 204 No Content**，则表示解关联操作执行成功。

<a name="fis-command-description"></a>
# 5 fis命令详解 #
<a name="help-subcommand"></a>
## 5.1 查看帮助信息 ##
用户通过执行 **fis help** 命令查看fis命令的帮助信息，执行 **fis help &lt;subcommand&gt;** 命令查看fis子命令的帮助信息。
<pre>
[root@ ~]# fis help 
usage: fis &lt;subcommand&gt; ...

Command-line interface to the fis API. 

positional arguments: 
  &lt;subcommand&gt; 
    configure           Invoke interactive (re)configuration tool
    fpga-image-delete   Delete an FPGA image
    fpga-image-list     Query FPGA images of a tenant
    fpga-image-register
                        Register an FPGA image
    fpga-image-relation-create
                        Create the relation of an FPGA image and an ECS image
    fpga-image-relation-delete
                        Delete the relation of an FPGA image and an ECS image
    fpga-image-relation-list
                        Query FPGA image relations visible to a tenant
    help                Display help about fis or one of its subcommands

See "fis help COMMAND" for help on a specific command.
</pre>

fis命令的格式为 **fis &lt;subcommand&gt; &lt;option&gt;**

- 子命令 **&lt;subcommand&gt;** 指定执行的fis命令的功能。
- 选项 **&lt;option&gt;** 特定于子命令，为子命令指定命令参数。

fis命令包含如下8个子命令。

| 命令 | 说明 |
| ------- | ----------- |
| **configure** | 配置子命令，用于调用 **fisclient** 的交互式配置工具 |
| **fpga-image-delete** | 删除子命令，用于删除FPGA镜像 |
| **fpga-image-list** | 查询子命令，用于查询租户拥有的FPGA镜像详情列表信息 |
| **fpga-image-register** | 注册子命令，用于注册FPGA镜像 |
| **fpga-image-relation-create** | 关联子命令，用于创建FPGA镜像与弹性云服务器镜像的关联关系 |
| **fpga-image-relation-delete** | 解关联子命令，用于删除FPGA镜像与弹性云服务器镜像的关联关系 |
| **fpga-image-relation-list** | 查询关联子命令，用于查询租户可见的FPGA镜像与弹性云服务器镜像的关联关系 |
| **help** | 帮助子命令，用于显示fis命令或fis子命令的帮助信息 |

> fis注册子命令 **fpga-image-register** 在用户执行 **AEI_Register.sh** 脚本时将自动调用。用户不需要单独执行该命令实现注册FPGA镜像。

<a name="deletion-subcommand"></a>
## 5.2 删除子命令 ##
删除子命令为用户提供删除FPGA镜像管理模块中的相应FPGA镜像的功能。在成功执行删除操作后，针对被删除的FPGA镜像的加载、关联、删除等操作都将失败。

### 命令格式 ###
**fis fpga-image-delete --fpga-image-id** *&lt;UUID&gt;* **[--force]**

### 参数说明 ###
| 参数 | 说明 | 取值 | 备注 |
| --------- | ----------- | ----- | ------- |
| **--fpga-image-id** | （必选）待删除的FPGA镜像的ID。 | **fpga-image-id**参数是由英文小写字母a-f，数字0-9组成的32位字符串。 | 在查询子命令执行成功后，用户可以在回显信息中查找到相应的FPGA镜像ID。 |
| **--force** | （可选）一个命令选项，不带参数，用于取消删除确认功能。 | - | 在默认情况下，fisclient为用户提供删除确认功能，在用户执行删除操作时会提示用户输入 **yes** 或 **no** 以确认是否进行删除操作：输入 **yes** 则执行当前的删除操作；输入 **no** 则取消当前的删除操作。 |

> **--force** 选项只是取消了删除确认功能，并不保证一定能够成功删除FPGA镜像。

### 使用说明 ###
当回显信息中包含 **Success: 204 No Content** 时，表示删除子命令执行成功。
<pre>
Success: 204 No Content
</pre>
删除子命令执行成功并不代表FPGA镜像删除成功。用户需要进一步执行查询子命令，若查找不到待删除的FPGA镜像的信息，则表示FPGA镜像删除成功。
<br/><br/>
当删除子命令执行失败时，回显信息中会包含相应的错误原因信息。

- Example 1
<pre>
[root@ ~]# fis fpga-image-delete --fpga-image-id 123456 --force
Error: parameter "fpga_image_id" value (123456) is malformed
</pre>
错误信息表示 **fpga-image-id** 参数不符合相应的限制条件，不是由英文小写字母a-f和数字0-9组成的32位字符串。

- Example 2
<pre>
[root@ ~]# fis fpga-image-delete --fpga-image-id 000000005d44********44df075a003c --force   
Error: 400 Bad Request
tenant [495440c1********8e9403e708ad4d9d] fpga image [000000005d44********44df075a003c] is protected 
</pre>
错误信息表示FPGA镜像已经和某个弹性云服务器镜像关联，处于“保护”状态，无法被删除。

### 示例 ###
从FPGA镜像管理模块中删除ID为 **000000005d19076b015d30dc17ab02ab** 的FPGA镜像。
<pre>
[root@ ~]# fis fpga-image-delete --fpga-image-id 000000005d19********30dc17ab02ab 
Deleted fpga-image cannot be restored! Are you absolutely sure? (yes/no): no 
cancel fpga-image-delete 
[root@ ~]# fis fpga-image-delete --fpga-image-id 000000005d19********30dc17ab02ab --force 
Success: 204 No Content
</pre>

<a name="query-subcommand"></a>
## 5.3 查询子命令 ##
查询子命令以表格的形式呈现租户拥有的FPGA镜像的信息。同时，查询子命令提供分页查询功能。

### 命令格式 ###
**fis fpga-image-list** **[--page** *&lt;Int&gt;***] [--size** *&lt;Int&gt;***]**

### 参数说明 ###
| 参数 | 说明 | 取值 | 备注 |
| --------- | ----------- | ----- | ------- |
| **--page** | （可选）分页查询时的页编号。 | **page**参数是[1,65535)范围内的十进制整数，并且不能包含+号。 | 由用户自行指定。 |
| **--size** | （可选）分页查询时的页大小。 | **size**参数是[1,100]范围内的十进制整数，并且不能包含+号。 | 由用户自行指定。 |

> **page** 参数和 **size** 参数必须同时存在或同时不存在，并且只有当两个参数同时存在时分页查询功能才能生效。

### 使用说明 ###
当回显信息中包含 **Success: 200 OK** 时，表示查询子命令执行成功，此时回显信息是一个包含下述列标题的表格。
<pre>
+----+------+--------+-----------+------+-----------+-------------+----------+---------+
| id | name | status | protected | size | createdAt | description | metadata | message |
+----+------+--------+-----------+------+-----------+-------------+----------+---------+
</pre>
The following table describes the table headers.

| 参数 | 说明 |
| --------- | ----------- |
| **id** | FPGA镜像的ID |
| **name** | FPGA镜像的名称 |
| **status** | FPGA镜像的状态 |
| **protected** | FPGA镜像是否处于“保护”状态 |
| **size** | FPGA镜像的文件大小，单位为MB |
| **createdAt** | FPGA镜像的创建时间（UTC） |
| **description** | FPGA镜像的描述信息 |
| **metadata** | FPGA镜像的元数据信息 |
| **message** | FPGA镜像的附加消息 |

<br/>
当查询子命令执行失败时，回显信息中会包含相应的错误原因信息。

- Example 1
<pre>
[root@ ~]# fis fpga-image-list --page 1
Error: argument --page and --size must exist or not exist at the same time
</pre>
错误信息表示用户只设置了 **page** 参数，而没有设置 **size** 参数。

- Example 2
<pre>
[root@ ~]# fis fpga-image-list --page 1 --size 101
Error: parameter "size" value (101) is malformed
</pre>
错误信息表示 **size** 参数不符合相应的限制条件，不是位于[1,100]范围内的十进制整数。

### 示例 ###
使用分页功能（页编号为1，页大小为2）查询租户拥有的FPGA镜像信息。
<pre>
[root@ ~] fis fpga-image-list --page 1 --size 2
Success: 200 OK
+----------------------------------+----------+--------+-----------+------+---------------------+-------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
| id                               | name     | status | protected | size | createdAt           | description | metadata                                                                                                                                                                                                                                                                                                                                                                                  | message |
+----------------------------------+----------+--------+-----------+------+---------------------+-------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
| 00000000********015e98dce81501db | dpdk_001 | active | False     | 45   | 2017-09-18 16:29:27 | example_01  | {"manifest_format_version": "1", "pci_vendor_id": "0x19e5", "pci_device_id": "0xD503", "pci_subsystem_id": "-", "pci_subsystem_vendor_id": "-", "dcp_hash": "ced75657********60f212a9454f6c5ae33d50f0a248e99dbef638231b26960c", "shell_type": "0x101", "shell_version": "0x0013", "dcp_file_name": "ul_pr_top_routed.dcp", "hdk_version": "Vivado 2017.2", "date": "2017/09/18_13:41:41"} |         |
| 00000000********015e97f6408d01cd | OCL_001  | active | False     | 43   | 2017-09-18 02:37:31 | mmult_01    | {"manifest_format_version": "1", "pci_vendor_id": "0x19e5", "pci_device_id": "0xD512", "pci_subsystem_id": "-", "pci_subsystem_vendor_id": "-", "shell_type": "0x121", "shell_version": "0x0001", "hdk_version": "SDx 2017.1", "date": "2017/09/17_18:37:12"}                                                                                                                             |         |
+----------------------------------+----------+--------+-----------+------+---------------------+-------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
</pre>

<a name="association-subcommand"></a>
## 5.4 关联子命令 ##
关联子命令为用户提供关联FPGA镜像与弹性云服务器镜像的功能。在建立关联关系后，FPGA镜像会被置于“保护”状态，无法被删除。

### 命令格式 ###
**fis fpga-image-relation-create --fpga-image-id** *&lt;UUID&gt;* **--image-id** *&lt;UUID&gt;*
 
### 参数说明 ###
| 参数 | 说明 | 取值 | 备注 |
| --------- | ----------- | ----- | ------- |
| **--fpga-image-id** | （必选）待关联的FPGA镜像的ID。 | **fpga-image-id**参数是由英文小写字母a-f，数字0-9组成的32位字符串。 | 在查询子命令执行成功后，用户可以在回显信息中查找到相应的FPGA镜像ID。 |
| **--image-id** | （必选）待关联的弹性云服务器镜像的ID。 | **image-id**参数遵循IMS（镜像服务）的镜像ID限制。 | 用户可以在镜像的详情页面中获取镜像ID。 |

> FPGA镜像管理模块在进行关联操作时，要求弹性云服务器镜像的类型是 **private**，即私有镜像。而私有镜像在发布到云市场或进行共享操作后，镜像类型会分别变为 **market** 和 **shared**，从而无法进行关联操作。用户需要将已发布到云市场的镜像下架，或将已共享的镜像取消共享，才能进行关联操作。

### 使用说明 ###
当回显信息中包含 **Success: 204 No Content** 时，表示关联子命令执行成功。
<pre>
Success: 204 No Content
</pre>

当关联子命令执行失败时，回显信息中会包含相应的错误原因信息。

- Example 1
<pre>
[root@ ~]# fis fpga-image-relation-create --fpga-image-id 000000005d44********53b720a40210 --image-id 5633dfaf-7**e-4**4-9**8-6202****6f7b
Error: 400 Bad Request
tenant [495440c1********8e9403e708ad4d9d] image [5633dfaf-7**e-4**4-9**8-6202****6f7b] type [shared] is not private
</pre>
错误信息表示用户指定的弹性云服务器镜像的类型是 **shared**，不是要求的 **private**，从而无法执行关联操作。

- Example 2
<pre>
[root@ ~]# fis fpga-image-relation-create --fpga-image-id 4010a32c5f5b********5bd53ab4004b --image-id 5633dfaf-7**e-4**4-9**8-6202****6f7b
Error: 400 Bad Request
tenant [495440c1********8e9403e708ad4d9d] relate fpga image [4010a32c5f5b********5bd53ab4004b] and image [5633dfaf-7**e-4**4-9**8-6202****6f7b] failed: relation already exist or the related fpga image count of image reach the max limit 
</pre>
错误信息表示用户待创建的关联关系违反了FPGA镜像管理模块对关联关系的约束，从而无法执行关联操作。
<br/>
FPGA镜像管理模块对待创建的关联关系有如下的**约束**：

 - FPGA镜像和弹性云服务器镜像之间 **不能** 重复创建关联关系。
 - 一个弹性云服务器镜像最多只允许关联 **10** 个FPGA镜像。


### 示例 ###
创建ID为 **000000005d19\*\*\*\*\*\*\*\*30dec20e02b3** 的FPGA镜像与ID为 **b79bbfe9-9\*\*a-4\*\*b-8\*\*f-9d61\*\*\*\*efa0** 的弹性云服务器镜像之间的关联关系。
<pre>
[root@ ~]# fis fpga-image-relation-create --fpga-image-id 000000005d19********30dec20e02b3 --image-id b79bbfe9-9**a-4**b-8**f-9d61****efa0
Success: 204 No Content
</pre>

<a name="disassociation-subcommand"></a>
## 5.5 解关联子命令 ##
解关联子命令为用户提供删除FPGA镜像与弹性云服务器镜像之间的关联关系的功能。在删除关联关系后，如果FPGA镜像没有再与其他的弹性云服务器镜像关联，则不再处于“保护”状态，允许进行删除操作。

### 命令格式 ###
**fis fpga-image-relation-delete --fpga-image-id** *&lt;UUID&gt;* **--image-id** *&lt;UUID&gt;*

### 参数说明 ###
| 参数 | 说明 | 取值 | 备注 |
| --------- | ----------- | ----- | ------- |
| **--fpga-image-id** | （必选）待解关联的FPGA镜像的ID。 | **fpga-image-id**参数是由英文小写字母a-f，数字0-9组成的32位字符串。 | 在查询关联子命令执行成功后，用户可以在回显信息中查找到相应的FPGA镜像ID。 |
| **--image-id** | （必选）待解关联的弹性云服务器镜像的ID。 | **image-id**参数遵循IMS（镜像服务）的镜像ID限制。 | 在查询关联子命令执行成功后，用户可以在回显信息中查找到相应的镜像ID。 |

> FPGA镜像管理模块在进行解关联操作时，要求弹性云服务器镜像的类型是 **private**，即私有镜像。而私有镜像在发布到云市场或进行共享操作后，镜像类型会分别变为 **market** 和 **shared**，从而无法进行解关联操作。用户需要将已发布到云市场的镜像下架，或将已共享的镜像取消共享，才能进行解关联操作。

### 使用说明 ###
当回显信息中包含 **204 No Content** 时，表示解关联子命令执行成功。
<pre>
Success: 204 No Content
</pre>

当解关联子命令执行失败时，回显信息中会包含相应的错误原因信息。

- Example 1
<pre>
[root@ ~]# fis fpga-image-relation-delete --fpga-image-id 4010a32b5f57********5ce866fc0004 --image-id ae606e5b-1**b-4**1-b**a-d37d****26dd
Error: 400 Bad Request 
tenant [495440c1********8e9403e708ad4d9d] is not the owner of fpga image [4010a32b5f57********5ce866fc0004]
</pre>
错误信息表示用户不是待解关联的FPGA镜像的拥有者，无权执行解关联操作。

- Example 2
<pre>
[root@ ~]# fis fpga-image-relation-delete --fpga-image-id 4010a32b5f52********53b0104d0176 --image-id 5633dfaf-7**e-4**4-9fa8-6202****6f7b
Error: 400 Bad Request
tenant [495440c1********8e9403e708ad4d9d] image [5633dfaf-7**e-4**4-9fa8-6202****6f7b] type [shared] is not private
</pre>
错误信息表示用户指定的弹性云服务器镜像的类型是 **shared**，不是要求的 **private**，从而无法执行解关联操作。

### 示例 ###
删除ID为 **000000005d19\*\*\*\*\*\*\*\*30dec20e02b3** 的FPGA镜像与ID为 **b79bbfe9-9\*\*a-4\*\*b-8\*\*f-9d61\*\*\*\*efa0** 的弹性云服务器镜像之间的关联关系。
<pre>
[root@ ~]# fis fpga-image-relation-delete --fpga-image-id 000000005d19********30dec20e02b3 --image-id b79bbfe9-9**a-4**b-8**f-9d61****efa0
Success: 204 No Content
</pre>

<a name="association-query-subcommand"></a>
## 5.6 查询关联子命令 ##
查询关联子命令以表格的形式呈现租户可见的FPGA镜像与弹性云服务器镜像的关联关系信息。同时，查询关联子命令提供分页查询功能。

### 命令格式 ###
**fis fpga-image-relation-list [--fpga-image-id** *&lt;UUID&gt;***]** **[--image-id** *&lt;UUID&gt;***]** **[--page** *&lt;Int&gt;***] [--size** *&lt;Int&gt;***]**

### 参数说明 ###
| 参数 | 说明 | 取值 | 备注 |
| --------- | ----------- | ----- | ------- |
| **--fpga-image-id** | （可选）待查询关联的FPGA镜像的ID。 | **fpga-image-id**参数是由英文小写字母a-f，数字0-9组成的32位字符串。 | 在查询子命令执行成功后，用户可以在回显信息中查找到相应的FPGA镜像ID。 |
| **--image-id** | （可选）待查询关联的弹性云服务器镜像的ID。 | **image-id**参数遵循IMS（镜像服务）的镜像ID限制。 | 用户可以在镜像的详情页面中获取镜像ID。 |
| **--page** | （可选）分页查询时的页编号。 | **page**参数是[1,65535)范围内的十进制整数，并且不能包含+号。 | 由用户自行指定。 |
| **--size** | （可选）分页查询时的页大小。 | **size**参数是[1,100]范围内的十进制整数，并且不能包含+号。 | 由用户自行指定。 |

> 只有至少指定 **fpga-image-id** 和 **image-id** 参数中的一个时，用户才可能查询到关联关系，否则只会返回一个空列表。<br/>
> **page** 和 **size** 参数必须同时存在或同时不存在，并且只有当两个参数同时存在时分页查询功能才能生效。<br/>
> 当同时指定 **fpga_image_id** 和 **image_id** 参数时，分页查询参数 **page** 和 **size** 将不起作用。


### 使用说明 ###
当回显信息中包含 **Success: 200 OK** 时，表示查询关联子命令执行成功。此时，回显信息是一个包含下述列标题的表格，表格的每一行表示一条关联关系。
<pre>
+----------+---------------+------+--------+-----------+------+-----------+-------------+----------+---------+
| image_id | fpga_image_id | name | status | protected | size | createdAt | description | metadata | message |
+----------+---------------+------+--------+-----------+------+-----------+-------------+----------+---------+
</pre>
列标题的含义如下表所示。

| 参数 | 说明 |
| --------- | ----------- |
| **image_id** | 弹性云服务器镜像的ID |
| **fpga_image_id** | FPGA镜像的ID |
| **name** | FPGA镜像的名称 |
| **status** | FPGA镜像的状态 |
| **protected** | FPGA镜像是否处于“保护”状态 |
| **size** | FPGA镜像的文件大小，单位为MB |
| **createdAt** | FPGA镜像的创建时间（UTC） |
| **description** | FPGA镜像的描述信息 |
| **metadata** | FPGA镜像的元数据信息 |
| **message** | FPGA镜像的附加消息 |

<br/>
当查询子命令执行失败时，回显信息中会包含相应的错误原因信息。

- Example 1
<pre>
[root@ ~]# fis fpga-image-relation-list --page 1
Error: argument --page and --size must exist or not exist at the same time
</pre>
错误信息表示用户只设置了 **page** 参数，而没有设置 **size** 参数。

- Example 2
<pre>
[root@ ~]# fis fpga-image-relation-list --fpga-image-id 4010a3ac5c5b********5b3524850023
Error: 404 Not Found
tenant [495440c1********8e9403e708ad4d9d] fpga image [4010a3ac5c5b********5b3524850023] doesn't exist
</pre>
错误信息表示用户指定的FPGA镜像不存在于FPGA镜像管理模块中，无法查询相应的关联关系。

### 示例 ###
使用分页功能（页编号为1，页大小为2），指定弹性云服务器镜像ID为 **404223ca-8\*\*b-4\*\*2-a\*\*e-d187\*\*\*\*61bc** 查询租户可见的关联关系信息。
<pre>
[root@ ~]# fis fpga-image-relation-list --image-id 404223ca-8**b-4**2-a**e-d187****61bc --page 1 --size 2
Success: 200 OK
+--------------------------------------+----------------------------------+----------+--------+-----------+------+---------------------+-------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
| image_id                             | fpga_image_id                    | name     | status | protected | size | createdAt           | description | metadata                                                                                                                                                                                                                                                                                                                                                                                  | message |
+--------------------------------------+----------------------------------+----------+--------+-----------+------+---------------------+-------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
| 404223ca-8**b-4**2-a**e-d187****61bc | 00000000********015e98dce81501db | dpdk_001 | active | True      | 45   | 2017-09-18 16:29:27 | example_01  | {"manifest_format_version": "1", "pci_vendor_id": "0x19e5", "pci_device_id": "0xD503", "pci_subsystem_id": "-", "pci_subsystem_vendor_id": "-", "dcp_hash": "ced75657********60f212a9454f6c5ae33d50f0a248e99dbef638231b26960c", "shell_type": "0x101", "shell_version": "0x0013", "dcp_file_name": "ul_pr_top_routed.dcp", "hdk_version": "Vivado 2017.2", "date": "2017/09/18_13:41:41"} |         |
| 404223ca-8**b-4**2-a**e-d187****61bc | 00000000********015e97f6408d01cd | OCL_001  | active | True      | 43   | 2017-09-18 02:37:31 | mmult_01    | {"manifest_format_version": "1", "pci_vendor_id": "0x19e5", "pci_device_id": "0xD512", "pci_subsystem_id": "-", "pci_subsystem_vendor_id": "-", "shell_type": "0x121", "shell_version": "0x0001", "hdk_version": "SDx 2017.1", "date": "2017/09/17_18:37:12"}                                                                                                                             |         |
+--------------------------------------+----------------------------------+----------+--------+-----------+------+---------------------+-------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------+
</pre>
