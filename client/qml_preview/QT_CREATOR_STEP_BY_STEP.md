# Hướng Dẫn Sử Dụng Qt Creator - Từng Bước Một

## 📚 Mục Lục
1. [Mở Qt Creator](#1-mở-qt-creator)
2. [Mở File QML](#2-mở-file-qml)
3. [Preview QML](#3-preview-qml)
4. [Design Mode](#4-design-mode)
5. [Tips & Tricks](#5-tips--tricks)

---

## 1. Mở Qt Creator

### Cách 1: Từ Terminal
```bash
qtcreator
```

### Cách 2: Từ Menu Applications
- Click **Applications** (hoặc Super key)
- Tìm **Qt Creator**
- Click để mở

### Cách 3: Từ File Manager
- Điều hướng đến thư mục có file QML
- Click chuột phải vào file `.qml`
- Chọn **Open With** → **Qt Creator**

---

## 2. Mở File QML

### Bước 1: Mở File Dialog
Có 3 cách:

**Cách A: Menu**
- Click **File** ở menu bar trên cùng
- Chọn **Open File or Project...**
- Hoặc nhấn phím tắt: `Ctrl+O`

**Cách B: Phím Tắt**
- Nhấn `Ctrl+O` trực tiếp

**Cách C: Welcome Screen**
- Nếu thấy Welcome screen khi mở Qt Creator
- Click **Open** ở bên trái
- Chọn **Open File or Project...**

### Bước 2: Chọn File
1. Trong file dialog, điều hướng đến:
   ```
   /home/khoi/NetworkProgramming/client/qml_preview/
   ```

2. Chọn một trong các file preview:
   - `preview_MenuStart.qml` (Màn hình bắt đầu)
   - `preview_LoginPage.qml` (Trang đăng nhập)
   - `preview_SignUpPage.qml` (Trang đăng ký)
   - `preview_MenuMain.qml` (Menu chính)
   - `preview_GameScreen.qml` (Màn hình game)
   - `preview_ChangePasswordPage.qml` (Đổi mật khẩu)

3. Click **Open**

### Bước 3: File Đã Mở
- File sẽ mở trong editor ở giữa màn hình
- Bạn sẽ thấy code QML

---

## 3. Preview QML

Có 3 cách để preview QML file:

### Cách 1: QML Preview (Khuyến Nghị - Nhanh Nhất)

**Bước 1:** Đảm bảo file QML đang được chọn trong editor

**Bước 2:** Nhấn phím tắt:
```
Ctrl+Shift+P
```

**Hoặc:**
- Click chuột phải vào file trong editor
- Chọn **Preview** hoặc **Open QML Preview**

**Bước 3:** Cửa sổ preview sẽ xuất hiện!
- Bạn sẽ thấy UI của màn hình
- Có thể tương tác với các button, text field, etc.

**Bước 4:** Đóng preview
- Click nút X ở góc trên bên phải cửa sổ preview
- Hoặc nhấn `Esc`

### Cách 2: Design Mode (Tốt Nhất Cho Design)

**Bước 1:** Click tab **"Design"** ở dưới editor
- Tab này nằm giữa **"Edit"** và **"Help"**

**Hoặc nhấn phím tắt:**
```
Alt+Shift+D
```

**Bước 2:** Màn hình sẽ chia làm 2 phần:
- **Bên trái:** Code editor
- **Bên phải:** Design view (preview UI)

**Bước 3:** Trong Design mode, bạn có thể:
- Xem UI trực quan
- Thay đổi properties trong **Properties** panel (bên phải)
- Xem component hierarchy trong **Navigator** panel (bên trái)
- Drag & drop components (nếu có)

**Bước 4:** Quay lại Edit mode
- Click tab **"Edit"** hoặc nhấn `Alt+Shift+E`

### Cách 3: Run (Chạy Như Ứng Dụng)

**Bước 1:** Nhấn phím tắt:
```
Ctrl+R
```

**Hoặc:**
- Click nút **Run** (mũi tên xanh) ở thanh toolbar bên trái

**Bước 2:** Cửa sổ ứng dụng sẽ mở
- Chạy như ứng dụng thật
- Có thể tương tác đầy đủ

**Bước 3:** Đóng ứng dụng
- Click nút X ở cửa sổ ứng dụng
- Hoặc nhấn `Esc` trong Qt Creator

---

## 4. Design Mode - Chi Tiết

### Layout của Design Mode:

```
┌─────────────────────────────────────────────────────────┐
│  [Edit] [Design] [Help]                                  │
├──────────────┬────────────────────────────────────────────┤
│              │                                            │
│  Navigator   │         Design View (Preview)             │
│  (Component  │         (UI hiển thị ở đây)               │
│   Tree)      │                                            │
│              │                                            │
├──────────────┼────────────────────────────────────────────┤
│  Code Editor │         Properties Panel                  │
│  (QML Code)  │         (Thay đổi properties ở đây)       │
│              │                                            │
└──────────────┴───────────────────────────────────────────┘
```

### Sử Dụng Properties Panel:

1. **Chọn component** trong Navigator hoặc Design view
2. **Properties panel** sẽ hiển thị các properties của component đó
3. **Thay đổi properties** trực tiếp:
   - Width, Height
   - Color
   - Text
   - Anchors
   - etc.

### Sử Dụng Navigator:

- Hiển thị cấu trúc component tree
- Click vào component để chọn
- Click chuột phải để xem context menu

---

## 5. Tips & Tricks

### Tip 1: Live Editing
- Khi bạn thay đổi code trong editor
- Preview sẽ tự động cập nhật (trong Design mode)
- Không cần nhấn Run lại!

### Tip 2: Multiple Previews
- Bạn có thể mở nhiều file preview cùng lúc
- Mỗi file một tab
- Preview từng file một

### Tip 3: Zoom Preview
- Trong Design mode, bạn có thể zoom in/out
- Sử dụng scroll wheel trên preview area
- Hoặc dùng zoom controls ở toolbar

### Tip 4: Switch Between Files
- Click vào tab của file khác để chuyển
- Hoặc dùng `Ctrl+Tab` để switch giữa các tab

### Tip 5: Split View
- View → Views → Split
- Xem code và preview cùng lúc

### Tip 6: Full Screen Preview
- Trong Design mode, double-click vào preview area
- Hoặc nhấn `F11` để full screen

---

## 6. Troubleshooting

### Vấn Đề: Không thấy tab "Design"
**Giải pháp:**
- Đảm bảo file là `.qml` file
- Qt Creator có thể cần restart
- Kiểm tra View → Views → Design

### Vấn Đề: Preview không hiển thị
**Giải pháp:**
- Kiểm tra console ở dưới (có thể có lỗi)
- Đảm bảo file QML hợp lệ
- Thử Run thay vì Preview

### Vấn Đề: Properties panel không hiển thị
**Giải pháp:**
- View → Views → Properties
- Hoặc Window → Views → Properties

### Vấn Đề: Navigator không hiển thị
**Giải pháp:**
- View → Views → Navigator
- Hoặc Window → Views → Navigator

---

## 7. Phím Tắt Quan Trọng

| Phím Tắt | Chức Năng |
|----------|-----------|
| `Ctrl+O` | Open File |
| `Ctrl+Shift+P` | QML Preview |
| `Alt+Shift+D` | Design Mode |
| `Alt+Shift+E` | Edit Mode |
| `Ctrl+R` | Run |
| `Ctrl+B` | Build (không cần cho preview) |
| `F11` | Full Screen |
| `Esc` | Đóng preview/dialog |

---

## 8. Workflow Đề Xuất

### Để Preview Tất Cả Các Màn Hình:

1. **Mở Qt Creator**
   ```bash
   qtcreator
   ```

2. **Mở file đầu tiên**
   - `Ctrl+O` → Chọn `preview_MenuStart.qml`
   - `Ctrl+Shift+P` để preview
   - Xem UI
   - Đóng preview (`Esc`)

3. **Mở file tiếp theo**
   - `Ctrl+O` → Chọn `preview_LoginPage.qml`
   - `Ctrl+Shift+P` để preview
   - Xem UI
   - Đóng preview

4. **Lặp lại** cho các file khác:
   - `preview_SignUpPage.qml`
   - `preview_MenuMain.qml`
   - `preview_GameScreen.qml`
   - `preview_ChangePasswordPage.qml`

### Hoặc Dùng Design Mode:

1. **Mở Qt Creator**
2. **Mở file**: `Ctrl+O` → Chọn preview file
3. **Design Mode**: `Alt+Shift+D`
4. **Xem UI** ở bên phải
5. **Thay đổi properties** nếu muốn test
6. **Chuyển file**: `Ctrl+O` → Chọn file khác
7. **Lặp lại**

---

## 9. Ví Dụ Cụ Thể

### Preview `preview_MenuStart.qml`:

```bash
# 1. Mở terminal
cd ~/NetworkProgramming/client/qml_preview

# 2. Mở Qt Creator
qtcreator

# 3. Trong Qt Creator:
#    - Nhấn Ctrl+O
#    - Điều hướng đến: /home/khoi/NetworkProgramming/client/qml_preview/
#    - Chọn: preview_MenuStart.qml
#    - Click Open

# 4. Preview:
#    - Nhấn Ctrl+Shift+P
#    - Hoặc Alt+Shift+D cho Design mode

# 5. Bạn sẽ thấy:
#    - Logo placeholder
#    - Text "Ai Là Triệu Phú"
#    - 4 buttons: Bắt đầu, Hướng dẫn, Giới thiệu, Thoát
```

---

## 10. Tóm Tắt Nhanh

**Để preview QML file:**

1. Mở Qt Creator: `qtcreator`
2. Mở file: `Ctrl+O` → Chọn file `.qml`
3. Preview: `Ctrl+Shift+P`
4. Xong!

**Hoặc Design Mode:**

1. Mở Qt Creator: `qtcreator`
2. Mở file: `Ctrl+O` → Chọn file `.qml`
3. Design Mode: `Alt+Shift+D`
4. Xem UI ở bên phải
5. Xong!

---

## ✅ Kết Luận

Qt Creator rất dễ sử dụng để preview QML:
- **Không cần build** project
- **Không cần setup** gì cả
- **Preview ngay lập tức** với `Ctrl+Shift+P`
- **Design mode** để xem và chỉnh sửa UI

**Chúc bạn preview thành công!** 🎉

