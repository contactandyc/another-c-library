import MoveFunctions from "./move_functions.js";

export default class KeyboardFunctions {
    keydown(e, Render) {
        let Move=new MoveFunctions();
        switch (e.key.toLowerCase()) {
            case "arrowleft":
                Move.rotate(Render, 1);
                break;
            case "arrowright":
                Move.rotate(Render, 3);
                break;
            case "a":
                Move.strafe(Render, [-1, 0]);
                break;
            case "d":
                Move.strafe(Render, [1, 0]);
                break;
            case "s":
                Move.strafe(Render, [0, 1]);
                break;
        }
        Render.update();
    }
}