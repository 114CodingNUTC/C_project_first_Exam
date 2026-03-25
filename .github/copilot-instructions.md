# Project Guidelines

## Code Style
- 使用純 C（Dev C++ 相容），維持函式短小、單一職責與清楚命名。
- 禁止硬編碼：事件、狀態、錯誤、文案與可調參數必須集中在 `events/messages/config`。
- 所有 `.c` 檔放在 `src/`；輸出執行檔名稱維持 `game.exe`。
- 可用標頭限制為：`stdio.h` `stdlib.h` `string.h` `ctype.h` `time.h` `conio.h` `windows.h`。

## Architecture
- 專案主規格以 [PLAN.md](../PLAN.md) 為唯一真實來源（Phase A-F 與決策鎖定項）。
- 玩家與 AI 必須共用單一落子流程（`try_place_stone(...)`），禁止規則分叉。
- 模組邊界：`board`（棋盤）/ `rules`（勝負）/ `input`（輸入解析）/ `ui`（渲染）/ `ai_*`（AI）/ `main`（狀態機）。
- 三中心：`config`（可重編譯設定）/ `messages`（語系文案鍵值）/ `events`（事件碼）。

## Build and Test
- Dev C++：以專案 Build & Run 為主流程。
- 命令列（MinGW/gcc）：`gcc -o game.exe src/*.c -Iinclude`。
- 執行：`game.exe`。
- 目前尚未建立自動化測試；新增規則或輸入邏輯時，請至少做最小 smoke 驗證（勝利、和局、非法落子）。

## Conventions
- UI 規範需保留：右下角單一訊息槽、最後落子高亮、終局成線五子高亮。
- 若新增常數/事件/文案，必須同步更新對應中心與 README 說明。
- 詳細開發流程與檢查清單請參考 [.github/agents/gomoku-dev.agent.md](agents/gomoku-dev.agent.md)。