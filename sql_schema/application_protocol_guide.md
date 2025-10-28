Based on the provided PDF content, here are all the parts that discuss **application layer protocol design**, including text representations for image-based pages.

---

### **Page 52: APPLICATION PROTOCOL DESIGN**
*(This page only contains the title)*

---

### **Page 53: Protocol**

- **Set of rules:**
  - Message format
  - Message sequence
  - Process message

- **Goals**
  - Everyone must know
  - Everyone must agree
  - Unambiguous
  - Complete

---

### **Page 54: Example: POP session**

```
C: <client connects to service port 110>
S: +OK POP3 server ready <1896.6971@mailgate.dobbs.org>
C: USER bob
S: +OK bob
C: PASS redqueen
S: +OK bob's maildrop has 2 messages (320 octets)
C: LIST
S: +OK 2 messages (320 octets)
S: 1 120
S: 2 200
S: .
C: QUIT
S: +OK dewey POP3 server signing off (maildrop empty)
C: <client hangs up>
```

---

### **Page 55: Example: FTP authentication**

```
- ftp 202.191.56.65
- Connected to 202.91.56.65
- **220** Servers identifying string
**User:** **tungbt (C: USER tungbt)**
- **331** Password required for tungbt
**Password: (C: PASS)**
- **530** Login incorrect
- **1s**
- **530** Please login with USER and PASS
- USER tungbt
- **331** Password required for tungbt
**Password: (C: PASS)**
- **230** User tungbt logged in
```

---

### **Page 56: Steps in design**

1. Define services  
2. Choose application model (client/server, P2P, ...)  
3. Establish the design goals  
4. Design the message structure: format, fields, types of messages, encoding, ...  
5. Protocol processing  
6. Interaction with environment (DNS, DHCP...)

---

### **Page 57: Design Goals**

- Do we need reliable exchanges?  
- How many types of parties are involved? Can they all communicate to each other?  
- Is the authentication of parties needed?  
- How important is the authentication of parties?  
- Is the transferred data confidential? What degree of authorization is needed?  
- Do we need complex error handling?

---

### **Page 58: Design Issues**

- Is it to be stateful vs stateless?  
- Is the transport protocol reliable or unreliable?  
- Are replies needed?  
  - How to respond to lost replies?  
- Is it to be broadcast, multicast, or unicast?  
  - Broadcast, multicast: must use UDP Socket  
- Are there multiple connections?  
  - How to synchronize?  
- How many types of parties are involved? Can they all communicate to each other?  
- Session management  
- Security: authentication, authorization, confidential...

---

### **Page 59: Designing the Message**

- **Header**: contains structured fields describing the actual data in the message, such as:
  - message type  
  - command  
  - body size  
  - recipient information  
  - sequence information  
  - retransmission count...

- **Body**: the actual data to be transmitted:
  - the command parameters  
  - the data payload  

**Structure:**
```
[ Header ] [ Body ]
```

**The simplest formats:**
- Type – Length – Value (TLV)  
- Type – Value

---

### **Page 60: Control Messages**

- Define the stages of the dialogue between the parties  
- Control the dialogue between the parties  
- Address various communication aspects:
  - communication initiation or ending  
  - describe the communication stage (e.g., authentication, status request, data transfer)  
  - coordination (e.g., receipt confirmation, retry requests)  
  - resource changes (e.g., requests for new communication channels)  

**Usual format:**
```
[ Command ] [ Parameter ]
```
- **Command**: SHOULD have fixed length or use delimiter  
- Example: USER, PASS, PWD (FTP)

---

### **Page 61: Data transfer**

- Messages that carry data over the network  
- Usually sent as responses to specific commands  
- Data is usually fragmented into multiple messages  
- **Header** describes:
  - the type of the binary data format  
  - clues for the layout of the structured data (when the structure is flexible/dynamic)  
  - data size, offset, or sequence information  
  - type of the data block: last / intermediary

---

### **Page 62: Message Format – Byte oriented**

- The first part of the message is typically a byte to distinguish between message types.  
- Further bytes contain message content according to a pre-defined format.  
- **Advantages**: compactness  
- **Disadvantages**: harder to process, debug, or test  
- **Example**: DHCP, DNS

---

### **Page 63: Data Format – Text-oriented**

- A message is a sequence of one or more lines  
- The start of the first line is typically a word that represents the message type  
- The rest of the first line and successive lines contain the data  

**Advantages:**
- easy to understand, monitor  
- flexible  
- easy to test  

**Disadvantages:**
- may make messages unjustifiably large  
- may become complex  

**Example:** HTTP, FTP, email protocols

---

### **Page 64: Protocol Processing**

- Describe the sequences of messages, at each and all stages in each communication scenario, for all parties in the system  
- **Finite State Machine is mandatory**:
  - **State**  
  - **Transaction**: Trigger[Guard]/[Effect]  
  - **Choose**  
  - And/Or use **State Table**

| Current State | Transaction | Next State |
|---------------|-------------|------------|
|               | Receive     | Send       |
|               |             |            |

---

### **Page 65: Example: POP3 and IMAP4 session**
*(This page is image-based; no detailed text content provided in the PDF)*

---

### **Page 66: Example: TCP connection**
*(This page contains a finite state machine diagram for TCP connection. Text representation is not fully extractable, but it includes states like CLOSED, LISTEN, SYN_SENT, ESTABLISHED, etc.)*

---

### **Page 67: Message Transaction Diagram – Example: POP3**

**Text Representation of the Diagram:**

```
POP3 Client                                      POP3 Server
1. Request Mailbox Status
2. Receive STAT; Send +OK Reply, Mailbox Size and Number of Messages
3. Receive Statistics; Request List of Messages In Mailbox
4. Receive LIST; Send +OK Reply, List of Message Numbers and Sizes
5. Receive Message List, Retrieve Message Number 1
6. Receive RETR; Send +OK Reply, Then Message Text
7. Receive Successful Reply, Delete Message 1
8. Receive DELE; Mark Message For Deletion, Send +OK Reply
9. Receive Successful Reply, Retrieve Message Number 2
10. Receive RETR; Send +OK Reply, Then Message Text
11. Receive Successful Reply, Delete Message 2
12. Receive DELE; Mark Message For Deletion, Send +OK Reply
13. Receive Successful Reply, Perform Other Transactions...
```

---

### **Page 68: Implementing an Application Protocol**

**Type of message:**
- Use integer: `enum msg_type { ... }`
- Use string

**Data structure:**
- Use struct. Example:

```c
struct message {
    char msg_type[4];
    char data_type[8];
    int value;
};

struct message {
    msg_type type;
    struct msg_payload payload;
};

struct msg_payload {
    int id;
    char fullname[30];
    int age;
    //...
};
```

**Alternative:**
- Use string or byte array

**Format:**
```
msg_type | data_type | length | value | data_type | length | value | ...
```
*(fixed length)*

---

### **Page 69: Implementing an Application Protocol – Message Handler**

**Pseudo-code:**
```c
// handle message
switch (msg_type) {
    case MSG_TYPE1:
    {
        //...
    }
    case MSG_TYPE2:
    {
        //...
        if (data_type == DATA_TYPE1)
            //...
    }
    //...
}
```

---