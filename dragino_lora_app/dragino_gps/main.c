/**
 * http://szak.hu/linux/
 * serial.c
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "main.h"

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS0"

#define FALSE 0
#define TRUE 1

#define DEBUG_COORDINATES 1

volatile int STOP=FALSE; 

/**
 * Given a string representation of a latitude or longitude value 
 * this function converts it into its decimal equivalent.
 *
 * Sample NMEA string:
 * $GPGGA,142652.000,4723.3816,N,01827.7294,E,2,9,1.27,350.0,M,41.3,M,,*5E
 *
 * Sample input:
 * "7423.3816", "N"
 *
 * @param latLon const char* - the string representation of the latitude or longitude value.
 * @param direction const char* - the direction character (E/N or S/W)
 * @returns double
 */
double convertToDecimalDegrees(const char *latLon, const char *direction)
{
    char deg[4] = {0};
    char *dot, *min;
    int len;
    double dec = 0;

    if ((dot = strchr(latLon, '.')))
    {                                           // decimal point was found
        min = dot - 2;                          // mark the start of minutes 2 chars back
        len = min - latLon;                     // find the length of degrees
        strncpy(deg, latLon, len);              // copy the degree string to allow conversion to float
        dec = atof(deg) + atof(min) / 60;       // convert to float

        if (strcmp(direction, "S") == 0 || strcmp(direction, "W") == 0)
            dec *= -1;
    }
    return dec;
}

/**
 * Given an NMEA string this function separates its tokens split by comma.
 *
 * Hardware: LM80 MT3339 Dragino LoRa GPS HAT
 *
 * Sample NMEA input string:
 * $GPGGA,142652.000,4723.3816,N,01827.7294,E,2,9,1.27,350.0,M,41.3,M,,*5E
 *
 * @param str char*
 */
void parseGGAString(char *str)
{
    const char s[4] = ",";
    char *tok = "\0";
    char *latitude = "\0";
    char *longitude = "\0";
    char *dir_lat = "\0";
    char *dir_lon = "\0";
    unsigned short i = 0;
    unsigned short gpgga = 0;
    
    tok = strtok(str, s);

    while (tok != 0) {
        if (i==0) {
            if (strcmp(tok, "$GPGGA")==0)
            {
                gpgga = 1;
            }
        }
        if ( (gpgga==1) && (i>=2 && i<=5) ) {
            switch (i)
            {
                case 2: latitude = tok; break;
                case 3: dir_lat = tok; break;
                case 4: longitude = tok; break;
                case 5: dir_lon = tok; break;
                default: ;
            }
        }

        tok = strtok(0, s);
        i++;
    }

    if (gpgga==1) {
        ptr_coords->latitude = convertToDecimalDegrees(latitude, dir_lat);
        ptr_coords->longitude = convertToDecimalDegrees(longitude, dir_lon);
        ptr_coords->dir_lat = dir_lat;
        ptr_coords->dir_lon = dir_lon;
#if DEBUG_COORDINATES==1
        printf("\r%f %s, %f %s", ptr_coords->latitude, ptr_coords->dir_lat, ptr_coords->longitude, ptr_coords->dir_lon);
        fflush(stdout);
#endif
    }
}

int main(void)
{
    int fd, res;
    struct termios oldtio, newtio;
    char buf[256];

    ptr_coords = &coordinates;

    /* Megnyitjuk a modemet olvasasra es irasra. A tty kontrollt 
       kikapcsoljuk, hogy a CTRL-C karakterre ne szakadjon meg a 
       program futasa. */
    fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
    if(fd < 0) { perror(MODEMDEVICE); exit(-1); }

    /* Elmentjuk a soros port aktualis beallitasait. */
    tcgetattr(fd, &oldtio);
    /* Inicializaljuk a strukturat az uj beallitasokhoz. */
    bzero(&newtio, sizeof(newtio));

    /* 
      BAUDRATE: a soros port sebessege
      CRTSCTS : a port hardver atvitel szabalyozasa
      CS8     : 8n1 (8 bit, nincs paritas,1 stopbit)
      CLOCAL  : lokalis kapcsolat, nincsenek modem jelek
      CREAD   : a karakterek fogadasanak bekapcsolasa
    */
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;

    /*
      IGNPAR  : a paritas hibas bajtokat figyelmen kivul hagyja
      ICRNL   : a CR karaktert NL karakterre konvertalja
    */
    newtio.c_iflag = IGNPAR;// | ICRNL;

    /* Nyers kimenet beallitasa. */
    newtio.c_oflag = 0;

    /* Engedelyezi a kanonikus bemenetet, kikapcsolja a karakter 
       visszajelzest, es letiltja a szignalok kuldeset. */
    newtio.c_lflag = ICANON;

    /* A kontroll karakterek inicializalasa. */
    newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
    newtio.c_cc[VEOL]     = 0x0A;     /* '\0' */
    newtio.c_cc[VEOL2]    = 0;     /* '\0' */
    newtio.c_cc[VERASE]   = 0;     /* del */
    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
    newtio.c_cc[VKILL]    = 0;     /* @ */
    newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
    newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */

    newtio.c_cc[VTIME]    = 0;     /* karakterek kozti idozito */
    newtio.c_cc[VMIN]     = 1;     /* blokkolja az olvasast, amig 
                                        nem erkezik egy karakter */

    // Toroljuk soros port buffereit es aktivaljuk a beallitasokat. 
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);
      
    while(STOP == FALSE) /* A kilepesig tart a ciklus. */
    {
      res = read(fd, buf, 256); 
      buf[res] = 0;
      // $GPGGA,142652.000,4723.3816,N,01827.7294,E,2,9,1.27,350.0,M,41.3,M,,*5E
      // $          - start character of data
      // GPGGA      - this needs us
      // 142652.000 - UTC time in format of 'hhmmss.sss'
      // 4723.3816  - latitude in format 'ddmm.mmmm'
      // N          - N/S
      // 01827.7294 - longitude in format 'dddmm.mmmm'
      // E          - E/W
      // 2          - fix status [0=invalid, 1=GNSS fix, 2=DGPS fix]
      // 9          - number of satellites used
      // 1.27       - horizontal dilution of precision
      // 350.0      - Altitude in meters according to WGS84 ellipsoid
      // M          - fixed field, meter
      // 41.3       - height of GeoID (mean sea level) above WGS84 ellipsoid
      // M          - fixed field, meter
      //            - DGPS age
      //            - DGPS station id
      // *          - end character of data field
      // 5E         - checksum in hex (XOR all characters between $ and *)
      parseGGAString(buf);
    }
    /* Visszaallitjuk a regi port beallitasokat. */
    tcsetattr(fd, TCSANOW, &oldtio);

    return 0;
}

