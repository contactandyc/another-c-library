export default class MusicPlayer extends Audio {
  constructor() {
    super();
    this.playlist = [];
    this.currentTrack = 0;
    this.addEventListener('ended', () => {
      this.startNextTrack();
    });
  }

  startNextTrack() {
    if (this.playlist.length > 0) {
      let numTracks = this.playlist.length;
      this.currentTrack++;
      if (this.currentTrack >= numTracks) this.currentTrack = 0;
      this.src = this.playlist[this.currentTrack];
      this.load();
      this.play();
    }
  }

  play() {
    if (this.src === '')
      if (this.playlist[0] != undefined) {
        this.src = this.playlist[0];
        this.load();
      } else return;
    super.play();
  }

  addTrack(src) {
    this.playlist.push(src);
  }
}
