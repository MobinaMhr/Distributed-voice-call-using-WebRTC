import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts

Window {
    width: 360
    height: 570
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
                Layout.preferredHeight: 30
            }
            Label{
                text: "IceCandidate: " + callManager.iceCandidate
                Layout.fillWidth: true
                Layout.preferredHeight: 30
            }
            Label{
                text: "Id: " + textfield1.text
                Layout.fillWidth: true
                Layout.preferredHeight: 30
            }

            Label{
                text: "CallerId: " + textfield.text
                Layout.fillWidth: true
                Layout.preferredHeight: 80
            }

        }

        TextField{
            id: textfield1
            placeholderText: "Your Id"
            anchors {
                bottom: textfield.top
                bottomMargin: 10
                left: parent.left
                right: parent.right
                margins: 20  // Add a 20-pixel margin on all sides
            }enabled: !idbtn.pushed
            onTextChanged: callManager.setUserName(text)
        }

        TextField {
            id: textfield
            placeholderText: "Phone Number"
            anchors {
                bottom: callbtn.top
                bottomMargin: 10
                left: parent.left
                right: parent.right
                margins: 20  // Add a 20-pixel margin on all sides
            }
            enabled: !callbtn.pushed
            onTextChanged: callManager.setCallerId(text)
        }

        // Register user
        Button{
            id: idbtn

            property bool pushed: false

            height: 47
            width: parent.width / 2 - 30  // Adjust to make both buttons symmetric
            text: "SET ID"
            Material.background: "green"
            Material.foreground: "white"
            anchors{
                bottom: parent.bottom
                left: parent.left
                right: parent.center + 30
                margins: 20
            }

            onClicked: {
                pushed = !pushed
                if(pushed){
                    callManager.registerUser();
                    Material.background = "red"
                    text = "UN Set"
                }
                else{
                    Material.background = "green"
                    text = "Set"
                    textfield1.clear()
                }
            }
        }

        // Start call - End call
        Button{
            id: callbtn

            property bool pushed: false

            height: 47
            width: parent.width / 2 - 30  // Make the width the same as callerIdBtn
            text: "Call"
            Material.background: "green"
            Material.foreground: "white"
            anchors{
                bottom: parent.bottom
                left: parent.center
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
