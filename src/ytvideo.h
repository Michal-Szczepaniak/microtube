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

#ifndef YTVIDEO_H
#define YTVIDEO_H

#include <QtCore>

class VideoDefinition;

class YTVideo : public QObject {
    Q_OBJECT

public:
    YTVideo(const QString &videoId, QObject *parent);
    void loadStreamUrl();
    int getDefinitionCode() const { return definitionCode; }

signals:
    void gotStreamUrl(const QUrl &streamUrl);
    void errorStreamUrl(const QString &message);

private slots:
    void gotVideoInfo(const QByteArray &bytes);
    void errorVideoInfo(const QString &message);
    void scrapeWebPage(const QByteArray &bytes);
    void parseJsPlayer(const QByteArray &bytes);
    void parseDashManifest(const QByteArray &bytes);

private:
    void getVideoInfo();
    void parseFmtUrlMap(const QString &fmtUrlMap, bool fromWebPage = false);
    void captureFunction(const QString &name, const QString &js);
    void captureObject(const QString &name, const QString &js);
    QString decryptSignature(const QString &s);
    void saveDefinitionForUrl(const QString &url, const VideoDefinition &definition);

    QString videoId;
    QUrl m_streamUrl;
    int definitionCode;
    bool loadingStreamUrl;
    // current index for the elTypes list
    // needed to iterate on elTypes
    int elIndex;
    bool ageGate;
    QString videoToken;
    QString fmtUrlMap;
    QString sigFuncName;
    QHash<QString, QString> sigFunctions;
    QHash<QString, QString> sigObjects;
    QString dashManifestUrl;
    QString jsPlayer;
};

#endif // YTVIDEO_H
