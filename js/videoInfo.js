import { Innertube, UniversalCache } from 'youtubei.js';

const query = process.argv[2];
const parameters = JSON.parse(process.argv[3]);

const youtube = await Innertube.create({
    lang: parameters.language,
    location: parameters.country,
    enable_safety_mode: parameters.safeSearch,
    po_token: parameters.poToken.poToken,
    visitor_data: parameters.poToken.visitorData,
    cache: new UniversalCache(true),
    generate_session_locally: true
});

const info = await youtube.getInfo(query);

const formats = [
  ...(info?.streaming_data?.formats || []),
  ...(info?.streaming_data?.adaptive_formats || [])
];

const urls = formats.map((format) => {
                             format.url = format.decipher(youtube.session.player);
                             return format;
                         })

console.log(JSON.stringify({info: info, formats: urls}, null, 2))
