
/*! ------------------------------------------------------------------------------------------------------------------
 * @file    ext_api_fulltest.c
 * @brief   Decawave device configuration and control functions
 *
 * @attention
 *
 * Copyright 2017 (c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 */

#include <string.h>
#include "dwm_api.h"
#include "hal.h"
#include "hal_log.h"
#include "../test_util/test_util.h"


int main(int argc, char*argv[])
{   
	int RV_set_anchor;
	int RV_set_pos;
	
	dwm_cfg_anchor_t cfg_an;
	dwm_cfg_t cfg_node;
   
	printf("dwm_init(): dev%d\n", HAL_DevNum());
	dwm_init();
	
	dwm_cfg_get(&cfg_node);

	printf("Setting to anchor: dev%d.\n", HAL_DevNum());
	cfg_an.initiator = 1;
	cfg_an.bridge = 0;
	cfg_an.common.enc_en = 0;
	cfg_an.common.led_en = 1;
	cfg_an.common.ble_en = 0;
	cfg_an.common.uwb_mode = DWM_UWB_MODE_ACTIVE;//DWM_UWB_MODE_ACTIVE;
	cfg_an.common.fw_update_en = 1;
	printf("dwm_cfg_anchor_set(&cfg_anchor): dev%d.\n", HAL_DevNum());
	RV_set_anchor = dwm_cfg_anchor_set(&cfg_an); 
	
	dwm_reset();
	printf("Wait 2s for node to reset\n");
	HAL_Delay(2000);
	
	printf("Getting configurations: dev%d.\n", HAL_DevNum());
	dwm_cfg_get(&cfg_node);

	printf("Comparing set vs. get: dev%d.\n", HAL_DevNum());
	printf("initiator           cfg_anchor=%d : cfg_node=%d\n", cfg_an.initiator, cfg_node.initiator); 
	printf("bridge              cfg_anchor=%d : cfg_node=%d\n", cfg_an.bridge, cfg_node.bridge); 
	printf("common.led_en       cfg_anchor=%d : cfg_node=%d\n", cfg_an.common.led_en, cfg_node.common.led_en); 
	printf("common.ble_en       cfg_anchor=%d : cfg_node=%d\n", cfg_an.common.ble_en, cfg_node.common.ble_en); 
	printf("common.uwb_mode     cfg_anchor=%d : cfg_node=%d\n", cfg_an.common.uwb_mode, cfg_node.common.uwb_mode); 
	printf("common.fw_update_en cfg_anchor=%d : cfg_node=%d\n", cfg_an.common.fw_update_en, cfg_node.common.fw_update_en); 
	
	
	printf("Setting anchor's position\n");
	dwm_pos_t pos_set;
	pos_set.qf = 100;
	pos_set.x = 0;
	pos_set.y = 0;
	pos_set.z = 0;
	printf("\t[%d,%d,%d,%u]\n", pos_set.x, pos_set.y, pos_set.z,pos_set.qf);
	RV_set_pos = dwm_pos_set(&pos_set);
	
	dwm_pos_t pos_get;
	dwm_pos_get(&pos_get);
	printf("Getting anchor's position\n");
	printf("\t[%d,%d,%d,%u]\n", pos_get.x, pos_get.y, pos_get.z,pos_get.qf);
	
	
	if((cfg_an.initiator != cfg_node.initiator) 
	|| (cfg_an.bridge != cfg_node.bridge) 
	|| (cfg_an.common.enc_en != cfg_node.common.enc_en)
	|| (cfg_an.common.led_en != cfg_node.common.led_en) 
	|| (cfg_an.common.ble_en != cfg_node.common.ble_en) 
	|| (cfg_an.common.uwb_mode != cfg_node.common.uwb_mode) 
	|| (cfg_an.common.fw_update_en != cfg_node.common.fw_update_en)
	|| (RV_set_pos != 0) || (RV_set_anchor != 0))
	{
	  printf("\nConfiguration failed.\n\n");
	}
	else
	{
	  printf("\nConfiguration succeeded.\n\n");
	}
}


