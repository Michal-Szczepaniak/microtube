#include "captionfactory.h"

Caption CaptionFactory::fromJson(QJsonObject json)
{
    Caption caption{};
    caption.code = json["languageCode"].toString();
    caption.label = json["name"].toObject()["simpleText"].toString();
    caption.id = json["vssId"].toString();
    caption.translatable = json["isTranslatable"].toBool();
    caption.baseUrl = json["baseUrl"].toString();

    return caption;
}
