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
                printf("No option specified. Dropping to text interface\n ");
                fflush(stdout);
                cli_interactive();
                return 1;
        }

        if(argc > 1) {
                int server_id;
                server_id = cli(argc, argv);
                waitpid(server_id);
                return 1;
        }
}
