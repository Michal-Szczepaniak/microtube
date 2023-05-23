import { Innertube, YTNodes } from 'youtubei.js';

const yt = await Innertube.create({
    lang: 'en',
    location: 'US',
});

const channelId = process.argv[2];
const continuation = JSON.parse(process.argv[3])

let videos = null
let continuationData = null
if (Object.keys(continuation).length) {
    const response = await yt.actions.execute(continuation.endpoint.metadata.api_url, {
        ...continuation.endpoint.payload,
        parse: true
    });

    videos = response.on_response_received_actions_memo.getType(YTNodes.Video)
    continuationData = response.on_response_received_actions_memo.getType(YTNodes.ContinuationItem).at(0)
} else {
    const channel = await yt.getChannel(channelId)
    const channelVideos = await channel.getVideos()
    continuationData = channelVideos.memo.getType(YTNodes.ContinuationItem).at(0)
    videos = channelVideos.memo.getType(YTNodes.Video)
}

const result = {
    items: videos,
    continuation: continuationData
}

console.log(JSON.stringify(result, null, 2))
