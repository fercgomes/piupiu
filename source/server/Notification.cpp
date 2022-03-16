#pragma once
#include <cstdint>
#include <ctime>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <vector>
#include "Notification.hpp"
#include "ProfileManager.hpp"

int id_count = 0;
Notification NotificationList;


//Recebe uma lista de perfis
//Retorna uma estrutura de notificacao
Notification FeedNotificationList(Profile* user_profile, std::string message_sent)
{
    //Pick up the timestamp
    time_t now = time(0);

    //This is hipotetical to create a id fetch
    if(id_count == 0)
    {
        NotificationList.id = 0;
        NotificationList.timestamp = now;
        NotificationList.profile = *user_profile;
        NotificationList.message = message_sent;
        NotificationList.sentCount = 0; //Nao sei o que esse valor deveria significar na pratica
        //Aqui vai ter que ter um laco para percorrer a lista de seguidores
        //Pegar o ip de cada um
        //e colocar no vetor targets
        for (Profile x :  user_profile.following)
        {
            NotificationList
        }
    }
    else
    {

    }


    //We need to fetch a profile to mount a row to do a line input
    //Here we not focus in pick up the IP, just to create a notification struct





/*
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
*/
}

    