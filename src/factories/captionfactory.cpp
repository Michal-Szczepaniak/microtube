#include "captionfactory.h"

Caption CaptionFactory::fromJson(QJsonObject json)
{
    Caption caption{};
    caption.code = json["language_code"].toString();
    caption.label = json["name"].toObject()["text"].toString();
    caption.id = json["vss_id"].toString();
    caption.translatable = json["is_translatable"].toBool();
    caption.baseUrl = json["base_url"].toString();

    return caption;
}
