const ytsr = require('ytsr');

Object.prototype.pop = function() {
    for (var key in this) {
        if (!Object.hasOwnProperty.call(this, key)) continue;
        var result = this[key];
        if (!delete this[key]) throw new Error();
        return [key, result];
    }
};

let query = process.argv[2];
let options = JSON.parse(process.argv[3]);
let filterMap = new Map(Object.entries(JSON.parse(process.argv[4])));

function fixData(d) {
    for (i of d.items) {
        i.thumbnail = i.bestThumbnail.url;
        i.link = i.url;
        i.uploaded_at = i.uploadedAt;
        if (i.author) i.author.ref = i.author.url;
    }
    if (d.continuation) {
//        ytsrSearch.continuation = d.continuation;
        d.nextpageRef = d.continuation;
    }
    console.log(JSON.stringify(d, null, 2))
}

if (options.limit) {
    options.limit = undefined;
    options.pages = 1;
}

if (options.nextpageRef) {
    ytsr.continueReq(options.nextpageRef).then(d => fixData(d));
} else if (filterMap.size === 0) {
    ytsr(options.nextpageRef ? options.nextpageRef : query, options).then(d => fixData(d));
} else {
    function applyFilters(filterRef) {
        if (filterMap.size === 0) {
            ytsr(filterRef, options).then(d => {
                if (d.items.length) fixData(d);
                else return (new Error('No results'));
            });
        } else {
            let entry = filterMap.entries().next().value;
            let name = entry[0];
            let value = entry[1];
            filterMap.delete(name);
            ytsr.getFilters(filterRef).then(filters => {
                // let filter = filters.get(name).find(o => o.name.startsWith(value));
                let filterCategory = filters.get(name);
                let filter;
                filterCategory.forEach(o => {
                    if (o.name.startsWith(value)) {
                        filter = o;
                    }
                });
                if (filter) {
                    applyFilters(filter.url, options);
                } else {
                    console.log("Filter not found", name, value);
                    applyFilters(filterRef);
                }
            });
        }
    }
    applyFilters(query);
}

//ytsr(query).then(response => {
//   console.log(JSON.stringify(response, null, 2))
//}).catch((err) => {
//   console.log(err)
//})
