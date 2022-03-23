import MoveFunctions from "./move_functions.js";
import InfoFunctions from "./info_functions.js";
import SettingFunctions from "./setting_functions.js";
import Shapes from "./shapes.js";

export default class RenderFunctions {
    constructor(rows, cols) {
        let color_map=new Map();
        color_map.set(0, "teal");
        color_map.set(1, "blue");
        color_map.set(2, "brown");
        color_map.set(3, "yellow");
        color_map.set(4, "green");
        color_map.set(5, "purple");
        color_map.set(6, "red");
        color_map.set(7, "darkblue");
        this.color_map=color_map;
        this.speed_up=1, //measured in percent of percent
        this.speed_cap=250,
        this.score=0;
        this.Move=new MoveFunctions();
        this.Info=new InfoFunctions();
        this.Setting=new SettingFunctions();
        this.timer=1000;
        this.current_shape=Math.floor(Math.random()*Shapes.length);
        this.current_shape_rotation=0;
        this.next_shape=Math.floor(Math.random()*Shapes.length);
        this.board=[];
        this.boardRow=[];
        this.rows=rows;
        this.cols=cols;
        this.pause=true;
        this.pos=[Math.floor(this.rows/2-2), 0];
        this.initBoard();
    }
    initBoard() {
        let j;
        for (j=[]; this.board.length<this.rows; this.board.push(j)) {
            j=[];
            for (let i=0; i++<this.cols; j.push(0));
        }
        this.boardRow=j;
    }
    generateBoard() {
        let brd=document.getElementById("board");
        brd.style.width=`${this.rows*34}px`;
        brd.style.height=`${this.cols*34}px`;
        for (let x=0; x<this.rows; x++) {
            for (let y=0; y<this.cols; y++) {
                let d=document.createElement("div");
                d.id=`b_${x}_${y}`;
                brd.appendChild(d);
            }
        }
        this.Info.init(this);
    }
    getShape=()=>Shapes[this.current_shape];
    applyShape() {
        let rot_shp=this.Move.rotateShp(this, this.current_shape_rotation),
            outbrd=this.board,
            old=outbrd;
        for (let x=0; x<rot_shp.length; x++) {
            for (let y=0; y<rot_shp[0].length; y++) {
                if (!rot_shp[x][y]||this.pos[0]+x>=this.board.length||
                    this.pos[0]+x<0||this.pos[1]+y>=this.board[0].length
                   )
                    continue;
                outbrd[this.pos[0]+x][this.pos[1]+y]=1;
            }
        }
        return outbrd;
    }
    clearShape() {
        for (let x=0; x<this.board.length; x++)
            for (let y=0; y<this.board[0].length; y++)
                if (this.board[x][y]==1) {
                    document.getElementById(`b_${x}_${y}`).style.backgroundColor=
                        "#303030";
                        this.board[x][y]=0;
                }
    }
    generateBoardColors(col) {
        for (let y=col; y>0; y--)
            for (let x=0; x<this.rows; x++)
                document.getElementById(`b_${x}_${y}`).style.backgroundColor=
                document.getElementById(`b_${x}_${y-1}`).style.backgroundColor;
        this.clearRow(col); 
    }
    generateShapeColors() {
        let shp=(this.Move.rotateShp(this, this.current_shape_rotation));
        for (let x=0; x<shp.length; x++)
            for (let y=0; y<shp[0].length; y++)
                if (shp[x][y]==1)
                    document.getElementById(`b_${this.pos[0]+x}_${this.pos[1]+y}`).style.backgroundColor=
                        this.color_map.get(this.current_shape);
    }
    fillRow(col) {
        for (let y=col; y>0; y--)
            for (let x=0; x<this.rows; x++)
                this.board[x][y]=this.board[x][y-1];
    }
    clearRow(y) {
        for (let x=0; x<this.rows; x++)
            this.board[x][y]=0;
        this.fillRow(y);
    }
    scanClear(mul) {
        for (let y=this.cols; y-->0;)
            for (let x=0; 1; x++) {
                if (x>=this.rows) {
                    this.generateBoardColors(y);
                    return this.scanClear(mul+1);
                }
                if (!this.board[x][y])
                    break;
            }
        this.score+=Math.floor(([0, 40, 100, 300, 1200])[mul]*this.Setting.diffData[this.Setting.difficulty][4]);
        return -1;
    }
    nextShape() {
        this.pos[1]--;
        this.update();
        for (let x=0; x<this.board.length; x++)
            for (let y=0; y<this.board[0].length; y++)
                if (this.board[x][y])
                    this.board[x][y]=2;
        this.scanClear(0);
        this.current_shape=this.next_shape;
        this.next_shape=Math.floor(Math.random()*Shapes.length);
        this.current_shape_rotation=0;
        this.pos=[Math.floor(this.rows/2-2), 0];
        this.current_shape_rotation=0;
        if (!this.Move.outOfFloor(this, this.getShape()))
            return;
        this.Info.die(this);
        this.pause=true;
    }
    update() {
        this.clearShape();
        this.board=this.applyShape();
        this.generateShapeColors();
        this.Info.update(this);
        this.timer-=this.speed_cap>this.timer?0:this.speed_up;
    }
    restart() {
        this.board=[];
        document.getElementById("board").innerHTML="";
        document.getElementById("next-shape").innerHTML="";
        this.initBoard();
        this.generateBoard();
        this.current_shape=Math.floor(Math.random()*Shapes.length);
        this.next_shape=Math.floor(Math.random()*Shapes.length);
        this.timer=this.Setting.diffData[this.Setting.difficulty][3];
        this.score=0;
        this.pause=false;
        document.getElementById("pop-ups-end").style.display="none";
    }
}