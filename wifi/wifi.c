#include "wifi.h"

//Pico IP detection check 
bool pico_IP_Detected = false;
time_t last_time_received_frame;

//Receive data buffer 
server_To_Pico_Frame_t server_To_Pico_Data = {0};

//Thread and Queue data
pthread_t receive_thread, send_thread;
pico_To_Server_Queue_t pico_To_Server_Queue;

void wifi_Transmission_Init(void)
{
    //Queue Initialization
    queue_Init(&pico_To_Server_Queue);

    int result;

    //Receiving Thread
    result = pthread_create(&receive_thread, NULL, wifi_Receive_Thread, NULL);
    if (result != 0) {
        printf("receive task Error <-> Error code: %d\n", result);
        return;
    }

    //Sending Thread
    result = pthread_create(&send_thread, NULL, wifi_Send_Thread, NULL);
    if (result != 0) {
        printf("send task Error <-> Error code: %d\n", result);
        return;
    }

}

void *wifi_Receive_Thread(void* arg)
{
    WSADATA wsa_Receive;
    SOCKET receive_Socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    char buffer[buffer_Size];
    pico_To_Server_Frame_t pico_To_Server_Data = {0};

    if(WSAStartup(MAKEWORD(2, 2), &wsa_Receive) != 0)
    {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return NULL;
    }
    else
        printf("Winsock initialized.\n");


    if((receive_Socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d\n", WSAGetLastError());
        return NULL;
    }
    else
        printf("Socket created.\n");

    ///Non blocking socket configuration
    set_socket_nonblocking(receive_Socket);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(Pico_Port);

    if(bind(receive_Socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d\n", WSAGetLastError());
        closesocket(receive_Socket);
        WSACleanup();
        return NULL;
    }
    else
        printf("Bind done. Server is listening on port 4444...\n");


    while(true)
    {
        int bytes_received = recvfrom(receive_Socket, buffer, buffer_Size, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        
        if(difftime(time(NULL), last_time_received_frame) > 6)
            pico_IP_Detected = false;
        else
            pico_IP_Detected = true;


        if(bytes_received == SOCKET_ERROR)
            printf("recvfrom failed with error code : %d\n", WSAGetLastError());
        else
        {   
            if (!pico_IP_Detected)
            {
                pico_IP_Detected = true; // Pi Pico IP detected
                printf("Client detected with IP: %s and port: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }

            memcpy(&pico_To_Server_Data, buffer, sizeof(pico_To_Server_Frame_t));   // Copy received data from wifi buffer to structure
            queue_try_add(&pico_To_Server_Queue, &pico_To_Server_Data);
            //WIFI TRANSMISSION CHECK
            //printf("Received data from Pico:\n");
            //printf("Status: %d\n", pico_To_Server_Data.status);
            //printf("MPU_X: %.2f\n", pico_To_Server_Data.MPU_X);
            //printf("MPU_Y: %.2f\n", pico_To_Server_Data.MPU_Y);
            //printf("GPS_Latitude: %u.%u\n", pico_To_Server_Data.GPS_Latitude, pico_To_Server_Data.GPS_Latitude_dec);
            //printf("GPS_Latitude_Direction: %c\n", INT_To_ASCII(pico_To_Server_Data.GPS_Latitude_Direction));
            //printf("GPS_Longitude: %u.%u\n", pico_To_Server_Data.GPS_Longitude, pico_To_Server_Data.GPS_Longitude_dec);
            //printf("GPS_Longitude_Direction: %c\n", INT_To_ASCII(pico_To_Server_Data.GPS_Longitude_Direction));
            //printf("Metal Detection: %d\n", pico_To_Server_Data.metal_Detection);

            memset(&pico_To_Server_Data, 0, sizeof(pico_To_Server_Frame_t));        // Clear buffer
            last_time_received_frame = time(NULL);
        }
        Sleep(155);
    }
}

void *wifi_Send_Thread(void* arg)
{
    WSADATA wsa_Send;
    SOCKET send_Socket;
    struct sockaddr_in pico_addr;
    char buffer[buffer_Size];
    server_To_Pico_Frame_t server_To_Pico_Data = {0};

    if(WSAStartup(MAKEWORD(2, 2), &wsa_Send) != 0)
    {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return NULL;
    }
    else
        printf("Winsock initialized for sending.\n");


    if((send_Socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d\n", WSAGetLastError());
        WSACleanup();
        return NULL;
    }
    else
        printf("Send socket created.\n");


    //Set Pi Pico IP and port
    pico_addr.sin_family = AF_INET;
    pico_addr.sin_addr.s_addr = inet_addr(Pico_Ip_Address);
    pico_addr.sin_port = htons(Pico_Port);


    while (true)
    {
        if (pico_IP_Detected)
        {   
            if (GetAsyncKeyState('W') & 0x8000)
                server_To_Pico_Data.direction = 1;      // Drive forward
            else if (GetAsyncKeyState('A') & 0x8000)
                server_To_Pico_Data.direction = 3;      // Turn left
            else if (GetAsyncKeyState('S') & 0x8000)
                server_To_Pico_Data.direction = 2;      // Drive backward
            else if (GetAsyncKeyState('D') & 0x8000)
                server_To_Pico_Data.direction = 4;      // Turn right
            else
                server_To_Pico_Data.direction = 5;      // STOP

            server_To_Pico_Data.velocity = 250;         // 250 is strongly recommended velocity
            server_To_Pico_Data.status = 0;             // 0-> Server and transmission is OK, 1 otherwise 
            
            memcpy(buffer, &server_To_Pico_Data, sizeof(server_To_Pico_Frame_t));
            int send_result = sendto(send_Socket, buffer, sizeof(server_To_Pico_Frame_t), 0, (struct sockaddr*)&pico_addr, sizeof(pico_addr));
            
            if(send_result == SOCKET_ERROR)
            {
                printf("sendto failed with error code : %d\n", WSAGetLastError());
                break;
            }

            //printf("Data sent to %s, Port: %d\n", Pico_Ip_Address, Pico_Port);
            Sleep(135);
        }
    }
}

uint32_t INT_To_ASCII(uint32_t data)
{
    return data + '0'; 
}

void queue_Init(pico_To_Server_Queue_t *queue)
{
    queue->head = 0;
    queue->tail = 0;
    queue->size = queue_Size;
    queue->pico_To_Server_Data = malloc(sizeof(pico_To_Server_Frame_t) * queue_Size);
    
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
}

void queue_Add_Blocking(pico_To_Server_Queue_t *queue, pico_To_Server_Frame_t *data)
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

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
}

void queue_Remove_Blocking(pico_To_Server_Queue_t *queue, pico_To_Server_Frame_t *data)
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

bool queue_try_add(pico_To_Server_Queue_t *queue, pico_To_Server_Frame_t *data)
{
    bool added = false;
    
    pthread_mutex_lock(&queue->mutex);
    
    if ((queue->tail + 1) % queue->size != queue->head)
    {
        memcpy(&queue->pico_To_Server_Data[queue->tail], data, sizeof(pico_To_Server_Frame_t));
        queue->tail = (queue->tail + 1) % queue->size;
        added = true;
        pthread_cond_signal(&queue->not_empty); 
    }
    
    pthread_mutex_unlock(&queue->mutex);
    return added;
}

bool queue_try_remove(pico_To_Server_Queue_t *queue, pico_To_Server_Frame_t *data)
{
    bool removed = false;
    
    pthread_mutex_lock(&queue->mutex);
   
    if (queue->head != queue->tail)
    {
        memcpy(data, &queue->pico_To_Server_Data[queue->head], sizeof(pico_To_Server_Frame_t));
        queue->head = (queue->head + 1) % queue->size;
        removed = true;
        pthread_cond_signal(&queue->not_full); 
    }
    
    pthread_mutex_unlock(&queue->mutex);
    return removed;
}

void set_socket_nonblocking(SOCKET socket) {
    unsigned long mode = 1;
    ioctlsocket(socket, FIONBIO, &mode);
}