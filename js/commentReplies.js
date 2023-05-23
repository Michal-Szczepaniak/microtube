import { Innertube, YTNodes } from 'youtubei.js';

const yt = await Innertube.create({
    lang: 'en',
    location: 'US',
});

const continuation = JSON.parse(process.argv[2]);

const response = await yt.actions.execute(continuation.endpoint.metadata.api_url, {
    ...continuation.endpoint.payload,
    parse: true
});


let comments = []
let continuationData = null
response.on_response_received_endpoints[0].contents.forEach(element => {
                                                    if (element.type === "Comment") {
                                                        comments.push(element)
                                                    } else if (element.type === "ContinuationItem") {
                                                        continuationData = element
                                                    }
                                                })


const result = {
    items: comments,
    continuation: continuationData
}

console.log(JSON.stringify(result, null, 2))
