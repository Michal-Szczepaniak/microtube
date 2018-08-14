/* $BEGIN_LICENSE

This file is part of Minitube.
Copyright 2009, Flavio Tordini <flavio.tordini@gmail.com>
Copyright 2018, Michał Szczepaniak <m.szczepaniak.000@gmail.com>

Minitube is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Minitube is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Minitube.  If not, see <http://www.gnu.org/licenses/>.

$END_LICENSE */

#include "yt3.h"

#include <algorithm>
#include <ctime>

#include "jsfunctions.h"
#include "http.h"
#include "httputils.h"
#include "constants.h"
//#include "mainwindow.h"

#ifdef APP_EXTRA
#include "extra.h"
#endif

#define STR(x) #x
#define STRINGIFY(x) STR(x)

YT3 &YT3::instance() {
    static YT3 *i = new YT3();
    return *i;
}

const QString &YT3::baseUrl() {
    static const QString base = "https://www.googleapis.com/youtube/v3/";
    return base;
}

YT3::YT3() {
    initApiKeys();
}

void YT3::initApiKeys() {
    keys.clear();

    QByteArray customApiKey = qgetenv("GOOGLE_API_KEY");
    if (!customApiKey.isEmpty()) {
        keys << QString::fromUtf8(customApiKey);
        qDebug() << "API key from environment" << keys;
    }

    if (keys.isEmpty()) {
        QSettings settings;
        if (settings.contains("googleApiKey")) {
            keys << settings.value("googleApiKey").toString();
            qDebug() << "API key from settings" << keys;
        }
    }

#ifdef APP_GOOGLE_API_KEY
    if (keys.isEmpty()) {
        keys << STRINGIFY(APP_GOOGLE_API_KEY);
        qDebug() << "built-in API key" << keys;
    }
#endif

#ifdef APP_EXTRA
    if (keys.isEmpty())
        keys << Extra::apiKeys();
#endif

    if (keys.isEmpty()) {
        qWarning() << "No available API keys";
#ifdef APP_LINUX
        QMetaObject::invokeMethod(MainWindow::instance(), "missingKeyWarning", Qt::QueuedConnection);
#endif
    } else {
        key = keys.takeFirst();
        if (!keys.isEmpty()) testApiKey();
    }
}

void YT3::testApiKey() {
    QUrl url = method("videos");
    QUrlQuery q(url);
    q.addQueryItem("part", "id");
    q.addQueryItem("chart", "mostPopular");
    q.addQueryItem("maxResults", "1");
    url.setQuery(q);
    QObject *reply = HttpUtils::yt().get(url);
    connect(reply, SIGNAL(finished(HttpReply)), SLOT(testResponse(HttpReply)));
}

void YT3::addApiKey(QUrl &url) {
    if (key.isEmpty()) {
        qDebug() << __PRETTY_FUNCTION__ << "empty key";
        initApiKeys();
        return;
    }

    QUrlQuery q(url);
    q.addQueryItem("key", key);
    url.setQuery(q);
}

QUrl YT3::method(const QString &name) {
    QUrl url(baseUrl() + name);
    addApiKey(url);
    return url;
}

void YT3::testResponse(const HttpReply &reply) {
    int status = reply.statusCode();
    if (status != 200) {
        if (keys.isEmpty()) {
            qWarning() << "Fatal error: No working API keys!";
            return;
        }
        key = keys.takeFirst();
        testApiKey();
    } else {
        qDebug() << "Using key" << key;
    }
}
