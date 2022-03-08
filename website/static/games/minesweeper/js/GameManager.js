import MineSweeper from './MineSweeper.js';
import Modal from './Modal.js';
import MusicPlayer from './MusicPlayer.js';

class GameManager {
  constructor() {
    this.musicPlayer = new MusicPlayer();
    document.getElementById('container').style.display = 'none';
    this.modal = null;

    this.time = { m: 0, s: 0 };
    document.getElementById('time').innerText = '00:00';
    this.presentNewGameModal();
    document
      .getElementById('reset-button')
      .addEventListener('click', () => this.presentNewGameModal());
    document.getElementById('settings-button');
    this.begin();
  }

  updateTime() {
    if (this.game.state === 'playing') {
      this.time.s++;
      if (this.time.s > 60) {
        this.time.s = 0;
        this.time.m++;
      }
      document.getElementById(
        'time'
      ).innerText = `${this.time.m
        .toString()
        .padStart(2, '0')}:${this.time.s.toString().padStart(2, '0')}`;
    }
  }

  stopTimer() {
    if (this.timer) {
      clearInterval(this.timer);
      this.timer = null;
    }
  }

  startTimer() {
    this.timer = setInterval(() => this.updateTime(), 1000);
  }

  //disambiguated from newGame this happens only once per page load when you press the start button.
  begin() {
    this.setupMusicPlaylist();
    document.getElementById('container').style.display = 'flex';
  }

  setupMusicPlaylist() {
    this.musicPlayer.addTrack('./media/music/looking_for_adventure.wav');
    this.musicPlayer.addTrack('./media/music/Epic_Sport.wav');
    this.musicPlayer.addTrack('./media/music/Inspiring_Muted_Guitar.mp3');
  }

  exitClicked() {
    this.modal.remove();
  }

  newGame() {
    // console.log('size', this.size);
    // console.log('num bombs', this.numBombs);
    if (this.modal) {
      this.modal.remove();
    }
    if (this.game != undefined) {
      this.game.removeBoard();
    }
    delete this.game;
    this.game = new MineSweeper(
      this.size,
      this.numBombs,
      () => this.win(),
      () => this.lose()
    );
    this.stopTimer();
    this.time = { m: 0, s: 0 };
    this.startTimer();
  }

  closeNewGameModal() {
    this.numBombs = Number(document.getElementById('bombs-input').value);
    this.size = Number(document.getElementById('size-input').value);
    this.musicPlayer.play();
    this.newGame();
  }

  presentNewGameModal() {
    if (this.modal) {
      this.modal.remove();
    }
    let options = document.createElement('div');
    options.id = 'new-game-options';
    let label = document.createElement('h2');
    label.innerText = "Let's play a game...";
    options.appendChild(label);

    let sizeInput = document.createElement('input');
    sizeInput.type = 'text';
    sizeInput.id = 'size-input';
    sizeInput.placeholder = 'Size';
    sizeInput.defaultValue = 20;
    if (this.size) sizeInput.value = this.size;
    options.appendChild(sizeInput);

    let bombsInput = document.createElement('input');
    bombsInput.type = 'text';
    bombsInput.id = 'bombs-input';
    bombsInput.placeholder = '# of Bombs';
    sizeInput.defaultValue = 10;
    if (this.numBombs) bombsInput.value = this.numBombs;
    options.appendChild(bombsInput);

    this.modal = new Modal(options);
    this.modal.addButton('New Game', (e) => this.closeNewGameModal(), {
      color: 'blue',
      textColor: 'white',
    });
  }

  win() {
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
    this.modal.addButton('New Game', (e) => this.presentNewGameModal(), {
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
    this.modal.addButton('Walk of Shame', (e) => this.presentNewGameModal(), {
      color: 'blue',
      textColor: 'white',
    });
  }
}

let gm = new GameManager();
