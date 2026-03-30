---
description: "Use when implementing or reviewing PLAN Phase D AI layers: easy random, medium defense-first, and hard minimax alpha-beta candidate pruning."
name: "Phase D AI 守則"
applyTo: "src/ai_easy.c, src/ai_medium.c, src/ai_hard.c, src/game.c"
---

# Phase D（AI）指令

本指令只適用於 `PLAN.md` 的 Phase D：
- 4.1 簡單 AI（隨機合法點）
- 4.2 中等 AI（防守優先 + 基礎進攻）
- 4.3 困難 AI（Minimax + alpha-beta + 候選點縮減）
- 4.4 AI 僅回傳座標，最終交由共用落子流程提交

## 限定可變更檔案（Phase D）
- `include/config.h`
- `include/events.h`
- `include/messages.h`
- `src/ai_easy.c`
- `src/ai_medium.c`
- `src/ai_hard.c`
- `src/game.c`
- `src/main.c`
- `src/board.c`
- `src/rules.c`
- `src/messages.c`
- `tests/test_phase_d.c`、`tests/test_phase_d.exe`（測試檔案）
- `README.MD`（僅限同步 AI 難度與行為）

## 實作硬規則
- AI 禁止直接改寫棋盤，僅可回傳目標座標。
- AI 落子必須走 `try_place_stone(...)` 共用入口。
- 難度差異應體現在策略，不得破壞規則一致性。
- 不得在 AI 內硬編碼事件碼與顯示文案。

## Definition of Done（DoD）
- 三個難度皆可輸出合法座標。
- AI 不會繞過共用落子 API。
- 中/難模式能展現明顯強度差異（至少防守優先可觀察）。
- 專案可用 `gcc -o game.exe src/*.c -Iinclude` 成功編譯。
- README 已更新 AI 難度差異與限制。

## 驗收回報格式
- AI 變更檔案
- 各難度策略摘要（各 1-2 句）
- AI 入口與提交流程說明
- 基本對局驗證結果（至少簡/中/難各一局）
