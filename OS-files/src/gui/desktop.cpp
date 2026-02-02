#include <gui/desktop.h>

using namespace osfiles::common;
using namespace osfiles::drivers;
using namespace osfiles::gui;

// Simple string compare
static bool strequal(const char *a, const char *b) {
  while (*a && *b) {
    if (*a != *b)
      return false;
    a++;
    b++;
  }
  return *a == *b;
}

// Simple string length
static int strlen(const char *s) {
  int len = 0;
  while (s[len])
    len++;
  return len;
}

// Desktop implementation
Desktop::Desktop(VideoGraphicsArray *vga, MouseCursor *cursor) {
  this->vga = vga;
  this->cursor = cursor;
  this->backgroundColor = VGA_DESKTOP_BG;
  this->width = vga->GetWidth();
  this->height = vga->GetHeight();
  this->iconCount = 0;
  this->mouseX = width / 2;
  this->mouseY = height / 2;
  this->mouseButtons = 0;
  this->activeTerminal = 0;
  this->activeTicTacToe = 0;
  this->draggingWindow = false;
  this->draggingIcon = false;
  this->draggingIconIndex = -1;
  this->dragOffsetX = 0;
  this->dragOffsetY = 0;

  for (int i = 0; i < MAX_DESKTOP_ICONS; i++)
    icons[i] = 0;
}

Desktop::~Desktop() {}

void Desktop::SetBackgroundColor(uint8_t color) { backgroundColor = color; }

void Desktop::AddIcon(DesktopIcon *icon) {
  if (iconCount < MAX_DESKTOP_ICONS) {
    icons[iconCount++] = icon;
  }
}

void Desktop::SetActiveTerminal(TerminalWindow *terminal) {
  activeTerminal = terminal;
}

void Desktop::SetActiveTicTacToe(TicTacToeWindow *ttt) {
  activeTicTacToe = ttt;
}

void Desktop::Draw() {
  vga->Clear(backgroundColor);

  for (int i = 0; i < iconCount; i++) {
    if (icons[i])
      icons[i]->Draw(vga);
  }

  cursor->Draw(vga);
  vga->SwapBuffers();
}

void Desktop::Redraw() { Draw(); }

void Desktop::OnMouseDown(uint8_t button) {
  mouseButtons |= (1 << (button - 1));

  // Check TicTacToe window first
  if (activeTicTacToe && activeTicTacToe->IsVisible()) {
    if (activeTicTacToe->ContainsCloseButton(mouseX, mouseY)) {
      activeTicTacToe->SetVisible(false);
      activeTicTacToe->SetFocused(false);
      return;
    }
    if (activeTicTacToe->ContainsTitleBar(mouseX, mouseY)) {
      draggingWindow = true;
      int32_t wx, wy;
      activeTicTacToe->GetPosition(&wx, &wy);
      dragOffsetX = mouseX - wx;
      dragOffsetY = mouseY - wy;
      activeTicTacToe->SetFocused(true);
      if (activeTerminal)
        activeTerminal->SetFocused(false);
      return;
    }
    if (activeTicTacToe->Contains(mouseX, mouseY)) {
      activeTicTacToe->SetFocused(true);
      if (activeTerminal)
        activeTerminal->SetFocused(false);
      activeTicTacToe->OnMouseDown(mouseX, mouseY);
      return;
    }
  }

  // Check terminal window
  if (activeTerminal && activeTerminal->IsVisible()) {
    if (activeTerminal->ContainsCloseButton(mouseX, mouseY)) {
      activeTerminal->SetVisible(false);
      activeTerminal->SetFocused(false);
      return;
    }
    if (activeTerminal->ContainsTitleBar(mouseX, mouseY)) {
      draggingWindow = true;
      int32_t wx, wy;
      activeTerminal->GetPosition(&wx, &wy);
      dragOffsetX = mouseX - wx;
      dragOffsetY = mouseY - wy;
      activeTerminal->SetFocused(true);
      if (activeTicTacToe)
        activeTicTacToe->SetFocused(false);
      return;
    }
    if (activeTerminal->Contains(mouseX, mouseY)) {
      activeTerminal->SetFocused(true);
      if (activeTicTacToe)
        activeTicTacToe->SetFocused(false);
      return;
    }
  }

  // icons dragging
  for (int i = 0; i < iconCount; i++) {
    if (icons[i] && icons[i]->Contains(mouseX, mouseY)) {
      draggingIcon = true;
      draggingIconIndex = i;
      int32_t ix, iy;
      icons[i]->GetPosition(&ix, &iy);
      dragOffsetX = mouseX - ix;
      dragOffsetY = mouseY - iy;
      icons[i]->OnMouseDown(mouseX, mouseY, button);
      return;
    }
  }
}

void Desktop::OnMouseUp(uint8_t button) {
  mouseButtons &= ~(1 << (button - 1));
  draggingWindow = false;
  draggingIcon = false;
  draggingIconIndex = -1;
}

void Desktop::OnMouseMove(int32_t oldX, int32_t oldY, int32_t newX,
                          int32_t newY) {
  mouseX = newX;
  mouseY = newY;
  cursor->SetPosition(newX, newY);

  // window dragging for terminal
  if (draggingWindow && activeTerminal && activeTerminal->IsFocused()) {
    int32_t newWinX = mouseX - dragOffsetX;
    int32_t newWinY = mouseY - dragOffsetY;
    if (newWinX < 0)
      newWinX = 0;
    if (newWinY < 0)
      newWinY = 0;
    if (newWinX > width - 50)
      newWinX = width - 50;
    if (newWinY > height - 20)
      newWinY = height - 20;
    activeTerminal->SetPosition(newWinX, newWinY);
  }

  // window dragging for TicTacToe
  if (draggingWindow && activeTicTacToe && activeTicTacToe->IsFocused()) {
    int32_t newWinX = mouseX - dragOffsetX;
    int32_t newWinY = mouseY - dragOffsetY;
    if (newWinX < 0)
      newWinX = 0;
    if (newWinY < 0)
      newWinY = 0;
    if (newWinX > width - 50)
      newWinX = width - 50;
    if (newWinY > height - 20)
      newWinY = height - 20;
    activeTicTacToe->SetPosition(newWinX, newWinY);
  }

  // icon dragging
  if (draggingIcon && draggingIconIndex >= 0 && icons[draggingIconIndex]) {
    int32_t newIconX = mouseX - dragOffsetX;
    int32_t newIconY = mouseY - dragOffsetY;
    if (newIconX < 0)
      newIconX = 0;
    if (newIconY < 0)
      newIconY = 0;
    if (newIconX > width - 48)
      newIconX = width - 48;
    if (newIconY > height - 48)
      newIconY = height - 48;
    icons[draggingIconIndex]->SetPosition(newIconX, newIconY);
  }
}

void Desktop::OnKeyDown(char c) {
  if (activeTerminal && activeTerminal->IsVisible() &&
      activeTerminal->IsFocused()) {
    activeTerminal->OnKeyDown(c);
  }
}

int32_t Desktop::GetMouseX() { return mouseX; }
int32_t Desktop::GetMouseY() { return mouseY; }
VideoGraphicsArray *Desktop::GetVGA() { return vga; }
MouseCursor *Desktop::GetCursor() { return cursor; }

// TerminalWindow
TerminalWindow::TerminalWindow(int32_t x, int32_t y, int32_t w, int32_t h,
                               const char *title) {
  this->x = x;
  this->y = y;
  this->width = w;
  this->height = h;
  this->title = title;
  this->titleBarHeight = 14;
  this->bgColor = VGA_TERMINAL_BG;
  this->fgColor = VGA_TERMINAL_FG;
  this->titleBarColor = VGA_TITLEBAR;
  this->outputLineCount = 0;
  this->inputPos = 0;
  this->visible = false;
  this->focused = false;

  for (int i = 0; i < TERMINAL_MAX_LINES; i++)
    for (int j = 0; j < TERMINAL_LINE_WIDTH; j++)
      outputLines[i][j] = 0;
  for (int i = 0; i < 64; i++)
    inputBuffer[i] = 0;

  AddOutputLine("ShitOS Terminal v1.0");
  AddOutputLine("Type 'help' for cmds");
}

TerminalWindow::~TerminalWindow() {}

void TerminalWindow::AddOutputLine(const char *text) {
  if (outputLineCount >= TERMINAL_MAX_LINES) {
    for (int i = 0; i < TERMINAL_MAX_LINES - 1; i++)
      for (int j = 0; j < TERMINAL_LINE_WIDTH; j++)
        outputLines[i][j] = outputLines[i + 1][j];
    outputLineCount = TERMINAL_MAX_LINES - 1;
  }
  int len = strlen(text);
  if (len >= TERMINAL_LINE_WIDTH)
    len = TERMINAL_LINE_WIDTH - 1;
  for (int i = 0; i < len; i++)
    outputLines[outputLineCount][i] = text[i];
  outputLines[outputLineCount][len] = 0;
  outputLineCount++;
}

void TerminalWindow::ProcessCommand(const char *cmd) {
  if (strlen(cmd) == 0)
    return;

  char cmdLine[32];
  cmdLine[0] = '>';
  cmdLine[1] = ' ';
  int i;
  for (i = 0; i < 28 && cmd[i]; i++)
    cmdLine[i + 2] = cmd[i];
  cmdLine[i + 2] = 0;
  AddOutputLine(cmdLine);

  if (strequal(cmd, "help")) {
    AddOutputLine("Commands:");
    AddOutputLine(" help - show this");
    AddOutputLine(" clear - clear scr");
    AddOutputLine(" about - OS info");
  } else if (strequal(cmd, "clear")) {
    outputLineCount = 0;
    for (int i = 0; i < TERMINAL_MAX_LINES; i++)
      for (int j = 0; j < TERMINAL_LINE_WIDTH; j++)
        outputLines[i][j] = 0;
  } else if (strequal(cmd, "about")) {
    AddOutputLine("ShitOS v1.0");
    AddOutputLine("A hobby OS project");
    AddOutputLine("320x200 VGA Mode 13h");
  } else {
    AddOutputLine("Unknown command");
    AddOutputLine("Type 'help'");
  }
}

void TerminalWindow::Draw(VideoGraphicsArray *vga) {
  if (!visible)
    return;

  vga->FillRect(x + 2, y + 2, width, height, VGA_BLACK);
  vga->FillRect(x, y, width, height, bgColor);

  uint8_t tbColor = focused ? VGA_LIGHT_BLUE : titleBarColor;
  vga->FillRect(x, y, width, titleBarHeight, tbColor);
  vga->DrawRect(x, y, width, height, VGA_WINDOW_BORDER);
  vga->DrawLine(x, y + titleBarHeight, x + width - 1, y + titleBarHeight,
                VGA_WINDOW_BORDER);

  vga->FillRect(x + width - 12, y + 3, 8, 8, VGA_RED);
  vga->DrawLine(x + width - 11, y + 4, x + width - 6, y + 9, VGA_WHITE);
  vga->DrawLine(x + width - 6, y + 4, x + width - 11, y + 9, VGA_WHITE);

  Font::DrawString(vga, x + 4, y + 3, title, VGA_WHITE);

  int32_t textX = x + 4;
  int32_t textY = y + titleBarHeight + 4;

  for (int i = 0; i < outputLineCount && i < TERMINAL_MAX_LINES; i++) {
    Font::DrawString(vga, textX, textY, outputLines[i], fgColor);
    textY += 10;
  }

  Font::DrawString(vga, textX, textY, "> ", fgColor);
  Font::DrawString(vga, textX + 16, textY, inputBuffer, fgColor);
  vga->FillRect(textX + 16 + inputPos * 8, textY, 2, 8, fgColor);
}

void TerminalWindow::OnKeyDown(char c) {
  if (!visible || !focused)
    return;

  if (c == '\n' || c == '\r') {
    ProcessCommand(inputBuffer);
    inputPos = 0;
    for (int i = 0; i < 64; i++)
      inputBuffer[i] = 0;
  } else if (c == '\b' || c == 127) {
    if (inputPos > 0) {
      inputPos--;
      inputBuffer[inputPos] = 0;
    }
  } else if (c >= 32 && c < 127 && inputPos < 20) {
    inputBuffer[inputPos++] = c;
    inputBuffer[inputPos] = 0;
  }
}

void TerminalWindow::Clear() {
  outputLineCount = 0;
  inputPos = 0;
  for (int i = 0; i < 64; i++)
    inputBuffer[i] = 0;
  for (int i = 0; i < TERMINAL_MAX_LINES; i++)
    for (int j = 0; j < TERMINAL_LINE_WIDTH; j++)
      outputLines[i][j] = 0;
}

void TerminalWindow::SetVisible(bool v) {
  visible = v;
  if (v)
    focused = true;
}
bool TerminalWindow::IsVisible() { return visible; }
void TerminalWindow::SetFocused(bool f) { focused = f; }
bool TerminalWindow::IsFocused() { return focused; }
void TerminalWindow::SetPosition(int32_t newX, int32_t newY) {
  x = newX;
  y = newY;
}
void TerminalWindow::GetPosition(int32_t *outX, int32_t *outY) {
  *outX = x;
  *outY = y;
}
bool TerminalWindow::Contains(int32_t px, int32_t py) {
  return px >= x && px < x + width && py >= y && py < y + height;
}
bool TerminalWindow::ContainsTitleBar(int32_t px, int32_t py) {
  return px >= x && px < x + width - 12 && py >= y && py < y + titleBarHeight;
}
bool TerminalWindow::ContainsCloseButton(int32_t px, int32_t py) {
  return px >= x + width - 12 && px < x + width - 4 && py >= y + 3 &&
         py < y + 11;
}

// TicTacToe
TicTacToeWindow::TicTacToeWindow(int32_t x, int32_t y) {
  this->x = x;
  this->y = y;
  this->width = 110;
  this->height = 130;
  this->titleBarHeight = 14;
  this->visible = false;
  this->focused = false;
  Reset();
}

TicTacToeWindow::~TicTacToeWindow() {}

void TicTacToeWindow::Reset() {
  for (int i = 0; i < 9; i++)
    board[i] = 0;
  currentPlayer = 1;
  winner = 0;
  gameOver = false;
}

void TicTacToeWindow::CheckWin() {
  // Rows
  for (int i = 0; i < 3; i++) {
    if (board[i * 3] != 0 && board[i * 3] == board[i * 3 + 1] &&
        board[i * 3 + 1] == board[i * 3 + 2]) {
      winner = board[i * 3];
      gameOver = true;
      return;
    }
  }
  // Columns
  for (int i = 0; i < 3; i++) {
    if (board[i] != 0 && board[i] == board[i + 3] &&
        board[i + 3] == board[i + 6]) {
      winner = board[i];
      gameOver = true;
      return;
    }
  }
  // Diagonals
  if (board[0] != 0 && board[0] == board[4] && board[4] == board[8]) {
    winner = board[0];
    gameOver = true;
    return;
  }
  if (board[2] != 0 && board[2] == board[4] && board[4] == board[6]) {
    winner = board[2];
    gameOver = true;
    return;
  }
  // Draw check
  bool full = true;
  for (int i = 0; i < 9; i++) {
    if (board[i] == 0) {
      full = false;
      break;
    }
  }
  if (full) {
    winner = 3;
    gameOver = true;
  }
}

void TicTacToeWindow::Draw(VideoGraphicsArray *vga) {
  if (!visible)
    return;

  // Shadow and background
  vga->FillRect(x + 2, y + 2, width, height, VGA_BLACK);
  vga->FillRect(x, y, width, height, VGA_WHITE);

  // Title bar
  uint8_t tbColor = focused ? VGA_LIGHT_BLUE : VGA_TITLEBAR;
  vga->FillRect(x, y, width, titleBarHeight, tbColor);
  vga->DrawRect(x, y, width, height, VGA_WINDOW_BORDER);
  vga->DrawLine(x, y + titleBarHeight, x + width - 1, y + titleBarHeight,
                VGA_WINDOW_BORDER);

  // Close button
  vga->FillRect(x + width - 12, y + 3, 8, 8, VGA_RED);
  vga->DrawLine(x + width - 11, y + 4, x + width - 6, y + 9, VGA_WHITE);
  vga->DrawLine(x + width - 6, y + 4, x + width - 11, y + 9, VGA_WHITE);

  Font::DrawString(vga, x + 4, y + 3, "TicTacToe", VGA_WHITE);

  // Draw board
  int32_t boardX = x + 10;
  int32_t boardY = y + titleBarHeight + 5;
  int32_t cellSize = 28;

  // Grid lines
  vga->DrawLine(boardX + cellSize, boardY, boardX + cellSize,
                boardY + cellSize * 3, VGA_BLACK);
  vga->DrawLine(boardX + cellSize * 2, boardY, boardX + cellSize * 2,
                boardY + cellSize * 3, VGA_BLACK);
  vga->DrawLine(boardX, boardY + cellSize, boardX + cellSize * 3,
                boardY + cellSize, VGA_BLACK);
  vga->DrawLine(boardX, boardY + cellSize * 2, boardX + cellSize * 3,
                boardY + cellSize * 2, VGA_BLACK);

  // Draw X's and O's
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      int32_t cx = boardX + col * cellSize + cellSize / 2;
      int32_t cy = boardY + row * cellSize + cellSize / 2;
      int32_t half = cellSize / 2 - 4;

      if (board[row * 3 + col] == 1) {
        // Draw X
        vga->DrawLine(cx - half, cy - half, cx + half, cy + half, VGA_RED);
        vga->DrawLine(cx + half, cy - half, cx - half, cy + half, VGA_RED);
      } else if (board[row * 3 + col] == 2) {
        // Draw O
        vga->DrawRect(cx - half, cy - half, half * 2, half * 2, VGA_BLUE);
      }
    }
  }

  int32_t statusY = boardY + cellSize * 3 + 5;
  if (gameOver) {
    if (winner == 1)
      Font::DrawString(vga, boardX, statusY, "X Wins!", VGA_RED);
    else if (winner == 2)
      Font::DrawString(vga, boardX, statusY, "O Wins!", VGA_BLUE);
    else
      Font::DrawString(vga, boardX, statusY, "Draw!", VGA_BLACK);
    Font::DrawString(vga, boardX, statusY + 10, "Click:New", VGA_DARK_GRAY);
  } else {
    if (currentPlayer == 1)
      Font::DrawString(vga, boardX, statusY, "X's turn", VGA_RED);
    else
      Font::DrawString(vga, boardX, statusY, "O's turn", VGA_BLUE);
  }
}

void TicTacToeWindow::OnMouseDown(int32_t mx, int32_t my) {
  if (!visible || !focused)
    return;

  int32_t boardX = x + 10;
  int32_t boardY = y + titleBarHeight + 5;
  int32_t cellSize = 28;

  if (mx >= boardX && mx < boardX + cellSize * 3 && my >= boardY &&
      my < boardY + cellSize * 3) {
    if (gameOver) {
      Reset();
      return;
    }

    int col = (mx - boardX) / cellSize;
    int row = (my - boardY) / cellSize;
    int cell = row * 3 + col;

    if (cell >= 0 && cell < 9 && board[cell] == 0) {
      board[cell] = currentPlayer;
      CheckWin();
      currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }
  }
}

void TicTacToeWindow::SetVisible(bool v) {
  visible = v;
  if (v)
    focused = true;
}
bool TicTacToeWindow::IsVisible() { return visible; }
void TicTacToeWindow::SetFocused(bool f) { focused = f; }
bool TicTacToeWindow::IsFocused() { return focused; }
void TicTacToeWindow::SetPosition(int32_t newX, int32_t newY) {
  x = newX;
  y = newY;
}
void TicTacToeWindow::GetPosition(int32_t *outX, int32_t *outY) {
  *outX = x;
  *outY = y;
}
bool TicTacToeWindow::Contains(int32_t px, int32_t py) {
  return px >= x && px < x + width && py >= y && py < y + height;
}
bool TicTacToeWindow::ContainsTitleBar(int32_t px, int32_t py) {
  return px >= x && px < x + width - 12 && py >= y && py < y + titleBarHeight;
}
bool TicTacToeWindow::ContainsCloseButton(int32_t px, int32_t py) {
  return px >= x + width - 12 && px < x + width - 4 && py >= y + 3 &&
         py < y + 11;
}
