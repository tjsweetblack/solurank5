# Mini Server - Technical Reference Guide

## Table of Contents
1. [Architecture Overview](#architecture-overview)
2. [Data Structures](#data-structures)
3. [Function Reference](#function-reference)
4. [System Call Reference](#system-call-reference)
5. [Protocol Specification](#protocol-specification)
6. [Implementation Details](#implementation-details)
7. [Memory Management](#memory-management)
8. [Error Handling](#error-handling)
9. [Performance Analysis](#performance-analysis)
10. [Security Considerations](#security-considerations)

---

## Architecture Overview

### Design Pattern
**Event-Driven Single-Threaded Server**
- Uses I/O multiplexing (select) to handle multiple clients
- Non-blocking event loop
- Synchronous I/O operations

### Flow Diagram
```
┌─────────────────┐
│  Server Socket  │  (fd 3)
└────────┬────────┘
         │
         ├──► select() monitors all fds
         │
    ┌────┴────┐
    │ Activity │
    └────┬────┘
         │
    ┌────┴────────────┐
    │                 │
┌───▼────┐      ┌─────▼─────┐
│ Server │      │  Client   │
│   fd   │      │    fd     │
└───┬────┘      └─────┬─────┘
    │                 │
    ▼                 ▼
accept()         ft_response()
    │                 │
    ▼                 ▼
ft_get_new       recv() data
_client()             │
    │            ┌────┴────┐
    ▼            ▼         ▼
Broadcast    Buffer    Disconnect
arrival      message   (if error)
```

---

## Data Structures

### t_client
```c
typedef struct s_client
{
    int id;              // Client identifier (0, 1, 2, ...)
    char msg[100000];    // Incomplete message buffer
} t_client;
```

**Purpose:** Store per-client state
**Size:** 100,004 bytes per client
**Array:** `t_client clients[2048]` (200+ MB total)

**Fields:**
- `id`: Assigned sequentially from `increment_id`
- `msg`: Accumulates data until newline found

### Global State Variables

| Variable | Type | Scope | Purpose |
|----------|------|-------|---------|
| `clients` | `t_client[2048]` | Global | Per-client state storage |
| `readfds` | `fd_set` | Global | Working set for read monitoring |
| `writefds` | `fd_set` | Global | Working set for write monitoring |
| `activefds` | `fd_set` | Global | Master set of all active fds |
| `max_fd` | `int` | Global | Highest fd currently active |
| `increment_id` | `int` | Global | Counter for client IDs |
| `buffread` | `char[100000]` | Global | Temporary recv buffer |
| `buffwrite` | `char[100000]` | Global | Temporary send buffer |

### fd_set Implementation

**Internal Structure:**
```c
typedef struct {
    long fds_bits[FD_SETSIZE / NFDBITS];
} fd_set;
```

**Typical FD_SETSIZE:** 1024 (platform-dependent)
**Limitation:** Cannot monitor fd ≥ FD_SETSIZE

---

## Function Reference

### ft_putstr_fd()
```c
void ft_putstr_fd(char *msg)
```

**Purpose:** Print error message and exit

**Parameters:**
- `msg`: Error message string

**Behavior:**
1. Writes `msg` to STDERR (fd 2)
2. Exits with status code 1

**Used for:**
- Invalid argument count
- Socket creation failure
- Bind/listen failures

**Example:**
```c
if (argc != 2)
    ft_putstr_fd("Wrong number of arguments\n");
```

---

### ft_broadcast()
```c
void ft_broadcast(int sender_fd)
```

**Purpose:** Send message to all clients except sender

**Parameters:**
- `sender_fd`: File descriptor to exclude from broadcast

**Algorithm:**
```
for each fd from 0 to max_fd:
    if fd ≠ sender_fd AND fd in writefds:
        send(fd, buffwrite, strlen(buffwrite), 0)
```

**Key Points:**
- Uses global `buffwrite` buffer
- Checks `writefds` to ensure fd is valid
- Skips `sender_fd` to avoid echo
- No error checking on send()

**Performance:** O(max_fd)

**Called by:**
- `ft_get_new_client()`: Announce arrival
- `ft_disconnect_client()`: Announce departure  
- `ft_response()`: Broadcast messages

---

### ft_disconnect_client()
```c
void ft_disconnect_client(int fd)
```

**Purpose:** Clean up client connection

**Parameters:**
- `fd`: Client file descriptor

**Algorithm:**
```
1. Format "client X just left" to buffwrite
2. Broadcast to all other clients
3. Remove fd from activefds
4. Close socket
```

**Critical Operations:**
- Must clear from fd_set BEFORE next select()
- Socket close() releases kernel resources
- Client struct data NOT explicitly cleared

**Side Effects:**
- Modifies `buffwrite`
- Modifies `activefds`
- Closes file descriptor

---

### ft_get_new_client()
```c
void ft_get_new_client(int serverfd)
```

**Purpose:** Accept and initialize new client

**Parameters:**
- `serverfd`: Server socket file descriptor

**Algorithm:**
```
1. clientfd = accept(serverfd, NULL, NULL)
2. if clientfd > max_fd then max_fd = clientfd
3. FD_SET(clientfd, &activefds)
4. clients[clientfd].id = increment_id++
5. memset(clients[clientfd].msg, 0, ...)
6. Format "client X just arrived" to buffwrite
7. Broadcast announcement
```

**Error Handling:**
- Returns silently if accept() fails (-1)
- No validation of clientfd range

**Assumptions:**
- `clientfd < 2048` (array bounds)
- Accept won't fail under normal conditions

**State Changes:**
- Increments `increment_id`
- Updates `max_fd`
- Modifies `activefds` and `clients` array

---

### ft_response()
```c
void ft_response(int fd)
```

**Purpose:** Handle incoming client data

**Parameters:**
- `fd`: Client file descriptor

**Algorithm:**
```
1. bytes = recv(fd, buffread, sizeof(buffread), 0)
2. if bytes <= 0:
       ft_disconnect_client(fd)
       return
3. start = strlen(clients[fd].msg)
4. for i = 0 to bytes-1:
       if start < sizeof(clients[fd].msg):
           clients[fd].msg[start] = buffread[i]
           if buffread[i] == '\n':
               clients[fd].msg[start] = '\0'
               Format to buffwrite
               ft_broadcast(fd)
               Clear clients[fd].msg
               start = -1
           start++
```

**Message Protocol:**
- Messages delimited by '\n'
- Partial messages buffered in `clients[fd].msg`
- Complete messages broadcast immediately

**Buffer Overflow Protection:**
- Checks `start < sizeof(clients[fd].msg)`
- Silently drops excess data beyond buffer

**Edge Cases:**
- Multiple messages in one recv()
- Message split across multiple recv()
- Messages without newlines (buffer indefinitely)

---

### main()
```c
int main(int argc, char **argv)
```

**Purpose:** Server initialization and main loop

**Structure:**
1. Argument validation
2. Socket creation & configuration
3. Bind & listen
4. Event loop

**Initialization:**
```c
// Validate arguments
if (argc != 2)
    ft_putstr_fd("Wrong number of arguments\n");

// Create socket
int serverfd = socket(AF_INET, SOCK_STREAM, 0);
if (serverfd == -1)
    ft_putstr_fd("Fatal error\n");

// Configure address
struct sockaddr_in servaddr;
memset(&servaddr, 0, sizeof(servaddr));
servaddr.sin_family = AF_INET;
servaddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
servaddr.sin_port = htons(atoi(argv[1]));

// Bind and listen
if (bind(serverfd, ...) != 0)
    ft_putstr_fd("Fatal error\n");
if (listen(serverfd, 10) != 0)
    ft_putstr_fd("Fatal error\n");
```

**Main Loop:**
```c
max_fd = serverfd;
FD_ZERO(&activefds);
FD_SET(serverfd, &activefds);

while (1) {
    readfds = activefds;
    writefds = activefds;
    
    if (select(max_fd + 1, &readfds, &writefds, NULL, NULL) < 0)
        continue;
    
    for (int fd = 0; fd <= max_fd; fd++) {
        if (!FD_ISSET(fd, &readfds))
            continue;
        
        if (fd == serverfd) {
            ft_get_new_client(serverfd);
            break;
        } else {
            ft_response(fd);
            break;
        }
    }
}
```

**Loop Characteristics:**
- Infinite loop (no exit condition)
- Continues on select() error
- Processes one event per iteration (break)

---

## System Call Reference

### socket()
```c
int socket(int domain, int type, int protocol);
```

**Parameters:**
- `domain`: `AF_INET` (IPv4)
- `type`: `SOCK_STREAM` (TCP)
- `protocol`: `0` (default)

**Returns:**
- File descriptor on success
- -1 on error

**Errors:**
- `EMFILE`: Process fd limit reached
- `ENFILE`: System fd limit reached
- `EACCES`: Permission denied

---

### bind()
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Purpose:** Assign address to socket

**Returns:**
- 0 on success
- -1 on error

**Common Errors:**
- `EADDRINUSE`: Port already in use
- `EACCES`: Permission denied (port < 1024)

**Note:** Use `SO_REUSEADDR` option to avoid EADDRINUSE during development

---

### listen()
```c
int listen(int sockfd, int backlog);
```

**Parameters:**
- `sockfd`: Server socket
- `backlog`: 10 (max pending connections)

**Behavior:**
- Marks socket as passive (accepting connections)
- Queues up to 10 pending connections

---

### accept()
```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

**Usage in code:**
```c
int clientfd = accept(serverfd, NULL, NULL);
```

**Parameters:**
- `NULL, NULL`: Don't retrieve client address

**Returns:**
- New socket fd for client
- -1 on error

**Behavior:**
- Blocks until connection available (when select indicates ready)
- Creates new socket for client communication

---

### select()
```c
int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
```

**Usage:**
```c
select(max_fd + 1, &readfds, &writefds, NULL, NULL)
```

**Parameters:**
- `max_fd + 1`: Range to monitor [0, max_fd]
- `readfds`: Monitor for reading
- `writefds`: Monitor for writing
- `NULL`: No exception monitoring
- `NULL`: Block indefinitely

**Returns:**
- Number of ready fds
- 0 on timeout (not applicable here)
- -1 on error

**Side Effect:**
- Modifies fd_sets to indicate ready fds

**Errors:**
- `EINTR`: Interrupted by signal (code continues)
- `EINVAL`: Invalid parameters

---

### recv()
```c
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

**Usage:**
```c
bytes = recv(fd, buffread, sizeof(buffread), 0);
```

**Returns:**
- Number of bytes received
- 0 when connection closed gracefully
- -1 on error

**Behavior:**
- Blocks until data available (when select indicates ready)
- Returns up to `len` bytes

---

### send()
```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

**Usage:**
```c
send(fd, buffwrite, strlen(buffwrite), 0);
```

**Returns:**
- Number of bytes sent
- -1 on error

**Important:**
- May send less than `len` bytes (partial send)
- Code doesn't check return value (assumes success)

---

## Protocol Specification

### Message Format

#### Server Announcements
```
server: client <id> just arrived\n
server: client <id> just left\n
```

#### Client Messages
```
client <id>: <message>\n
```

### Wire Protocol
- **Encoding:** ASCII text
- **Delimiter:** '\n' (newline)
- **Max message size:** 100,000 bytes
- **No message headers or length prefixes**

### Connection Lifecycle
```
1. Client connects
   → Server accepts
   → Assigns ID
   → Broadcasts "just arrived"

2. Client sends data
   → Server buffers until '\n'
   → Broadcasts "client X: message"

3. Client disconnects (or error)
   → Server detects (recv ≤ 0)
   → Broadcasts "just left"
   → Closes socket
```

---

## Implementation Details

### Client ID Assignment
```c
clients[clientfd].id = increment_id++;
```

- IDs start at 0
- Monotonically increasing
- Never reused (even after disconnect)
- No overflow protection

### Buffer Management

**Three buffer types:**

1. **Per-client buffer:** `clients[fd].msg`
   - Persistent across recv() calls
   - Accumulates incomplete messages
   - Cleared after broadcasting

2. **Read buffer:** `buffread`
   - Temporary storage for recv()
   - Reused for every recv() call
   - Size: 100,000 bytes

3. **Write buffer:** `buffwrite`
   - Formatted messages for broadcast
   - Reused for every send() call
   - Size: 100,000 bytes

### Message Buffering Algorithm

**State machine per client:**
```
RECEIVING:
    Append to clients[fd].msg
    If '\n' found:
        → BROADCAST state
        
BROADCAST:
    Format to buffwrite
    Send to all others
    Clear clients[fd].msg
    → RECEIVING state
```

**Implementation:**
```c
start = strlen(clients[fd].msg);  // Find append position
for (int i = 0; i < bytes && start < sizeof(...); i++, start++) {
    clients[fd].msg[start] = buffread[i];
    if (clients[fd].msg[start] == '\n') {
        clients[fd].msg[start] = '\0';  // Null-terminate
        sprintf(buffwrite, "client %d: %s\n", ...);
        ft_broadcast(fd);
        memset(&clients[fd].msg, 0, sizeof(...));
        start = -1;  // Reset (will be 0 after i++)
    }
}
```

### select() Pattern

**Why copy fd_sets?**
```c
readfds = activefds;    // Must copy!
writefds = activefds;
select(..., &readfds, &writefds, ...);
```

`select()` modifies fd_sets to indicate ready fds. Must preserve master set (`activefds`) for next iteration.

**Why break after processing?**
```c
if (fd == serverfd) {
    ft_get_new_client(serverfd);
    break;  // WHY?
}
```

After `accept()` or `recv()`, fd_sets are stale. New fd might need monitoring. Must restart select() with updated sets.

---

## Memory Management

### Static Allocation
- All buffers statically allocated
- No malloc/free required
- Memory usage: ~200 MB (2048 clients × 100 KB)

### Memory Layout
```
BSS Segment (uninitialized data):
    clients[2048]      ~200 MB
    buffread[100000]   ~100 KB
    buffwrite[100000]  ~100 KB
    activefds          ~128 bytes (typical)
    readfds            ~128 bytes
    writefds           ~128 bytes
    
Data Segment:
    max_fd             4 bytes
    increment_id       4 bytes
```

### Memory Efficiency
- **Wasteful:** Allocates 100 KB per potential client
- **Benefit:** No allocation failures at runtime
- **Alternative:** Dynamic allocation per active client

---

## Error Handling

### Error Categories

| Error Type | Handling | Example |
|------------|----------|---------|
| Argument errors | Exit with message | argc != 2 |
| Initialization errors | Exit with "Fatal error" | socket/bind/listen fail |
| Runtime errors (select) | Continue (retry) | select() returns -1 |
| Client errors | Disconnect | recv() ≤ 0 |
| Send errors | Ignore | send() returns -1 |

### Missing Error Checks

**Unchecked operations:**
1. `send()` return value
2. `sprintf()` buffer overflow
3. `atoi()` validation
4. Client array bounds (`clientfd >= 2048`)

**Potential issues:**
- Partial sends not handled
- Buffer overflow in sprintf
- Invalid port numbers accepted
- Array out of bounds if fd ≥ 2048

---

## Performance Analysis

### Time Complexity

| Operation | Complexity | Notes |
|-----------|------------|-------|
| select() | O(max_fd) | Scans all bits |
| Accept client | O(max_fd) | Iterate to find ready fd |
| Receive message | O(max_fd) | Iterate to find ready fd |
| Broadcast | O(max_fd) | Send to all fds |
| Per-event | O(max_fd) | Dominated by fd iteration |

### Space Complexity
- **O(1)** - Fixed 200 MB regardless of active clients

### Scalability Limits

1. **FD_SETSIZE:** Typically 1024
   - Cannot monitor fd ≥ 1024
   - macOS raises fd limits, but FD_SETSIZE is compile-time constant

2. **Array size:** 2048
   - Mismatch with FD_SETSIZE
   - Out of bounds access possible

3. **select() O(n):** Inefficient for many clients
   - epoll/kqueue is O(1)

### Throughput Analysis

**Assumptions:**
- 100 clients
- 10 messages/second each
- Average message: 50 bytes

**select() overhead:**
- 1000 events/sec
- Each: O(100) to find ready fd
- ~100,000 fd checks/sec

**Broadcast overhead:**
- 1000 broadcasts/sec
- Each: 100 sends
- ~100,000 send() calls/sec

**Bottleneck:** Broadcasting

---

## Security Considerations

### Vulnerabilities

1. **Buffer Overflow Risk**
   ```c
   sprintf(buffwrite, "client %d: %s\n", clients[fd].id, clients[fd].msg);
   ```
   - If `clients[fd].msg` is 99,999 bytes
   - Plus "client 9999: " prefix
   - Could overflow `buffwrite[100000]`

2. **Denial of Service**
   - Client can send messages without newlines
   - Fills up 100 KB buffer
   - Server waits indefinitely for '\n'

3. **Resource Exhaustion**
   - No client limit enforced
   - Can connect until fd limit reached
   - Each client wastes 100 KB

4. **No Authentication**
   - Anyone can connect
   - No identity verification

5. **Local Only**
   - Binds to 127.0.0.1
   - Cannot accept remote connections
   - Intentional? Or oversight?

### Mitigations

1. **Use snprintf:**
   ```c
   snprintf(buffwrite, sizeof(buffwrite), "client %d: %s\n", ...);
   ```

2. **Message length limit:**
   ```c
   if (strlen(clients[fd].msg) > MAX_MSG_LEN) {
       ft_disconnect_client(fd);
   }
   ```

3. **Connection limit:**
   ```c
   if (client_count >= MAX_CLIENTS) {
       close(accept(...));
   }
   ```

4. **Timeout mechanism:**
   - Use select() timeout
   - Disconnect inactive clients

---

## Advanced Topics

### Comparison with Alternatives

| Feature | select() | poll() | epoll() | kqueue |
|---------|----------|--------|---------|--------|
| FD limit | FD_SETSIZE | No limit | No limit | No limit |
| Complexity | O(n) | O(n) | O(1) | O(1) |
| Portability | POSIX | POSIX | Linux only | BSD/macOS |
| State | Stateless | Stateless | Stateful | Stateful |

### Porting to poll()
```c
struct pollfd fds[2048];
int nfds = 0;

// Add server fd
fds[0].fd = serverfd;
fds[0].events = POLLIN;
nfds = 1;

// Main loop
poll(fds, nfds, -1);
for (int i = 0; i < nfds; i++) {
    if (fds[i].revents & POLLIN) {
        // Handle event
    }
}
```

### Event-Driven Architecture
This server follows the **Reactor pattern:**
- **Event demultiplexer:** select()
- **Event handlers:** ft_get_new_client, ft_response
- **Dispatcher:** main loop

---

## Debugging Guide

### Common Issues

**Problem:** Server exits immediately
- **Cause:** Port in use, bind() fails
- **Solution:** Check error, use different port

**Problem:** Clients don't receive messages
- **Cause:** fd not in writefds
- **Solution:** Verify FD_SET after accept

**Problem:** Messages corrupted
- **Cause:** Buffer not null-terminated
- **Solution:** Check '\n' handling

### Tracing Execution

**Add debug output:**
```c
fprintf(stderr, "DEBUG: select returned, checking fds\n");
fprintf(stderr, "DEBUG: fd=%d ready, is_server=%d\n", fd, fd == serverfd);
```

**Use strace (Linux) / dtruss (macOS):**
```bash
strace -e socket,bind,listen,accept,recv,send ./miniserv 4242
```

---

## Summary

This mini server demonstrates fundamental concepts:
- ✅ Socket programming basics
- ✅ I/O multiplexing with select()
- ✅ Event-driven architecture
- ✅ Multi-client message broadcasting
- ✅ Stateful connection management

Limitations:
- ❌ Not production-ready
- ❌ Security vulnerabilities
- ❌ Poor scalability (O(n))
- ❌ No error recovery

Ideal for:
- Learning network programming
- Understanding select()
- Building more complex servers
- Interview preparation
