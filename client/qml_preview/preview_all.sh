#!/bin/bash

# Script to preview all QML UI screens
# Usage: ./preview_all.sh

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CLIENT_DIR="$(dirname "$SCRIPT_DIR")"

echo "=========================================="
echo "QML UI Preview Script"
echo "=========================================="
echo ""
echo "Available preview screens:"
echo "1. MenuStart - Màn hình bắt đầu"
echo "2. LoginPage - Trang đăng nhập"
echo "3. SignUpPage - Trang đăng ký"
echo "4. MenuMain - Menu chính"
echo "5. GameScreen - Màn hình game"
echo "6. ChangePasswordPage - Trang đổi mật khẩu"
echo ""
echo "Enter number to preview (1-6), or 'all' to preview all, or 'q' to quit:"

read choice

case $choice in
    1)
        echo "Previewing MenuStart..."
        qmlscene "$SCRIPT_DIR/preview_MenuStart.qml"
        ;;
    2)
        echo "Previewing LoginPage..."
        qmlscene "$SCRIPT_DIR/preview_LoginPage.qml"
        ;;
    3)
        echo "Previewing SignUpPage..."
        qmlscene "$SCRIPT_DIR/preview_SignUpPage.qml"
        ;;
    4)
        echo "Previewing MenuMain..."
        qmlscene "$SCRIPT_DIR/preview_MenuMain.qml"
        ;;
    5)
        echo "Previewing GameScreen..."
        qmlscene "$SCRIPT_DIR/preview_GameScreen.qml"
        ;;
    6)
        echo "Previewing ChangePasswordPage..."
        qmlscene "$SCRIPT_DIR/preview_ChangePasswordPage.qml"
        ;;
    all)
        echo "Previewing all screens..."
        qmlscene "$SCRIPT_DIR/preview_MenuStart.qml" &
        sleep 2
        qmlscene "$SCRIPT_DIR/preview_LoginPage.qml" &
        sleep 2
        qmlscene "$SCRIPT_DIR/preview_SignUpPage.qml" &
        sleep 2
        qmlscene "$SCRIPT_DIR/preview_MenuMain.qml" &
        sleep 2
        qmlscene "$SCRIPT_DIR/preview_GameScreen.qml" &
        sleep 2
        qmlscene "$SCRIPT_DIR/preview_ChangePasswordPage.qml" &
        ;;
    q|Q)
        echo "Exiting..."
        exit 0
        ;;
    *)
        echo "Invalid choice. Exiting..."
        exit 1
        ;;
esac

