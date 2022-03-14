import GameBoard from './GameBoard.js';

export default class MineSweeper {
  constructor(size, numBombs, soundPlayer, winCb, loseCb) {
    this.flagsRemaining = numBombs;
    this.size = size;
    this.numBombs = numBombs;
    this.flaggedBombs = 0;

    this.state = 'playing';
    this.modal = null;
    this.winCb = winCb;
    this.loseCb = loseCb;
    this.soundPlayer = soundPlayer;
    this.flagging = false;
    this.revealedSquares = 0;
    this.gb = document.createElement('game-board');
    this.gb.setAttribute('size', this.size);
    this.gb.className = 'game-board-el';
    this.gb.id = 'thegame';
    this.gb.addEventListener('ready', () => {
      this.placeBombs();
    });
    this.gb.addEventListener('squareclick', (e) => {
      this.click(e.detail.pos);
    });
    this.gb.addEventListener('squarealtclick', (e) => {
      this.altClick(e.detail.pos);
    });
    document.getElementById('board').appendChild(this.gb);
    document.getElementById('num-flags').innerText = this.flagsRemaining;

    document.getElementById('flag-button').addEventListener('click', (e) => {
      e.target.classList.toggle('grayed-out');
      this.flagging = this.flagging ? false : true;
    });

    document.addEventListener('keyup', (e) => {
      if (e.code === 'ShiftLeft') {
        this.flagging = this.flagging ? false : true;
        document.getElementById('flag-button').classList.toggle('grayed-out');
      }
    });
    // document.getElementById('num-bombs').innerText = this.numBombs;
    // document.getElementById('size').innerText = `${this.size}x${this.size}`;
  }

  win() {
    this.state = 'ended';
    this.winCb();
  }

  lose(pos) {
    this.state = 'ended';
    this.gb.explode(pos);
    this.soundPlayer.src = './media/sounds/Explosion Powerful.mp3';
    this.soundPlayer.load();
    this.soundPlayer.play();
    setTimeout(() => {
      this.loseCb(this.time);
    }, 2000);
  }

  altClick(pos) {
    if (this.state != 'ended') {
      if (this.gb.isFlagged(pos)) {
        this.gb.toggleFlag(pos);
        if (this.gb.isBomb(pos)) this.flaggedBombs--;
        this.flagsRemaining++;
      } else if (!this.gb.isRevealed(pos)) {
        if (this.flagsRemaining > 0) {
          this.gb.toggleFlag(pos);
          if (this.gb.isBomb(pos)) this.flaggedBombs++;
          this.flagsRemaining--;
          if (this.flagsRemaining === 0 && this.flaggedBombs === this.numBombs)
            this.win();
        }
      }
      document.getElementById('num-flags').innerText = this.flagsRemaining;
    }
  }

  click(pos) {
    //doMove(gb, pos);
    if (this.state != 'ended') {
      if (this.gb.isFlagged(pos) || this.flagging) {
        this.altClick(pos);
      } else if (this.gb.isBomb(pos)) {
        this.lose(pos);
      } else {
        this.doMove(pos);
      }
    }
  }

  placeBombs() {
    let size = this.gb.size;
    for (let i = 0; i < this.numBombs; i++) {
      while (true) {
        let x = Math.floor(Math.random() * this.size);
        let y = Math.floor(Math.random() * this.size);
        if (this.gb.isBomb({ y: y, x: x }) === 1) continue;
        this.gb.addBomb({ y: y, x: x });
        break;
      }
    }
  }

  doMove(pos) {
    if (
      !this.gb.isInBounds(pos) ||
      this.gb.isRevealed(pos) ||
      this.gb.isExploded(pos)
    )
      return;
    if (this.gb.isFlagged(pos)) {
      this.altClick(pos);
    }
    this.gb.setState(pos, this.gb.getState(pos) | GameBoard.REVEALED);
    this.revealedSquares++;
    if (this.revealedSquares === this.size * this.size - this.numBombs)
      this.win();
    if (this.gb.countBombs(pos) === 0) {
      setTimeout(() => {
        this.doMove({ y: pos.y + 1, x: pos.x });
        this.doMove({ y: pos.y - 1, x: pos.x });
        this.doMove({ y: pos.y, x: pos.x + 1 });
        this.doMove({ y: pos.y, x: pos.x - 1 });
      }, 100);
    }
  }

  removeBoard() {
    document.getElementById('board').removeChild(this.gb);
  }
}
