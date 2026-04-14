const s1 = 'javascript:(function(){if(!window.__openclawBridge){window.__openclawBridge={receiveMessage:function(jsonStr){try{var msg=JSON.parse(jsonStr);var event;if(typeof(CustomEvent)===\'function\'){event=new CustomEvent(\'openclaw-message\',{detail:msg});}else{event=document.createEvent(\'CustomEvent\');event.initCustomEvent(\'openclaw-message\',true,true,msg);}window.dispatchEvent(event);}catch(e){if(window.console)console.error(\'[OpenClaw] Bridge Error:\',e);}},sendMessage:function(msg){window.location.href=\'app://message/\'+encodeURIComponent(JSON.stringify(msg));}};if(window.console)console.log(\'[OpenClaw] Bridge Ready (Legacy Polyfilled)\');}})();';
console.log('Script 1 length:', s1.length);
console.log('Script 1 char 144:', s1.substring(134, 154));

const s2 = 'javascript:(function(){if(typeof window.__openclawBridge!==\'undefined\'&&window.__openclawBridge.receiveMessage){window.__openclawBridge.receiveMessage(\"{\\\"payload\\\":{\\\"capabilities\\\":[\\\"widgets\\\",\\\"ai\\\",\\\"osc\\\"],\\\"version\\\":\\\"1.0.0\\\"},\\\"timestamp\\\":1712959800000,\\\"type\\\":\\\"plugin.init\\\"}\");}else{if(window.console)console.warn(\'OpenClaw Bridge: __openclawBridge non inizializzato\');}})();';
console.log('Script 2 length:', s2.length);
console.log('Script 2 char 144:', s2.substring(134, 154));
