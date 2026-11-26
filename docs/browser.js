const start=document.querySelector('#start-screen'),playScreen=document.querySelector('#play-screen');
const play=document.querySelector('#play-game'),loading=document.querySelector('#loading-status');
const canvas=document.querySelector('#canvas'),replaceDialog=document.querySelector('#replace-run');
const continueButton=document.querySelector('#continue-run');
const modeNames=['Classic','Daily Seed','Relic Draft','Echo Table'];
const starterNames=['Balanced','Planner','Performer'];
const starterDescriptions=[
 '$6 and Chrome Smile. A flexible start.',
 '$4 and Chrome Smile, with one extra discard every blind.',
 '$6 and Chrome Smile. Alternate hand types for a bonus up to +36%.'
];
let ready=false,hasRun=false,pendingRun=null;
function enterTable(){
 start.hidden=true;playScreen.hidden=false;document.body.classList.add('is-playing');canvas.focus();window.scrollTo(0,0);
}
// The engine validates the save before making Continue available.
window.neonGameReady=function(available,summary){
 ready=true;hasRun=Boolean(available);start.hidden=false;playScreen.hidden=true;
 document.body.classList.remove('is-playing');
 window.scrollTo(0,0);
 document.querySelector('#saved-run').hidden=!hasRun;document.querySelector('#no-save').hidden=hasRun;
 document.querySelector('#save-summary').textContent=hasRun?summary:'';
 play.disabled=false;continueButton.disabled=false;
 loading.textContent=hasRun?'Continue your saved run, or choose a fresh one.':'Choose a mode and starting style, then start your run.';
};
function launchNewRun(){
 if(!ready||!pendingRun)return;
 const result=Module.ccall('neon_begin_run','number',['number','number'],pendingRun);pendingRun=null;
 if(result){hasRun=true;enterTable();}else loading.textContent='That run could not start. Choose a mode and try again.';
}
play.addEventListener('click',()=>{
 if(!ready)return;
 pendingRun=[Number(document.querySelector('input[name="mode"]:checked').value),Number(document.querySelector('input[name="starter"]:checked').value)];
 if(hasRun){
  document.querySelector('#new-run-summary').textContent='New run: '+modeNames[pendingRun[0]]+' / '+starterNames[pendingRun[1]];
  replaceDialog.showModal();document.querySelector('#cancel-new-run').focus();
 }else launchNewRun();
});
document.querySelector('#cancel-new-run').addEventListener('click',()=>{pendingRun=null;replaceDialog.close();});
replaceDialog.addEventListener('cancel',()=>{pendingRun=null;});
document.querySelector('#confirm-new-run').addEventListener('click',()=>{replaceDialog.close();launchNewRun();});
continueButton.addEventListener('click',()=>{if(ready&&hasRun&&Module.ccall('neon_resume_run','number',[],[]))enterTable();});
document.querySelectorAll('input[name="starter"]').forEach(input=>input.addEventListener('change',()=>{
 document.querySelector('#starter-description').textContent=starterDescriptions[Number(input.value)];
}));
document.querySelector('#title-menu').addEventListener('click',async()=>{
 if(!ready)return;
 if(document.fullscreenElement)await document.exitFullscreen().catch(()=>{});
 Module.ccall('neon_return_title',null,[],[]);(hasRun?continueButton:play).focus();
});
function loadingFailed(){
 ready=false;play.disabled=true;continueButton.disabled=true;
 loading.textContent='The game could not load. Retry below, or use the Windows download.';
 document.querySelector('#retry-load').hidden=false;
}
window.Module={
 canvas,locateFile:path=>path+'?v=table-3d-3',
 setStatus(text){if(text&&!ready)loading.textContent=text;},onAbort:loadingFailed,
 printErr(...parts){console.error(...parts);}
};
document.querySelector('#retry-load').addEventListener('click',()=>location.reload());
const engineScript=document.createElement('script');engineScript.src='neon.js?v=table-3d-3';engineScript.onerror=loadingFailed;document.body.appendChild(engineScript);
canvas.addEventListener('contextmenu',event=>event.preventDefault());
canvas.addEventListener('pointerdown',event=>{
 canvas.focus();
 if(event.button===0&&ready&&!playScreen.hidden){
  const bounds=canvas.getBoundingClientRect();
  Module.ccall('neon_pointer_pressed',null,['number','number'],[(event.clientX-bounds.left)*1280/bounds.width,(event.clientY-bounds.top)*800/bounds.height]);
 }
});
window.addEventListener('keydown',event=>{
 if(!playScreen.hidden&&document.activeElement===canvas&&[' ','Backspace','Tab','ArrowUp','ArrowDown','ArrowLeft','ArrowRight'].includes(event.key))event.preventDefault();
});
document.querySelector('#export-build').addEventListener('click',()=>{if(ready)Module.ccall('neon_export_build',null,[],[]);});
document.querySelector('#build-upload').addEventListener('change',async event=>{
 const input=event.currentTarget,file=input.files[0],status=document.querySelector('#build-status');if(!file)return;
 if(file.size>8192){status.textContent='That build file is too large. Choose a .neonbuild file smaller than 8 KB.';input.value='';return;}
 try{const text=await file.text();Module.ccall('neon_import_build',null,['string'],[text]);status.textContent='Build checked. Open Modes for the import result.';canvas.focus();}
 catch{status.textContent='That file could not be read. Your current run is unchanged.';}
 input.value='';
});
document.querySelector('#fullscreen').addEventListener('click',async()=>{
 try{if(document.fullscreenElement)await document.exitFullscreen();else await document.querySelector('#game-frame').requestFullscreen();}catch{document.querySelector('#build-status').textContent='Fullscreen is unavailable in this browser. You can still play here.';}
});
