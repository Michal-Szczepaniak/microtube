import { Innertube } from 'youtubei.js';

const query = process.argv[2];
const parameters = JSON.parse(process.argv[3]);

const youtube = await Innertube.create({
    lang: parameters.language,
    location: parameters.country,
    retrieve_player: false,
    enable_safety_mode: parameters.safeSearch,
});

const channel = await youtube.getChannel(query)

console.log(JSON.stringify(channel, null, 2))
