let bombs = 30,
    bombs_des = 0,
    cols = 14,
    rows = 14,
    flags = 0,
    board = [],
    timer = 0,
    setting_danger = true,
    setting_flag = false,
    setting_visible = false,
    bef_bomb_change = bombs,
    crt_bomb_count = bombs,
    game_over = false,
    info;
function show(id, vis) {
    document.getElementById(id).style.display=vis?"flex":"none";
}
function bomb_move(x, y) {
    if (x < 0 || x >= rows || y < 0 || y >= cols || board[x][y] == 2)
        return;
    let b = document.getElementById(`b_${x}_${y}`).style;
    b.animationName = "boom";
    board[x][y] = 2;
    setTimeout(() => {
        bomb_move(x - 1, y + 1);
        bomb_move(x, y + 1);
        bomb_move(x + 1, y + 1);
        bomb_move(x - 1, y);
        bomb_move(x + 1, y);
        bomb_move(x - 1, y - 1);
        bomb_move(x, y - 1);
        bomb_move(x + 1, y - 1);
    }, 100);
}
function revert_settings() {
    bombs = bef_bomb_change;
}
function setting(type, id) {
    switch (type) {
        case 'flag':
            setting_flag = !setting_flag;
            document.getElementById("info_flags_img").style.filter = setting_flag ?
            "grayscale(0)" : "grayscale(100%)";
            break;
        case 'visible':
            setting_visible = !setting_visible;
            show("settings", setting_visible)
            break;
        case 'bomb':
            bef_bomb_change = bombs;
            let bmbs = document.getElementById(id);
            if (!isNaN(bmbs.value)) {
                if (bmbs.value > rows * cols)
                    alert(`Bomb limit is ${bmbs.value=rows*cols}, bomb count has been reduced to that.`);
                bombs = Number(bmbs.value);
            }
            else
                alert(`${bmbs} is not a number. Set bombs to default (99).`);
    }
}
function init_board() {
    for (let j = []; board.length < rows; board.push(j)) {
        j = [];
        for (let i = 0; i++ < cols; j.push(0));
    }
}
function place_bombs() {
    crt_bomb_count = bombs;
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
    flag.backgroundImage = "url('images/flag.png')";
    flag.animationName = "flag";
}
function is_flagged(x , y) {
    return document.getElementById(`b_${x}_${y}`).style.backgroundImage != "";
}
function lose() {
    update_info();
    game_over=true;
    show("loser", true);
    clearInterval(info);
}
function win() {
    game_over=true;
    update_info();
    let tmr = document.getElementById("winner_timeleft");
    timer--;
    let sec = timer % 60;
    tmr.innerText = `Time - ${Math.floor(timer / 60)}:${sec<10?"0":""}${timer % 60}`;
    show("winner", true)
    clearInterval(info);
}
function play_again(win) {
    game_over=false;
    let brd = document.getElementById("board");
    brd.innerHTML = "";
    board = [];
    bombs_des = flags = timer = 0;
    init_board();
    place_bombs();
    generate_board();
    info = setInterval(update_info, 1000);
    if (win=="restart")
        return;
    show(win ? "winner" : "loser", false)
}
function left_click(e) {
    if (game_over)
        return;
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
    if (board[cd[0]][cd[1]] == -1) {
        for (let i = 0; i < board.length; i++)
            for (let j = 0; j < board[i].length; j++)
                if (board[i][j] == -1) {
                    let b = document.getElementById(`b_${i}_${j}`).style;
                    b.animationDuration = "1.2s";
                    b.animationName = "boom";
                    b.backgroundImage = "url('images/bomb.png')";
                }
        setTimeout(lose, rows * 100);
        bomb_move(cd[0], cd[1]);
    }
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
function start() {

}
function zero_move(x, y) {
    if (x < 0 || x >= rows || y < 0 || y >= cols || board[x][y] == 1 || board[x][y] == 2 || is_flagged(x, y))
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
    IF.innerText = `${flags} / ${crt_bomb_count}`;
    timer++;
}
function generate_board() {
    let brd = document.getElementById("board");
    for (let x = 0; x < rows; x++) {
        for (let y = 0; y < cols; y++) {
            let d = document.createElement("div");
            let p = document.createElement("p");
            d.id = `b_${x}_${y}`;
            d.style.animationName="init";
            d.addEventListener("click", left_click);
            d.addEventListener("contextmenu", right_click);
            p.id = `p_${x}_${y}`;
            d.appendChild(p);
            brd.appendChild(d);
        }
    }
}
document.addEventListener("contextmenu", e=>{e.preventDefault()}); // to prevent misclicks to bring up menu
init_board();
window.onload = generate_board;