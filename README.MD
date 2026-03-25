# 五子棋（終端機）C 專案

## 概述
本專案目標為實作 Dev C++ 可編譯的純 C 終端機五子棋。開發順序與需求鎖定於 `PLAN.md`，採 Phase A-F 逐步完成。

核心原則：
- 單一落子流程：玩家與 AI 都必須走同一提交 API。
- 禁止硬編碼：常數、事件碼、文案集中管理。
- 模組化：規則、輸入、UI、AI 分層，避免邏輯交叉。

## 目前進度
- 現階段為初始化階段，`src/game.c` 為最小可編譯入口。
- 規格與分工主文件：`PLAN.md`。

## 預定目錄（依 PLAN）
- `src/main.c`：主流程與狀態機
- `src/game.c`：遊戲整合流程
- `src/board.c`：棋盤初始化/合法檢查/提交/回滾
- `src/rules.c`：勝負判定與終局資訊
- `src/input.c`：輸入解析（`a,1`/`a1`/`1a`）與游標控制
- `src/ui.c`：棋盤渲染與訊息槽/高亮
- `src/ai_easy.c` / `src/ai_medium.c` / `src/ai_hard.c`：AI
- `src/messages.c`：語系文案映射
- `include/config.h` / `include/messages.h` / `include/events.h`：三中心

## 建置與執行

### Dev C++
1. 開啟專案。
2. Build / Run。
3. 產出 `game.exe`。

### GCC（MinGW）
```bash
gcc -o game.exe src/*.c -Iinclude
game.exe
```

> 若尚未建立 `include/` 或完整 `src/*.c`，請先依 `PLAN.md` 的 Phase A 建立骨架。

## 開發規範（精簡）
- 僅使用：`stdio.h` `stdlib.h` `string.h` `ctype.h` `time.h` `conio.h` `windows.h`
- 所有 C 檔僅放 `src/`
- 事件碼集中在 `events.h`
- 文案鍵值集中在 `messages.h` / `messages.c`
- 設定集中在 `config.h`（修改後需重編譯）

## UI/流程要求（後續實作）
- 右下角單一訊息槽（提示/錯誤共區）
- 錯誤紅字顯示後自動回復（預設 2 秒）
- 高亮：最後落子、終局成線五子
- 狀態機：`InGame / Paused / MainMenu / ConfirmExit`

## 參考文件
- `PLAN.md`：完整需求、Phase 依賴、鎖定決策
- `.github/copilot-instructions.md`：工作區開發指引
- `.github/agents/gomoku-dev.agent.md`：五子棋專用代理規範

## 成員 Members
- 1411432047 吳欣柔
- 1411432048 林珈綺
- 1411432057 鄭欣承
- 1411432058 陳宇霆
- 1411432061 吳承祐