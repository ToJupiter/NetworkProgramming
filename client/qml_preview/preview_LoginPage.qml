import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 480
    height: 640
    title: qsTr("Đăng nhập - Preview")

    property color backGroundColor : "#394454"
    property color mainAppColor: "#6fda9c"
    property color mainTextColor: "#f0f0f0"
    property string applicationDirPath: "."

    background: Rectangle {
        color: backGroundColor
    }

    Page {
        id: loginPage
        title: "Đăng nhập"
        width: 480
        height: 640

        property bool isShowPassword: false

        background: Rectangle {
            color: backGroundColor
        }

        Rectangle {
            id: iconRect
            width: parent.width
            height: parent.height / 3
            color: backGroundColor

             Text {
                 id: icontext
                 text: qsTr("\uf136")
                 anchors.centerIn: parent
                 font.pointSize: 112
                 font.family: "fontawesome"
                 color: mainAppColor
             }
        }

        ColumnLayout {
            width: parent.width
            anchors.top: iconRect.bottom
            spacing: 15

            TextField {
                id: loginUsername
                placeholderText: qsTr("Tên đăng nhập")
                Layout.preferredWidth: parent.width - 20
                Layout.alignment: Qt.AlignHCenter
                color: mainTextColor
                font.pointSize: 14
                leftPadding: 30
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 50
                    radius: implicitHeight / 2
                    color: "transparent"

                    Text {
                        text: "\uf007"
                        font.pointSize: 14
                        color: mainAppColor
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        leftPadding: 10
                    }

                    Rectangle {
                        width: parent.width - 10
                        height: 1
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        color: mainAppColor
                    }
                }
            }

            TextField {
                id: loginPassword
                placeholderText: qsTr("Mật khẩu")
                Layout.preferredWidth: parent.width - 20
                Layout.alignment: Qt.AlignHCenter
                color: mainTextColor
                font.pointSize: 14
                leftPadding: 30
                echoMode: isShowPassword ? TextField.Normal : TextField.Password
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 50
                    radius: implicitHeight / 2
                    color: "transparent"
                    Text {
                        text: "\uf023"
                        font.pointSize: 14
                        color: mainAppColor
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        leftPadding: 10
                    }

                    Rectangle {
                        width: parent.width - 10
                        height: 1
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        color: mainAppColor
                    }
                }
            }

            Item {
                height: 20
            }

            Button {
                height: 50
                Layout.preferredWidth: loginPage.width - 20
                Layout.alignment: Qt.AlignHCenter
                text: "Đăng nhập"
                background: Rectangle {
                    color: mainAppColor
                    radius: height/2
                }
                contentItem: Text {
                    text: parent.parent.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: console.log("Login clicked")
            }

            Button {
                height: 50
                Layout.preferredWidth: loginPage.width - 20
                Layout.alignment: Qt.AlignHCenter
                text: "Đăng ký"
                background: Rectangle {
                    color: "transparent"
                    border.color: mainAppColor
                    border.width: 1
                    radius: height/2
                }
                contentItem: Text {
                    text: parent.parent.text
                    color: mainAppColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: console.log("Sign up clicked")
            }
        }
    }
}

