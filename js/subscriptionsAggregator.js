import { Innertube, YTNodes } from 'youtubei.js';

const query = process.argv[2];
const parameters = JSON.parse(process.argv[3]);

const youtube = await Innertube.create({
    lang: parameters.language,
    location: parameters.country,
    retrieve_player: false,
    enable_safety_mode: parameters.safeSearch,
});

let resultData = {};
const includeVideos = parameters.includeVideos;
const includeLivestreams = parameters.includeLivestreams;

const channel = await youtube.getChannel(query);
resultData["channel"] = channel;

if (includeVideos) {
    const videos = await channel.getVideos();
    resultData["videos"] = videos.memo.getType(YTNodes.Video, YTNodes.GridVideo, YTNodes.ReelItem, YTNodes.CompactVideo, YTNodes.PlaylistVideo, YTNodes.PlaylistPanelVideo, YTNodes.WatchCardCompactVideo, YTNodes.Playlist, YTNodes.GridPlaylist, YTNodes.Channel, YTNodes.GridChannel);
}

if (includeLivestreams) {
    const livestreams = await channel.getLiveStreams();
    resultData["livestreams"] = livestreams.memo.getType(YTNodes.Video, YTNodes.GridVideo, YTNodes.ReelItem, YTNodes.CompactVideo, YTNodes.PlaylistVideo, YTNodes.PlaylistPanelVideo, YTNodes.WatchCardCompactVideo, YTNodes.Playlist, YTNodes.GridPlaylist, YTNodes.Channel, YTNodes.GridChannel);
}

console.log(JSON.stringify(resultData, null, 2))
