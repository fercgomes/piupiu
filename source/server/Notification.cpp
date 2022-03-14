#pragma once
#include <cstdint>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <vector>
#include "ProfileManager.hpp"



int NotificationPush()
{
    time_t timeout = time(NULL);
    vector<string> ip_list;

    while(time(NULL) <= timeout + 60) {
        if (recv_len = recvfrom(ser_sock, recv_data, recv_size, 0, (struct sockaddr *)&cli_info, &cli_size) == SOCKET_ERROR) {
            printf("[WARNING] recv error occured.\n");
            continue;
        }

        if(strcmp(recv_data, "Save IP") == 0) {
            ip_list.push_back(inet_ntoa(cli_info.sin_addr));
            }
        }
    }
}

    