#ifndef _WIFI_
#define _WIFI_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <pthread.h>


#define buffer_Size 4096
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
    //uint32_t metal_Detection_Counter;  //metal Detection Counter 
}pico_To_Server_Frame_t; 

typedef struct server_To_Pico_Frame_t{
    bool status;                       //Server status -> 0-Active, 1-inactive
    uint8_t direction;                 //Vehicle direction command
    int16_t velocity;                  //Vehicle velocity command
}server_To_Pico_Frame_t;

typedef struct pico_To_Server_Queue{
    pico_To_Server_Frame_t *pico_To_Server_Data;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    uint32_t head, tail;
    uint32_t size;
}pico_To_Server_Queue;

/// @brief Pico to server transmission initialization
/// @param --
void wifi_Receive_Init(void);

/// @brief server to Pico transmission initialization
/// @param --
void wifi_Send_Init(void);

/// @brief Integer type to ASCII conversion
/// @param data int input data
/// @return converted data(ASCII)
uint32_t INT_To_ASCII(uint32_t data);

/// @brief queue initialization
/// @param queue queue data pointer
void Queue_init(pico_To_Server_Queue *queue);
#endif