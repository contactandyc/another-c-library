import MineSweeper from './MineSweeper.js';
import Modal from './Modal.js';
import MusicPlayer from './MusicPlayer.js';

class GameManager {
  constructor() {
    this.musicPlayer = new MusicPlayer();
    document.getElementById('container').style.display = 'none';
    this.modal = null;
    let tn = document.createTextNode(`Wanna play a game?`);
    this.modal = new Modal(tn);
    this.modal.addButton(
      'New Game',
      (e) => {
        this.musicPlayer.addTrack('./media/music/looking_for_adventure.wav');
        this.musicPlayer.addTrack('./media/music/Epic_Sport.wav');
        this.musicPlayer.addTrack('./media/music/Inspiring_Muted_Guitar.mp3');
        this.musicPlayer.play();
        document.getElementById('container').style.display = 'flex';
        this.newGame();
      },
      {
        color: 'blue',
        textColor: 'white',
      }
    );
  }

  exitClicked() {
    this.modal.remove();
  }

  newGame() {
    if (this.modal) {
      this.modal.remove();
    }
    if (this.game != undefined) {
      this.game.removeBoard();
    }
    this.game = new MineSweeper(
      25,
      20,
      (time) => this.win(time),
      () => this.lose()
    );
  }

  win(time) {
    let tn = document.createTextNode(
      `Congrats! You won in ${time.m} minute(s) and ${time.s} second(s)`
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

  lose() {
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
  }
}

let gm = new GameManager();
