import GameBoard from './GameBoard.js';
import Modal from './Modal.js';

export default class GameManager {
  constructor(size, numBombs) {
    this.flagsRemaining = numBombs;
    this.size = size;
    this.numBombs = numBombs;
    this.flaggedBombs = 0;
    this.time = { m: 0, s: 0 };
    this.state = 'playing';
    this.modal = null;
    this.gb = new GameBoard(
      'board',
      20,
      (pos) => this.click(pos),
      (pos) => this.rightClick(pos)
    );
    this.placeBombs();
    document.getElementById('num-flags').innerText = this.flagsRemaining;
    document.getElementById('num-bombs').innerText = this.numBombs;
    document.getElementById('size').innerText = `${this.size}x${this.size}`;
    document.getElementById('time').innerText = '00:00';
    setTimeout(() => this.updateTime(), 1000);
  }

  updateTime() {
    if (this.state === 'playing') {
      this.time.s++;
      if (this.time.s > 60) {
        this.time.s = 0;
        this.time.m++;
      }
      document.getElementById('time').innerText = `${this.time.m
        .toString()
        .padStart(2, '0')}:${this.time.s.toString().padStart(2, '0')}`;
    }
    setTimeout(() => this.updateTime(), 1000);
  }

  exitClicked() {
    this.modal.remove();
  }

  newGame() {
    window.location.reload();
  }

  win() {
    this.state = 'ended';
    let tn = document.createTextNode(
      `Congrats! You won in ${this.time.m} minute(s) and ${this.time.s} second(s)`
    );
    var audio = new Audio('./media/sounds/Victory.mp3');
    audio.play();
    this.modal = new Modal(tn);
    this.modal.addButton('Exit', (e) => this.exitClicked(), {
      color: 'red',
      textColor: 'white',
    });
    this.modal.addButton('New Game', (e) => this.newGame(), {
      color: 'blue',
      textColor: 'white',
    });
  }

  lose(pos) {
    this.state = 'ended';
    this.gb.explode(pos);
    var audio = new Audio('./media/sounds/Explosion Powerful.mp3');
    audio.play();
    setTimeout(() => {
      let gif = document.createElement('img');
      gif.src = './media/images/lose.gif';
      this.modal = new Modal(gif);
      this.modal.addButton('Wallow in defeat!', (e) => this.exitClicked(), {
        color: 'red',
        textColor: 'white',
      });
      this.modal.addButton('Walk of Shame', (e) => this.newGame(), {
        color: 'blue',
        textColor: 'white',
      });
    }, 2000);
  }

  rightClick(pos) {
    if (this.gb.isFlagged(pos)) {
      this.gb.toggleFlag(pos);
      if (this.gb.isBomb(pos)) this.flaggedBombs--;
      this.flagsRemaining++;
    } else if (!this.gb.isRevealed(pos)) {
      this.gb.toggleFlag(pos);
      if (this.gb.isBomb(pos)) this.flaggedBombs++;
      this.flagsRemaining--;
      if (this.flagsRemaining === 0 && this.flaggedBombs === this.numBombs)
        this.win();
    }
    document.getElementById('num-flags').innerText = this.flagsRemaining;
  }

  click(pos) {
    //doMove(gb, pos);
    if (this.state != 'ended') {
      if (this.gb.isFlagged(pos)) {
        this.gb.toggleFlag(pos);
        if (this.gb.isBomb(pos)) this.flaggedBombs--;
        this.flagsRemaining++;
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
    this.gb.setState(pos, this.gb.getState(pos) | GameBoard.REVEALED);
    if (this.gb.countBombs(pos) === 0) {
      setTimeout(() => {
        this.doMove({ y: pos.y + 1, x: pos.x });
        this.doMove({ y: pos.y - 1, x: pos.x });
        this.doMove({ y: pos.y, x: pos.x + 1 });
        this.doMove({ y: pos.y, x: pos.x - 1 });
      }, 100);
    }
  }
}
