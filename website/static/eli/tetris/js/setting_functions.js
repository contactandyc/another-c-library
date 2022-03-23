export default class SettingFunctions {
    constructor() {
        this.visible=false;
        this.Tvisible=this.visible;
        this.size=30;
        this.diffData=[
            ["Easy<br>(50% Score)", 1, 300, 1000, 0.5],
            ["Normal", 2, 250, 700, 1],
            ["Hard<br>(150% Score)", 2, 210, 600, 1.5],
            ["Impossible<br>(200% Score)", 3, 150, 500, 2],
        ];
        this.difficulty=1;
        this.Tdifficulty=this.difficulty;
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
        this.bindButton("settings-restart",()=>{
            Render.nextShape();
            Render.restart();
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
        }
    }
    handleChanges(submit) {
        if (submit) { //Reject changes is when submit=false
            this.visible=this.Tvisible;
            this.difficulty=this.Tdifficulty;
            return;
        }
        this.Tvisible=this.visible;
        this.Tdifficulty=this.difficulty;
    }
    pushChanges(Render) {
        document.getElementById("pop-ups-settings").style.display=this.visible?"flex":"none";
        Render.pause=this.visible;

        document.getElementById("diff-text").innerHTML=this.diffData[this.difficulty][0];
        Render.speed_up=this.diffData[this.difficulty][1];
        Render.speed_cap=this.diffData[this.difficulty][2];
    }
}