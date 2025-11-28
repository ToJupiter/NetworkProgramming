stateDiagram-v2
    [*] --> Đăng nhập: Kết nối thông qua TCP
    Đăng nhập --> Đăng ký: Yêu cầu đăng ký
    Đăng ký --> Đăng nhập: Đăng ký thất bại
    Đăng ký --> Menu: Đăng ký thành công
    Đăng nhập --> Menu: Đăng nhập thành công
    Đăng nhập --> [*]: Thoát trò chơi

    Menu --> Vào phòng hoặc tạo phòng mới: Bắt đầu vào chơi
    Menu --> Lịch sử chơi: Hiển thị lịch sử chơi
    Menu --> Chơi game: Lưu kết quả từng người

    Vào phòng hoặc tạo phòng mới --> Danh sách phòng: Tìm phòng
    Vào phòng hoặc tạo phòng mới --> Tạo phòng: Tạo phòng

    Tạo phòng --> Phòng chờ: Tạo thành công
    Danh sách phòng --> Phòng chờ: Tham gia phòng

    Phòng chờ --> Chơi game: Hết thời gian/Đủ người

    Chơi game --> Menu