export default class MusicPlayer {
  constructor() {
    this.playlist = [];
    this.currentTrack = 0;
    this.player = new Audio();
    this.player.addEventListener('ended', () => {
      this.startNextTrack();
    });
  }

  loadSrc(src) {
    this.player.src = src;
    this.player.load();
  }

  startNextTrack() {
    if (this.playlist.length > 0) {
      let numTracks = this.playlist.length;
      this.currentTrack++;
      if (this.currentTrack >= numTracks) this.currentTrack = 0;
      this.player.src = this.playlist[this.currentTrack];
      this.player.load();
      this.play();
    }
  }

  play() {
    if (this.player.src === '')
      if (this.playlist[0] != undefined) this.loadSrc(this.playlist[0]);
      else return;
    this.player.play();
  }

  pause() {
    this.player.pause();
  }

  addTrack(src) {
    this.playlist.push(src);
  }
}
