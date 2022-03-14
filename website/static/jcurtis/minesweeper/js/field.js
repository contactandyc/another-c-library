var fieldManager = (function () {
  var rowCount = 8;
  var columnCount = 8;
  var bombs = 8;
  var bombsCounter = 8;
  var defusedBombs = 0;
  var moves = 0;
  var cells = [];
  var rows = [];
  var firstMove = true;
  var alive = true;
  var placedFlags = 0;

  function initializeField(numRows, numCols, numBombs) {
    rowCount = numRows;
    columnCount = numCols;
    bombs = numBombs;
    bombsCounter = numBombs;
  }

  function clearField() {
    gameContainer.innerHTML = "";
    cells = [];
    rows = [];
    defusedBombs = 0;
    moves = 0;
    firstMove = true;
    alive = true;
    placedFlags = 0;
    resetTitle();
  }

  function generateCells() {
    for (var i = 0; i < rowCount * columnCount; i++) {
      cells.push(new Cell(i));
    }
  }

  function randomizeBombs() {
    var alreadySet = [];
    for (var i = 0; i < bombs; i++) {
      var number = Math.floor(Math.random() * (rowCount * columnCount));
      while (alreadySet.indexOf(number) >= 0) {
        number = Math.floor(Math.random() * (rowCount * columnCount));
      }
      alreadySet.push(number);
      cells[number].setType(0);
      updateNumbers(number);
    }
    console.log(cells);
  }

  function updateNumbers(cellNumber) {
    var leftSide = cellNumber % columnCount === 0;
    var rightSide = (cellNumber + 1) % columnCount === 0;
    var topSide = cellNumber - columnCount < 0;
    var bottomSide = cellNumber + columnCount >= columnCount * rowCount;
    if (!leftSide) {
      increaseBombCount(cellNumber - 1);
      if (!topSide) {
        increaseBombCount(cellNumber - columnCount - 1);
      }
      if (!bottomSide) {
        increaseBombCount(cellNumber + columnCount - 1);
      }
    }
    if (!rightSide) {
      if (!topSide) {
        increaseBombCount(cellNumber - columnCount + 1);
      }
      if (!bottomSide) {
        increaseBombCount(cellNumber + columnCount + 1);
      }
      increaseBombCount(cellNumber + 1);
    }
    if (!bottomSide) {
      increaseBombCount(cellNumber + columnCount);
    }
    if (!topSide) {
      increaseBombCount(cellNumber - columnCount);
    }
  }

  function increaseBombCount(cellNumber) {
    if (cells[cellNumber].cellType === -1) {
      cells[cellNumber].cellType = 1;
    } else if (cells[cellNumber].cellType === 0) {
    } else {
      cells[cellNumber].cellType++;
    }
  }

  function draw() {
    var rowDiv = document.createElement("div");
    rowDiv.classList.add("row");
    rows.push(rowDiv);
    for (var i = 0; i < cells.length; i++) {
      cells[i].draw(rowDiv, clicked, rightClicked);
      if ((i + 1) % columnCount == 0) {
        rowDiv = document.createElement("div");
        rowDiv.classList.add("row");
      }
      gameContainer.appendChild(rowDiv);
    }
  }

  function createField() {
    generateCells();
    randomizeBombs();
    draw(gameContainer);
  }

  function clicked(cell) {
    if (firstMove) {
      startTimer();
      firstMove = false;
    }

    moves++;

    if (cell.cellVisibility || !alive) {
      return;
    }

    if (mode.flagging) {
      toggleFlag(cell);
    } else {
      if (cell.isFlagged()) {
        cell.setFlagged(false);
        cell.update();
      } else {
        cells[cell.index].reveal();
        if (cell.isBomb()) {
          alive = false;
          stopTimer();
          revealAll();
          setRestartBtn();
          message.innerText = "😵YOU LOSE😵";
        }
        if (cell.isEmpty()) {
          var queue = traverseMatrix(cell.index, []);

          for (let i = 1; i < queue.length; i++) {
            setTimeout(function revealDelay() {
              cells[queue[i]].reveal();
            }, i * 5)
          }
        }
      }
    }

    updateMoves();

    if (checkWin() && alive) {
      stopTimer();
      revealAll();
      setRestartBtn();
      message.innerText = "🥳 YOU WON! 🥳";
    }
  }

  function rightClicked(cell) {
    if (!alive) return;

    moves++;

    console.log("right clicked", cell);

    toggleFlag(cell);

    if (checkWin() && alive) {
      stopTimer();
      revealAll();
      setRestartBtn();
      message.innerText = "🥳 YOU WON! 🥳";
    }
  }

  function toggleFlag(cell) {
    if (cell.isFlagged()) {
      cell.setFlagged(false);
      bombsCounter++;
      placedFlags--;

      if (cell.cellType == 0) {
        defusedBombs--;
      }
    } else {
      if (placedFlags >= bombs) return;
      cell.setFlagged(true);
      bombsCounter--;
      placedFlags++;

      if (cell.cellType == 0) {
        defusedBombs++;
      }
    }
    cell.update();
    updateBombs();
    updateMoves();
  }

  function traverseMatrix(index, queue) {
    if (queue.indexOf(index) !== -1) return queue;
    if (cells[index].isNumber()) {
      queue.push(index);

      if (cells[index].isNumber) {
        if (cells[index].isFlagged()) {
          toggleFlag(cells[index]);
        }
      }

      return queue;
    } else if (cells[index].isEmpty()) {
      queue.push(index);
    } else {
      return queue;
    }

    if (cells[index].isFlagged()) {
      toggleFlag(cells[index]);
    }

    var leftSide = index % columnCount === 0;
    var rightSide = (index + 1) % columnCount === 0;
    var topSide = index - columnCount < 0;
    var bottomSide = index + columnCount >= columnCount * rowCount;

    if (!leftSide) {
      queue.concat(traverseMatrix(index - 1, queue));
      if (!topSide) {
        queue.concat(traverseMatrix(index - columnCount - 1, queue));
      }
      if (!bottomSide) {
        queue.concat(traverseMatrix(index + columnCount - 1, queue));
      }
    }
    if (!rightSide) {
      if (!topSide) {
        queue.concat(traverseMatrix(index - columnCount + 1, queue));
      }
      if (!bottomSide) {
        queue.concat(traverseMatrix(index + columnCount + 1, queue));
      }
      queue.concat(traverseMatrix(index + 1, queue));
    }
    if (!bottomSide) {
      queue.concat(traverseMatrix(index + columnCount, queue));
    }
    if (!topSide) {
      queue.concat(traverseMatrix(index - columnCount, queue));
    }
    return queue;
  }

  function checkWin() {
    console.log(bombs);
    console.log(defusedBombs);
    if (defusedBombs == bombs) {
      console.log("won1");
      return true;
    } else {
      return cells.every(function (cell) {
        return cell.cellVisibility || cell.isBomb();
      });
    }
  }

  function revealAll() {
    cells.forEach(function (cell) {
      cell.reveal();
    });
  }

  function getBombs() {
    return bombsCounter;
  }

  function getMoves() {
    return moves;
  }

  function isDead() {
    return alive;
  }

  return {
    initializeField: initializeField,
    clearField: clearField,
    createField: createField,
    getBombs: getBombs,
    getMoves: getMoves,
    isDead: isDead,
  };
})();
