import { xhrWrapper } from "./lib/http";

const HCB_CLIENT_ID="";
const HCB_OAUTH_BASE="https://hcb.hackclub.com/api/v4/oauth";

Pebble.addEventListener("ready", (e) => {
    PebbleTS.sendAppMessage({'APP_READY': true });
});

Pebble.addEventListener("appmessage", (dict) => {
    switch (dict.payload["REQUEST"]) {
        case "BEGIN_AUTH":
          // This is sent by the client if it doesn't have an access token or a valid refresh token
          let user_code: string = "";
          let device_code: string = "";
          let interval: number = 5000;
          let access_token: string = "";
          let refresh_token: string = "";
          // Get device Code
          xhrWrapper(`${HCB_OAUTH_BASE}/authorize_device`, "POST", "application/x-www-form-urlencoded", `client_id=${HCB_CLIENT_ID}&scope=read`, (xhr) => {
            const response: HCBOauthAuthorizeDevice = JSON.parse(xhr.response);
            user_code = response.user_code;
            device_code = response.device_code;
            if (response.interval) {
              interval = response.interval;
            }
            // Send the User Code to watch
            PebbleTS.sendAppMessage({'USER_AUTH_CODE': user_code})
            const checkforconfirmed = setInterval(() => {
              // We need to poll to see if the user code has been authorized
              xhrWrapper(`${HCB_OAUTH_BASE}/token`, "POST", "application/x-www-form-urlencoded", `grant_type=urn:ietf:params:oauth:grant-type:device_code&device_code=${device_code}&client_id=${HCB_CLIENT_ID}`, (xhr) => {
                if (xhr.status != 200) {
                  const response: HCBOauthTokenError = JSON.parse(xhr.response)
                  if (response.error === "expired_token") {
                    // If token has expired, kill the loop
                    PebbleTS.sendAppMessage({'AUTH_TIMEDOUT': true})
                    checkforconfirmed.close();
                  }
                } else {
                  const response: HCBOauthTokenOK = JSON.parse(xhr.response)
                  // We should have our access token & refresh token now!
                  access_token = response.access_token,
                  refresh_token = response.refresh_token
                  PebbleTS.sendAppMessage({
                    "AUTH_ACCESS_TOKEN": access_token,
                    "AUTH_REFRESH_TOKEN": refresh_token
                  })
                }
              })
            }, interval);
          })
    }
});

interface HCBOauthAuthorizeDevice {
  device_code: string,
  user_code: string,
  verification_uri: string,
  verification_uri_complete: string,
  expires_in: number,
  interval?: number
}

interface HCBOauthTokenError {
  error: string,
  error_description: string
}

interface HCBOauthTokenOK {
  access_token: string,
  refresh_token: string,
  token_type: string,
  expires_in: number,
  scope: string,
  created_at: number
}