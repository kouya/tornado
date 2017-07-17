/* Tornado - Two player weather action game
 *
 * Copyright (C) 2000-2002 Oliver Feiler (kiza@gmx.net)
 * 
 * erwin.c - The program's AI
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

#include <stdlib.h>
#include "main.h"
#include "draw.h"


/* Lets the AI choose a weather based on the current house destruction.
 * The idea is to take lightning and tornado which do much "vertical" damage
 * (split the house) when the house is still in good condition. Rain, snow
 * and hail don't do as much damage but are great to "remove" small pieces
 * of the house that are difficult to hit otherwise. */

char erwin_choice(int erwin_destroy_count, int self_destroyed,
		  int cloud_x, int current_player) {
  int select;			/* just an integer that holds a random number */

  select = main_rand(9);
  if (select == 0)
    return 's';

  if (erwin_destroy_count < 45) {
    if (current_player == 2) {
      if (cloud_x <= 15)
	return 'l';
      else
	return 't';
    } else if (current_player == 1) {
      if (cloud_x >= 40)
	return 'l';
      else
	return 't';
    } 
  } else {
    select = main_rand(2);
    switch (select) {
    case 0:
      return 'h';
      break;
    case 1:
      return 'r';
      break;
    }
  }
  return 't';
}

   

/* Return aim based on the selected weather, the current windspeed,
 * the cloud position and the current player. */

int erwin_aim(char input, int windspeed, int cloud_x, int current_player) {
  int aim;							   /* aim the function returns to main */
   
  if (input == 'l')		/* If Lightning is selected, we don't
		                   need aim and can return immediately.*/
    return 0;
  
  if (input == 's') {
    if (current_player == 1) {
      if (windspeed <= 0)
	aim = ((abs(windspeed * 20) + (-3 - cloud_x)) / 20);
      else
	aim = ((-abs(windspeed * 20) + (-3 - cloud_x)) / 20);
    } else {
      if (windspeed <= 0)
	aim = ((abs(windspeed * 20) + (69 - cloud_x)) / 20.0);
      else
	aim = ((-abs(windspeed * 20) + (60 - cloud_x)) / 20.0);
    }
  } else if (current_player == 1) {	/* ...we aim at the right house */
    if (windspeed <= 0)
      aim = ((abs(windspeed * 20) + (69 - cloud_x)) / 20.0);
    else
      aim = ((-abs(windspeed * 20) + (60 - cloud_x)) / 20.0);
  } else {			/*  and now on the left */
    if (windspeed <= 0)
      aim = ((abs(windspeed * 20) + (-3 - cloud_x)) / 20);
    else
      aim = ((-abs(windspeed * 20) + (-3 - cloud_x)) / 20);
  }
   
  return aim; 
}

/* The new improved AI */
int erwin_choice_new(int cloud_x, int current_player) {

  int select;			/* random int */

  count_snow(59, 14, 17, 10);

  /* Choose snow randomly from time to time. */
  select = main_rand(4);
  if (select == 0)
    return 's';

  /* If the cloud is right above the enemy house and it has >75%
     then we strike with lightning to do most damage.
     If the cloud is close to the enemy house and it has >75%
     then we strike with tornado.
     If the cloud is far away from the enemy house
     then we strike with hail.
     If the place where we would like to hit is covered with snow
     then we use rain to get rid of it. */
  if (current_player == 1) {
    if (((count_destroyed(59, 14, 17, 10)-32) < 25) && (cloud_x >= 40))
      return 'l';
    else if (((count_destroyed(59, 14, 17, 10)-32) < 25) && (cloud_x >= 30))
      return 't';
    else if (count_snow(59, 14, 17, 10) > 5)
      return 'r';
    else
      return 'h';
  }

  if (current_player == 2) {
    if (((count_destroyed(5, 14, 17, 10)-32) < 25) && (cloud_x <= 15))
      return 'l';
    else if (((count_destroyed(5, 14, 17, 10)-32) < 25) && (cloud_x <= 25))
      return 't';
    else if (count_snow(5, 14, 17, 10) > 5)
      return 'r';
    else
      return 'h';
  }

  return 't';
}

int erwin_aim_new(char input, int windspeed, int cloud_x, int current_player) {

  int aim;			/* returned aim */

  if (input == 'l')		/* If Lightning is selected, we don't
		                   need aim and can return immediately.*/
    return 0;
  
  if (input == 's') {
    if (current_player == 1) {
      if (windspeed <= 0)
	aim = ((abs(windspeed * 20) + (-3 - cloud_x)) / 20);
      else
	aim = ((-abs(windspeed * 20) + (-3 - cloud_x)) / 20);
    } else {
      if (windspeed <= 0)
	aim = ((abs(windspeed * 20) + (69 - cloud_x)) / 20.0);
      else
	aim = ((-abs(windspeed * 20) + (60 - cloud_x)) / 20.0);
    }
  } else if (current_player == 1) {	/* ...we aim at the right house */
    if (windspeed <= 0)
      aim = ((abs(windspeed * 20) + (69 - cloud_x)) / 20.0);
    else
      aim = ((-abs(windspeed * 20) + (60 - cloud_x)) / 20.0);
  } else {			/*  and now on the left */
    if (windspeed <= 0)
      aim = ((abs(windspeed * 20) + (-3 - cloud_x)) / 20);
    else
      aim = ((-abs(windspeed * 20) + (-3 - cloud_x)) / 20);
  }
   
  return aim; 
}
