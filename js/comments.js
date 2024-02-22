import { Innertube, YTNodes } from 'youtubei.js';

const yt = await Innertube.create({
    lang: 'en',
    location: 'US',
    retrieve_player: false,
    enable_safety_mode: false,
});

const channelId = process.argv[2];
const continuation = JSON.parse(process.argv[3])

let comments = null
let continuationData = null
if (Object.keys(continuation).length) {
    const response = await yt.actions.execute(continuation.endpoint.metadata.api_url, {
        ...continuation.endpoint.payload,
        parse: true
    });

    comments = []
    response.on_response_received_endpoints[0].contents.forEach(element => {
                                                        if (element.type === "CommentThread") {
                                                            comments.push(element)
                                                        } else if (element.type === "ContinuationItem") {
                                                            continuationData = element
                                                        }
                                                    })
} else {
    const commentsData = await yt.getComments(channelId);

    continuationData = commentsData.page.on_response_received_endpoints.at(1).contents.firstOfType(YTNodes.ContinuationItem);
    comments = commentsData.contents;
}

const result = {
    items: comments,
    continuation: continuationData
}

console.log(JSON.stringify(result, null, 2))
