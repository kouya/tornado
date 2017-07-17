/*
 * network.h: A simple network library.
 */

int network_listen(int port);
int network_connect(char *address, int port);
int network_close(void);
char const *network_get_remote_name(void);

int network_read(char *buf, int size);
int network_readint(int *i);
int network_readch(char *ch);
		    
int network_write(char *buf);
int network_writech(char ch);
int network_writeint(int i);
