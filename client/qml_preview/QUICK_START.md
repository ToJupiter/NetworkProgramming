# Quick Start - Preview QML UI

## Cách nhanh nhất: Qt Creator

1. **Mở Qt Creator**
   ```bash
   qtcreator
   ```

2. **Mở file preview**
   - File → Open File (Ctrl+O)
   - Chọn file từ `client/qml_preview/`
   - Ví dụ: `preview_MenuStart.qml`

3. **Preview**
   - Nhấn `Ctrl+Shift+P` (QML Preview)
   - Hoặc `Alt+Shift+D` (Design Mode)
   - Hoặc `Ctrl+R` (Run)

## Các file preview có sẵn

| File | Màn hình |
|------|---------|
| `preview_MenuStart.qml` | Màn hình bắt đầu |
| `preview_LoginPage.qml` | Trang đăng nhập |
| `preview_SignUpPage.qml` | Trang đăng ký |
| `preview_MenuMain.qml` | Menu chính |
| `preview_GameScreen.qml` | Màn hình game |
| `preview_ChangePasswordPage.qml` | Đổi mật khẩu |

## Command Line (qmlscene)

```bash
cd client/qml_preview
qmlscene preview_MenuStart.qml
```

Hoặc dùng script:
```bash
cd client/qml_preview
./preview_all.sh
```

## Lưu ý

- Preview files là standalone, không cần build project
- Có thể preview trực tiếp trong Qt Creator
- Assets (images) có thể không hiển thị, nhưng layout đúng
- Font đặc biệt có thể không load, nhưng không ảnh hưởng layout

