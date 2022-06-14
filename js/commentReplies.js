const ytcm = require("@freetube/yt-comment-scraper");

let query = process.argv[2];
let replyToken = process.argv[3];

const payload = {
  videoId: query,
  replyToken: replyToken,
}

ytcm.getCommentReplies(payload).then(d => console.log(JSON.stringify(d, null, 2)), e => console.error(JSON.stringify(e, null, 2)));
