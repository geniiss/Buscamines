import initModule from './game/game.js'

const Module = await initModule();
const Game = Module.Game;
let myGame;

const grid = document.getElementById('grid');
const face = document.getElementById('face');
const timer = document.querySelector('.timer');
const mines = document.querySelector('.mines');
const overlay = document.getElementById('overlay');
face.addEventListener('click', startGame); 

const GameStates = {
  READY: 0,
  PLAYING: 1,
  WIN: 2,
  LOSE: 3
};

let gameState;
let rows = 16;
let cols = 16;
let totalMines = 50;


let time = 0;
let intervalId;

// paint grid
function paintGrid() {
  mines.textContent = `${pad(myGame.getRemainingMines())}`
  while (grid.lastChild) {
    grid.removeChild(grid.lastChild);
  }
  const data = myGame.printBoard();
  for (let i = 0; i < rows; i++) {
    for (let j = 0; j < cols; j++) {
      const cell = document.createElement('div');
      cell.classList.add('cell');

      const info = data[i*cols + j];
      if (gameState === GameStates.LOSE) {
        if (info === 'e' || info === 'm') cell.classList.add('mine');
        else if (info !== 'c' && info !== 'f') {
          cell.classList.add('revealed', 'number', `number-${info}`);
          if (info !== '0') cell.innerHTML = info;
        }
      }
      else {
        if (info === 'f') cell.classList.add('flagged');
        else if (info !== 'c') {
          cell.classList.add('revealed', 'number', `number-${info}`);
          if (info !== '0') cell.innerHTML = info;
        }
      }
      cell.dataset.i = i;
      cell.dataset.j = j;
      cell.addEventListener('click', handleClick);
      cell.addEventListener('contextmenu', handleRightClick);
      grid.appendChild(cell);
    }
  }
}

// Start game
function startGame() {
  myGame = new Game(rows, cols, totalMines);
  //change grid space
  var r = document.querySelector(':root');
  r.style.setProperty('--rows', rows);
  r.style.setProperty('--cols', cols);

  gameState = myGame.getGameState();
  time = 0;
  timer.textContent = '000';
  face.classList.remove('sad');
  face.classList.remove('win');
  face.classList.add('happy');
  overlay.classList.add('hidden');

  // Reset grid
  const cells = grid.querySelectorAll('.cell');
  cells.forEach(cell => {
    cell.classList.remove('revealed', 'flagged', 'mine', 'number');
    cell.textContent = '';
  });
}

// Handle cell click
function handleClick(e) {
  if (gameState === GameStates.LOSE || gameState === GameStates.WIN) return;

  const cell = e.target;
  const i = parseInt(cell.dataset.i);
  const j = parseInt(cell.dataset.j);

  if (gameState === GameStates.READY) {
      // Start timer
      intervalId = setInterval(() => {
        time++;
        timer.textContent = pad(time);
      }, 1000);
  }

  myGame.clickCell(i, j);
  gameState = myGame.getGameState();
  if (gameState === GameStates.WIN) {
    face.classList.add('win');
    clearInterval(intervalId);
  }
  else if (gameState === GameStates.LOSE) {
    face.classList.add('sad');
    clearInterval(intervalId);
  }
  paintGrid();

}

// Handle right click
function handleRightClick(e) {
  if (gameState === GameStates.LOSE || gameState === GameStates.WIN) return;
  e.preventDefault();
  const cell = e.target;
  const i = parseInt(cell.dataset.i);
  const j = parseInt(cell.dataset.j);

  if (gameState === GameStates.READY) {
    // Start timer
    intervalId = setInterval(() => {
      time++;
      timer.textContent = pad(time);
    }, 1000);
  }

  myGame.rightClickCell(i, j);
  gameState = myGame.getGameState();
  if (gameState === GameStates.WIN) {
    face.classList.add('win');
    clearInterval(intervalId);
  }
  else if (gameState === GameStates.LOSE) {
    face.classList.add('sad');
    clearInterval(intervalId);
  }
  paintGrid();
}

// Pad number with zeros
function pad(num) {
  return ('000' + num).slice(-3);
}

// Initialize game
startGame();
paintGrid();