#include "wifi.h"

WSADATA wsa;
SOCKET sockfd;
struct sockaddr_in server_addr, client_addr;
int client_addr_len = sizeof(client_addr);
char buffer[buffer_Size];
pico_To_Server_Frame_t pico_To_Server_Data = {0};
server_To_Pico_Frame_t server_To_Pico_Data = {0};

void wifi_Init(void)
{
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return;
    }
    else
        printf("Winsock initialized.\n");

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0 )) == INVALID_SOCKET) {
        printf("Could not create socket : %d\n", WSAGetLastError());
        return;
    }
    else
        printf("Socket created.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(4444);

    if(bind(sockfd , (struct sockaddr *)&server_addr , sizeof(server_addr)) == SOCKET_ERROR) 
    {
        printf("Bind failed with error code : %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return;
    }
    else
        printf("Bind done. Server is listening on port 4444...\n");

    while(true)
    {
        int bytes_received = recvfrom(sockfd, buffer, buffer_Size, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        
        if(bytes_received == SOCKET_ERROR)
            printf("recvfrom failed with error code : %d\n", WSAGetLastError());
        
        memcpy(&pico_To_Server_Data, buffer, sizeof(pico_To_Server_Frame_t));

        printf("Status: %d\n", pico_To_Server_Data.status);
        printf("MPU_X: %.2f\n", pico_To_Server_Data.MPU_X);
        printf("MPU_Y: %.2f\n", pico_To_Server_Data.MPU_Y);
        printf("GPS_Latitude: %u.%u\n", pico_To_Server_Data.GPS_Latitude, pico_To_Server_Data.GPS_Latitude_dec);
        printf("GPS_Latitude_Direction: %u\n", pico_To_Server_Data.GPS_Latitude_Direction);
        printf("GPS_Longitude: %u.%u\n", pico_To_Server_Data.GPS_Longitude, pico_To_Server_Data.GPS_Longitude_dec);
        printf("GPS_Longitude_Direction: %u\n", pico_To_Server_Data.GPS_Longitude_Direction);
        printf("Metal Detection: %d\n", pico_To_Server_Data.metal_Detection);

        
    }
}




void wifi_Deinit(void)
{
    closesocket(sockfd);
    WSACleanup();
}