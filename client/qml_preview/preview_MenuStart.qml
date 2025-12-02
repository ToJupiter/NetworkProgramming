import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 480
    height: 640
    maximumWidth: 480
    maximumHeight: 640
    minimumWidth: 480
    minimumHeight: 640
    title: qsTr("Ai Là Triệu Phú - Preview")

    property color backGroundColor : "#394454"
    property color mainAppColor: "#6fda9c"
    property color mainTextColor: "#f0f0f0"

    // Mock applicationDirPath for preview
    property string applicationDirPath: "."

    background: Rectangle {
        color: backGroundColor
    }

    Page{
      id: menuStart
      width: 480
      height: 640

      background: Rectangle {
          color: backGroundColor
      }

      // Logo placeholder
      Rectangle {
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: parent.top
          anchors.topMargin: 40
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
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: parent.top
          anchors.topMargin: 125
          width: 220
          height: 50
          font.bold: true
          font.pointSize: 20
          text: qsTr("Ai Là Triệu Phú")
          verticalAlignment: Text.AlignVCenter
          horizontalAlignment: Text.AlignHCenter
          color: "white"
      }

      // Buttons
      ColumnLayout {
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: parent.top
          anchors.topMargin: 350
          spacing: 15

          Button {
              Layout.preferredWidth: 400
              Layout.preferredHeight: 65
              text: "Bắt đầu"
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
              onClicked: console.log("Bắt đầu clicked")
          }

          Button {
              Layout.preferredWidth: 400
              Layout.preferredHeight: 65
              text: "Hướng dẫn"
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
              onClicked: guideDialog.open()
          }

          Button {
              Layout.preferredWidth: 400
              Layout.preferredHeight: 65
              text: "Giới thiệu"
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
              onClicked: creditDialog.open()
          }

          Button {
              Layout.preferredWidth: 400
              Layout.preferredHeight: 65
              text: "Thoát"
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
              onClicked: Qt.quit()
          }
      }

      Dialog {
        id: guideDialog
        visible: false
        title: "Cách chơi"

        contentItem: Rectangle {
          implicitWidth: 400
          implicitHeight: 400

          Text {
            id: titleGuide
            text: "<h1><b><i><font color='red'>Hướng dẫn chơi</font></i></b></h1>"
            font.family: "Times New Roman"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 30
          }
          Text {
            id: guideText
            width: 350
            height: 400
            text: "
            1. Người chơi chọn bắt đầu câu hỏi sẽ xuất hiện, bắt đầu tính thời gian 15s cho mỗi câu hỏi<br>
            2. Có 4 phương án. Người chơi chọn 1 đáp án trả lời<br>
            3. Đáp án đúng câu hỏi tiếp theo sẽ xuất hiện<br>
            4. Nếu sai lượt chơi sẽ kết thúc và hiển thị kết quả với số điểm nhận được tương ứng"
            font.family: "Times New Roman"
            font.pointSize: 15
            wrapMode: Text.WordWrap
            anchors.top: titleGuide.bottom
            anchors.topMargin: 25
            anchors.left: parent.left
            anchors.leftMargin: 30
          }
        }
      }

      Dialog {
        id: creditDialog
        visible: false
        title: "Giới thiệu"

        contentItem: Rectangle {
          implicitWidth: 450
          implicitHeight: 400

          Text {
            id: titleCredit
            text: "<h1><b><i><font color='red'>Giới thiệu</font></i></b></h1>"
            font.family: "Times New Roman"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 30
          }

          Text{
            id: contentCredit1
            text: "<h3><font color='#148F77'><i>1. Nguyễn Minh Khôi - 20225642</i></font></h3>"
            font.family: "Times New Roman"
            font.pointSize: 12
            anchors.top: titleCredit.bottom
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 40
          }

          Text{
            id: contentCredit2
            text: "<h3><font color='#148F77'><i>2. Phan Hoàng Hải - 20225715</i></font></h3>"
            font.family: "Times New Roman"
            font.pointSize: 12
            anchors.top: contentCredit1.bottom
            anchors.topMargin: 33
            anchors.left: parent.left
            anchors.leftMargin: 40
          }
        }
      }
    }
}

