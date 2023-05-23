import { Innertube } from 'youtubei.js';

const youtube = await Innertube.create({
    lang: 'en',
    location: 'US',
});

const channelId = process.argv[2];
const channel = await youtube.getChannel(channelId)

console.log(JSON.stringify(channel, null, 2))
