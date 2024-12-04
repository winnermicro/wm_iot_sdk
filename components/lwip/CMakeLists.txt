if(CONFIG_COMPONENT_LWIP_ENABLED)

    list(APPEND ADD_INCLUDE "src/include"
			    "port"
                            "src/include/compat/posix")

    #list(APPEND ADD_PRIVATE_INCLUDE ""
    #                                )

    list(APPEND ADD_SRCS "src/api/api_lib.c"
                         "src/api/api_msg.c"
                         "src/api/err.c"
                         "src/api/if_api.c"
                         "src/api/netbuf.c"
                         "src/api/netdb.c"
                         "src/api/netifapi.c"
                         "src/api/sockets.c"
                         "src/api/tcpip.c"
                         "src/core/altcp.c"
                         "src/core/altcp_alloc.c"
                         "src/core/altcp_tcp.c"
                         "src/core/def.c"
                         "src/core/dns.c"
                         "src/core/inet_chksum.c"
                         "src/core/init.c"
                         "src/core/ip.c"
                         "src/core/mem.c"
                         "src/core/memp.c"
                         "src/core/netif.c"
                         "src/core/pbuf.c"
                         "src/core/raw.c"
                         "src/core/stats.c"
                         "src/core/sys.c"
                         "src/core/tcp.c"
                         "src/core/tcp_in.c"
                         "src/core/tcp_out.c"
                         "src/core/timeouts.c"
                         "src/core/udp.c"
                         "src/core/ipv4/autoip.c"
                         "src/core/ipv4/dhcp.c"
                         "src/core/ipv4/etharp.c"
                         "src/core/ipv4/icmp.c"
                         "src/core/ipv4/igmp.c"
                         "src/core/ipv4/ip4.c"
                         "src/core/ipv4/ip4_addr.c"
                         "src/core/ipv4/ip4_frag.c"
                         "src/netif/bridgeif.c"
                         "src/netif/bridgeif_fdb.c"
                         "src/netif/ethernet.c"
                         "port/sys_arch.c"
                         "port/sockets_ext.c"
                         "port/wm_lwip.c"
                         "src/apps/dhcp_server/dhcp_server.c"
                         "src/apps/wm_ping/wm_ping.c"
                         "src/apps/sntp/sntp.c"
                         "src/apps/wm_sntp/wm_sntp.c"
                         )

    if(CONFIG_LWIP_ENABLE_IPV6)
        list(APPEND ADD_SRCS "src/core/ipv6/dhcp6.c"
                             "src/core/ipv6/ethip6.c"
                             "src/core/ipv6/icmp6.c"
                             "src/core/ipv6/inet6.c"
                             "src/core/ipv6/ip6.c"
                             "src/core/ipv6/ip6_addr.c"
                             "src/core/ipv6/ip6_frag.c"
                             "src/core/ipv6/mld6.c"
                             "src/core/ipv6/nd6.c"
                             )
    endif()

    if(CONFIG_LWIP_ENABLE_NAPT)
        list(APPEND ADD_SRCS "src/core/ipv4/ip4_napt.c")
    endif()

    if(CONFIG_LWIP_ENABLE_NAPT)
        list(APPEND ADD_SRCS "src/core/ipv4/ip4_napt.c")
    endif()

    if(CONFIG_LWIP_ENABLE_DNS_SERVER)
        list(APPEND ADD_SRCS "src/apps/dns_server/dns_server.c")
    endif()

    if(CONFIG_UNIT_TEST_ENABLE_LWIP_TEST)
        list(APPEND ADD_INCLUDE "test/unit"
                                )
        append_srcs_dir(ADD_SRCS "test/unit"
                                 "test/unit/api"
                                 "test/unit/core"
                                 "test/unit/dhcp"
                                 "test/unit/etharp"
                                 "test/unit/ip4"
                                 "test/unit/tcp"
                                 "test/unit/udp"
                                 )
        if(CONFIG_LWIP_ENABLE_IPV6)
            append_srcs_dir(ADD_SRCS "test/unit/ip6"
                                     )
        endif()
        list(REMOVE_ITEM ADD_SRCS "test/unit/core/test_mem.c"
                                  "test/unit/core/test_timers.c"
                                  )

        list(APPEND ADD_DEFINITIONS -DLWIP_UNITTESTS_LIB
                                    -DLWIP_UNITTESTS_NOFORK
                                    )
    endif()

    list(APPEND ADD_DEFINITIONS -DCONFIG_LWIP_ENABLE_DEINIT
                                )

    if (CONFIG_UNIT_TEST_ENABLE_CODE_COVERAGE)
        list(APPEND ADD_DEFINITIONS -fno-profile-arcs -fno-test-coverage)
    endif()

    register_component()

endif()