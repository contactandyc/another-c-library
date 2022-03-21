import RenderFunctions from "./render_functions.js";
import KeyboardFunctions from "./keyboard_functions.js";
import MoveFunctions from "./move_functions.js";
import InfoFunctions from "./info_functions.js";

let speed_up=1, //measured in percent of percent
    speed_cap=250,
    Render=new RenderFunctions(10, 20),
    Move=new MoveFunctions(),
    Keyboard=new KeyboardFunctions(),
    Info=new InfoFunctions();
function loop() {
    Render.update();
    Move.strafe(Render, [0, 1]);
    setTimeout(loop, Render.timer-=speed_cap>Render.timer?0:speed_up);
}

document.addEventListener("keydown", e=>{
    Keyboard.keydown(e, Render);
});
window.onload=()=>{
    Render.generateBoard();
    Render.update();
    document.getElementById("pop-ups-intro-go").addEventListener("click", ()=>{
        loop();
        document.getElementById("pop-ups-intro").style.display="none";
    });
};