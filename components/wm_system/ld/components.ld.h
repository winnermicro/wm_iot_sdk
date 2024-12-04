  KEEP(*(SORT(.wm_component_init0_* )))
  KEEP(*(SORT(.wm_component_init_* )))

  wm_dt_table_start = .;
  KEEP (*(.wm_dt_table))
  wm_dt_table_end = .;

#if CONFIG_COMPONENT_WM_ATCMD_ENABLED
  wm_at_table_start = .;
  KEEP (*(.wm_at_table))
  wm_at_table_end = .;
#endif

#if CONFIG_COMPONENT_CLI_ENABLED
  wm_cli_cmd_start = .;
  KEEP (*(.wm_cli_cmd))
  wm_cli_cmd_end = .;
#endif

#if CONFIG_COMPONENT_BT_ENABLED
  . = ALIGN(0x4) ;
  _bt_gatt_service_static_list_start = .;
  KEEP(*(SORT_BY_NAME(._bt_gatt_service_static.static.*)))
  _bt_gatt_service_static_list_end = .;
  . = ALIGN (4) ;
  _bt_conn_cb_list_start = .;
  KEEP(*(SORT_BY_NAME(._bt_conn_cb.static.*)))
  _bt_conn_cb_list_end = .;
  . = ALIGN (4) ;
  _bt_l2cap_fixed_chan_list_start = .;
  KEEP(*(SORT_BY_NAME(._bt_l2cap_fixed_chan.static.*)))
  _bt_l2cap_fixed_chan_list_end = .;
  . = ALIGN(0x4) ;
  _settings_handler_static_list_start = .;
  KEEP(*(SORT_BY_NAME(._settings_handler_static.static.*)))
  _settings_handler_static_list_end = .;
  . = ALIGN(0x4) ;
#if CONFIG_BT_MESH
  . = ALIGN (4) ;
  _bt_mesh_subnet_cb_list_start = .;
  KEEP(*(SORT_BY_NAME(._bt_mesh_subnet_cb.static.*)))
  _bt_mesh_subnet_cb_list_end = .;
  . = ALIGN (4) ;
  _bt_mesh_app_key_cb_list_start = .;
  KEEP(*(SORT_BY_NAME(._bt_mesh_app_key_cb.static.*)))
  _bt_mesh_app_key_cb_list_end = .;
  . = ALIGN (4) ;
  _bt_mesh_hb_cb_list_start = .;
  KEEP(*(SORT_BY_NAME(._bt_mesh_hb_cb.static.*)))
  _bt_mesh_hb_cb_list_end = .;
   . = ALIGN (4) ;
  _bt_mesh_proxy_cb_list_start = .;
  KEEP(*(SORT_BY_NAME(._bt_mesh_proxy_cb.static.*)))
  _bt_mesh_proxy_cb_list_end = .;
     . = ALIGN (4) ;
  _bt_mesh_friend_cb_list_start = .;
  KEEP(*(SORT_BY_NAME(._bt_mesh_friend_cb.static.*)))
  _bt_mesh_friend_cb_list_end = .;
       . = ALIGN (4) ;
  _bt_mesh_ext_adv_list_start = .;
  KEEP(*(SORT_BY_NAME(._bt_mesh_ext_adv.static.*)))
  _bt_mesh_ext_adv_list_end = .;
#endif
#endif
