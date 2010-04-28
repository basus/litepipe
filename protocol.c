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
        signal(SIGPIPE, time_client_quit);
        struct IncomingData *data;
        data = (struct IncomingData *) idata;
        
        time_t seconds;
        seconds = time(NULL);

        while(1) {
                seconds = time(NULL);
                sendData(&(data->remoteConnection), &seconds, sizeof(time_t));
                sleep(30);
        }
}

void time_client_quit()
{
        printf("Time server client quit\n");
        pthread_exit(NULL);
}

void serve_info(void *idata)
{
        struct IncomingData *data = (struct IncomingData *) idata;
        char *request = (char *) data->data;

        if (strcmp(request, HELP) == 0) 
                serve_file("HELP", "HELP", &(data->remoteConnection));
        else if (strcmp(request, VERSION) == 0)
                serve_proc("/proc/version", &(data->remoteConnection));
        else if (strcmp(request, UPTIME) == 0)
                serve_proc("/proc/uptime", &(data->remoteConnection));
        else if (strcmp(request, FS) == 0)
                serve_proc("/proc/filesystems", &(data->remoteConnection));
        else if (strcmp(request,CPU) == 0)
                serve_proc("/proc/cpuinfo", &(data->remoteConnection));
        else if (strcmp(request,PART) == 0)
                serve_proc("/proc/partitions", &(data->remoteConnection));
        else
                serve_file("HELP", "HELP", &(data->remoteConnection));
}

void serve_http(void *idata)
{
        struct IncomingData *data = (struct IncomingData *) idata;
        const char *request = (const char *) data->data;

        if (fopen(request, "r"))
                if (sanitize_request(request))
                        serve_file(request, request, &(data->remoteConnection));
                else
                        sendData(&(data->remoteConnection), "File access denied", sizeof(char)*19);
        else
                sendData(&(data->remoteConnection), "File not accessible", sizeof(char)*20);
}

int sanitize_request(const char *request)
{
        if ((request[0] == '/') || (request[0] == '.' && request[1] == '.'))
                return 0;

        int depth = 0;
        int index = 0;
        char *dir, req[MAX_FILE_PATH];
        strcpy(req,request);
        dir = strtok(req, "/");

        while(dir != NULL) {
                if (strcmp(dir, ".") == 0);
                else if (strcmp(dir, "..") == 0)
                        depth--;
                else depth++;
                dir = strtok(NULL,"/");
        }

        if(depth < 1)
                return 0;
        else
                return 1;
        
}

void serve_proc(const char *filename, struct RemoteConnection *client)
{
        FILE *fp = fopen(filename, "r");
        FILE *tmp = fopen("/tmp/litepipe-server", "w");
        char buffer[MAX_BUFFER];
        char fnametmp[MAX_FILE_PATH];
        char *infoname;
        size_t readin;

        do {
                readin = fread(buffer, sizeof(char), MAX_BUFFER, fp);
                fwrite(buffer, sizeof(char), readin, tmp);
                fprintf(stderr, "%s\n", buffer);
        } while(readin == MAX_BUFFER);

        fclose(fp);
        fclose(tmp);

        strcpy(fnametmp, filename);
        infoname = strtok(fnametmp, "/");
        infoname = strtok(NULL, "/");
        fprintf(stderr, "Info name is: %s", infoname);

        serve_file("/tmp/litepipe-server", infoname, client);
}

void serve_file(const char *filename, char *fileid, struct RemoteConnection *client)
{
        char *buffer;
        long num_chars, buflen;
        int fname_len;
        FILE *infile = fopen(filename, "r");
        fseek(infile, 0L, SEEK_END);
        num_chars = ftell(infile);
 
/* reset the file position indicator to 
   the beginning of the file */
        fseek(infile, 0L, SEEK_SET);	
 
/* grab sufficient memory for the 
   buffer to hold the text and insert null*/
        fname_len = strlen(filename);
        buflen = num_chars+fname_len+2;
        buffer = (char*)calloc(buflen, sizeof(char));
        buffer[buflen-1] = '\0';
        strcpy(buffer, fileid);        

/* copy all the text into the buffer */
        fread(buffer+fname_len+1, sizeof(char), num_chars, infile);
        fclose(infile);
        fprintf(stderr, "fname is %s\n", buffer);
        fprintf(stderr, "tmp has %s\n", buffer+fname_len+1);

        sendData(client, buffer, buflen);
}
