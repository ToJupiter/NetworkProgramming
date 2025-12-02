# Quick Fix: qmlscene Error

## ✅ Giải Pháp Nhanh Nhất: Dùng Qt Creator

**Thay vì fix qmlscene, hãy dùng Qt Creator - đơn giản hơn nhiều!**

```bash
# Mở Qt Creator
qtcreator

# Hoặc
/usr/bin/qtcreator
```

Sau đó:
1. **File** → **Open File** (Ctrl+O)
2. Chọn file preview (ví dụ: `preview_MenuStart.qml`)
3. Nhấn `Ctrl+Shift+P` để preview
4. **Xong!**

---

## 🔧 Nếu Thực Sự Cần qmlscene

Lỗi này thường xảy ra khi Qt5 không được cài đặt đầy đủ hoặc environment variables chưa được set.

### Cách 1: Cài Qt5 đầy đủ
```bash
# Đợi hệ thống hoàn tất cập nhật tự động
# Kiểm tra: ps aux | grep unattended-upgr

# Sau đó cài:
sudo apt-get update
sudo apt-get install qt5-qmltooling-plugins qtdeclarative5-dev qtbase5-dev
```

### Cách 2: Set Environment và chạy
```bash
cd ~/NetworkProgramming/client/qml_preview

# Set Qt5 environment
export QTDIR=/usr
export QML2_IMPORT_PATH=/usr/lib/x86_64-linux-gnu/qt5/qml
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH

# Chạy với full path
/usr/bin/qmlscene preview_MenuStart.qml
```

### Cách 3: Dùng script wrapper
```bash
cd ~/NetworkProgramming/client/qml_preview
./preview_qml.sh preview_MenuStart.qml
```

---

## 💡 Khuyến Nghị

**Dùng Qt Creator thay vì qmlscene:**
- ✅ Không cần setup
- ✅ Preview tốt hơn
- ✅ Design mode
- ✅ Live editing
- ✅ Đã có sẵn

**Chỉ dùng qmlscene nếu:**
- Không có GUI
- Cần automation
- Thực sự cần command line

---

## 🎯 Tóm Tắt

**Cách đơn giản nhất:** Dùng Qt Creator (`Ctrl+Shift+P`)

**Cách fix qmlscene:** Cài đầy đủ Qt5 packages hoặc set environment variables

