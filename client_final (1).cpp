/*MCA-IV SEMESTER ********UNIX NETWORKING PROGRAMMING ASSIGNMENT*****************************************
    SUBMITTED TO:Ms.Nisha Singh
Author:Priya Jain(22)
         Radha Verma(24)
************************CLIENT****************************************************************

Objective: To immplement chat server application using socket programming in linux..The model supports two-way communication across server
Approach: Select function call is used for interprocesss communication
*/

//necessary header files

#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <cstring>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

using namespace std;

#define PORT 8000
#define BufferLimit 1028

//send message from client to socket

int SendSocketMsg(int socket, char* username)
{
    int Read_Value;
    char Buffer[BufferLimit], msg[BufferLimit];

    Read_Value = read(STDIN_FILENO, Buffer, BufferLimit - 1);
    Buffer[Read_Value] = '\0';

    if (Read_Value == 0) 
    	return -1;

    strcpy(msg,username);
    strcat(msg, " : ");
    strcat(msg, Buffer);

    send(socket, msg , strlen(msg) , 0);
}

// receive message from server

int RecvSocketMsg(int socket, char* username)
{
    int Read_Value;
    char Buffer[BufferLimit] = {0};
    
    Read_Value = read(socket, Buffer, BufferLimit - 1);
    Buffer[BufferLimit] = '\0';

    if (Read_Value == 0) 
    	return -1;
    
    

    else 
    	cout<<Buffer;

    return 0;
}

int main(int argc, char* argv[])
{	
	char ip[50], username[50]={0}, password[50]= {0};
	fd_set ReadFd;
	int sockfd,option,ReadValue,SelectActivity;
	char RegisterProcess[] ="Register";  
	char LoginProcess[] ="Login";
	char LoginSuccess[] ="Authentication Successful";
	char RegistrationSuccess[] ="Registration Successful";
	char buffer[BufferLimit]; 
	struct sockaddr_in ServerAddress;
	setbuf(stdout, NULL);
	if (argc > 1) 
		strcpy(ip, argv[1]);
    else 
    {
        cout<<"Enter Server's IP Address: ";
        cin>>ip;
    }
    //create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
    	perror("Cannot Create Socket");
    	exit(-1);
    }
    int enable=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)) < 0)
        perror("Socket option Error");

    memset(&ServerAddress, '0', sizeof(ServerAddress));     
    ServerAddress.sin_family = AF_INET;                   
    ServerAddress.sin_port = htons(PORT);                  
    ServerAddress.sin_addr.s_addr = inet_addr(ip);
    
    if (connect(sockfd, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)) < 0)
    {
    	perror("Cannot Connect to server");
    	exit(-1);
    }
    while(1) 
    {
        memset(buffer, 0, BufferLimit);

        cout<<"*****************Welcome************************"<<endl;
        cout<<"Select an option to continue"<<endl;
        cout<<"****Register*****"<<"Enter 1"<<endl;
        cout<<"****Login*****"<<"Enter 2"<<endl;
        cout<<"****Leave*****"<<"Enter 3"<<endl;
		cin>>option;
		
    if(option==3)
		{	
			cout<<"You are about to go offline......";
			exit(0); //kill the process
		}

		if(option==1)
        {
				cout<<"Enter Username: \n";
        		cin>>username;
        		cout<<"Enter Password: \n";
        		cin>>password;
        		strcpy(buffer, RegisterProcess);
        }
        if(option==2)
        {
        		cout<<"Enter Username: \n";
        		cin>>username;
        		cout<<"Enter Password: \n";
        		cin>>password;
        		strcpy(buffer, LoginProcess);

        }

		strcat(buffer, username);
        strcat(buffer, " ");
        strcat(buffer, password);

        send(sockfd, buffer, strlen(buffer), 0);

        ReadValue= read(sockfd, buffer,BufferLimit);        
        buffer[ReadValue] = '\0';
       
        cout<<buffer<<endl;  

        if (strcmp(buffer,RegistrationSuccess) == 0 || strcmp(buffer, LoginSuccess) == 0)
        {   
            cout<<"Send message:"<<endl;
        	break;
        }
        else 
        {
        	cout<<"Invalid Username/Password...Try again";
	        continue;
	    }
           
	}	
	while(1)
	{
		FD_ZERO(&ReadFd);                
        FD_SET(STDIN_FILENO, &ReadFd);   // STDIN_FILENO -- is default standard input fd no. which 0
        FD_SET(sockfd, &ReadFd);           

        SelectActivity = select(sockfd + 1, &ReadFd, NULL, NULL, NULL);
        if (SelectActivity <= 0) 
			exit(EXIT_FAILURE);
		
		if (FD_ISSET(STDIN_FILENO, &ReadFd)) 
		{
            if (SendSocketMsg(sockfd, username) < 0) 
            {
                perror("Unexpected Error");
                exit(-1);
            }
         }
         if (FD_ISSET(sockfd, &ReadFd)) 
         {
            if (RecvSocketMsg(sockfd, username) < 0) 
            {
                perror("Connection Lost");
                exit(-1);
            }
        }  
    }
    return 0;
}
	
	
	   