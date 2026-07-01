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

Following Logos framework conventions, this is a **UI App / View Module** with:
- **Frontend**: QML UI (using logos-design-system) running in Basecamp process
- **Backend**: C++ Logos Module (isolated `logos_host` subprocess) handling chess engine, AI, networking, and game state
- **Communication**: QtRO (Qt Remote Objects) bridging QML frontend ↔ C++ backend via `LogosQmlBridge`
- **Build Template**: [logos-module-builder](https://github.com/logos-co/logos-module-builder/) `#ui-qml-backend`
- **Reference**: [logos-tutorial ui-qml-backend example](https://github.com/logos-co/logos-tutorial/blob/tutorial-v4/outputs/tutorial-cpp-ui-app.md)

**Process Architecture**:
```
Basecamp (Main Process)
├── QML UI (chess_app_ui.qml)
└── LogosQmlBridge ──┐
                     │ IPC (QLocalSocket + QtRO)
                     ↓
                  ui-host subprocess
                  └── C++ ChessGameBackend
                      ├── Chess engine (move validation)
                      ├── AI opponent logic
                      ├── Network manager (game state broadcast)
                      └── Game state / signals
```

## Core Components

### Backend (C++ Logos Module)

**1. ChessEngine** (`src/backend/chess_engine.h/cpp`)
- Chess move validation and game logic
- Wrapper around Stockfish library or custom implementation
- Methods: `isValidMove()`, `makeMove()`, `getGameStatus()`, `getMoveList()`
- Handles FEN serialization for network transmission

**2. GameState** (`src/backend/game_state.h/cpp`)
- Manages current game FEN, whose turn, game status
- Emits Qt signals on state changes (for QML UI binding)
- Signals: `gameFenChanged()`, `turnChanged()`, `statusChanged()`

**3. AiOpponent** (`src/backend/ai_opponent.h/cpp`)
- Integrates Stockfish engine for move selection
- `getBestMove(fen, depth)`: Returns best move for position
- Configurable difficulty via search depth

**4. NetworkManager** (`src/backend/network_manager.h/cpp`)
- Broadcast discovery (mDNS or local UDP broadcast)
- Heartbeat: sends game state every 3s if active
- Receives incoming game states and validates moves
- 5s timeout: creates new game if no state heard
- Emits `stateReceived()` signal when new state available

**5. ChessGameBackend** (`src/backend/chess_game_backend.h/cpp`)
- Main backend class inheriting `LogosModuleContext`
- Orchestrates ChessEngine, GameState, AiOpponent, NetworkManager
- Q_INVOKABLE methods exposed to QML:
  - `makeMove(fromSquare, toSquare)` → bool
  - `resetGame()` → void
  - `setMode(solo/network)` → void
  - `setDifficulty(1-10)` → void
- Emits Qt signals to QML: `fenChanged()`, `statusChanged()`, `modeChanged()`

### Frontend (QML UI)

**1. ChessBoard.qml** (`src/ui/ChessBoard.qml`)
- 8x8 grid displaying pieces
- Click-to-move interaction with valid move highlighting
- Always white's perspective
- Binds to backend FEN via `LogosQmlBridge.callModule()`

**2. GameStatus.qml** (`src/ui/GameStatus.qml`)
- Text display: current turn, game status (checkmate, check, etc.)
- Binds to backend status signals

**3. ModeToggle.qml** (`src/ui/ModeToggle.qml`)
- Radio buttons: Solo vs Network mode
- Difficulty slider for solo mode
- Calls backend methods via `LogosQmlBridge.callModule()`

**4. Main.qml** (`src/ui/Main.qml`)
- Main UI container orchestrating ChessBoard, GameStatus, ModeToggle
- Uses logos-design-system components

## Implementation Phases

### Phase 0: Project Setup (Prerequisite)
1. Clone this repo and run `nix flake init -t github:logos-co/logos-module-builder#ui-qml-backend`
2. Set up `metadata.json` with module name, type (`ui_qml`), dependencies
3. Run `nix build .#generate` to set up code generation structure
4. Verify flake builds: `nix build .#default`

**Reference**: [logos-tutorial-v4 setup](https://github.com/logos-co/logos-tutorial/blob/tutorial-v4/outputs/tutorial-cpp-ui-app.md)

### Phase 1: Basic Chess Backend & UI (Milestone: Playable chess in UI)
1. Implement `ChessEngine` with move validation (use Stockfish or custom)
2. Implement `GameState` with FEN tracking and Qt signals
3. Implement `ChessGameBackend` with Q_INVOKABLE methods
4. Implement `ChessBoard.qml` with 8x8 grid and piece rendering
5. Implement `GameStatus.qml` text display
6. Wire QML ↔ backend via `LogosQmlBridge` (callModule)
7. Test click-to-move in UI

**Files**: `src/backend/chess_engine.h/cpp`, `src/backend/game_state.h/cpp`, `src/backend/chess_game_backend.h/cpp`, `src/ui/ChessBoard.qml`, `src/ui/GameStatus.qml`, `src/ui/Main.qml`

### Phase 2: Solo Play with AI (Milestone: Play against AI opponent)
1. Integrate Stockfish in `AiOpponent`
2. Implement `ChessGameBackend::makeMove()` to call AI when in solo mode and Black's turn
3. Implement `ModeToggle.qml` for solo/network mode selection
4. Add difficulty slider for search depth
5. Auto-move AI opponent after user move

**Files**: `src/backend/ai_opponent.h/cpp`, `src/ui/ModeToggle.qml`, `src/backend/chess_game_backend.cpp` (updated)

### Phase 3: Network Broadcasting (Milestone: Multi-app sync)
1. Implement `NetworkManager` for local game state broadcast (mDNS or UDP)
2. Implement 3s heartbeat in NetworkManager
3. Implement 5s timeout (create new game if no state heard)
4. Implement move validation for received states
5. Connect `NetworkManager` to `GameState` and `ChessGameBackend`
6. Test with `nix run` multiple instances

**Files**: `src/backend/network_manager.h/cpp`, `src/backend/chess_game_backend.cpp` (updated with network orchestration)

### Phase 4: Portability & Testing (Milestone: Production ready)
1. Build portable `.lgx` bundle: `nix build .#lgx-portable`
2. Test in Basecamp with other modules
3. Validate all variants (`linux-x86_64`, `darwin-arm64`, etc.) included
4. Unit tests for ChessEngine, AiOpponent, NetworkManager
5. Integration tests for move propagation and state sync

**Files**: Test suite, CI/CD config, metadata.json validation

## Key Files to Create

```
src/
├── backend/
│   ├── chess_engine.h/cpp        # Move validation, game logic
│   ├── game_state.h/cpp          # FEN tracking, Qt signals
│   ├── ai_opponent.h/cpp         # Stockfish integration
│   ├── network_manager.h/cpp     # Local broadcast, sync
│   └── chess_game_backend.h/cpp  # Main Logos module, orchestration
├── ui/
│   ├── ChessBoard.qml            # 8x8 board display
│   ├── GameStatus.qml            # Turn/status text
│   ├── ModeToggle.qml            # Solo/Network mode switch
│   └── Main.qml                  # Top-level UI container
├── CMakeLists.txt                # Build config
├── metadata.json                 # Module metadata (type: ui_qml)
└── chess_game_backend.rep        # QtRO contract (QML ↔ backend)
```

## Critical Dependencies

- **Stockfish**: Chess engine (C++ or precompiled)
- **Qt 6**: QML, QtRO, Core libs
- **logos-cpp-sdk**: Code generation, module context
- **logos-design-system**: QML UI components (recommended)
- **logos-view-module-runtime**: QML ↔ C++ bridge

## Testing Strategy

1. **Unit Tests**: ChessEngine (move validation), AiOpponent (move selection)
2. **Integration Tests**: NetworkManager (broadcast/receive), full game loop
3. **Manual Testing**: 
   - Solo: Make moves, verify AI responds
   - Network: `nix run` multiple instances, verify state sync within 3s
   - 5s timeout: Stop one instance, verify new game after 5s
   - Edge cases: Checkmate, stalemate, invalid moves
4. **Portability**: Build `.lgx-portable`, test in Basecamp alongside other modules

## Success Criteria

- ✓ Can play full chess game against AI in solo mode (Phase 2)
- ✓ Multiple app instances sync game state within 3s (Phase 3)
- ✓ New game creates automatically after 5s of silence (Phase 3)
- ✓ UI always shows white's perspective (Phase 1)
- ✓ Portable `.lgx` bundle works in Basecamp (Phase 4)
- ✓ Runs alongside other Logos modules without state conflicts (Phase 4)