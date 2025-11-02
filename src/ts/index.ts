Pebble.addEventListener("ready", (e) => {
    PebbleTS.sendAppMessage({'APP_READY': true });
});

Pebble.addEventListener("appmessage", (dict) => {
    switch (dict.payload["REQUEST"]) {
        case "LIST_ORG":
            
    }
});


function xhrWrapper(url: string, type: string, data: any, callback: (xhr: XMLHttpRequest) => {}) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(xhr);
  };
  xhr.open(type, url);
  if(data) {
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send(JSON.stringify(data));
  } else {
    xhr.send();
  }
}