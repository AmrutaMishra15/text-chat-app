# Text Chat Application
This project is a command-line chat application developed using C++ and TCP socket programming, implementing a client-server architecture.
It supports multiple concurrent clients, secure login, message exchange, broadcast/unicast messaging, user blocking, and message buffering for offline clients. 

#Key Features
1. Client-Server Architecture: One server facilitates communication between up to 4 active clients.
2. Secure Login: Clients authenticate with the server before sending/receiving messages.
3. Messaging: a. Unicast --> send direct message to specific client
              b. Broadcast -->Send messages to all currently logged-in clients.
              c. Buffer Message delivery --> stores messages for offline clients and delivers upon login.
4. All messages go through the server; no direct client-to-client communication.
5. Server tracks message counts and login status for every client.
6. Blocking & Unblocking --> Block messages from specific clients. Also Server enforces message filtering based on client block lists.
7. Efficient IP/port management.
8. Other features included --> Displaying logged-in clients. provide feature to logout from server, etc.,

