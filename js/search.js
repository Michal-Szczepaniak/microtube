const ytsr = require('ytsr');

let query = process.argv[2];
let options = JSON.parse(process.argv[3]);

if ("continuation" in options && options["continuation"] !== "") {
    ytsr.continueReq(options["continuation"]).then(response => {
           console.log(JSON.stringify(response, null, 2));
       },
       error => {
       }
   )
} else {
    ytsr(query, options).then(response => {
            console.log(JSON.stringify(response, null, 2));
        },
        error => {
		console.error(error)
        }
    )
}
