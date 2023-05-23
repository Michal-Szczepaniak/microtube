import { Innertube } from 'youtubei.js';
const parameters = JSON.parse(process.argv[2]);
const youtube = await Innertube.create({
    lang: parameters.lang,
    location: parameters.location,
});

const trending = await youtube.getTrending()
const trendingTab = await trending.getTabByName(parameters.page)

console.log(JSON.stringify(trendingTab.videos, null, 2))
