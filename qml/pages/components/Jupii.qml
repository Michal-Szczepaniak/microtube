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

import QtQuick 2.0
import Nemo.DBus 2.0

Item {
    property bool connected: false
    property bool running: false
    readonly property string _serviceName: "org.mkiol.jupii"

    onVisibleChanged: ping()

    function ping() {
        connected = running ? (jupiiPlayer.getProperty('canControl') === true) : false
    }

    function addUrlOnceAndPlay(url, origUrl, title, author, description, type, app, icon) {
        jupiiPlayer.call('add', [url, origUrl, title, author, "", type, app, icon, true, true])

    }

    function _updateSatatus() {
        dbus.call("NameHasOwner", _serviceName, function(result) {
            running = result
            ping()
        },
        function(error, message) {
            console.log("updateSatatus error:", error, message)
            running = false
            ping()
        })
    }

    DBusInterface {
        id: jupiiPlayer

        service: running ? _serviceName : ''
        iface: 'org.jupii.Player'
        path: '/'
    }

    DBusInterface {
        id: dbus

        service: "org.freedesktop.DBus"
        iface: "org.freedesktop.DBus"
        path: "/org/freedesktop/DBus"
        signalsEnabled: true

        function nameOwnerChanged(name, oldOwner, newOwner) {
            if (name === _serviceName) {
                _updateSatatus()
            }
        }
    }
}
