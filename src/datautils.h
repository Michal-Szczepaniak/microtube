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

#ifndef DATAUTILS_H
#define DATAUTILS_H

#include <QtCore>

class DataUtils {

public:
    static QString stringToFilename(const QString &s);
    static QString regioneCode(const QLocale &locale);
    static QString systemRegioneCode();
    static uint parseIsoPeriod(const QString &isoPeriod);
    static QString formatDateTime(const QDateTime &dt);
    static QString formatDuration(uint secs);

private:
    DataUtils() { }

};

#endif // DATAUTILS_H
