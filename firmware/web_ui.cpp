#include "web_ui.h"
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

static WebServer server(80);
static DNSServer dns;
static ConfigCb  cbConfig = nullptr;
static ResetCb   cbReset  = nullptr;
static StateFn   cbState  = nullptr;

static const char PAGE[] PROGMEM = R"HTML(
<!DOCTYPE html><html><head><meta name=viewport content="width=device-width,initial-scale=1">
<title>Scoreboard</title><style>
body{font-family:sans-serif;background:#111;color:#eee;margin:0;padding:1.2em;max-width:480px}
h1{font-size:1.2em}label{display:block;margin:.8em 0 .2em}
select,input{width:100%;padding:.6em;font-size:1em;border-radius:8px;border:1px solid #444;background:#222;color:#eee;box-sizing:border-box}
button{width:100%;padding:.8em;margin-top:1em;font-size:1.05em;border:0;border-radius:8px;background:#2d7;color:#012;font-weight:700}
#reset{background:#444;color:#eee}.s{display:flex;gap:1em;margin-top:1.2em;font-size:2em;text-align:center}
.s div{flex:1;background:#1c1c1c;border-radius:8px;padding:.4em}
</style></head><body>
<h1>Scoreboard setup</h1>
<label>Game</label>
<select id=game>
<option value=0>Table Tennis</option><option value=1>Tennis</option>
<option value=2>Badminton</option><option value=3>Free Count</option>
<option value=4>Pickleball</option><option value=5>Volleyball</option>
<option value=6>Basketball 21</option><option value=7>Cornhole</option>
</select>
<label>Player A name</label><input id=a maxlength=16 placeholder="Player A">
<label>Player B name</label><input id=b maxlength=16 placeholder="Player B">
<button onclick="apply()">Apply</button>
<button id=reset onclick="rst()">Reset match</button>
<div class=s><div id=sa>-</div><div id=sb>-</div></div>
<script>
function apply(){fetch('/config?game='+game.value+'&a='+encodeURIComponent(a.value)+'&b='+encodeURIComponent(b.value))}
function rst(){fetch('/reset')}
setInterval(function(){fetch('/state').then(r=>r.json()).then(j=>{sa.textContent=j.a+' ['+j.ga+']';sb.textContent=j.b+' ['+j.gb+']'})},1000)
</script></body></html>
)HTML";

static void handleRoot()   { server.send_P(200, "text/html", PAGE); }

static void handleConfig() {
  int g = server.arg("game").toInt();
  String a = server.arg("a"), b = server.arg("b");
  if (cbConfig) cbConfig(g, a.c_str(), b.c_str());
  server.send(200, "text/plain", "ok");
}

static void handleReset()  { if (cbReset) cbReset(); server.send(200, "text/plain", "ok"); }

static void handleState()  {
  char buf[96]; buf[0] = 0;
  if (cbState) cbState(buf, sizeof(buf));
  server.send(200, "application/json", buf);
}

// captive-portal: redirect every unknown host to the page
static void handleCaptive() { server.sendHeader("Location", "http://192.168.4.1/"); server.send(302, "text/plain", ""); }

void web_init(ConfigCb onConfig, ResetCb onReset, StateFn stateJson) {
  cbConfig = onConfig; cbReset = onReset; cbState = stateJson;

  uint8_t mac[6]; WiFi.macAddress(mac);
  char ssid[24]; snprintf(ssid, sizeof(ssid), "Scoreboard-%02X%02X", mac[4], mac[5]);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  IPAddress ip = WiFi.softAPIP();              // 192.168.4.1
  dns.start(53, "*", ip);

  server.on("/", handleRoot);
  server.on("/config", handleConfig);
  server.on("/reset", handleReset);
  server.on("/state", handleState);
  server.onNotFound(handleCaptive);
  server.begin();
}

void web_tick() { dns.processNextRequest(); server.handleClient(); }
