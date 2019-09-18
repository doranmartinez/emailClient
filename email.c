#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void sender(char uemail[50], char netid[6], char msg[1024]);
void receiver(char netid[6], char pw[20]);

int main() {
    
    char  choice[1], uemail[50], netid[6], msg[1024], body[256], lineno[5], pw[20];
    int i = 1, line;

    printf("\n*** Email Client Menu ***\n\n");
    
    // Gather user's University Email Info for Full Send
    printf("\nEnter your email address:\n");
    scanf("%s", uemail);
    printf("\nEnter your netID:\n");
    scanf("%s", netid);
    printf("\nEnter your password:\n");
    scanf("%s", pw);

    while (1) {

        printf("\nInput (1) to Send.\n");
        printf("Input (2) to Receive.\n");
        scanf("%s", choice);

        fgets(msg, sizeof(msg), stdin); // Flushes stdin for fgets()

        if (strcmp(choice, "1") == 0) { // strcmp() compares s1 with s2. Outputs 0 when equal.

            printf("\nWrite email body with a period at the end on a single line:\n");

            while (1) {
                printf("Line %i:", i);
                fgets(body, sizeof(body), stdin);
                strcat(msg, body);

                if (strcmp(body, ".\n") == 0)
                    break;
                
                i++;
            }

            sender(uemail, netid, msg);
        }

        else if (strcmp(choice, "2") == 0)
            receiver(netid, pw);

        memset(choice, 0, strlen(choice));

        printf("\nExit? (y/n):\n");
        scanf("%s", choice);

        if (strcmp(choice, "y") == 0)
            break;
    }
}

void sender(char uemail[50], char netid[6], char msg[1024]) {

    int sock;
    struct sockaddr_in source, dest;
    struct hostent *hp, *gethostbyname();

    // Creates a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        perror("Opening stream socket");
        exit(1);
    }

    // get host address
    dest.sin_family = AF_INET;
    dest.sin_port = htons(25);

    hp = gethostbyname("hopper.csueastbay.edu");
    if (hp == 0)
        printf("hostname\n");

    bcopy(hp -> h_addr, &dest.sin_addr, hp -> h_length);

    // declare values
    char rec[1024];
    char eml[100];
    char neti[100];

    bzero(eml, sizeof(eml));
    bzero(neti, sizeof(neti));

    // connects the socket to the server.
    if (connect(sock, (struct sockaddr *)&dest, sizeof(dest)) < 0)
        printf("error connecting\n");

    //repeated user input data and then formats to SMTP command
    strcpy(eml, "MAIL FROM:<");
    strcat(eml, uemail);
    strcat(eml, ">\r\n");

    //repeat destination data and then format
    strcpy(neti, "RCPT TO:<");
    strcat(neti, netid);
    strcat(neti, ">\r\n");

    // read connection ack
    int n = read(sock, rec, sizeof(rec));
    printf("%s\n", rec);
    memset(rec, 0, strlen(rec));

    // send MAILFROM to server
    int m = write(sock, eml, strlen(eml));

    // receive ack
    n = read(sock, rec, sizeof(rec));
    printf("recv;%s\n", rec);
    memset(rec, 0, strlen(rec));

    // send RCPTTO to server
    m = write (sock, neti, strlen(neti));

    // receive ack
    n = read(sock, rec, sizeof(rec));
    printf("recv:%s\n", rec);
    memset(rec, 0, strlen(rec));

    // data
    char dat[] = "DATA\r\n";
    m = write(sock, dat, strlen(dat));
    n = read(sock, rec, sizeof(rec));
    printf("recv:%s\n", rec);
    memset(rec, 0, strlen(rec));

    // email content
    printf("email content:%s\n", msg);
    m = write(sock, msg, strlen(msg));
    n = read(sock, rec, sizeof(rec));
    printf("recv:%s\n", rec);
    close(sock);
}

void receiver(char netid[6], char pw[20]) {

    int sock;
    struct sockaddr_in source, dest;
    struct hostent *hp, *gethostbyname();

    // Creates a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        perror("Opening stream socket");
        exit(1);
    }

    // get host address
    dest.sin_family = AF_INET;
    dest.sin_port = htons(110);

    hp = gethostbyname("hopper.csueastbay.edu");
    if (hp == 0)
        printf("hostname\n");

    bcopy(hp -> h_addr, &dest.sin_addr, hp -> h_length);

    // declare variables
    char u[12];
    char pas[30], msg[7] = "RETR ";
    char rec[1024], list[6] = "LIST\r\n";
    bzero(u, sizeof(u));
    bzero(pas, sizeof(pas));

    // connects the socket to the server
    if (connect(sock, (struct sockaddr *)&dest, sizeof(dest)) < 0)
        printf("error connection\n");

    // format username
    strcpy(u, "USER ");
    strcat(u, netid);
    strcat(u, "\r\n");

    // read connection ack
    int n = read(sock, rec, sizeof(rec));
    printf("%s\n", rec);
    memset(rec, 0, strlen(rec));

    //enter username
    int m = write(sock, u, strlen(u));

    // receive ack
    n = read(sock, rec, sizeof(rec));
    printf("recv:%s\n", rec);
    memset(rec, 0, strlen(rec));

    // enter password
    m = write(sock, pas, strlen(pas));

    // receive ack
    n = read(sock, rec, sizeof(rec));
    printf("recv:%s\n", rec);
    memset(rec, 0, strlen(rec));

    // list of messages
    m = write(sock, list, strlen(list));

    //get list
    n = read(sock, rec, sizeof(rec));
    printf("recv:%s\n", rec);
    memset(rec, 0, strlen(rec));

    char resp[2];
    // email to display
    printf("Which email to read: \n");
    scanf("%s", resp);
    strcat(msg, resp);
    strcat(msg, "\r\n");

    //enter command
    m = write(sock, msg, strlen(msg));

    n = read(sock, rec, sizeof(rec));
    printf("%s\n", rec);
    close(sock);
}