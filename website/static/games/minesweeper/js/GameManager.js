import MineSweeper from './MineSweeper.js';
import Modal from './Modal.js';
import MusicPlayer from './MusicPlayer.js';

class GameManager {
  constructor() {
    this.musicVol = 0.5;
    this.soundVol = 0.5;
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
    if (this.modal) {
      this.modal.remove();
    }
    if (this.game === undefined || this.game === null) {
      this.game = new MineSweeper(
        this.size,
        this.numBombs,
        this.soundPlayer,
        () => this.win(),
        () => this.lose()
      );
    } else {
      this.game.removeBoard();
      this.game.newGame(
        this.size,
        this.numBombs,
        this.soundPlayer,
        () => this.win(),
        () => this.lose()
      );
    }
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

  presentHelpModal() {
    let options = document.createElement('div');
    options.id = 'new-game-options';
    let label = document.createElement('h2');
    label.innerText = 'How to Play';
    options.appendChild(label);

    let helpContent = document.createElement('div');
    helpContent.className = 'help-content';

    let hc = `<h3>Clearing Land</h3>`;
    hc += `<p>Clear land by clicking on a tile. If the tile contains a bomb, you lose, otherwise the number of bombs that are adjacent to the tile is revealed on the face of the tile. If there are no adjacent bombs, then the adjacent squares will also be revealed and the process will repeat until a square with adjacent bombs is reachd or the edge of the board.</p>`;
    hc += `<img src="./media/images/help_clearing.jpg"/>`;

    hc += `<h3>Flagging Bombs</h3>`;
    hc += `<p>Mark a suspected bomb location with a flag by right clicking on a tile or by clicking on a tile while in flagging mode.</p>`;
    hc += `<img src="./media/images/help_flagging.png"/>`;
    hc += `<p>Toggle flagging mode by clicking the flag icon in the bottom bar.</p>`;
    hc += `<img src="./media/images/help_flagging_mode.jpg"/>`;

    hc += `<h3>Winning the Game</h3>`;
    hc += `<p>Win the game by either revealing all non-bomb tiles or by flagging all the bombs!</p>`;
    hc += `<img src="./media/images/help_winning.jpg"/>`;

    helpContent.innerHTML = hc;
    options.appendChild(helpContent);
    let modal = new Modal(options);
    modal.addButton(
      'Exit',
      (e) => {
        modal.remove();
      },
      {
        color: '#006416',
        textColor: 'white',
      }
    );
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

    let difLabel = document.createElement('label');
    difLabel.innerText = 'Difficulty';
    options.appendChild(difLabel);
    let difSetting = document.createElement('input');
    difSetting.type = 'range';
    difSetting.id = 'music-volume';
    difSetting.min = 0;
    difSetting.max = 4;
    difSetting.defaultValue = 0;
    difSetting.addEventListener('change', (e) => {
      let difficulties = [[10, 10], [15, 30], [20, 40], [30, 60], [40, 100]];
      let dif = difficulties[e.currentTarget.value];
      sizeInput.value = dif[0];
      bombsInput.value = dif[1];
    });
    options.appendChild(difSetting);

    var sizeInput = document.createElement('input'); // var here so I can use the sizeInput in difSetting
    sizeInput.type = 'text';
    sizeInput.id = 'size-input';
    sizeInput.placeholder = 'Size';
    sizeInput.defaultValue = 10;
    //if (this.size) sizeInput.value = this.size;
    options.appendChild(sizeInput);

    var bombsInput = document.createElement('input'); // var here so I can use the bombsInput in difSetting
    bombsInput.type = 'text';
    bombsInput.id = 'bombs-input';
    bombsInput.placeholder = '# of Bombs';
    bombsInput.defaultValue = 10;
    //if (this.numBombs) bombsInput.value = this.numBombs;
    options.appendChild(bombsInput);

    this.modal = new Modal(options);
    this.modal.addButton('New Game', (e) => this.closeNewGameModal(), {
      color: '#006416',
      textColor: 'white',
    });
    this.modal.addButton('Help', (e) => this.presentHelpModal(), {
      color: 'white',
      textColor: '#006416',
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
