const ytch = require('yt-channel-info')

let channelId = process.argv[2];
const payload = {
   channelId: channelId,
}

ytch.getChannelInfo(payload).then(d => console.log(JSON.stringify(d, null, 2)), e => console.error(JSON.stringify(e, null, 2)));
