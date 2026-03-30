---
description: "Use when implementing or reviewing PLAN Phase E integration flow: turn loop, ESC operations, state machine, and event trace sequence."
name: "Phase E 流程整合守則"
applyTo: "src/main.c, src/game.c, src/input.c, include/events.h"
---

# Phase E（流程整合）指令

本指令只適用於 `PLAN.md` 的 Phase E：
- 5.1 標準回合流程整合
- 5.2 ESC 暫停/繼續/重開/回主選單/退出確認
- 5.3 狀態機（`InGame / Paused / MainMenu / ConfirmExit`）
- 5.4 事件追蹤輸出

## 限定可變更檔案（Phase E）
- `include/config.h`
- `include/events.h`
- `include/messages.h`
- `src/main.c`
- `src/game.c`
- `src/input.c`
- `src/ui.c`
- `src/messages.c`
- `tests/test_phase_e.c`、`tests/test_phase_e.exe`（測試檔案）
- `README.MD`（僅限同步流程與狀態機）

## 實作硬規則
- 主流程必須遵守：顯示 → 取行動 → 共用落子 → 判定 → 換手。
- ESC 相關操作需經狀態機控制，不得散落多處跳轉。
- 事件追蹤需對應 `events.h` 事件碼，不可直接寫裸字串。
- 暫停與退出確認不得破壞既有棋局狀態。

## Definition of Done（DoD）
- 標準回合循環可穩定執行並正確換手。
- ESC 選單可完成暫停/繼續/重開/回主選單/退出確認。
- 狀態機轉移可追蹤且無非法跳轉。
- 事件碼序列可用於除錯（至少涵蓋開局、落子、終局、退出）。
- README 已更新流程與狀態機說明。

## 驗收回報格式
- 流程整合變更檔案
- 狀態轉移表（來源狀態 -> 事件 -> 目標狀態）
- ESC 流程驗證結果
- 事件碼序列範例
