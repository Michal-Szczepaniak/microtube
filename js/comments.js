const ytcm = require("@freetube/yt-comment-scraper");

let query = process.argv[2];
let continuation = process.argv[3];

const payload = {
  videoId: query,
  sortByNewest: false,
  continuation: continuation,
}

ytcm.getComments(payload).then(d => console.log(JSON.stringify(d, null, 2)), e => console.error(JSON.stringify(e, null, 2)));
