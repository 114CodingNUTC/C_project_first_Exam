---
description: "Use when implementing or reviewing PLAN Phase B rule core: board operations, win check, terminal info, and turn control."
name: "Phase B 規則核心守則"
applyTo: "src/board.c, src/rules.c, src/game.c, include/events.h, include/config.h"
---

# Phase B（規則核心）指令

本指令只適用於 `PLAN.md` 的 Phase B：
- 2.1 棋盤操作（初始化/合法檢查/提交/回滾）
- 2.2 勝負檢查（四方向連五）
- 2.3 終局資訊（勝方 + 成線五子座標）
- 2.4 回合控制（換手/和局/狀態轉移）

## 限定可變更檔案（Phase B）
- `include/config.h`
- `include/events.h`
- `include/messages.h`
- `src/board.c`
- `src/rules.c`
- `src/game.c`
- `src/main.c`
- `src/messages.c`
- `tests/test_phase_b.c`、`tests/test_phase_b.exe`（測試檔案）
- `README.MD`（僅限同步 Phase B 規則與事件）

## 實作硬規則
- 所有落子必須經過 `try_place_stone(...)`。
- 連五判定邏輯只能在規則核心維護，禁止 UI/AI 複製判定。
- 終局資訊需可提供 UI 高亮使用（五子座標）。
- 事件碼與訊息鍵值不得硬編碼於流程中。

## Definition of Done（DoD）
- 可正確判定勝利、和局、非法落子。
- 已輸出終局必要資訊（勝方與成線五子）。
- 回合切換與終局狀態轉移一致，無玩家/AI 分叉。
- 專案可用 `gcc -o game.exe src/*.c -Iinclude` 成功編譯。
- README 已更新 Phase B 核心規則摘要。

## 驗收回報格式
- 規則核心變更檔案
- 勝利/和局/非法落子驗證結果
- 終局資訊結構摘要（含成線五子）
- 新增或調整的事件碼/文案鍵
