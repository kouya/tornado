/* scores.h -- highscore functions */

/* score file name */
#define SCOREFILE_NAME "/var/games/tornado.scores"

/* maximum length for a player name (includes \0) */
#define MAX_PLAYER_LEN 22

int highscore_add(char const *player, int const score);
int highscore_get(int const rank, char *player, int *score);
