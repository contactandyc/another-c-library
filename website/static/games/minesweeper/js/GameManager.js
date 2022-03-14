import MineSweeper from './MineSweeper.js';
import Modal from './Modal.js';
import MusicPlayer from './MusicPlayer.js';

class GameManager {
  constructor() {
    this.musicVol = 0.5;
    this.soundVol = 1;
    this.muted = true;
    this.musicPlayer = new MusicPlayer();
    this.musicPlayer.volume = this.musicVol;
    this.musicPlayer.muted = true;
    this.soundPlayer = new Audio();
    this.soundPlayer.volume = this.soundVol;
    this.soundPlayer.muted = true;
    this.modal = null;

    this.time = { m: 0, s: 0 };
    document.getElementById('time').innerText = '00:00';
    this.presentNewGameModal();

    document
      .getElementById('menu-button')
      .addEventListener('click', () => this.presentMenuModal());
    this.begin();

    document.getElementById('mute-button').addEventListener('click', (e) => {
      e.target.classList.toggle('mute-button-sound');
      e.target.classList.toggle('mute-button-muted');
      this.muted = !this.muted;
      this.musicPlayer.muted = this.muted;
      this.soundPlayer.muted = this.muted;
    });
  }

  addClickAndLongClickListener(el, clickHandler, longClickHandler) {
    this.longClick = false;
    el.addEventListener('onmousedown', (e) => {
      this.pressTimer = setTimeout(() => {
        this.longClick = true;
        longClickHandler(e);
      }, 500);
    });
    el.addEventListener('onmouseup', (e) => {
      clearTimeout(this.pressTimer);
      if (this.longClick === false) clickHandler(e);
      this.pressTimer = null;
      this.longClick = false;
    });
  }

  updateTime() {
    if (this.game.state === 'playing') {
      this.time.s++;
      if (this.time.s >= 60) {
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
      this.soundPlayer,
      () => this.win(),
      () => this.lose()
    );
    //document.getElementById('flag-select').checked = false;
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
    bombsInput.defaultValue = 40;
    if (this.numBombs) bombsInput.value = this.numBombs;
    options.appendChild(bombsInput);

    this.modal = new Modal(options);
    this.modal.addButton('New Game', (e) => this.closeNewGameModal(), {
      color: '#006416',
      textColor: 'white',
    });
  }

  presentMenuModal() {
    if (this.modal) {
      this.modal.remove();
    }
    let label = document.createElement('h2');
    label.innerText = 'Main Menu';

    this.modal = new Modal(label);
    this.modal.addButton('New Game', (e) => this.presentNewGameModal(), {
      color: '#006416',
      textColor: 'white',
    });
    this.modal.addButton('Settings', (e) => this.presentSettingsModal(), {
      color: '#006416',
      textColor: 'white',
    });
    this.modal.addButton('Exit', (e) => this.exitClicked(), {
      color: 'white',
      textColor: '#006416',
    });
  }

  exitSettings() {
    this.musicPlayer.volume = this.musicVol;
    this.soundPlayer.volume = this.soundVol;
    this.modal.remove();
  }

  saveSettings() {
    this.musicVol = document.getElementById('music-volume').value / 100;
    this.soundVol = document.getElementById('sound-volume').value / 100;
    this.exitSettings();
  }

  presentSettingsModal() {
    if (this.modal) {
      this.modal.remove();
    }
    let options = document.createElement('div');
    options.id = 'new-game-options';
    let label = document.createElement('h2');
    label.innerText = 'Settings';
    options.appendChild(label);

    let musicLabel = document.createElement('label');
    musicLabel.innerText = 'Music Volume';
    options.appendChild(musicLabel);
    let musicVolume = document.createElement('input');
    musicVolume.type = 'range';
    musicVolume.id = 'music-volume';
    musicVolume.min = 0;
    musicVolume.max = 100;
    musicVolume.value = this.musicVol * 100;
    musicVolume.addEventListener('change', (e) => {
      this.musicPlayer.volume = e.currentTarget.value / 100;
    });
    options.appendChild(musicVolume);

    let soundsLabel = document.createElement('label');
    soundsLabel.innerText = 'Sounds Volume';
    options.appendChild(soundsLabel);
    let soundsVolume = document.createElement('input');
    soundsVolume.type = 'range';
    soundsVolume.id = 'sound-volume';
    soundsVolume.min = 0;
    soundsVolume.max = 100;
    soundsVolume.value = this.soundVol * 100;
    soundsVolume.addEventListener('change', (e) => {
      this.soundPlayer.volume = e.currentTarget.value / 100;
      this.soundPlayer.src = './media/sounds/Victory.mp3';
      this.soundPlayer.load();
      this.soundPlayer.play();
    });
    options.appendChild(soundsVolume);

    this.modal = new Modal(options);
    this.modal.addButton('Save', (e) => this.saveSettings(), {
      color: '#006416',
      textColor: 'white',
    });
    this.modal.addButton('Exit', (e) => this.exitSettings(), {
      color: 'white',
      textColor: '#006416',
    });
  }

  win() {
    let tn = document.createTextNode(
      `Congrats! You won in ${this.time.m} minute(s) and ${this.time.s} second(s)`
    );
    this.soundPlayer.src = './media/sounds/Victory.mp3';
    this.soundPlayer.load();
    this.soundPlayer.play();
    this.modal = new Modal(tn);
    this.modal.addButton('New Game', (e) => this.presentNewGameModal(), {
      color: '#006416',
      textColor: 'white',
    });
    this.modal.addButton('Exit', (e) => this.exitClicked(), {
      color: 'white',
      textColor: '#006416',
    });
  }

  lose() {
    let gif = document.createElement('img');
    gif.src = './media/images/lose.gif';
    this.modal = new Modal(gif);
    this.modal.addButton('Wallow in defeat!', (e) => this.exitClicked(), {
      color: '#006416',
      textColor: 'white',
    });
    this.modal.addButton('New Game', (e) => this.presentNewGameModal(), {
      color: 'white',
      textColor: '#006416',
    });
  }
}

let gm = new GameManager();
