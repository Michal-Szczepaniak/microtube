#include "captionfactory.h"

Caption CaptionFactory::fromJson(QJsonObject json)
{
    Caption caption{};
    caption.label = json["name"].toString();
    caption.baseUrl = json["url"].toString();

    return caption;
}
