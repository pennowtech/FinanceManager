pragma Singleton

import QtQuick 2.15

QtObject {
    readonly property QtObject fonts: QtObject {
        readonly property int small: 12
        readonly property int medium: 16
        readonly property int large: 20
        readonly property int title: 24
    }

    property FontLoader robotoRegularFont: FontLoader {
        source: "qrc:/fonts/Roboto-Regular.ttf"
    }
    property FontLoader robotoBoldFont: FontLoader {
        source: "qrc:/fonts/Roboto-Bold.ttf"
    }
    property FontLoader rubikMediumFont: FontLoader {
        source: "qrc:/fonts/Rubik-Medium.ttf"
    }

    // Define font properties for easy access
    readonly property font bodyFont: Qt.font({
        family: robotoRegularFont.name,
        pixelSize: 16
    })
    readonly property font titleFont: Qt.font({
        family: rubikMediumFont.name,
        pixelSize: 24
    })
    readonly property font subtitleFont: Qt.font({
        family: rubikMediumFont.name,
        pixelSize: 18
    })

    readonly property QtObject colors: QtObject {
        // Primary colors
        readonly property color primary: "#007AFF"
        readonly property color primaryLight: "#5AC8FA"
        readonly property color primaryDark: "#0056B3"

        // Secondary colors
        readonly property color secondary: "#FF9500"
        readonly property color secondaryLight: "#FFCC00"
        readonly property color secondaryDark: "#FF3B30"

        // Neutral colors
        readonly property color background: "#F2F2F7"
        readonly property color surface: "#FFFFFF"
        readonly property color error: "#FF3B30"

        // Text colors
        readonly property color textPrimary: "#000000"
        readonly property color textSecondary: "#8E8E93"
        readonly property color textHint: "#C7C7CC"

        // Additional colors
        readonly property color accent: "#5856D6"
        readonly property color success: "#34C759"
        readonly property color warning: "#FF9500"
    }
}
