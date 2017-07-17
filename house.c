/* Tornado - Two player weather action game
 *
 * Copyright (C) 2000-2002 Oliver Feiler (kiza@gmx.net)
 * 
 * house.c - buildings for the game
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/* generate the house inside the array house[17][10] and return to main.
 * choose the type with the char *type.
 */

// int generate_house(char *type) {
int generate_house(int type) {

  switch (type) {

  case 0:
    /* generate the array, classical house
     *
     *   01234567890123456
     * 0           ___
     * 1           |||
     * 2   /-------'|`-\
     * 3 /'/////////////`\
     * 4 |---------------|
     * 5 |-,_____,--,__,-|
     * 6 |-|__|__|--|++|-|
     * 7 |-|__|__|--|o+|-|
     * 8 |----------|++|-|
     * 9 |__________|__|_|
     */
    
    
    house[10][0] = '_';
    house[11][0] = '_';
    house[12][0] = '_';
    house[10][1] = '|';
    house[11][1] = '|';
    house[12][1] = '|';
    house[2][2]  = '/';
    house[3][2]  = '-';
    house[4][2]  = '-';
    house[5][2]  = '-';
    house[6][2]  = '-';
    house[7][2]  = '-';
    house[8][2]  = '-';
    house[9][2]  = '-';
    house[10][2] = '\'';
    house[11][2] = '|';
    house[12][2] = '`';
    house[13][2] = '\\';
    house[0][3]  = '/';
    house[1][3]  = '\'';
    house[2][3]  = '/';
    house[3][3]  = '/';
    house[4][3]  = '/';
    house[5][3]  = '/';
    house[6][3]  = '/';
    house[7][3]  = '/';
    house[8][3]  = '/';
    house[9][3]  = '/';
    house[10][3] = '/';
    house[11][3] = '/';
    house[12][3] = '/';
    house[13][3] = '/';
    house[14][3] = '/';
    house[15][3] = '`';
    house[16][3] = '\\';
    house[0][4]  = '|';
    house[1][4]  = '-';
    house[2][4]  = '-';
    house[3][4]  = '-';
    house[4][4]  = '-';
    house[5][4]  = '-';
    house[6][4]  = '-';
    house[7][4]  = '-';
    house[8][4]  = '-';
    house[9][4]  = '-';
    house[10][4]  = '-';
    house[11][4]  = '-';
    house[12][4]  = '-';
    house[13][4]  = '-';
    house[14][4]  = '-';
    house[15][4]  = '-';
    house[16][4]  = '|';
    house[0][5]  = '|';
    house[1][5]  = '-';
    house[2][5]  = ',';
    house[3][5]  = '_';
    house[4][5]  = '_';
    house[5][5]  = '_';
    house[6][5]  = '_';
    house[7][5]  = '_';
    house[8][5]  = ',';
    house[9][5]  = '-';
    house[10][5]  = '-';
    house[11][5]  = ',';
    house[12][5]  = '_';
    house[13][5]  = '_';
    house[14][5]  = ',';
    house[15][5]  = '-';
    house[16][5]  = '|';
    house[0][6]  = '|';
    house[1][6]  = '-';
    house[2][6]  = '|';
    house[3][6]  = '_';
    house[4][6]  = '_';
    house[5][6]  = '|';
    house[6][6]  = '_';
    house[7][6]  = '_';
    house[8][6]  = '|';
    house[9][6]  = '-';
    house[10][6]  = '-';
    house[11][6]  = '|';
    house[12][6]  = '+';
    house[13][6]  = '+';
    house[14][6]  = '|';
    house[15][6]  = '-';
    house[16][6]  = '|';
    house[0][7]  = '|';
    house[1][7]  = '-';
    house[2][7]  = '|';
    house[3][7]  = '_';
    house[4][7]  = '_';
    house[5][7]  = '|';
    house[6][7]  = '_';
    house[7][7]  = '_';
    house[8][7]  = '|';
    house[9][7]  = '-';
    house[10][7]  = '-';
    house[11][7]  = '|';
    house[12][7]  = 'o';
    house[13][7]  = '+';
    house[14][7]  = '|';
    house[15][7]  = '-';
    house[16][7]  = '|';
    house[0][8]  = '|';
    house[1][8]  = '-';
    house[2][8]  = '-';
    house[3][8]  = '-';
    house[4][8]  = '-';
    house[5][8]  = '-';
    house[6][8]  = '-';
    house[7][8]  = '-';
    house[8][8]  = '-';
    house[9][8]  = '-';
    house[10][8]  = '-';
    house[11][8]  = '|';
    house[12][8]  = '+';
    house[13][8]  = '+';
    house[14][8]  = '|';
    house[15][8]  = '-';
    house[16][8]  = '|';
    house[0][9]  = '|';
    house[1][9]  = '-';
    house[2][9]  = '|';
    house[3][9]  = '_';
    house[4][9]  = '_';
    house[5][9]  = '|';
    house[6][9]  = '_';
    house[7][9]  = '_';
    house[8][9]  = '|';
    house[9][9]  = '-';
    house[10][9]  = '-';
    house[11][9]  = '|';
    house[12][9]  = '+';
    house[13][9]  = '+';
    house[14][9]  = '|';
    house[15][9]  = '-';
    house[16][9]  = '|';
    break;
  }
}

