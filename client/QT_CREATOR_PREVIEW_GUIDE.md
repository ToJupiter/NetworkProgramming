# Hướng dẫn Preview QML UI trong Qt Creator

## Phương pháp 1: Sử dụng Qt Creator Design Mode (Khuyến nghị - KHÔNG CẦN CÀI THÊM GÌ)

**Đây là cách đơn giản nhất và không cần cài đặt thêm package nào!**

### Bước 1: Mở Qt Creator
```bash
qtcreator
```

### Bước 2: Mở file QML
1. **File** → **Open File or Project** (Ctrl+O)
2. Chọn file QML bạn muốn preview từ thư mục `client/qml_preview/`
   - `preview_MenuStart.qml`
   - `preview_LoginPage.qml`
   - `preview_SignUpPage.qml`
   - `preview_MenuMain.qml`
   - `preview_GameScreen.qml`
   - `preview_ChangePasswordPage.qml`

### Bước 3: Preview UI
Có 3 cách để preview:

#### Cách A: Design Mode
- Click vào tab **"Design"** ở dưới editor
- Hoặc nhấn `Alt+Shift+D`
- UI sẽ hiển thị ở bên phải editor

#### Cách B: QML Preview
- Click chuột phải vào file QML trong editor
- Chọn **"Preview"** hoặc **"Open QML Preview"**
- Hoặc nhấn `Ctrl+Shift+P`

#### Cách C: Run
- Nhấn `Ctrl+R` để chạy file QML
- Hoặc click nút **Run** (mũi tên xanh)

### Bước 4: Tương tác với UI
- Trong Design mode, bạn có thể:
  - Xem UI trực quan
  - Thay đổi properties trong **Properties** panel
  - Xem component hierarchy trong **Navigator**
  - Test các animation và transitions

---

## Phương pháp 2: Sử dụng qmlscene (Command Line) - Tùy chọn

**Lưu ý:** Qt Creator có thể preview QML mà không cần cài qmlscene. Chỉ cần cài nếu bạn muốn preview từ command line.

### Cài đặt qmlscene (nếu chưa có)
```bash
# Ubuntu/Debian
# Đợi hệ thống hoàn tất cập nhật tự động trước
# Kiểm tra process: ps aux | grep unattended-upgr
# Sau đó:
sudo apt-get install qt5-qmltooling-plugins

# Hoặc với Qt5 đầy đủ
sudo apt-get install qtdeclarative5-dev
```

### Nếu gặp lỗi lock (dpkg đang được sử dụng):
```bash
# Kiểm tra process đang chạy
ps aux | grep unattended-upgr

# Đợi process hoàn tất (thường 5-10 phút)
# Hoặc nếu cần thiết, có thể kill (không khuyến nghị):
# sudo killall unattended-upgr
# Nhưng tốt nhất là đợi nó hoàn tất
```

### Preview từng màn hình
```bash
cd /home/khoi/NetworkProgramming/client/qml_preview

# Preview MenuStart
qmlscene preview_MenuStart.qml

# Preview LoginPage
qmlscene preview_LoginPage.qml

# Preview SignUpPage
qmlscene preview_SignUpPage.qml

# Preview MenuMain
qmlscene preview_MenuMain.qml

# Preview GameScreen
qmlscene preview_GameScreen.qml

# Preview ChangePasswordPage
qmlscene preview_ChangePasswordPage.qml
```

### Sử dụng script tự động
```bash
cd /home/khoi/NetworkProgramming/client/qml_preview
./preview_all.sh
```

Script sẽ hiển thị menu để bạn chọn màn hình muốn preview.

---

## Phương pháp 3: Preview trong Qt Creator với Project

### Tạo Qt Quick Project tạm thời

1. **File** → **New File or Project**
2. Chọn **Qt Quick Application**
3. Đặt tên project: `QMLPreview`
4. Chọn location: `/home/khoi/NetworkProgramming/client/qml_preview/`
5. Trong file `main.qml`, thay đổi nội dung để load file preview:

```qml
import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    visible: true
    width: 480
    height: 640
    
    Loader {
        anchors.fill: parent
        source: "preview_MenuStart.qml"
    }
}
```

6. Build và Run project (Ctrl+R)

---

## Danh sách các màn hình có thể preview

| File Preview | Mô tả |
|--------------|-------|
| `preview_MenuStart.qml` | Màn hình bắt đầu với logo và menu chính |
| `preview_LoginPage.qml` | Trang đăng nhập với form nhập username/password |
| `preview_SignUpPage.qml` | Trang đăng ký với form đầy đủ |
| `preview_MenuMain.qml` | Menu chính sau khi đăng nhập |
| `preview_GameScreen.qml` | Màn hình game với câu hỏi và các lựa chọn |
| `preview_ChangePasswordPage.qml` | Trang đổi mật khẩu |

---

## Troubleshooting

### Lỗi: "Cannot find module"
- Đảm bảo đã cài đặt Qt5 Quick và Qt5 Quick Controls 2
- Kiểm tra QML2_IMPORT_PATH environment variable

### Lỗi: "applicationDirPath is not defined"
- Preview files đã có mock `applicationDirPath = "."`
- Nếu vẫn lỗi, set environment variable:
  ```bash
  export QML2_IMPORT_PATH=/path/to/client/assets
  ```

### Assets không hiển thị
- Preview files sử dụng mock data, không cần assets
- Nếu muốn hiển thị assets thật, copy thư mục `assets/` vào cùng thư mục với preview file

### Font không hiển thị đúng
- FontAwesome và Roboto fonts được load từ assets
- Preview files có thể không hiển thị font đặc biệt, nhưng layout vẫn đúng

### Qt Creator không mở được Design mode
- Đảm bảo đã cài Qt Creator với QML Designer plugin
- Tools → Options → Qt Quick → QML Designer → Enable QML Designer

---

## Tips & Tricks

1. **Live Preview**: Trong Design mode, thay đổi code sẽ tự động cập nhật preview
2. **Properties Panel**: Thay đổi properties trực tiếp trong Design mode để test
3. **Component Inspector**: Xem component tree và properties của từng element
4. **State Editor**: Test các states khác nhau của UI
5. **Animation Preview**: Xem animation trong Design mode

---

## Quick Reference

| Action | Shortcut |
|--------|----------|
| Open Design Mode | `Alt+Shift+D` |
| QML Preview | `Ctrl+Shift+P` |
| Run | `Ctrl+R` |
| Build | `Ctrl+B` |
| Open File | `Ctrl+O` |

---

## Next Steps

Sau khi preview các UI, bạn có thể:
1. Chỉnh sửa trực tiếp trong Qt Creator
2. Test các tương tác
3. Điều chỉnh layout và styling
4. Export screenshots để documentation

