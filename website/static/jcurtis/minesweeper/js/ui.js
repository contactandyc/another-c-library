var bottomBar = document.querySelector("#bottom-bar");
var gameElement = document.querySelector("#game-page");
var settingsPage = document.querySelector("#settings-page");
var bombCounter = document.querySelector("#bomb-counter");
var movesCounter = document.querySelector("#moves-counter");
var message = document.querySelector("#status");
var settingsForm = document.querySelector("#settings");
var helpPage = document.querySelector("#help-page");

var mode = {
  div: document.querySelector("#mode-div"),
  icon: document.querySelector("#mode-icon"),
  text: document.querySelector("#mode-text"),
  flagging: false,
  restartMode: false,
};

var timerInt;

var timer = {
  text: document.querySelector("#timer"),
  startTime: null,
  interval: null,
};

var settingsPageEnabled = false;

function toggleSettingsPage() {
  if (!settingsPageEnabled) {
    openSettingsPage();
  } else {
    closeSettingsPage();
  }
}

function closeSettingsPage() {
  bottomBar.style.display = "flex";
  gameElement.style.display = "flex";
  settingsPage.style.display = "none";

  settingsPageEnabled = false;
}

function openSettingsPage() {
  closeHelpPage();

  bottomBar.style.display = "none";
  gameElement.style.display = "none";
  settingsPage.style.display = "flex";

  settingsPageEnabled = true;
}

var helpPageEnabled = false;

function toggleHelpPage() {
  if (!helpPageEnabled) {
    openHelpPage();
  } else {
    closeHelpPage();
  }
}

function closeHelpPage() {
  bottomBar.style.display = "flex";
  gameElement.style.display = "flex";
  helpPage.style.display = "none";

  helpPageEnabled = false;
}

function openHelpPage() {
  closeSettingsPage();

  bottomBar.style.display = "none";
  gameElement.style.display = "none";
  helpPage.style.display = "flex";

  helpPageEnabled = true;
}

function toggleModes() {
  if (mode.flagging) {
    mode.icon.innerHTML = "⛏️";
    mode.text.innerHTML = "MINE";

    mode.flagging = false;
  } else {
    mode.icon.innerHTML = "🏴";
    mode.text.innerHTML = "FLAG";

    mode.flagging = true;
  }
}

function updateBombs() {
  bombCounter.textContent = fieldManager.getBombs();
}

function updateMoves() {
  movesCounter.textContent = fieldManager.getMoves();
}

function startTimer() {
  timer.startTime = new Date();

  timerInt = setInterval(function () {
    timer.text.textContent = ((new Date() - timer.startTime) / 1000).toFixed(2);
  }, 100);
}

function stopTimer() {
  console.log("test");
  var seconds = ((new Date() - timer.startTime) / 1000).toFixed(2);
  clearInterval(timerInt);
  console.log(seconds);
  timer.text.textContent = seconds;
}

function resetTitle() {
  message.textContent = "MINESWEEPER";
}

function setRestartBtn() {
  console.log("restart button active");
  mode.icon.textContent = "♻️";
  mode.text.textContent = "RETRY";

  mode.restartMode = true;
}

function multiBtn() {
  if (mode.restartMode) {
    resetGame();
  } else {
    toggleModes();
  }
}

function resetGame() {
  start(
    Number(settingsForm.rows.value),
    Number(settingsForm.columns.value),
    Number(settingsForm.bombs.value)
  );

  mode.icon.innerHTML = "⛏️";
  mode.text.innerHTML = "MINE";

  mode.flagging = false;

  mode.restartMode = false;
  resetTimer();

  movesCounter.textContent = "0";
}

function resetTimer() {
  timer.text.textContent = "0.00";
  timer.startTime = null;
  timer.interval = null;
}
