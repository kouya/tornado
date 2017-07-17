/* Tornado - Two player weather action game
 * 
 * Copyright (C) 2000  Rene Puls (kianga@claws-and-paws.com)
 * network.c
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
#include <assert.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

/* local variables */
static int socketfd;		       /* active socket file descriptor */
static int const one = 1; 	       /* for the stupid setsockopt(7) function */
static char *recvbuf = NULL;       /* our receive buffer */
static char remote_name[256];	   /* remote host name */

/* description: 
 *    opens a listening socket and waits for a connection
 * 
 * arguments:
 *    port -- the port number on which we should listen
 * 
 * returns:
 *    0 -- success
 *   -1 -- failure
 */

int network_listen(int port) 
{
  int listenfd;		       /* listening file descriptor */
  int result;			       /* used to hold result codes of
					* various functions */

  socklen_t addr_len;
  struct sockaddr_in my_address;      /* our own address (ip+port) */
  struct sockaddr_in remote_address;  /* remote address */
  struct hostent *rhostent;
   
  /* fill in the port */
  my_address.sin_family = AF_INET;    /* TCP/IP family */
  my_address.sin_addr.s_addr = htonl(INADDR_ANY); /* (all interfaces) */
  my_address.sin_port = htons(port);  /* port to listen on */
   
  /* create a socket */
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0)
    return -1;

  /* set SO_REUSEADDR so that we can bind to the socket even if there is
   * still a TIME_WAIT connection on this port (maybe from a previous
   * instance of this program) */
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

  /* bind the socket on a port */
  result = bind(listenfd, (struct sockaddr *) &my_address, sizeof(my_address));
  if (result < 0)
    return -1;
   
  /* wait for a connection */
  listen(listenfd, 1);
   
  addr_len = sizeof(struct sockaddr_in);
   
  /* accept the connection */
  socketfd = accept(listenfd, (struct sockaddr *) &remote_address, &addr_len);
  if (socketfd < 0)
    return -1;

  /* try to resolve the remote address */
  rhostent = gethostbyaddr((char *)&remote_address.sin_addr, 
			   sizeof(struct in_addr), AF_INET);
  if (rhostent != NULL) 
    strcpy(remote_name, rhostent->h_name);
  else /* if we cannot resolve it, use the IP address */
    strcpy(remote_name, inet_ntoa(remote_address.sin_addr));

  /* close the listening socket */
  if (close(listenfd) != 0)
    return -1;

  return 0;
}

/* description:
 *    connects to a remote host
 * 
 * arguments:
 *    host -- a remote address (host name or IP)
 *    port -- the port to connect to
 * 
 * returns:
 *    0 -- success
 *   -1 -- failure
 */

int network_connect(char *host, int port)
{
  int result;			       /* used to hold result codes of
					  various functions */
  struct sockaddr_in address;      /* our own address (ip+port) */
  struct hostent *h;
   
  /* resolve the host name */
  h = gethostbyname(host);
  if (h == NULL)
    return -2;						   /* exitstat -2 is checked seperately in main.c */
   
  /* fill in the port */
  address.sin_family = AF_INET;    /* TCP/IP family */
  memcpy((char *) &address.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  address.sin_port = htons(port);  /* port to listen on */

  /* create the socket */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
    return -1;
   
  /* connect to the remote host */
  result = connect(socketfd, (struct sockaddr *) &address, sizeof(address));
  if (result < 0)
    return -1;

  /* try to resolve the remote address */
  h = gethostbyaddr((char *) &address, sizeof(address), AF_INET);
  if (h != NULL) {
    strcpy(remote_name, h->h_name);
  } else {
    /* if not resolvable, use the IP dotted notation */
    strcpy(remote_name, inet_ntoa(address.sin_addr));
  }
   
  return 0;
}

/* description:
 *    closes the active connection
 * 
 * returns:
 *    0 -- success
 *   -1 -- failure
 */

int network_close(void)
{
  if (close(socketfd) != 0)
    return -1;
  else
    return 0;
}

/* description:
 *    returns the host name or IP address of the remote host
 * 
 * arguments:
 *    none
 * 
 * returns:
 *    a pointer to the remote host name
 *    this is a statically allocated buffer - it must not be free()'d
 */

char const *network_get_remote_name(void)
{
  return remote_name;
}



/* description:
 *    reads data from the active connection
 * 
 * arguments:
 *    buf -- buffer to hold a null-terminated string containing the received
 *           data
 *    size -- size of the above buffer
 * 
 * returns:
 *    0 -- success
 *   -1 -- failure
 */

int network_read(char *buf, int size)
{
  char tmpbuf[1024];		       /* temporary receive buffer */
  char *eol_tmp;		       /* pointer to the eol of the first string */
  int result;
  int newlen;

  if (recvbuf == NULL) {	       /* allocate recvbuf the first time it is used */
    recvbuf = (char *) malloc(1);
    recvbuf[0] = '\0';
  }

  /* now loop until at least one complete line (\n) is in the buffer */
  while (memchr(recvbuf, '\n', strlen(recvbuf)) == NULL) {
      
    /* read data from the socket */
    result = read(socketfd, tmpbuf, sizeof(tmpbuf));
    if (result < 0) /* abort immediately if read failed */
      return -1;
      
    /* ASSERTION: received data must not contain any \0 characters */
    assert(memchr(tmpbuf, '\0', result) == NULL);
      
    /* compute the new size of the buffer */
    newlen = strlen(recvbuf)+result;
      
    /* increase the size of the recvbuf so that the new data fits in */
    recvbuf = (char *) realloc(recvbuf, newlen+1);
      
    /* append the new data to the buffer */
    memcpy(&recvbuf[strlen(recvbuf)], tmpbuf, result);
      
    /* terminate the buffer with \0 */
    recvbuf[newlen] = '\0';
  }
   
  /* now read the first line from the buffer and remove it afterwards: */
   
  /* save the position of the end-of-line char */
  eol_tmp = memchr(recvbuf, '\n', strlen(recvbuf));
   
  /* cut off the first part and let eol_tmp point to the following string */
  eol_tmp[0] = '\0';
  ++eol_tmp;

  /* okay. copy the first string into user supplied buffer */
  strncpy(buf, recvbuf, size);
   
  /* now move the new string plus \0 to the beginning of the buffer */
  memmove(recvbuf, eol_tmp, strlen(eol_tmp)+1);
   
  /* finally, shrink the buffer to fit just the new string */
  recvbuf = (char *) realloc(recvbuf, strlen(recvbuf)+1);
   
  return 0;
}

/* description:
 *    reads an char from the network
 * 
 * arguments:
 *    ch -- pointer to a char variable
 * 
 * returns:
 *    0 -- success
 *   -1 -- failure
 * 
 * notes:
 *   this actually reads a string and returns just the first character.
 */

int network_readch(char *ch)
{
  char buf[2];
   
  /* read a line from the network and convert it to an integer */
  network_read(buf, sizeof(buf));
  *ch = buf[0];
   
  return 0;
} 

/* description:
 *    reads an integer from the network
 * 
 * arguments:
 *    i -- pointer to an integer variable
 * 
 * returns:
 *    0 -- success
 *   -1 -- failure
 */

int network_readint(int *i)
{
  char buf[20];
   
  /* read a line from the network and convert it to an integer */
  network_read(buf, sizeof(buf));
  *i = atoi(buf);
   
  return 0;
}

/* description:
 *    sends data over the active connection
 * 
 * returns:
 *    0 -- success
 *   -1 -- failure
 */

int network_write(char *buf)
{
  char *tmpbuf;		       /* temporary send buffer */
  int result;
   
  /* allocate a temporary buffer - we need to append a \n to
   * the end of the data. */
  tmpbuf = (char *) malloc(strlen(buf)+2);
  strcpy(tmpbuf, buf);
  strcat(tmpbuf, "\n");

  /* send the line */
  result = write(socketfd, tmpbuf, strlen(tmpbuf));
   
  /* deallocate the temp buffer again */
  free(tmpbuf);
   
  if (result < 0)
    return -1;
  else
    return 0;
}

/* description:
 *    sends a character over the active connection
 * 
 * arguments:
 *    ch -- a single character
 * 
 * returns:
 *    0 -- success
 *   -1 -- failure
 */

int network_writech(char ch)
{
  char *tmpbuf;
  int result;

  /* convert the character to a short string with \n */
  tmpbuf = (char *) malloc(2);
  tmpbuf[0] = ch;
  tmpbuf[1] = '\n';

  /* send it */
  result = write(socketfd, tmpbuf, 2);
   
  free(tmpbuf);
   
  if (result < 0)
    return -1;
  else
    return 0;
}

/* description:
 *    write an integer to the network
 * 
 * arguments:
 *    i -- an integer
 * 
 * returns:
 *    0 -- success
 *   -1 -- failure
 */

int network_writeint(int i)
{
  char tmpbuf[20];
  int result;
   
  sprintf(tmpbuf, "%d\n", i);

  result = write(socketfd, tmpbuf, strlen(tmpbuf));
   
  if (result < 0)
    return -1;
  else
    return 0;
}

