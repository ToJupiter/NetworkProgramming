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
## Requirements:
1. Focus on clean code, beautiful code, strong code structure both in client and server (if any changes is needed). For server, try as much as you can not to touch it, but rather focus on the client code. The server code is already quite complete and clean, so try not to touch it as much as you do with the client code. For client, refractoring for cleanliness is needed rigorously, especially when touching socket programming components (this is the main part of our project).


## Implementation:
1. The replay functionality is having some problems due to the server, protocol and maybe SQL error in handling. While the client will be displaying the questions in the replay session (meaning it will display the questions, along with 4 answers of the questions). The server and the protocol is missing on the return of all questions metadata and answers, making the information not enough to render a well done replay. Please have a look at the server-side implementation, since it is only doing game_log table. It should also take the questions table into account.
2. When we log out, please route to the login/register screen. Do not disconnect the client immediately. When we leave a room, also route us to the mainmenuwindow, the homepage that you implemented.
3. For the ranked points system, maybe we should do some revamps in both client and server. For instance, we should be displaying which percentile that person is ranked on the table, and like their #top (like #1, 99.9 percentile). And also we have a ranking system here:
**Tier Thresholds:**
- **Bronze:** 0-1199 RP
- **Silver:** 1200-1599 RP
- **Gold:** 1600-1999 RP
- **Platinum:** 2000-2399 RP
- **Diamond:** 2400+ RP
Implement it. 
4. Also implement of a very important functionality, the Game History for each user. This needs coordination in both the server-side and the client-side. The Game History queries the database for games that a player played and their stats when they played the game:
- number of right answer, average response time: game_log table in database 
- the score they gained during the game, their rank: session_participants table
- game-mode of that game: game_sessions table.
This Game History will add another button on the MainMenu window of the game. Focus on making it right and beautiful.

# Client Refactoring and Server Enhancements for Quiz Game Platform

## Project Context
We have a multiplayer quiz game platform with:
- **Server**: C++ with SQLite
- **Client**: C++ with Qt. Client and server connects using POSIX socket.
- **Database tables**: `users`, `questions`, `game_sessions`, `session_participants`, `game_log`
- **Current features**: Real-time multiplayer, ranked games, replay functionality (partially working)

## Specific Requirements

### 1. Replay Functionality Fix
**Problem**: Client displays questions in replay session but server/protocol returns insufficient data from only `game_log` table.
**Solution**: Enhance server-side to include questions metadata from `questions` table alongside game logs from `game_log`. Update protocol to return:
- Question content, options (1-4), correct answer, difficulty
- User's selected answer and response time for each question
- Game session metadata (mode, timestamp, participants)
Update the client also to fit with the changes in the server.

### 2. Navigation & Routing
When we end a game, the Room only displays a button `Leave Room`. When we press this button, client immediately disconnected. This is not good. When we press this button, it should route us to the `MainMenuWindow`. 
- **Logout**: Route to login/register screen without immediate client disconnection
- **Leave Room**: Route to `MainMenuWindow` (homepage)

### 3. Ranked Points System Revamp
Implement tier system with RP thresholds:
- **Bronze**: 0-1199 RP
- **Silver**: 1200-1599 RP  
- **Gold**: 1600-1999 RP
- **Platinum**: 2000-2399 RP
- **Diamond**: 2400+ RP

**Display enhancements**:
- Show percentile ranking (e.g., "#1, 99.9th percentile")
- Calculate percentile based on all ranked players

### 4. Game History Feature
**New button** on `MainMenuWindow` to view player's game history.

**Server-side**: New endpoint to query combined data from:
- `game_log`: Number of correct answers, average response time
- `session_participants`: Score gained, final rank in session
- `game_sessions`: Game mode (scoring/elimination), timestamp.

**Client-side**: Clean, responsive UI displaying:
- Game date/time, mode, score, rank, correct answers, avg response time

### 5. Code Quality Focus
- **Client**: Minimal changes, clean code.
- **Server**: Minimal changes; focus on query optimization and clean endpoints
- **Structure**: Maintain strong separation of concerns, clean interfaces. THIS IS IMPORTANT: CLEAN CODE MEANS NO COMMENTS ARE ALLOWED IN CODE. EVERY BLOCK OF CODE MUST BE WRITTEN CLEANLY FROM THE START, IT DOES NOT NEED COMMENTS. THE NEWLY GENERATED CODE MUST ALSO BLENDS IN WITH THE EXISTING CODE, WITH THE SAME STYLE AND SAME FUNCTIONAL DESIGN. MUST BE COMPATIBLE AND ACCLIMATED WITH THE EXISTING CODE.

## Need to fix in this final sprint:
### Requirements:
1. Focus on clean code, beautiful code, strong code structure both in client and server (if any changes is needed). For server, try as much as you can not to touch it, but rather focus on the client code. The server code is already quite complete and clean, so try not to touch it as much as you do with the client code. For client, refractoring for cleanliness is needed rigorously.
### Implementation
1. We need to focus on fixing some kind of issue happening with ranked_points in the users table of the SQLite database. Please look carefully at the server code in order to find us the part where it saves the ranked_points and watch me if somehow it resets my ranked_points back to the default value of 1000. This is really dangerous because it poses something with data loss.
2. When we end the game, you should generate us one more screen: the screen displaying the results of the game, sorted by score gained during the game, number of right questions, average time to answer for each of the question. This is the room stats display screen.
3. When we press leave room (when we are in a Room in UI), please return us (route us to the MainMenu screen and use the leave room request in the protocol to get us out of the room). When we press leave room, please call the request to kick us out of the room and then return to MainMenuWindow. When we end a game, the Room only displays a button `Leave Room`. When we press this button, client immediately disconnected. This is not good. When we press this button, it should route us to the `MainMenuWindow`. 
4. Please fix us the replay function by looking carefully at `protocol.h`, `client` and `server` code, also the `game_log` and `questions` table. This functionality still does not work at all, always an error. If authentication issues in the server side is the problem, remove all authentication because our game is open for replaying and every one should be able to watch it. And also, in the replay session chooser, list us a list of sessions we can see the replay (like a list of old games and we will choose one of them to watch the replay).
5. Please edit the MainMenuWindow UI to be better looking, and tailor the Room member list as well. Everything should be done perfectly and clearly. 


## Final final sprint:

### Requirements:
1. Focus on clean code, beautiful code, strong code structure both in client and server (if any changes is needed). For server, try as much as you can not to touch it, but rather focus on the client code. The server code is already quite complete and clean, so try not to touch it as much as you do with the client code. For client, refractoring for cleanliness is needed rigorously. Clean code means no comments in code, comprehensive structure, beautiful format. Code must be structured, compatible with old code.


### Implementation
1. The replay functionality is not working as intended. On the client side, it freezes at the "Waiting for the replay data..." with very slow response. The replay should work like this: 
``` 
-- It should only take into account the question that the current user_id answers, hence only query the rows in the game_log with the current user_id. Then, it will open a similar to GameWindow.ui window (the GameWindow.ui window displays the questions in a room game for the user to select and answer). Then, it will replay the questions that the current user answers (recorded in the game_log) and their answer (no need to display the correct answer to that question). It is like when you see a replay video, you see your actions, your selection (this time question answering is a selection) in the past.

-- The loading phase of the database should be faster, whether by trying to optimize the query or set a good index to it. Try to write good queries instead of focusing too much on indexes.
```
2. When in-game, please add us a button named "Quit" and when player press the button, a prompt will be displayed with the quote "Are you sure you want to quit the game? Your ELO will be negatively affected". And when the player affirms, 100 ELO punishment is applied directly to his ranked_points. This should be updated immediately to the database! This is called forfeiting. This button routes directly to `MainMenuWindow` window of the client-side UI.
3. At the end of the game, there is currently a Leave Room button only. But when I press this Leave Room button, client automatically shut itself down (disconnect from the server entirely). The process of the client auto turned itself off. This is unacceptable. And also, add us a button to continue being in that room. We can still play more games together, Leaving Room is not the only option.

