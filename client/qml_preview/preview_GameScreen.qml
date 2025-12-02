import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 480
    height: 640
    title: qsTr("Game Screen - Preview")

    property color backGroundColor : "#394454"
    property color mainAppColor: "#6fda9c"
    property color mainTextColor: "#f0f0f0"
    property string applicationDirPath: "."

    // Mock BackEnd
    QtObject {
        id: mockBackEnd
        property int prize: 5
        property string question: "Câu hỏi mẫu: Thủ đô của Việt Nam là gì?"
        property string a: "Hà Nội"
        property string b: "Hồ Chí Minh"
        property string c: "Đà Nẵng"
        property string d: "Huế"
        property int correct_answer: 1
    }

    background: Rectangle {
        color: backGroundColor
    }

    Item {
      width: 480
      height: 640

      ColumnLayout {
          anchors.fill: parent
          spacing: 20

          // Countdown timer placeholder
          Rectangle {
              Layout.alignment: Qt.AlignHCenter
              Layout.topMargin: 10
              width: 76
              height: 76
              radius: 38
              color: "#3eefff"
              border.width: 8
              border.color: "#3eefff"
              Text {
                  anchors.centerIn: parent
                  text: "15"
                  font.pointSize: 20
                  color: "white"
              }
          }

          // Question area
          Rectangle {
              Layout.fillWidth: true
              Layout.preferredHeight: 200
              Layout.leftMargin: 40
              Layout.rightMargin: 40
              color: "#2a3441"
              radius: 10
              border.color: mainAppColor
              border.width: 2

              ColumnLayout {
                  anchors.fill: parent
                  anchors.margins: 10

                  Text {
                      Layout.alignment: Qt.AlignHCenter
                      text: "Câu hỏi " + (mockBackEnd.prize + 1)
                      font.pointSize: 14
                      color: "white"
                  }

                  Text {
                      Layout.fillWidth: true
                      Layout.fillHeight: true
                      text: mockBackEnd.question
                      font.pointSize: 14
                      color: "white"
                      wrapMode: Text.WordWrap
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                  }
              }
          }

          // Answer buttons
          ColumnLayout {
              Layout.fillWidth: true
              Layout.leftMargin: 20
              Layout.rightMargin: 20
              spacing: 15

              Button {
                  Layout.fillWidth: true
                  Layout.preferredHeight: 65
                  text: "A. " + mockBackEnd.a
                  background: Rectangle {
                      color: mainAppColor
                      radius: height/2
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                      font.pointSize: 14
                      wrapMode: Text.WordWrap
                  }
                  onClicked: console.log("Answer A clicked")
              }

              Button {
                  Layout.fillWidth: true
                  Layout.preferredHeight: 65
                  text: "B. " + mockBackEnd.b
                  background: Rectangle {
                      color: mainAppColor
                      radius: height/2
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                      font.pointSize: 14
                      wrapMode: Text.WordWrap
                  }
                  onClicked: console.log("Answer B clicked")
              }

              Button {
                  Layout.fillWidth: true
                  Layout.preferredHeight: 65
                  text: "C. " + mockBackEnd.c
                  background: Rectangle {
                      color: mainAppColor
                      radius: height/2
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                      font.pointSize: 14
                      wrapMode: Text.WordWrap
                  }
                  onClicked: console.log("Answer C clicked")
              }

              Button {
                  Layout.fillWidth: true
                  Layout.preferredHeight: 65
                  text: "D. " + mockBackEnd.d
                  background: Rectangle {
                      color: mainAppColor
                      radius: height/2
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                      font.pointSize: 14
                      wrapMode: Text.WordWrap
                  }
                  onClicked: console.log("Answer D clicked")
              }
          }

          // Helper buttons row
          RowLayout {
              Layout.alignment: Qt.AlignHCenter
              spacing: 15

              Button {
                  Layout.preferredWidth: 70
                  Layout.preferredHeight: 40
                  text: "50/50"
                  background: Rectangle {
                      color: "#555"
                      radius: 5
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      font.pointSize: 10
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                  }
                  onClicked: console.log("50/50 clicked")
              }

              Button {
                  Layout.preferredWidth: 70
                  Layout.preferredHeight: 40
                  text: "Call"
                  background: Rectangle {
                      color: "#555"
                      radius: 5
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      font.pointSize: 10
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                  }
                  onClicked: console.log("Call phone clicked")
              }

              Button {
                  Layout.preferredWidth: 70
                  Layout.preferredHeight: 40
                  text: "Vote"
                  background: Rectangle {
                      color: "#555"
                      radius: 5
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      font.pointSize: 10
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                  }
                  onClicked: console.log("Vote clicked")
              }

              Button {
                  Layout.preferredWidth: 70
                  Layout.preferredHeight: 40
                  text: "Change"
                  background: Rectangle {
                      color: "#555"
                      radius: 5
                  }
                  contentItem: Text {
                      text: parent.parent.text
                      color: "white"
                      font.pointSize: 10
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                  }
                  onClicked: console.log("Change question clicked")
              }
          }
      }
    }
}

