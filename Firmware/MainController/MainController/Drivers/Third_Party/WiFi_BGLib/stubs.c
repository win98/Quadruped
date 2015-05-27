
/*****************************************************************************
 *
 *
 *
 *
 *
 ****************************************************************************/

#include "cmd_def.h"
void wifi_default(const void*v)
{
}

void wifi_rsp_dfu_reset(const void* nul)
{
}

void wifi_rsp_dfu_flash_set_address(const struct wifi_msg_dfu_flash_set_address_rsp_t *msg)
{
}

void wifi_rsp_dfu_flash_upload(const struct wifi_msg_dfu_flash_upload_rsp_t *msg)
{
}

void wifi_rsp_dfu_flash_upload_finish(const struct wifi_msg_dfu_flash_upload_finish_rsp_t *msg)
{
}

void wifi_rsp_system_sync(const void* nul)
{
}

void wifi_rsp_system_reset(const void* nul)
{
}

void wifi_rsp_system_hello(const void* nul)
{
}

void wifi_rsp_system_set_max_power_saving_state(const struct wifi_msg_system_set_max_power_saving_state_rsp_t *msg)
{
}

void wifi_rsp_config_get_mac(const struct wifi_msg_config_get_mac_rsp_t *msg)
{
}

void wifi_rsp_config_set_mac(const struct wifi_msg_config_set_mac_rsp_t *msg)
{
}

void wifi_rsp_sme_wifi_on(const struct wifi_msg_sme_wifi_on_rsp_t *msg)
{
}

void wifi_rsp_sme_wifi_off(const struct wifi_msg_sme_wifi_off_rsp_t *msg)
{
}

void wifi_rsp_sme_power_on(const struct wifi_msg_sme_power_on_rsp_t *msg)
{
}

void wifi_rsp_sme_start_scan(const struct wifi_msg_sme_start_scan_rsp_t *msg)
{
}

void wifi_rsp_sme_stop_scan(const struct wifi_msg_sme_stop_scan_rsp_t *msg)
{
}

void wifi_rsp_sme_set_password(const struct wifi_msg_sme_set_password_rsp_t *msg)
{
}

void wifi_rsp_sme_connect_bssid(const struct wifi_msg_sme_connect_bssid_rsp_t *msg)
{
}

void wifi_rsp_sme_connect_ssid(const struct wifi_msg_sme_connect_ssid_rsp_t *msg)
{
}

void wifi_rsp_sme_disconnect(const struct wifi_msg_sme_disconnect_rsp_t *msg)
{
}

void wifi_rsp_sme_set_scan_channels(const struct wifi_msg_sme_set_scan_channels_rsp_t *msg)
{
}

void wifi_rsp_sme_set_operating_mode(const struct wifi_msg_sme_set_operating_mode_rsp_t *msg)
{
}

void wifi_rsp_sme_start_ap_mode(const struct wifi_msg_sme_start_ap_mode_rsp_t *msg)
{
}

void wifi_rsp_sme_stop_ap_mode(const struct wifi_msg_sme_stop_ap_mode_rsp_t *msg)
{
}

void wifi_rsp_sme_scan_results_sort_rssi(const struct wifi_msg_sme_scan_results_sort_rssi_rsp_t *msg)
{
}

void wifi_rsp_sme_ap_client_disconnect(const struct wifi_msg_sme_ap_client_disconnect_rsp_t *msg)
{
}

void wifi_rsp_sme_set_ap_password(const struct wifi_msg_sme_set_ap_password_rsp_t *msg)
{
}

void wifi_rsp_sme_set_ap_max_clients(const struct wifi_msg_sme_set_ap_max_clients_rsp_t *msg)
{
}

void wifi_rsp_sme_start_wps(const struct wifi_msg_sme_start_wps_rsp_t *msg)
{
}

void wifi_rsp_sme_stop_wps(const struct wifi_msg_sme_stop_wps_rsp_t *msg)
{
}

void wifi_rsp_sme_get_signal_quality(const struct wifi_msg_sme_get_signal_quality_rsp_t *msg)
{
}

void wifi_rsp_tcpip_start_tcp_server(const struct wifi_msg_tcpip_start_tcp_server_rsp_t *msg)
{
}

void wifi_rsp_tcpip_tcp_connect(const struct wifi_msg_tcpip_tcp_connect_rsp_t *msg)
{
}

void wifi_rsp_tcpip_start_udp_server(const struct wifi_msg_tcpip_start_udp_server_rsp_t *msg)
{
}

void wifi_rsp_tcpip_udp_connect(const struct wifi_msg_tcpip_udp_connect_rsp_t *msg)
{
}

void wifi_rsp_tcpip_configure(const struct wifi_msg_tcpip_configure_rsp_t *msg)
{
}

void wifi_rsp_tcpip_dns_configure(const struct wifi_msg_tcpip_dns_configure_rsp_t *msg)
{
}

void wifi_rsp_tcpip_dns_gethostbyname(const struct wifi_msg_tcpip_dns_gethostbyname_rsp_t *msg)
{
}

void wifi_rsp_tcpip_udp_bind(const struct wifi_msg_tcpip_udp_bind_rsp_t *msg)
{
}

void wifi_rsp_tcpip_dhcp_set_hostname(const struct wifi_msg_tcpip_dhcp_set_hostname_rsp_t *msg)
{
}

void wifi_rsp_endpoint_send(const struct wifi_msg_endpoint_send_rsp_t *msg)
{
}

void wifi_rsp_endpoint_set_streaming(const struct wifi_msg_endpoint_set_streaming_rsp_t *msg)
{
}

void wifi_rsp_endpoint_set_active(const struct wifi_msg_endpoint_set_active_rsp_t *msg)
{
}

void wifi_rsp_endpoint_set_streaming_destination(const struct wifi_msg_endpoint_set_streaming_destination_rsp_t *msg)
{
}

void wifi_rsp_endpoint_close(const struct wifi_msg_endpoint_close_rsp_t *msg)
{
}

void wifi_rsp_endpoint_set_transmit_size(const struct wifi_msg_endpoint_set_transmit_size_rsp_t *msg)
{
}

void wifi_rsp_endpoint_disable(const struct wifi_msg_endpoint_disable_rsp_t *msg)
{
}

void wifi_rsp_hardware_set_soft_timer(const struct wifi_msg_hardware_set_soft_timer_rsp_t *msg)
{
}

void wifi_rsp_hardware_external_interrupt_config(const struct wifi_msg_hardware_external_interrupt_config_rsp_t *msg)
{
}

void wifi_rsp_hardware_change_notification_config(const struct wifi_msg_hardware_change_notification_config_rsp_t *msg)
{
}

void wifi_rsp_hardware_change_notification_pullup(const struct wifi_msg_hardware_change_notification_pullup_rsp_t *msg)
{
}

void wifi_rsp_hardware_io_port_config_direction(const struct wifi_msg_hardware_io_port_config_direction_rsp_t *msg)
{
}

void wifi_rsp_hardware_io_port_config_open_drain(const struct wifi_msg_hardware_io_port_config_open_drain_rsp_t *msg)
{
}

void wifi_rsp_hardware_io_port_write(const struct wifi_msg_hardware_io_port_write_rsp_t *msg)
{
}

void wifi_rsp_hardware_io_port_read(const struct wifi_msg_hardware_io_port_read_rsp_t *msg)
{
}

void wifi_rsp_hardware_output_compare(const struct wifi_msg_hardware_output_compare_rsp_t *msg)
{
}

void wifi_rsp_hardware_adc_read(const struct wifi_msg_hardware_adc_read_rsp_t *msg)
{
}

void wifi_rsp_hardware_rtc_init(const struct wifi_msg_hardware_rtc_init_rsp_t *msg)
{
}

void wifi_rsp_hardware_rtc_set_time(const struct wifi_msg_hardware_rtc_set_time_rsp_t *msg)
{
}

void wifi_rsp_hardware_rtc_get_time(const struct wifi_msg_hardware_rtc_get_time_rsp_t *msg)
{
}

void wifi_rsp_hardware_rtc_set_alarm(const struct wifi_msg_hardware_rtc_set_alarm_rsp_t *msg)
{
}

void wifi_rsp_hardware_uart_conf(const struct wifi_msg_hardware_uart_conf_rsp_t *msg)
{
}

void wifi_rsp_hardware_uart_conf_get(const struct wifi_msg_hardware_uart_conf_get_rsp_t *msg)
{
}

void wifi_rsp_flash_ps_defrag(const struct wifi_msg_flash_ps_defrag_rsp_t *msg)
{
}

void wifi_rsp_flash_ps_dump(const struct wifi_msg_flash_ps_dump_rsp_t *msg)
{
}

void wifi_rsp_flash_ps_erase_all(const struct wifi_msg_flash_ps_erase_all_rsp_t *msg)
{
}

void wifi_rsp_flash_ps_save(const struct wifi_msg_flash_ps_save_rsp_t *msg)
{
}

void wifi_rsp_flash_ps_load(const struct wifi_msg_flash_ps_load_rsp_t *msg)
{
}

void wifi_rsp_flash_ps_erase(const struct wifi_msg_flash_ps_erase_rsp_t *msg)
{
}

void wifi_rsp_i2c_start_read(const struct wifi_msg_i2c_start_read_rsp_t *msg)
{
}

void wifi_rsp_i2c_start_write(const struct wifi_msg_i2c_start_write_rsp_t *msg)
{
}

void wifi_rsp_i2c_stop(const struct wifi_msg_i2c_stop_rsp_t *msg)
{
}

void wifi_rsp_https_enable(const struct wifi_msg_https_enable_rsp_t *msg)
{
}

void wifi_rsp_ethernet_set_dataroute(const struct wifi_msg_ethernet_set_dataroute_rsp_t *msg)
{
}

void wifi_rsp_ethernet_close(const void* nul)
{
}

void wifi_rsp_ethernet_connected(const struct wifi_msg_ethernet_connected_rsp_t *msg)
{
}

void wifi_evt_dfu_boot(const struct wifi_msg_dfu_boot_evt_t *msg)
{
}

void wifi_evt_system_boot(const struct wifi_msg_system_boot_evt_t *msg)
{
}

void wifi_evt_system_state(const struct wifi_msg_system_state_evt_t *msg)
{
}

void wifi_evt_system_sw_exception(const struct wifi_msg_system_sw_exception_evt_t *msg)
{
}

void wifi_evt_system_power_saving_state(const struct wifi_msg_system_power_saving_state_evt_t *msg)
{
}

void wifi_evt_config_mac_address(const struct wifi_msg_config_mac_address_evt_t *msg)
{
}

void wifi_evt_sme_wifi_is_on(const struct wifi_msg_sme_wifi_is_on_evt_t *msg)
{
}

void wifi_evt_sme_wifi_is_off(const struct wifi_msg_sme_wifi_is_off_evt_t *msg)
{
}

void wifi_evt_sme_scan_result(const struct wifi_msg_sme_scan_result_evt_t *msg)
{
}

void wifi_evt_sme_scan_result_drop(const struct wifi_msg_sme_scan_result_drop_evt_t *msg)
{
}

void wifi_evt_sme_scanned(const struct wifi_msg_sme_scanned_evt_t *msg)
{
}

void wifi_evt_sme_connected(const struct wifi_msg_sme_connected_evt_t *msg)
{
}

void wifi_evt_sme_disconnected(const struct wifi_msg_sme_disconnected_evt_t *msg)
{
}

void wifi_evt_sme_interface_status(const struct wifi_msg_sme_interface_status_evt_t *msg)
{
}

void wifi_evt_sme_connect_failed(const struct wifi_msg_sme_connect_failed_evt_t *msg)
{
}

void wifi_evt_sme_connect_retry(const struct wifi_msg_sme_connect_retry_evt_t *msg)
{
}

void wifi_evt_sme_ap_mode_started(const struct wifi_msg_sme_ap_mode_started_evt_t *msg)
{
}

void wifi_evt_sme_ap_mode_stopped(const struct wifi_msg_sme_ap_mode_stopped_evt_t *msg)
{
}

void wifi_evt_sme_ap_mode_failed(const struct wifi_msg_sme_ap_mode_failed_evt_t *msg)
{
}

void wifi_evt_sme_ap_client_joined(const struct wifi_msg_sme_ap_client_joined_evt_t *msg)
{
}

void wifi_evt_sme_ap_client_left(const struct wifi_msg_sme_ap_client_left_evt_t *msg)
{
}

void wifi_evt_sme_scan_sort_result(const struct wifi_msg_sme_scan_sort_result_evt_t *msg)
{
}

void wifi_evt_sme_scan_sort_finished(const void* nul)
{
}

void wifi_evt_sme_wps_stopped(const struct wifi_msg_sme_wps_stopped_evt_t *msg)
{
}

void wifi_evt_sme_wps_completed(const struct wifi_msg_sme_wps_completed_evt_t *msg)
{
}

void wifi_evt_sme_wps_failed(const struct wifi_msg_sme_wps_failed_evt_t *msg)
{
}

void wifi_evt_sme_wps_credential_ssid(const struct wifi_msg_sme_wps_credential_ssid_evt_t *msg)
{
}

void wifi_evt_sme_wps_credential_password(const struct wifi_msg_sme_wps_credential_password_evt_t *msg)
{
}

void wifi_evt_sme_signal_quality(const struct wifi_msg_sme_signal_quality_evt_t *msg)
{
}

void wifi_evt_tcpip_configuration(const struct wifi_msg_tcpip_configuration_evt_t *msg)
{
}

void wifi_evt_tcpip_dns_configuration(const struct wifi_msg_tcpip_dns_configuration_evt_t *msg)
{
}

void wifi_evt_tcpip_endpoint_status(const struct wifi_msg_tcpip_endpoint_status_evt_t *msg)
{
}

void wifi_evt_tcpip_dns_gethostbyname_result(const struct wifi_msg_tcpip_dns_gethostbyname_result_evt_t *msg)
{
}

void wifi_evt_tcpip_udp_data(const struct wifi_msg_tcpip_udp_data_evt_t *msg)
{
}

void wifi_evt_endpoint_syntax_error(const struct wifi_msg_endpoint_syntax_error_evt_t *msg)
{
}

void wifi_evt_endpoint_data(const struct wifi_msg_endpoint_data_evt_t *msg)
{
}

void wifi_evt_endpoint_status(const struct wifi_msg_endpoint_status_evt_t *msg)
{
}

void wifi_evt_endpoint_closing(const struct wifi_msg_endpoint_closing_evt_t *msg)
{
}

void wifi_evt_endpoint_error(const struct wifi_msg_endpoint_error_evt_t *msg)
{
}

void wifi_evt_hardware_soft_timer(const struct wifi_msg_hardware_soft_timer_evt_t *msg)
{
}

void wifi_evt_hardware_change_notification(const struct wifi_msg_hardware_change_notification_evt_t *msg)
{
}

void wifi_evt_hardware_external_interrupt(const struct wifi_msg_hardware_external_interrupt_evt_t *msg)
{
}

void wifi_evt_hardware_rtc_alarm(const void* nul)
{
}

void wifi_evt_hardware_uart_conf(const struct wifi_msg_hardware_uart_conf_evt_t *msg)
{
}

void wifi_evt_flash_ps_key(const struct wifi_msg_flash_ps_key_evt_t *msg)
{
}

void wifi_evt_flash_ps_key_changed(const struct wifi_msg_flash_ps_key_changed_evt_t *msg)
{
}

void wifi_evt_flash_low_voltage(const void* nul)
{
}

void wifi_evt_https_on_req(const struct wifi_msg_https_on_req_evt_t *msg)
{
}

void wifi_evt_https_button(const struct wifi_msg_https_button_evt_t *msg)
{
}

void wifi_evt_ethernet_link_status(const struct wifi_msg_ethernet_link_status_evt_t *msg)
{
}

