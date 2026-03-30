---
description: "Use when implementing or reviewing PLAN Phase F validation and documentation: matrix checks, UI stability, rule correctness, anti-hardcode audit, and README updates."
name: "Phase F 驗證與文件守則"
applyTo: "README.MD, src/**, include/**"
---

# Phase F（驗證與文件）指令

本指令只適用於 `PLAN.md` 的 Phase F：
- 6.1 功能矩陣驗證
- 6.2 UI 穩定檢查（防閃爍/刷新正確）
- 6.3 規則正確性（成線五子/勝方/和局）
- 6.4 反硬編碼與語系/設定完整性檢查
- 6.5 README 更新

## 限定可變更檔案（Phase F）
- `README.MD`
- `.github/prompts/phase-smoke-test.prompt.md`
- `.github/agents/rule-audit.agent.md`
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
- `tests/test_phase_*.c`、`tests/test_phase_*.exe`（所有測試檔案）
- `src/ai_medium.c`
- `src/ai_hard.c`

## 實作硬規則
- 驗證結果不可虛構；無法執行時必須明確標記缺口。
- 缺陷修正需最小化，避免超出本階段範圍的大改。
- 反硬編碼檢查需覆蓋 `config/messages/events` 三中心。
- README 必須與實作現況一致，不得保留過期流程。

## Definition of Done（DoD）
- 已完成最小 smoke：勝利、和局、非法落子。
- UI 穩定性結果可描述（至少含已知限制）。
- 已完成事件碼與文案鍵缺漏檢查。
- README 包含建置、操作、模組邊界、事件碼/設定說明。
- 所有驗證結論皆可追溯到實際執行或明確檢查記錄。

## 驗收回報格式
- 驗證矩陣摘要（模式 × 棋盤 × 輸入 × AI）
- smoke 結果（勝/和/非法）
- 反硬編碼檢查摘要（config/messages/events）
- README 更新重點與待補項
