import QtQuick 2.9
import QtQuick.Controls 2.4
import eu.elettra.cumbiaqmlcontrols 1.0


SwipeView {
    id:swipeView

    currentIndex: 0
    anchors.fill: parent

    Page1 {}

    Page3{}

    Page2 {}

    PageIndicator {
        id: indicator

        count: swipeView.count
        currentIndex: swipeView.currentIndex

        anchors.bottom: parent.bottom
//            anchors.horizontalCenter: swipeView.horizontalCenter
    }


    //            case Qt.ApplicationActive:
    //                console.log("++++++++++++++++ App ACTIVE")
    //                cu_websocket_client.open()
    //                spectrumChart.backend.start()
    //                cumbiaReadLabel.backend.start()
    //                break
    //            }
    //        }
    //    }
}


