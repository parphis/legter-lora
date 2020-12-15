#ifndef _MAINH_
#define _MAINH_

typedef struct LM80_MT3339 {
    double longitude;
    double latitude;
    char* dir_lat;
    char* dir_lon;
} gps_type;

gps_type coordinates, *ptr_coords;

typedef struct d {
    int a;
} typ;

double convertToDecimalDegrees(const char *latLon, const char *direction);
void parseGGAString(char *str);

#endif
