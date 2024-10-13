import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts

Window {
    width: 280
    height: 520
    visible: true
    title: qsTr("CA1")

    Item{
        anchors.fill: parent

        ColumnLayout {
            anchors{
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: textfield.top
                margins: 20
            }

            Label{
                text: "Ip: " + callManager.ipAddress
                Layout.fillWidth: true
                Layout.preferredHeight: 40
            }
            Label{
                text: "IceCandidate: " + callManager.iceCandidate
                Layout.fillWidth: true
                Layout.preferredHeight: 40
            }
            Label{
                text: "CallerId: " + textfield.text
                Layout.fillWidth: true
                Layout.preferredHeight: 40
            }

        }

        TextField{
            id: textfield
            placeholderText: "Phone Number"
            anchors.bottom: callbtn.top
            anchors.bottomMargin: 10
            anchors.left: callbtn.left
            anchors.right: callbtn.right
            enabled: !callbtn.pushed
            onTextChanged: callManager.setCallerId(text)
        }

        Button{
            id: callbtn

            property bool pushed: false

            height: 47
            text: "Call"
            Material.background: "green"
            Material.foreground: "white"
            anchors{
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                margins: 20
            }

            onClicked: {
                pushed = !pushed
                if(pushed){
                    Material.background = "red"
                    text = "End Call"
                    callManager.startCall()
                }
                else{
                    Material.background = "green"
                    text = "Call"
                    callManager.endCall()
                    textfield.clear()
                }
            }
        }
    }
}
