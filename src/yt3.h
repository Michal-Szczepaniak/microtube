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

#ifndef YT3_H
#define YT3_H

#include <QtCore>

class HttpReply;

class YT3 : public QObject {

    Q_OBJECT

public:
    static YT3 &instance();
    static const QString &baseUrl();

    void initApiKeys();
    void testApiKey();
    void addApiKey(QUrl &url);
    QUrl method(const QString &name);

private slots:
    void testResponse(const HttpReply &reply);

private:
    YT3();

    QStringList keys;
    QString key;
};

#endif // YT3_H
