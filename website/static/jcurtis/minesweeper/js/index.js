var gameContainer = document.querySelector("#game");
var body = document.getElementsByTagName("body");

var date = 0;
var alive = true;

var rows = 10;
var cols = 10;
var bombs = 5;

function start(rows, columns, bombs) {
  fieldManager.clearField();
  fieldManager.initializeField(rows, columns, bombs);
  fieldManager.createField();
  date = new Date();
  updateBombs();
}

function updateTime() {}
