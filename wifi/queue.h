#ifndef _QUEUE_
#define _QUEUE_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <pthread.h>

#define queue_Size 25


typedef struct pico_To_Server_Frame_t{
    bool status;                       //Pi pico status -> 0-Active, 1-inactive
    
    float MPU_X;                       //MPU gyroscope calculated X current coordinate
    float MPU_Y;                       //MPU gyroscope calculated Y current coordinate

    uint32_t GPS_Latitude;             //GPS current Latitude
	uint32_t GPS_Latitude_dec;         //GPS current Latitude fract value
	uint32_t GPS_Latitude_Direction;   //GPS current Latitude Direction
	uint32_t GPS_Longitude;            //GPS current Longitude
	uint32_t GPS_Longitude_dec;		   //GPS current Longitude fract value
	uint32_t GPS_Longitude_Direction;  //GPS current Longitude Direction
 
    bool metal_Detection;              //1 -> metal detected 0 otherwise
    //uint32_t metal_Detection_Counter;//metal Detection Counter 
}pico_To_Server_Frame_t; 

typedef struct server_To_Pico_Frame_t{
    bool status;                       //Server status -> 0-Active, 1-inactive
    uint8_t direction;                 //Vehicle direction command
    int16_t velocity;                  //Vehicle velocity command
}server_To_Pico_Frame_t;

typedef struct pico_To_Server_Queue_t{
    pico_To_Server_Frame_t *pico_To_Server_Data;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    uint32_t head, tail;
    uint32_t size;
}pico_To_Server_Queue_t;


/// @brief queue initialization
/// @param queue queue data pointer
void Queue_init(pico_To_Server_Queue_t *queue);

/// @brief queue init
/// @param queue pointer to queue data structure
void queue_Init(pico_To_Server_Queue_t *queue);

/// @brief queue add data to queue buffer and wait until data will be added 
/// @param queue pointer to queue data structure 
/// @param data data to add 
void queue_Add_Blocking(pico_To_Server_Queue_t *queue, pico_To_Server_Frame_t *data);

/// @brief queue remove data from queue buffer and wait until data will be removed
/// @param queue pointer to queue data structure
/// @param data data place where data will be saved
void queue_Remove_Blocking(pico_To_Server_Queue_t *queue, pico_To_Server_Frame_t *data);

/// @brief SIMPLE queue try add function
/// @param queue 
/// @param data data to add to queue
/// @return true -> data successful added, false otherwise 
bool queue_try_add(pico_To_Server_Queue_t *queue, pico_To_Server_Frame_t *data);

/// @brief SIMPLE queue try remove function
/// @param queue 
/// @param data data to remove from queue
/// @return true -> data successful added, false otherwise 
bool queue_try_remove(pico_To_Server_Queue_t *queue, pico_To_Server_Frame_t *data);

#endif