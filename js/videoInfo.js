import { Innertube, Platform } from 'youtubei.js';

Platform.shim.eval = async (data, env) => {
  const properties = [];

  if(env.n) {
    properties.push(`n: exportedVars.nFunction("${env.n}")`)
  }

  if (env.sig) {
    properties.push(`sig: exportedVars.sigFunction("${env.sig}")`)
  }

  const code = `${data.output}\nreturn { ${properties.join(', ')} }`;

  return new Function(code)();
}

const query = process.argv[2];
const parameters = JSON.parse(process.argv[3]);

const youtube = await Innertube.create({
    lang: parameters.language,
    location: parameters.country,
    enable_safety_mode: parameters.safeSearch,
    generate_session_locally: true,
    enable_session_cache: false,
});

const info = await youtube.getInfo(query);

console.log(JSON.stringify(info, null, 2))
