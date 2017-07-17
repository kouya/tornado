/* Tornado - Two player weather action game
 *
 * Copyright (C) 2000-2002  Oliver Feiler (kiza@gmx.net)
 * 
 * main.c
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


#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef LOCALEPATH
#  include <libintl.h>
#  include <locale.h>
#endif

#include "draw.h"
#include "version.h"
#include "erwin.h"
#include "network.h"
#include "scores.h"
//#include "house.h"

#define SERVER_PORT 3000	/* Port that the server should run on. */

#ifdef LOCALEPATH
#  define _(String) gettext (String)
#else
#  define _(String) (String)
#endif


char player1[21];		/* name for player 1 */
char player2[21];		/* guess what */
int current_player = 1;		/* just what it says */
int cloud_x;			/* horizontal cloud position */
int networkplay = 0;		/* indicates whether the game runs in
				 * network mode or not:
				 * 0 = nope
				 * 1 = server
				 * 2 = client */
int login = 0;			/* logged in at the tornado website? */
char side = '0';        	/* the side (client/server) that quit the game
                                 * 0 = we did; 1 = other side */

void main_network_readint(int *i);
void main_network_writeint(int i);

/**
 * Return a random integer between (and including) 0 and max.
 **/

int main_rand(int max) {
  return (float)rand() / (float)RAND_MAX * max;
}


/* Ask for the player names and put them into player1 and player2. */

void get_player_names(void) {   
  draw_dialog(_("Please enter your name Player 1"), player1, sizeof(player1)-1);
  draw_dialog(_("Please enter your name Player 2"), player2, sizeof(player2)-1);
}

/* Init the curses library, clear the screen and hide the cursor. */

void init_curses(void) {
	initscr();
	start_color();
	cbreak();
	noecho();
	clear();
	curs_set(0);
	refresh();
	
	/* White text on black bg. Set WA_BOLD to get pure white. */
	init_pair (1, COLOR_WHITE, COLOR_BLACK);
	
	/* Brown on black, set WA_BOLD to get yellow. */
	init_pair (2, COLOR_YELLOW, COLOR_BLACK);
	
	init_pair (3, COLOR_RED, COLOR_BLACK);
	
	init_pair (4, COLOR_CYAN, COLOR_BLACK);
	
	init_pair (5, COLOR_GREEN, COLOR_BLACK);
	
	init_pair (6, COLOR_BLUE, COLOR_BLACK);;
	
	/* "Clear" screen with new background color. */
	bkgdset(' '|COLOR_PAIR(1));
	clrtobot();
  	/*
	if (has_colors())
		mvprintw (0,0,"%d colours available in %d pairs", COLORS, COLOR_PAIRS);

	attron (COLOR_PAIR(1));
	attron(WA_BOLD);
	mvaddstr (2,0,"white on black");
	
	attroff(WA_BOLD);
	attroff (COLOR_PAIR(1));
	
	attron (COLOR_PAIR(2));
	attron(WA_BOLD);
	mvaddstr (3,0,"yellow on black");
	attroff(WA_BOLD);
	attroff(COLOR_PAIR(2));
	
	attron(COLOR_PAIR(3));
	mvaddstr(4,0,"red on black");
	attroff(COLOR_PAIR(3));
	
	attron(COLOR_PAIR(4));
	mvaddstr(5,0,"cyan on black");
	attroff(COLOR_PAIR(4));
    
	refresh();
	getch();
	endwin();
	exit(1);
	*/
}
/* Clear the screen, deinitialize ncurses and quit. */

void main_quit(void) {
  clear();
  refresh();
  endwin();

  if (networkplay) {
    if (side == '1') {
      if (current_player == 1)
	printf(_("%s quit the game.\n"), player1);
      if (current_player == 2)
	printf(_("%s quit the game.\n"), player2);
    } else if (side == '0')
      printf(_("You quit the game.\n"));
  }
  printf(_("Thanks for playing Tornado!\n"));

  network_close();
  exit(0);
}

/* main_network_function (the following 5 functions):
 * These functions are used for error checking rather than checking for
 * errors inside the main program. If a function in network.c
 * returns an error (-1) the program aborts and prints an error message
 * to stderr. */

void main_network_read(char *buf, int size){
  int result;   
  result = network_read(buf, size);
  if (result == -1) {
    perror(_("Error reading from network: "));
    main_quit();
  }
}

void main_network_write(char *buf) {
  int result;
  result = network_write(buf);
  if (result == -1) {
    perror(_("Error writing to network: "));
    main_quit();
  }
}

void main_network_readint(int *i) {
  int result;
  result = network_readint(i);
  if (result == -1) {
    perror(_("Error reading from network: "));
    main_quit();
  }
}

void main_network_writeint(int i) {
  int result;
  result = network_writeint(i);
  if (result == -1) {
    perror(_("Error writing to network: "));
    main_quit();
  }
}

void main_network_writech(char ch) {
  int result;
  result = network_writech(ch);
  if (result == -1) {
    perror(_("Error writing to network: "));
    main_quit();
  }
}

void main_network_readch(char *ch) {
  int result;
  result = network_readch(ch);
  if (result == -1) {
    perror(_("Error reading network: "));
    main_quit();
  }
}

/*
 * main function
 *
 * Setup everything and enter the main program loop.
 */

int main(int argc, char *argv[]) {
  char input;			/* used for various keyboard inputs */
  char input_net[81];		/* var read from network */
  char statusline[81];		/* holds statusline text */
  char windspeed_str[23];	/* input buffer for aim */
  int windspeed = 0;		/* randomly generated windspeed */
  int userinput = 0;		/* atoi'd version of windspeed_str */
  //  int current_player = 1;	/* just what it says */
  int auto_mode = 0;		/* if 1, play in demo mode */
  //  int erwin_destroy_count = 0;	/* how much did the AI destroy yet? */
  //  int self_destroyed = 0;	/* how much of my house is destroyed? */
  int score_player1 = 0;	/* --- player --- */
  int score_player2 = 0;	/* --- scores --- */
  int score_tmp = 0;		/*  */
  char version_check[100];	/* var for comparing client/server version */
  int rand_pos;			/* random cloud position */
  char yesno;			/* used for the winning dialog */
  char tmp1[80], tmp2[80], ch;
  /*  char *split_ptr, *split_name, *split_ip, *split_time; */
  int net_connect_error;
  /*  FILE *wget; */
  
  /* Builds the house inside the array */
  /************************************************
   * THIS IS THE NEW ARRAY HOUSE GENERATION STUFF *
   ************************************************/
  //  char house[17][10] = 0;	/* array holding the house state */
  //  generate_house(0);		/* call the generator function */
  
#ifdef LOCALEPATH
  setlocale (LC_ALL, "");
  bindtextdomain ("tornado", LOCALEPATH);
  textdomain ("tornado");
#endif
   
  /* Check for arguments */
   
  if (argc > 1) {
    if (strcmp(argv[1], "--demo") == 0)
      auto_mode = 1;


    /*
    if (strcmp(argv[1], "--get-players") == 0) {
      printf("\n");
      printf("Name                   | IP              | Login time\n");
      printf("-----------------------+-----------------+-----------\n");
      wget = popen("wget -q -O - http://spot.local/~kiza/linux/tornado/online-players", "r");
      while (!feof(wget)) {
	
	if (fgets(tmp1, sizeof(tmp1), wget)) {
	  
	  
	  
          split_name = tmp1;

	  split_ptr = strchr(split_name, ':');
	  if (!split_ptr)
	    continue;
	  *split_ptr = '\0';

	  split_ip = split_ptr + 1;

	  split_ptr = strchr(split_ip, ':');
	  if (!split_ptr)
	    continue;
	  *split_ptr = '\0';
	  
	  split_time = split_ptr + 1;
	  
	  printf("%-22.22s   %-16.16s  %-9.9s\n", split_name, split_ip, split_time);

	}

      }

      fclose(wget);
      printf("\n");
      exit(0);
    }
    */

    /* If "--login" is specified execute login code and change argv[1] to
       "--server". */
    /*
    if (strcmp(argv[1], "--login") == 0) {
      printf("debug: --login\n");
      argv[1] = "--server";
      login = 1;
    }
    */


	  
    /* The game will act as server, open a socket and liste on port
     * SERVER_PORT for a connecting client. If opening the socket fails
     * the program aborts.
     * If everything is fine, server and client program version are checked.
     * If they match, go on, if they don't match print out a warning
     * message. */
    if (strcmp(argv[1], "--server") == 0) {
      networkplay = 1;			   /* Set server mode. */
      printf(_("Waiting for connection on port %d...\n"), SERVER_PORT);
      if (network_listen(SERVER_PORT) == -1) {
	perror(_("Could not initialize Network"));
	printf(_("Exiting.\n"));
	exit (1);
      } else {
	printf(_("Client from %s connected at port %d.\n\n"),
	       network_get_remote_name(), SERVER_PORT);
	main_network_write(NETVERSION);
	main_network_read(version_check, sizeof(version_check)-1);
	if (strcmp(version_check, NETVERSION) != 0) {
	  printf(_("Program versions mismatch! You are running version %s, "
		 "client is %s.\nYou can play, but the game will get out of sync and probably crash.\n\n"),
		 NETVERSION, version_check);
	  sleep(3);
	}
      }
      
    }
    /* The game will act a client and connect to the specified IP at port
     * SERVER_PORT. If the connection fails, the program exits.
     * See comment for section "--server" above. */
    if (strcmp(argv[1], "--connect") == 0 ) {
      networkplay = 2;			   /* Set client mode. */
      if (argc != 3) {			   /* Check of the user has given enough
					    * arguments. If not, quit instead of segfault.*/
	printf(_("No hostname specified. Exiting...\n"));
	exit(1);
      }
      printf(_("Connecting to server %s on port %d...\n"), argv[2], SERVER_PORT);
      net_connect_error = network_connect(argv[2], SERVER_PORT);
      if (net_connect_error == -1) {
	perror(_("Could not connect to server"));
	printf(_("Exiting.\n"));
	exit (1);
      } else if (net_connect_error == -2) {
	printf(_("Could not connect to server: hostname lookup failure\n"));
	printf(_("Exiting.\n"));
	exit (1);
      } else {
	printf(_("Connection to server %s established.\n\n"), argv[2]);
	main_network_write(NETVERSION);
	main_network_read(version_check, sizeof(version_check)-1);
	if (strcmp(version_check, NETVERSION) != 0) {
	  printf(_("Program versions mismatch! Your are running version %s, "
		 "server is %s.\nYou can play, but the game will get out of sync and probably crash.\n\n"),
		 NETVERSION, version_check);
	  sleep(3);
	}
      }
    }	  

    if (strcmp(argv[1], "--version") == 0) {
      printf(_("Tornado version %s\n"), VERSION);
      return(1);
    }

    /* Print usage summary for every unknown command line. */
    if (strcmp(argv[1], "--help") == 0 || (strcmp(argv[1], "--connect") != 0
					   && strcmp(argv[1], "--demo") != 0
					   && strcmp(argv[1], "--server") != 0
					   && strcmp(argv[1], "--version") != 0
					   && strcmp(argv[1], "--get-players") != 0)) {
      printf(_("Tornado %s\n\n"), VERSION);
      printf(_("usage: [--connect IP|--demo|--help|--server]\n\n"));
      printf(_("\t--connect [IP]\tConnect to IP/host for network play.\n"));
      printf(_("\t--demo\t\tRun in demo mode.\n"));
      /*      printf("\t--get-players\tGet list of online players from online players webpage.\n");*/
      printf(_("\t--help\t\tPrint this help message.\n"));
      /*      printf("\t--login\t\tLogin on the online players webpage.\n"*/
      /*	     "\t\t\tThis also activates \"--server\". (see manpage)\n");*/
      printf(_("\t--server\tRun as server so others can connect.\n"));
      printf(_("\t--version\tPrint version number and exit.\n"));
      return(1);
    }	  
  }

  srand(time(0));			      /* initializes the random number generator */
   
  /* init ncurses and draw the startup screen */

  init_curses();
  draw_house(5,14);
  draw_house(59,14);
  draw_cloud(10,0);

  /* if started with --demo use default player names.
   * Player who has server will be assigned to Player #1;
   * client will be assigned Player #2
   * Otherwise ask both players */
   
  if (networkplay == 1) {
    draw_dialog(_("Please enter your name"), player1, sizeof(player1)-1);
    main_network_write(player1);
    draw_window("", _("Waiting for other player..."), "", "");
    main_network_read(player2, sizeof(player2)-1);
    draw_window_delete();
  } else if (networkplay == 2) {
    draw_dialog(_("Please enter your name"), player2, sizeof(player2)-1);
    main_network_write(player2);
    draw_window("", _("Waiting for other player..."), "", "");
    main_network_read(player1, sizeof(player1)-1);
    draw_window_delete();
  } else if (auto_mode == 0) {
    get_player_names();
    if (strcmp(player1, "") == 0)
      strcpy(player1, "Computer");
    if (strcmp(player2, "") == 0)
      strcpy(player2, "Computer");
  } else {
    strcpy(player1, "Computer");
    strcpy(player2, "Computer");
  }
   
  /**                      **/
  /* The main program loop. */
  /**                      **/
 
  while (1) {

    /* Check if one of the houses is destroyed and draw the winning screen
     * for the winning player. Aaaand restart the game if necessary. */

    /* The new count_destroyed if-loop doesn't count snow as part of
     * house. count_destroyed + count_snow */
    //    if ((count_destroyed(5, 14, 17, 10)-32) == 138) {
    //      score_player2 *= (100-((count_destroyed(59, 14, 17, 10)-32) / 138.0 * 100.0));
    if (((count_destroyed(5, 14, 17, 10)-32)+ (count_snow(5, 14, 17, 10))) == 138) {
      score_player2 *= (100-((count_destroyed(59, 14, 17, 10)-32) / 138.0 * 100.0));
      highscore_add(player2, score_player2);
      yesno = draw_win(2, score_player2, networkplay, player2);
      
      if (networkplay == 1)
	main_network_writech(yesno);
      else if (networkplay == 2)
	main_network_readch(&yesno);
		 
      if (yesno == 'y') {			 
	clear();
	draw_house(5,14);
	draw_house(59,14);
	score_player1 = 0;
	score_player2 = 0;
      } else {
	main_quit();
      }
    }

//    else if ((count_destroyed(59, 14, 17, 10)-32) == 138) {
//      score_player1 *= (100-((count_destroyed(5, 14, 17, 10)-32) / 138.0 * 100.0));
    else if (((count_destroyed(59, 14, 17, 10)-32) + (count_snow(59, 14, 17, 10))) == 138) {
      score_player1 *= (100-((count_destroyed(5, 14, 17, 10)-32) / 138.0 * 100.0));
      highscore_add(player1, score_player1);
      yesno = draw_win(1, score_player1, networkplay, player1);
      
      if (networkplay == 1)
	main_network_writech(yesno);
      else if (networkplay == 2)
	main_network_readch(&yesno);

      if (yesno == 'y') {
	clear();		/* make sure everything is cleared - 1.0.4*/
	draw_house(5,14);
	draw_house(59,14);
	score_player1 = 0;
	score_player2 = 0;
      } else {
	main_quit();
      }
    }

    /* Clear the sky after each round and set the
     * windspeed to a random value */
	  
    draw_clear_sky();
    windspeed = (main_rand(10) - 5);

    /* Send the windspeed to the other side if we are server or
     * wait until we received it if we're client. */
	  
    if (networkplay == 1) {
      sprintf(input_net, "%d", windspeed);
      main_network_write(input_net);
    } else if (networkplay == 2) {
      main_network_read(input_net, sizeof(input_net));
      windspeed = atoi(input_net);
    }
	  
    /* Sprintf the statusline... */
	  
    sprintf(statusline, _("%c%-23s  %3.0f%% | Windspeed: %2d | %3.0f%%   %23s%c"),
	    (current_player == 1) ? '*' : ' ',
	    player1,
	    //	    100-((count_destroyed(5, 14, 17, 10)-32) / 138.0 * 100.0),
	    100-(((count_destroyed(5, 14, 17, 10)-32) + (count_snow(5, 14, 17, 10))) / 138.0 * 100.0),
	    windspeed,
	    //	    100-((count_destroyed(59, 14, 17, 10)-32) / 138.0 * 100.0),
	    100-(((count_destroyed(59, 14, 17, 10)-32) + (count_snow(59, 14, 17, 10))) / 138.0 * 100.0),
	    player2,
	    (current_player == 2) ? '*' : ' ');
	  
    /* ... and display it. */
	  
    draw_statusline(statusline);

    /* Get a random value for the cloud position. */
	  
    cloud_x = main_rand(80 - 29);	  

    /* Send it over the network if we are server or recieve it, if
     * we are client. */

    if (networkplay == 1) {		 sprintf(input_net, "%d", cloud_x);
    main_network_write(input_net);
    } else if (networkplay == 2) {
      main_network_read(input_net, sizeof(input_net));
      cloud_x = atoi(input_net);
    }
	  
    /* Now draw the cloud. */
	  
    draw_cloud(cloud_x,0);
	  
    /**                                         **
     * if: get a weather and an aim from the AI; *
     * else: ask the player for his choice.      *
     **                                         **/

    if (((current_player == 1) && (strcmp(player1, "Computer") == 0))
	|| ((current_player == 2) && (strcmp(player2, "Computer") == 0))) {
		 
      /* Get a weather and an aim from the AI */

      /*
      if (current_player == 2) {
	erwin_destroy_count = (count_destroyed(5, 14, 17, 10)-32);
	self_destroyed = (count_destroyed(59, 14, 17, 10)-32);
      }
      if (current_player == 1) {
	erwin_destroy_count = (count_destroyed(59, 14, 17, 10)-32);
	self_destroyed = (count_destroyed(5, 14, 17, 10)-32);
	}
      */
		 
//      input = erwin_choice(erwin_destroy_count, self_destroyed, cloud_x, current_player);
//      userinput = erwin_aim(input, windspeed, cloud_x, current_player);
      input = erwin_choice_new(cloud_x, current_player);
      userinput = erwin_aim_new(input, windspeed, cloud_x, current_player);
		 
      switch (input) {
      case 'l':					   /* lightning */
	score_tmp = draw_lightning(cloud_x - 6 + main_rand(27), current_player);
	break;
      case 'r':					   /* rain */
	score_tmp = draw_weather(cloud_x + main_rand(14), windspeed + userinput, '/', 1);
	break;
      case 's':					   /* snow */
	/*	score_tmp = draw_weather(cloud_x + main_rand(14), windspeed + userinput, '*', 1);*/
	score_tmp = draw_snow(cloud_x + main_rand(14), windspeed + userinput, '*');
	break;
      case 'h':					   /* hail */
	score_tmp = draw_weather(cloud_x + main_rand(14), windspeed + userinput, ':', 2);
	break;
      case 't':					   /* tornado */
	draw_tornado_cloud();
	score_tmp = draw_tornado(cloud_x, 1, windspeed + userinput, current_player);
	break;
      }
    } else {

    input_here:
		 
      /* If we are playing a network game, draw a little window to let
       * the player know what's going on and wait until we got the
       * data from the other player over the network. */
      if ((networkplay == 1) && (current_player == 2)) {
	draw_window("", _("Waiting for other player..."), "", "");
	main_network_read(input_net, sizeof(input_net));
	draw_window_delete();
	input = input_net[0];
      } else if ((networkplay == 2) && (current_player == 1)) {
	draw_window("", _("Waiting for other player..."), "", "");
	main_network_read(input_net, sizeof(input_net));
	draw_window_delete();
	input = input_net[0];
      } else {
			
	/* If there is no network game running, proceed normally
	 * and draw the user interface. */
	input = draw_user_interface();
			
	/* If we are the current player in network mode,
	 * send the data to the other side when we entered it. */
	if ((input != 'o') && (input != 'c') && (networkplay))
	  network_writech(input);
      }

			
      switch (input) {
      case 'q':					   /* program quit */
	if (input_net[0] == 'q')
	  side = '1';
	main_quit();
	break;
      case 'o':					   /* draw the highscores table */
	draw_highscores();
	goto input_here;
	break;
      case 'c':
	sprintf(tmp1, _("%d points"), score_player1);
	sprintf(tmp2, _("%d points"), score_player2);
	draw_window(player1, tmp1, player2, tmp2);
	ch = getch();
	draw_window_delete();
	goto input_here;
	break;
      case 'r':					   /* rain */
	rand_pos = main_rand(14);

	/* If we are not the current player in network mode get the values
	 * over the network. */
	if (((networkplay == 1) && (current_player == 2)) ||
	    ((networkplay == 2) && (current_player == 1))) {
	  main_network_read(windspeed_str, sizeof(windspeed_str));
	  main_network_readint(&rand_pos);
	} else {				   /* If we're not in network mode, */
	  do {					   /* ask the player for his aim. */
	    draw_dialog("Aim?", windspeed_str, sizeof(windspeed_str)-1);
	  } while (strlen(windspeed_str) == 0);
			   
	  /* If we are current player in network mode
	   * write the values we entered to the network. */
	  if (networkplay) {
	    main_network_write(windspeed_str);
	    main_network_writeint(rand_pos);
	  }
	}
			
	/* Pass all variables to the draw_weather() funtion. */
	userinput = atoi(windspeed_str);
	score_tmp = draw_weather(cloud_x + rand_pos, windspeed + userinput, '/', 1);
	break;
      case 's':					   /* snow */
	rand_pos = main_rand(14);
	if (((networkplay == 1) && (current_player == 2)) ||
	    ((networkplay == 2) && (current_player == 1))) {
	  main_network_read(windspeed_str, sizeof(windspeed_str));
	  main_network_readint(&rand_pos);
	}
	else {
	  do {
	    draw_dialog(_("Aim?"), windspeed_str, sizeof(windspeed_str)-1);
	  } while (strlen(windspeed_str) == 0);
	  if (networkplay) {
	    main_network_write(windspeed_str);
	    main_network_writeint(rand_pos);
	  }
	}
	userinput = atoi(windspeed_str);
	score_tmp = draw_snow(cloud_x + rand_pos, windspeed + userinput, '*');
	break;
      case 'h':					   /* hail */
	rand_pos = main_rand(14);
	if (((networkplay == 1) && (current_player == 2)) ||
	    ((networkplay == 2) && (current_player == 1))) {
	  main_network_read(windspeed_str, sizeof(windspeed_str));
	  main_network_readint(&rand_pos);
	}
	else {
	  do {
	    draw_dialog(_("Aim?"), windspeed_str, sizeof(windspeed_str)-1);
	  } while (strlen(windspeed_str) == 0);
	  if (networkplay) {
	    main_network_write(windspeed_str);
	    main_network_writeint(rand_pos);
	  }
	}
	userinput = atoi(windspeed_str);
	score_tmp = draw_weather(cloud_x + rand_pos, windspeed + userinput, ':', 2);
	break;
      case 'l':					   /* lightning */
	rand_pos = main_rand(27);
	if (((networkplay == 1) && (current_player == 2)) ||
	    ((networkplay == 2) && (current_player == 1)))
	  main_network_readint(&rand_pos);
	else if (networkplay)
	  main_network_writeint(rand_pos);
	score_tmp = draw_lightning(cloud_x - 6 + rand_pos, current_player);
	break;
      case 't':					   /* tornado */
	draw_tornado_cloud();
	if (((networkplay == 1) && (current_player == 2)) ||
	    ((networkplay == 2) && (current_player == 1)))
	  main_network_read(windspeed_str, sizeof(windspeed_str));
	else {
	  do {		   
	    draw_dialog(_("Aim?"), windspeed_str, sizeof(windspeed_str)-1);
	  } while (strlen(windspeed_str) == 0);
	  if (networkplay)
	    main_network_write(windspeed_str);
	}
	userinput = atoi(windspeed_str);
	score_tmp = draw_tornado(cloud_x, 1, windspeed + userinput, current_player);
	break;
      default:
	beep();
	goto input_here;
      }
    }
	  
    /* Cycle players */
	  
    if (current_player == 1) {
      current_player = 2;
      score_player1 = score_player1 + (score_tmp * score_tmp);
    } else if (current_player == 2) {
      current_player = 1;
      score_player2 = score_player2 + (score_tmp * score_tmp);
    }
  }
   
  main_quit();
  return 0;
}
