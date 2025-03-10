/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for libcoap server
 *
 * @author      Raul Fuentes <>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#include "coap3/coap.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int server_coap_init(int argc, char **argv);

static const shell_command_t shell_commands[] = {
  { "coaps", "Start a libcoap server", server_coap_init },
  { NULL, NULL, NULL }
};

int
main(void) {
  /* we need a message queue for the thread running the shell in order to
   * receive potentially fast incoming networking packets */
  msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
  puts("RIOT libcoap server testing implementation");

  /* start shell */
  puts("All up, running the shell now");
  char line_buf[SHELL_DEFAULT_BUFSIZE];
  shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

  /* should be never reached */
  return 0;
}
