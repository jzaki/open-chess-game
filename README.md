# Build a Logos App with AI

This repo is intended to help AI agents quickly and canonically create a Logos App, without going astray or stumbling along the way.

Fork it for your app idea.

# Your App Idea

A single game of chess, shared by all running the app.
## UI
- Simple standard chess UI
- Always from white side of the board
- Game status on the right of the board, as text "White to play..."
- Toggle for solo/network play, starts on solo so can play against small chess library opponent

## Networking
- App connects and listens for someone else to broadcast the game state
- If no game state heard for 5s, create a new game and broadcast entire game state
- If an active game state has not be heard for 3s, send game state
- If multiple game states are received, update to the first that is a valid next move

## Storage
No storage requirements, the game only persists in the apps memory.

## Blockchain
No blockchain requirements.

# AI agent instruction

AI Agent to create a plan to deliver a Logos App with [your app idea](#your-app-idea), populating the [tech design/plan](#tech-designplan).

# Tech design/plan

## Architecture Overview

A Logos app composed of two modules:
1. **UI Module** (QML): Displays the chess board and game controls using logos-design-system
2. **ChessEngine Module** (C++): Handles chess logic, move validation, AI opponent, and network synchronization via Logos broadcast

The modules communicate through C++ interfaces. The UI listens to board state changes and publishes user moves; the engine maintains game state and broadcasts it to peers.

## Module Structure

```
open-chess-game/
├── ui-module/              # QML UI module
│   ├── src/
│   │   ├── main.qml
│   │   ├── BoardView.qml   # 8x8 chess board component
│   │   └── GamePanel.qml   # Status/controls panel
│   └── CMakeLists.txt
├── engine-module/          # C++ Chess Engine backend
│   ├── include/
│   │   └── chess_engine.h  # Public interface
│   ├── src/
│   │   ├── chess_engine.cpp
│   │   ├── move_validator.cpp
│   │   └── network_sync.cpp
│   └── CMakeLists.txt
└── CMakeLists.txt         # Root project file
```

## Core Components

### 1. Chess Engine Module (C++)
- **Move Validation**: Legal move checking, check/checkmate detection
- **Board State**: FEN representation, move history tracking
- **Signals**: Emit board changes for UI to display
- **Networking**: Broadcast via Logos delivery_module (see AGENTS.md skills)
  - Send GameState (FEN + timestamp + game ID) every 3s if active
  - Listen for peer broadcasts, validate moves, accept first-valid
  - Create new game if no state heard for 5s

### 2. AI Opponent (C++)
- **Solo Mode Logic**: Generates legal moves for Black with configurable difficulty
- **Embedded**: Part of chess_engine module
- **Difficulty Levels**: 
  - Easy: random legal move
  - Medium: simple position evaluation (piece count, centrality)

### 3. UI Module (QML)
- **BoardView**: 8x8 grid using logos-design-system, clickable squares for move input
- **GamePanel** (right sidebar):
  - "White/Black to play" status
  - Captured pieces display
  - Check/checkmate/draw notifications
  - Move history log
  - Solo/Network toggle button
  - New Game button
  - Difficulty selector (solo mode)
- **Network Status**: Visual indicator when synced with peers

## Implementation Phases

### Phase 1: Project Setup (Day 1)
- [ ] Clone logos-module-builder and use `#ui-qml-backend` template
- [ ] Set up engine module with CMakeLists.txt (C++ backend)
- [ ] Set up UI module with CMakeLists.txt (QML frontend)
- [ ] Configure module dependencies in parent CMakeLists.txt
- [ ] Verify build system works (cmake + build)

### Phase 2: Chess Engine Core (Day 1-2)
- [ ] Implement move_validator.cpp (legal move checking, check/checkmate)
- [ ] Build chess_engine.h interface with signals for board changes
- [ ] Implement board state management (FEN representation, move history)
- [ ] Test move validation with unit tests
- [ ] Implement "New Game" reset functionality

### Phase 3: UI Module (Day 2-3)
- [ ] Build BoardView.qml (8x8 grid with clickable squares)
- [ ] Create GamePanel.qml (status, controls, piece display) using logos-design-system
- [ ] Connect UI to engine via C++ signals/slots
- [ ] Implement move input handling and validation feedback
- [ ] Add solo/network toggle button and difficulty selector

### Phase 4: Solo AI Mode (Day 3)
- [ ] Implement AI move generation (easy: random, medium: evaluation)
- [ ] Auto-play opponent's move after user move with delay
- [ ] Test solo gameplay end-to-end

### Phase 5: Network Sync (Day 4)
- [ ] Integrate delivery_module for broadcast messaging (see [use-delivery-module skill](AGENTS.md))
- [ ] Implement GameState encoding/decoding (FEN + timestamp + game ID)
- [ ] Add 5s timeout for new game creation when idle
- [ ] Add 3s state broadcast heartbeat when active
- [ ] Add move legality validation for received states
- [ ] Implement first-valid-move acceptance on conflicts

### Phase 6: Polish & Testing (Day 4-5)
- [ ] Add network status indicator in UI
- [ ] Test multi-client synchronization
- [ ] Test network failover (no broadcast → new game)
- [ ] Test invalid move rejection from network
- [ ] Performance and responsiveness testing
- [ ] Documentation of module interface

## State Flow Diagram

```
User Input
    ↓
Move Validation (chess.js)
    ↓
Update Local Board → Broadcast State
    ↓
Listen for Remote Broadcasts
    ↓
Validate Remote Move → Accept/Reject
    ↓
Sync Local & Remote if Consistent
```

## Technology Stack
- **Template**: logos-module-builder `#ui-qml-backend` (provides base module structure)
- **UI Framework**: QML (Qt) + logos-design-system
- **Backend**: C++17
- **Chess Logic**: Custom move validation (no external library to keep dependencies minimal)
- **Networking**: Logos delivery_module for broadcast messaging
- **Build System**: CMake + C++ compiler
- **Development**: Follow [logos-tutorial](https://github.com/logos-co/logos-tutorial/blob/tutorial-v4/) (tutorial-v4 tag)

## Success Criteria
- ✓ Solo play works with valid move enforcement
- ✓ Multiple clients see the same board state in real-time
- ✓ Network failures gracefully create new games after 5s
- ✓ Invalid moves are rejected silently
- ✓ UI clearly shows whose turn it is