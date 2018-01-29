# Directory Structure

[切换到中文版](./README_CN.md)

The structure of **build** in **example1/prj** is as follows:

* [example1/prj/build](#example1/prj/build_dir)/
   - checkpoints
   - README.md (This document)
   - reports

# Contents of Each File or Folder

* checkpoints    
   This directory stores the files generated during example building. The `checkpoints/` folder contains the following folders and files:
   - readme.md
   - xxx.dcp (.dcp file generated during building for one-click or single-step execution.)
   - to_facs             
      + 'user_prj_name '_partial.bin
      + 'user_prj_name '_partial.bit
      + 'user_prj_name '_routed.dcp  
        `to_facs` stores the latest xxx.bin file, xxx.bit file, and binary file such as xxx_routed.dcp generated during example 1 building.

* readme.md  
  This document describes other documents.

*  reports  
  This directory stores the latest xxx.rpt file generated during example 1 building. You can view the `xxx.rpt` file for the project execution information and determine whether the project is executed successfully.    
  The FPGA Accelerated Cloud Server (FACS) platform also provides a more intuitive method to view the result of project building, displaying the result directly on the display terminal. (The time required for each execution is slightly different.)
     - By one-click execution of `sh build.sh`, information similar to the following is displayed:
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

    - After `build.sh ` is completed, you can run the following commands for `single-step execution`.
      + -s | -S | -synth     : only executes synthesis.
      + -i | -I | -impl      : only executes placing and routing.
      + -p | -P | -pr        : only executes PR verification.
      + -b | -B | -bit       : only executes .bit file generation.
      + -e | -E | -encrypt   : indicates that no encryption is executed.
    - Run the `sh build.sh -s` command for single-step execution of synthesis. If "synth_design completed successfully." is displayed, the synthesis is successful. Information similar to the following is displayed:
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
    - Run the `sh build.sh -i` command for single-step execution of placing and routing. If "route_design completed successfully" is displayed, the placing and routing is successful. Information similar to the following is displayed:
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
    - Run the `sh build.sh -pr` command for single-step execution of PR verification. If "PR_VERIFY: check points/home/.../example1/prj/build/checkpoints/to_facs/ul_pr_top_routed.dcp and /home/.../lib/checkpoints/SH_UL_BB_routed.dcp are compatible" is displayed, the PR verification is successful. Information similar to the following is displayed:
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
    - Run the `sh build.sh -b` command for single-step execution of .bit file generation. If "Bitgen Completed Successfully." is displayed, the .bit file generation is successful.Information similar to the following is displayed:
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


  

