/*MCA-IV SEMESTER ********UNIX NETWORKING PROGRAMMING ASSIGNMENT*****************************************
    SUBMITTED TO:Ms.Nisha Singh
Author:Priya Jain(22)
         Radha Verma(24)
************************SERVER****************************************************************

Objective: To immplement chat server application using socket programming in linux..The model supports two-way communication across server
Approach: Select function call is used for interprocesss communication
*/

//necessary header files

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdio>
#include <sys/socket.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <vector>

using namespace std;

#define BufferSize 1024
#define Port 8000
#define ClientLimit 25

struct Client
{
    char username[50];
    char password[50];
    int clientfd;   
} client[1000];

//send message from socket to desired client

int SendMessage(char (&msg)[BufferSize], int clientsocket[])  
{
    char *temp1, *temp2, msg_client[BufferSize] , client_username[50];
    int recvclientfd , flag=0;
    char delim[]= ",";

    temp1= strtok(msg , delim);
    
    while((temp1!=NULL) || (flag==0))
    {
        if(flag==0)
        {
            strcpy(msg_client,temp1);
        }
        else
        {
            strcpy(client_username,temp1);
        }
        
        if(temp1==NULL)
            break;
        
        temp1 = strtok(NULL, delim);
        
        flag=1;
    }
    
    
    for (int i = 0; i < ClientLimit; i++)
    {
        if (strncmp((client[i].username), client_username, strlen(client[i].username)-1) == 0)
        {
             recvclientfd=client[i].clientfd;
             break; 
        }
    }
        if (recvclientfd > 0)
        {
            for(int i=0;i<ClientLimit;i++)
            {   
                if(clientsocket[i]==recvclientfd)
                {
                    send(recvclientfd, msg_client, strlen(msg_client), 0);
                    break;
                }
            }
        }
            
        else
            return -1;

return 0;    
}

//check the validity of the user

int AuthenticateClient(char* username, char* password, struct Client client[])
{
   
    for (int i = 0; i < ClientLimit; i++)
    {
        if (strcmp((client[i].username), username) == 0 && strcmp((client[i].password), password) == 0)
        	return 0; 
    }
    return -1;
}
  
//add the valid user into the OnlineClientFdArray

int AddClient(int OnlineClientFdArray[], int sock)
{
  
    for (int i = 0; i < ClientLimit; i++)
    {
        if (OnlineClientFdArray[i] == 0)
        {
            OnlineClientFdArray[i] = sock;
            break;
        }
    }
}

//remove socket

int RemoveClient(int OnlineClientFdArray[], int sock)
{
    
    for (int i = 0; i < ClientLimit; i++)
    {
        if (OnlineClientFdArray[i] == sock)
        {
            OnlineClientFdArray[i] = 0;
            break;
        }
    }
}

int GetClientDetails(char* buffer, char* username, char* password)
{
    char* temp;
    if ((temp = strtok(buffer," ")) == NULL)
    	return -1;
    strcpy(username, temp);

    if ((temp = strtok(NULL," ")) == NULL)
    	return -1;
    strcpy(password, temp);

    return 0; 
}

// add new user to database after checking that username is already exist or not

int LoginArrayAdd(int sockfd,char* username, char* password, struct Client client[])
{
    for (int i = 0; i < ClientLimit; i++)
     	if (strcmp((client[i].username), username) == 0)
     	{
     		cout<<"The username already exists\n";
     		return -1; 
     	}
    for (int i = 0; i < ClientLimit; i++)
	{
        if ( *(client[i].username) == '\0')
        {
            strcpy((client[i].username), username);
            strcpy((client[i].password), password);
            client[i].clientfd=sockfd ;  

            return 0; 
        }
    }
    return -1;
}


int main()
{
    string localhost = "127.0.0.1";
	int ServerFd , MaxSd , sd , activity , NewSockFd, i , ReadCount;
	
    setbuf(stdout, NULL);
	
    struct sockaddr_in ServerAddr;
	struct sockaddr_in ClientAddr;
	char Buffer[BufferSize];
	int ClientFdArray[ClientLimit] = {0};
	int OnlineClientFdArray[ClientLimit] = {0};
	int enable=1;
	fd_set ReadFd;
	socklen_t clilen;
	
    clilen=sizeof(ClientAddr);
	
    memset(&ClientAddr, 0, sizeof(ClientAddr));
	
    char username[50] , password[50];
	char Welcome[50] , Register[50] , Login[50] , LoginFailed[50] , RegisterFailed[50] , LoginSuccess[50] , RegisterSucess[50],MessageFailed[50] ;
	
    strcpy(Welcome,"Welcome to the chat room");
    strcpy (Register,"Register");
    strcpy (Login,"Login");
    strcpy (LoginFailed,"Authentication Failed");
    strcpy (RegisterFailed,"Registration Failed");
    strcpy (LoginSuccess,"Authentication Successful");
    strcpy (RegisterSucess,"Registration Successful");
    strcpy (MessageFailed,"Message Sending Failed");

    //create socket

	if ((ServerFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
		perror("Socket Error");

	if (setsockopt(ServerFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)) < 0)
		perror("Socket option Error");
	
    ServerAddr.sin_family=AF_INET;  //hostbyte order
	ServerAddr.sin_port=htons(Port);//short network byte ordred;
	ServerAddr.sin_addr.s_addr = inet_addr(localhost.c_str());
    
    memset(&(ServerAddr.sin_zero),0,8); //memset allows max portability

	if (bind(ServerFd, (struct sockaddr *)&ServerAddr, sizeof(struct sockaddr))<0)
		perror("Bind Error");
	
    if (listen(ServerFd, 5) < 0)
		perror("Listen Error");
    
    cout<<"Server started: \n";
	
    while(1)
	{
		memset(Buffer, 0, BufferSize);
        FD_ZERO(&ReadFd);
        FD_SET(ServerFd, &ReadFd);
        MaxSd = ServerFd;

        for (i = 0 ; i < ClientLimit ; i++)
        {
            sd = ClientFdArray[i];
            if(sd > 0) 
            	FD_SET(sd , &ReadFd);
            if(sd > MaxSd)
            	MaxSd= sd;
        }

        activity = select(MaxSd + 1 , &ReadFd , NULL , NULL , NULL);

        if ((activity < 0) && (errno != EINTR))
        	perror("Select Error\n");

        if (FD_ISSET(ServerFd, &ReadFd))   
        {
        	if ((NewSockFd = accept(ServerFd, (struct sockaddr *)&ClientAddr, &clilen))<0)
        		perror("Accept Error");
        	else
        	{
        		cout<<"Connection has been established with"<<inet_ntoa(ClientAddr.sin_addr)<<ntohs(ClientAddr.sin_port)<<"with socket_fd "<<NewSockFd;
                for (i = 0; i < ClientLimit; i++)
                {
                    if(ClientFdArray[i] == 0)
                    {
                        ClientFdArray[i] = NewSockFd;
                        cout<<"Adding to list of sockets "<<i<<endl;                        
                        break;
                    }
                }
        	}
        }

        for (i = 0; i < ClientLimit; i++)
        {
        	sd = ClientFdArray[i];

            if (FD_ISSET(sd, &ReadFd))
            {
                if ((ReadCount = read(sd, Buffer, BufferSize)) == 0)
                {
                    getpeername(sd, (struct sockaddr*)&ClientAddr, &clilen);
                    cout<<"Host disconnected "<<inet_ntoa(ClientAddr.sin_addr)<<ntohs(ClientAddr.sin_port);
                    close(sd);
                    ClientFdArray[i] = 0;
                    RemoveClient(OnlineClientFdArray, sd);
                }

                else
                {   cout<<Buffer<<endl;
                	if (strncmp(Buffer, Register, strlen(Register) - 1 ) == 0)
                	{
                		if (GetClientDetails(Buffer + strlen(Register), username, password) != 0)
                			send(sd,RegisterFailed, strlen(RegisterFailed), 0);

                		if (LoginArrayAdd(sd,username, password, client) != 0) 
                				send(sd, RegisterFailed, strlen(RegisterFailed), 0);

                		else
                		{
                            AddClient(OnlineClientFdArray, sd);
                            send(sd, RegisterSucess, strlen(RegisterSucess), 0);

                        }
					}

					else if (strncmp(Buffer, Login, strlen(Login) - 1) == 0)
					{
						if (GetClientDetails(Buffer + strlen(Login), username, password) != 0)
							send(sd, LoginFailed, strlen(LoginFailed), 0);
						if (AuthenticateClient(username, password, client) != 0)
							send(sd,LoginFailed, strlen(LoginFailed), 0);
                        if (LoginArrayAdd(sd,username, password, client) != 0) 
                            send(sd, RegisterFailed, strlen(RegisterFailed), 0);
						else
						{
                            AddClient(OnlineClientFdArray, sd);
                            send(sd, LoginSuccess, strlen(LoginSuccess), 0);
                            send(NewSockFd , Welcome , strlen(Welcome) , 0);

                        }
					}

					else
					{
						if(SendMessage(Buffer, OnlineClientFdArray)!=0)
                            send(sd,MessageFailed,strlen(MessageFailed),0);
					}
            }    }
        }
	}
	return 0;
}


