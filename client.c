#include <service.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

// --------------------------------------------------------------------

#define DEFAULT_PORT 8995
#define MAX_PARALLEL_CONNECTIONS 16
#define MAX_INPUT_LENGTH 2 * 1024

// --------------------------------------------------------------------


int make_connection(int port)
{
    int sock;
    struct sockaddr_in server;
    
   //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        exit(1);
    }
    return sock;
}

// --------------------------------------------------------------------

void send_receive(int sock, char* input, char* output) 
{
    char server_reply[MAX_INPUT_LENGTH];
    // add the \n
    strcat(input, "\n");
        
    //Send some data
    if( send(sock , input , strlen(input) , 0) < 0)
    {
        puts("Send failed");
        exit(1);
    }

    //Receive a reply from the server
    if( recv(sock , output , 2000 , 0) < 0)
    {
        puts("recv failed");
        exit(1);
    }

    //  puts("Server reply :");
    // puts(server_reply);   
}

char * strrev(char *str)
{
    int i = strlen(str)-1,j=0;
    char ch;
    while(i>j)
    {
        ch = str[i];
        str[i]= str[j];
        str[j] = ch;
        i--;
        j++;
    }
    return str;
}
/**
 * Check if the answer from the reverse server ist the reversed input string,
 * 
 * @param input  the input for the reverse server
 * @param reverse_answer the answer from the reverse server
 * @return 0 (false) if the output is equal the reverse string input, else <> 0 (true)
 */
int check_reverse(char* input, char* reverse_answer)
{
    char myReverse[MAX_INPUT_LENGTH];
    int result = 0;
    unsigned inPos = 0;
    strncpy(myReverse, input, strlen(input)-1);
    strrev(myReverse);
    strcat(myReverse, "\n\0");
    // Not sure about the missing \0 from the reverse server.
    result = strncmp(myReverse, reverse_answer, strlen(myReverse));
    return(result);
}
/**
 * Send an message string to the reverse server and check if the result is the
 * reversed input. 
 * 
 * @param message string we send to the reverse server.
 */
void send_and_compare(int sock, char* message)
{
    char server_reply [MAX_INPUT_LENGTH];
    
    send_receive(sock, message, server_reply);
    strlen(server_reply);
        
    if (check_reverse(message, server_reply))
    {
       puts("server_reply is not a reverse String from message!"); 
       puts("message :");
       puts(message);       
       puts("Server reply :");
       puts(server_reply); 
    }    
}

char * generate_testdata (int len)
{
    char *p;
    char *pattern = "ABCDEFGHIJKLMNOPQ";
    unsigned j = 0;
    p = (char *) malloc(2048);
    memset(p, 0, 2048);
    for (unsigned i=0; i < len; ++i)
    {
        p[i] = pattern[j];
        if(j < strlen(pattern)-1)
        {
            j++;
        }
        else 
        {
            j = 0;
        }
    }
    return(p);
}

int main(int argc, char** argv)
{
    fprintf(stdout,
            "client - connect a server to transfer input and receive that input reversed ...\n\n"
            "syntax: client input [port] \n"
            "\tport - change listening prot (default: %d)\n",
            DEFAULT_PORT);

    unsigned short port = DEFAULT_PORT;
    
    int sock;
    struct sockaddr_in server;
    char message[MAX_INPUT_LENGTH], inputStr[MAX_INPUT_LENGTH]; 

    
    if (argc >= 2)
    {
        if(strlen(argv[1]) >= MAX_INPUT_LENGTH) 
        {
            fprintf(stderr, "inputStr is greater as %d\n", MAX_INPUT_LENGTH);
        } 
        else
        {
            strcpy(inputStr,argv[1]);
        }        
    }
    
    if (argc >= 3)
    {
        port = strtol(argv[2], 0, 10);
    }

    fprintf(stdout, "inputStr %s\n", inputStr);
    fprintf(stdout, "running on port %d\n", port);
    
    
    //Create socket
    sock = make_connection(port);


//keep communicating with server       
   // printf("Send message : ");        
   // scanf("%s" , message);
    char *testMessage;
    testMessage = generate_testdata(8);
    puts("Test with 8 bytes");
    send_and_compare(sock, testMessage);
    free(testMessage);
    
    testMessage = generate_testdata(510);
    puts("Test with 510 bytes");
    send_and_compare(sock, testMessage);
    free(testMessage);
  
    testMessage = generate_testdata(512);
    puts("Test with 512 bytes");
    send_and_compare(sock, testMessage);
    free(testMessage);
    
    testMessage = generate_testdata(513);
    puts("Test with 513 bytes");
    send_and_compare(sock, testMessage);
    free(testMessage);
    
    
    close(sock);

    /*
    service_t* service = create_service(port, MAX_PARALLEL_CONNECTIONS);

    if (!service)
    {
        fprintf(stderr, "cannot create service, exiting ...\n");
        return 0;
    }

    service_run(service);

    destroy_service(service);
     * */

    return 0;
}
