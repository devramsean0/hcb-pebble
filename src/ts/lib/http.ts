export function xhrWrapper(url: string, type: string, content_type: string, data: any, callback: (xhr: XMLHttpRequest) => void) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(xhr);
  };
  xhr.open(type, url);
  if(data) {
    xhr.setRequestHeader('Content-Type', content_type);
    if (content_type == "application/json") {
        xhr.send(JSON.stringify(data));
    } else {
        xhr.send(data);
    }
  } else {
    xhr.send();
  }
}