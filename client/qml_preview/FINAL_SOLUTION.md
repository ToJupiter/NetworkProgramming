# ✅ Giải Pháp Cuối Cùng: Dùng Qt Creator

## ❌ Vấn Đề Với qmlscene

`/usr/bin/qmlscene` là symbolic link tới `qtchooser`, và `qtchooser` không tìm thấy Qt5 installation. Đây là vấn đề phổ biến trên Ubuntu/Debian.

**Fix qmlscene sẽ phức tạp và không cần thiết!**

---

## ✅ Giải Pháp: Dùng Qt Creator (Đơn Giản Nhất)

Qt Creator có thể preview QML mà **KHÔNG CẦN FIX GÌ CẢ**!

### Các Bước:

```bash
# 1. Mở Qt Creator
qtcreator

# 2. Trong Qt Creator:
#    - File → Open File (Ctrl+O)
#    - Chọn: preview_MenuStart.qml
#    - Nhấn: Ctrl+Shift+P
#    - Xong!
```

### Hoặc Dùng Design Mode:

```bash
qtcreator
```

Sau đó:
1. **File** → **Open File** → Chọn `preview_MenuStart.qml`
2. Click tab **"Design"** ở dưới editor
3. Hoặc nhấn `Alt+Shift+D`
4. UI sẽ hiển thị ở bên phải!

---

## 🎯 Tại Sao Qt Creator Tốt Hơn?

| | qmlscene | Qt Creator |
|---|---|---|
| **Setup** | ❌ Cần fix qtchooser | ✅ Không cần |
| **Preview** | ⚠️ Basic window | ✅ Design mode với properties |
| **Live editing** | ❌ Không | ✅ Tự động cập nhật |
| **Properties panel** | ❌ Không | ✅ Có |
| **Component tree** | ❌ Không | ✅ Có |
| **Debugging** | ❌ Không | ✅ Có |

---

## 📋 Preview Tất Cả Các Màn Hình

1. Mở Qt Creator: `qtcreator`
2. Mở từng file preview:
   - `preview_MenuStart.qml`
   - `preview_LoginPage.qml`
   - `preview_SignUpPage.qml`
   - `preview_MenuMain.qml`
   - `preview_GameScreen.qml`
   - `preview_ChangePasswordPage.qml`
3. Nhấn `Ctrl+Shift+P` để preview từng file

---

## 🔧 Nếu Thực Sự Muốn Fix qmlscene

### Cách 1: Cài Qt5 đầy đủ
```bash
# Đợi hệ thống hoàn tất cập nhật
# Sau đó:
sudo apt-get install qt5-qmltooling-plugins qtdeclarative5-dev qtbase5-dev qt5-default
```

### Cách 2: Tạo wrapper script
```bash
# Tạo script mới
cat > ~/preview_qml.sh << 'EOF'
#!/bin/bash
QT_SELECT=qt5 /usr/bin/qmlscene "$@"
EOF

chmod +x ~/preview_qml.sh
~/preview_qml.sh preview_MenuStart.qml
```

**Nhưng khuyến nghị: Dùng Qt Creator thay vì fix qmlscene!**

---

## ✅ Tóm Tắt

**Cách đơn giản nhất:**
1. Mở Qt Creator: `qtcreator`
2. File → Open File → Chọn preview file
3. Nhấn `Ctrl+Shift+P`
4. **Xong!**

**Không cần:**
- ❌ Fix qtchooser
- ❌ Setup environment variables
- ❌ Cài thêm packages
- ❌ Troubleshoot qmlscene

**Kết luận:** Qt Creator là giải pháp tốt nhất! 🎉

