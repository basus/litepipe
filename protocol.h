/***************************************************************************
 *   Copyright (C) 2010 by Shrutarshi Basu   *
 *   basus@lafayette.edu   *
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
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <time.h>
#include <string.h>
#include "communicator.h"

/* Port numbers (also used as protocol identifiers) */
#define TIME 70001
#define INFO 70002
#define HTTP 70003

/* Legal information requests */
#define HELP "help"
#define VERSION "version"
#define UPTIME "uptime"
#define FS "filesystem"
#define CPU "cpuinfo"
#define PART "partitions"

/* Constants */
#define MAX_FILE_PATH 256
#define MAX_BUFFER 1024
#define INTERVAL 1

/* Functions */
void serve_time(void *data);
void serve_info(void *data);
void serve_http(void *data);
void serve_proc(const char *fname, struct RemoteConnection *client);
void serve_file(const char *fname, char *fileid, struct RemoteConnection *client);
void time_client_quit();

#endif
