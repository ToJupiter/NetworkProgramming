# Hướng dẫn Preview QML UI trong Qt Creator

## Cách 1: Sử dụng Qt Creator QML Preview (Khuyến nghị)

### Bước 1: Mở file QML trong Qt Creator
1. Mở Qt Creator
2. File → Open File or Project
3. Chọn file QML bạn muốn preview (ví dụ: `MenuStart.qml`, `LoginPage.qml`, etc.)
4. File sẽ mở trong editor

### Bước 2: Preview QML
1. Click chuột phải vào file QML trong editor
2. Chọn **"Preview"** hoặc **"Design"** mode
3. Hoặc nhấn `Alt+Shift+D` để mở Design mode
4. Hoặc nhấn `Ctrl+Shift+P` để preview

### Bước 3: Preview với qmlscene (Command line)
```bash
# Di chuyển đến thư mục client
cd /home/khoi/NetworkProgramming/client

# Preview một file QML cụ thể
qmlscene qml/MenuStart.qml
qmlscene qml/LoginPage.qml
qmlscene qml/SignUpPage.qml
```

**Lưu ý**: Các file QML gốc có thể không preview được trực tiếp vì chúng phụ thuộc vào:
- BackEnd C++ class
- Context properties (applicationDirPath)
- StackView navigation

## Cách 2: Sử dụng Preview Files (Đã tạo sẵn)

Tôi đã tạo các file preview độc lập trong thư mục `qml_preview/`:
- `preview_MenuStart.qml` - Preview màn hình bắt đầu
- `preview_LoginPage.qml` - Preview trang đăng nhập
- `preview_SignUpPage.qml` - Preview trang đăng ký
- `preview_MenuMain.qml` - Preview menu chính
- `preview_GameScreen.qml` - Preview màn hình game
- `preview_ChangePasswordPage.qml` - Preview trang đổi mật khẩu

### Cách sử dụng Preview Files:
```bash
cd /home/khoi/NetworkProgramming/client

# Preview từng màn hình
qmlscene qml_preview/preview_MenuStart.qml
qmlscene qml_preview/preview_LoginPage.qml
qmlscene qml_preview/preview_SignUpPage.qml
qmlscene qml_preview/preview_MenuMain.qml
qmlscene qml_preview/preview_GameScreen.qml
qmlscene qml_preview/preview_ChangePasswordPage.qml
```

Hoặc mở trực tiếp trong Qt Creator:
1. File → Open File
2. Chọn file trong `qml_preview/`
3. Nhấn `Ctrl+Shift+P` để preview

## Cách 3: Build và chạy ứng dụng đầy đủ

```bash
cd client
mkdir -p build && cd build
cmake ..
make
./client
```

## Troubleshooting

### Lỗi: "Cannot find module AiLaTrieuPhu.BackEnd"
- Preview files đã có mock BackEnd, không cần lo lắng
- Chỉ cần dùng các file trong `qml_preview/`

### Lỗi: "Cannot find applicationDirPath"
- Preview files đã set giá trị mặc định
- Hoặc set environment variable: `export QML2_IMPORT_PATH=/path/to/assets`

### Lỗi: Assets không hiển thị
- Đảm bảo đường dẫn đến assets đúng
- Hoặc copy assets vào cùng thư mục với preview file

