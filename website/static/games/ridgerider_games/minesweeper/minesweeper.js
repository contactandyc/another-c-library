let bombs = 99,
    bombs_des = 0,
    cols = 20,
    rows = 30,
    flags = 0,
    board = [],
    timer = 0,
    setting_danger = true,
    setting_flag = false;
    info = setInterval(update_info, 1000);
function setting(type) {
    let outter = document.getElementById(`setting_${type}`);
    let inner =  document.getElementById(`setting_${type}_inner`);
    switch (type) {
        case 'danger':
            setting_danger = !setting_danger;
            inner.style.left = setting_danger ? "12px" : "-12px";
            outter.style.backgroundColor = setting_danger ? "green" : "red";
            break;
        case 'flag':
            setting_flag = !setting_flag;
            inner.style.left = setting_flag ? "12px" : "-12px";
            outter.style.backgroundColor = setting_flag ? "green" : "red";
    }
}
function init_board() {
    for (let j = []; board.length < rows; board.push(j)) {
        j = [];
        for (let i = 0; i++ < cols; j.push(0));
    }
}
function place_bombs() {
    let bmbs = document.getElementById("setting_bombs").value;
    if (!isNaN(bmbs)) {
        if (bmbs > rows * cols) {
            alert(`Bomb limit is ${rows*cols}, bomb count has been reduced to that.`);
            bmbs = 600;
        }
        bombs = Number(bmbs);
    }
    else
        alert(`${bmbs} is not a number. Set bombs to default (99).`);
    for (let i = bombs; i;) {
        let x = Math.floor(Math.random() * rows) 
        let y = Math.floor(Math.random() * cols);
        if (board[x][y])
            continue;
        board[x][y] = -1;
        i--;
    }
}
function count_bomb(x, y) {
    return (!(x < 0 || x >= rows || y < 0 || y >= cols) && board[x][y] == -1)?
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
function flag(x , y) {
    let flag = document.getElementById(`b_${x}_${y}`).style;
    flag.backgroundImage = "url('flag.png')";
    flag.animationName = "flag";
}
function is_flagged(x , y) {
    return document.getElementById(`b_${x}_${y}`).style.backgroundImage != "";
}
function lose() {
    let bkg = document.getElementById("background");
    let lsr = document.getElementById("loser");
    lsr.style.animationName = "slidein";
    bkg.style.backgroundColor = "#707070";
    
    for (let i = 0; i < board.length; i++)
        for (let j = 0; j < board[i].length; j++)
            if (board[i][j] == -1) {
                let b = document.getElementById(`b_${i}_${j}`).style;
                b.animationDuration = "2s";
                b.animationName = "boom";
                b.backgroundImage = "url('bomb.png')";
            }
    clearInterval(info);
}
function win() {
    let bkg = document.getElementById("background");
    let wnr = document.getElementById("winner");
    let tmr = document.getElementById("winner_timeleft");
    tmr.innerText = `Time - ${Math.floor(timer / 60)}:${timer % 60 - 1}`; 
    wnr.style.animationName = "slidein";
    bkg.style.backgroundColor = "#007000";
    clearInterval(info);
}
function play_again(win) {
    let bkg = document.getElementById("background");
    let dlg = document.getElementById(win ? "winner" : "loser");
    let brd = document.getElementById("board");
    dlg.style.animationName = "slideout";
    bkg.style.backgroundColor = "#006000";
    brd.innerHTML = "";
    board = [];
    bombs_des = flags = timer = 0;
    info = setInterval(update_info, 1000);
    init_board();
    place_bombs();
    generate_board();
}
function left_click(e) {
    if (setting_flag) {
        right_click(e);
        return;
    }
    let d = e.currentTarget;
    let spl = d.id.split("_");
    let cd = [Number(spl[1]), Number(spl[2])];
    if (board[cd[0]][cd[1]] == -1)
        lose();
    else if (board[cd[0]][cd[1]] == 0)
        zero_move(cd[0], cd[1]);
    else
        clear_number(cd[0], cd[1]);
}
function right_click(e) {
    let d = e.currentTarget;
    e.preventDefault();
    let spl = d.id.split("_");
    let cd = [Number(spl[1]), Number(spl[2])];
    if (is_flagged(cd[0], cd[1])) {
        flags--;
        if (board[cd[0]][cd[1]] == -1)
            bombs_des--;
        document.getElementById(`b_${cd[0]}_${cd[1]}`).style.backgroundImage = "";
        return;
    }
    if (board[cd[0]][cd[1]] == -1)
        bombs_des++;
    else if (board[cd[0]][cd[1]] == 1)
        return;
    if (bombs_des == bombs)
        win();
    flag(cd[0], cd[1]);
    flags++;
}
function zero_move(x, y) {
    if (x < 0 || x >= rows || y < 0 || y >= cols || board[x][y] == 1 || is_flagged(x, y))
        return;
    let b = document.getElementById(`b_${x}_${y}`).style;
    b.backgroundColor = "#909090";
    board[x][y] = 1;
    let cb;
    if (cb = count_bombs(x, y)) {
        document.getElementById(`p_${x}_${y}`).innerHTML = cb;
        b.animationName = "reveal";
        if (setting_danger)
                document.getElementById(`b_${x}_${y}`).style.color = `rgb(
                    ${cb>4?64*(cb-4):0},
                     0,
                     ${64 *cb}
                )`;
        return;
    }
    setTimeout(() => {
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
function is_flag_valid(x, y) {
    if (x < 0 || x >= rows || y < 0 || y >= cols)
        return 1;
    return ((board[x][y] == -1 && is_flagged(x, y)) || board[x][y] == 1 || board[x][y] == 0)
}
function clear_number(x, y) {
    if (is_flag_valid(x - 1, y + 1) +
        is_flag_valid(x, y + 1) +
        is_flag_valid(x + 1, y + 1) +
        is_flag_valid(x - 1, y) +
        is_flag_valid(x + 1, y) +
        is_flag_valid(x - 1, y - 1) +
        is_flag_valid(x, y - 1) +
        is_flag_valid(x + 1, y - 1) != 8
    ) {
        lose();
        return;
    }
    zero_move(x - 1, y + 1);
    zero_move(x, y + 1);
    zero_move(x + 1, y + 1);
    zero_move(x - 1, y);
    zero_move(x + 1, y);
    zero_move(x - 1, y - 1);
    zero_move(x, y - 1);
    zero_move(x + 1, y - 1);
}
function update_info() {
    let IT = document.getElementById("info_timer");
    let IF = document.getElementById("info_flags");
    let IB = document.getElementById("info_bombs");
    IT.innerText = `Timer - ${Math.floor(timer / 60)}:${timer % 60}`;
    IF.innerText = `Flags used - ${flags}`
    timer++;
}
function generate_board() {
    let brd = document.getElementById("board");
    for (let x = 0; x < rows; x++) {
        let r = document.createElement("div");
        r.style.position = "absolute";
        r.style.left = `${x * 28}px`;
        for (let y = 0; y < cols; y++) {
            let d = document.createElement("div");
            let p = document.createElement("p");
            d.id = `b_${x}_${y}`;
            d.addEventListener("click", left_click);
            d.addEventListener("contextmenu", right_click);
            p.id = `p_${x}_${y}`;
            d.appendChild(p);
            r.appendChild(d);
        }
        brd.appendChild(r);
    }
}
init_board();
window.onload = ()=>{
    place_bombs();
    generate_board();
}