#
#-------------------------------------------------------------------------------
#      Copyright 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
# 
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the Huawei Software License (the "License").
#      A copy of the License is located in the "LICENSE" file accompanying 
#      this file.
# 
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#      Huawei Software License for more details. 
#-------------------------------------------------------------------------------

set syn_strategy $::env(SYN_STRATEGY)
set impl_strategy $::env(IMPL_STRATEGY)         

set LIB_DIR $::env(LIB_DIR)
set UL_DIR $::env(UL_DIR)
set ENCRYPT_DIR $::env(ENCRYPT_DIR)
set TOP $::env(TOP)
set PRJ_NAME $::env(PRJ_NAME)

set SYNTH_EN $::env(SYNTH_EN)
set IMPL_EN $::env(IMPL_EN)
set PR_EN $::env(PR_EN)
set BIT_EN $::env(BIT_EN)
set ENCRYPT_EN $::env(ENCRYPT_EN)

set_msg_config -id {Timing 38-35}         -suppress
set_msg_config -id {Timing 38-2}          -suppress
set_msg_config -id {Timing 38-3}          -suppress

set_msg_config -id {Constraints 18-550}   -suppress
set_msg_config -id {Constraints 18-633}   -suppress
set_msg_config -id {Constraints 18-4422}  -suppress
set_msg_config -id {Constraints 18-4421}  -suppress
set_msg_config -id {Project 1-486}        -suppress


#******************************************************************************
#         encrypt  
#****************************************************************************** 
exec chmod -R +w {*}[glob $ENCRYPT_DIR/*]

if { $ENCRYPT_EN==1} {
    # encrypt .v/.sv/.vh/inc as verilog files   
    proc encrypt_file { myDir LIB_DIR } {
        if {[catch {cd $myDir} err]} {
            puts $err 
            return
            }
        foreach myfile [glob -nocomplain *] {
            cd $myDir
            if {[string equal $myfile ""]} {
                return
            }
            set fullfile [file join $myDir $myfile]
            if {[file isdirectory $myfile]} {
                encrypt_file $fullfile $LIB_DIR 
            } elseif {[string match *.v $myfile]} {
                encrypt -k $LIB_DIR/scripts/keyfile_ver.txt -lang verilog $myfile
            } elseif {[string match *.h $myfile]} {
                encrypt -k $LIB_DIR/scripts/keyfile_ver.txt -lang verilog  $myfile
            } elseif {[string match *.vhd? $myfile]} {  
                 # encrypt *vhdl files
                encrypt -k $LIB_DIR/scripts/keyfile_vhd.txt -lang vhdl -quiet $myfile
            }  

        }
    }
    encrypt_file $ENCRYPT_DIR $LIB_DIR

}

#******************************************************************************
#         create project
#****************************************************************************** 
if { $SYNTH_EN==1} {
    create_project -force $PRJ_NAME $UL_DIR/prj/build/$PRJ_NAME
    set obj [get_projects $PRJ_NAME]
    #******************************************************************************
    #         set_property
    #****************************************************************************** 
    set_property part xcvu9p-flgb2104-2-i $obj
    set_property "default_lib" "xil_defaultlib" $obj
    set_property "simulator_language" "Mixed" $obj
    set_property verilog_define XSDB_SLV_DIS=1 [current_fileset]
    set_property -name {STEPS.SYNTH_DESIGN.ARGS.MORE OPTIONS} -value {-mode out_of_context} -objects [get_runs synth_1]
    set_msg_config -id {filemgmt 20-742}  -new_severity "ERROR"

    #******************************************************************************
    #         read usr files
    #****************************************************************************** 
    #read_verilog [glob $UL_DIR/src_encrypt/*] 
    add_files [glob $UL_DIR/src_encrypt/*] 
    #******************************************************************************
    #         findfiles function
    #****************************************************************************** 
    proc Findfiles { myDir filetype } {

   if {[catch {cd $myDir} err]} {
      puts $err
      return
   }
   foreach myfile [glob -nocomplain *] {

      cd $myDir
      if {[string equal $myfile ""]} {
        return
      }
      set fullfile [file join $myDir $myfile]

      if {[file isdirectory $myfile]} {
        Findfiles $fullfile $filetype
      } elseif { ($filetype == "dcp") && [string match *.dcp $myfile]} {
        read_checkpoint  $myfile
      } elseif { ($filetype == "common")  && [string match *.v* $myfile]} {
        read_verilog  $myfile
      } elseif { ($filetype == "head")  && [string match *.h $myfile]} {
        read_verilog  $myfile
      }  

   }
    }
    #******************************************************************************
    #         read sh files    
    #****************************************************************************** 
    Findfiles $LIB_DIR/common common
    Findfiles $LIB_DIR/common head
    Findfiles $LIB_DIR/common dcp

    #******************************************************************************
    #         read ip files    
    #******************************************************************************
    proc FindIP { myDir n } {
    	if {$n==0} {
	    return 
	} else {
    		if {[catch {cd $myDir} err]} {
       			puts "$err"
        		return
    		}
    		foreach myfile [glob -nocomplain *] {
        		cd $myDir
       			if {[string equal $myfile ""]} {
            			return
        		}
        		set fullfile [file join $myDir $myfile]
        		if {[file isdirectory $myfile]} {
	     			FindIP $fullfile [expr $n-1]
        		} elseif {[string match *.xci $myfile]} {
        			read_ip  $myfile
        		}
    		}
    	    }
	}

    FindIP $LIB_DIR/ip 2
    cd $UL_DIR/prj

    set_property top $TOP [current_fileset]   
    #******************************************************************************
    #         read xdc 
    #****************************************************************************** 
    
     if {[string equal [get_filesets -quiet constrs_1] ""]} {
       create_fileset -constrset constrs_1
     }
     
     set obj [get_filesets constrs_1]
     
     add_files -fileset constrs_1 [list \
                "[file normalize "$UL_DIR/prj/constraints/$PRJ_NAME.xdc"]"\
                "[file normalize "$LIB_DIR/constraints/ddra_pin_x8.xdc"]"\
                "[file normalize "$LIB_DIR/constraints/ddrb_pin_x8.xdc"]"\
                "[file normalize "$LIB_DIR/constraints/ddrd_pin_x8.xdc"]"\
                ]
     

    #******************************************************************************
    #      echo   set_property
    #******************************************************************************
    puts "---------------------------------------------------------------------------------"
    puts "                         SYN_STRATEGY = $syn_strategy                            "
    puts "---------------------------------------------------------------------------------"
    #******************************************************************************
    #         set_property
    #******************************************************************************
      if { $syn_strategy=="DEFAULT"} {
      set syn_strategy "Vivado Synthesis Defaults"
    } else {
      set syn_strategy Flow_$syn_strategy
    }

    # Create 'synth_1' run (if not found)
    if {[string equal [get_runs -quiet synth_1] ""]} {
       create_run -name synth_1 -part xcvu9p-flgb2104-2-i -flow {Vivado Synthesis 2017} -strategy "Vivado Synthesis Defaults" -constrset constrs_1
    } else {
      set_property strategy $syn_strategy [get_runs synth_1]     
      set_property flow "Vivado Synthesis 2017" [get_runs synth_1]
    }
    set obj [get_runs synth_1]
    ##########################################################################################3
    #  SET OUT_OF_CONTEXT for RM synthesis
    set_property -name {steps.synth_design.args.more options} -value {-mode out_of_context} -objects $obj
    
    # set the current synth run
    current_run -synthesis [get_runs synth_1]

    #******************************************************************************
    #         write ul_post_synth checkpoint 
    #****************************************************************************** 
      launch_runs synth_1
      wait_on_run synth_1
      open_run synth_1
      implement_mig_cores   
      write_checkpoint -force $UL_DIR/prj/build/checkpoints/${PRJ_NAME}_post_synth.dcp

    #******************************************************************************
    #         close_project 
    #****************************************************************************** 
    # close_project
}

#******************************************************************************
#         open SH_UL_BB_routed checkpoint 
#****************************************************************************** 
if { $IMPL_EN==1} {

    #******************************************************************************
    #         md5sum 
    #******************************************************************************
    set md5_value [lindex [exec sha256sum $LIB_DIR/checkpoints/SH_UL_BB_routed.dcp] 0]
    set f [open $LIB_DIR/checkpoints/SH_UL_BB_routed.sha256 r+]
    set file_md5_value [gets $f]
    if {![string equal $md5_value $file_md5_value]} {
        puts "md5_value       : $md5_value"
        puts "file_md5_value  : $file_md5_value"
        puts "ERROR:HDK shell's checkpoint version is incorrect"
        exit
    }

    #******************************************************************************
    #         open checkpoint 
    #****************************************************************************** 
    open_checkpoint $LIB_DIR/checkpoints/SH_UL_BB_routed.dcp

    #******************************************************************************
    #         lock_design 
    #******************************************************************************
    #lock_design -level routing
   read_xdc $LIB_DIR/constraints/pr_pblock.xdc
   set_property HD.RECONFIGURABLE TRUE [get_cells u_ul_pr_top] 
    
    #******************************************************************************
    #         open ul_post_synth checkpoint 
    #****************************************************************************** 
    read_checkpoint -strict -cell u_ul_pr_top $UL_DIR/prj/build/checkpoints/${PRJ_NAME}_post_synth.dcp
    #******************************************************************************
    #         echo IMPL_STRATEGY
    #******************************************************************************  
    puts "---------------------------------------------------------------------------------" 
    puts "                             IMPL_STRATEGY = $impl_strategy                      "
    puts "---------------------------------------------------------------------------------"
    switch $impl_strategy {
        "Explore" {
            puts "Explore strategy."
            opt_design -directive Explore
            place_design -directive Explore
            phys_opt_design -directive Explore
            route_design -directive Explore
        }
        "ExplorePostRoutePhysOpt" {
        puts "ExplorePostRoutePhysOpt strategy."
        opt_design -directive Explore
        place_design -directive Explore
        phys_opt_design -directive Explore
        route_design -directive Explore -tns_cleanup
        phys_opt_design -directive Explore
        }
        "WLBlockPlacement" {
            puts "WLBlockPlacement strategy."
            opt_design
            place_design -directive WLDrivenBlockPlacement
            phys_opt_design -directive AlternateReplication
            route_design -directive Explore
        }
        "WLBlockPlacementFanoutOpt" {
            puts "WLBlockPlacementFanoutOpt strategy."
            opt_design
            place_design -directive WLDrivenBlockPlacement
            phys_opt_design -directive AggressiveFanoutOpt
            route_design -directive Explore
        }
        "NetDelay_high" {
            puts "NetDelay_high strategy."
            opt_design
            place_design -directive ExtraNetDelay_high
            phys_opt_design -directive AggressiveExplore
            route_design -directive Explore
        }
        "NetDelay_low" {
            puts "NetDelay_low strategy."
            opt_design
            place_design -directive ExtraNetDelay_low
            phys_opt_design -directive AggressiveExplore
            route_design -directive Explore
        }
        "Retiming" {
            puts "Retiming strategy."
            opt_design
            place_design -directive ExtraPostPlacementOpt
            phys_opt_design -directive AlternateFlowWithRetiming
            route_design -directive Explore
        }
        "ExtraTimingOpt" {
            puts "ExtraTimingOpt strategy."
            opt_design
            place_design -directive ExtraTimingOpt
            phys_opt_design -directive Explore
            route_design -directive Explore
        }
        "RefinePlacement" {
            puts "RefinePlacement strategy."
            opt_design
            place_design -directive ExtraPostPlacementOpt
            phys_opt_design -directive Explore
            route_design -directive Explore
        }
        "SpreadSLLs" {
            puts "SpreadSLLs strategy."
            opt_design
            place_design -directive SSI_SpreadSLLs
            phys_opt_design -directive Explore
            route_design -directive Explore
        }
        "BalanceSLLs" {
            puts "BalanceSLLs strategy."
            opt_design
            place_design -directive SSI_BalanceSLLs
            phys_opt_design -directive Explore
            route_design -directive Explore
        }
        "SpreadLogic_high" {
            puts "SpreadLogic_high strategy."
            opt_design
            place_design -directive AltSpreadLogic_high
            phys_opt_design -directive AggressiveExplore
            route_design -directive AlternateCLBRouting
        }
        "SpreadLogic_medium" {
            puts "SpreadLogic_medium strategy."
            opt_design
            place_design -directive AltSpreadLogic_medium
            phys_opt_design -directive AggressiveExplore
            route_design -directive AlternateCLBRouting
        }
        "SpreadLogic_low" {
            puts "SpreadLogic_low strategy."
            opt_design
            place_design -directive AltSpreadLogic_low
            phys_opt_design -directive AggressiveExplore
            route_design -directive AlternateCLBRouting
        }
        "SpreadLogic_Explore" {
            puts "SpreadLogic_Explore strategy."
            opt_design
            place_design -directive AltSpreadLogic_high
            phys_opt_design -directive AggressiveExplore
            route_design -directive Explore
        }
        "SSI_SpreadLogic_high" {
            puts "SSI_SpreadLogic_high strategy."
            opt_design
            place_design -directive SSI_SpreadLogic_high
            phys_opt_design -directive AggressiveExplore
            route_design -directive AlternateCLBRouting
        }
        "SSI_SpreadLogic_low" {
            puts "SSI_SpreadLogic_low strategy."
            opt_design
            place_design -directive SSI_SpreadLogic_low
            phys_opt_design -directive AggressiveExplore
            route_design -directive AlternateCLBRouting
        }
        "SSI_SpreadLogic_Explore" {
            puts "SSI_SpreadLogic_Explore strategy."
            opt_design
            place_design -directive SSI_SpreadLogic_high
            phys_opt_design -directive AggressiveExplore
            route_design -directive Explore
        }
        "Area_Explore" {
            puts "Area_Explore strategy."
            opt_design -directive ExploreArea
            place_design
            route_design
        }
        "Area_ExploreSequential" {
            puts "Area_ExploreSequential strategy."
            opt_design -directive ExploreSequentialArea
            place_design
            route_design
        }
        "Area_ExploreWithRemap" {
            puts "Area_ExploreWithRemap strategy."
            opt_design -directive ExploreWithRemap
            place_design
            route_design
        }
        "Power_DefaultOpt" {
            puts "Power_DefaultOpt strategy."
            opt_design
            power_opt_design
            place_design
            route_design
        }
        "Power_ExploreArea" {
            puts "Power_ExploreArea strategy."
            opt_design -directive ExploreSequentialArea
            power_opt_design
            place_design
            route_design
        }
        "Flow_RunPhysOpt" {
            puts "Flow_RunPhysOpt strategy."
            opt_design
            place_design
            phys_opt_design -directive Explore        
            route_design
        }
        "Flow_RunPostRoutePhysOpt" {
            puts "Flow_RunPostRoutePhysOpt strategy."
            opt_design
            place_design
            phys_opt_design -directive Explore        
            route_design -tns_cleanup
            phys_opt_design  
        }
        "Flow_RuntimeOptimized" {
            puts "Flow_RuntimeOptimized strategy."
            opt_design -directive RuntimeOptimized
            place_design -directive RuntimeOptimized
            route_design -directive RuntimeOptimized        
        }
        "Flow_Quick" {
            puts "Flow_Quick strategy."
            opt_design -directive RuntimeOptimized
            place_design -directive Quick
            route_design -directive Quick        
        }
        "DEFAULT" {
            puts "DEFAULT strategy."
            opt_design
            place_design
            route_design
        }
        default {
       		puts "ERROR:'$impl_strategy' is NOT a valid strategy."
        	puts "Please check your strategy in usr_prj_cfg."
        	exit
        }
    }
    
    #******************************************************************************
    #         phys_opt_design
    #******************************************************************************
    #phys_opt_design  -directive Explore
    
    #******************************************************************************
    #         report timing_summary 
    #******************************************************************************
    report_timing_summary -file $UL_DIR/prj/build/reports/${PRJ_NAME}_final_timing_summary.rpt
    
    #******************************************************************************
    #         write SH_CL_routed checkpoint
    #******************************************************************************
    write_checkpoint -force $UL_DIR/prj/build/checkpoints/to_facs/${PRJ_NAME}_routed.dcp
    
    #******************************************************************************
    #         close_project
    #******************************************************************************
    close_project
}
#******************************************************************************
#         pr_verify
#******************************************************************************
if { $PR_EN==1} {

    pr_verify -full_check $UL_DIR/prj/build/checkpoints/to_facs/${PRJ_NAME}_routed.dcp $LIB_DIR/checkpoints/SH_UL_BB_routed.dcp
}
#******************************************************************************
#         open_checkpoint
#******************************************************************************
if { $BIT_EN==1} {

    open_checkpoint $UL_DIR/prj/build/checkpoints/to_facs/${PRJ_NAME}_routed.dcp
    
    #******************************************************************************
    #         write_bitstream
    #******************************************************************************
    set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design ]
    set_param bitstream.enablePR 4123
    write_bitstream -force -bin_file $UL_DIR/prj/build/checkpoints/to_facs/$PRJ_NAME
}
