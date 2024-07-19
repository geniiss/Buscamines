
const grid = document.getElementById('grid');
const face = document.getElementById('face');
const timer = document.querySelector('.timer');
const mines = document.querySelector('.mines');
const overlay = document.getElementById('overlay');

let gameStarted = false;
let gameOver = false;
let time = 0;
let intervalId;
let totalMines = 10;
let revealedCells = 0;
let flaggedCells = 0;
let mineLocations = [];

// Create grid
function createGrid() {
  for (let i = 0; i < 81; i++) {
    const cell = document.createElement('div');
    cell.classList.add('cell');
    cell.dataset.index = i;
    cell.addEventListener('click', handleClick);
    cell.addEventListener('contextmenu', handleRightClick);
    grid.appendChild(cell);
  }
}

// Start game
function startGame() {
  gameStarted = true;
  gameOver = false;
  time = 0;
  revealedCells = 0;
  flaggedCells = 0;
  mineLocations = [];
  clearInterval(intervalId);
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

  // Place mines randomly
  while (mineLocations.length < totalMines) {
    let randomIndex = Math.floor(Math.random() * 81);
    if (!mineLocations.includes(randomIndex)) {
      mineLocations.push(randomIndex);
    }
  }

  // Start timer
  intervalId = setInterval(() => {
    time++;
    timer.textContent = pad(time);
  }, 1000);
}

// Handle cell click
function handleClick(e) {
  if (gameOver) return;

  const cell = e.target;
  const index = parseInt(cell.dataset.index);

  // Start game on first click
  if (!gameStarted) {
    startGame();
    // Make sure first click is not on a mine
    if (mineLocations.includes(index)) {
      let newIndex = Math.floor(Math.random() * 81);
      while (mineLocations.includes(newIndex)) {
        newIndex = Math.floor(Math.random() * 81);
      }
      mineLocations[mineLocations.indexOf(index)] = newIndex;
    }
  }

  // Reveal cell
  if (cell.classList.contains('revealed') || cell.classList.contains('flagged')) return;

  if (mineLocations.includes(index)) {
    gameOver = true;
    face.classList.remove('happy');
    face.classList.add('sad');
    overlay.classList.remove('hidden');

    // Reveal all mines
    mineLocations.forEach(i => {
      const mineCell = grid.querySelector(`.cell[data-index="${i}"]`);
      mineCell.classList.add('mine');
    });
  } else {
    revealCell(index);
  }
}

// Handle right click
function handleRightClick(e) {
  if (gameOver) return;

  e.preventDefault();
  const cell = e.target;
  if (cell.classList.contains('revealed')) return;

  if (cell.classList.contains('flagged')) {
    cell.classList.remove('flagged');
    flaggedCells--;
    mines.textContent = pad(totalMines - flaggedCells);
  } else {
    cell.classList.add('flagged');
    flaggedCells++;
    mines.textContent = pad(totalMines - flaggedCells);
  }
}

// Reveal a cell and its neighbors
function revealCell(index) {
  const cell = grid.querySelector(`.cell[data-index="${index}"]`);
  if (cell.classList.contains('revealed')) return;

  revealedCells++;
  cell.classList.add('revealed');
  if (mineLocations.includes(index)) {
    gameOver = true;
    face.classList.remove('happy');
    face.classList.add('sad');
    overlay.classList.remove('hidden');
    return;
  }

  // Check if cell is adjacent to a mine
  const adjacentMines = countAdjacentMines(index);
  if (adjacentMines > 0) {
    cell.classList.add('number', `number-${adjacentMines}`);
    cell.textContent = adjacentMines;
  } else {
    // Recursively reveal empty neighbors
    revealNeighbors(index);
  }

  // Check if game is won
  if (revealedCells === 81 - totalMines) {
    gameOver = true;
    face.classList.remove('happy');
    face.classList.add('win');
    overlay.classList.remove('hidden');
    clearInterval(intervalId);
  }
}

// Count adjacent mines
function countAdjacentMines(index) {
  let count = 0;
  const neighbors = getNeighbors(index);
  neighbors.forEach(neighbor => {
    if (mineLocations.includes(neighbor)) {
      count++;
    }
  });
  return count;
}

// Get neighbors of a cell
function getNeighbors(index) {
  const neighbors = [];
  const row = Math.floor(index / 9);
  const col = index % 9;

  // Top row
  if (row > 0) {
    neighbors.push(index - 9);
    if (col > 0) neighbors.push(index - 10);
    if (col < 8) neighbors.push(index - 8);
  }

  // Middle row
  if (col > 0) neighbors.push(index - 1);
  if (col < 8) neighbors.push(index + 1);

  // Bottom row
  if (row < 8) {
    neighbors.push(index + 9);
    if (col > 0) neighbors.push(index + 8);
    if (col < 8) neighbors.push(index + 10);
  }

  return neighbors;
}

// Recursively reveal empty neighbors
function revealNeighbors(index) {
  const neighbors = getNeighbors(index);
  neighbors.forEach(neighbor => {
    const neighborCell = grid.querySelector(`.cell[data-index="${neighbor}"]`);
    if (!neighborCell.classList.contains('revealed') && !neighborCell.classList.contains('flagged')) {
      revealCell(neighbor);
    }
  });
}

// Pad number with zeros
function pad(num) {
  return ('000' + num).slice(-3);
}

// Initialize game
createGrid();
startGame();