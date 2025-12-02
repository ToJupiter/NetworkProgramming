# Preview Files - Không Cần Build Project

## ✅ Câu Trả Lời Ngắn Gọn

**KHÔNG**, bạn **KHÔNG CẦN**:
- ❌ `mainwindow.h`
- ❌ `BackEnd.h` 
- ❌ Bất kỳ file C++ nào
- ❌ Build project
- ❌ CMake
- ❌ Make

## ✅ Preview Files Là Gì?

Các file preview (`preview_*.qml`) là **standalone QML files** được thiết kế để:
- Chạy độc lập không cần C++ backend
- Preview trực tiếp trong Qt Creator
- Không cần compile hay build

## ✅ Cấu Trúc Preview Files

```qml
import QtQuick 2.12
import QtQuick.Controls 2.5
// Chỉ import Qt modules, KHÔNG import custom C++ types

ApplicationWindow {
    // Mock data thay vì BackEnd thật
    QtObject {
        id: mockBackEnd
        property string user_name: "Nguyễn Minh Khôi"
        // ...
    }
    
    // Standalone UI, không cần C++ backend
}
```

## ✅ So Sánh: Preview vs Real Project

| | Preview Files | Real Project Files |
|---|---|---|
| **C++ Files** | ❌ Không cần | ✅ Cần (BackEnd.h, main.cpp) |
| **Build** | ❌ Không cần | ✅ Cần (CMake, Make) |
| **BackEnd** | Mock data (QtObject) | Real C++ class |
| **Import** | Chỉ Qt modules | Import custom types |
| **Chạy** | Trực tiếp trong Qt Creator | Cần build và run |

## ✅ Cách Sử Dụng Preview Files

### Trong Qt Creator:
1. **File** → **Open File** (Ctrl+O)
2. Chọn file preview (ví dụ: `preview_MenuStart.qml`)
3. Nhấn `Ctrl+Shift+P` để preview
4. **Xong!** Không cần build gì cả

### Command Line (nếu có qmlscene):
```bash
qmlscene preview_MenuStart.qml
# Không cần build, không cần C++ files
```

## ✅ Tại Sao Preview Files Hoạt Động?

1. **Standalone QML**: Chỉ dùng Qt Quick Controls, không cần custom C++ types
2. **Mock Data**: Dùng `QtObject` để mock data thay vì `BackEnd` thật
3. **No Dependencies**: Không import `AiLaTrieuPhu.BackEnd` hay bất kỳ custom type nào

## ✅ Khi Nào Cần Build Project?

Chỉ khi bạn muốn:
- ✅ Chạy ứng dụng thật (với BackEnd C++ thật)
- ✅ Test kết nối server
- ✅ Test authentication thật
- ✅ Test game logic thật

**Nhưng để preview UI, KHÔNG CẦN BUILD!**

## ✅ Troubleshooting

### Lỗi: "Cannot find module AiLaTrieuPhu.BackEnd"
- **Nguyên nhân**: Preview file đang import custom C++ type
- **Giải pháp**: Đảm bảo preview file không import `AiLaTrieuPhu.BackEnd`
- **Kiểm tra**: Tất cả preview files đã được thiết kế standalone

### Lỗi: "applicationDirPath is not defined"
- **Nguyên nhân**: Preview file cần `applicationDirPath`
- **Giải pháp**: Preview files đã có `property string applicationDirPath: "."`
- **Không cần lo**: Đã được handle sẵn

## ✅ Tóm Tắt

```
Preview Files = Pure QML = No C++ = No Build = Just Preview!
```

**Chỉ cần Qt Creator và mở file preview là xong!** 🎉

