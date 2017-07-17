/* Tornado - Two player weather action game
 * 
 * Copyright (c) 2000  Rene Puls (kianga@claws-and-paws.com)
 * Copyright (c) 2001-2002 Oliver Feiler (kiza@gmx.net)
 *
 * scores.c
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
#include <string.h>
#include "scores.h"

struct highscore_entry {
  char player[MAX_PLAYER_LEN];
  int score;
};

/* Read the score file into an array of highscore_entry.
 * The array must be large enough to hold ten (10) entries.
 */

static int read_score_file(struct highscore_entry scores[]) {
  FILE *scorefile;
  char line[128];
  char *tmp;
  int i;
   
  scorefile = fopen(SCOREFILE_NAME, "r");
  if (scorefile == NULL) {
    /* error opening score file */
    return -1;
  }
   
  fgets(line, sizeof(line), scorefile);
  fgets(line, sizeof(line), scorefile);
   
  for (i=0; i < 10; ++i) {
	  
    if (fgets(line, sizeof(line), scorefile) == NULL) {
      /* error reading file */
      return -1;
    }

    line[strlen(line)-1] = '\0'; /* remove trailing newline */

    /* We now need to split the line at the \t character. */
    tmp = strchr(line, '\t');

    if (tmp == NULL)		/* Abort if there is no \t */
      return -1;
    
    /* Split the strings and advance tmp so that it points to the second
       part (the actual highscore number). */

    *tmp = '\0';
    tmp++;

    if (*tmp == '\0')		/* Abort if there is no score... */
      return -1;

    /* check if the player name is not too long, otherwise truncate it */
    if (strlen(line) > MAX_PLAYER_LEN-1)
      line[MAX_PLAYER_LEN-1] = '\0';

    strcpy(scores[i].player, line);
    sscanf(tmp, "%d", &scores[i].score);
  }
   
  fclose(scorefile);
   
  return 0;
}

/* Writes a highscore_entry[10] array to the score file on disk. */

static int write_score_file(struct highscore_entry scores[]) {
  FILE *scorefile;
  char line[128];
  int i;
   
  scorefile = fopen(SCOREFILE_NAME, "w");
  if (scorefile == NULL) {
    /* error opening score file */
    return -1;
  }
   
  fprintf(scorefile, "# Tornado highscores file - do not edit! Editing voids the file's warranty! ;-)\n\n");
   
  for (i=0; i<10; ++i) {
    sprintf(line, "%s\t%d\n", scores[i].player, scores[i].score);
    fputs(line, scorefile);
  }
   
  fclose(scorefile);
  return(0);
}

/* Add a new high score to the score file.
 * 
 * This function checks if the score deserves to get into the score file,
 * and adds it if necessary.
 */

int highscore_add(char const *player, int const score) 
{
  struct highscore_entry scores[11];
  struct highscore_entry temp;
  int i;
   
  if (read_score_file(scores) == -1) {
    /* error reading score file */
    return -1;
  }

  /* add the new score at the bottom */
  strncpy(scores[10].player, player, MAX_PLAYER_LEN-1);
  scores[10].score = score;
   
  /* now sort the table */
  for (i = 10; i > 0; --i) {
    if (scores[i].score > scores[i-1].score) {
      /* switch the highscore entries */
      memcpy(&temp, &scores[i-1], sizeof(scores[i-1]));
      memcpy(&scores[i-1], &scores[i], sizeof(scores[i]));
      memcpy(&scores[i], &temp, sizeof(temp));
    }
  }

  if (write_score_file(scores) == -1) {
    /* error reading score file */
    return -1;
  }

  return 0;
}

/* Returns the highscore from the given rank on table. */

int highscore_get(int const rank, char *player, int *score) 
{
  struct highscore_entry scores[10];

  /* FIXME: this should probably be done only once, not for every call */
  if (read_score_file(scores) == -1) {
    /* error reading score file */
    *score = -1;
    player = NULL;
    return -1;
  }
   
  strcpy(player, scores[rank-1].player);
  *score = scores[rank-1].score;
   
  return 0;
}
