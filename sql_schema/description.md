Here is the transcription of the provided document into both Vietnamese and English versions.

---

## **Vietnamese Version**

**Mô tả dự án - Nhóm 03 - Lớp 161265**

**1. Thành viên:**
1. Nguyễn Minh Khôi - 20225642.
2. Phan Hoàng Hải - 20225715.

**2. Tổng quan**
Dự án này nhằm mục đích xây dựng một nền tảng trò chơi nhiều người chơi mô phỏng các gameshow truyền hình nổi tiếng như "Ai là triệu phú?", "1 vs 100", và các ứng dụng di động để chơi game/làm bài tập trắc nghiệm trên lớp như Quizziz, Kahoot!.

*   **Frontend:** Sử dụng Qt Designer + Qt Framework (C++) để xây dựng giao diện ứng dụng trên máy tính (desktop).
*   **Backend:** C++ sử dụng Socket programming để xử lý giao tiếp real-time.
*   **Database:** SQLite kết hợp với một thư viện TinyORM (C++) để quản lý dữ liệu người dùng, thống kê trò chơi và lịch sử trận đấu.

**3. Các Tính năng**

**2.1. Quản lý Người dùng**
*   **Đăng ký:** Người dùng có thể tạo tài khoản mới bằng email và mật khẩu.
*   **Đăng nhập:** Người dùng sử dụng thông tin đã đăng ký để đăng nhập. Hệ thống sẽ tạo và quản lý một phiên làm việc (session) cho mỗi lần đăng nhập thành công.
*   **Quản lý tài khoản:** Người dùng có thể xem và cập nhật thông tin cá nhân cơ bản (ví dụ: tên hiển thị, ảnh đại diện).

**2.2. Quản lý Phòng chơi**
*   **Tạo phòng:** Người chơi (chủ phòng) có thể tạo một phòng chơi mới, đặt tên phòng và cấu hình các tùy chọn cơ bản cho trò chơi (chế độ chơi, số lượng câu hỏi).
*   **Tham gia phòng:** Người chơi khác có thể thấy danh sách các phòng đang có sẵn và tham gia vào một phòng bất kỳ.
*   **Sảnh chờ (Lobby):** Sau khi tham gia, người chơi sẽ vào một sảnh chờ, nơi họ có thể thấy danh sách người chơi trong phòng.

**2.3. Trạng thái & Lời mời**
*   **Trạng thái người chơi:** Hệ thống hiển thị trạng thái của bạn bè hoặc người chơi khác (trực tuyến, ngoại tuyến, đang trong trận).

**2.4. Chế độ Chơi (Gameplay Modes)**
*   **Loại trực tiếp (Elimination):**
    *   Người chơi trả lời một loạt câu hỏi.
    *   Ai trả lời sai sẽ bị loại ngay lập tức.
    *   Người cuối cùng còn lại là người chiến thắng.
*   **Tính điểm (Scoring):**
    *   Tất cả người chơi trả lời cùng một bộ câu hỏi trong một khoảng thời gian nhất định (ví dụ 5 giây).
    *   Điểm được tính dựa trên:
        *   Độ chính xác: Trả lời đúng được điểm.
        *   Tốc độ: Trả lời càng nhanh, điểm thưởng càng cao.
    *   Người chơi có tổng điểm cao nhất sau khi kết thúc loạt câu hỏi sẽ chiến thắng.

**2.5. Cơ chế Trò chơi**
*   **Luồng trò chơi:** Máy chủ sẽ quản lý toàn bộ luồng của trận đấu: gửi câu hỏi, nhận câu trả lời từ người chơi, tính toán kết quả và thông báo trạng thái thắng/thua.
*   **Rời trận:** Người chơi có thể chọn rời khỏi trận đấu đang diễn ra. Hành động này sẽ được ghi nhận là một trận thua.
*   **Ghi log hoạt động:** Mọi hành động quan trọng (tạo phòng, tham gia, trả lời câu hỏi, rời trận) đều được ghi lại trên máy chủ.

**2.6. Thống kê và Lịch sử**
*   **Thống kê cá nhân:** Mỗi người dùng sẽ có một trang hồ sơ hiển thị các thông số như:
    *   Tổng số trận đã chơi.
    *   Tỷ lệ thắng/thua.
    *   Số câu trả lời đúng/sai.
    *   Điểm số trung bình.
*   **Lưu dữ liệu trận đấu:** Kết quả và các sự kiện chính của mỗi trận đấu sẽ được lưu lại.
*   **Xem lại trận đấu (Replay - Tùy chọn nâng cao):** Cho phép người dùng xem lại diễn biến của một trận đấu đã qua.

**2.7. Giao diện Người dùng (GUI)**
*   Giao diện được thiết kế trực quan, dễ sử dụng.
*   Hiển thị rõ ràng câu hỏi, các lựa chọn trả lời, thời gian đếm ngược, và bảng xếp hạng/trạng thái người chơi trong trận.
*   Cung cấp phản hồi tức thì cho các hành động của người dùng (ví dụ: thông báo trả lời đúng/sai).
*   Giao diện sảnh chờ trong Qt sẽ có danh sách người chơi, nút sẵn sàng, và trạng thái phòng được cập nhật realtime qua Socket.

**3. Kiến trúc Hệ thống**
*   **Client (Qt Framework):**
    *   Gửi yêu cầu (login, join room, trả lời...) đến server qua Socket TCP.
    *   Nhận phản hồi và cập nhật giao diện theo thời gian thực.
    *   Sử dụng signal-slot của Qt để đồng bộ UI và dữ liệu.
*   **Server (C++):**
    *   **Socket Listener:** Luôn lắng nghe các kết nối mới từ client.
    *   **Session Manager:** Quản lý phiên làm việc và trạng thái của từng người dùng đã đăng nhập.
    *   **Game Logic Handler:** Xử lý logic của các phòng chơi và các trận đấu.
    *   **Database Interface (ORM):** Giao tiếp với cơ sở dữ liệu SQLite để đọc/ghi dữ liệu.
*   **Database (SQLite):** Lưu trữ thông tin người dùng, câu hỏi, lịch sử trận đấu, và thống kê.

---

## **English Version**

**Project Description - Group 03 - Class 161265**

**1. Members:**
1. Nguyen Minh Khoi - 20225642.
2. Phan Hoang Hai - 20225715.

**2. Overview**
This project aims to build a multiplayer gaming platform simulating famous TV game shows such as "Who Wants to Be a Millionaire?", "1 vs 100", and mobile apps for playing games/quizzes in class like Quizziz and Kahoot!.

*   **Frontend:** Uses Qt Designer + Qt Framework (C++) to build the desktop application interface.
*   **Backend:** C++ using Socket programming for real-time communication.
*   **Database:** SQLite combined with the TinyORM library (C++) to manage user data, game statistics, and match history.

**3. Features**

**2.1. User Management**
*   **Registration:** Users can create a new account using an email and password.
*   **Login:** Users use their registered information to log in. The system will create and manage a session for each successful login.
*   **Account Management:** Users can view and update basic personal information (e.g., display name, profile picture).

**2.2. Room Management**
*   **Create Room:** A player (room host) can create a new game room, set a room name, and configure basic game options (game mode, number of questions).
*   **Join Room:** Other players can see a list of available rooms and join any room.
*   **Lobby:** After joining, players enter a lobby where they can see the list of players in the room.

**2.3. Status & Invitations**
*   **Player Status:** The system displays the status of friends or other players (online, offline, in-game).

**2.4. Game Modes**
*   **Elimination:**
    *   Players answer a series of questions.
    *   Anyone who answers incorrectly is immediately eliminated.
    *   The last remaining player is the winner.
*   **Scoring:**
    *   All players answer the same set of questions within a fixed time limit (e.g., 5 seconds).
    *   Points are calculated based on:
        *   Accuracy: Correct answers earn points.
        *   Speed: Faster answers earn higher bonus points.
    *   The player with the highest total score after the question round wins.

**2.5. Game Mechanics**
*   **Game Flow:** The server manages the entire flow of the match: sending questions, receiving answers from players, calculating results, and announcing win/loss status.
*   **Leave Match:** Players can choose to leave an ongoing match. This action is recorded as a loss.
*   **Activity Logging:** All important actions (creating a room, joining, answering questions, leaving a match) are logged on the server.

**2.6. Statistics and History**
*   **Personal Statistics:** Each user has a profile page displaying metrics such as:
    *   Total matches played.
    *   Win/Loss ratio.
    *   Number of correct/incorrect answers.
    *   Average score.
*   **Match Data Storage:** Results and key events of each match are saved.
*   **Match Replay (Advanced Feature):** Allows users to replay the progression of a past match.

**2.7. User Interface (GUI)**
*   The interface is designed to be intuitive and user-friendly.
*   Clearly displays the question, answer choices, countdown timer, and leaderboard/player status during the match.
*   Provides immediate feedback for user actions (e.g., notifications for correct/incorrect answers).
*   The Qt lobby interface will show a player list, ready button, and real-time room status updates via Socket.

**3. System Architecture**
*   **Client (Qt Framework):**
    *   Sends requests (login, join room, answer...) to the server via TCP Socket.
    *   Receives responses and updates the interface in real-time.
    *   Uses Qt's signal-slot mechanism to synchronize UI and data.
*   **Server (C++):**
    *   **Socket Listener:** Constantly listens for new connections from clients.
    *   **Session Manager:** Manages sessions and the state of each logged-in user.
    *   **Game Logic Handler:** Handles the logic for game rooms and matches.
    *   **Database Interface (ORM):** Communicates with the SQLite database for reading/writing data.
*   **Database (SQLite):** Stores user information, questions, match history, and statistics.