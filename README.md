# Chat-Server
Chat server implementation that facilitates two-way communication with peer to peer message sending.

The chat server project let's you send messages from client-client. The project must not be mistaken as a broadcast implementation.It allows to send messages from one client to another client via server.
The client must send message seperated with a comma "," to indicate the client username to whom the message should be sent.The client is required to register itself when the server starts.During the lifetime of server a number of clients can register itself.The client can also login with the server with the existing combination of username and password only after registering with the server.The server maintains the dtatabse of client dynamicaly i.e,the deatils of clients registered is maintained only during the lifetime of server active.

Only the client's username indicated after comma will receive the message sent by the sender client.multiple clients can interact simultaneously allowing sending messages from one peer client to another client.

A structure of client information is maintained for storing username,password and socket descriptors to facilitate the communication process...

