#----------------------摄像头接口的时钟---------------------------
#72M
create_clock -period 13.888 -name cam_pclk [get_ports cam_pclk_0]
set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets cam_pclk_0_IBUF]

create_clock -period 13.888 -name cam_pclk [get_ports cam_pclk_1]
set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets cam_pclk_1_IBUF]


#----------------------摄像头接口1---------------------------
set_property -dict {PACKAGE_PIN M19 IOSTANDARD LVCMOS33} [get_ports cam_rst_n_0]
set_property -dict {PACKAGE_PIN G19 IOSTANDARD LVCMOS33} [get_ports cam_pwdn_0]
set_property -dict {PACKAGE_PIN M20 IOSTANDARD LVCMOS33} [get_ports {cam_data_0[0]}]
set_property -dict {PACKAGE_PIN L19 IOSTANDARD LVCMOS33} [get_ports {cam_data_0[1]}]
set_property -dict {PACKAGE_PIN L20 IOSTANDARD LVCMOS33} [get_ports {cam_data_0[2]}]
set_property -dict {PACKAGE_PIN K19 IOSTANDARD LVCMOS33} [get_ports {cam_data_0[3]}]
set_property -dict {PACKAGE_PIN J19 IOSTANDARD LVCMOS33} [get_ports {cam_data_0[4]}]
set_property -dict {PACKAGE_PIN H16 IOSTANDARD LVCMOS33} [get_ports {cam_data_0[5]}]
set_property -dict {PACKAGE_PIN H17 IOSTANDARD LVCMOS33} [get_ports {cam_data_0[6]}]
set_property -dict {PACKAGE_PIN G17 IOSTANDARD LVCMOS33} [get_ports {cam_data_0[7]}]
set_property -dict {PACKAGE_PIN M17 IOSTANDARD LVCMOS33} [get_ports cam_href_0]
set_property -dict {PACKAGE_PIN G18 IOSTANDARD LVCMOS33} [get_ports cam_pclk_0]
set_property -dict {PACKAGE_PIN N20 IOSTANDARD LVCMOS33} [get_ports cam_vsync_0]
#cam_scl:
set_property -dict {PACKAGE_PIN P20 IOSTANDARD LVCMOS33} [get_ports {emio_sccb_tri_io[0]}]
#cam_sda:
set_property -dict {PACKAGE_PIN M18 IOSTANDARD LVCMOS33} [get_ports {emio_sccb_tri_io[1]}]
#pull up
set_property PULLUP true [get_ports {emio_sccb_tri_io[0]}]
set_property PULLUP true [get_ports {emio_sccb_tri_io[1]}]

#----------------------摄像头接口2---------------------------
set_property -dict {PACKAGE_PIN E19 IOSTANDARD LVCMOS33} [get_ports cam_rst_n_1]
set_property -dict {PACKAGE_PIN H15 IOSTANDARD LVCMOS33} [get_ports cam_pwdn_1]
set_property -dict {PACKAGE_PIN E18 IOSTANDARD LVCMOS33} [get_ports {cam_data_1[0]}]
set_property -dict {PACKAGE_PIN A20 IOSTANDARD LVCMOS33} [get_ports {cam_data_1[1]}]
set_property -dict {PACKAGE_PIN B19 IOSTANDARD LVCMOS33} [get_ports {cam_data_1[2]}]
set_property -dict {PACKAGE_PIN B20 IOSTANDARD LVCMOS33} [get_ports {cam_data_1[3]}]
set_property -dict {PACKAGE_PIN C20 IOSTANDARD LVCMOS33} [get_ports {cam_data_1[4]}]
set_property -dict {PACKAGE_PIN D20 IOSTANDARD LVCMOS33} [get_ports {cam_data_1[5]}]
set_property -dict {PACKAGE_PIN D19 IOSTANDARD LVCMOS33} [get_ports {cam_data_1[6]}]
set_property -dict {PACKAGE_PIN F20 IOSTANDARD LVCMOS33} [get_ports {cam_data_1[7]}]
set_property -dict {PACKAGE_PIN D18 IOSTANDARD LVCMOS33} [get_ports cam_href_1]
set_property -dict {PACKAGE_PIN F19 IOSTANDARD LVCMOS33} [get_ports cam_pclk_1]
set_property -dict {PACKAGE_PIN F17 IOSTANDARD LVCMOS33} [get_ports cam_vsync_1]
#cam_scl:
set_property -dict {PACKAGE_PIN F16 IOSTANDARD LVCMOS33} [get_ports {emio_sccb_tri_io[2]}]
#cam_sda:
set_property -dict {PACKAGE_PIN E17 IOSTANDARD LVCMOS33} [get_ports {emio_sccb_tri_io[3]}]
#pull up
set_property PULLUP true [get_ports {emio_sccb_tri_io[2]}]
set_property PULLUP true [get_ports {emio_sccb_tri_io[3]}]

set_property PACKAGE_PIN K16 [get_ports {TMDS_tmds_data_p[1]}]
set_property PACKAGE_PIN L14 [get_ports {TMDS_tmds_data_p[2]}]
set_property PACKAGE_PIN K14 [get_ports {TMDS_tmds_data_p[0]}]
set_property PACKAGE_PIN K17 [get_ports TMDS_tmds_clk_p]

set_property PACKAGE_PIN T11 [get_ports emio_UART_0_rxd]
set_property PACKAGE_PIN T12 [get_ports emio_UART_0_txd]

set_property IOSTANDARD LVCMOS33 [get_ports emio_UART_0_rxd]
set_property IOSTANDARD LVCMOS33 [get_ports emio_UART_0_txd]
