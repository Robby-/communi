import QtQuick 1.1
import com.nokia.meego 1.0
import "UIConstants.js" as UI

CommonPage {
    id: root

    ListView {
        id: listView
        anchors.fill: parent
        model: SessionModel
        delegate: Column {
            width: parent.width
            ListItem {
                busy: modelData.busy
                title: modelData.title
                subtitle: modelData.subtitle
                highlighted: modelData.highlighted
                onClicked: chatPage.push(modelData)
            }
            Repeater {
                model: modelData.childItems
                ListItem {
                    title: modelData.title
                    subtitle: modelData.subtitle
                    highlighted: modelData.highlighted
                    onClicked: chatPage.push(modelData)
                }
            }
            Rectangle {
                color: "#505050"
                height: 1
                visible: index < listView.count - 1
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: UI.PAGE_MARGIN
                    rightMargin: UI.PAGE_MARGIN
                }
            }
        }
    }

    ChatPage {
        id: chatPage
        function push(data) {
            modelData = data;
            title = modelData.title;
            model = modelData.messages;
            session = modelData.session;
            root.pageStack.push(chatPage);
            if (modelData.busy !== undefined)
                chatPage.busy = modelData.busy;
        }
        onStatusChanged: modelData.current = (status == PageStatus.Active)
    }
}
