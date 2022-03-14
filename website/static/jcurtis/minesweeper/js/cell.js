function Cell(index) {
  this.index = index;
  this.cellType = -1;
  this.cellVisibility = false;
  this.flagged = false;
}

Cell.prototype.setType = function (cellType) {
  this.cellType = cellType;
};

Cell.prototype.setVisibility = function (cellVisibility) {
  this.cellVisibility = cellVisibility;
};

Cell.prototype.setFlagged = function (isFlagged) {
  this.flagged = isFlagged;
};

Cell.prototype.draw = function (rowDiv, clicked, rightClicked) {
  var that = this;
  if (this.div) {
    this.update();
    return;
  }
  this.div = document.createElement("div");
  this.div.className = "cell";
  this.update();
  this.div.addEventListener(
    "click",
    function () {
      clicked(that);
    },
    false
  );
  this.div.addEventListener("contextmenu", function (e) {
    rightClicked(that);
    e.preventDefault();
  });
  rowDiv.appendChild(this.div);
};

Cell.prototype.update = function () {
  if (this.flagged) {
    this.div.innerHTML = "🏴";
  } else {
    if (this.cellVisibility) {
      if (this.cellType < 0) {
        this.div.classList.add("empty");

        if (fieldManager.isDead()) {
          this.div.innerText = "🟩";
        } else {
          this.div.innerText = "🟨";
        }
      }
      if (this.cellType == 0) {
        this.div.classList.add("bomb");

        if (fieldManager.isDead()) {
          this.div.innerText = "💣";
        } else {
          this.div.innerText = "💥";
        }
      }
      if (this.cellType > 0) {
        this.div.classList.add("number");

        if (fieldManager.isDead()) {
          switch (this.cellType) {
            case 1:
              this.div.innerText = "1️⃣";
              break;
            case 2:
              this.div.innerText = "2️⃣";
              break;
            case 3:
              this.div.innerText = "3️⃣";
              break;
            case 4:
              this.div.innerText = "4️⃣";
              break;
            case 5:
              this.div.innerText = "5️⃣";
              break;
            case 6:
              this.div.innerText = "6️⃣";
              break;
            case 7:
              this.div.innerText = "7️⃣";
              break;
            case 8:
              this.div.innerText = "8️⃣";
              break;
          }
        } else {
          this.div.innerText = "🔥";
        }
      }
    } else {
      this.div.innerText = "⬜";
    }
  }
};

Cell.prototype.isBomb = function () {
  return this.cellType == 0;
};

Cell.prototype.isEmpty = function () {
  return this.cellType == -1;
};

Cell.prototype.isFlagged = function () {
  return this.flagged;
};

Cell.prototype.isNumber = function () {
  return this.cellType > 0;
};

Cell.prototype.reveal = function () {
  this.setVisibility(true);
  this.update();
};
