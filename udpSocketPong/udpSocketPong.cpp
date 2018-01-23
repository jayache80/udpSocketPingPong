#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

int main()
{
    SOCKET s;
    struct sockaddr_in si_me, si_other;
    int slen, recv_len;
    char buf[BUFLEN];
    WSADATA wsa;

    slen = sizeof(si_other);

    //Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");

    //Create a socket
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d", WSAGetLastError());
    }

    const char enable[1] = {1};
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, enable, sizeof(int)) < 0)
        puts("setsockopt(SO_REUSEADDR) failed");

    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    si_me.sin_family = AF_INET;
    si_me.sin_addr.s_addr = inet_addr("192.168.1.139"); //INADDR_ANY;
    si_me.sin_port = htons(PORT);

    //Bind
    if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("Bind done");

    //keep listening for data
    while (1)
    {
        printf("Waiting for data...");
        fflush(stdout);

        //clear the buffer by filling null, it might have previously received data
        memset(buf, '\0', BUFLEN);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
        {
            int error = WSAGetLastError();
            printf("recvfrom() failed with error code : %d", error);
            exit(EXIT_FAILURE);
        }

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n", buf);

        //now reply with the same data
        puts("Doing a pong...");
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        //sleep 1000ms to give the other application a chance to grab data
        //from 8888 before this one does
        Sleep(1000);
    }

    closesocket(s);
    WSACleanup();

    return 0;
}
