let bombs = 30,
    bombs_des = 0,
    cols = 14,
    rows = 14,
    flags = 0,
    board = [],
    timer = 0,
    is_on_phone = false,
    setting_danger = true,
    setting_flag = false,
    setting_visible = false;
    info = setInterval(update_info, 1000);
if( /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent))
    is_on_phone = true;
function setting(type) {
    switch (type) {
        case 'flag':
            setting_flag = !setting_flag;
            document.getElementById("info_flags_img").style.filter = setting_flag ?
            "grayscale(0)" : "grayscale(100%)";
            break;
        case 'visible':
            setting_visible = !setting_visible;
            document.getElementById("settings").style.animationName=setting_visible?"slidein":"slideout";
            break;
    }
}
function init_board() {
    for (let j = []; board.length < rows; board.push(j)) {
        j = [];
        for (let i = 0; i++ < cols; j.push(0));
    }
}
function place_bombs() {
    let bmbs = document.getElementById("setting_bombs");
    if (!isNaN(bmbs.value)) {
        if (bmbs.value > rows * cols) {
            alert(`Bomb limit is ${rows*cols}, bomb count has been reduced to that.`);
            bmbs.value = rows*cols;
        }
        bombs = Number(bmbs.value);
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
function count_func(x, y, func) {
    return func(x - 1, y + 1) +
           func(x, y + 1) +
           func(x + 1, y + 1) +
           func(x - 1, y) + 
           func(x + 1, y) + 
           func(x - 1, y - 1) +
           func(x, y - 1) +
           func(x + 1, y - 1);
}
function count_bomb(x, y) {
    return (!(x < 0 || x >= rows || y < 0 || y >= cols) && board[x][y] == -1)?
    1 : 0;
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
    let sec = timer % 60;
    tmr.innerText = `Time - ${Math.floor(timer / 60)}:${sec<10?"0":""}${timer % 60}`;
    wnr.style.animationName = "slidein";
    clearInterval(info);
}
function play_again(win) {
    let bkg = document.getElementById("background");
    let dlg = document.getElementById(win ? "winner" : "loser");
    let brd = document.getElementById("board");
    dlg.style.animationName = "slideout";
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
    if (is_flagged(cd[0], cd[1])) {
        flags--;
        if (board[cd[0]][cd[1]] == -1)
            bombs_des--;
        document.getElementById(`b_${cd[0]}_${cd[1]}`).style.backgroundImage = "";
        return;
    }
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
        return clear_number(cd[0], cd[1]);;
    if (bombs_des == bombs)
        win();
    flag(cd[0], cd[1]);
    flags++;
}
function zero_move(x, y) {
    if (x < 0 || x >= rows || y < 0 || y >= cols || board[x][y] == 1 || is_flagged(x, y))
        return;
    let b = document.getElementById(`b_${x}_${y}`).style;
    b.animationName = "reveal";
    board[x][y] = 1;
    let cb;
    if (cb = count_func(x, y, count_bomb)) {
        document.getElementById(`p_${x}_${y}`).innerHTML = cb;
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
function count_flag(x, y) {
    return (!(x < 0 || x >= rows || y < 0 || y >= cols) && is_flagged(x, y))?
    1 : 0;
}
function clear_number(x, y) {
    if (count_func(x, y, count_flag) != count_func(x, y, count_bomb)) 
        return;
    if (count_func(x, y, is_flag_valid) != 8
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
    let IT = document.getElementById("info_timer_text");
    let IF = document.getElementById("info_flags_text");
    let sec = timer % 60;
    IT.innerText = `${Math.floor(timer / 60)}:${sec<10?"0":""}${timer % 60}`;
    IF.innerText = `${flags} / ${bombs}`;
    timer++;
}
function generate_board() {
    let brd = document.getElementById("board");
    for (let x = 0; x < rows; x++) {
        let r = document.createElement("div");
        r.style.left = is_on_phone ? 
        `${x * 60}px` : `${x * 42}px`;

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