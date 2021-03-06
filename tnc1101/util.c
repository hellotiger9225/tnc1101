#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include "util.h"

int verbose_level = 0;

// -------------------------------------------------------------------------------------------------
void _verbprintf(int verb_level, const char *fmt, ...)
// -------------------------------------------------------------------------------------------------
{
    int flush = 1;

    if (verb_level > verbose_level)
        return;

    va_list args;
    va_start(args, fmt);

    {
        vfprintf(stderr, fmt, args);
        if(flush)
            fflush(stderr);
    }

    va_end(args);
}

// -------------------------------------------------------------------------------------------------
void _verbprintft(int verb_level, const char *fmt, ...)
// -------------------------------------------------------------------------------------------------
{
    char tbuf[20];
    struct timeval tv;
    time_t curtime;
    int flush = 1;

    if (verb_level > verbose_level)
        return;

    va_list args;
    va_start(args, fmt);

    {
        gettimeofday(&tv, NULL);
        curtime = tv.tv_sec;
        strftime(tbuf, 20, "%T", localtime(&curtime));
        fprintf(stderr, "%s.%06d: ", tbuf, tv.tv_usec);
        vfprintf(stderr, fmt, args);
        if(flush)
            fflush(stderr);
    }

    va_end(args);
}

// -------------------------------------------------------------------------------------------------
void _print_block(int verb_level, const uint8_t *pblock, int size)
// -------------------------------------------------------------------------------------------------
{
    size_t i;
    char   c;
    size_t lsize = 16;
    int    flush = 1;

    if (verb_level > verbose_level)
        return;

    if (size < 0)
        return;

    fprintf(stderr, "     ");

    for (i=0; i<lsize; i++)
    {
        fprintf(stderr, "%X    ", i);
    }

    for (i=0; i<size; i++)
    {
        if (i % 16 == 0)
        {
            fprintf(stderr, "\n%03X: ", i);
        }

        if ((pblock[i] < 0x20) || (pblock[i] >= 0x7F))
        {
            c = '.';
        }
        else
        {
            c = (char) pblock[i];
        }

        fprintf(stderr, "%02X:%c ", pblock[i], c);
    }

    fprintf(stderr, "\n");

    if (flush)
        fflush(stderr);
}

// -------------------------------------------------------------------------------------------------
int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
// -------------------------------------------------------------------------------------------------
{
    // Perform the carry for the later subtraction by updating y. 
    if (x->tv_usec < y->tv_usec) 
    {  
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;  
        y->tv_usec -= 1000000 * nsec;  
        y->tv_sec += nsec;  
    }

    if (x->tv_usec - y->tv_usec > 1000000) 
    {  
        int nsec = (y->tv_usec - x->tv_usec) / 1000000;  
        y->tv_usec += 1000000 * nsec;  
        y->tv_sec -= nsec;  
    }  

    // Compute the time remaining to wait. tv_usec is certainly positive.
    result->tv_sec = x->tv_sec - y->tv_sec;  
    result->tv_usec = x->tv_usec - y->tv_usec;  

    // Return 1 if result is negative. 
    return x->tv_sec < y->tv_sec;  
}

// -------------------------------------------------------------------------------------------------
// Get a timeval timestamp in microseconds
uint32_t ts_us(struct timeval *x)
// -------------------------------------------------------------------------------------------------
{
    return 1000000 * x->tv_sec + x->tv_usec;
}

// ------------------------------------------------------------------------------------------------
// Calculate RSSI in dBm from decimal RSSI read out of RSSI status register
float rssi_dbm(uint8_t rssi_dec)
// ------------------------------------------------------------------------------------------------
{
    if (rssi_dec < 128)
    {
        return (rssi_dec / 2.0) - 74.0;
    }
    else
    {
        return ((rssi_dec - 256) / 2.0) - 74.0;
    }
}

// ------------------------------------------------------------------------------------------------
// Extract CRC and LQI from the CRC+LQI combination byte
uint8_t get_crc_lqi(uint8_t crc_lqi, uint8_t *lqi)
// ------------------------------------------------------------------------------------------------
{
    *lqi = crc_lqi & 0x7F;
    return (crc_lqi & 0x80)>>7;
}