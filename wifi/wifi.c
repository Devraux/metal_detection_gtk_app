#include "wifi.h"

WSADATA wsa_Receive, wsa_Send;
SOCKET receive_Socket, send_Socket;
struct sockaddr_in server_addr, client_addr;
int client_addr_len = sizeof(client_addr);
char buffer[buffer_Size];
pico_To_Server_Frame_t pico_To_Server_Data = {0};
server_To_Pico_Frame_t server_To_Pico_Data = {0};
bool pico_IP_Detected = false;


void wifi_Receive_Init(void)
{
    if (WSAStartup(MAKEWORD(2, 2), &wsa_Receive) != 0)
    {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return;
    }
    else
        printf("Winsock initialized.\n");

    if ((receive_Socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d\n", WSAGetLastError());
        return;
    }
    else
        printf("Socket created.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(4444);

    if (bind(receive_Socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d\n", WSAGetLastError());
        closesocket(receive_Socket);
        WSACleanup();
        return;
    }
    else
        printf("Bind done. Server is listening on port 4444...\n");

    while(true)
    {
        int bytes_received = recvfrom(receive_Socket, buffer, buffer_Size, 0, (struct sockaddr*)&client_addr, &client_addr_len);

        if(bytes_received == SOCKET_ERROR)
        {
            printf("recvfrom failed with error code : %d\n", WSAGetLastError());
        }
        else
        {
            memcpy(&pico_To_Server_Data, buffer, sizeof(pico_To_Server_Frame_t));

            if (!pico_IP_Detected)
            {
                pico_IP_Detected = true;
                printf("Client detected with IP: %s and port: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }

            /* WIFI TRANSMISSION CHECK
            printf("Received data from Pico:\n");
            printf("Status: %d\n", pico_To_Server_Data.status);
            printf("MPU_X: %.2f\n", pico_To_Server_Data.MPU_X);
            printf("MPU_Y: %.2f\n", pico_To_Server_Data.MPU_Y);
            printf("GPS_Latitude: %u.%u\n", pico_To_Server_Data.GPS_Latitude, pico_To_Server_Data.GPS_Latitude_dec);
            printf("GPS_Latitude_Direction: %c\n", int_To_ASCII(pico_To_Server_Data.GPS_Latitude_Direction));
            printf("GPS_Longitude: %u.%u\n", pico_To_Server_Data.GPS_Longitude, pico_To_Server_Data.GPS_Longitude_dec);
            printf("GPS_Longitude_Direction: %c\n", pico_To_Server_Data.GPS_Longitude_Direction);
            printf("Metal Detection: %d\n", int_To_ASCII(pico_To_Server_Data.metal_Detection));
            */
        }
    }
}

void wifi_Send_Init(void)
{
    if (WSAStartup(MAKEWORD(2, 2), &wsa_Send) != 0)
    {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return;
    }
    else
        printf("Winsock initialized for sending.\n");

    if ((send_Socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }
    else
        printf("Send socket created.\n");

    while(true)
    {
        if(pico_IP_Detected)
        {
            if(GetAsyncKeyState('w') && 0x8000)
                server_To_Pico_Data.direction = 1; //Drive forward

            else if(GetAsyncKeyState('a') && 0x8000)
                server_To_Pico_Data.direction = 3; //Turn left

            else if(GetAsyncKeyState('s') && 0x8000)
                server_To_Pico_Data.direction = 2; //Drive backward

            else if(GetAsyncKeyState('d') && 0x8000)
                server_To_Pico_Data.direction = 4; //Turn right

            else
                server_To_Pico_Data.direction = 5; // STOP

            server_To_Pico_Data.velocity = 250;
            server_To_Pico_Data.status = 0; // 0-> Server and transmission is OK, 1 otherwise 


            memcpy(buffer, &server_To_Pico_Data, sizeof(server_To_Pico_Frame_t));

            int send_result = sendto(send_Socket, buffer, sizeof(server_To_Pico_Frame_t), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
            if (send_result == SOCKET_ERROR)
            {
                printf("sendto failed with error code : %d\n", WSAGetLastError());
                break;
            }

            printf("Data sent to %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            Sleep(175);
        }
    }
}

uint32_t INT_To_ASCII(uint32_t data)
{
    return data + '0'; 
}

void queue_Init(pico_To_Server_Queue *queue)
{
    queue->head = 0;
    queue->tail = 0;
    queue->size = queue_Size;
    queue->pico_To_Server_Data = malloc(sizeof(pico_To_Server_Frame_t) * queue_Size);
    
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
}

void queue_Add_Blocking(pico_To_Server_Queue *queue, pico_To_Server_Frame_t *data)
{
    pthread_mutex_lock(&queue->mutex);

    while ((queue->tail + 1) % queue->size == queue->head)
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    
    queue->pico_To_Server_Data[queue->tail].GPS_Latitude             = data->GPS_Latitude;
    queue->pico_To_Server_Data[queue->tail].GPS_Latitude_dec         = data->GPS_Latitude_dec;
    queue->pico_To_Server_Data[queue->tail].GPS_Latitude_Direction   = data->GPS_Latitude_Direction;
    queue->pico_To_Server_Data[queue->tail].GPS_Longitude            = data->GPS_Longitude;
    queue->pico_To_Server_Data[queue->tail].GPS_Longitude_dec        = data->GPS_Longitude_dec;
    queue->pico_To_Server_Data[queue->tail].GPS_Longitude_Direction  = data->GPS_Longitude_Direction;

    queue->tail = (queue->tail + 1) % queue->size;

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
}

void queue_Remove_Blocking(pico_To_Server_Queue *queue, pico_To_Server_Frame_t *data)
{
    pthread_mutex_lock(&queue->mutex);

    while (queue->head == queue->tail) 
        pthread_cond_wait(&queue->not_empty, &queue->mutex);

    data->GPS_Latitude            = queue->pico_To_Server_Data[queue->head].GPS_Latitude;
    data->GPS_Latitude_dec        = queue->pico_To_Server_Data[queue->head].GPS_Latitude_dec;
    data->GPS_Latitude_Direction  = queue->pico_To_Server_Data[queue->head].GPS_Latitude_Direction;
    data->GPS_Longitude           = queue->pico_To_Server_Data[queue->head].GPS_Longitude;
    data->GPS_Longitude_dec       = queue->pico_To_Server_Data[queue->head].GPS_Longitude_dec;
    data->GPS_Longitude_Direction = queue->pico_To_Server_Data[queue->head].GPS_Longitude_Direction;

    queue->head = (queue->head + 1) % queue->size;

    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);
}