# Mini Server - Cheat Sheet

## Quick Overview
Multi-client TCP chat server using `select()` for I/O multiplexing. Broadcasts messages from one client to all others.

---

## Core Functions at a Glance

### ft_putstr_fd(msg)
```c
void ft_putstr_fd(char *msg)
```
- Writes error message to STDERR
- Exits program with status 1
- Used for fatal errors

### ft_broadcast(sender_fd)
```c
void ft_broadcast(int sender_fd)
```
- Sends `buffwrite` to all clients except sender
- Iterates through all fds in `writefds`
- Skips `sender_fd`

### ft_disconnect_client(fd)
```c
void ft_disconnect_client(int fd)
```
- Broadcasts "client X just left" message
- Removes fd from `activefds`
- Closes socket

### ft_get_new_client(serverfd)
```c
void ft_get_new_client(int serverfd)
```
- Accepts new connection
- Updates `max_fd` if needed
- Adds to `activefds`
- Assigns unique ID
- Broadcasts "client X just arrived"

### ft_response(fd)
```c
void ft_response(int fd)
```
- Receives data from client
- Disconnects if recv <= 0
- Buffers data until '\n'
- Broadcasts complete messages

---

## Data Structures

### Client Structure
```c
typedef struct s_client {
    int id;              // Unique identifier
    char msg[100000];    // Message buffer
} t_client;
```

### Global Variables
| Variable | Type | Purpose |
|----------|------|---------|
| `clients[2048]` | t_client array | Store all client data |
| `activefds` | fd_set | Master set of active fds |
| `readfds` | fd_set | Working copy for reading |
| `writefds` | fd_set | Working copy for writing |
| `max_fd` | int | Highest fd number |
| `increment_id` | int | Client ID counter |
| `buffread[100000]` | char array | Temporary read buffer |
| `buffwrite[100000]` | char array | Temporary write buffer |

---

## Socket System Calls

### Creating & Binding
```c
// Create socket
int fd = socket(AF_INET, SOCK_STREAM, 0);

// Set up address
struct sockaddr_in addr;
memset(&addr, 0, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
addr.sin_port = htons(port);

// Bind and listen
bind(fd, (struct sockaddr *)&addr, sizeof(addr));
listen(fd, 10);
```

### Accepting & I/O
```c
// Accept connection
int client_fd = accept(server_fd, NULL, NULL);

// Receive data
int bytes = recv(fd, buffer, size, 0);

// Send data
send(fd, buffer, length, 0);

// Close connection
close(fd);
```

---

## fd_set Operations

| Macro | Description |
|-------|-------------|
| `FD_ZERO(&set)` | Clear all fds from set |
| `FD_SET(fd, &set)` | Add fd to set |
| `FD_CLR(fd, &set)` | Remove fd from set |
| `FD_ISSET(fd, &set)` | Check if fd is in set |

### select() Usage
```c
readfds = activefds;    // Create working copy
writefds = activefds;   // Create working copy

select(max_fd + 1, &readfds, &writefds, NULL, NULL);

// Check which fds are ready
if (FD_ISSET(fd, &readfds)) {
    // fd is ready for reading
}
```

---

## Program Flow

### 1. Initialization
```
1. Check argc == 2
2. Create socket
3. Setup address (127.0.0.1:port)
4. bind() + listen()
5. Initialize fd_set
```

### 2. Main Loop
```
while (1) {
    1. readfds = writefds = activefds
    2. select(max_fd + 1, &readfds, &writefds, ...)
    3. for fd in 0..max_fd:
        if fd == serverfd → accept new client
        else → handle client message
}
```

### 3. Message Flow
```
Client sends data
    ↓
recv() in ft_response()
    ↓
Append to clients[fd].msg
    ↓
Find '\n' delimiter
    ↓
Format to buffwrite
    ↓
ft_broadcast() to all others
```

---

## Key Algorithms

### Message Buffering
```c
start = strlen(clients[fd].msg);
for (int i = 0; i < bytes; i++, start++) {
    clients[fd].msg[start] = buffread[i];
    if (clients[fd].msg[start] == '\n') {
        clients[fd].msg[start] = '\0';
        // Broadcast message
        memset(&clients[fd].msg, 0, sizeof(...));
        start = -1;  // Reset buffer
    }
}
```

### Broadcasting
```c
for (int fd = 0; fd <= max_fd; fd++) {
    if ((fd != sender_fd) && FD_ISSET(fd, &writefds)) {
        send(fd, buffwrite, strlen(buffwrite), 0);
    }
}
```

---

## Network Byte Order Functions

| Function | Meaning | Converts |
|----------|---------|----------|
| `htons()` | Host to network short | 16-bit host → network |
| `htonl()` | Host to network long | 32-bit host → network |
| `ntohs()` | Network to host short | 16-bit network → host |
| `ntohl()` | Network to host long | 32-bit network → host |

**Why needed?** Different CPUs store multi-byte values differently (endianness).

---

## Common Constants

| Constant | Value | Meaning |
|----------|-------|---------|
| `AF_INET` | 2 | IPv4 address family |
| `SOCK_STREAM` | 1 | TCP socket type |
| `2130706433` | 0x7F000001 | 127.0.0.1 in decimal |
| `STDERR_FILENO` | 2 | Standard error fd |

---

## Error Cases

| Error | Message | Action |
|-------|---------|--------|
| argc != 2 | "Wrong number of arguments" | Exit |
| socket() fails | "Fatal error" | Exit |
| bind() fails | "Fatal error" | Exit |
| listen() fails | "Fatal error" | Exit |
| recv() <= 0 | (none) | Disconnect client |

---

## Message Formats

### Client Connect
```
server: client <id> just arrived\n
```

### Client Disconnect
```
server: client <id> just left\n
```

### Client Message
```
client <id>: <message>\n
```

---

## Compilation & Usage

### Compile
```bash
gcc -Wall -Wextra -Werror miniserv.c -o miniserv
```

### Run
```bash
./miniserv <port>
```

### Connect
```bash
telnet localhost <port>
nc localhost <port>
```

---

## Testing Checklist

- [ ] Server starts with valid port
- [ ] Error on wrong arguments
- [ ] Client connects successfully
- [ ] "just arrived" broadcast received
- [ ] Messages broadcast to all others
- [ ] Multiple clients work simultaneously
- [ ] Client disconnect broadcasts "just left"
- [ ] Multi-line messages handled correctly
- [ ] Partial messages buffered properly

---

## Important Notes

⚠️ **select() modifies fd_set**: Always copy from master set  
⚠️ **Update max_fd**: When new client fd > current max_fd  
⚠️ **Break after processing**: fd_sets become stale after accept/recv  
⚠️ **Buffer bounds**: Check ``start < sizeof(msg)`` before writing  
⚠️ **Newline delimiter**: Messages split on '\n' only  

---

## Quick Debug Tips

1. **Client not receiving broadcasts?**
   - Check FD_ISSET(fd, &writefds)
   - Verify fd in activefds

2. **Messages garbled?**
   - Check message buffer initialization
   - Verify '\n' handling

3. **Server crashes on disconnect?**
   - Check recv() return value handling
   - Ensure FD_CLR before close()

4. **New clients not monitored?**
   - Verify max_fd updated
   - Check FD_SET(clientfd, &activefds)

---

## Performance Considerations

| Aspect | Limit/Note |
|--------|------------|
| Max clients | 2048 (array size) |
| Message size | 100,000 bytes per client |
| Backlog | 10 pending connections |
| select() limitation | ~1024 fds on many systems |
| Scalability | O(n) per select call |

---

## Alternatives & Improvements

- **poll()**: No fd limit, but still O(n)
- **epoll()**: Linux-specific, O(1) complexity
- **kqueue**: BSD/macOS equivalent of epoll
- **Non-blocking I/O**: Avoid blocking on single client
- **Message validation**: Check length/format
- **Graceful shutdown**: Handle SIGINT/SIGTERM
