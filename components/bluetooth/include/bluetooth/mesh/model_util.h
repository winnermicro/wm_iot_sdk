/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __MODEL_UTIL_H__
#define __MODEL_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define OP_GEN_ONOFF_GET               BT_MESH_MODEL_OP_2(0x82, 0x01)
#define OP_GEN_ONOFF_SET               BT_MESH_MODEL_OP_2(0x82, 0x02)
#define OP_GEN_ONOFF_SET_UNACK         BT_MESH_MODEL_OP_2(0x82, 0x03)
#define OP_GEN_ONOFF_STATUS            BT_MESH_MODEL_OP_2(0x82, 0x04)
#define OP_GEN_LEVEL_GET               BT_MESH_MODEL_OP_2(0x82, 0x05)
#define OP_GEN_LEVEL_SET               BT_MESH_MODEL_OP_2(0x82, 0x06)
#define OP_GEN_LEVEL_SET_UNACK         BT_MESH_MODEL_OP_2(0x82, 0x07)
#define OP_GEN_LEVEL_STATUS            BT_MESH_MODEL_OP_2(0x82, 0x08)
#define OP_GEN_DELTA_SET               BT_MESH_MODEL_OP_2(0x82, 0x09)
#define OP_GEN_DELTA_SET_UNACK         BT_MESH_MODEL_OP_2(0x82, 0x0a)
#define OP_GEN_MOVE_SET                BT_MESH_MODEL_OP_2(0x82, 0x0b)
#define OP_GEN_MOVE_SET_UNACK          BT_MESH_MODEL_OP_2(0x82, 0x0c)
#define OP_LIGHT_LIGHTNESS_GET         BT_MESH_MODEL_OP_2(0x82, 0x4b)
#define OP_LIGHT_LIGHTNESS_SET         BT_MESH_MODEL_OP_2(0x82, 0x4c)
#define OP_LIGHT_LIGHTNESS_SET_UNACK   BT_MESH_MODEL_OP_2(0x82, 0x4d)
#define OP_LIGHT_LIGHTNESS_STATUS      BT_MESH_MODEL_OP_2(0x82, 0x4e)


typedef struct {
	bool val;
	uint8_t tid;
	uint16_t src;
	uint32_t transition_time;
	struct k_work_delayable work;
} onoff_t;

int32_t model_time_decode(uint8_t val);
uint8_t model_time_encode(int32_t ms);



#ifdef __cplusplus
}
#endif

#endif /* __MODEL_UTIL_H__ */

