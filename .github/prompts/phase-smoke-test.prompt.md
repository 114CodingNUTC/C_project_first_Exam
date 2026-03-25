---
name: "Phase Smoke Test"
description: "Run a fixed minimal Gomoku smoke validation for win, draw, and illegal move scenarios, then report concise pass/fail results."
argument-hint: "可選：提供棋盤大小（預設 15）與模式（預設 1v1）"
agent: "agent"
---

請對此五子棋專案執行「固定最小 smoke 驗證流程」，僅包含以下三類：
1. 勝利判定（連五）
2. 和局判定（滿盤無連五）
3. 非法落子（越界/重複落子/格式錯誤其一即可）

執行要求：
- 先嘗試編譯：`gcc -o game.exe src/*.c -Iinclude`
- 若目前程式未完成、無法完整互動，請退化為「可執行的最小驗證計畫 + 缺口清單」，不要虛構結果。
- 所有落子測試敘述需確認是否經過共用入口 `try_place_stone(...)`。

輸出格式（固定）：

## Smoke 結果
- 編譯：PASS/FAIL（附一句原因）
- 勝利判定：PASS/FAIL（附重現步驟摘要）
- 和局判定：PASS/FAIL（附重現步驟摘要）
- 非法落子：PASS/FAIL（附重現步驟摘要）

## 缺口與風險
- 列出阻礙 smoke 完成的缺少模組/API/事件碼

## 下一步
- 只給 1-3 個最高優先修正項目
