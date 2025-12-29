# Client and server changes:
## Requirements:
1. Focus on clean code, beautiful code, strong code structure both in client and server (if any changes is needed). For server, try as much as you can not to touch it, but rather focus on the client code. The server code is already quite complete and clean, so try not to touch it as much as you do with the client code. For client, refractoring for cleanliness is needed rigorously, especially when touching socket programming components (this is the main part of our project).
2. MUST REMEMBER NOTE: DO NOT USE ANY QT NETWORKING COMPONENTS, EVERY SINGLE LINE OF CODE IN CLIENT NETWORKING MUST BE DONE IN PURE C POSIX SOCKET PROGRAMMING (YOU CAN EMBED THESE IN CPP FILES, IT IS OKAY BUT IT MUST BE TECHNICALLY C SOCKET PROGRAMMING). Not a single Qt TCP socket involved, not a single thing is allowed. Destroy all of Qt's networking-related components.

## Client is missing, but server is implemented:
1. The host did not have the chance to pause the game, while the server already implements the option for the host to pause the game while it is running. This is absolutely crucial and need to be focused on carefully. The pause and resume functionalities should be checked very thoroughly and then implement on the client side.
2. Also, the client should now implement the replay screens. Let me describe you the screens for a replay: whenever we trigger a replay, it iterates through the questions of that game just like in the original gameplay, but add a list of users' answers and their timestamp of answering the question. This way, we can see every other players in the game and how they interact with the game. This will recycle parts of the question displaying screen in-game, and just expand the functionality of it to serve the replaying purpose.

## Client bugs and needed to add:
1. Crucial thing to focus on: focus on refractoring all of client code to be clean code, use POSIX socket/networking components (specifically for Linux, we do not use BSD) and removing/destroying all of Qt's networking components. Focus on putting the networking handler in a seperate code, and when you refractor, ensure that the socket programming code for POSIX socket is clean, beautiful and no comments in code. Functions, class, attributes be made clearly. 
2. The Qt client is missing some functionalities: we cannot see our stats, our rank, because whenever we log in, we immediately meet with a room choosing screen. Add another screen before the room chooser, displaying the simple stats about our game, and make the user choose between seeing their stats, seeing replay and joining a room to play games.

---
# Final changes in client and server
1. The replay functionality is having some problems due to the server, protocol and maybe SQL error in handling. While the client will be displaying the questions in the replay session (meaning it will display the questions, along with 4 answers of the questions). The server and the protocol is missing on the return of all questions metadata and answers, making the information not enough to render a well done replay.
2. When we log out, please route to the login/register screen. Do not disconnect the client immediately. When we leave a room, also route us to the main screen, the homepage that you implemented.

