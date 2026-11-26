const start=document.querySelector('#start-screen'),playScreen=document.querySelector('#play-screen');
const play=document.querySelector('#play-game'),loading=document.querySelector('#loading-status');
const canvas=document.querySelector('#canvas');
let started=false,ready=false;
window.neonShowStart=function(saved=false){
 start.hidden=false;playScreen.hidden=true;play.disabled=false;
 play.textContent=ready?'Resume Neon Ante':'Play Neon Ante';
 if(saved)loading.textContent='Run saved in this browser. Resume whenever you are ready.';
};
window.neonGameReady=function(){
 ready=true;start.hidden=true;playScreen.hidden=false;play.disabled=false;canvas.focus();
};
window.Module={
 canvas,
 setStatus(text){if(text)loading.textContent=text;},
 onAbort(){play.disabled=false;loading.textContent='The game could not start. Refresh to try again, or use the Windows download.';},
 printErr(...parts){console.error(...parts);}
};
play.addEventListener('click',()=>{
 if(ready){window.neonGameReady();return;}
 if(started){location.reload();return;}
 started=true;play.disabled=true;play.textContent='Loading the table…';loading.textContent='Loading the game and fonts…';
 const script=document.createElement('script');script.src='neon.js';script.onerror=()=>{play.disabled=false;loading.textContent='Download interrupted. Press Play to reload and try again.';};document.body.appendChild(script);
});
canvas.addEventListener('contextmenu',event=>event.preventDefault());
canvas.addEventListener('pointerdown',()=>canvas.focus());
window.addEventListener('keydown',event=>{
 if(ready&&document.activeElement===canvas&&[' ','Backspace','Tab','ArrowUp','ArrowDown','ArrowLeft','ArrowRight'].includes(event.key))event.preventDefault();
});
document.querySelector('#export-build').addEventListener('click',()=>{if(ready)Module.ccall('neon_export_build',null,[],[]);});
document.querySelector('#build-upload').addEventListener('change',async event=>{
 const input=event.currentTarget,file=input.files[0],status=document.querySelector('#build-status');
 if(!file)return;
 if(file.size>8192){status.textContent='That build file is too large. Choose a .neonbuild file smaller than 8 KB.';input.value='';return;}
 try{const text=await file.text();Module.ccall('neon_import_build',null,['string'],[text]);status.textContent='Build checked. Open Modes for the import result.';canvas.focus();}
 catch{status.textContent='That file could not be read. Your current run is unchanged.';}
 input.value='';
});
document.querySelector('#fullscreen').addEventListener('click',async()=>{
 try{if(document.fullscreenElement)await document.exitFullscreen();else await document.querySelector('#game-frame').requestFullscreen();}catch{document.querySelector('#build-status').textContent='Fullscreen is unavailable in this browser. You can still play here.';}
});
