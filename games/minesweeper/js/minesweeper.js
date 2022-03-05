import GameManager from './GameManager.js';
import MusicPlayer from './MusicPlayer.js';

let gm = new GameManager(20, 5);

let mp = new MusicPlayer();
mp.addTrack('./media/music/looking_for_adventure.wav');
mp.addTrack('./media/music/Epic_Sport.wav');
mp.addTrack('./media/music/Inspiring_Muted_Guitar.mp3');
mp.play();
