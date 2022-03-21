import Shapes from "./shapes.js";

export default class InfoFunctions {
    init(Render) {
        this.rows=5;
        this.cols=5;
        this.generatePreview();
        document.getElementById("pop-ups-end-restart").addEventListener("click",
            ()=>Render.restart());
    }
    update(Render) {
        document.getElementById("score").innerText=`Score: ${Render.score}`;
        this.updatePreview(Render);
    }
    generatePreview() {
        let nsh=document.getElementById("next-shape");
        nsh.style.width=`${this.rows*20}px`;
        nsh.style.height=`${this.cols*20}px`;
        for (let x=0; x<this.rows; x++) {
            for (let y=0; y<this.cols; y++) { 
                let d=document.createElement("div");
                d.id=`pr_${x}_${y}`;
                nsh.appendChild(d);
            }
        }
    }
    updatePreview(Render) {
        let shp=Shapes[Render.next_shape];
        for (let x=0; x<this.rows; x++) {
            for (let y=0; y<this.cols; y++) {
                let d=document.getElementById(`pr_${x}_${y}`);
                if (x<shp.length&&y<shp[0].length&&shp[x][y]==1)
                    d.style.backgroundColor=
                    Render.color_map.get(Render.next_shape);
                else
                    d.style.backgroundColor=
                    "#101010";
            }
        }
    }
    die(Render) {
        document.getElementById("pop-ups-end").style.display="flex";
        document.getElementById("pop-ups-end-score").innerHTML=`Score: ${Render.score}`;
    }
}