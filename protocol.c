/***************************************************************************
 *   Copyright (C) 2010 by Shrutarshi Basu                                 *
 *   basus@lafayette.edu                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "protocol.h"

void serve_time(void *idata)
{
        struct IncomingData *data;
        data = (struct IncomingData *) idata;
        
        time_t seconds;
        seconds = time(NULL);

        sendData(&(data->remoteConnection), &seconds, sizeof(time_t));
}

void serve_info(void *idata)
{
        struct IncomingData *data = (struct IncomingData *) idata;
        char *request = (char *) data->data;
        FILE *fp;

        if (strcmp(request, HELP) == 0) 
                serve_file("HELP", &(data->remoteConnection));
        else if (strcmp(request, VERSION) == 0)
                serve_file("/proc/version", &(data->remoteConnection));
        else if (strcmp(request, UPTIME) == 0)
                serve_file("/proc/uptime", &(data->remoteConnection));
        else if (strcmp(request, FS) == 0)
                serve_file("/proc/filesystems", &(data->remoteConnection));
        else if (strcmp(request,CPU) == 0)
                serve_file("/proc/cpuinfo", &(data->remoteConnection));
        else if (strcmp(request,PART) == 0)
                serve_file("/proc/partitions", &(data->remoteConnection));
        else
                serve_file("HELP", &(data->remoteConnection));
}

void serve_http(void *idata)
{
        
}

void serve_file(char *filename, struct RemoteConnection *client)
{
        char *buffer;
        long bytes;
        FILE *infile = fopen(filename, "r");
        fseek(infile, 0L, SEEK_END);
        bytes = ftell(infile);
 
/* reset the file position indicator to 
   the beginning of the file */
        fseek(infile, 0L, SEEK_SET);	
 
/* grab sufficient memory for the 
   buffer to hold the text */
        buffer = (char*)calloc(bytes, sizeof(char));	
 
/* copy all the text into the buffer */
        fread(buffer, sizeof(char), bytes, infile);
        fclose(infile);

        sendData(client, buffer, bytes);
}
