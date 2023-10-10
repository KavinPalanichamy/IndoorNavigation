#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "dwm_api.h"
#include "hal.h"

// Function to establish a TCP connection and send a message
int tcp_connect(const char* message) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(4568);
    serverAddress.sin_addr.s_addr = inet_addr("10.2.1.223");

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error connecting to the server");
        close(clientSocket);
        return 1;
    }
   

    if (send(clientSocket, message, strlen(message), 0) == -1) {
        perror("Error sending data to the server");
        close(clientSocket);
        return 1;
    }

    close(clientSocket);

    return 0;
}

int gps_receive(){


}

int main(void) {

    //Init. Tag
    int i;
    int wait_period = 1000;
    dwm_cfg_tag_t cfg_tag;
    dwm_cfg_t cfg_node;

    HAL_Print("dwm_init(): dev%d\n", HAL_DevNum());
    dwm_init();

    HAL_Print("Setting to tag: dev%d.\n", HAL_DevNum());
    cfg_tag.low_power_en = 0;
    cfg_tag.meas_mode = DWM_MEAS_MODE_TWR;
    cfg_tag.loc_engine_en = 1;
    cfg_tag.common.led_en = 1;
    cfg_tag.common.ble_en = 1;
    cfg_tag.common.uwb_mode = DWM_UWB_MODE_ACTIVE;
    cfg_tag.common.fw_update_en = 0;
    HAL_Print("dwm_cfg_tag_set(&cfg_tag): dev%d.\n", HAL_DevNum());
    dwm_cfg_tag_set(&cfg_tag);

    HAL_Print("Wait 2s for node to reset.\n");
    HAL_Delay(2000);
    dwm_cfg_get(&cfg_node);

    HAL_Print("Comparing set vs. get: dev%d.\n", HAL_DevNum());
    if ((cfg_tag.low_power_en != cfg_node.low_power_en)
        || (cfg_tag.meas_mode != cfg_node.meas_mode)
        || (cfg_tag.loc_engine_en != cfg_node.loc_engine_en)
        || (cfg_tag.common.led_en != cfg_node.common.led_en)
        || (cfg_tag.common.ble_en != cfg_node.common.ble_en)
        || (cfg_tag.common.uwb_mode != cfg_node.common.uwb_mode)
        || (cfg_tag.common.fw_update_en != cfg_node.common.fw_update_en)) {
        HAL_Print("low_power_en        cfg_tag=%d : cfg_node=%d\n", cfg_tag.low_power_en, cfg_node.low_power_en);
        HAL_Print("meas_mode           cfg_tag=%d : cfg_node=%d\n", cfg_tag.meas_mode, cfg_node.meas_mode);
        HAL_Print("loc_engine_en       cfg_tag=%d : cfg_node=%d\n", cfg_tag.loc_engine_en, cfg_node.loc_engine_en);
        HAL_Print("common.led_en       cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.led_en, cfg_node.common.led_en);
        HAL_Print("common.ble_en       cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.ble_en, cfg_node.common.ble_en);
        HAL_Print("common.uwb_mode     cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.uwb_mode, cfg_node.common.uwb_mode);
        HAL_Print("common.fw_update_en cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.fw_update_en, cfg_node.common.fw_update_en);
        HAL_Print("\nConfiguration failed.\n\n");
    } else {
        HAL_Print("\nConfiguration succeeded.\n\n");
    }//End of Init. Tag

    
    //Successively receive Location in regular intervals
    dwm_loc_data_t loc;
    dwm_pos_t pos;
    loc.p_pos = &pos;
    while (1) {
        HAL_Print("Wait %d ms...\n", wait_period);
        HAL_Delay(wait_period);

        HAL_Print("dwm_loc_get(&loc):\n");

        if (dwm_loc_get(&loc) == RV_OK) {
            HAL_Print("\t[X , Y , Z , Q]  -->   [ %d , %d , %d , %u ]\n", loc.p_pos->x, loc.p_pos->y, loc.p_pos->z, loc.p_pos->qf);
            char send_message[100];
            sprintf(send_message, " \t[X , Y , Z , Q]  --> [ %d , %d , %d , %u ]\n", loc.p_pos->x, loc.p_pos->y, loc.p_pos->z, loc.p_pos->qf);
            tcp_connect(send_message);

            for (i = 0; i < loc.anchors.dist.cnt; ++i) {
                HAL_Print("\t%u)", i);
                HAL_Print("0x%llx", loc.anchors.dist.addr[i]);
                HAL_Print("=%u,\n", loc.anchors.dist.dist[i]);
            }
        }
    }

    return 0;
}
