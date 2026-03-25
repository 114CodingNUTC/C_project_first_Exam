# 五子棋（終端機）C 專案

## 概述
本專案目標為實作 Dev C++ 可編譯的純 C 終端機五子棋。開發順序與需求鎖定於 `PLAN.md`，採 Phase A-F 逐步完成。

核心原則：
- 單一落子流程：玩家與 AI 都必須走同一提交 API。
- 禁止硬編碼：常數、事件碼、文案集中管理。
- 模組化：規則、輸入、UI、AI 分層，避免邏輯交叉。

## 目前進度
- ✓ 已完成 Phase A 骨架：`include/` 三中心與 `src/` 模組最小可編譯版本。
- ✓ 已完成 Phase B 規則核心：
  - 棋盤操作（初始化、合法檢查、提交）
  - 勝負檢查（四方向連五判定）
  - 終局資訊（勝方與成線五子座標）
  - 回合控制（換手、和局判定）
  - 共用 API：`try_place_stone(...)` 統一玩家與 AI 落子流程
- ✓ 已完成 Phase C（第一版）UI 與輸入：
   - 主選單模式選擇（1v1 / 1vAI 簡中難）
   - 棋盤大小選擇（9x9 / 15x15 / 19x19）
   - 文字輸入解析（`a,1` / `a1` / `1a`）
   - 方向鍵移動游標 + Enter 落子
   - 訊息槽提示與錯誤顯示（含錯誤紅字）
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

## Phase A 契約（已落地）
- 共用落子 API（位於 `src/game.c`）：

```c
int try_place_stone(
	GomokuGame *game,
	int player,
	int row,
	int col,
	int *out_event_code,
	int *out_message_key
);
```

- `out_event_code`：回報事件碼（如 `EV_STONE_PLACED` / `EV_WIN` / `EV_DRAW` / `EV_INVALID_MOVE`）
- `out_message_key`：回報文案鍵值（由 `messages.c` 映射語系字串）
- 玩家與 AI 必須共用此 API，禁止規則分叉。

## Phase B 規則核心（已落地）

### 實現內容
1. **棋盤操作**（`src/board.c`）
   - `board_init()`：初始化棋盤為指定大小
   - `board_is_in_bounds()`：檢查座標是否在棋盤范圍內
   - `board_is_empty()`：檢查位置是否為空
   - `board_place_stone()`：放置棋子

2. **勝負檢查**（`src/rules.c`）
   - `rules_check_win()`：判定四方向連五（橫、縱、兩條斜線）
   - 返回成線五子的座標陣列供 UI 高亮使用
   - `rules_is_draw()`：判定和局

3. **遊戲狀態管理**（`src/game.c`）
   - `game_reset()`：重置遊戲狀態
   - 單一落子流程統一驗證：邊界、占用、當前玩家、遊戲結束狀態
   - 自動切換回合、判定勝負與和局
   - 保存終局資訊（勝方、成線五子座標）

### 驗收清單
- ✓ 棋盤初始化與邊界檢查無誤
- ✓ 四方向連五判定正確
- ✓ 終局座標正確捕獲（供UI高亮）
- ✓ 回合自動切換
- ✓ 非法落子正確拒絕
- ✓ 和局判定正確
- ✓ 所有落子必經 `try_place_stone()` 入口
- ✓ 編譯無警告（使用 -Wall -Wextra）

### Smoke 測試結果
已通過 20 項全面 smoke 測試，涵蓋以下場景：
- ✓ 棋盤操作（初始化、邊界檢查、合法提交）
- ✓ 回合管理（自動切換、當前玩家設定）
- ✓ 錯誤檢測（邊界外、占用位置、非當前玩家、遊戲結束）
- ✓ 橫向連五判定 + 成線座標正確捕獲
- ✓ 縱向連五判定
- ✓ 正斜線連五判定
- ✓ 反斜線連五判定
- ✓ 多子連線（6+ 子仍判定為勝）
- ✓ 不同棋盤大小（9×9、15×15、19×19）
- ✓ 棋盤邊角位置
- ✓ 遊戲狀態鎖定（勝利後禁止落子）
- ✓ 狀態複位功能
- ✓ 和局判定

## Phase C 操作方式（目前可用）
- 啟動後先選擇遊戲模式，再選擇棋盤大小。
- 玩家可用方向鍵移動游標，按 Enter 在游標位置落子。
- 也可直接輸入座標：`a1`、`a,1`、`1a`（大小寫皆可）。
- 訊息槽統一顯示提示與錯誤訊息；錯誤訊息以紅字顯示。
- 高亮規則：最後落子以小寫 `x/o` 顯示，終局成線五子以 `#` 顯示。

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