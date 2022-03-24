import RenderFunctions from "./render_functions.js";
import MoveFunctions from "./move_functions.js";
import KeyboardFunctions from "./keyboard_functions.js";

let Render=new RenderFunctions(),
    Move=new MoveFunctions(),
    Keyboard=new KeyboardFunctions();
function loop() {
    setTimeout(loop, Render.timer);
    if (Render.pause)
        return;
    Render.update();
    Move.strafe(Render, [0, 1]);
}

document.addEventListener("keydown", e=>{
    Keyboard.keydown(e, Render);
});
window.onload=()=>{
    Render.generateBoard();
    document.getElementById("pop-ups-intro-go").addEventListener("click", ()=>{
        Render.pause=false;
        loop();
        document.getElementById("pop-ups-intro").style.display="none";
        Render.restart();
    });
    Render.Setting.bindButtons(Render);
};