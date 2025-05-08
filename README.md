<h2>Project Structure</h2>
<p>
There are two main programs:<br>
1. server.c - handles all client connections and message routing<br>
2. client.c - connects to the server and sends/receives messages<br>
</p>

<h2>Requirements</h2>
<p>
1. Create a chat server and client using socket().<br>
2. The server should receive and print messages from each of the connected clients and echo received messages back 
to all connected clients.<br>
3. The client should read from stdin and send messages to the server and be able to receive and print server responses.<br>
4. The server should be able to support multiple connected clients using multithreading.<br>
5. The server should keep track of all connected client sockets in a list or map.<br>
6. Add features like usernames. The client sends a name first to the server and all messages from that client will be displayed
with that name.<br>
7. The user should be able to update their usernames.<br>
8. The server should show when new users join/leave.<br>
9. The server should send messages when any clients get disconnected.<br>
</p>

<h2>Request Format</h2>
<p>
Instead of using HTTP, the application will use a custom length-fixed protocol with structured messages. The structure of the 
message is as follows: <br>
[4-byte length][payload]<br>
Here, the 4-byte length refers to the total number of bytes in the payload. The payload is further structed as:<br>
TYPE|USERNAME|MESSAGE<br>
The types of messages supported are:<br>
1. MSG - signifying messages sent by a connected client<br>
2. JOIN - signifying request to join a chat by a client<br>
3. LEAVE - signifying request to leave a chat by a connected client<br>
4. NAME - signifying changing the name of a connected client<br>
</p>

<h2>Response Format</h2>
<p>
Server responses will follow the following pattern:<br>
TYPE|STATUS|MESSAGE<br>
The types of responses supported are:<br>
1. ACK - used for simple acknowledgement of a request sent by a client<br>
2. ERR - used to indicate an error<br>
3. INFO - used to indicate system events/notifications<br>
</p>

<h2>Joining the Chat</h2>
<p>
When a new user joins the chat(a new client connects to the server), they first post a username. It must be unique:<br>
[0][JOIN|Alice\r\n]<br>
</p>

<h2>Sending Messages</h2>
<p>
Once the user has joined the chat, they can send messages:<br>
[12][MSG|Alice|Hello there!\r\n]<br>
</p>

<h2>Printing the Messages</h2>
<p>
Once the server receives the messages, it sends them back to all the connected clients:<br>
INFO|MESSAGE|Alice: Hello there!\r\n<br>
</p>