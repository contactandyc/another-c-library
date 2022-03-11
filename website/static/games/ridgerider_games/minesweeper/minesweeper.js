let bombs=50,
    rows=30,
    cols=20,
    flags=0,
    bombs_des=0,
    board=[];
function init_board() {
    for (let j=[]; board.length<rows; board.push(j)) {
        j=[];
        for (let i=0; i++<cols; j.push(0));
    } 
}
function place_bombs() {
    for (let i=0; i<bombs;) {
        let x=Math.floor(Math.random()*rows);
        let y=Math.floor(Math.random()*cols);
        if (board[x][y])
            continue;
        board[x][y]=-1;
        i++;
    }
}
function count_bomb(x, y) {
    return (x<0||x>=rows||y<0||y>=cols||board[x][y]==1) ?
        0 : board[x][y]==-1 ?
        1 : 0;
}
function count_bombs(x, y) {
    return count_bomb(x - 1, y + 1) +
           count_bomb(x, y + 1) +
           count_bomb(x + 1, y + 1) +
           count_bomb(x - 1, y) +
           count_bomb(x + 1, y) +
           count_bomb(x - 1, y - 1) +
           count_bomb(x, y - 1) +
           count_bomb(x + 1, y - 1);

}
function zero_move(x, y) {
    if (x<0||x>=rows||y<0||y>=cols||board[x][y]==1||board[x][y]==-1)
        return;
    board[x][y]=1;
    let d=document.getElementById(`b_${x}_${y}`).style;
    d.backgroundColor = "#909090";
    if (count_bombs(x, y)) {
        document.getElementById(`p_${x}_${y}`).innerText=count_bombs(x, y);
        return;
    }
    setTimeout(()=>{
        zero_move(x - 1, y + 1);
        zero_move(x, y + 1);
        zero_move(x + 1, y + 1);
        zero_move(x - 1, y);
        zero_move(x + 1, y);
        zero_move(x - 1, y - 1);
        zero_move(x, y - 1);
        zero_move(x + 1, y - 1);
    }, 100);
}
function left_click(e) {
    let trg=e.currentTarget;
    let spl=trg.id.split("_");
    let cd=[Number(spl[1]), Number(spl[2])];
    if (board[cd[0]][cd[1]]==0)
        zero_move(cd[0], cd[1]);
    else if (board[cd[0]][cd[1]]==-1)
        alert("You lose");
}
function right_click(e) {
    let trg=e.currentTarget;
    e.preventDefault();
    let spl=trg.id.split("_");
    let cd=[Number(spl[1]), Number(spl[2])];
    if (board[cd[0]][cd[1]]==1)
        return;
    if (board[cd[0]][cd[1]]==-1)
        bombs_des++;
    if (bombs_des==bombs)
        alert("you won");
    flags++;
    e.currentTarget.style.backgroundImage="url('flag.png')";
}
function generate_board() {
    let brd=document.getElementById("board");
    for (let x=0; x<rows; x++) {
        let r=document.createElement("div");
        r.style.left=`${x*28}px`;
        for (let y=0; y<cols; y++) {
            let d=document.createElement("div");
            let p=document.createElement("div");
            d.addEventListener("click", left_click);
            d.addEventListener("contextmenu", right_click);
            d.id=`b_${x}_${y}`;
            p.id=`p_${x}_${y}`;
            d.appendChild(p);
            r.appendChild(d);
        }
        brd.appendChild(r);
    }
}
init_board();
place_bombs();
window.onload=generate_board;
console.log(board);