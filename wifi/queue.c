#include "queue.h"

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

void set_socket_nonblocking(SOCKET socket)
{
    unsigned long mode = 1;
    ioctlsocket(socket, FIONBIO, &mode);
}