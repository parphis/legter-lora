#pragma once
#ifndef _LM80_MT3339H_
#define _LM80_MT3339H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

#define GPS_MAX_WAIT 10

typedef struct LM80_MT3339 {
    double longitude = 0.0;
    double latitude = 0.0;
    char* dir_lat = (char*)calloc(1024, sizeof(char));
    char* dir_lon  = (char*)calloc(1024, sizeof(char));
} gps_type;

int openPort();
void closePort(int fd);
double convertToDecimalDegrees(const char *latLon, const char *direction);
void parseGGAString(char *str, gps_type *ptr_coords);

#endif
