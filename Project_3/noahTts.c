#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

int player_count = 0;
pthread_mutex_t mutexcount;

int movesPlayed = 0;
int lastIndex = 0;

void setMovesPlayed()
{
    movesPlayed = 0;
    lastIndex = 0;
}

void error(const char *msg)
{
    perror(msg);
    pthread_exit(NULL);
}

int convertBoardCords(int x, int y)
{
    if (x == 1 && y == 1)
    {
        return 0;
    }
    else if (x == 2 && y == 1)
    {
        return 1;
    }
    else if (x == 3 && y == 1)
    {
        return 2;
    }
    else if (x == 1 && y == 2)
    {
        return 3;
    }
    else if (x == 2 && y == 2)
    {
        return 4;
    }
    else if (x == 3 && y == 2)
    {
        return 5;
    }
    else if (x == 1 && y == 3)
    {
        return 6;
    }
    else if (x == 2 && y == 3)
    {
        return 7;
    }
    else if (x == 3 && y == 3)
    {
        return 8;
    }
    else
    {
        return -1; // invalid coordiantes
    }
}

// int sockfd = connect_to_server(argv[1], atoi(argv[2]));

//     int num = 0;
//     int bytes_sent = 0;
//     char *msg1 = malloc(sizeof(char) * 8);

//     // while (bytes_sent > 0)
//     // {
//     //     bytes_sent = read(sockfd, msg, 20);
//     // }

//     while (bytes_sent < 8 && (msg1[6] != '|' && msg1[7] != '|'))
//     {
//         bytes_sent += read(sockfd, msg1, 8);
//     }
//     int remain_bytes = atoi(&msg1[5]);
//     char *rest_of_msg = malloc(remain_bytes + 2);
//     int bytes_remaining = remain_bytes;
//     while (bytes_remaining > 0 && rest_of_msg[remain_bytes] != '|')
//     {
//         bytes_remaining -= read(sockfd, rest_of_msg, remain_bytes + 1); // try adding 1 after remain bytes
//     }
//     rest_of_msg[remain_bytes + 1] = '\0';
//     char *fullMsg = malloc(sizeof(msg1) + sizeof(rest_of_msg));
//     fullMsg = strdup(strcat(msg1, rest_of_msg));
//     printf("Full message is: %s\n", fullMsg);

// PLAN: Replace the recv_int with my own receive message, and get it to convert to an int at the end
// Before we do that, we can check if the message says something like "DRAW".
// Next, we can use the coordinate converter I made in the parse.c file
// We should be testing after every step, so as to make sure everything works smoothly

void write_client_int(int cli_sockfd, int msg)
{
    int n = write(cli_sockfd, &msg, sizeof(int));
    if (n < 0)
        error("ERROR writing int to client socket");
}

void write_client_msg(int cli_sockfd, char *msg)
{
    // int len = strlen(msg);
    // printf("current len is %d\n", len);
    // write(cli_sockfd, &len, sizeof(int));
    int n = write(cli_sockfd, msg, strlen(msg));
    if (n < 0)
        error("ERROR writing msg to client socket");
}

char *write_movd(int cli_sockfd, int pId, int index, char board[9])
{
    // pId will tell us if it was X or Y who just moved
    //  sample: MOVD|16|X|2,2|....X....|
    //  we'll have to use the global board to finish the message

    //     void draw_board(char board[9])
    // {
    //     for (int i = 0; i < 8; i++)
    //     {
    //         printf("%c", board[i]);
    //     }
    //     printf("\n");
    // }
    char *movd = malloc(sizeof(char) * 25); // TOTAL BYTES ISN't 16
    strcat(movd, "MOVE|16|");
    printf("%sand break\n", movd);
    if (pId == 0)
    {
        // add X
        strcat(movd, "X|");
    }
    else
    {
        strcat(movd, "O|");
    }
    if (index == 0)
    {
        strcat(movd, "1,1");
    }
    else if (index == 1)
    {
        strcat(movd, "2,1");
    }
    else if (index == 2)
    {
        strcat(movd, "3,1");
    }
    else if (index == 3)
    {
        strcat(movd, "1,2");
    }
    else if (index == 4)
    {
        strcat(movd, "2,2");
    }
    else if (index == 5)
    {
        strcat(movd, "3,2");
    }
    else if (index == 6)
    {
        strcat(movd, "1,3");
    }
    else if (index == 7)
    {
        strcat(movd, "2,3");
    }
    else if (index == 8)
    {
        strcat(movd, "3,3");
    }
    strcat(movd, "|");
    strcat(movd, board);
    strcat(movd, "|");
    movd[24] = '\0';
    return movd;
    // now we just need the write statement?
}

void write_clients_msg(int *cli_sockfd, char *msg)
{
    write_client_msg(cli_sockfd[0], msg);
    write_client_msg(cli_sockfd[1], msg);
}

void write_clients_int(int *cli_sockfd, int msg)
{
    write_client_int(cli_sockfd[0], msg);
    write_client_int(cli_sockfd[1], msg);
}

int setup_listener(int portno)
{
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening listener socket.");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR binding listener socket.");

#ifdef DEBUG
    printf("[DEBUG] Listener set.\n");
#endif

    return sockfd;
}

// this method converts the MOVE string to an int that represents the
// coordinate we are moving
// my goal is to change this so it doesn't just return coordiantes,
// but returns other ints representing commands like DRAW
int recv_int(int cli_sockfd)
{
    //-----------------ORIGINAL---------------------
    // int msg = 0;
    // int n = read(cli_sockfd, &msg, sizeof(int));

    // if (n < 0 || n != sizeof(int))
    //     return -1;

    // printf("[DEBUG] Received int: %d\n", msg);

    // return msg;
    //-----------------------------------------------
    int numBytes;
    signal(SIGPIPE, SIG_IGN);

    int sockfd = cli_sockfd;

    int num = 0;
    int bytes_sent = 0;
    char *msg1 = malloc(sizeof(char) * 7);

    while (bytes_sent < 8 && (msg1[6] != '|' && msg1[6] != '|'))
    {
        bytes_sent += read(sockfd, msg1, 7);
    }
    int remain_bytes = atoi(&msg1[5]);

    char *rest_of_msg = malloc(remain_bytes + 2);
    int bytes_remaining = remain_bytes;
    while (bytes_remaining > 0 && rest_of_msg[remain_bytes] != '|')
    {
        bytes_remaining -= read(sockfd, rest_of_msg, remain_bytes + 1); // try adding 1 after remain bytes
    }
    rest_of_msg[remain_bytes + 1] = '\0';
    char *fullMsg = malloc(sizeof(msg1) + sizeof(rest_of_msg));
    fullMsg = strdup(strcat(msg1, rest_of_msg));

    // this should only be used if the message is one that says MOVE

    movesPlayed++;
    char type = fullMsg[7];
    int xCord = atoi(&fullMsg[9]);
    int yCord = atoi(&fullMsg[11]);

    int indexOnBoard = convertBoardCords(xCord, yCord);
    if (indexOnBoard != -1)
    {
        lastIndex = indexOnBoard;
    }
    // check if this returns -1

    // we will either return the index on board, or we'll return other numbers that mean we are going to
    // send a message regarding a draw, or the game being over, or the

    // so, we could return -1 for invalid, regular nums for moves, and various numbers for an error message
    //  when should we call MOVD tho? -> wherever this code sends the updated board
    return indexOnBoard;
}

void get_clients(int lis_sockfd, int *cli_sockfd)
{
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

#ifdef DEBUG
    printf("[DEBUG] Listening for clients...\n");
#endif

    int num_conn = 0;
    while (num_conn < 2)
    {
        listen(lis_sockfd, 253 - player_count);

        memset(&cli_addr, 0, sizeof(cli_addr));

        clilen = sizeof(cli_addr);

        cli_sockfd[num_conn] = accept(lis_sockfd, (struct sockaddr *)&cli_addr, &clilen);

        if (cli_sockfd[num_conn] < 0)
            error("ERROR accepting a connection from a client.");

#ifdef DEBUG
        printf("[DEBUG] Accepted connection from client %d\n", num_conn);
#endif

        write(cli_sockfd[num_conn], &num_conn, sizeof(int));

#ifdef DEBUG
        printf("[DEBUG] Sent client %d it's ID.\n", num_conn);
#endif

        pthread_mutex_lock(&mutexcount);
        player_count++;
        printf("Number of players is now %d.\n", player_count);
        pthread_mutex_unlock(&mutexcount);

        if (num_conn == 0)
        {
            write_client_msg(cli_sockfd[0], "HLD");

#ifdef DEBUG
            printf("[DEBUG] Told client 0 to hold.\n");
#endif
        }

        num_conn++;
    }
}

int get_player_move(int cli_sockfd, char board[9])
{

    // original-----------------
    write_client_msg(cli_sockfd, "TRN");

    return recv_int(cli_sockfd);
}

int check_move(char board[9], int move, int player_id)
{
    if ((move == 9) || (board[move] == '.'))
    {

#ifdef DEBUG
        printf("[DEBUG] Player %d's move was valid.\n", player_id);
#endif

        return 1;
    }
    else
    {
#ifdef DEBUG
        printf("[DEBUG] Player %d's move was invalid.\n", player_id);
#endif

        return 0;
    }
}

void update_board(char board[9], int move, int player_id)
{
    board[move] = player_id ? 'X' : 'O';
}

void draw_board(char board[9])
{
    for (int i = 0; i < 8; i++)
    {
        printf("%c", board[i]);
    }
    printf("\n");
}

void send_update(int *cli_sockfd, int move, int player_id)
{
    write_clients_msg(cli_sockfd, "UPD");

    write_clients_int(cli_sockfd, player_id);

    write_clients_int(cli_sockfd, move);
}

// this checks if we won
int check_board(char board[9], int last_move)
{
    // return 1 if won, 0 if not

    if ((board[0] == 'X' && board[1] == 'X' && board[2] == 'X') || board[0] == 'O' && board[1] == 'O' && board[2] == 'O')
    {
        return 1;
    }
    if ((board[3] == 'X' && board[4] == 'X' && board[5] == 'X') || board[3] == 'O' && board[4] == 'O' && board[5] == 'O')
    {
        return 1;
    }
    if ((board[6] == 'X' && board[7] == 'X' && board[8] == 'X') || board[6] == 'O' && board[7] == 'O' && board[8] == 'O')
    {
        return 1;
    }
    if ((board[0] == 'X' && board[3] == 'X' && board[6] == 'X') || board[0] == 'O' && board[3] == 'O' && board[6] == 'O')
    {
        return 1;
    }
    if ((board[1] == 'X' && board[4] == 'X' && board[7] == 'X') || board[1] == 'O' && board[4] == 'O' && board[7] == 'O')
    {
        return 1;
    }
    if ((board[2] == 'X' && board[5] == 'X' && board[8] == 'X') || board[2] == 'O' && board[5] == 'O' && board[8] == 'O')
    {
        return 1;
    }
    if ((board[0] == 'X' && board[4] == 'X' && board[8] == 'X') || board[0] == 'O' && board[4] == 'O' && board[8] == 'O')
    {
        return 1;
    }
    if ((board[6] == 'X' && board[4] == 'X' && board[2] == 'X') || board[6] == 'O' && board[4] == 'O' && board[2] == 'O')
    {
        return 1;
    }

    return 0;
}

void *run_game(void *thread_data)
{
    int *cli_sockfd = (int *)thread_data;
    char board[9] = ".........";

    printf("Game on!\n");

    write_clients_msg(cli_sockfd, "SRT");

    draw_board(board);

    int prev_player_turn = 1;
    int player_turn = 0;
    int game_over = 0;
    int turn_count = 0;
    while (!game_over)
    {

        if (prev_player_turn != player_turn)
            write_client_msg(cli_sockfd[(player_turn + 1) % 2], "WAT");

        int valid = 0;
        int move = 0;
        while (!valid)
        {
            move = get_player_move(cli_sockfd[player_turn], board);
            if (move == -1)
                break;
            printf("Player %d played position %d\n", player_turn, move);

            valid = check_move(board, move, player_turn);
            if (!valid)
            {
                printf("Move was invalid. Let's try this again...\n");
                write_client_msg(cli_sockfd[player_turn], "INV");
            }
        }

        if (move == -1)
        {
            printf("Player disconnected.\n");
            break;
        }
        else
        {
            update_board(board, move, player_turn);
            send_update(cli_sockfd, move, player_turn);

            draw_board(board);

            game_over = check_board(board, move);

            if (game_over == 1)
            {
                write_client_msg(cli_sockfd[player_turn], "WIN");
                write_client_msg(cli_sockfd[(player_turn + 1) % 2], "LSE");
                printf("Player %d won.\n", player_turn);
            }
            else if (turn_count == 8)
            {
                printf("Draw.\n");
                write_clients_msg(cli_sockfd, "DRW");
                game_over = 1;
            }

            prev_player_turn = player_turn;
            player_turn = (player_turn + 1) % 2;
            turn_count++;
        }
    }

    printf("Game over.\n");

    close(cli_sockfd[0]);
    close(cli_sockfd[1]);

    pthread_mutex_lock(&mutexcount);
    player_count--;
    printf("Number of players is now %d.", player_count);
    player_count--;
    printf("Number of players is now %d.", player_count);
    pthread_mutex_unlock(&mutexcount);

    free(cli_sockfd);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    setMovesPlayed();
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    int lis_sockfd = setup_listener(atoi(argv[1]));
    pthread_mutex_init(&mutexcount, NULL);

    while (1)
    {
        if (player_count <= 252)
        {
            int *cli_sockfd = (int *)malloc(2 * sizeof(int));
            memset(cli_sockfd, 0, 2 * sizeof(int));

            get_clients(lis_sockfd, cli_sockfd);

            pthread_t thread;
            int result = pthread_create(&thread, NULL, run_game, (void *)cli_sockfd);
            if (result)
            {
                printf("Thread creation failed with return code %d\n", result);
                exit(-1);
            }

            printf("[DEBUG] New game thread started.\n");
        }
    }

    close(lis_sockfd);

    pthread_mutex_destroy(&mutexcount);
    pthread_exit(NULL);
}