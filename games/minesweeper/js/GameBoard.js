export default class GameBoard {
  static BOMB = 1;
  static FLAG = 2;
  static HIDDEN = 4;
  static REVEALED = 8;
  static EXPLODED = 16;
  constructor(parent, size, click, rightClick) {
    this.size = size;
    this.parent = document.getElementById(parent);
    this.click = click;
    this.rightClick = rightClick;
    if (parent === undefined) {
      throw Error(
        `[Error] GameBoard: parent element, ${parent}, does not exist`
      );
    }
    this.buildBoard();
  }

  isInBounds(pos) {
    return pos.y < this.size && pos.y >= 0 && pos.x < this.size && pos.x >= 0;
  }

  compareState(state1, state2) {
    return (state1 & state2) === state2;
  }

  hasState(pos, state) {
    return this.isInBounds(pos) && this.compareState(this.getState(pos), state);
  }

  isRevealed(pos) {
    return this.hasState(pos, GameBoard.REVEALED);
  }

  isExploded(pos) {
    return this.hasState(pos, GameBoard.EXPLODED);
  }

  isFlagged(pos) {
    return this.hasState(pos, GameBoard.FLAG);
  }

  isBomb(pos) {
    if (this.hasState(pos, GameBoard.BOMB)) return 1;
    return 0;
  }

  countBombs(pos) {
    let numBombs =
      this.isBomb({ y: pos.y + 1, x: pos.x }) +
      this.isBomb({ y: pos.y + 1, x: pos.x + 1 }) +
      this.isBomb({ y: pos.y, x: pos.x + 1 }) +
      this.isBomb({ y: pos.y + 1, x: pos.x - 1 }) +
      this.isBomb({ y: pos.y - 1, x: pos.x }) +
      this.isBomb({ y: pos.y - 1, x: pos.x + 1 }) +
      this.isBomb({ y: pos.y, x: pos.x - 1 }) +
      this.isBomb({ y: pos.y - 1, x: pos.x - 1 });
    return numBombs;
  }

  getState(pos) {
    if (!this.isInBounds(pos)) return 0;
    return this.board[pos.y][pos.x].state;
  }

  showNumber(pos, num) {
    if (!this.isInBounds(pos)) {
      throw Error(
        `[Error]GameBoard: Position not in-bounds for showNumber: (${pos.x},${pos.y})`
      );
    }
    if (num > 0) {
      this.board[pos.y][pos.x].el.innerText = num;
    } else {
      this.board[pos.y][pos.x].el.innerText = '';
    }
  }

  setState(pos, state = null) {
    if (!this.isInBounds(pos)) return;
    if (state === null) state = this.getState(pos);
    let square = this.board[pos.y][pos.x];
    this.showNumber(pos, 0);
    if (!this.compareState(state, GameBoard.REVEALED)) {
      square.el.className = 'board-square';
      if (
        (pos.y % 2 == 0 && pos.x % 2 == 0) ||
        (pos.y % 2 == 1 && pos.x % 2 == 1)
      )
        square.el.classList.add('even');
      else square.el.classList.add('odd');
    } else {
      square.el.className = 'board-square dirt';
      if (
        this.compareState(state, GameBoard.BOMB) &&
        !this.compareState(state, GameBoard.FLAG)
      ) {
        square.el.classList.add('bomb');
      } else this.showNumber(pos, this.countBombs(pos));
    }
    if (this.compareState(state, GameBoard.FLAG)) {
      square.el.classList.add('flag');
    }
    square.state = state;
  }

  updateAdjacent(pos) {
    this.setState({ y: pos.y, x: pos.x + 1 });
    this.setState({ y: pos.y, x: pos.x - 1 });
    this.setState({ y: pos.y + 1, x: pos.x });
    this.setState({ y: pos.y - 1, x: pos.x });
    this.setState({ y: pos.y + 1, x: pos.x + 1 });
    this.setState({ y: pos.y + 1, x: pos.x - 1 });
    this.setState({ y: pos.y - 1, x: pos.x + 1 });
    this.setState({ y: pos.y - 1, x: pos.x - 1 });
  }

  addBomb(pos) {
    if (!this.isInBounds(pos)) {
      throw Error(
        `[Error]GameBoard: Position not in-bounds for addBomb: (${pos.x},${pos.y})`
      );
    }
    let state = this.getState(pos);
    this.setState(pos, state | GameBoard.BOMB);
    this.updateAdjacent(pos);
  }

  toggleFlag(pos) {
    if (!this.isInBounds(pos)) {
      throw Error(
        `[Error]GameBoard: Position not in-bounds for toggleFlag: (${pos.x},${pos.y})`
      );
    }
    let state = this.getState(pos);
    this.setState(pos, state ^ GameBoard.FLAG);
  }

  nextExplode(pos, step) {
    this.explode({ y: pos.y, x: pos.x + 1 }, step);
    this.explode({ y: pos.y, x: pos.x - 1 }, step);
    this.explode({ y: pos.y + 1, x: pos.x }, step);
    this.explode({ y: pos.y - 1, x: pos.x }, step);
    if (step % 2 === 0) {
      this.explode({ y: pos.y + 1, x: pos.x + 1 }, step);
      this.explode({ y: pos.y + 1, x: pos.x - 1 }, step);
      this.explode({ y: pos.y - 1, x: pos.x + 1 }, step);
      this.explode({ y: pos.y - 1, x: pos.x - 1 }, step);
    }
  }

  explode(pos, step = 0) {
    if (!this.isInBounds(pos) || this.hasState(pos, GameBoard.EXPLODED)) return;
    let square = this.board[pos.y][pos.x];
    square.el.classList.add('exploding');
    square.state |= GameBoard.EXPLODED;
    if (this.hasState(pos, GameBoard.BOMB)) square.el.classList.add('bomb');
    setTimeout(() => {
      this.nextExplode(pos, step + 1);
    }, 100);
  }

  buildBoard() {
    let b = this.parent;
    b.classList.add('game-board');
    b.style.setProperty('--grid-col', this.size);
    b.style.setProperty('--grid-rows', this.size);
    this.board = [];
    for (let y = 0; y < this.size; y++) {
      this.board[y] = [];
      for (let x = 0; x < this.size; x++) {
        let d = document.createElement('div');
        d.id = `b_${y}_${x}`;
        if (this.click != undefined) {
          d.addEventListener('click', (e) => {
            e.preventDefault();
            this.click({ y: y, x: x });
          });
        }
        if (this.rightClick != undefined) {
          d.addEventListener('contextmenu', (e) => {
            e.preventDefault();
            this.rightClick({ y: y, x: x });
          });
        }
        b.appendChild(d);
        this.board[y].push({ el: d, state: GameBoard.GRASS });
        this.setState({ y: y, x: x }, GameBoard.GRASS);
      }
    }
  }
}
