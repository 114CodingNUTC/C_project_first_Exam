# 五子棋（終端機）C 專案

## 概述
本專案目標為實作 Dev C++ 可編譯的純 C 終端機五子棋。開發順序與需求鎖定於 `PLAN.md`，採 Phase A-F 逐步完成。

核心原則：
- 單一落子流程：玩家與 AI 都必須走同一提交 API。
- 禁止硬編碼：常數、事件碼、文案集中管理。
- 模組化：規則、輸入、UI、AI 分層，避免邏輯交叉。


## 下載連結
- Game.exe：[點此下載](https://github.com/114CodingNUTC/C_project_first_Exam/raw/refs/heads/main/game.exe)

## 目前進度
- ? 已完成 Phase A 骨架：`include/` 三中心與 `src/` 模組最小可編譯版本。
- ? 已完成 Phase B 規則核心：
  - 棋盤操作（初始化、合法檢查、提交）
  - 勝負檢查（四方向連五判定）
  - 終局資訊（勝方與成線五子座標）
  - 回合控制（換手、和局判定）
  - 共用 API：`try_place_stone(...)` 統一玩家與 AI 落子流程
- ? 已完成 Phase C（第一版）UI 與輸入：
   - 主選單模式選擇（1v1 / 1vAI 簡中難）
   - 棋盤大小選擇（9x9 / 15x15 / 19x19）
   - 文字輸入解析（`a,1` / `a1` / `1a`）
   - 方向鍵移動游標 + Enter 落子
   - Tab 立即投降並結束對局
   - 訊息槽提示與錯誤顯示（含錯誤紅字）
- ? 已完成 Phase D AI 實現：
   - **簡單難度**：隨機選擇合法位置
   - **中等難度**：防守優先 + 基礎進攻（檢查對手連4防守，檢查自己連4進攻，優先靠近棋子位置）
   - **困難難度**：Minimax + Alpha-Beta 修剪 + 候選點縮減（搜索深度 5，主要考慮棋盤活躍區域）
   - 所有 AI 難度都走統一的 `try_place_stone()` 落子 API
- ? 已完成 Phase E 流程整合（第一版）：
   - 標準回合流程：顯示 → 取行動 → 共用落子 → 判定 → 換手
   - ESC 進入暫停選單（繼續 / 重開 / 回主選單 / 退出確認）
   - 狀態機：`InGame / Paused / ConfirmExit / MainMenu`
   - 事件追蹤輸出（EV_READY / EV_STONE_PLACED / EV_WIN / EV_DRAW / EV_PAUSED / EV_EXIT_*）
- ? 已完成 Phase F 驗證與文件（第一版）：
   - 功能矩陣驗證（模式 × 棋盤大小 × 輸入格式 × AI 難度）
   - smoke 驗證（勝利 / 和局 / 非法落子）
   - 三中心檢查（config / messages / events）
   - README 與測試流程同步更新
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
# Windows PowerShell (需要完整文件列表)
gcc -o game.exe src/main.c src/game.c src/board.c src/rules.c src/input.c src/ui.c src/messages.c src/ai_easy.c src/ai_medium.c src/ai_hard.c -Iinclude
game.exe

# Linux/macOS 或 Bash
gcc -o game.exe src/*.c -Iinclude
./game.exe
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
- ? 棋盤初始化與邊界檢查無誤
- ? 四方向連五判定正確
- ? 終局座標正確捕獲（供UI高亮）
- ? 回合自動切換
- ? 非法落子正確拒絕
- ? 和局判定正確
- ? 所有落子必經 `try_place_stone()` 入口
- ? 編譯無警告（使用 -Wall -Wextra）

### Smoke 測試結果
已通過 20 項全面 smoke 測試，涵蓋以下場景：
- ? 棋盤操作（初始化、邊界檢查、合法提交）
- ? 回合管理（自動切換、當前玩家設定）
- ? 錯誤檢測（邊界外、占用位置、非當前玩家、遊戲結束）
- ? 橫向連五判定 + 成線座標正確捕獲
- ? 縱向連五判定
- ? 正斜線連五判定
- ? 反斜線連五判定
- ? 多子連線（6+ 子仍判定為勝）
- ? 不同棋盤大小（9×9、15×15、19×19）
- ? 棋盤邊角位置
- ? 遊戲狀態鎖定（勝利後禁止落子）
- ? 狀態複位功能
- ? 和局判定

## Phase D AI 實現動作（目前可用）

### AI 難度對比

| 難度 | 策略 | 搜索範圍 | 特點 |
|------|------|---------|------|
| 簡單 | 隨機選擇 | 全盤 | 速度快，行為不可預測 |
| 中等 | 防守優先 + 進攻 | 活躍區域 | 能防守對手連4，能進攻自己連4，優先靠近棋子 |
| 困難 | Minimax + Alpha-Beta | 活躍區域 + 候選縮減 | 搜索深度 5，評估所有可能?法，採用Alpha-Beta修剪加速 |

### 實現細節
- **ai_easy.c**：
  - 收集所有空位，用 `rand()` 隨機選擇一個
  
- **ai_medium.c**：
  - 先檢查對手是否有連4位置（防守優先）
  - 再檢查自己是否有連4位置（能立即贏）
  - 若無，優先選擇距離現有棋子為 2 格以內的位置（靠近棋子）
  - 最後才選擇任意空位
  
- **ai_hard.c**：
  - 使用 Minimax 算法搜索（深度 5）
  - Alpha-Beta 修剪加速搜索
  - 只考慮距現有棋子 2 格以內的候選位置（候選縮減）
  - 評估函數：計算雙方連子數（連5=無限分、連4=10000、連3=1000 等）
  - 棋盤空空時，預設先在中心落子

### AI 測試驗收
已通過 Phase D 測試套件，涵蓋：
- ? 簡單難度：隨機選擇所有位置（可驗證多次調用不同結果）
- ? 中等難度：防守對手連4、進攻自己連4、靠近棋子策略
- ? 困難難度：Minimax 尋找即時贏?、策略選擇、防守威脅

## Phase E 流程整合（目前可用）

### 狀態機
- `InGame`：正常回合進行
- `Paused`：ESC 後進入暫停選單
- `ConfirmExit`：退出確認（是/否）
- `MainMenu`：回主選單

### 狀態轉移
- `InGame --ESC--> Paused`
- `Paused --繼續--> InGame`
- `Paused --重開--> InGame（重置棋局）`
- `Paused --回主選單--> MainMenu`
- `Paused --退出--> ConfirmExit`
- `ConfirmExit --否--> Paused`
- `ConfirmExit --是--> 結束程式`

### 事件追蹤樣例
- `[TRACE] EV=100`（EV_READY）
- `[TRACE] EV=120`（EV_STONE_PLACED）
- `[TRACE] EV=130`（EV_WIN）
- `[TRACE] EV=160`（EV_PAUSED）
- `[TRACE] EV=220`（EV_EXIT_APP）

## Phase F 驗證與文件（目前可用）

### 驗證矩陣摘要
- 模式：`1v1 / 1vAI(Easy/Medium/Hard)`
- 棋盤：`9x9 / 15x15 / 19x19`
- 輸入：`a1 / 1a / a,1`
- AI：`easy / medium / hard`（合法座標輸出驗證）

### smoke 驗證
- 勝利：連五觸發 `EV_WIN`
- 和局：滿盤觸發 `EV_DRAW`
- 非法落子：越界觸發 `EV_INVALID_MOVE`

### 反硬編碼檢查摘要
- `events.h`：事件碼唯一性檢查
- `messages.h/messages.c`：文案鍵值唯一性與映射完整性檢查（中/英）
- `config.h`：棋盤大小與輸入/訊息設定值合理性檢查

### Phase F 測試執行
```bash
gcc -o tests/test_phase_f.exe tests/test_phase_f.c src/game.c src/board.c src/rules.c src/messages.c src/ui.c src/input.c src/ai_easy.c src/ai_medium.c src/ai_hard.c -Iinclude
./tests/test_phase_f.exe
```

### UI 穩定性已知限制
- 目前採用 ANSI 清屏序列（`ESC[3J ESC[2J ESC[H]`），需終端支援 ANSI 控制碼。
- 在標準 Windows Terminal / VS Code Terminal 表現正常；若終端禁用 ANSI，需改回 WinAPI 清屏策略。

## Phase C 操作方式（目前可用）
- 啟動後先選擇遊戲模式，再選擇棋盤大小。
- 玩家可用方向鍵移動游標，按 Enter 在游標位置落子。
- 對局中按 Tab 可立即投降並直接結束遊戲。
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