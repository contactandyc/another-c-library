export default class SettingFunctions {
    constructor() {
        this.diffData=[
            ["Easy<br>(50% Score)", 1, 300, 1000, 0.5],
            ["Normal", 2, 250, 700, 1],
            ["Hard<br>(150% Score)", 2, 210, 600, 1.5],
            ["Impossible<br>(200% Score)", 3, 150, 500, 2],
        ];
        this.themData=[
            ["Default", "teal", "blue", "brown", "yellow", "green", "purple", "red", "darkblue"],
            ["Monochrome", "#007000", "#008700", "#00a000", "#00b700", "#00d000", "#00e700", "#00ff00"]
        ];
        this.visible=false;
        this.Tvisible=this.visible;
        this.difficulty=1;
        this.Tdifficulty=this.difficulty;
        this.cols=15;
        this.Tcols=this.cols;
        this.theme=0;
        this.Ttheme=this.theme;
        this.grid=true;
        this.Tgrid=this.grid;
    }
    bindButton(id, func) {
        document.getElementById(id).addEventListener("click", func);
    }
    bindButtons(Render) {
        this.bindButton("settings-visible", ()=>{
            this.change("visible", true);
            this.handleChanges(true);
            this.pushChanges(Render);
        });
        this.bindButton("pop-ups-button-ok", ()=>{
            this.change("visible", false);
            this.handleChanges(true);
            this.pushChanges(Render);
        });
        this.bindButton("pop-ups-button-cancel", ()=>{
            this.handleChanges(false); // Cancel previous changes
            this.change("visible", false);
            this.handleChanges(true);
            this.pushChanges(Render);
        });
        this.bindButton("diff-down", ()=>{
            this.change("difficulty", this.Tdifficulty-1<0?
                this.diffData.length-1:this.Tdifficulty-1
            )
        });
        this.bindButton("diff-up", ()=>{
            this.change("difficulty", this.Tdifficulty==this.diffData.length-1?
                0:this.Tdifficulty+1
            )
        });
        this.bindButton("cols-up", ()=>{
            this.change("columns", this.Tcols+1)
        });
        this.bindButton("cols-down", ()=>{
            this.change("columns", this.Tcols-1<5?
            5 : this.Tcols-1
            )
        });
        this.bindButton("theme-down", ()=>{
            this.change("theme", this.Ttheme-1<0?
                this.themData.length-1:this.Ttheme-1
            )
        });
        this.bindButton("theme-up", ()=>{
            this.change("theme", this.Ttheme==this.themData.length-1?
                0:this.Ttheme+1
            )
        });
        this.bindButton("grid-up", ()=>{
            this.change("grid", !this.Tgrid)
        });
        this.bindButton("grid-down", ()=>{
            this.change("grid", !this.Tgrid)
        });
        this.bindButton("settings-restart",()=>{
            Render.nextShape();
            Render.restart();
        });
        this.bindButton("settings-restart",()=>{
            Render.restart();
        });
        this.bindButton("move-left",()=>{
            Render.Move.strafe(Render, [-1, 0]);
            Render.update();
        });
        this.bindButton("rotate-left",()=>{
            Render.Move.rotate(Render, 1);
            Render.update();
        });
        this.bindButton("move-down",()=>{
            Render.Move.strafe(Render, [0, 1]);
            Render.update();
        });
        this.bindButton("move-right",()=>{
            Render.Move.strafe(Render, [1, 0]);
            Render.update();
        });
        this.bindButton("rotate-right",()=>{
            Render.Move.rotate(Render, 3);
            Render.update();
        });
    }
    change(setting, value) {
        switch (setting) {
            case "visible":
                this.Tvisible=value;
                return;
            case "difficulty":
                this.Tdifficulty=value;
                document.getElementById("diff-text").innerHTML=this.diffData[value][0];
                return;
            case "columns":
                this.Tcols=value;
                document.getElementById("cols-text").innerHTML=this.Tcols;
                return;
            case "theme":
                this.Ttheme=value;
                document.getElementById("theme-text").innerHTML=this.themData[value][0];
                return;
            case "grid":
                this.Tgrid=value;
                document.getElementById("grid-text").innerHTML=this.Tgrid?"On":"Off";
                return;
        }
    }
    handleChanges(submit) {
        if (submit) { //Reject changes is when submit=false
            this.visible=this.Tvisible;
            this.difficulty=this.Tdifficulty;
            this.cols=this.Tcols;
            this.theme=this.Ttheme;
            this.grid=this.Tgrid;
            return;
        }
        this.Tvisible=this.visible;
        this.Tdifficulty=this.difficulty;
        this.Tcols=cols;
        this.Ttheme=theme;
        this.Tgrid=grid;
    }
    pushChanges(Render) {
        document.getElementById("pop-ups-settings").style.display=this.visible?"flex":"none";
        Render.pause=this.visible;
        document.getElementById("diff-text").innerHTML=this.diffData[this.difficulty][0];
        Render.speed_up=this.diffData[this.difficulty][1];
        Render.speed_cap=this.diffData[this.difficulty][2];
    }
}