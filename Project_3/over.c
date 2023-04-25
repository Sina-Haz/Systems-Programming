// char *create_over_msg(char *pName, int outcome)
// {
//     // we'll need the player's name, and the outocme
//     // length will be 4+1+1+1+16+strlen(name)

//     char message[strlen(pName)];
//     // OVER|26|W|Joe Smith has resigned.|

//     if (outcome == 3)
//     {
//         // draw proposed by player
//         char message[24 + strlen(pName)];
//         strcat(message, "OVER|");
//         sprintf(message + strlen(message), "%ld", strlen(pName) + 16);
//         strcat(message, "|W|");
//         strcat(message, pName);
//         strcat(message, " has resigned.|");
//     }
//     else if (outcome == 2)
//     {
//         // WIN
//         char message[24 + strlen(pName)];
//         strcat(message, "OVER|");
//         sprintf(message + strlen(message), "%ld", strlen(pName) + 7);
//         strcat(message, "|W|");
//         strcat(message, pName);
//         strcat(message, " wins.");
//     }
//     else if (outcome == 1)
//     {
//         // LOSE
//         char message[24 + strlen(pName)];
//         strcat(message, "OVER|");
//         sprintf(message + strlen(message), "%ld", strlen(pName) + 9);
//         strcat(message, "|L|You Lost|");
//     }
//     else
//     {
//         // TIE
//         char message[24 + strlen(pName)];
//         strcat(message, "OVER|");
//         sprintf(message + strlen(message), "%ld", strlen(pName) + 9);
//         strcat(message, "|D|You Tied|");
//     }

//     return message;
// }

// void handle_draw(int *cli_sockfd, int sender, int recipient)
// {
//     // cli_sockfd represents our int array of sockets
//     // sender is the socket index who's proposing the draw
//     // recipient is the socket index of who's receiving the draw requenst

//     // we'll call this method if recv_int returns the number 12 or something
//     // basically, this means we got the message DRAW|S|, so we must now send DRAW|S|
//     // onto the other socket and await a reply, and handle accordingly

//     int sent_bytes = 0;
//     while (sent_bytes < 8)
//     {
//         sent_bytes += write(cli_sockfd[recipient], "DRAW|3|S|", 10);
//     }

//     // now we have sent the suggestion and need to await a reply and handle accordingly
//     int bytes_received = 0;
//     char answer[8];
//     char *over = malloc(sizeof(char) * 256);

//     // player names will come from the play message

//     while (bytes_received < 8)
//     {
//         bytes_received += read(cli_sockfd[sender], answer, 8);
//     }
//     if (answer == "DRAW|3|A|")
//     {
//         // send out over to both
//         // over has 2 fields, the outcome: W, L, or D, and the reason
//         // example: OVER|26|W|Joe Smith has resigned.|
//         // we'll create a method for formulating the over message

//         over = create_over_msg(p2Name, 3);
//         int x = 0;
//         while (x < strlen(over))
//         {
//             x += write(cli_sockfd[recipient], over, strlen(over));
//         }
//         // over = create_over_msg(pname, outcome)
//         // send that over?
//         // outcome for DRAW|A| would be 3 I think
//     }
//     else
//     {
//         int x = 0;
//         while (x < 10)
//         {
//             x += write(cli_sockfd[recipient], "DRAW|3|R|", 10);
//         }
//     }
// }