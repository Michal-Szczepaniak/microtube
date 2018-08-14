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

#include "httputils.h"
#include "constants.h"
#include "http.h"
#include "throttledhttp.h"
#include "cachedhttp.h"
#include "localcache.h"

Http &HttpUtils::notCached() {
    static Http *h = [] {
        Http *http = new Http;
        http->addRequestHeader("User-Agent", userAgent());

        return http;
    }();
    return *h;
}

Http &HttpUtils::cached() {
    static Http *h = [] {
        Http *http = new Http;
        http->addRequestHeader("User-Agent", userAgent());

        CachedHttp *cachedHttp = new CachedHttp(*http, "http");

        return cachedHttp;
    }();
    return *h;
}

Http &HttpUtils::yt() {
    static Http *h = [] {
        Http *http = new Http;
        http->addRequestHeader("User-Agent", stealthUserAgent());

        CachedHttp *cachedHttp = new CachedHttp(*http, "yt");
        cachedHttp->setMaxSeconds(3600);

        return cachedHttp;
    }();
    return *h;
}

void HttpUtils::clearCaches() {
    LocalCache::instance("yt")->clear();
    LocalCache::instance("http")->clear();
}

const QByteArray &HttpUtils::userAgent() {
    static const QByteArray ua = [] {
        return QString(QLatin1String(Constants::NAME)
                       + QLatin1Char('/') + QLatin1String(Constants::VERSION)
                       + QLatin1String(" ( ") + Constants::WEBSITE + QLatin1String(" )")).toUtf8();
    }();
    return ua;
}

const QByteArray &HttpUtils::stealthUserAgent() {
    static const QByteArray ua = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.95 Safari/537.36";
    return ua;
}
