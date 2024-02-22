#ifndef SEARCH_H
#define SEARCH_H

#include <QObject>
#include <vector>
#include <QJsonArray>
#include <memory>
#include "video.h"
#include "parsers/videosparser.h"

struct Search {
    Q_GADGET
public:
    QString query{};
    bool safeSearch = false;
    QString country = "US";
    QString language = "en";

    enum UploadDateFilter {
        AllUploadDates,
        Hour,
        Today,
        Week,
        Month,
        Year
    } uploadDateFilter{};
    Q_ENUM(UploadDateFilter)

    enum TypeFilter {
        AllTypes,
        VideoType,
        ChannelType,
        PlaylistType,
        MovieType
    } typeFilter;
    Q_ENUM(TypeFilter)

    enum DurationFilter {
        AllDurations,
        Short,
        Medium,
        Long
    } durationFilter{};
    Q_ENUM(DurationFilter)

    enum SortBy {
        Relevance,
        Rating,
        UploadDate,
        ViewCount
    } sortBy{};
    Q_ENUM(SortBy)

    enum FeaturesFilter {
        HD,
        Subtitles,
        CreativeCommons,
        V3D,
        Live,
        Purchased,
        V4K,
        VR360,
        Location,
        HDR,
        VR180
    } featuresFilter{};
    Q_ENUM(FeaturesFilter)

    enum SearchType {
        Query,
        Category,
        Channel,
        ChannelShorts,
        ChannelLiveStreams,
        ChannelPlaylists,
        Subscriber,
        Subscriptions,
        UnwatchedSubscriptions,
        Playlist,
    } type{};
    Q_ENUM(SearchType)
};
Q_DECLARE_METATYPE(Search*)

#endif // SEARCH_H
