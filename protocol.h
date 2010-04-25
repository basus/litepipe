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
#include "communicator.h"

#define HELP "help\n"
#define VERSION "version\n"
#define UPTIME "uptime\n"
#define FS "filesystem\n"
#define CPU "cpuinfo\n"
#define PART "partitions\n"

void serve_time(void *data);
void serve_info(void *data);
void serve_http(void *data);
void serve_file(char *fname, struct RemoteConnection *client);

#endif
