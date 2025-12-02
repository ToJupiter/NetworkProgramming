# Fix: qmlscene "could not find a Qt installation"

## ✅ Giải Pháp 1: Dùng Qt Creator (Khuyến Nghị - Dễ Nhất)

**Không cần fix gì cả!** Qt Creator có thể preview QML mà không cần `qmlscene`.

### Cách làm:
```bash
# Mở Qt Creator
qtcreator

# Hoặc nếu không có trong PATH:
/usr/bin/qtcreator
```

Sau đó:
1. **File** → **Open File** (Ctrl+O)
2. Chọn `preview_MenuStart.qml`
3. Nhấn `Ctrl+Shift+P` để preview
4. **Xong!** Không cần setup gì

---

## 🔧 Giải Pháp 2: Fix qmlscene (Nếu Thực Sự Cần)

### Bước 1: Tìm Qt Installation Path
```bash
# Tìm qmake path
which qmake

# Tìm Qt version
qmake -v

# Tìm Qt installation
qmake -query QT_INSTALL_PREFIX
```

### Bước 2: Tìm qmlscene
```bash
# Tìm qmlscene
find /usr -name qmlscene 2>/dev/null
find /opt -name qmlscene 2>/dev/null

# Hoặc kiểm tra package
dpkg -L qtdeclarative5-dev | grep qmlscene
dpkg -L qt5-qmltooling-plugins | grep qmlscene
```

### Bước 3: Set Environment Variables
```bash
# Tìm Qt path
QT_PATH=$(qmake -query QT_INSTALL_PREFIX)

# Set environment variables
export QTDIR=$QT_PATH
export PATH=$QT_PATH/bin:$PATH
export LD_LIBRARY_PATH=$QT_PATH/lib:$LD_LIBRARY_PATH
export QML2_IMPORT_PATH=$QT_PATH/qml:$QML2_IMPORT_PATH

# Thử lại
qmlscene preview_MenuStart.qml
```

### Bước 4: Tạo Script Wrapper (Permanent Fix)
Tạo file `~/preview_qml.sh`:
```bash
#!/bin/bash
QT_PATH=$(qmake -query QT_INSTALL_PREFIX 2>/dev/null || echo "/usr")
export QTDIR=$QT_PATH
export PATH=$QT_PATH/bin:$PATH
export LD_LIBRARY_PATH=$QT_PATH/lib:$LD_LIBRARY_PATH
export QML2_IMPORT_PATH=$QT_PATH/qml:$QML2_IMPORT_PATH

# Tìm qmlscene
QMLSCENE=$(which qmlscene 2>/dev/null || find /usr -name qmlscene 2>/dev/null | head -1)

if [ -z "$QMLSCENE" ]; then
    echo "Error: qmlscene not found. Please install:"
    echo "  sudo apt-get install qtdeclarative5-dev"
    exit 1
fi

$QMLSCENE "$@"
```

Chmod và sử dụng:
```bash
chmod +x ~/preview_qml.sh
~/preview_qml.sh preview_MenuStart.qml
```

---

## 📦 Giải Pháp 3: Cài Đặt qmlscene (Nếu Chưa Có)

```bash
# Đợi hệ thống hoàn tất cập nhật tự động
# Kiểm tra: ps aux | grep unattended-upgr

# Sau đó cài:
sudo apt-get install qtdeclarative5-dev
# hoặc
sudo apt-get install qt5-qmltooling-plugins
```

---

## 🎯 Khuyến Nghị

**Dùng Qt Creator thay vì qmlscene:**
- ✅ Không cần setup environment variables
- ✅ Preview tốt hơn với Design mode
- ✅ Live editing khi code thay đổi
- ✅ Properties panel để test
- ✅ Component inspector
- ✅ Đã có sẵn trong hệ thống

**Chỉ dùng qmlscene nếu:**
- Bạn muốn preview từ command line
- Không có GUI (headless server)
- Cần automation/scripting

---

## ✅ Quick Test

```bash
# Test Qt Creator
qtcreator --version

# Nếu Qt Creator chạy được, dùng nó!
# Nếu không, fix qmlscene theo Giải Pháp 2
```

