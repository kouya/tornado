#define FALLING_SPEED 150000
#define DELETE_SPEED  50000
void draw_house(int x, int y);
void draw_cloud(int x, int y);
int count_destroyed(int x, int y, int width, int height);
int draw_weather(int x, int shift, char object, int strength);
int draw_snow(int x, int shift, char object);
int draw_lightning(int x, int player);
int draw_tornado(int x, int interval, int shift, int player);
void draw_tornado_cloud(void);
void draw_statusline(char *text);
void draw_clear_sky(void);
char draw_user_interface(void);
void draw_dialog(char *question, char *answer, int length);
char draw_win(int player, int score_player, int networkplay, char *player_name);
void draw_highscores(void);
void draw_window(char *text1, char *text2, char *text3, char *text4);
void draw_window_delete(void);
char draw_dialog_ch(char *string1, char *string2, char *string3, char *string4);
int count_snow(int x, int y, int width, int height);
