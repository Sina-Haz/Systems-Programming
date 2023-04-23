#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define h_addr h_addr_list[0]

void error(const char *msg)
{
    perror(msg);
    printf("Either the server shut down or the other player disconnected.\nGame over.\n");

    exit(0);
}

void recv_msg(int sockfd, char *msg)
{
    // ./a.out plastic.cs.rutgers.edu 15000
    // this is reading too many bytes

    memset(msg, 0, 4);
    int n = read(sockfd, msg, 3);
    // not equipped to read variable length functions
    printf("DEBUGGGING: MESSAGE = %s\n", msg);
    printf("[DEBUG] Received message: %s\n", msg);
}

void draw_board(char board[9])
{
    for (int i = 0; i < 8; i++)
    {
        printf("%c", board[i]);
    }
    printf("\n");
}

int recv_int(int sockfd)
{
    int msg = 0;
    int n = read(sockfd, &msg, sizeof(int));

    if (n < 0 || n != sizeof(int))
        error("ERROR reading int from server socket");

    printf("[DEBUG] Received int: %d\n", msg);

    return msg;
}

void write_server_int(int sockfd, int msg)
{
    int n = write(sockfd, &msg, sizeof(int));
    if (n < 0)
    {
        error("ERROR writing int to server socket");
    }

    printf("[DEBUG] Wrote int to server: %d\n", msg);
}

int connect_to_server(char *hostname, int portno)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket for server.");

    server = gethostbyname(hostname);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    memmove(server->h_addr, &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting to server");

    printf("[DEBUG] Connected to server.\n");
    return sockfd;
}

void take_turn(int sockfd)
{
    char buffer[256];
    int max_len = 256;

    while (1)
    {
        // Could I send over a length?
        printf("Enter your move: ");
        fgets(buffer, 256, stdin);
        write(sockfd, buffer, strlen(buffer));
        break;
    }
}

void get_update(int sockfd, char board[9])
{

    int player_id = recv_int(sockfd);
    int move = recv_int(sockfd);
    board[move] = player_id ? 'X' : 'O';
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    int sockfd = connect_to_server(argv[1], atoi(argv[2]));

    int id = recv_int(sockfd);

#ifdef DEBUG
    printf("[DEBUG] Client ID: %d\n", id);
#endif

    char msg[4]; // this was originally 4
    // ./a.out plastic.cs.rutgers.edu 14999
    char board[9] = ".........";

    printf("Tic-Tac-Toe\n------------\n");

    do
    {
        recv_msg(sockfd, msg);
        if (!strcmp(msg, "HLD"))
            printf("Waiting for a second player...\n");
    } while (strcmp(msg, "SRT"));

    /* The game has begun. */
    printf("Game on!\n");
    printf("Your are %c's\n", id ? 'X' : 'O');

    draw_board(board);

    while (1)
    {
        recv_msg(sockfd, msg);

        if (!strcmp(msg, "TRN"))
        {
            printf("%s\n", msg);
            take_turn(sockfd);
        }
        else if (!strcmp(msg, "INV"))
        {
            printf("That position has already been played. Try again.\n");
        }
        else if (!strcmp(msg, "CNT"))
        {
            int num_players = recv_int(sockfd);
            printf("There are currently %d active players.\n", num_players);
        }
        else if (!strcmp(msg, "UPD"))
        {
            get_update(sockfd, board);
            draw_board(board);
        }
        else if (!strcmp(msg, "WAT"))
        {
            printf("Waiting for other players move...\n");
        }
        else if (!strcmp(msg, "WIN"))
        {
            printf("You win!\n");
            break;
        }
        else if (!strcmp(msg, "LSE"))
        {
            printf("You lost.\n");
            break;
        }
        else if (!strcmp(msg, "DRW"))
        {
            printf("Draw.\n");
            break;
        }
        else
            error("Unknown message.");
    }

    printf("Game over.\n");
    close(sockfd);
    return 0;
}