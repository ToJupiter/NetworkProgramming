# Quick Tutorial: Preview QML trong Qt Creator (5 Phút)

## 🎯 Mục Tiêu
Preview file QML trong Qt Creator mà không cần build project.

---

## ⚡ Các Bước Nhanh (30 Giây)

### Bước 1: Mở Qt Creator
```bash
qtcreator
```

### Bước 2: Mở File QML
- Nhấn `Ctrl+O`
- Điều hướng đến: `/home/khoi/NetworkProgramming/client/qml_preview/`
- Chọn: `preview_MenuStart.qml`
- Click **Open**

### Bước 3: Preview
- Nhấn `Ctrl+Shift+P`
- **Xong!** UI sẽ hiển thị

---

## 📖 Giải Thích Chi Tiết

### 1. Mở Qt Creator

**Cách 1: Terminal**
```bash
qtcreator
```

**Cách 2: Applications Menu**
- Click **Applications** (hoặc Super key)
- Tìm **Qt Creator**
- Click để mở

### 2. Mở File QML

**Trong Qt Creator:**
1. Nhấn `Ctrl+O` (hoặc File → Open File)
2. File dialog sẽ mở
3. Điều hướng đến thư mục:
   ```
   /home/khoi/NetworkProgramming/client/qml_preview/
   ```
4. Chọn file (ví dụ: `preview_MenuStart.qml`)
5. Click **Open**

**File sẽ mở trong editor ở giữa màn hình**

### 3. Preview QML

**Có 3 cách:**

#### Cách 1: QML Preview (Nhanh Nhất) ⭐
- Nhấn `Ctrl+Shift+P`
- Cửa sổ preview sẽ mở
- Xem UI ngay lập tức

#### Cách 2: Design Mode (Tốt Nhất)
- Nhấn `Alt+Shift+D`
- Hoặc click tab **"Design"** ở dưới editor
- UI hiển thị ở bên phải
- Có thể thay đổi properties

#### Cách 3: Run
- Nhấn `Ctrl+R`
- Chạy như ứng dụng thật

---

## 🎨 Design Mode - Chi Tiết Hơn

### Layout:
```
┌─────────────────────────────────────┐
│  [Edit] [Design] [Help]             │
├──────────┬──────────────────────────┤
│          │                          │
│ Navigator│    Design View (UI)      │
│ (Tree)   │    (Preview ở đây)        │
│          │                          │
├──────────┼──────────────────────────┤
│  Code    │    Properties Panel      │
│  Editor  │    (Thay đổi ở đây)       │
│          │                          │
└──────────┴──────────────────────────┘
```

### Sử Dụng:
1. **Chọn component** trong Navigator (bên trái)
2. **Xem properties** trong Properties panel (bên phải)
3. **Thay đổi properties** để test
4. **Xem UI** tự động cập nhật

---

## 📋 Preview Tất Cả Các Màn Hình

### Danh Sách Files:
- `preview_MenuStart.qml` - Màn hình bắt đầu
- `preview_LoginPage.qml` - Trang đăng nhập
- `preview_SignUpPage.qml` - Trang đăng ký
- `preview_MenuMain.qml` - Menu chính
- `preview_GameScreen.qml` - Màn hình game
- `preview_ChangePasswordPage.qml` - Đổi mật khẩu

### Workflow:
1. Mở Qt Creator: `qtcreator`
2. Mở file đầu tiên: `Ctrl+O` → Chọn `preview_MenuStart.qml`
3. Preview: `Ctrl+Shift+P`
4. Xem UI
5. Đóng preview: `Esc` hoặc click X
6. Mở file tiếp theo: `Ctrl+O` → Chọn file khác
7. Lặp lại

---

## ⌨️ Phím Tắt Quan Trọng

| Phím Tắt | Chức Năng |
|----------|-----------|
| `Ctrl+O` | Mở file |
| `Ctrl+Shift+P` | Preview QML |
| `Alt+Shift+D` | Design Mode |
| `Ctrl+R` | Run |
| `Esc` | Đóng preview |

---

## ❓ Troubleshooting

### Không thấy tab "Design"?
- Đảm bảo file là `.qml`
- Thử restart Qt Creator

### Preview không hiển thị?
- Kiểm tra console ở dưới (có thể có lỗi)
- Thử Run (`Ctrl+R`) thay vì Preview

### Properties panel không hiển thị?
- View → Views → Properties
- Hoặc Window → Views → Properties

---

## ✅ Tóm Tắt

**3 Bước Đơn Giản:**
1. `qtcreator` - Mở Qt Creator
2. `Ctrl+O` - Mở file QML
3. `Ctrl+Shift+P` - Preview

**Xong!** 🎉

---

## 📚 Tài Liệu Tham Khảo

- `QT_CREATOR_STEP_BY_STEP.md` - Hướng dẫn chi tiết từng bước
- `SIMPLE_PREVIEW.md` - Hướng dẫn đơn giản
- `QT_CREATOR_PREVIEW_GUIDE.md` - Hướng dẫn đầy đủ

