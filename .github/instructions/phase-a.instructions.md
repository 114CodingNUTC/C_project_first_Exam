---
description: "Use when implementing or reviewing PLAN Phase A architecture contract, module skeleton, try_place_stone API, and config/messages/events centers."
name: "Phase A 實作守則"
applyTo: "src/**, include/**, README.MD"
---

# Phase A（架構與契約）指令

本指令只適用於 `PLAN.md` 的 Phase A：
- 1.1 建立模組骨架
- 1.2 定義單一落子 API `try_place_stone(...)`
- 1.3 建立 `config/messages/events` 三中心

## 限定可變更檔案（Phase A）
- `include/config.h`
- `include/messages.h`
- `include/events.h`
- `src/messages.c`
- `src/main.c`
- `src/game.c`
- `src/board.c`
- `src/rules.c`
- `src/input.c`
- `src/ui.c`
- `src/ai_easy.c`
- `src/ai_medium.c`
- `src/ai_hard.c`
- `tests/test_phase_a.c`、`tests/test_phase_a.exe`（測試檔案）
- `README.MD`（僅限同步 Phase A 文件）

## 實作硬規則
- 玩家與 AI 的落子必須共用 `try_place_stone(...)`，禁止分叉判定。
- 禁止硬編碼文案、事件、狀態與錯誤碼；必須集中在三中心。
- 僅使用專案允許標頭：`stdio.h` `stdlib.h` `string.h` `ctype.h` `time.h` `conio.h` `windows.h`。
- 所有 `.c` 檔必須位於 `src/`。

## Definition of Done（DoD）
- 已建立 `include/` 與 `src/` 的 Phase A 骨架檔案。
- `try_place_stone(...)` 已定義且玩家/AI 呼叫路徑一致。
- `config/messages/events` 具備最小可用常數、鍵值與事件碼。
- 專案可用 `gcc -o game.exe src/*.c -Iinclude` 成功編譯。
- README 已反映 Phase A 新增檔案與 API 契約。

## 驗收回報格式
- 變更檔案清單
- `try_place_stone(...)` 所在檔案與簽名
- 三中心新增項目摘要（config/messages/events）
- 編譯結果（成功/失敗 + 失敗原因）
