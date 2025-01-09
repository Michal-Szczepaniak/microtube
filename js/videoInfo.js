import ytdl from '@distube/ytdl-core';

let query = process.argv[2];

ytdl.lastUpdateCheck = Date.now();
ytdl.getInfo(query).then(d => console.log(JSON.stringify(d, null, 2)), e => console.error(JSON.stringify(e, null, 2)));
