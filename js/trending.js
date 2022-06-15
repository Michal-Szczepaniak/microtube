const ytrend = require("@freetube/yt-trending-scraper")

let parameters = JSON.parse(process.argv[2]);

ytrend.scrape_trending_page(parameters).then(data => {
    console.log(JSON.stringify(data, null, 2));
})
