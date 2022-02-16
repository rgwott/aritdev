/**
 * \file client.h
 *
 * \date 2022/02/16
 * \author Rodolfo Wottrich
 * \version 1.0
 *
 * \brief Declarations related to client.c.
 *
 */


#ifndef CLIENT_H_
#define CLIENT_H_

#define REQUEST_LEN  17
#define SERVER_HOST  "127.0.0.1"
#define SERVER_PORT  8080

#define SUCCESS       0
#define EINVLEN       1
#define EINVOP        2

void print_menu(void);
void read_input(uint8_t *, int64_t *, int64_t *);

#endif // CLIENT_H_

