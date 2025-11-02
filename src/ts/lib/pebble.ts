// Helper functions for communicating with the pebble
export function sendToPebble(data: any) {
    try {
        PebbleTS.sendAppMessage(data);
        console.log("Sent message to pebble", JSON.stringify(data));
    } catch (e) {
        console.log("Failed to send message to pebble", e);
    }
};