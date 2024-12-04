#include "wmsdk_config.h"

#include "lwip_check.h"

#include "ip4/test_ip4.h"
#include "ip6/test_ip6.h"
#include "udp/test_udp.h"
#include "tcp/test_tcp.h"
#include "tcp/test_tcp_oos.h"
#include "core/test_def.h"
#include "core/test_mem.h"
#include "core/test_netif.h"
#include "core/test_pbuf.h"
#include "core/test_timers.h"
#include "etharp/test_etharp.h"
#include "dhcp/test_dhcp.h"
//#include "mdns/test_mdns.h"
//#include "mqtt/test_mqtt.h"
#include "api/test_sockets.h"

#include "lwip/init.h"
#if !NO_SYS
#include "lwip/tcpip.h"
#endif

Suite* create_suite(const char* name, testfunc *tests, size_t num_tests, SFun setup, SFun teardown)
{
  size_t i;
  Suite *s = suite_create(name);

  for(i = 0; i < num_tests; i++) {
    TCase *tc_core = tcase_create(name);
    if ((setup != NULL) || (teardown != NULL)) {
      tcase_add_checked_fixture(tc_core, setup, teardown);
    }
    tcase_add_named_test(tc_core, tests[i]);
    suite_add_tcase(s, tc_core);
  }
  return s;
}

void lwip_check_ensure_no_alloc(unsigned int skip)
{
#if MEMP_STATS
  int i;
  unsigned int mask;
#endif

#if MEM_STATS
  if (!(skip & SKIP_HEAP)) {
    fail_unless(lwip_stats.mem.used == 0);
  }
#endif
#if MEMP_STATS
  for (i = 0, mask = 1; i < MEMP_MAX; i++, mask <<= 1) {
    if (!(skip & mask)) {
      fail_unless(lwip_stats.memp[i]->used == 0);
    }
  }
#endif
}

#ifdef LWIP_UNITTESTS_LIB
int lwip_unittests_run(void)
#else
int main(void)
#endif
{
  int number_failed;
  SRunner *sr;
  size_t i;
  suite_getter_fn* suites[] = {
#if CONFIG_UNIT_TEST_LWIP_IP4
    ip4_suite,
#endif
#if CONFIG_UNIT_TEST_LWIP_IP6
#if LWIP_IPV6
    ip6_suite,
#endif
#endif
#if CONFIG_UNIT_TEST_LWIP_UDP
    udp_suite,
#endif
#if CONFIG_UNIT_TEST_LWIP_TCP
    tcp_suite,
#endif
#if CONFIG_UNIT_TEST_LWIP_TCP_OOS
    tcp_oos_suite,
#endif
#if CONFIG_UNIT_TEST_LWIP_DEF
    def_suite,
#endif
    //mem_suite,
#if CONFIG_UNIT_TEST_LWIP_NETIF
    netif_suite,
#endif
#if CONFIG_UNIT_TEST_LWIP_PBUF
    pbuf_suite,
#endif
    //timers_suite,
#if CONFIG_UNIT_TEST_LWIP_ETHARP
    etharp_suite,
#endif
#if CONFIG_UNIT_TEST_LWIP_DHCP
    dhcp_suite,
#endif
    //mdns_suite,
    //mqtt_suite,
#if CONFIG_UNIT_TEST_LWIP_SOCKETS
    sockets_suite
#endif
  };
  size_t num = sizeof(suites)/sizeof(void*);
  LWIP_ASSERT("No suites defined", num > 0);

#if 0
#if NO_SYS
  lwip_init();
#else
  tcpip_init(NULL, NULL);
#endif
#endif

  sr = srunner_create((suites[0])());
  //srunner_set_xml(sr, "lwip_unittests.xml");
  for(i = 1; i < num; i++) {
    srunner_add_suite(sr, ((suite_getter_fn*)suites[i])());
  }

#ifdef LWIP_UNITTESTS_NOFORK
  srunner_set_fork_status(sr, CK_NOFORK);
#endif
#ifdef LWIP_UNITTESTS_FORK
  srunner_set_fork_status(sr, CK_FORK);
#endif

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
