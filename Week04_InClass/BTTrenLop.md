# BÁO CÁO THỰC HÀNH LẬP TRÌNH MẠNG BUỔI 5

  - **Họ và tên:** Phan Hoàng Hải
  - **Mã số sinh viên:** 20225715

-----

### I. Kịch bản 1: Echo cơ bản
- **Thao tác thực hiện:** Chạy 1 server và 1 client trên cùng máy (localhost). Client gửi 3 chuỗi ký tự khác nhau và chờ server phản hồi.

  - **Kết quả ghi nhận:** Client nhận lại chính xác 3 chuỗi đã gửi. Server ghi log đã nhận và phản hồi thành công cho cùng một địa chỉ client.

![Server log](image-1.png)<br>

**Log trên 3 Clients:**
![Client log](image.png)<br>

Wireshark TCP Port 9000:
![Wireshark image](image-2.png)

- **Giải thích:** Client và server thiết lập kết nối TCP bằng bắt tay 3 bước (SYN, SYN-ACK, ACK). Client (`53516`) gửi mỗi chuỗi ký tự bằng lệnh send(), nên stack TCP của client gửi các phân đoạn dữ liệu kèm cờ PSH,ACK để yêu cầu bên nhận đẩy dữ liệu lên ứng dụng ngay lập tức và đồng thời xác nhận thứ tự (ACK). Server nhận qua recv(), in log và gọi send() để trả lại cùng dữ liệu về client (destination port `53516`), do đó stack TCP của server cũng gửi các phân đoạn với PSH,ACK. Vì vậy trong Wireshark sẽ thấy các gói PSH,ACK theo cả hai chiều (PSH là gợi ý đẩy dữ liệu lên ứng dụng, ACK là xác nhận nhận dữ liệu). Vòng lặp recv()/send() trong server đảm bảo mỗi lần nhận được dữ liệu thì sẽ echo trả ngay cho client.

### II. Kịch bản 2: Nhiều client đồng thời

  - **Mục tiêu:** Quan sát cách server TCP xử lý yêu cầu từ nhiều client. Log server: <br>
![Server log](image-4.png)

- **Kết quả ghi nhận:** Log trên server chỉ hiển thị các thông điệp từ client được server chấp nhận và xử lý đầu tiên. <br>
![3 client](image-3.png) <br>

Wireshark TCP Port 9000:
![Wireshark log](image-5.png) <br>

- **Giải thích:**
1. Server đang xử lý client 1 trong vòng lặp recv()/send(), nên không thể gọi accept() để xử lý client 2 và 3. Các client này chỉ tồn tại trong hàng đợi kernel (thấy qua Wireshark), nhưng dữ liệu của chúng không được xử lý cho đến khi client 1 ngắt kết nối.<br>
2. Kernel đã thiết lập kết nối TCP cho cả 3 client (SYN, SYN-ACK, ACK), nhưng mã ứng dụng server không đọc dữ liệu từ client 2 và 3 do đang bận với client 1.

### III. Kịch bản 3: Nhiều bản tin bị gộp tại server

- **Mục tiêu:** Thử nghiệm và quan sát việc server gộp 2 thông điệp gửi liên tiếp.<br>
- **Thao tác thực hiện:** send() 2 chuỗi "bull" và "dog" liên tiếp, quan sát.

Client gửi data: <br>
![client sending bulldog](image-6.png)

Server nhận data:<br>
![bulldog server](image-7.png)

- **Giải thích:**: TCP là một giao thức hướng luồng (stream-oriented), không phải hướng thông điệp (message-oriented). Điều này có nghĩa là:

1. TCP không quan tâm đến ranh giới của các thông điệp. Nó coi tất cả dữ liệu là một chuỗi byte liên tục.<br>
2. Để tối ưu hiệu suất, cơ chế của TCP hoặc bộ đệm của hệ điều hành có thể tạm giữ các gói tin nhỏ và gửi chúng đi cùng lúc trong một gói lớn hơn.<br>
3. Ở phía nhận, recv() sẽ đọc tất cả các byte có sẵn trong bộ đệm TCP của nó tại thời điểm gọi, cho đến khi đầy bộ đệm.

### IV. Kịch bản 4: Dữ liệu gửi lớn

- **Mục tiêu:** Thử nghiệm gửi một gói tin TCP có kích thước lớn (>4kB).

- **Thao tác thực hiện:** Client gửi một file văn bản chứa khoảng 5KB dữ liệu tới server.

Client gửi data: <br>
![data send](image-8.png)
Server nhận data:<br>
![tcp server fragmentation](image-9.png)

- **Giải thích:** Hàm recv(socket, buffer, length, flags) hoạt động như sau:

1. Nó sẽ cố gắng đọc tối đa length byte từ bộ đệm TCP của socket vào buffer.<br>

2. Nếu lượng dữ liệu có sẵn trong bộ đệm TCP lớn hơn length, recv() chỉ đọc đúng length byte và trả về. Phần dữ liệu còn lại vẫn nằm trong bộ đệm TCP, chờ lần recv() tiếp theo.

