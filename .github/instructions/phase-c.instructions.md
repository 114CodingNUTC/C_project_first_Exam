---
description: "Use when implementing or reviewing PLAN Phase C UI and input: menu, parsing, cursor controls, message slot, highlights, and safe redraw."
name: "Phase C UI 與輸入守則"
applyTo: "src/input.c, src/ui.c, src/main.c, include/messages.h"
---

# Phase C（UI 與輸入）指令

本指令只適用於 `PLAN.md` 的 Phase C：
- 主選單與模式/棋盤設定
- 輸入解析（`a,1`/`a1`/`1a`，大小寫皆可）與方向鍵提交
- 右下角單一訊息槽
- 畫面高亮（最後落子、終局五子）
- 防閃爍與刷新一致性修復

## 限定可變更檔案（Phase C）
- `include/config.h`
- `include/events.h`
- `include/messages.h`
- `src/input.c`
- `src/ui.c`
- `src/game.c`
- `src/main.c`
- `src/messages.c`
- `README.MD`（僅限同步操作與 UI 規範）

## 實作硬規則
- 必須保留右下角單一訊息槽；提示與錯誤共用同區。
- 錯誤訊息需紅字顯示並自動回復（預設 2 秒，由 config 管理）。
- 必須顯示當前思考方、最後落子高亮、終局成線五子高亮。
- 輸入與 UI 不得自行繞過 `try_place_stone(...)`。

## Definition of Done（DoD）
- 可從主選單完成模式與棋盤大小選擇。
- 輸入解析支援文字格式與方向鍵 + Enter 提交。
- 訊息槽可穩定顯示提示/錯誤且會回復。
- 刷新過程無明顯殘影錯位，失敗時有自動修復重繪策略。
- README 已更新操作方式與 UI 規範。

## 驗收回報格式
- UI/輸入變更檔案
- 支援輸入格式清單與範例
- 訊息槽與高亮行為摘要
- 已知刷新限制與修復策略
