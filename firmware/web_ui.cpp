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
button{transition:transform .08s,opacity .2s}button:active{transform:scale(.97)}
button:disabled{opacity:.55}
#msg{margin:.8em 0;min-height:1.3em;padding:.55em .8em;border-radius:8px;font-weight:600;opacity:0;transition:opacity .15s}
#msg.show{opacity:1}#msg.ok{background:#143d2b;color:#bdf5d6}#msg.err{background:#4a1f1f;color:#ffbcbc}
@keyframes flash{0%{transform:scale(.92)}55%{transform:scale(1.05)}100%{transform:scale(1)}}
#msg.flash{animation:flash .35s ease}
.spin{display:inline-block;width:1em;height:1em;border:2px solid rgba(0,0,0,.35);border-top-color:#012;border-radius:50%;animation:spin .7s linear infinite;vertical-align:-2px;margin-right:.45em}
@keyframes spin{to{transform:rotate(360deg)}}
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
<button id=applyBtn onclick="apply()">Apply</button>
<button id=reset onclick="rst()">Reset match</button>
<p id=msg></p>
<div class=s><div id=sa>-</div><div id=sb>-</div></div>
<p style="font-size:.8em;color:#888">Buttons not responding? <b>Turn off mobile
data</b> (the phone may send requests over cellular), and open
<b>http://192.168.4.1</b> in a normal Chrome/Safari tab (not the auto sign-in popup).</p>
<script>
function $(i){return document.getElementById(i)}
var FAIL='Could not reach the board. Open http://192.168.4.1 in a normal browser tab.';
function banner(t,cls){var m=$('msg');m.className='show flash '+cls;m.textContent=t;
  setTimeout(function(){m.classList.remove('flash')},360);}
function busy(btn,on,label){btn.disabled=on;btn.innerHTML=on?'<span class=spin></span>'+label:label;}
function apply(){
  var b=$('applyBtn'); busy(b,true,'Applying...'); banner('Applying...','ok');
  fetch('/config?game='+$('game').value+'&a='+encodeURIComponent($('a').value)+'&b='+encodeURIComponent($('b').value))
    .then(function(r){return r.text()})
    .then(function(){busy(b,false,'Apply'); banner('Applied ✓  board updated','ok');})
    .catch(function(){busy(b,false,'Apply'); banner(FAIL,'err');});
}
function rst(){
  var b=$('reset'); busy(b,true,'Resetting...'); banner('Resetting...','ok');
  fetch('/reset')
    .then(function(r){return r.text()})
    .then(function(){busy(b,false,'Reset match'); banner('Match reset ✓','ok');})
    .catch(function(){busy(b,false,'Reset match'); banner(FAIL,'err');});
}
setInterval(function(){
  fetch('/state').then(function(r){return r.json()}).then(function(j){
    $('sa').textContent=j.a+' ['+j.ga+']'; $('sb').textContent=j.b+' ['+j.gb+']';
  }).catch(function(){});
},1000);
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

// Connectivity-check replies. Phones probe these URLs to decide whether a Wi-Fi
// network "has internet". If we DON'T answer success, the phone marks the AP as
// dead and routes traffic (incl. the page's fetch() calls) over mobile data, so
// requests to 192.168.4.1 never arrive — the page loads but buttons do nothing.
// Replying "success" keeps the phone on Wi-Fi; the user opens 192.168.4.1.
static void handle204()     { server.send(204, "text/plain", ""); }                 // Android generate_204
static void handleApple()   { server.send(200, "text/html",
                                "<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>"); } // iOS
static void handleNcsi()    { server.send(200, "text/plain", "Microsoft NCSI"); }    // Windows
static void handleConnTest(){ server.send(200, "text/plain", "Microsoft Connect Test"); }

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
  // OS connectivity probes → "success" so the phone keeps the Wi-Fi route
  server.on("/generate_204", handle204);
  server.on("/gen_204", handle204);
  server.on("/hotspot-detect.html", handleApple);
  server.on("/library/test/success.html", handleApple);
  server.on("/ncsi.txt", handleNcsi);
  server.on("/connecttest.txt", handleConnTest);
  server.onNotFound(handleRoot);   // any other path → just serve the setup page
  server.begin();
}

void web_tick() { dns.processNextRequest(); server.handleClient(); }
