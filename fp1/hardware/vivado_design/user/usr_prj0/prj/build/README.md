# 目录结构

../prj/build文件目录文件层级结构如下：

* [prj/build](#/prj/build_dir)/
   - checkpoints
   - README.md （本文档）
   - reports

# 各文件或文件夹的内容

* checkpoints    
   该目录主要用于存放用户工程构建产生的文件，`checkpoints/`一般包含如下文件夹及层级文件：
   - README.md
   - xxx.dcp(构建过程中产生的一键执行的，或者是其他单步执行的.dcp文件。)
   - to_facs             
      + 'user_prj_name '_partial.bin
      + 'user_prj_name '_partial.bit
      + 'user_prj_name '_routed.dcp  
      `to_facs`主要用于存放 用户工程 构建过程中产生的最新版本的 xxx.bin 文件和 xxx.bit文件,还有xxx_routed.dcp等二进制文件。

* README.md  
  即本文档，用于介绍其他文档。

*  reports  
  该目录主要用于存放 用户工程 构建过程中产生的最近一次的报告xxx.rpt文件，通过查看`xxx.rpt`文件可以得到工程执行的所有信息，判断工程执行是否成功。    
  FACS平台还提供了一种更直观的方法查看构建工程的结果，即显示终端上直接呈现结果并打印出来（每次执行所需的时间time略有不同）。
     - 一键式执行`sh build.sh`的结果打印格式如下：
     ```bash
      +-----------+------------------------------------------------------------+
      |   time    |                                            [0d_1h_36m_12s] |
      +-----------+------------------------------------------------------------+
      |   synth   |                                               successfully |
      +-----------+------------------------------------------------------------+
      |   impl    |                                               successfully |
      +-----------+------------------------------------------------------------+
      |    pr     |                                                         NA |
      +-----------+------------------------------------------------------------+
      |   bitgen  |                                                         NA |
      +-----------+------------------------------------------------------------+

             PPPPPPPPPP        AAAA        SSSSSSSS      SSSSSSSS
             PPPP    PPPP    AAAAAAAA    SSSS    SSSS  SSSS    SSSS
             PPPP    PPPP  AAAA    AAAA  SSSS          SSSS
             PPPP    PPPP  AAAA    AAAA    SSSS          SSSS
             PPPPPPPPPP    AAAA    AAAA      SSSS          SSSS
             PPPP          AAAAAAAAAAAA        SSSS          SSSS
             PPPP          AAAA    AAAA          SSSS          SSSS
             PPPP          AAAA    AAAA  SSSS    SSSS  SSSS    SSSS
             PPPP          AAAA    AAAA    SSSSSSSS      SSSSSSSS
      ```

    - 我们通过`build.sh `后面可跟随以下命令,进行`单步执行`操作。
      + -s | -S | -synth     : 只执行综合
      + -i | -I | -impl      : 只执行布局布线
      + -p | -P | -pr        :只执行pr校验
      + -b | -B | -bit       :只执行生成bit文件
      + -e | -E | -encrypt   :表示不进行加密
    - 用命令`sh build.sh -s`单步执行综合打印提示出现“ synth_design completed successfully.”表示综合成功；打印结果如下：
      ```bash
      +-----------+------------------------------------------------------------+
      |   time    |                                            [0d_0h_07m_12s] |
      +-----------+------------------------------------------------------------+
      |   synth   |                                               successfully |
      +-----------+------------------------------------------------------------+
      |   impl    |                                                         NA |
      +-----------+------------------------------------------------------------+
      |    pr     |                                                         NA |
      +-----------+------------------------------------------------------------+
      |   bitgen  |                                                         NA |
      +-----------+------------------------------------------------------------+

             PPPPPPPPPP        AAAA        SSSSSSSS      SSSSSSSS
             PPPP    PPPP    AAAAAAAA    SSSS    SSSS  SSSS    SSSS
             PPPP    PPPP  AAAA    AAAA  SSSS          SSSS
             PPPP    PPPP  AAAA    AAAA    SSSS          SSSS
             PPPPPPPPPP    AAAA    AAAA      SSSS          SSSS
             PPPP          AAAAAAAAAAAA        SSSS          SSSS
             PPPP          AAAA    AAAA          SSSS          SSSS
             PPPP          AAAA    AAAA  SSSS    SSSS  SSSS    SSSS
             PPPP          AAAA    AAAA    SSSSSSSS      SSSSSSSS
      ```
    - 用命令`sh build.sh -i`单步执行布局布线打印提示出现“route_design completed successfully”表示布局布线成功。打印结果如下：
      ```bash
      +-----------+------------------------------------------------------------+
      |   time    |                                            [0d_0h_07m_12s] |
      +-----------+------------------------------------------------------------+
      |   synth   |                                                          NA|
      +-----------+------------------------------------------------------------+
      |   impl    |                                               successfully |
      +-----------+------------------------------------------------------------+
      |    pr     |                                                         NA |
      +-----------+------------------------------------------------------------+
      |   bitgen  |                                                         NA |
      +-----------+------------------------------------------------------------+

             PPPPPPPPPP        AAAA        SSSSSSSS      SSSSSSSS
             PPPP    PPPP    AAAAAAAA    SSSS    SSSS  SSSS    SSSS
             PPPP    PPPP  AAAA    AAAA  SSSS          SSSS
             PPPP    PPPP  AAAA    AAAA    SSSS          SSSS
             PPPPPPPPPP    AAAA    AAAA      SSSS          SSSS
             PPPP          AAAAAAAAAAAA        SSSS          SSSS
             PPPP          AAAA    AAAA          SSSS          SSSS
             PPPP          AAAA    AAAA  SSSS    SSSS  SSSS    SSSS
             PPPP          AAAA    AAAA    SSSSSSSS      SSSSSSSS
       ```
    - 用命令`sh build.sh -pr` 单步执行pr校验打印提示出现“PR_VERIFY: check points /home/.../usr_prjxx/prj/build/checkpoints/to_facs/usr_prjxx_top_routed.dcp and /home/.../lib/checkpoints/SH_UL_BB_routed.dcp are compatible”表示PR校验成功；打印结果如下：
      ```bash
      +-----------+------------------------------------------------------------+
      |   time    |                                            [0d_0h_07m_12s] |
      +-----------+------------------------------------------------------------+
      |   synth   |                                                          NA|
      +-----------+------------------------------------------------------------+
      |   impl    |                                                          NA|
      +-----------+------------------------------------------------------------+
      |    pr     |                                               successfully |
      +-----------+------------------------------------------------------------+
      |   bitgen  |                                                         NA |
      +-----------+------------------------------------------------------------+

             PPPPPPPPPP        AAAA        SSSSSSSS      SSSSSSSS
             PPPP    PPPP    AAAAAAAA    SSSS    SSSS  SSSS    SSSS
             PPPP    PPPP  AAAA    AAAA  SSSS          SSSS
             PPPP    PPPP  AAAA    AAAA    SSSS          SSSS
             PPPPPPPPPP    AAAA    AAAA      SSSS          SSSS
             PPPP          AAAAAAAAAAAA        SSSS          SSSS
             PPPP          AAAA    AAAA          SSSS          SSSS
             PPPP          AAAA    AAAA  SSSS    SSSS  SSSS    SSSS
             PPPP          AAAA    AAAA    SSSSSSSS      SSSSSSSS
      ```
    - 用命令`sh build.sh -b`单步执行bit文件生成打印提示出现“Bitgen Completed Successfully.”表示bit文件生成成功。打印结果如下：
      ```bash
      +-----------+------------------------------------------------------------+
      |   time    |                                            [0d_0h_07m_12s] |
      +-----------+------------------------------------------------------------+
      |   synth   |                                                          NA|
      +-----------+------------------------------------------------------------+
      |   impl    |                                                          NA|
      +-----------+------------------------------------------------------------+
      |    pr     |                                                          NA|
      +-----------+------------------------------------------------------------+
      |   bitgen  |                                               successfully |
      +-----------+------------------------------------------------------------+

             PPPPPPPPPP        AAAA        SSSSSSSS      SSSSSSSS
             PPPP    PPPP    AAAAAAAA    SSSS    SSSS  SSSS    SSSS
             PPPP    PPPP  AAAA    AAAA  SSSS          SSSS
             PPPP    PPPP  AAAA    AAAA    SSSS          SSSS
             PPPPPPPPPP    AAAA    AAAA      SSSS          SSSS
             PPPP          AAAAAAAAAAAA        SSSS          SSSS
             PPPP          AAAA    AAAA          SSSS          SSSS
             PPPP          AAAA    AAAA  SSSS    SSSS  SSSS    SSSS
             PPPP          AAAA    AAAA    SSSSSSSS      SSSSSSSS
      ```


  