# QML Preview Guide

## ⚠️ Lỗi qmlscene: "could not find a Qt installation"

**Giải pháp tốt nhất: Dùng Qt Creator thay vì qmlscene!**

---

## ✅ Cách 1: Dùng Qt Creator (Khuyến Nghị - Dễ Nhất)

Qt Creator có thể preview QML mà không cần setup gì cả!

```bash
# Mở Qt Creator
qtcreator
```

Sau đó:
1. **File** → **Open File** (Ctrl+O)
2. Chọn file preview (ví dụ: `preview_MenuStart.qml`)
3. Nhấn `Ctrl+Shift+P` để preview
4. **Xong!** Không cần fix gì

**Ưu điểm:**
- ✅ Không cần setup environment variables
- ✅ Preview tốt hơn với Design mode
- ✅ Live editing khi code thay đổi
- ✅ Properties panel để test
- ✅ Component inspector
- ✅ Đã có sẵn trong hệ thống

---

## 🔧 Cách 2: Fix qmlscene (Nếu Thực Sự Cần)

### Option A: Dùng Script Wrapper
```bash
cd ~/NetworkProgramming/client/qml_preview
./preview_qml.sh preview_MenuStart.qml
```

### Option B: Set Environment Manual
```bash
cd ~/NetworkProgramming/client/qml_preview

# Set Qt5 environment
export QTDIR=/usr/lib/x86_64-linux-gnu/qt5
export QML2_IMPORT_PATH=/usr/lib/x86_64-linux-gnu/qt5/qml
export QT_PLUGIN_PATH=/usr/lib/x86_64-linux-gnu/qt5/plugins
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH

# Chạy qmlscene
/usr/bin/qmlscene preview_MenuStart.qml
```

### Option C: Cài Qt5 đầy đủ
```bash
# Đợi hệ thống hoàn tất cập nhật tự động
# Kiểm tra: ps aux | grep unattended-upgr

# Sau đó cài:
sudo apt-get update
sudo apt-get install qt5-qmltooling-plugins qtdeclarative5-dev qtbase5-dev
```

---

## 📋 Danh Sách Preview Files

| File | Màn hình |
|------|---------|
| `preview_MenuStart.qml` | Màn hình bắt đầu |
| `preview_LoginPage.qml` | Trang đăng nhập |
| `preview_SignUpPage.qml` | Trang đăng ký |
| `preview_MenuMain.qml` | Menu chính |
| `preview_GameScreen.qml` | Màn hình game |
| `preview_ChangePasswordPage.qml` | Đổi mật khẩu |

---

## 🎯 Khuyến Nghị

**Dùng Qt Creator:**
- Đơn giản nhất
- Không cần setup
- Preview tốt hơn
- Design mode
- Live editing

**Chỉ dùng qmlscene nếu:**
- Không có GUI
- Cần automation/scripting
- Thực sự cần command line

---

## 📚 Tài Liệu Tham Khảo

- `SIMPLE_PREVIEW.md` - Hướng dẫn preview đơn giản
- `QUICK_FIX.md` - Fix nhanh lỗi qmlscene
- `FIX_QLMSCENE_ERROR.md` - Chi tiết về lỗi qmlscene
- `NO_BUILD_NEEDED.md` - Giải thích tại sao không cần build
- `QT_CREATOR_PREVIEW_GUIDE.md` - Hướng dẫn chi tiết Qt Creator

---

## ✅ Tóm Tắt

**Cách đơn giản nhất:** Dùng Qt Creator (`Ctrl+Shift+P`)

**Cách fix qmlscene:** Dùng script wrapper hoặc set environment variables

**Không cần:** Build project, C++ files, hoặc mainwindow.h

