import { RequestBeginAuth } from "./request_options/begin_auth";

const HCB_CLIENT_ID="wJzKxOlT5rqnJh9dx0Q8HEQUXVRdn32Q2qH0dUBeEtI";
const HCB_OAUTH_BASE="http://192.168.1.119:3000";

Pebble.addEventListener("ready", (e) => {
    PebbleTS.sendAppMessage({'APP_READY': true });
});

Pebble.addEventListener("appmessage", (dict) => {
  switch (dict.payload["REQUEST"]) {
    case "BEGIN_AUTH":
      RequestBeginAuth(`${HCB_OAUTH_BASE}/api/v4/oauth`, HCB_CLIENT_ID)
    case "":
  }
});