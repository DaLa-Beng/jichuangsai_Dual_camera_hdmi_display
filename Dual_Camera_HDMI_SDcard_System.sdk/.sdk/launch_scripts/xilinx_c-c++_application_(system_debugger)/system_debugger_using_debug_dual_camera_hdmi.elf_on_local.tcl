connect -url tcp:127.0.0.1:3121
source E:/User_Coding/Xilinx_prj_2018_3/Dual_Camera_HDMI_SDcard_System/Dual_Camera_HDMI_SDcard_System.sdk/system_wrapper_hw_platform_2/ps7_init.tcl
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Avnet MiniZed V1 1234-oj1A"} -index 0
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Avnet MiniZed V1 1234-oj1A" && level==0} -index 1
fpga -file E:/User_Coding/Xilinx_prj_2018_3/Dual_Camera_HDMI_SDcard_System/Dual_Camera_HDMI_SDcard_System.sdk/system_wrapper_hw_platform_2/system_wrapper.bit
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Avnet MiniZed V1 1234-oj1A"} -index 0
loadhw -hw E:/User_Coding/Xilinx_prj_2018_3/Dual_Camera_HDMI_SDcard_System/Dual_Camera_HDMI_SDcard_System.sdk/system_wrapper_hw_platform_2/system.hdf -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Avnet MiniZed V1 1234-oj1A"} -index 0
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Avnet MiniZed V1 1234-oj1A"} -index 0
dow E:/User_Coding/Xilinx_prj_2018_3/Dual_Camera_HDMI_SDcard_System/Dual_Camera_HDMI_SDcard_System.sdk/dual_camera_hdmi/Debug/dual_camera_hdmi.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Avnet MiniZed V1 1234-oj1A"} -index 0
con
