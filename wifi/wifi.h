#ifndef _WIFI_
#define _WIFI_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define buffer_Size 4096


typedef struct __attribute__((packed)){
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

typedef struct __attribute__((packed)){
    bool status;                       //Server status -> 0-Active, 1-inactive
    uint8_t direction;                 //Vehicle direction command
    int16_t velocity;                  //Vehicle velocity command
}server_To_Pico_Frame_t;


void wifi_Init(void);

void wifi_Deinit(void);


#endif