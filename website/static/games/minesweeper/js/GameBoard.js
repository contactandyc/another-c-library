//Emitted Events
// - ready
// - squareclick
// - squarealtclick
export default class GameBoard extends HTMLElement {
  static BOMB = 1;
  static FLAG = 2;
  static HIDDEN = 4;
  static REVEALED = 8;
  static EXPLODED = 16;

  constructor() {
    super();
    this.shadow = this.attachShadow({ mode: 'open' });
    let styleLink = document.createElement('link');
    styleLink.setAttribute('rel', 'stylesheet');
    styleLink.setAttribute('href', './css/game_board.css');
    this.shadow.appendChild(styleLink);
    this.contentBox = document.createElement('div');
    this.contentBox.className = 'board-content';
    this.shadow.appendChild(this.contentBox);
    this.boardEl = null;
    this.board = null;
    this.pressTimer = null;
    this.altClick = false;
  }

  static observedAttributes = ['size'];

  attributeChangedCallback(name, oldValue, newValue) {
    this.size = Number(newValue);
  }

  connectedCallback() {
    this._buildBoard();
    const event = new CustomEvent('ready');
    this.dispatchEvent(event);
  }

  addClickAndLongClickListener(el, clickHandler, longClickHandler) {
    this.longClick = false;
    el.addEventListener('mousedown', (e) => {
      this.pressTimer = setTimeout(() => {
        this.altClick = true;
        longClickHandler(e);
      }, 300);
    });
    el.addEventListener('mouseup', (e) => {
      clearTimeout(this.pressTimer);
      if (this.altClick === false) clickHandler(e);
      this.pressTimer = null;
      this.altClick = false;
    });
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
    if (this.hasState(pos, GameBoard.BOMB)) {
      //Could do a check here to see if the bomb should be auto-flagged
      return 1;
    }
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

  _showNumber(pos, num) {
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
    this._showNumber(pos, 0);
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
      } else this._showNumber(pos, this.countBombs(pos));
    }
    if (this.compareState(state, GameBoard.FLAG)) {
      square.el.classList.add('flag');
    }
    square.state = state;
  }

  _updateAdjacent(pos) {
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
    this._updateAdjacent(pos);
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

  _nextExplode(pos, step) {
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
      this._nextExplode(pos, step + 1);
    }, 100);
  }

  _buildBoard() {
    this.boardEl = document.createElement('div');
    this.boardEl.classList.add('game-board');
    this.boardEl.style.setProperty('--grid-col', this.size);
    this.boardEl.style.setProperty('--grid-rows', this.size);
    this.board = [];
    for (let y = 0; y < this.size; y++) {
      this.board[y] = [];
      for (let x = 0; x < this.size; x++) {
        let d = document.createElement('div');
        d.id = `b_${y}_${x}`;
        d.style.setProperty('--square-font-size', `${20 / this.size}em`);
        this.addClickAndLongClickListener(
          d,
          (e) => {
            e.preventDefault();
            const event = new CustomEvent('squareclick', {
              detail: { pos: { y: y, x: x } },
            });
            this.dispatchEvent(event);
          },
          (e) => {
            e.preventDefault();
            const event = new CustomEvent('squarealtclick', {
              detail: { pos: { y: y, x: x } },
            });
            this.dispatchEvent(event);
          }
        );
        d.addEventListener('contextmenu', (e) => {
          e.preventDefault();
          this.altClick = true;
          const event = new CustomEvent('squarealtclick', {
            detail: { pos: { y: y, x: x } },
          });
          this.dispatchEvent(event);
        });
        this.boardEl.appendChild(d);
        this.board[y].push({ el: d, state: GameBoard.HIDDEN });
        this.setState({ y: y, x: x }, GameBoard.HIDDEN);
      }
    }
    this.contentBox.appendChild(this.boardEl);
  }

  removeBoard() {
    this.contentBox.removeChild(this.boardEl);
    this.boardEl = null;
  }

  resetBoard() {
    this.removeBoard();
    this._buildBoard();
  }
}

customElements.define('game-board', GameBoard);
