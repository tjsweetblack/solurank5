# Mini Server - Study Guide

## Overview
This is a simple multi-client TCP chat server implementation using the `select()` system call for I/O multiplexing. The server allows multiple clients to connect simultaneously and broadcasts messages from one client to all other connected clients.

---

## Core Concepts

### 1. Socket Programming Fundamentals

#### TCP Sockets
- **Connection-oriented protocol**: Reliable, ordered delivery of data
- **Stream-based**: Data flows as a continuous stream of bytes
- **Three-way handshake**: Establishes connection before data transfer

#### Key System Calls
1. **socket()**: Creates a socket endpoint
   - `AF_INET`: IPv4 Internet protocols
   - `SOCK_STREAM`: TCP socket type

2. **bind()**: Assigns address (IP + port) to socket
   - Associates socket with a specific network interface and port

3. **listen()**: Marks socket as passive (ready to accept connections)
   - Backlog parameter: Maximum pending connections queue

4. **accept()**: Accepts incoming connection
   - Blocks until client connects
   - Returns new socket descriptor for the client

5. **recv()**: Receives data from socket
   - Returns number of bytes received
   - Returns 0 when connection is closed
   - Returns -1 on error

6. **send()**: Sends data to socket
   - Returns number of bytes sent

---

### 2. I/O Multiplexing with select()

#### Why select()?
- Monitor multiple file descriptors simultaneously
- Avoid blocking on a single client
- Handle multiple clients in a single thread

#### How select() Works
```
select(max_fd + 1, &readfds, &writefds, NULL, NULL)
```

**Parameters:**
- `max_fd + 1`: Highest file descriptor number + 1
- `readfds`: Set of file descriptors to monitor for reading
- `writefds`: Set of file descriptors to monitor for writing
- `NULL`: Exception set (not used)
- `NULL`: Timeout (NULL = wait indefinitely)

**Return Value:**
- Number of ready descriptors
- -1 on error

#### fd_set Macros
- `FD_ZERO(&set)`: Clear all descriptors from set
- `FD_SET(fd, &set)`: Add descriptor to set
- `FD_CLR(fd, &set)`: Remove descriptor from set
- `FD_ISSET(fd, &set)`: Test if descriptor is in set

---

### 3. Data Structures

#### Client Structure
```c
typedef struct s_client
{
    int id;              // Unique client identifier
    char msg[100000];    // Buffer for incomplete messages
} t_client;
```

#### Global State
- `clients[2048]`: Array to store all client data
- `activefds`: Master set of active file descriptors
- `readfds`: Working copy for reading
- `writefds`: Working copy for writing
- `max_fd`: Highest file descriptor number
- `increment_id`: Counter for assigning client IDs

---

## Program Flow

### 1. Server Initialization
```
1. Parse command line (port number)
2. Create server socket
3. Set up server address (127.0.0.1:port)
4. Bind socket to address
5. Listen for connections
6. Initialize fd_set with server socket
```

### 2. Main Event Loop
```
while (1):
    1. Copy activefds to readfds and writefds
    2. Call select() - blocks until activity
    3. Loop through all file descriptors (0 to max_fd)
    4. If fd is server socket → accept new client
    5. If fd is client socket → handle client data
```

### 3. New Client Connection
```
1. Accept connection (get new fd)
2. Update max_fd if needed
3. Add fd to activefds
4. Assign unique ID to client
5. Initialize client message buffer
6. Broadcast "client X just arrived" to all other clients
```

### 4. Client Message Handling
```
1. Receive data from client socket
2. If bytes <= 0 → disconnect client
3. Append data to client's message buffer
4. Scan for newline characters
5. When newline found:
   - Extract complete message
   - Broadcast "client X: message" to all other clients
   - Clear message buffer
```

### 5. Client Disconnection
```
1. Broadcast "client X just left"
2. Remove fd from activefds
3. Close socket
```

---

## Key Implementation Details

### Message Buffering
- Each client has a persistent buffer (msg array)
- Partial messages are accumulated until '\n' is received
- This handles cases where messages arrive in fragments

### Broadcasting
- Sends message to all clients EXCEPT sender
- Iterates through all file descriptors
- Checks if fd is in writefds (ready for writing)
- Uses send() to transmit message

### Error Handling
- Invalid arguments → exit with error message
- Socket/bind/listen failures → "Fatal error"
- recv() returns <=0 → assume client disconnected

### Network Byte Order
- `htonl()`: Host to network long (32-bit)
- `htons()`: Host to network short (16-bit)
- Ensures portability across different architectures

### Loopback Address
- `2130706433` = `127.0.0.1` in decimal
- Server only accepts local connections
- Useful for testing and development

---

## Common Pitfalls & Solutions

### 1. Modifying fd_set During select()
**Problem:** select() modifies the fd_set parameters
**Solution:** Maintain master set (activefds) and working copies (readfds, writefds)

### 2. Not Updating max_fd
**Problem:** New clients might not be monitored
**Solution:** Always update max_fd when accepting new clients

### 3. Buffer Overflow
**Problem:** Client msg buffer could overflow
**Solution:** Check bounds before appending (`start < sizeof(clients[fd].msg)`)

### 4. Incomplete Messages
**Problem:** TCP is stream-based, messages may arrive fragmented
**Solution:** Accumulate data in client buffer until delimiter ('\n') found

### 5. Breaking After Processing
**Problem:** The code breaks after processing each fd
**Solution:** This is intentional - after accept() or recv(), fd_sets are stale, need new select()

---

## Testing Scenarios

### Basic Functionality
1. Start server: `./miniserv 8080`
2. Connect with telnet: `telnet localhost 8080`
3. Observe "client 0 just arrived" message
4. Send message and see broadcast

### Multiple Clients
1. Open 3+ telnet connections
2. Send messages from different clients
3. Verify all clients receive broadcasts
4. Disconnect one client, verify "just left" message

### Edge Cases
1. Very long messages (test buffer limits)
2. Messages without newlines
3. Rapid connect/disconnect
4. Multiple messages in single recv()

---

## Learning Progression

### Level 1: Understanding
- Read and understand each function
- Trace program flow with 2 clients
- Understand select() operation

### Level 2: Analysis
- Identify potential buffer overflow scenarios
- Analyze what happens if send() fails
- Consider race conditions (though none exist here)

### Level 3: Enhancement Ideas
- Add maximum message length validation
- Handle send() failures gracefully
- Add server shutdown command
- Log messages to file
- Support custom usernames
- Handle partial sends

### Level 4: Advanced Topics
- Compare with poll() and epoll()
- Implement non-blocking sockets
- Add encryption (TLS/SSL)
- Implement private messaging
- Add authentication

---

## Memory Map

```
Server Socket [fd=3]
    ↓
activefds: {3}
max_fd: 3
    ↓
Client connects → fd=4
    ↓
activefds: {3, 4}
clients[4].id = 0
max_fd: 4
    ↓
Another client → fd=5
    ↓
activefds: {3, 4, 5}
clients[5].id = 1
max_fd: 5
```

---

## Quick Reference Commands

### Compile
```bash
gcc -Wall -Wextra -Werror miniserv.c -o miniserv
```

### Run
```bash
./miniserv 8080
```

### Test with telnet
```bash
telnet localhost 8080
```

### Test with netcat
```bash
nc localhost 8080
```

### Multiple clients
```bash
# Terminal 1
./miniserv 4242

# Terminal 2, 3, 4...
telnet localhost 4242
```

---

## Related Concepts to Study

1. **Network Programming**
   - TCP/IP stack
   - OSI model
   - IPv4 vs IPv6

2. **System Calls**
   - File descriptors
   - System call overhead
   - Blocking vs non-blocking I/O

3. **Concurrency Models**
   - Multi-threading
   - Multi-processing
   - Asynchronous I/O
   - Event-driven architecture

4. **Alternative Approaches**
   - poll() vs select()
   - epoll() (Linux-specific)
   - kqueue (BSD/macOS)
   - libuv / libevent libraries

---

## Summary

This mini server demonstrates:
- ✓ Socket creation and management
- ✓ I/O multiplexing with select()
- ✓ Multi-client handling
- ✓ Message broadcasting
- ✓ Proper client lifecycle management
- ✓ Buffer management for incomplete messages

It's an excellent foundation for understanding networked applications and serves as a stepping stone to more complex server architectures.
