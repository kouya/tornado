/* Tornado - Two player weather action game
 *
 * Copyright (C) 2000-2002  Oliver Feiler (kiza@gmx.net)
 * 
 * draw.c
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


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <errno.h>

#ifdef LOCALEPATH
#  include <libintl.h>
#  include <locale.h>
#endif

#include "version.h"
#include "draw.h"
#include "scores.h"

#ifdef LOCALEPATH
#  define _(String) gettext (String)
#else
#  define _(String) (String)
#endif
//#define _(String) gettext (String)


/* draw the statusline */
void draw_statusline(char *text) { 
	attron(WA_BOLD);
	
	mvaddstr(24, 0, text);
	refresh();
	
	attroff(WA_BOLD);
}

/* count the amount destroyed */
int count_destroyed(int x, int y, int width, int height) {

  int count = 0;
  int i, j;
  chtype tmp;

  for (i = 0; i < height; ++i) {
    for (j = 0; j < width; ++j) {
      tmp = mvinch(y+i, x+j);
      //      if ( ((tmp & A_CHARTEXT) == 32) || ((tmp & A_CHARTEXT) == 42) ) {
      if ((tmp & A_CHARTEXT) == 32) {
	  ++count;
      }
    }
  }
   
  return(count);

}

/* count the amount of snow on ones house */
int count_snow(int x, int y, int width, int height) {

  int count = 0;
  int i, j;
  chtype tmp;

  for (i = 0; i < height; ++i) {
    for (j = 0; j < width; ++j) {
      tmp = mvinch(y+i, x+j);
      if ((tmp & A_CHARTEXT) == 42) {
	++count;
      }
    }
  }

  //  printf("Snowcount: %d\n", count);
  return(count);
}

/* Draws the house at screen positions x, y.
 * (17x10 pixels)
 */

void draw_house(int x, int y) {
	attron(WA_BOLD);
	attron(COLOR_PAIR(3));
	mvaddstr(y, x+10,          "___");
	mvaddstr(y+1, x+10,        "|||");
	mvaddstr(y+2, x+2, "/-------'|`-\\");
	mvaddstr(y+3, x, "/'/////////////`\\");
	attroff(COLOR_PAIR(3));
	
	if (x == 5) {
		/* Left house. */
		attron(COLOR_PAIR(2));
	} else {
		/* Right house. */
		attron(COLOR_PAIR(5));
	}

	mvaddstr(y+4, x, "|---------------|");
	mvaddstr(y+5, x, "|-,_____,--,__,-|");
	mvaddstr(y+6, x, "|-|__|__|--|++|-|");
	mvaddstr(y+7, x, "|-|__|__|--|o+|-|");
	mvaddstr(y+8, x, "|----------|++|-|");
	mvaddstr(y+9, x, "|__________|__|_|");
	
	if (x == 5) {
		/* Left house. */
		attroff(COLOR_PAIR(2));
		//attron(WA_BOLD);
	} else {
		/* Right house. */
		attroff(COLOR_PAIR(5));
	}
	
	attroff(WA_BOLD);
	
	/* Overwrite some parts with coloured output. */
	attron(COLOR_PAIR(2));
	mvaddstr(y+5, x+2, ",_____,");
	mvaddstr(y+5, x+11, ",__,");
	mvaddstr(y+6, x+2, "|__|__|");
	mvaddstr(y+6, x+11, "|++|");
	mvaddstr(y+7, x+2, "|__|__|");
	mvaddstr(y+7, x+11, "|");
	mvaddstr(y+7, x+13, "+|");
	mvaddstr(y+8, x+11, "|++|");
	mvaddstr(y+9, x+11, "|__|");
	attroff(COLOR_PAIR(2));
	
	mvaddstr(y+7, x+12, "o");

	refresh();
}


/* Draws the cloud at x, y. */

void draw_cloud(int x, int y) {
	attron(WA_BOLD);

#ifdef BETA_CLOUD
   
	mvaddstr(y, x,   "|                           |");
	mvaddstr(y+1, x, "|      *** CLOUD ***        |");
	mvaddstr(y+2, x, "|                           |");
	mvaddstr(y+3, x, "+---------------------------+");
	refresh();
   
#else
   
	mvaddstr(y, x,   ",/+++*++***+****++++++***+\\, ");
	mvaddstr(y+1, x, "(*+++*****+++++***+++****++) ");
	mvaddstr(y+2, x, "`\\++******+*++*++*****+++*/' ");
	mvaddstr(y+3, x, " `°*,.,*°'`°*,.,*°`°*,.,*°'  ");
	refresh();

#endif

	attroff(WA_BOLD);

}

/*
 * Next generation weather function
 */

int draw_weather(int x, int shift, char object, int strength) {
  struct point {
    int startx, starty;
    int x, y;
    char object;
    int hits;
  } mypoints[13];
   
  chtype tmp;
  int i, num_moved, num_destroyed;
  char objectstr[2];
  
	if (object == '/')
  		attron(COLOR_PAIR(6));
	else if (object == ':')
		attron(COLOR_PAIR(4));

  num_destroyed = 0;
   
  /* generate starting positions and chars for each object */
   
  for (i = 0; i < 13; ++i) {
    mypoints[i].y = mypoints[i].starty = 4;
    mypoints[i].x = mypoints[i].startx = x + i;
	  
    if (i % 3 == 0) /* draw an object only every three columns */
      mypoints[i].object = object;
    else 
      mypoints[i].object = ' ';
	  
    mypoints[i].hits = 0;
  }
   
  num_moved = 1;
  while (num_moved > 0) {
      
    num_moved = 0;
	  
    /* move each character */
    for (i = 0; i < 13; ++i) {

      /* skip if this object has disappeared (hits >= strength) */
      if (mypoints[i].hits >= strength)
	continue;
		 
      ++num_moved;
		 
      /* did we hit something? */
      if (count_destroyed(mypoints[i].x, mypoints[i].y, 1, 1) == 1) {
	/* no hit */

	/* construct the object string (one char + \0) to use with mvaddstr */
	sprintf(objectstr, "%c", mypoints[i].object);

	/* draw the object */
	mvaddstr(mypoints[i].y, mypoints[i].x, objectstr);
	refresh();
      } else {
	/* we've hit something */
	++mypoints[i].hits;

	/* construct the object string (one char + \0) to use with mvaddstr */
	sprintf(objectstr, "%c", object); /* always use object */

	/* if object==hail and we've hit snow, don't delete */
	tmp = mvinch(mypoints[i].y, mypoints[i].x);
	if ( !(((tmp & A_CHARTEXT) == '*') && (object == ':')) )  {
	  /* draw the object */
	  mvaddstr(mypoints[i].y, mypoints[i].x, objectstr);
	  ++num_destroyed;
	} else {
	  /* "expire" the object and move it back one step so we
	   * don't overwrite it later when deleting. */
	  mypoints[i].hits = strength;
	  --mypoints[i].y;
	  mypoints[i].x -= shift;
	}
			
	refresh();
      }

      if ((mypoints[i].y >= 23) || (mypoints[i].x >= 80)
	  || (mypoints[i].x <= 0)) {
	/* bottom, left, or right border */
	mypoints[i].hits = strength;
      }
		 
      /* move the object */
      ++mypoints[i].y;
      mypoints[i].x += shift;
		 
    }
	  
    usleep(FALLING_SPEED);
      
  }
   
   
  sleep(1);
   
  /* delete the rain/hail/whatever */
   
  sprintf(objectstr, "%c", ' ');

  num_moved = 1;
  while (num_moved > 0) {
	  
    num_moved = 0;
	  
    for (i = 0; i < 13; ++i) {
      if (mypoints[i].starty >= mypoints[i].y)
	continue;
		 
      mvaddstr(mypoints[i].starty, mypoints[i].startx, objectstr);
		 
      ++mypoints[i].starty;
      mypoints[i].startx += shift;
		 
      ++num_moved;
    }

    refresh();
    usleep(DELETE_SPEED);
	  
  }
  
	if (object == '/')
  		attroff(COLOR_PAIR(6));
	else if (object == ':')
		attroff(COLOR_PAIR(4));
   
  return num_destroyed;
}

/* 
 * Kianga's Next Generation Snow Machine
 * Includes Special SnowLogic(tm)
 */

int draw_snow(int x, int shift, char object) {
  struct point {
    int startx, starty;
    int x, y;
    char object;
    int hits;
  } mypoints[13];
   
  int i, num_moved;
  char objectstr[2];
  
  attron(WA_BOLD);

  /* generate starting positions and chars for each object */
   
  for (i = 0; i < 13; ++i) {
    mypoints[i].y = mypoints[i].starty = 4;
    mypoints[i].x = mypoints[i].startx = x + i;
	  
    if (i % 3 == 0) /* draw an object only every three columns */
      mypoints[i].object = object;
    else 
      mypoints[i].object = ' ';
	  
    mypoints[i].hits = 0;
  }
   
  num_moved = 1;
  while (num_moved > 0) {
      
    num_moved = 0;
	  
    /* move each character */
    for (i = 0; i < 13; ++i) {

      /* skip if this object has disappeared (hits >= strength) */
      if (mypoints[i].hits >= 1)
	continue;
		 
      ++num_moved;
		 
      /* did we hit something? */
      if (count_destroyed(mypoints[i].x, mypoints[i].y, 1, 1) == 1) {
	/* no hit */

	/* construct the object string (one char + \0) to use with mvaddstr */
	sprintf(objectstr, "%c", mypoints[i].object);

	/* draw the object */
	mvaddstr(mypoints[i].y, mypoints[i].x, objectstr);
	refresh();
      } else {
	/* we've hit something */
	++mypoints[i].hits;

	/* construct the object string (one char + \0) to use with mvaddstr */
	sprintf(objectstr, "%c", object); /* always use object */

	/* now delete the part at (y-1,x-shift) */
	mvaddstr(mypoints[i].y-1, mypoints[i].x-shift, objectstr);
	refresh();

	continue;
      }

      if (mypoints[i].y == 23) {
	/* bottom */

	/* construct the object string (one char + \0) to use with mvaddstr */
	sprintf(objectstr, "%c", object); /* always use object */
			
	mvaddstr(mypoints[i].y, mypoints[i].x, objectstr);
	refresh();
	mypoints[i].hits = 1;
			
	/* stupid correction:
	 * 
	 * we need this in order for deletion to work properly - this has
	 * got to do with the delete line above (y-1,x-shift).
	 */
	++mypoints[i].y;
			
      }
		 
      /* move the object */
      ++mypoints[i].y;
      mypoints[i].x += shift;
		 
    }
	  
    usleep(FALLING_SPEED);
      
  }
   
   
  sleep(1);
   
  /* delete the snow */
   
  sprintf(objectstr, "%c", ' ');

  num_moved = 1;
  while (num_moved > 0) {
	  
    num_moved = 0;
	  
    for (i = 0; i < 13; ++i) {
      if (mypoints[i].starty >= mypoints[i].y-2)
	continue;
		 
      mvaddstr(mypoints[i].starty, mypoints[i].startx, objectstr);
		 
      ++mypoints[i].starty;
      mypoints[i].startx += shift;
		 
      ++num_moved;
    }

    refresh();
    usleep(DELETE_SPEED);
	  
  }
  
  attroff(WA_BOLD);
  return 0;
}

/* Draw a lightning at position x from the cloud */

int draw_lightning(int x, int player) {
	const int y = 4;
	int num_destroyed = 0;

	if (player == 1)
		num_destroyed = count_destroyed(59, 14, 17, 10) - 32;
	else if (player == 2)
		num_destroyed = count_destroyed(5, 14, 17, 10) - 32;
   
	attron(COLOR_PAIR(4));
	/* draw it... */
	flash();
	mvaddstr(y, x+6, "||");
	mvaddstr(y+1, x+6, "||");
	mvaddstr(y+2, x+5, "//");
	mvaddstr(y+3, x+4, "//\\\\");
	mvaddstr(y+4, x+3, "//"); mvaddstr(y+4, x+7, "\\\\");
	mvaddstr(y+5, x+3, "||"); mvaddstr(y+5, x+8, "\\\\");
	mvaddstr(y+6, x+9, "\\\\");
	mvaddstr(y+7, x+8, "//\\\\");
	mvaddstr(y+8, x+7, "//"); mvaddstr(y+8, x+11, "\\\\");
	mvaddstr(y+9, x+7, "||");
	refresh();
	usleep(250000);
	mvaddstr(y+10, x+6, "//\\\\");
	mvaddstr(y+11, x+5, "//"); mvaddstr(y+11, x+9, "\\\\");
	mvaddstr(y+12, x+4, "//"); mvaddstr(y+12, x+10, "\\\\");
	mvaddstr(y+13, x+10, "||");
	mvaddstr(y+14, x+9, "//\\\\");
	mvaddstr(y+15, x+8, "//"); mvaddstr(y+15, x+12, "\\\\");
	mvaddstr(y+16, x+7, "//");
	mvaddstr(y+17, x+7, "\\\\");
	mvaddstr(y+18, x+8, "\\\\");
	mvaddstr(y+19, x+9, "\\\\");
	refresh();
	sleep(1);

	/* ...and delete it again */
	mvaddstr(y, x+6, "  ");
	mvaddstr(y+1, x+6, "  ");
	mvaddstr(y+2, x+5, "  ");
	mvaddstr(y+3, x+4, "    ");
	mvaddstr(y+4, x+3, "  "); mvaddstr(y+4, x+7, "  ");
	mvaddstr(y+5, x+3, "  "); mvaddstr(y+5, x+8, "  ");
	mvaddstr(y+6, x+9, "  ");
	mvaddstr(y+7, x+8, "    ");
	mvaddstr(y+8, x+7, "  "); mvaddstr(y+8, x+11, "  ");
	mvaddstr(y+9, x+7, "  ");
	mvaddstr(y+10, x+6, "    ");
	mvaddstr(y+11, x+5, "  "); mvaddstr(y+11, x+9, "  ");
	mvaddstr(y+12, x+4, "  "); mvaddstr(y+12, x+10, "  ");
	mvaddstr(y+13, x+10, "  ");
	mvaddstr(y+14, x+9, "    ");
	mvaddstr(y+15, x+8, "  "); mvaddstr(y+15, x+12, "  ");
	mvaddstr(y+16, x+7, "  ");
	mvaddstr(y+17, x+7, "  ");
	mvaddstr(y+18, x+8, "  ");
	mvaddstr(y+19, x+9, "  ");
	refresh();   
 	attroff(COLOR_PAIR(4));
   
	if (player == 1)
		num_destroyed = (count_destroyed(59, 14, 17, 10) - 32) - num_destroyed;
	else if (player == 2)
		num_destroyed = (count_destroyed(5, 14, 17, 10) - 32) - num_destroyed;
   
	return num_destroyed;
}

/* Draw Tornado */
int draw_tornado(int x, int interval, int shift, int player) {
  const int y = 3;
  int tmp = 0;
  int shift_right = 0;
  int tmp_x, tmp_y;
  int width, width_tmp;
  int num_destroyed = 0;
  tmp_x = x;
  tmp_y = y;
  
  attron(COLOR_PAIR(2));

  if (player == 1)
    num_destroyed = count_destroyed(59, 14, 17, 10) - 32;
  else if (player == 2)
    num_destroyed = count_destroyed(5, 14, 17, 10) - 32;
   
   
  width = 29;
   
  while(tmp_y < 24) {

    if (width > 3) {
      width -= 2;
      shift_right += 1;
    }
	  
    if (tmp_y == 18) width = 2;
    if (tmp_y == 21) width = 1;
    if (tmp_y == 23) { width = 7; shift_right -= 3; }
	  
    for (width_tmp = 0; width_tmp < width; ++width_tmp)
      mvaddstr(tmp_y, tmp_x+shift_right+width_tmp, "#");
	  
    refresh();
    ++tmp_y;
    ++tmp;
    if (tmp == interval) {
      tmp_x += shift;
      tmp = 0;
    }
    usleep(FALLING_SPEED);

  }

  sleep(1);
   
  tmp_x = x;
  tmp_y = y;
  shift_right = 0;

  width = 29;
   
  while(tmp_y < 24) {
	   
    if (width > 3) {
      width -= 2;
      shift_right += 1;
    }
	  
    if (tmp_y == 18) width = 2;
    if (tmp_y == 21) width = 1;
    if (tmp_y == 23) { width = 7; shift_right -= 3; }
	  
    for (width_tmp = 0; width_tmp < width; ++width_tmp)
      mvaddstr(tmp_y, tmp_x+shift_right+width_tmp, " ");
	  
    refresh();
    ++tmp_y;
    ++tmp;
    if (tmp == interval) {
      tmp_x += shift;
      tmp = 0;
    }
    usleep(DELETE_SPEED);
	  
  }
   
  if (player == 1)
    num_destroyed = (count_destroyed(59, 14, 17, 10) - 32) - num_destroyed;
  else if (player == 2)
    num_destroyed = (count_destroyed(5, 14, 17, 10) - 32) - num_destroyed;
	
  attroff(COLOR_PAIR(2));
   
  return num_destroyed;
}

/* Replace the normal cloud with a bigger one. */

void draw_tornado_cloud(void) {
  int i;

  attron(COLOR_PAIR(2));
  for (i = 0; i < 80; ++i) {
    mvaddstr(0, i, (rand() % 2 == 0) ? "+" : "*");
    mvaddstr(1, i, (rand() % 2 == 0) ? "+" : "*");
    mvaddstr(2, i, (rand() % 2 == 0) ? "+" : "*");
  }
  
  attroff(COLOR_PAIR(2));
  
  move(3,0);
  clrtoeol();
  refresh();   
}

/* Clear the sky. */
void draw_clear_sky(void) { 
  int i;
   
  for (i = 0; i < 80; ++i) {   
    mvaddstr(0, i, " ");
    mvaddstr(1, i, " ");
    mvaddstr(2, i, " ");
    mvaddstr(3, i, " ");
  }
  refresh();
}

/* Draw the main user interface. */
	char draw_user_interface(void) {
	char ch;

	mvaddstr(6, 22, ",___________________________________,");
	mvaddstr(7, 22, "|                                   |");
	attron(COLOR_PAIR(5));
	mvaddstr(7, 23, "              Tornado ");
	attroff(COLOR_PAIR(5));
	attron(WA_BOLD);
	mvaddstr(8, 22, _("|  s = Snow               r = Rain  |"));
	mvaddstr(9, 22, _("|  h = Hail          l = Lightning  |"));
	mvaddstr(10,22, _("|  t = Tornado  c = Current Scores  |"));
	mvaddstr(11,22, _("|  q = Quit         o = Highscores  |"));
	mvaddstr(12,22, "+-----------------------------------+");
	attroff(WA_BOLD);
	attron(COLOR_PAIR(6));
	mvaddstr(6, 22, ",___________________________________,");
	mvaddstr(7, 22, "|");
	mvaddstr(7, 58, "|");
	mvaddstr(8, 22, "|");
	mvaddstr(8, 58, "|");
	mvaddstr(9, 22, "|");
	mvaddstr(9, 58, "|");
	mvaddstr(10,22, "|");
	mvaddstr(10,58, "|");
	mvaddstr(11,22, "|");
	mvaddstr(11,58, "|");
	mvaddstr(12,22, "+-----------------------------------+");
	attroff(COLOR_PAIR(6));
	refresh();
	ch = getch();

	mvaddstr(6, 22, "                                     ");   
	mvaddstr(7, 22, "                                     ");   
	mvaddstr(8, 22, "                                     ");   
	mvaddstr(9, 22, "                                     ");   
	mvaddstr(10,22, "                                     ");   
	mvaddstr(11,22, "                                     ");   
	mvaddstr(12,22, "                                     ");   
   
	refresh();
	return ch;
}


/* Draw a multi purpose dialog box. Display "char *question" in it
 * and get "char *answer". */

void draw_dialog(char *question, char *answer, int length) {
   
	attron (COLOR_PAIR(6));
	mvaddstr(6, 22, ",___________________________________,");
	mvaddstr(7, 22, "|                                   |");
	mvaddstr(8, 22, "|                                   |");
	mvaddstr(9, 22, "|                                   |");
	mvaddstr(10,22, "|                                   |");
	mvaddstr(11,22, "+-----------------------------------+");
	attroff(COLOR_PAIR(6));
	
	attron(COLOR_PAIR(5));
	mvaddstr(8, 24, question);
	attroff(COLOR_PAIR(5));
	refresh();
	echo();
	curs_set(1);
	move(9, 24);
	getnstr(answer, length);
	curs_set(0);
	noecho();
   
	mvaddstr(6, 22, "                                     ");   
	mvaddstr(7, 22, "                                     ");   
	mvaddstr(8, 22, "                                     ");   
	mvaddstr(9, 22, "                                     ");   
	mvaddstr(10,22, "                                     ");   
	mvaddstr(11,22, "                                     ");   
	refresh();   
}


/* Draw a multi purpose dialog box and return a single char. */

char draw_dialog_ch(char *string1, char *string2, char *string3, char *string4) {
  char tmp;
   
  attron(COLOR_PAIR(6));
  mvaddstr(6, 22, ",___________________________________,");
  mvaddstr(7, 22, "|                                   |");
  mvaddstr(8, 22, "|                                   |");
  mvaddstr(9, 22, "|                                   |");
  mvaddstr(10,22, "|                                   |");
  mvaddstr(11,22, "+-----------------------------------+");
  attroff(COLOR_PAIR(6));

  attron(WA_BOLD);
  mvaddstr(7, 24, string1);
  mvaddstr(8, 24, string2);
  mvaddstr(9, 24, string3);
  mvaddstr(10,24, string4);
  attroff(WA_BOLD);
  refresh();
  echo();
  curs_set(1);
  move(10, 30);
  tmp = getch();
  curs_set(0);
  noecho();
   
  mvaddstr(6, 22, "                                     ");   
  mvaddstr(7, 22, "                                     ");   
  mvaddstr(8, 22, "                                     ");   
  mvaddstr(9, 22, "                                     ");   
  mvaddstr(10,22, "                                     ");   
  mvaddstr(11,22, "                                     ");   
  refresh();   

  return tmp;
}


/* Draw the winning screen and ask if you want to play another game. */

char draw_win(int player, int score_player, int networkplay, char *player_name) {

  char player_tmp[81];		/* tmp var for sprintf */
  char score_tmp[81];		/* tmp var for sprintf */
  int yesno = 0;		/* play again */
  char yesno_submit;		/* submit to website */
  char tmp[40];			/* tmp var for sprintf */
  int i;			/* clear -*/
  int y;			/* screen */
  char wget_cmdline[1000];
   
  sprintf(player_tmp, _("|          Player %d wins!           |"), player);
  sprintf(score_tmp,  _("|         Your score is %6d.     |"), score_player);
   
  mvaddstr(6, 22, ",___________________________________,");
  mvaddstr(7, 22, player_tmp);
  mvaddstr(8, 22, score_tmp);
  mvaddstr(9, 22, _("|    Do you want to play again?     |"));
  mvaddstr(10,22, "|              (y/n)                |");
  mvaddstr(11,22, "+-----------------------------------+");
  
  attron(COLOR_PAIR(6));
  mvaddstr(6, 22, ",___________________________________,");
  mvaddch(7, 22, '|');
  mvaddch(7, 58, '|');
  mvaddch(8, 22, '|');
  mvaddch(8, 58, '|');
  mvaddch(9, 22, '|');
  mvaddch(9, 58, '|');
  mvaddch(10, 22, '|');
  mvaddch(10, 58, '|');
  mvaddstr(11,22, "+-----------------------------------+");
  attroff(COLOR_PAIR(6));

  refresh();
  while ((yesno != 'y') && (yesno != 'n'))
    yesno = getch();


  /* ask submit */
  if (strcmp(player_name, "Computer") != 0) {
    if ((player == networkplay) || (networkplay == 0)) {
      sprintf(tmp, _("Your score is %6d"), score_player);
      yesno_submit = draw_dialog_ch(tmp,
				    _("Submit highscore to website?"),
				    _("Needs an internet connection."),
				    _("(y/n)"));
      
      if (yesno_submit == 'y') {
	sprintf(wget_cmdline,
		"wget -q -O - 'http://home.kcore.de/~kiza/linux/tornado/score.cgi?score=%d&name=%s'",
		score_player, player_name);
	system(wget_cmdline);
      }
      
    }
  }
  
  /* clear the screen */
  y = 6;
  for(i = 0; i <= 5; ++i) {
    mvaddstr(y, 22, "                                     ");
    y++;
  }
   
  return(yesno);
    
}

/* Draw the highscores table */

void draw_highscores(void) {
  char ch;
  char line[81];
  int i;							   /* used for counting */
  int y = 6;						   /* y coordinate */
  char name[23];					   /* holds tmp name var */
  int p_score;						   /* holds score */
   
   
  /* Read the scores line for line and print them on the screen. */
  if (highscore_get(1, name, &p_score) == -1) {
    draw_window(_("Error reading scorefile:"), SCOREFILE_NAME, errno ? strerror(errno) : _("File format error"), "");
    sleep(3);
    draw_window_delete();
    return;
  }
  
  attron(COLOR_PAIR(6));
  mvaddstr(4, 22, ",___________________________________,");
  attroff(COLOR_PAIR(6));
  attron(COLOR_PAIR(5));
  mvaddstr(5, 22, _("|             Highscores            |"));
  attroff(COLOR_PAIR(5));
  attron(COLOR_PAIR(6));
  mvaddstr(5,22, "|");
  mvaddstr(5,58, "|");
  attroff(COLOR_PAIR(6));

  
  for(i = 1; i <= 10; ++i) {
    highscore_get(i, name, &p_score);
    sprintf(line, "| %2d  %21s  %6d |", i, name, p_score);
	attron(WA_BOLD);
    mvaddstr(y, 22, line);
	attroff(WA_BOLD);
	attron(COLOR_PAIR(6));
	mvaddstr (y, 22, "|");
	mvaddstr (y, 58, "|");
	attroff(COLOR_PAIR(6));
    ++y;
    refresh();
  }
  
  attron(COLOR_PAIR(6));
  mvaddstr(16,22, "+-----------------------------------+");  
  attroff(COLOR_PAIR(6));
  refresh();
  ch = getch();

  /* The following lines delete the screen at the positions where
   * the highscore window was shown. */
  y = 4;
  for(i = 0; i <= 12; ++i) {
    mvaddstr(y, 22, "                                     ");
    ++y;
  }	  
  refresh();
}

/* Draw a multi purpose window.
 * Similar to draw_dialog() but without a return value. */

void draw_window(char *text1, char *text2, char *text3, char *text4) {
	attron(COLOR_PAIR(6));
	mvaddstr(6, 22, ",___________________________________,");
	mvaddstr(7, 22, "|                                   |");
	mvaddstr(8, 22, "|                                   |");
	mvaddstr(9, 22, "|                                   |");
	mvaddstr(10,22, "|                                   |");
	mvaddstr(11,22, "+-----------------------------------+");
	attroff(COLOR_PAIR(6));
	
	attron(WA_BOLD);
	mvaddstr(7, 24, text1);
	mvaddstr(8, 24, text2);
	mvaddstr(9, 24, text3);
	mvaddstr(10,24, text4);
	refresh();
}

/* Delete the window drawn by draw_network_window(). */

void draw_window_delete(void) {
   mvaddstr(6, 22, "                                     ");   
   mvaddstr(7, 22, "                                     ");   
   mvaddstr(8, 22, "                                     ");   
   mvaddstr(9, 22, "                                     ");   
   mvaddstr(10,22, "                                     ");   
   mvaddstr(11,22, "                                     ");   
   refresh();   
}

