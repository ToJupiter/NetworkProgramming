import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 480
    height: 640
    title: qsTr("Menu Chính - Preview")

    property color backGroundColor : "#394454"
    property color mainAppColor: "#6fda9c"
    property color mainTextColor: "#f0f0f0"
    property string applicationDirPath: "."

    // Mock BackEnd
    QtObject {
        id: mockBackEnd
        property string user_name: "Nguyễn Minh Khôi"
        property int prize: 0
        property int reward: 0
    }

    background: Rectangle {
        color: backGroundColor
    }

    Page {
      width: 480
      height: 640
      property bool showMenuMain: true
      property var sTatus : 1

      background: Rectangle {
          color: backGroundColor
      }

      ColumnLayout {
          anchors.fill: parent
          spacing: 20

          // Logo placeholder
          Rectangle {
              Layout.alignment: Qt.AlignHCenter
              Layout.topMargin: 40
              width: 220
              height: 220
              color: "transparent"
              border.color: mainAppColor
              border.width: 2
              Text {
                  anchors.centerIn: parent
                  text: "LOGO"
                  color: mainAppColor
                  font.pointSize: 24
              }
          }

          Text {
            id: text
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 20
            width: 220
            height: 50
            font.bold: true
            font.pointSize: 16
            text: "Hello, <font color='red'>" + mockBackEnd.user_name + "</font>"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: "white"
          }

          ColumnLayout {
              Layout.fillWidth: true
              Layout.alignment: Qt.AlignHCenter
              spacing: 15

              Button {
                  Layout.preferredWidth: 400
                  Layout.preferredHeight: 50
                  Layout.alignment: Qt.AlignHCenter
                  text: "Chơi 1 mình"
                  background: Rectangle {
                      color: mainAppColor
                      radius: height/2
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                      font.pointSize: 15
                  }
                  onClicked: console.log("Play alone clicked")
              }

              Button {
                  Layout.preferredWidth: 400
                  Layout.preferredHeight: 50
                  Layout.alignment: Qt.AlignHCenter
                  text: "Thi đấu online"
                  background: Rectangle {
                      color: mainAppColor
                      radius: height/2
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                      font.pointSize: 15
                  }
                  onClicked: console.log("Play PvP clicked")
              }

              Button {
                  Layout.preferredWidth: 400
                  Layout.preferredHeight: 50
                  Layout.alignment: Qt.AlignHCenter
                  text: "Đổi mật khẩu"
                  background: Rectangle {
                      color: mainAppColor
                      radius: height/2
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                      font.pointSize: 15
                  }
                  onClicked: console.log("Change password clicked")
              }

              Button {
                  Layout.preferredWidth: 400
                  Layout.preferredHeight: 50
                  Layout.alignment: Qt.AlignHCenter
                  text: "Đăng xuất"
                  background: Rectangle {
                      color: mainAppColor
                      radius: height/2
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                      font.pointSize: 15
                  }
                  onClicked: console.log("Logout clicked")
              }
          }
      }
    }
}

