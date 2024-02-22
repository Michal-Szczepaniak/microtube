import { Innertube, YTNodes } from 'youtubei.js';

const query = process.argv[2];
const parameters = JSON.parse(process.argv[3]);

const youtube = await Innertube.create({
    lang: parameters.language,
    location: parameters.country,
    retrieve_player: false,
    enable_safety_mode: parameters.safeSearch,
});

let videos = null
let continuationData = null
if ('continuation' in parameters) {
    const response = await youtube.actions.execute(parameters['continuation'].endpoint.metadata.api_url, {
        ...parameters['continuation'].endpoint.payload,
        parse: true
    });

    if (Object.keys(response).length !== 0) {
        const contents = response.on_response_received_actions?.first().contents ||
                         response.on_response_received_commands?.first().contents.first()?.contents;

        videos = contents.filterType(YTNodes.Video, YTNodes.GridVideo, YTNodes.ReelItem, YTNodes.CompactVideo, YTNodes.PlaylistVideo, YTNodes.PlaylistPanelVideo, YTNodes.WatchCardCompactVideo, YTNodes.Playlist, YTNodes.GridPlaylist, YTNodes.Channel, YTNodes.GridChannel)
        if (videos.length === 0) {
            const items = contents.filterType(YTNodes.RichItem);
            if (items.length !== 0) {
                videos = items.map((item) => item.content);
            }
        }

        if (response.on_response_received_actions_memo) {
            continuationData = response.on_response_received_actions_memo.getType(YTNodes.ContinuationItem).at(0)
        } else if (response.on_response_received_commands_memo) {
            continuationData = response.on_response_received_commands_memo.getType(YTNodes.ContinuationItem).at(0)
        }
    }
} else {
    var data = undefined

    switch (parameters.type) {
    case 0:
        data = await youtube.search(query, {
                                        upload_date: parameters.uploadDateFilter,
                                        duration: parameters.durationFilter
                                    })
        break;
    case 1:
    {
        const trending = await youtube.getTrending()
        data = await trending.getTabByName(query)
    }
        break;
    case 2:
    {
        const channel = await youtube.getChannel(query)
        data = await channel.getVideos()
    }
        break;
    case 3:
    {
        const channel = await youtube.getChannel(query)
        data = await channel.getShorts()
    }
        break;
    case 4:
    {
        const channel = await youtube.getChannel(query)
        data = await channel.getLiveStreams()
    }
        break;
    case 5:
    {
        const channel = await youtube.getChannel(query)
        data = await channel.getPlaylists()
    }
        break;
    case 9:
        data = await youtube.getPlaylist(query)
        break;
    }

    continuationData = data.memo.getType(YTNodes.ContinuationItem).at(0)
    if ('results' in data) {
        videos = data.results.filterType(YTNodes.Video, YTNodes.GridVideo, YTNodes.ReelItem, YTNodes.CompactVideo, YTNodes.PlaylistVideo, YTNodes.PlaylistPanelVideo, YTNodes.WatchCardCompactVideo, YTNodes.Playlist, YTNodes.GridPlaylist, YTNodes.Channel, YTNodes.GridChannel)
    } else {
        videos = data.memo.getType(YTNodes.Video, YTNodes.GridVideo, YTNodes.ReelItem, YTNodes.CompactVideo, YTNodes.PlaylistVideo, YTNodes.PlaylistPanelVideo, YTNodes.WatchCardCompactVideo, YTNodes.Playlist, YTNodes.GridPlaylist, YTNodes.Channel, YTNodes.GridChannel)
    }
}

const result = {
    items: videos,
    continuation: continuationData
}

console.log(JSON.stringify(result, null, 2))
