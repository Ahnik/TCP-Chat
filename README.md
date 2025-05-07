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
7. Add timestamps to messages.<br>
8. The server should show when new users join/leave.<br>
9. The server should send messages when any clients get disconnected.<br>
</p>

<h2>Joining the Chat</h2>
<p>
When a new user joins the chat(a new client connects to the server), they first post a username. It must be unique:<br>

</p>

<h2>Sending Messages</h2>
<p>
Once the user has joined the chat, they can send messages:<br>

</p>

<h2>Printing the Messages</h2>
<p>
Once the server receives the messages, it sends them back to all the connected clients:<br>

</p>