transcript on
if {[file exists rtl_work]} {
	vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

vlib lab61soc
vmap lab61soc lab61soc
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/lab61soc.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_reset_controller.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_reset_synchronizer.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0_avalon_st_adapter_005.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0_avalon_st_adapter.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_avalon_sc_fifo.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_sysid_qsys_0.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_sdram_pll.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_sdram.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_onchip_memory2_0.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_nios2_gen2_0.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_nios2_gen2_0_cpu.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_nios2_gen2_0_cpu_debug_slave_sysclk.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_nios2_gen2_0_cpu_debug_slave_tck.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_nios2_gen2_0_cpu_debug_slave_wrapper.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_nios2_gen2_0_cpu_test_bench.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_led.v}
vlog -sv -work work +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1 {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_irq_mapper.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0_avalon_st_adapter_005_error_adapter_0.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0_avalon_st_adapter_error_adapter_0.sv}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_avalon_st_handshake_clock_crosser.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_avalon_st_clock_crosser.v}
vlog -vlog01compat -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_std_synchronizer_nocut.v}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_merlin_width_adapter.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_merlin_burst_uncompressor.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0_rsp_mux.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_merlin_arbitrator.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0_rsp_demux.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0_cmd_mux.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0_cmd_demux.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_merlin_burst_adapter.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_merlin_burst_adapter_uncmpr.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0_router_007.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0_router_002.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/lab61soc_mm_interconnect_0_router.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_merlin_slave_agent.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_merlin_master_agent.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_merlin_slave_translator.sv}
vlog -sv -work lab61soc +incdir+C:/Users/calvi/Downloads/ECE\ 385/Lab\ 6/wk1/lab61soc/synthesis/submodules {C:/Users/calvi/Downloads/ECE 385/Lab 6/wk1/lab61soc/synthesis/submodules/altera_merlin_master_translator.sv}

