/*
    Copyright (C) 2018 Michał Szczepaniak

    This file is part of Microtube.

    Microtube is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Microtube is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Microtube.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.6
import Sailfish.Silica 1.0
import com.verdanditeam.yt 1.0

BackgroundItem {
    id: barButton

    height: Theme.itemSizeLarge

    property bool attention: false
    property color baseColor: Theme.highlightBackgroundColor
    property color attentionColor: "red"
    highlightedColor: Theme.rgba(Theme.highlightBackgroundFromColor(attention ? attentionColor : baseColor, Theme.colorScheme),
                                 Theme.highlightBackgroundOpacity)

    function _color(alpha) {
        alpha = attention ? alpha*0.75 : alpha*0.25
        return Theme.rgba(attention ? attentionColor : baseColor, alpha)
    }

    Rectangle {
        x: -1
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: _color(1) }
            GradientStop { position: 0.2; color: _color(0.6) }
            GradientStop { position: 0.8; color: _color(0.4) }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }

    Label {
        id: label
        anchors.right: rightIcon.left
        anchors.rightMargin: Theme.paddingMedium
        text: name
        anchors.verticalCenter: parent.verticalCenter
        color: barButton.highlighted ? Theme.highlightColor : Theme.primaryColor
    }

    Image {
        id: rightIcon
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter
        source: "image://theme/icon-m-right"
    }
}
