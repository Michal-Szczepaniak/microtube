import { Innertube, UniversalCache } from 'youtubei.js';

const query = process.argv[2];
const parameters = JSON.parse(process.argv[3]);

const youtube = await Innertube.create({
    lang: parameters.language,
    location: parameters.country,
    retrieve_player: false,
    enable_safety_mode: parameters.safeSearch,
});

const info = await youtube.getInfo(query);

console.log(JSON.stringify({info: info, formats: []}, null, 2))
