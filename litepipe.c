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

#include "litepipe.h"

int main(int argc, char *argv[])
{
        if(argc == 1) {
                printf("No option specified.\n Please see the README file\n");
                fflush(stdout);
                return 1;
        }

        if(strcmp(argv[1], "server") == 0) {
                printf("Now starting Litepipe server\n");
                fflush(stdout);
                start_server();
                return 0;
        }
        else if (strcmp(argv[1], "client") == 0) {
                printf("Now starting Litepipe client\n");
                fflush(stdout);
                start_client();
                return 0;
        }
        else {
                printf("Unrecognized option. Please see the README file\n");
                fflush(stdout);
                return 1;
        }
        
}
