import initModule from './game/game.js'

const Module = await initModule();
const Game = Module.Game;
let myGame;

const modeButton = document.getElementById('mode-button');
const grid = document.getElementById('grid');
const face = document.getElementById('face');
const timer = document.querySelector('.timer');
const mines = document.querySelector('.mines');
const menuDiv = document.getElementById('menu');

const difficulties = [
  { difficulty: 'Easy',
    rows: 8,
    cols: 8,
    mines: 10
  },
  { difficulty: 'Medium',
    rows: 16,
    cols: 16,
    mines: 40
  },
  { difficulty: 'Hard',
    rows: 16,
    cols: 30,
    mines: 99
  }
];


const GameStates = {
  READY: 0,
  PLAYING: 1,
  WIN: 2,
  LOSE: 3
};

let gameState;
let rows = 8;
let cols = 8;
let totalMines = 13;
let discoverMode = true;

let time = 0;
let intervalId;

function showDifficulties () {
  const buttonContainer = document.createElement('div');
  buttonContainer.className = 'button-container';

  for (let i = 0; i < difficulties.length; ++i) {
    const d = difficulties[i];
    const button = document.createElement('button');
    button.textContent = d.difficulty;
    button.addEventListener('click', () => {
      rows = d.rows;
      cols = d.cols;
      totalMines = d.mines;
      menuDiv.remove();
      startGame();
      paintGrid();
    })
    button.className = `difficulty-button ${d.difficulty.toLowerCase()}`;

    buttonContainer.appendChild(button);
  };

  menuDiv.appendChild(buttonContainer);
}

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
        if (info === 'e') cell.innerHTML = '🚩';
        else if (info === 'm') cell.innerHTML = '💣';
        else if (info === 'f') cell.innerHTML = '❌';
        else if (info !== 'c' && info !== 'f') {
          cell.classList.add('revealed', 'number', `number-${info}`);
          if (info !== '0') cell.innerHTML = info;
        }
      }
      else {
        if (info === 'f') cell.innerHTML = '🚩';
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
  clearInterval(intervalId);
  myGame = new Game(rows, cols, totalMines, discoverMode);
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

function toggleMode() {
  discoverMode = !discoverMode;
  if (discoverMode) modeButton.innerHTML='⛏️';
  else modeButton.innerHTML='🚩';
  myGame.setGameMode(discoverMode);
}

// Initialize game
showDifficulties()
modeButton.addEventListener('click', toggleMode);
face.addEventListener('click', startGame); 
