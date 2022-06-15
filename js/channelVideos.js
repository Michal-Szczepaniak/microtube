const ytch = require('yt-channel-info')

let channelId = process.argv[2];
let sortBy = process.argv[3];
let continuation = process.argv[4];

if (continuation) {
    const payload = {
        continuation: continuation,
    }
    ytch.getChannelVideosMore(payload).then(d => console.log(JSON.stringify(d, null, 2)), e => console.error(JSON.stringify(e, null, 2)));
} else {
    const payload = {
        channelId: channelId,
	sortBy: sortBy,
    }
    ytch.getChannelVideos(payload).then(d => console.log(JSON.stringify(d, null, 2)), e => console.error(JSON.stringify(e, null, 2)));
}
