#ifndef __PING_H__
#define __PING_H__

typedef struct wm_ping_stat {
    uint32_t sent_num;
    uint32_t rcvd_num;
    uint32_t total_time;
    uint32_t time_spent;
} wm_ping_stat_t;

typedef void (*wm_ping_stat_cb_func_t)(void *cb_para, wm_ping_stat_t *stat);

typedef struct wm_ping_param {
    uint32_t ping_max_send_times; //0: not care
    uint16_t ping_data_size;
    uint16_t ping_timeout;  //ms
    uint32_t ping_max_time; //0: not care, ms

    wm_ping_stat_cb_func_t cb;
    void *cb_param;
} wm_ping_param_t;

int wm_net_ping(const char *target);
int wm_net_ping_withcb(const char *target, wm_ping_param_t *pararm, wm_ping_stat_cb_func_t cb, void *cb_param);

int wm_ping_async(const ip_addr_t *ping_addr, wm_ping_param_t *para, wm_ping_stat_cb_func_t cb, void *cb_para);

#endif /* __PING_H__ */
