#pragma once

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="zh">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<title>FSD 控制器</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:-apple-system,system-ui,"PingFang SC","Microsoft YaHei",sans-serif;background:radial-gradient(circle at top,#15213a 0,#0b1120 45%,#090e1a 100%);color:#e2e8f0;min-height:100vh;padding:16px}
.page{width:100%;margin:0 auto}
.page-header{text-align:center;padding:10px 0 22px}
h1{font-size:22px;color:#38bdf8;font-weight:700;letter-spacing:1px}
.summary-strip{display:flex;align-items:stretch;justify-content:flex-start;gap:14px;flex-wrap:wrap;margin-bottom:16px;padding:14px 16px;background:rgba(19,29,50,.92);border:1px solid rgba(56,189,248,.12);border-radius:16px;box-shadow:0 18px 40px rgba(2,8,23,.18);backdrop-filter:blur(8px)}
.summary-title{display:flex;flex-direction:column;align-items:center;justify-content:center;flex:0 0 auto;min-width:136px;padding:2px 4px;gap:6px}
.summary-title h1{margin:0;font-size:30px;line-height:1.02;text-align:center}
.summary-title h1 span{display:block}
.summary-title-version{font-size:10px;line-height:1;color:#64748b;letter-spacing:.8px}
.summary-title-version-btn{background:transparent;border:none;padding:0;cursor:pointer;transition:.2s;color:#64748b}
.summary-title-version-btn:hover,.summary-title-version-btn:focus{color:#7dd3fc;outline:none}
.summary-group{display:flex;align-items:stretch;justify-content:flex-start;gap:10px;flex-wrap:wrap;flex:1 1 320px}
.summary-actions{display:flex;align-items:center;justify-content:flex-end;gap:10px;flex:0 0 auto;margin-left:auto}
.summary-action{display:flex;flex-direction:column;align-items:center;justify-content:center;gap:8px;width:90px;min-width:90px;padding:10px 12px;border-radius:14px;background:#172033;border:1px solid rgba(56,189,248,.08);text-align:center}
.summary-action-copy{display:flex;flex-direction:column;align-items:center;justify-content:center;gap:0;min-width:0;text-align:center}
.summary-action-label{font-size:13px;font-weight:700;color:#e2e8f0}
.summary-restart-btn{width:60px;min-width:60px;min-height:62px;padding:10px 6px;border-radius:18px;background:#e31937;color:#fff;border:none;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:2px;line-height:1.05;text-align:center;white-space:normal;font-weight:700}
.summary-restart-btn span{display:block}
.summary-restart-btn:hover:not(:disabled){background:#c41530}
.summary-restart-btn:disabled{opacity:.4;cursor:not-allowed}
.summary-pill{flex:0 0 auto;min-width:0;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:8px;padding:10px 10px;border-radius:12px;background:#172033;border:1px solid rgba(56,189,248,.08);text-align:center}
.summary-pill-button{appearance:none;-webkit-appearance:none;cursor:pointer;color:inherit;font:inherit;transition:border-color .2s}
.summary-pill-button:hover,.summary-pill-button:focus{border-color:rgba(56,189,248,.24);outline:none}
.summary-pill-label{font-size:11px;letter-spacing:1px;color:#64748b;text-align:center;white-space:nowrap}
.summary-pill-value{display:inline-flex;align-items:center;justify-content:center;align-self:center;min-height:28px;padding:0 10px;border-radius:999px;font-size:13px;font-weight:700;white-space:nowrap}
.summary-pill-value.status-ok,.summary-pill-value.status-yes{color:#86efac;background:rgba(34,197,94,.14)}
.summary-pill-value.status-warn{color:#fde68a;background:rgba(234,179,8,.14)}
.summary-pill-value.status-err{color:#fca5a5;background:rgba(239,68,68,.14)}
.summary-pill-value.status-no{color:#94a3b8;background:rgba(148,163,184,.12)}
.dashboard{display:grid;grid-template-columns:1fr;gap:16px;align-items:start}
.card{background:rgba(19,29,50,.96);border:1px solid rgba(56,189,248,.08);border-radius:16px;padding:18px;backdrop-filter:blur(8px);box-shadow:0 18px 40px rgba(2,8,23,.22)}
.card-title{font-size:12px;font-weight:700;color:#64748b;letter-spacing:2px;margin-bottom:14px}
.row{display:flex;align-items:center;justify-content:space-between;padding:12px 0;border-bottom:1px solid #1e293b;gap:12px}
.row:last-child{border-bottom:none}
.row-label{font-size:14px;font-weight:500;flex:1;min-width:0}
.row-inline-toggle{align-items:flex-start;justify-content:flex-start;flex-wrap:wrap}
.speed-offset-row-main{display:flex;align-items:center;gap:12px;flex:0 0 auto}
.speed-offset-row-main .row-label{flex:0 0 auto;text-align:left}
.speed-offset-inline-stats{display:flex;align-items:center;gap:10px;flex-wrap:wrap}
.speed-offset-inline-stat{display:flex;align-items:center;gap:8px;padding:8px 12px;border-radius:999px;background:#172033;border:1px solid rgba(56,189,248,.08);font-size:12px;color:#94a3b8}
.speed-offset-inline-value{font-size:13px;font-weight:700}
.control-picker-field{padding:12px 0;border-top:1px solid #1e293b}
.control-picker-field .field-label{margin-bottom:10px;font-size:14px;font-weight:600;color:#e2e8f0;letter-spacing:0}
.toggle{position:relative;width:50px;height:28px;flex:0 0 auto}
.toggle input{opacity:0;width:0;height:0}
.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background:#334155;border-radius:28px;transition:.3s}
.slider:before{content:"";position:absolute;height:22px;width:22px;left:3px;bottom:3px;background:#fff;border-radius:50%;transition:.3s}
input:checked+.slider{background:#22c55e}
input:checked+.slider:before{transform:translateX(22px)}
select,.text-input,.picker-trigger{width:100%;background:#1e293b;color:#e2e8f0;border:1px solid #334155;border-radius:8px;padding:10px 12px;font-size:13px}
.text-area{min-height:120px;resize:vertical;font-family:ui-monospace,SFMono-Regular,Menlo,monospace}
select{padding-right:32px;appearance:none;-webkit-appearance:none;background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='12' height='12' fill='%2394a3b8' viewBox='0 0 16 16'%3E%3Cpath d='M8 11L3 6h10z'/%3E%3C/svg%3E");background-repeat:no-repeat;background-position:right 10px center;min-width:110px;cursor:pointer}
select:focus,.text-input:focus,.picker-trigger:focus{outline:none;border-color:#38bdf8}
select:disabled,.text-input:disabled,.toggle input:disabled+.slider{opacity:.45;cursor:not-allowed}
.picker-wrap{width:100%;min-width:110px}
.picker-native{display:none}
.picker-trigger{display:flex;align-items:center;justify-content:space-between;gap:12px;text-align:left;cursor:pointer}
.picker-trigger:after{content:"";flex:0 0 auto;width:0;height:0;border-left:5px solid transparent;border-right:5px solid transparent;border-top:6px solid #94a3b8}
.picker-trigger:disabled{opacity:.45;cursor:not-allowed}
.picker-trigger:disabled:after{opacity:.45}
.field{padding:10px 0}
.field+.field{border-top:1px solid #1e293b}
.field-label{display:block;font-size:12px;color:#94a3b8;margin-bottom:8px;letter-spacing:.5px}
.hint{font-size:12px;line-height:1.5;color:#94a3b8;margin-top:10px}
.actions{display:flex;gap:10px;margin-top:14px}
.actions>button{flex:1}
.save-btn,.upload-btn,.ghost-btn{border-radius:8px;padding:10px 14px;font-size:14px;font-weight:600;cursor:pointer;letter-spacing:.5px}
.save-btn,.upload-btn{background:#e31937;color:#fff;border:none}
.ghost-btn{background:#1e293b;color:#e2e8f0;border:1px solid #334155}
.save-btn{width:100%}
.upload-btn{width:100%;margin-top:10px}
.ghost-btn.small-btn{padding:8px 12px;font-size:12px;flex:0 0 auto}
.save-btn:disabled,.upload-btn:disabled,.ghost-btn:disabled{opacity:.4;cursor:not-allowed}
.save-btn:hover:not(:disabled),.upload-btn:hover:not(:disabled){background:#c41530}
.ghost-btn:hover:not(:disabled){background:#24324d}
.stats{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-bottom:14px}
.stat{background:#1a2740;border-radius:10px;padding:14px;text-align:center}
.stat-val{font-size:28px;font-weight:800;color:#38bdf8}
.stat-label{font-size:11px;color:#64748b;margin-top:2px;letter-spacing:1px}
.stat-val.green{color:#22c55e}
.stat-val.amber{color:#eab308}
.status-row{display:flex;justify-content:space-between;align-items:center;padding:10px 0;border-bottom:1px solid #1e293b;font-size:14px;gap:12px}
.status-row:last-child{border-bottom:none}
.status-text{max-width:55%;text-align:right;word-break:break-all}
.status-wide{max-width:68%}
.status-ok{color:#22c55e;font-weight:700}
.status-err{color:#ef4444;font-weight:700}
.status-warn{color:#f59e0b;font-weight:700}
.status-yes{color:#22c55e;font-weight:700}
.status-no{color:#64748b;font-weight:700}
.ota-row{display:flex;gap:10px;align-items:center;flex-wrap:wrap}
.file-btn{background:#1e293b;color:#94a3b8;border:1px solid #334155;border-radius:8px;padding:8px 14px;font-size:12px;cursor:pointer}
.file-name{font-size:12px;color:#64748b;flex:1}
.progress{width:100%;height:6px;background:#1e293b;border-radius:3px;margin-top:10px;display:none}
.progress-bar{height:100%;background:#22c55e;border-radius:3px;width:0%;transition:width .3s}
.msg{text-align:center;font-size:12px;margin-top:8px;min-height:16px}
.msg.ok{color:#22c55e}
.msg.err{color:#ef4444}
.ota-layout{display:grid;grid-template-columns:1fr;gap:16px}
.ota-panel{background:#172033;border:1px solid rgba(56,189,248,.08);border-radius:14px;padding:16px}
.ota-panel-title{font-size:14px;font-weight:700;color:#e2e8f0;margin-bottom:12px}
.ota-panel .actions{margin-top:14px}
.ota-panel .hint{margin-top:10px}
.inline-actions{display:flex;align-items:center;justify-content:space-between;gap:10px}
.inline-actions .field-label{margin-bottom:0}
.upstream-search-field .inline-actions{margin-bottom:5px}
.section-head{display:flex;align-items:center;justify-content:space-between;gap:10px;margin-top:14px}
.section-head .field-label{margin-bottom:0}
.saved-list{display:flex;flex-direction:column;gap:10px;margin-top:12px}
.saved-item{display:flex;align-items:center;justify-content:space-between;gap:12px;background:#1a2740;border:1px solid #334155;border-radius:10px;padding:12px}
.saved-main{min-width:0;flex:1}
.saved-name{font-size:14px;font-weight:600;word-break:break-all}
.saved-tags{display:flex;gap:6px;flex-wrap:wrap;margin-top:6px}
.tag{display:inline-flex;align-items:center;padding:4px 8px;border-radius:999px;background:#273449;color:#94a3b8;font-size:11px;line-height:1}
.tag.ok{background:rgba(34,197,94,.16);color:#86efac}
.tag.busy{background:rgba(56,189,248,.16);color:#7dd3fc}
.tag.warn{background:rgba(234,179,8,.16);color:#fde68a}
.empty-box{border:1px dashed #334155;border-radius:10px;padding:12px;text-align:center;font-size:12px;color:#64748b}
.bucket-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(80px,1fr));gap:12px;margin-top:10px}
.bucket-grid .field{padding:0;border-top:none}
.bucket-grid .field-label{text-align:center}
.number-input-wrap{position:relative;display:flex;align-items:center;width:100%;min-width:80px;max-width:100%}
.speed-offset-input{width:100%;min-width:80px;max-width:100%;padding-right:28px}
.number-suffix{position:absolute;top:50%;right:12px;transform:translateY(-50%);font-size:12px;color:#64748b;pointer-events:none}
.speed-offset-input::-webkit-outer-spin-button,.speed-offset-input::-webkit-inner-spin-button{-webkit-appearance:none;margin:0}
.speed-offset-input[type=number]{-moz-appearance:textfield}
.picker-modal{position:fixed;inset:0;display:none;align-items:flex-end;justify-content:center;background:rgba(2,8,23,.72);z-index:9999;padding:16px}
.picker-modal.open{display:flex}
.picker-sheet{width:min(560px,100%);max-height:min(80vh,640px);background:#111827;border:1px solid rgba(56,189,248,.12);border-radius:18px;box-shadow:0 28px 70px rgba(2,8,23,.45);overflow:hidden}
.picker-head{display:flex;align-items:center;justify-content:space-between;gap:12px;padding:16px 18px;border-bottom:1px solid #1e293b}
.picker-title{font-size:15px;font-weight:700;color:#e2e8f0}
.picker-close{background:transparent;border:none;color:#94a3b8;font-size:22px;line-height:1;cursor:pointer;padding:4px 6px}
.picker-body{padding:10px;overflow:auto;max-height:min(60vh,520px)}
.picker-option{width:100%;display:flex;align-items:center;justify-content:space-between;gap:12px;padding:14px 16px;border:none;border-radius:12px;background:#172033;color:#e2e8f0;font-size:15px;text-align:left;cursor:pointer}
.picker-option+.picker-option{margin-top:8px}
.picker-option.active{background:rgba(56,189,248,.18);color:#7dd3fc}
.picker-option:after{content:"";width:10px;height:10px;border-radius:999px;border:2px solid currentColor;opacity:.25}
.picker-option.active:after{opacity:1;background:currentColor}
.confirm-modal{position:fixed;inset:0;display:none;align-items:center;justify-content:center;background:rgba(2,8,23,.78);z-index:10001;padding:16px}
.confirm-modal.open{display:flex}
.confirm-sheet{width:min(420px,100%);background:#111827;border:1px solid rgba(56,189,248,.12);border-radius:18px;box-shadow:0 28px 70px rgba(2,8,23,.45);overflow:hidden}
.confirm-head{padding:18px 20px 12px}
.confirm-title{font-size:18px;font-weight:700;color:#e2e8f0}
.confirm-body{padding:0 20px 18px;font-size:14px;line-height:1.7;color:#94a3b8}
.confirm-actions{display:flex;gap:10px;padding:0 20px 20px}
.confirm-actions button{flex:1}
.confirm-confirm-btn{background:#e31937;color:#fff;border:none}
.confirm-confirm-btn:hover:not(:disabled){background:#c41530}
.version-modal{position:fixed;inset:0;display:none;align-items:center;justify-content:center;background:rgba(2,8,23,.78);z-index:10000;padding:16px}
.version-modal.open{display:flex}
.version-sheet{width:min(460px,100%);background:#111827;border:1px solid rgba(56,189,248,.12);border-radius:18px;box-shadow:0 28px 70px rgba(2,8,23,.45);overflow:hidden}
.version-head{display:flex;align-items:center;justify-content:space-between;gap:12px;padding:18px 20px 12px}
.version-title{font-size:18px;font-weight:700;color:#e2e8f0}
.version-body{padding:0 20px 20px}
.version-actions{display:flex;gap:10px;margin-top:16px}
.version-actions button{flex:1}
.status-layout{display:flex;flex-direction:column;gap:18px}
.status-controls-panel{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:12px 16px}
.status-control-item{background:#172033;border:1px solid rgba(56,189,248,.08);border-radius:14px;padding:14px 16px}
.status-controls-panel .field,
.status-controls-panel .row{padding:0;border-top:none}
.status-controls-panel .field+.field{border-top:none}
.status-controls-panel .row:last-child{border-bottom:none}
.status-controls-panel .field-label{margin-bottom:0;flex:0 0 92px}
.status-control-item.control-picker-field{display:flex;align-items:center;gap:14px;padding:14px 18px}
.status-controls-panel .row.status-control-item{padding:14px 18px;border-bottom:none}
.status-control-item.control-picker-field .picker-wrap{flex:0 0 180px;min-width:0;margin-left:auto}
.hotspot-layout{display:grid;grid-template-columns:1fr;gap:18px}
.hotspot-panel{background:#172033;border:1px solid rgba(56,189,248,.08);border-radius:14px;padding:16px}
.dns-layout{display:grid;grid-template-columns:1fr;gap:18px;align-items:start}
.dns-panel{background:#172033;border:1px solid rgba(56,189,248,.08);border-radius:14px;padding:16px}
.dns-editor-panel{display:flex;flex-direction:column;align-items:stretch}
.dns-editor-panel .field{display:flex;flex-direction:column;flex:none;min-height:auto}
.dns-editor-panel .text-area{flex:none;height:140px;min-height:140px}
.dns-stats-strip{display:flex;align-items:center;gap:10px;flex-wrap:wrap;margin-bottom:14px}
.dns-stat-chip{display:flex;align-items:center;gap:10px;min-width:0;padding:10px 12px;border-radius:12px;background:#1a2740;border:1px solid #334155;flex:1 1 0}
.dns-stat-label{font-size:12px;color:#94a3b8;white-space:nowrap}
.dns-stat-value{margin-left:auto;font-size:20px;line-height:1;font-weight:800}
#dnsBlockedList{max-height:540px;overflow-y:auto;padding-right:6px;scrollbar-width:thin;scrollbar-color:#334155 rgba(15,23,42,.38)}
#dnsBlockedList::-webkit-scrollbar{width:8px}
#dnsBlockedList::-webkit-scrollbar-track{background:rgba(15,23,42,.38);border-radius:999px}
#dnsBlockedList::-webkit-scrollbar-thumb{background:#334155;border-radius:999px}
#dnsBlockedList::-webkit-scrollbar-thumb:hover{background:#475569}
@media (min-width:980px){
body{padding:24px}
.summary-strip{padding:16px 18px}
.summary-title h1{font-size:34px}
.summary-group:last-child{justify-content:flex-start;flex:1 1 auto}
.summary-actions{margin-left:auto}
.dashboard{grid-template-columns:1fr 1.1fr 1.1fr;gap:18px}
.card-status{grid-column:1 / -1;grid-row:1}
.card-speed-offset{grid-column:1 / -1;grid-row:2}
.card-hotspot{grid-column:1 / -1;grid-row:3}
.card-dns{grid-column:1 / -1;grid-row:4}
.card-ota{grid-column:1 / -1;grid-row:5}
.ota-layout{grid-template-columns:repeat(2,minmax(0,1fr))}
.hotspot-layout{grid-template-columns:1fr 1fr}
.dns-layout{grid-template-columns:repeat(2,minmax(0,1fr));align-items:start}
.dns-editor-panel .field{flex:none;min-height:auto}
.dns-editor-panel .text-area{height:170px;min-height:170px}
.card{padding:20px}
.row,.status-row{padding:13px 0}
.status-text{max-width:60%}
}
@media (max-width:640px){
body{padding:12px}
.summary-strip{padding:12px}
.summary-title{width:100%;padding:0}
.summary-title h1{font-size:24px}
.summary-title-version{font-size:9px}
.summary-group{flex:1 1 100%}
.summary-actions{flex:1 1 100%;justify-content:flex-start;margin-left:0}
.summary-action{min-width:0;flex:0 0 auto}
.summary-restart-btn{flex:0 0 auto}
.summary-pill{flex:1 1 calc(50% - 5px);min-width:calc(50% - 5px)}
.speed-offset-row-main{width:100%;justify-content:flex-start}
.status-controls-panel{grid-template-columns:1fr}
.ota-layout{grid-template-columns:1fr}
.hotspot-layout{grid-template-columns:1fr}
.dns-layout{grid-template-columns:1fr}
.dns-stats-strip{flex-direction:column;align-items:stretch}
.inline-actions,.section-head{align-items:flex-start;flex-direction:column}
.actions{flex-direction:column}
.saved-item{align-items:flex-start;flex-direction:column}
.saved-item .ghost-btn.small-btn{align-self:flex-end}
}
</style>
</head>
<body>

<div class="page">
<div class="summary-strip">
  <div class="summary-title">
    <h1><span>FSD</span><span>控制器</span></h1>
    <button type="button" class="summary-title-version summary-title-version-btn" id="topHeaderVersion" onclick="openVersionDialog()">--</button>
  </div>
  <div class="summary-group">
    <div class="summary-pill">
      <span class="summary-pill-label" id="topHwModeLabel">硬件版本</span>
      <span class="summary-pill-value status-no" id="topHwMode">--</span>
    </div>
    <div class="summary-pill">
      <span class="summary-pill-label">CAN 总线</span>
      <span class="summary-pill-value status-no" id="topCanState">--</span>
    </div>
    <div class="summary-pill">
      <span class="summary-pill-label">上游网络</span>
      <span class="summary-pill-value status-no" id="topUpstreamState">--</span>
    </div>
    <div class="summary-pill">
      <span class="summary-pill-label">白/黑名单</span>
      <span class="summary-pill-value status-no" id="topDnsRules">--</span>
    </div>
    <button type="button" class="summary-pill summary-pill-button" id="topCpuUsageCard" onclick="openHardwareInfoDialog()" aria-haspopup="dialog">
      <span class="summary-pill-label">CPU占用</span>
      <span class="summary-pill-value status-no" id="topCpuUsage">--</span>
    </button>
    <div class="summary-pill">
      <span class="summary-pill-label">芯片温度</span>
      <span class="summary-pill-value status-no" id="topThermalState">--</span>
    </div>
  </div>
  <div class="summary-actions">
    <label class="summary-action">
      <span class="summary-action-copy">
        <span class="summary-action-label">FSD 开关</span>
      </span>
      <span class="toggle"><input type="checkbox" id="topFsdEnable" onchange="confirmTopFsdToggle(this)"><span class="slider"></span></span>
    </label>
    <button class="summary-restart-btn" id="topRestartBtn" onclick="restartDevice()"><span>设备</span><span>重启</span></button>
  </div>
</div>
<div class="dashboard">

<div class="card card-status">
  <div class="card-title">状态</div>
  <div class="status-layout">
    <div class="stats">
      <div class="stat"><div class="stat-val green" id="sModified">0</div><div class="stat-label">已修改</div></div>
      <div class="stat"><div class="stat-val" id="sRX">0</div><div class="stat-label">已接收</div></div>
      <div class="stat"><div class="stat-val amber" id="sErrors">0</div><div class="stat-label">错误</div></div>
      <div class="stat"><div class="stat-val" id="sUptime">0秒</div><div class="stat-label">运行时间</div></div>
    </div>
    <div class="status-controls-panel">
      <div class="field control-picker-field status-control-item">
        <label class="field-label" for="hwMode">硬件版本</label>
        <div class="picker-wrap">
          <select id="hwMode" class="picker-native" data-picker-title="硬件版本" data-picker-trigger="hwModeBtn" onchange="setVal('hwMode',this.value)">
            <option value="0">LEGACY</option>
            <option value="1">HW3</option>
            <option value="2" selected>HW4</option>
          </select>
          <button type="button" class="picker-trigger" id="hwModeBtn" onclick="openPicker('hwMode')"></button>
        </div>
      </div>
      <div class="field control-picker-field status-control-item">
        <label class="field-label" for="speedProfile">速度模式</label>
        <div class="picker-wrap">
          <select id="speedProfile" class="picker-native" data-picker-title="速度模式" data-picker-trigger="speedProfileBtn" onchange="setVal('speedProfile',this.value)">
            <option value="0">保守</option>
            <option value="1" selected>默认</option>
            <option value="2">适中</option>
            <option value="3">激进</option>
            <option value="4">最大</option>
          </select>
          <button type="button" class="picker-trigger" id="speedProfileBtn" onclick="openPicker('speedProfile')"></button>
        </div>
      </div>
      <div class="field control-picker-field status-control-item">
        <label class="field-label" for="profileMode">模式来源</label>
        <div class="picker-wrap">
          <select id="profileMode" class="picker-native" data-picker-title="模式来源" data-picker-trigger="profileModeBtn" onchange="setVal('profileMode',this.value)">
            <option value="1" selected>自动（拨杆）</option>
            <option value="0">手动</option>
          </select>
          <button type="button" class="picker-trigger" id="profileModeBtn" onclick="openPicker('profileMode')"></button>
        </div>
      </div>
      <div class="row status-control-item">
        <span class="row-label">限速提示音抑制</span>
        <label class="toggle"><input type="checkbox" id="isaChime" onchange="confirmConfigToggle(this,'isaChime','限速提示音抑制')"><span class="slider"></span></label>
      </div>
      <div class="row status-control-item">
        <span class="row-label">紧急车辆检测</span>
        <label class="toggle"><input type="checkbox" id="emergencyDet" checked onchange="confirmConfigToggle(this,'emergencyDet','紧急车辆检测')"><span class="slider"></span></label>
      </div>
      <div class="row status-control-item">
        <span class="row-label">中国模式 🇨🇳</span>
        <label class="toggle"><input type="checkbox" id="chinaMode" onchange="confirmConfigToggle(this,'chinaMode','中国模式')"><span class="slider"></span></label>
      </div>
    </div>
  </div>
</div>

<div class="card card-speed-offset">
  <div class="card-title">速度偏移</div>
  <div class="row row-inline-toggle">
    <div class="speed-offset-row-main">
      <span class="row-label">启用速度偏移</span>
      <label class="toggle"><input type="checkbox" id="speedOffsetEnable" onchange="confirmConfigToggle(this,'speedOffsetEnable','速度偏移')"><span class="slider"></span></label>
    </div>
    <div class="speed-offset-inline-stats">
      <div class="speed-offset-inline-stat">
        <span>当前道路限速</span>
        <span class="speed-offset-inline-value status-no" id="speedOffsetRoadInline">--</span>
      </div>
      <div class="speed-offset-inline-stat">
        <span>当前偏移</span>
        <span class="speed-offset-inline-value status-no" id="speedOffsetCurrentInline">--</span>
      </div>
      <div class="speed-offset-inline-stat">
        <span>偏移结果</span>
        <span class="speed-offset-inline-value status-no" id="speedOffsetResultInline">--</span>
      </div>
    </div>
  </div>
  <div class="bucket-grid">
    <div class="field">
      <label class="field-label" for="speedOffsetPct0">限速 &lt; 40</label>
      <div class="number-input-wrap">
        <input class="text-input speed-offset-input" type="number" inputmode="numeric" min="0" max="50" step="1" id="speedOffsetPct0" oninput="handleSpeedOffsetInput(0)" onchange="saveSpeedOffsetInput(0)">
        <span class="number-suffix">%</span>
      </div>
    </div>
    <div class="field">
      <label class="field-label" for="speedOffsetPct1">40 - 50</label>
      <div class="number-input-wrap">
        <input class="text-input speed-offset-input" type="number" inputmode="numeric" min="0" max="50" step="1" id="speedOffsetPct1" oninput="handleSpeedOffsetInput(1)" onchange="saveSpeedOffsetInput(1)">
        <span class="number-suffix">%</span>
      </div>
    </div>
    <div class="field">
      <label class="field-label" for="speedOffsetPct2">50 - 60</label>
      <div class="number-input-wrap">
        <input class="text-input speed-offset-input" type="number" inputmode="numeric" min="0" max="50" step="1" id="speedOffsetPct2" oninput="handleSpeedOffsetInput(2)" onchange="saveSpeedOffsetInput(2)">
        <span class="number-suffix">%</span>
      </div>
    </div>
    <div class="field">
      <label class="field-label" for="speedOffsetPct3">60 - 70</label>
      <div class="number-input-wrap">
        <input class="text-input speed-offset-input" type="number" inputmode="numeric" min="0" max="50" step="1" id="speedOffsetPct3" oninput="handleSpeedOffsetInput(3)" onchange="saveSpeedOffsetInput(3)">
        <span class="number-suffix">%</span>
      </div>
    </div>
    <div class="field">
      <label class="field-label" for="speedOffsetPct4">70 - 80</label>
      <div class="number-input-wrap">
        <input class="text-input speed-offset-input" type="number" inputmode="numeric" min="0" max="50" step="1" id="speedOffsetPct4" oninput="handleSpeedOffsetInput(4)" onchange="saveSpeedOffsetInput(4)">
        <span class="number-suffix">%</span>
      </div>
    </div>
    <div class="field">
      <label class="field-label" for="speedOffsetPct5">80 - 90</label>
      <div class="number-input-wrap">
        <input class="text-input speed-offset-input" type="number" inputmode="numeric" min="0" max="50" step="1" id="speedOffsetPct5" oninput="handleSpeedOffsetInput(5)" onchange="saveSpeedOffsetInput(5)">
        <span class="number-suffix">%</span>
      </div>
    </div>
    <div class="field">
      <label class="field-label" for="speedOffsetPct6">90 - 100</label>
      <div class="number-input-wrap">
        <input class="text-input speed-offset-input" type="number" inputmode="numeric" min="0" max="50" step="1" id="speedOffsetPct6" oninput="handleSpeedOffsetInput(6)" onchange="saveSpeedOffsetInput(6)">
        <span class="number-suffix">%</span>
      </div>
    </div>
    <div class="field">
      <label class="field-label" for="speedOffsetPct7">100 - 110</label>
      <div class="number-input-wrap">
        <input class="text-input speed-offset-input" type="number" inputmode="numeric" min="0" max="50" step="1" id="speedOffsetPct7" oninput="handleSpeedOffsetInput(7)" onchange="saveSpeedOffsetInput(7)">
        <span class="number-suffix">%</span>
      </div>
    </div>
    <div class="field">
      <label class="field-label" for="speedOffsetPct8">110 - 120</label>
      <div class="number-input-wrap">
        <input class="text-input speed-offset-input" type="number" inputmode="numeric" min="0" max="50" step="1" id="speedOffsetPct8" oninput="handleSpeedOffsetInput(8)" onchange="saveSpeedOffsetInput(8)">
        <span class="number-suffix">%</span>
      </div>
    </div>
    <div class="field">
      <label class="field-label" for="speedOffsetPct9">≥ 120</label>
      <div class="number-input-wrap">
        <input class="text-input speed-offset-input" type="number" inputmode="numeric" min="0" max="50" step="1" id="speedOffsetPct9" oninput="handleSpeedOffsetInput(9)" onchange="saveSpeedOffsetInput(9)">
        <span class="number-suffix">%</span>
      </div>
    </div>
  </div>
  <div class="hint">仅 HW3 生效。每个区间默认 0；区间按车辆当前显示的限速单位理解。如果当前道路限速未知，则按 0 处理。</div>
</div>

<div class="card card-feature card-hotspot">
  <div class="card-title">热点网络</div>
  <div class="hotspot-layout">
    <div class="hotspot-panel">
      <div class="card-title">上游热点</div>
      <div class="row">
        <span class="row-label">启用上游热点接入</span>
        <label class="toggle"><input type="checkbox" id="upstreamEnable" onchange="confirmUpstreamToggle(this)"><span class="slider"></span></label>
      </div>
      <div class="field upstream-search-field">
        <div class="inline-actions">
          <label class="field-label" for="upstreamSSID">名称</label>
          <button class="ghost-btn small-btn" id="scanBtn" onclick="scanUpstreamNetworks()">搜索热点</button>
        </div>
        <input class="text-input" type="text" id="upstreamSSID" maxlength="32" placeholder="搜索不到时可手动填写">
        <select id="scanResults" class="picker-native" data-picker-title="选择附近热点" onchange="applyScanResultSelection()">
          <option value="">点击“搜索热点”查看附近可用热点</option>
        </select>
      </div>
      <div class="field">
        <label class="field-label" for="upstreamPass">密码</label>
        <input class="text-input" type="password" id="upstreamPass" maxlength="63" placeholder="首次添加或更新密码时填写">
      </div>
      <div class="actions">
        <button class="save-btn" onclick="saveSelectedUpstream()">保存上游热点</button>
      </div>
      <div class="hint">ESP32接收的热点。可从列表选择，也可手动输入热点名称。</div>
      <div class="saved-list" id="savedNetworks"></div>
      <div class="msg" id="netMsg"></div>
      <div class="status-row"><span>上游状态</span><span id="sUpstream" class="status-no status-text">--</span></div>
      <div class="status-row"><span>当前上游热点</span><span id="sCurrentUpstream" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>已保存上游热点</span><span id="sSavedUpstreams" class="status-no status-text">0</span></div>
      <div class="status-row"><span>上游 RSSI</span><span id="sUpstreamRSSI" class="status-no status-text">--</span></div>
      <div class="status-row"><span>信号质量</span><span id="sUpstreamSignal" class="status-no status-text">--</span></div>
      <div class="status-row"><span>当前信道</span><span id="sWiFiChannel" class="status-no status-text">--</span></div>
      <div class="status-row"><span>上游 IP</span><span id="sUpstreamIP" class="status-no status-text">--</span></div>
      <div class="status-row"><span>NAT 转发</span><span id="sNAT" class="status-no status-text">--</span></div>
    </div>
    <div class="hotspot-panel">
      <div class="card-title">下游热点</div>
      <div class="field">
        <label class="field-label" for="apSSID">名称</label>
        <input class="text-input" type="text" id="apSSID" maxlength="32" placeholder="例如：FSD-Controller" oninput="markApDirty()">
      </div>
      <div class="field">
        <label class="field-label" for="apPass">密码</label>
        <input class="text-input" type="password" id="apPass" maxlength="63" placeholder="8-63 个字符" oninput="markApDirty()">
      </div>
      <div class="actions">
        <button class="save-btn" onclick="saveApConfig()">保存下游热点</button>
      </div>
      <div class="msg" id="apMsg"></div>
      <div class="hint">ESP32发射的热点。</div>
      <div class="status-row"><span>下游客户端</span><span id="sAPClients" class="status-no status-text">0</span></div>
      <div class="status-row"><span>下游热点</span><span id="sAP" class="status-ok status-text">--</span></div>
      <div class="status-row"><span>下游地址</span><span id="sAPIP" class="status-ok status-text">--</span></div>
    </div>
  </div>
</div>

<div class="card card-feature card-dns">
  <div class="card-title">DNS 规则</div>
  <div class="dns-layout">
    <div class="dns-panel dns-editor-panel">
      <div class="row">
        <span class="row-label">启用 DNS 规则</span>
        <label class="toggle"><input type="checkbox" id="dnsWhitelistEnable" onchange="confirmDnsToggle(this)"><span class="slider"></span></label>
      </div>
      <div class="field">
        <label class="field-label" for="dnsAllowlist">允许解析的域名</label>
        <textarea class="text-input text-area" id="dnsAllowlist" maxlength="384" placeholder="每行一个域名，例如：&#10;tesla.com&#10;apple.com" oninput="markDnsDirty()"></textarea>
      </div>
      <div class="field">
        <label class="field-label" for="dnsBlocklist">禁止解析的域名</label>
        <textarea class="text-input text-area" id="dnsBlocklist" maxlength="384" placeholder="每行一个域名，例如：&#10;google.com&#10;doubleclick.net" oninput="markDnsDirty()"></textarea>
      </div>
      <div class="hint">支持逗号、空格或换行分隔。填写 `tesla.com` 会同时允许 `api.tesla.com` 这类子域名。</div>
      <div class="hint">黑名单优先于白名单。若白名单留空，则表示“除黑名单外全部放行”；白名单非空时，只允许解析白名单域名。</div>
      <div class="hint">这项规则只影响连到 ESP32 本地 AP 且把 ESP32 当成 DNS 的设备；命中黑名单或未命中白名单的域名会被拒绝。</div>
      <div class="actions">
        <button class="save-btn" onclick="saveDns()">保存 DNS 规则</button>
      </div>
      <div class="msg" id="dnsMsg"></div>
    </div>
    <div class="dns-panel">
      <div class="dns-stats-strip">
        <div class="dns-stat-chip">
          <span class="dns-stat-label">白名单数量</span>
          <span id="sDNSCount" class="dns-stat-value status-no">0</span>
        </div>
        <div class="dns-stat-chip">
          <span class="dns-stat-label">黑名单数量</span>
          <span id="sDNSBlockCount" class="dns-stat-value status-no">0</span>
        </div>
        <div class="dns-stat-chip">
          <span class="dns-stat-label">拦截总数</span>
          <span id="sDNSBlocked" class="dns-stat-value status-no">0</span>
        </div>
      </div>
      <div class="section-head">
        <div class="field-label">最近被拦截请求</div>
        <button class="ghost-btn small-btn" onclick="clearBlockedDns()">清空记录</button>
      </div>
      <div class="hint">这里按域名聚合统计被拦截次数，并按次数从小到大显示；设备重启后会清空。</div>
      <div class="saved-list" id="dnsBlockedList"></div>
    </div>
  </div>
</div>

<div class="card card-full card-ota">
  <div class="card-title">固件更新</div>
  <div class="ota-layout">
    <div class="ota-panel">
      <div class="ota-panel-title">自动更新</div>
      <div class="status-row"><span>当前固件版本</span><span id="sFirmwareVersion" class="status-ok status-text status-wide">--</span></div>
      <div class="status-row"><span>GitHub 最新版本</span><span id="sGitHubLatestVersion" class="status-no status-text status-wide">未检查</span></div>
      <div class="status-row"><span>更新包大小</span><span id="sGitHubAssetSize" class="status-no status-text status-wide">未检查</span></div>
      <div class="status-row"><span>下载进度</span><span id="sGitHubProgressText" class="status-no status-text status-wide">未开始</span></div>
      <div class="actions">
        <button class="save-btn" type="button" id="githubOtaBtn" onclick="doGitHubOTA()">检查更新</button>
      </div>
      <div class="progress" id="githubOtaProgWrap"><div class="progress-bar" id="githubOtaProgBar"></div></div>
      <div class="hint">设备默认从 GitHub Release 下载当前板型对应的 OTA 固件包更新，无需手动填写固件地址。</div>
      <div class="msg" id="otaGitHubMsg"></div>
    </div>
    <div class="ota-panel">
      <div class="ota-panel-title">手动更新</div>
      <div class="field-label">本地上传固件</div>
      <div class="ota-row">
        <label class="file-btn" for="fwFile">选择文件</label>
        <input type="file" id="fwFile" accept=".bin" style="display:none" onchange="fileChosen(this)">
        <span class="file-name" id="fileName">未选择文件</span>
      </div>
      <button class="upload-btn" id="uploadBtn" disabled onclick="doOTA()">上传固件</button>
      <div class="progress" id="progWrap"><div class="progress-bar" id="progBar"></div></div>
      <div class="hint">适用于本地调试或手动指定固件文件更新。</div>
      <div class="msg" id="otaUploadMsg"></div>
    </div>
  </div>
</div>

</div>
</div>

<div class="picker-modal" id="pickerModal" onclick="closePicker(event)">
  <div class="picker-sheet" onclick="event.stopPropagation()">
    <div class="picker-head">
      <div class="picker-title" id="pickerTitle">请选择</div>
      <button type="button" class="picker-close" onclick="closePicker()">&times;</button>
    </div>
    <div class="picker-body" id="pickerBody"></div>
  </div>
</div>

<div class="confirm-modal" id="confirmModal" onclick="closeConfirmDialog(false,event)">
  <div class="confirm-sheet" onclick="event.stopPropagation()">
    <div class="confirm-head">
      <div class="confirm-title" id="confirmTitle">请确认</div>
    </div>
    <div class="confirm-body" id="confirmMessage">确认执行当前操作吗？</div>
    <div class="confirm-actions">
      <button type="button" class="ghost-btn" onclick="closeConfirmDialog(false)">取消</button>
      <button type="button" class="save-btn confirm-confirm-btn" id="confirmOkBtn" onclick="closeConfirmDialog(true)">确认</button>
    </div>
  </div>
</div>

<div class="version-modal" id="versionModal" onclick="closeVersionDialog(event)">
  <div class="version-sheet" onclick="event.stopPropagation()">
    <div class="version-head">
      <div class="version-title">固件更新</div>
      <button type="button" class="picker-close" onclick="closeVersionDialog()">&times;</button>
    </div>
    <div class="version-body">
      <div class="status-row"><span>当前固件版本</span><span id="versionModalCurrent" class="status-ok status-text status-wide">--</span></div>
      <div class="status-row"><span>GitHub 最新版本</span><span id="versionModalLatest" class="status-no status-text status-wide">未检查</span></div>
      <div class="status-row"><span>更新包大小</span><span id="versionModalSize" class="status-no status-text status-wide">未检查</span></div>
      <div class="status-row"><span>下载进度</span><span id="versionModalProgress" class="status-no status-text status-wide">未开始</span></div>
      <div class="version-actions">
        <button type="button" class="save-btn" id="versionActionBtn" onclick="doGitHubOTA()">检查更新</button>
      </div>
      <div class="progress" id="versionGitHubProgWrap"><div class="progress-bar" id="versionGitHubProgBar"></div></div>
      <div class="hint">设备默认从 GitHub Release 下载当前板型对应的 OTA 固件包更新，无需手动填写固件地址。</div>
      <div class="msg" id="versionGitHubMsg"></div>
    </div>
  </div>
</div>

<div class="version-modal" id="hardwareInfoModal" onclick="closeHardwareInfoDialog(event)">
  <div class="version-sheet" onclick="event.stopPropagation()">
    <div class="version-head">
      <div class="version-title">硬件信息</div>
      <button type="button" class="picker-close" onclick="closeHardwareInfoDialog()">&times;</button>
    </div>
    <div class="version-body">
      <div class="status-row"><span>固件版本</span><span id="hardwareInfoFw" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>芯片型号</span><span id="hardwareInfoChipModel" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>芯片修订</span><span id="hardwareInfoChipRevision" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>核心数量</span><span id="hardwareInfoChipCores" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>CPU 频率</span><span id="hardwareInfoCpuFreq" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>Flash 大小</span><span id="hardwareInfoFlashSize" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>Flash 速度</span><span id="hardwareInfoFlashSpeed" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>程序大小</span><span id="hardwareInfoSketchSize" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>堆总量</span><span id="hardwareInfoHeapSize" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>当前空闲堆</span><span id="hardwareInfoFreeHeap" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>最小空闲堆</span><span id="hardwareInfoMinFreeHeap" class="status-no status-text status-wide">--</span></div>
      <div class="status-row"><span>PSRAM</span><span id="hardwareInfoPsramSize" class="status-no status-text status-wide">--</span></div>
    </div>
  </div>
</div>

<script>
let dnsDirty=false;
let apDirty=false;
let otaBusy=false;
let githubOtaCheckBusy=false;
let githubLatestVersion='';
let githubLatestAssetUrl='';
let githubLatestDownloadUrl='';
let githubLatestAssetSizeBytes=0;
let githubUpdateAvailable=false;
let githubOtaDownloading=false;
let githubOtaPendingVerify=false;
let githubOtaExpectedVersion='';
let githubOtaVerifyDeadline=0;
let currentFirmwareVersion='--';
let latestStatusData=null;
let scanResults=[];
let pendingScanResultsRender=false;
let latestBlockedDnsRequests=[];
let latestStatusUptime=0;
let activePickerId='';
let latestFsdEnableState=true;
let confirmResolver=null;
let confirmReturnFocus=null;
const speedOffsetBucketIds=['speedOffsetPct0','speedOffsetPct1','speedOffsetPct2','speedOffsetPct3','speedOffsetPct4','speedOffsetPct5','speedOffsetPct6','speedOffsetPct7','speedOffsetPct8','speedOffsetPct9'];
const speedOffsetPendingValues={};

function markDnsDirty(){
  dnsDirty=true;
  renderBlockedDnsRequests(latestBlockedDnsRequests,latestStatusUptime);
}

function markApDirty(){
  apDirty=true;
}

function setStatusText(id,text,className){
  const el=document.getElementById(id);
  if(!el)return;
  el.textContent=text;
  el.className=className+' status-text';
}

function setWideStatusText(id,text,className){
  const el=document.getElementById(id);
  if(!el)return;
  el.textContent=text;
  el.className=className+' status-text status-wide';
}

function setSummaryPill(id,text,className){
  const el=document.getElementById(id);
  if(!el)return;
  el.textContent=text;
  el.className='summary-pill-value '+className;
}

function setSpeedOffsetInlineValue(id,text,className){
  const el=document.getElementById(id);
  if(!el)return;
  el.textContent=text;
  el.className='speed-offset-inline-value '+className;
}

function setDnsStatValue(id,text,className){
  const el=document.getElementById(id);
  if(!el)return;
  el.textContent=text;
  el.className='dns-stat-value '+className;
}

function syncFsdToggleUI(enabled){
  const topToggle=document.getElementById('topFsdEnable');
  if(topToggle)topToggle.checked=!!enabled;
}

function syncHardwareInfoDialog(){
  const data=latestStatusData||{};
  setWideStatusText('hardwareInfoFw',data.fwVersion||'--',data.fwVersion?'status-ok':'status-no');
  setWideStatusText('hardwareInfoChipModel',data.chipModel||'--',data.chipModel?'status-ok':'status-no');
  setWideStatusText('hardwareInfoChipRevision',typeof data.chipRevision==='number'&&Number.isFinite(data.chipRevision)?('Rev '+String(data.chipRevision)):'--',typeof data.chipRevision==='number'&&Number.isFinite(data.chipRevision)?'status-ok':'status-no');
  setWideStatusText('hardwareInfoChipCores',typeof data.chipCores==='number'&&Number.isFinite(data.chipCores)?String(data.chipCores):'--',typeof data.chipCores==='number'&&Number.isFinite(data.chipCores)?'status-ok':'status-no');
  setWideStatusText('hardwareInfoCpuFreq',typeof data.cpuFreqMHz==='number'&&Number.isFinite(data.cpuFreqMHz)?(String(data.cpuFreqMHz)+' MHz'):'--',typeof data.cpuFreqMHz==='number'&&Number.isFinite(data.cpuFreqMHz)?'status-ok':'status-no');
  setWideStatusText('hardwareInfoFlashSize',typeof data.flashChipSize==='number'&&Number.isFinite(data.flashChipSize)?formatBytes(data.flashChipSize):'--',typeof data.flashChipSize==='number'&&Number.isFinite(data.flashChipSize)?'status-ok':'status-no');
  setWideStatusText('hardwareInfoFlashSpeed',typeof data.flashChipSpeedMHz==='number'&&Number.isFinite(data.flashChipSpeedMHz)?(String(data.flashChipSpeedMHz)+' MHz'):'--',typeof data.flashChipSpeedMHz==='number'&&Number.isFinite(data.flashChipSpeedMHz)?'status-ok':'status-no');
  setWideStatusText('hardwareInfoSketchSize',typeof data.sketchSize==='number'&&Number.isFinite(data.sketchSize)?formatBytes(data.sketchSize):'--',typeof data.sketchSize==='number'&&Number.isFinite(data.sketchSize)?'status-ok':'status-no');
  setWideStatusText('hardwareInfoHeapSize',typeof data.heapSize==='number'&&Number.isFinite(data.heapSize)?formatBytes(data.heapSize):'--',typeof data.heapSize==='number'&&Number.isFinite(data.heapSize)?'status-ok':'status-no');
  setWideStatusText('hardwareInfoFreeHeap',typeof data.freeHeap==='number'&&Number.isFinite(data.freeHeap)?formatBytes(data.freeHeap):'--',typeof data.freeHeap==='number'&&Number.isFinite(data.freeHeap)?'status-ok':'status-no');
  setWideStatusText('hardwareInfoMinFreeHeap',typeof data.minFreeHeap==='number'&&Number.isFinite(data.minFreeHeap)?formatBytes(data.minFreeHeap):'--',typeof data.minFreeHeap==='number'&&Number.isFinite(data.minFreeHeap)?'status-ok':'status-no');
  const psramText=typeof data.psramSize==='number'&&Number.isFinite(data.psramSize)&&data.psramSize>0?formatBytes(data.psramSize):'无';
  setWideStatusText('hardwareInfoPsramSize',psramText,typeof data.psramSize==='number'&&Number.isFinite(data.psramSize)&&data.psramSize>0?'status-ok':'status-no');
}

function syncToggleInput(inputOrId,enabled){
  const input=typeof inputOrId==='string'?document.getElementById(inputOrId):inputOrId;
  if(input)input.checked=!!enabled;
}

function saveBooleanSetting(key,enabled){
  return fetch('/api/set?'+key+'='+(enabled?'1':'0')).then(r=>{
    if(!r.ok)throw new Error('save failed');
    return r.text();
  });
}

function openConfirmDialog(options){
  if(confirmResolver){
    confirmResolver(false);
    confirmResolver=null;
  }

  const modal=document.getElementById('confirmModal');
  const title=document.getElementById('confirmTitle');
  const message=document.getElementById('confirmMessage');
  const okBtn=document.getElementById('confirmOkBtn');

  title.textContent=options&&options.title?options.title:'请确认';
  message.textContent=options&&options.message?options.message:'确认执行当前操作吗？';
  okBtn.textContent=options&&options.confirmText?options.confirmText:'确认';
  okBtn.className='save-btn confirm-confirm-btn';

  confirmReturnFocus=document.activeElement&&typeof document.activeElement.focus==='function'
    ? document.activeElement
    : null;

  modal.classList.add('open');
  setTimeout(()=>okBtn.focus(),0);

  return new Promise(resolve=>{
    confirmResolver=resolve;
  });
}

function closeConfirmDialog(confirmed,evt){
  const modal=document.getElementById('confirmModal');
  if(evt&&evt.target&&evt.target!==modal)return;
  if(!modal.classList.contains('open'))return;

  modal.classList.remove('open');

  const resolve=confirmResolver;
  confirmResolver=null;

  if(confirmReturnFocus&&typeof confirmReturnFocus.focus==='function'){
    setTimeout(()=>confirmReturnFocus.focus(),0);
  }
  confirmReturnFocus=null;

  if(resolve)resolve(!!confirmed);
}

async function confirmToggleAction(input,options){
  if(!input)return false;

  const sync=typeof options.sync==='function'
    ? options.sync
    : value=>syncToggleInput(input,value);
  const nextValue=!!input.checked;
  const previousValue=typeof options.getPreviousValue==='function'
    ? !!options.getPreviousValue()
    : !nextValue;

  if(nextValue===previousValue){
    sync(previousValue);
    return false;
  }

  const actionText=nextValue?'开启':'关闭';
  const label=options.label||'该开关';
  const confirmed=await openConfirmDialog({
    title:options.title||('确认切换'+label),
    message:typeof options.getMessage==='function'
      ? options.getMessage(nextValue,actionText)
      : '确定要'+actionText+label+'吗？',
    confirmText:actionText
  });

  if(!confirmed){
    sync(previousValue);
    if(typeof options.onCancel==='function')options.onCancel(previousValue,nextValue);
    return false;
  }

  if(typeof options.onBeforeApply==='function')options.onBeforeApply(nextValue,previousValue);
  else sync(nextValue);

  try{
    await options.apply(nextValue,previousValue);
    if(typeof options.onSuccess==='function')options.onSuccess(nextValue,previousValue);
    return true;
  }catch(err){
    sync(previousValue);
    if(typeof options.onError==='function')options.onError(err,nextValue,previousValue);
    return false;
  }
}

async function confirmConfigToggle(input,key,label){
  await confirmToggleAction(input,{
    label,
    title:'确认切换'+label,
    apply(nextValue){
      return saveBooleanSetting(key,nextValue).then(()=>{
        poll();
      });
    },
    onError(){
      poll();
    }
  });
}

function formatChipTemp(current,average){
  const currentValid=typeof current==='number'&&Number.isFinite(current);
  const avgValid=typeof average==='number'&&Number.isFinite(average);
  if(!currentValid&&!avgValid)return '--';
  if(currentValid&&avgValid)return current.toFixed(1)+'°C / 平均 '+average.toFixed(1)+'°C';
  if(currentValid)return current.toFixed(1)+'°C';
  return '平均 '+average.toFixed(1)+'°C';
}

function getSignalClass(rssi){
  if(typeof rssi!=='number'||!Number.isFinite(rssi))return 'status-no';
  if(rssi>=-55)return 'status-ok';
  if(rssi>=-67)return 'status-ok';
  if(rssi>=-75)return 'status-warn';
  return 'status-err';
}

function syncNetworkForm(d){
  if(!apDirty){
    document.getElementById('apSSID').value=d.apSSID||'';
    document.getElementById('apPass').value=d.apPassword||'';
  }
  document.getElementById('upstreamEnable').checked=!!d.upstreamEnable;
  const savedNetworks=Array.isArray(d.upstreamNetworks)?d.upstreamNetworks:[];
  let scanSavedStateChanged=false;
  scanResults.forEach(net=>{
    const nextSaved=savedNetworks.some(saved=>saved.ssid===net.ssid);
    if(net.saved!==nextSaved){
      net.saved=nextSaved;
      scanSavedStateChanged=true;
    }
  });
  if(scanSavedStateChanged){
    refreshScanResultsSelect();
  }
  renderSavedNetworks(savedNetworks);
}

function syncDnsForm(d){
  if(dnsDirty)return;
  document.getElementById('dnsWhitelistEnable').checked=!!d.dnsWhitelistEnable;
  document.getElementById('dnsAllowlist').value=d.dnsAllowlist||'';
  document.getElementById('dnsBlocklist').value=d.dnsBlocklist||'';
}

function syncOtaForm(d){
  currentFirmwareVersion=d.fwVersion||'--';
  setWideStatusText('sFirmwareVersion',currentFirmwareVersion,'status-ok');
  if(d.githubLatestDownloadUrl)githubLatestDownloadUrl=d.githubLatestDownloadUrl;
  syncGitHubOtaMetrics(Number(d.otaOnlineTotalBytes||0),Number(d.otaOnlineWrittenBytes||0));
  syncOnlineOTAState(d);
  syncGitHubOtaVerification();
  syncGitHubOtaButtons();
  syncVersionDialog();
}

function setNetMessage(text,type){
  const msg=document.getElementById('netMsg');
  msg.textContent=text;
  msg.className='msg'+(type?' '+type:'');
}

function setApMessage(text,type){
  const msg=document.getElementById('apMsg');
  msg.textContent=text;
  msg.className='msg'+(type?' '+type:'');
}

function normalizeDomain(domain){
  let normalized=(domain||'').trim().toLowerCase();
  while(normalized.endsWith('.'))normalized=normalized.slice(0,-1);
  return normalized;
}

function getDnsRulesFromTextarea(id){
  return (document.getElementById(id).value||'')
    .split(/[\s,;]+/)
    .map(normalizeDomain)
    .filter(Boolean);
}

function ruleMatchesDomain(domain,rule){
  if(!domain||!rule)return false;
  return domain===rule||(domain.length>rule.length&&domain.endsWith('.'+rule));
}

function isDomainAlreadyAllowed(domain){
  const normalizedDomain=normalizeDomain(domain);
  if(!normalizedDomain)return false;
  return getDnsRulesFromTextarea('dnsAllowlist').some(rule=>ruleMatchesDomain(normalizedDomain,rule));
}

function isDomainBlockedByBlacklist(domain){
  const normalizedDomain=normalizeDomain(domain);
  if(!normalizedDomain)return false;
  return getDnsRulesFromTextarea('dnsBlocklist').some(rule=>ruleMatchesDomain(normalizedDomain,rule));
}

function renderScanResults(){
  const select=document.getElementById('scanResults');
  const current=select.value;
  select.innerHTML='';

  if(!scanResults.length){
    const opt=document.createElement('option');
    opt.value='';
    opt.textContent='点击“搜索热点”查看附近可用热点';
    opt.disabled=true;
    select.appendChild(opt);
    syncPickerButton('scanResults');
    return;
  }

  const placeholder=document.createElement('option');
  placeholder.value='';
  placeholder.textContent='请选择要保存的热点';
  placeholder.disabled=true;
  select.appendChild(placeholder);

  scanResults.forEach(net=>{
    const opt=document.createElement('option');
    opt.value=net.ssid;
    let label=net.ssid;
    if(typeof net.rssi==='number')label+=' · '+net.rssi+' dBm';
    if(net.saved)label+=' · 已保存';
    opt.textContent=label;
    select.appendChild(opt);
  });

  if(scanResults.some(net=>net.ssid===current)){
    select.value=current;
  }
  syncPickerButton('scanResults');
}

// 将弹窗中选中的热点名称回填到手动输入框。
function applyScanResultSelection(){
  const select=document.getElementById('scanResults');
  const input=document.getElementById('upstreamSSID');
  if(!select||!input||!select.value)return;
  input.value=select.value;
  setNetMessage('已填入热点名称，请填写密码后保存','ok');
}

function refreshScanResultsSelect(force){
  const select=document.getElementById('scanResults');
  if(!force&&document.activeElement===select){
    pendingScanResultsRender=true;
    return;
  }
  pendingScanResultsRender=false;
  renderScanResults();
}

function appendTag(parent,text,className){
  const tag=document.createElement('span');
  tag.className='tag'+(className?' '+className:'');
  tag.textContent=text;
  parent.appendChild(tag);
}

function renderSavedNetworks(networks){
  const wrap=document.getElementById('savedNetworks');
  wrap.innerHTML='';

  if(!Array.isArray(networks)||!networks.length){
    const empty=document.createElement('div');
    empty.className='empty-box';
    empty.textContent='还没有保存热点。先搜索附近热点，再保存需要自动连接的候选热点。';
    wrap.appendChild(empty);
    return;
  }

  networks.forEach(net=>{
    const item=document.createElement('div');
    item.className='saved-item';

    const main=document.createElement('div');
    main.className='saved-main';

    const name=document.createElement('div');
    name.className='saved-name';
    name.textContent=net.ssid;
    main.appendChild(name);

    const tags=document.createElement('div');
    tags.className='saved-tags';
    if(net.connected)appendTag(tags,'已连接','ok');
    else if(net.active)appendTag(tags,'连接中','busy');
    else appendTag(tags,'已保存','');
    if(!net.hasPass)appendTag(tags,'无密码','warn');
    main.appendChild(tags);

    const delBtn=document.createElement('button');
    delBtn.className='ghost-btn small-btn';
    delBtn.textContent='删除';
    delBtn.onclick=()=>deleteSavedUpstream(net.ssid);

    item.appendChild(main);
    item.appendChild(delBtn);
    wrap.appendChild(item);
  });
}

function formatRelativeTime(seconds){
  const delta=Math.max(0,seconds||0);
  if(delta<60)return delta+'秒前';
  if(delta<3600)return Math.floor(delta/60)+'分前';
  if(delta<86400)return Math.floor(delta/3600)+'小时前';
  return Math.floor(delta/86400)+'天前';
}

function renderBlockedDnsRequests(requests,currentUptime){
  const wrap=document.getElementById('dnsBlockedList');
  wrap.innerHTML='';

  if(!Array.isArray(requests)||!requests.length){
    const empty=document.createElement('div');
    empty.className='empty-box';
    empty.textContent='暂时没有被拦截域名统计。';
    wrap.appendChild(empty);
    return;
  }

  requests.forEach(item=>{
    const row=document.createElement('div');
    row.className='saved-item';
    const normalizedDomain=normalizeDomain(item.domain);
    const blockedByBlacklist=isDomainBlockedByBlacklist(normalizedDomain);
    const alreadyAllowed=isDomainAlreadyAllowed(normalizedDomain);

    const main=document.createElement('div');
    main.className='saved-main';

    const name=document.createElement('div');
    name.className='saved-name';
    name.textContent=item.domain||'(未知域名)';
    main.appendChild(name);

    const tags=document.createElement('div');
    tags.className='saved-tags';
    appendTag(tags,String(item.count||0)+'次','warn');
    appendTag(tags,formatRelativeTime((currentUptime||0)-(item.lastBlockedAt||0)),'busy');
    main.appendChild(tags);

    const addBtn=document.createElement('button');
    addBtn.className='ghost-btn small-btn';
    addBtn.textContent=blockedByBlacklist?'已在黑名单':(alreadyAllowed?'已在白名单':'加入白名单');
    addBtn.disabled=blockedByBlacklist||alreadyAllowed||!normalizedDomain;
    addBtn.onclick=()=>addBlockedDomainToAllowlist(normalizedDomain);

    row.appendChild(main);
    row.appendChild(addBtn);
    wrap.appendChild(row);
  });
}

function poll(){
  fetch('/api/status').then(r=>r.json()).then(d=>{
    latestStatusData=d;
    document.getElementById('sModified').textContent=d.modified;
    document.getElementById('sRX').textContent=d.rx;
    document.getElementById('sErrors').textContent=d.errors;
    let u=d.uptime;
    let h=Math.floor(u/3600),m=Math.floor((u%3600)/60),s=u%60;
    document.getElementById('sUptime').textContent=h>0?h+'时'+m+'分':m>0?m+'分'+s+'秒':s+'秒';

    setSummaryPill('topCanState',d.canOK?'正常':'异常',d.canOK?'status-ok':'status-err');

    const hwModeText=String(d.hwMode)==='0'?'LEGACY':(String(d.hwMode)==='1'?'HW3':'HW4');
    document.getElementById('topHwModeLabel').textContent=hwModeText;
    currentFirmwareVersion=d.fwVersion||'--';
    document.getElementById('topHeaderVersion').textContent=currentFirmwareVersion;
    const speedProfileText=String(d.speedProfile)==='0'?'保守':(String(d.speedProfile)==='1'?'默认':(String(d.speedProfile)==='2'?'适中':(String(d.speedProfile)==='3'?'激进':'最大')));
    setSummaryPill('topHwMode',speedProfileText,'status-ok');

    const roadLimitValid=typeof d.roadSpeedLimit==='number'&&Number.isFinite(d.roadSpeedLimit);
    const visionLimitValid=typeof d.visionSpeedLimit==='number'&&Number.isFinite(d.visionSpeedLimit);
    let roadLimitText='--';
    if(roadLimitValid){
      roadLimitText=String(d.roadSpeedLimit);
      if(visionLimitValid&&d.visionSpeedLimit!==d.roadSpeedLimit){
        roadLimitText+=' / 视觉 '+String(d.visionSpeedLimit);
      }
    }
    const offsetValid=typeof d.activeSpeedOffsetPct==='number'&&Number.isFinite(d.activeSpeedOffsetPct);
    const resultValid=(typeof d.effectiveSpeedLimit==='number'&&Number.isFinite(d.effectiveSpeedLimit))||(typeof d.effectiveSpeedLimit==='string'&&d.effectiveSpeedLimit!=='');
    setSpeedOffsetInlineValue('speedOffsetRoadInline',roadLimitText,roadLimitValid?'status-ok':'status-no');
    setSpeedOffsetInlineValue('speedOffsetCurrentInline',offsetValid?('+'+String(d.activeSpeedOffsetPct)+'%'):'--',offsetValid?(d.activeSpeedOffsetPct>0?'status-ok':'status-no'):'status-no');
    setSpeedOffsetInlineValue('speedOffsetResultInline',resultValid?String(d.effectiveSpeedLimit):'--',resultValid?'status-ok':'status-no');

    const cpuUsageValid=typeof d.cpuUsagePct==='number'&&Number.isFinite(d.cpuUsagePct);
    let cpuUsageClass='status-ok';
    if(cpuUsageValid&&d.cpuUsagePct>=85)cpuUsageClass='status-err';
    else if(cpuUsageValid&&d.cpuUsagePct>=65)cpuUsageClass='status-warn';
    setSummaryPill('topCpuUsage',cpuUsageValid?(String(Math.round(d.cpuUsagePct))+'%'):'--',cpuUsageValid?cpuUsageClass:'status-no');

    let thermalClass='status-ok';
    if(d.thermalProtect)thermalClass='status-err';
    else if((d.thermalStatus||'').includes('降频')||(d.thermalStatus||'').includes('偏高'))thermalClass='status-warn';
    let signalClass=getSignalClass(d.upstreamRSSI);
    const currentTempValid=typeof d.chipTempC==='number'&&Number.isFinite(d.chipTempC);
    const avgTempValid=typeof d.chipTempAvgC==='number'&&Number.isFinite(d.chipTempAvgC);
    let topThermalText='--';
    if(currentTempValid&&avgTempValid)topThermalText=d.chipTempC.toFixed(1)+'°C';
    else if(currentTempValid)topThermalText=d.chipTempC.toFixed(1)+'°C';
    else if(avgTempValid)topThermalText='-- / '+d.chipTempAvgC.toFixed(1)+'°C';
    setSummaryPill('topThermalState',topThermalText,typeof d.chipTempC==='number'&&Number.isFinite(d.chipTempC)?thermalClass:'status-no');

    latestFsdEnableState=!!d.fsdEnable;
    syncFsdToggleUI(latestFsdEnableState);
    document.getElementById('hwMode').value=d.hwMode;
    document.getElementById('speedProfile').value=d.speedProfile;
    document.getElementById('profileMode').value=d.profileMode?'1':'0';
    document.getElementById('speedOffsetEnable').checked=!!d.speedOffsetEnable;
    document.getElementById('isaChime').checked=!!d.isaChime;
    document.getElementById('emergencyDet').checked=!!d.emergencyDet;
    document.getElementById('chinaMode').checked=!!d.chinaMode;

    const isHw3=String(d.hwMode)==='1';
    document.getElementById('speedOffsetEnable').disabled=!isHw3;
    const bucketValues=Array.isArray(d.speedOffsetBuckets)?d.speedOffsetBuckets:[];
    if(!isHw3){
      Object.keys(speedOffsetPendingValues).forEach(key=>delete speedOffsetPendingValues[key]);
    }
    speedOffsetBucketIds.forEach((id,index)=>{
      const el=document.getElementById(id);
      if(!el)return;
      syncSpeedOffsetInput(el,id,Number.isFinite(bucketValues[index])?bucketValues[index]:0);
      el.disabled=!isHw3;
    });
    document.querySelectorAll('.picker-native').forEach(select=>syncPickerButton(select.id));

    syncNetworkForm(d);
    syncDnsForm(d);
    syncOtaForm(d);
    setStatusText('sUpstream',d.upstreamStatus||'--',d.upstreamConnected?'status-ok':(d.upstreamEnable?'status-err':'status-no'));
    setWideStatusText('sCurrentUpstream',d.connectedUpstreamSSID||d.upstreamSSID||'--',d.upstreamConnected?'status-ok':'status-no');
    setSummaryPill('topUpstreamState',d.upstreamStatus||'--',d.upstreamConnected?'status-ok':(d.upstreamEnable?'status-warn':'status-no'));
    setStatusText('sSavedUpstreams',String(d.upstreamSavedCount||0),d.upstreamSavedCount?'status-ok':'status-no');
    setStatusText('sUpstreamRSSI',typeof d.upstreamRSSI==='number'&&Number.isFinite(d.upstreamRSSI)?String(d.upstreamRSSI)+' dBm':'--',signalClass);
    setStatusText('sUpstreamSignal',d.upstreamSignal||'--',signalClass);
    setStatusText('sWiFiChannel',d.wifiChannel?String(d.wifiChannel):'--',d.wifiChannel?'status-ok':'status-no');
    setStatusText('sAPClients',String(d.apClients||0),(d.apClients||0)>1?'status-warn':'status-ok');
    setStatusText('sUpstreamIP',d.upstreamIP||'--',d.upstreamConnected?'status-ok':'status-no');
    setStatusText('sNAT',d.natStatus||'--',d.natEnabled?'status-ok':(d.upstreamConnected?'status-err':'status-no'));
    setStatusText('sAP',d.apSSID||'--','status-ok');
    setStatusText('sAPIP',d.apIP||'--','status-ok');
    const dnsEnabled=!!d.dnsWhitelistEnable;
    const dnsSummaryText=dnsEnabled
      ? String(d.dnsWhitelistCount||0)+'/'+String(d.dnsBlacklistCount||0)
      : '未启用';
    setSummaryPill('topDnsRules',dnsSummaryText,dnsEnabled?'status-ok':'status-no');
    setDnsStatValue('sDNSCount',String(d.dnsWhitelistCount||0),d.dnsWhitelistCount?'status-ok':'status-no');
    setDnsStatValue('sDNSBlockCount',String(d.dnsBlacklistCount||0),d.dnsBlacklistCount?'status-err':'status-no');
    setDnsStatValue('sDNSBlocked',String(d.dnsBlockedCount||0),d.dnsBlockedCount?'status-err':'status-no');
    syncHardwareInfoDialog();
    latestBlockedDnsRequests=Array.isArray(d.dnsBlockedRequests)?d.dnsBlockedRequests:[];
    latestStatusUptime=d.uptime||0;
    renderBlockedDnsRequests(latestBlockedDnsRequests,latestStatusUptime);
  }).catch(()=>{});
}

function setVal(key,val){
  fetch('/api/set?'+key+'='+val).catch(()=>{});
}

function updateSpeedOffsetInputWidth(input){
  if(!input)return;
  const charCount=Math.max(4,String(input.value||'').length);
  input.style.setProperty('--input-chars',String(charCount));
}

function handleSpeedOffsetInput(index){
  updateSpeedOffsetInputWidth(document.getElementById('speedOffsetPct'+index));
}

function syncSpeedOffsetInput(el,id,nextValue){
  if(!el)return;
  if(document.activeElement===el)return;
  if(Object.prototype.hasOwnProperty.call(speedOffsetPendingValues,id)){
    if(speedOffsetPendingValues[id]===nextValue){
      delete speedOffsetPendingValues[id];
    }else{
      el.value=String(speedOffsetPendingValues[id]);
      updateSpeedOffsetInputWidth(el);
      return;
    }
  }
  el.value=String(nextValue);
  updateSpeedOffsetInputWidth(el);
}

function saveSpeedOffsetInput(index){
  const key='speedOffsetPct'+index;
  const input=document.getElementById(key);
  if(!input)return;
  let value=Number(input.value);
  if(!Number.isFinite(value))value=0;
  value=Math.round(value);
  if(value<0)value=0;
  if(value>50)value=50;
  input.value=String(value);
  updateSpeedOffsetInputWidth(input);
  speedOffsetPendingValues[key]=value;
  setVal(key,value);
}

async function confirmTopFsdToggle(input){
  await confirmToggleAction(input,{
    label:'FSD',
    title:'确认切换 FSD',
    sync:syncFsdToggleUI,
    getPreviousValue(){
      return latestFsdEnableState;
    },
    onBeforeApply(nextValue){
      latestFsdEnableState=nextValue;
      syncFsdToggleUI(nextValue);
    },
    apply(nextValue){
      return saveBooleanSetting('fsdEnable',nextValue).then(()=>{
        poll();
      });
    },
    onError(err,nextValue,previousValue){
      latestFsdEnableState=previousValue;
      poll();
    }
  });
}

function getSelectDisplayText(select){
  if(!select||!select.options.length)return '请选择';
  const option=select.options[select.selectedIndex>=0?select.selectedIndex:0];
  return option&&option.textContent?option.textContent:'请选择';
}

function syncPickerButton(selectId){
  const select=document.getElementById(selectId);
  if(!select)return;
  const btnId=select.dataset.pickerTrigger;
  if(!btnId)return;
  const btn=document.getElementById(btnId);
  if(!btn)return;
  btn.textContent=getSelectDisplayText(select);
  btn.disabled=!!select.disabled;
}

// 在底部弹层中展示搜索状态或空结果提示。
function showPickerMessage(title,message){
  activePickerId='';
  document.getElementById('pickerTitle').textContent=title||'请选择';
  const body=document.getElementById('pickerBody');
  body.innerHTML='';
  const empty=document.createElement('div');
  empty.className='empty-box';
  empty.textContent=message;
  body.appendChild(empty);
  document.getElementById('pickerModal').classList.add('open');
}

function openPicker(selectId){
  const select=document.getElementById(selectId);
  if(!select||select.disabled)return;
  activePickerId=selectId;
  document.getElementById('pickerTitle').textContent=select.dataset.pickerTitle||'请选择';
  const body=document.getElementById('pickerBody');
  body.innerHTML='';
  for(let i=0;i<select.options.length;i++){
    const option=select.options[i];
    if(option.disabled)continue;
    const btn=document.createElement('button');
    btn.type='button';
    btn.className='picker-option'+(option.value===select.value?' active':'');
    btn.textContent=option.textContent;
    btn.onclick=()=>choosePickerValue(selectId,option.value);
    body.appendChild(btn);
  }
  if(!body.children.length){
    const empty=document.createElement('div');
    empty.className='empty-box';
    empty.textContent='没有可选项';
    body.appendChild(empty);
  }
  document.getElementById('pickerModal').classList.add('open');
}

function choosePickerValue(selectId,value){
  const select=document.getElementById(selectId);
  if(!select)return;
  select.value=value;
  syncPickerButton(selectId);
  select.dispatchEvent(new Event('change',{bubbles:true}));
  closePicker();
}

function closePicker(evt){
  if(evt&&evt.target&&evt.target!==document.getElementById('pickerModal'))return;
  activePickerId='';
  document.getElementById('pickerModal').classList.remove('open');
}

function setUpstreamEnabled(enabled){
  setNetMessage('保存中...','');
  return saveBooleanSetting('upstreamEnable',enabled).then(()=>{
    setNetMessage(enabled?'热点接入已启用':'热点接入已关闭','ok');
    poll();
  }).catch(err=>{
    setNetMessage('保存失败','err');
    poll();
    throw err;
  });
}

async function confirmUpstreamToggle(input){
  await confirmToggleAction(input,{
    label:'上游热点接入',
    title:'确认切换上游热点接入',
    apply(nextValue){
      return setUpstreamEnabled(nextValue);
    }
  });
}

function saveApConfig(){
  const ssid=(document.getElementById('apSSID').value||'').trim();
  const pass=document.getElementById('apPass').value||'';
  if(!ssid){
    setApMessage('热点名称不能为空','err');
    return;
  }
  if(ssid.length>32){
    setApMessage('热点名称最多 32 个字符','err');
    return;
  }
  if(pass.length<8||pass.length>63){
    setApMessage('热点密码长度必须为 8-63 个字符','err');
    return;
  }
  const params=new URLSearchParams();
  params.set('apSSID',ssid);
  params.set('apPass',pass);
  setApMessage('保存中，热点将重新启动...','');
  fetch('/api/set?'+params.toString()).then(async r=>{
    if(!r.ok){
      const txt=await r.text();
      throw new Error(txt||'save failed');
    }
    return r.text();
  }).then(()=>{
    apDirty=false;
    setApMessage('本地热点设置已保存，约 1 秒后会切换到新的名称和密码','ok');
  }).catch(err=>{
    setApMessage(err.message||'保存失败','err');
  });
}

function scanUpstreamNetworks(){
  const btn=document.getElementById('scanBtn');
  btn.disabled=true;
  setNetMessage('搜索中...','');
  showPickerMessage('搜索附近上游热点','搜索中...');

  fetch('/api/upstream/scan').then(r=>{
    if(!r.ok){
      return r.text().then(t=>{throw new Error(t||'scan failed');});
    }
    return r.json();
  }).then(d=>{
    scanResults=Array.isArray(d.results)?d.results:[];
    refreshScanResultsSelect(true);
    if(scanResults.length){
      setNetMessage('已更新附近热点','ok');
      openPicker('scanResults');
    }else{
      setNetMessage('没有搜索到可用热点','ok');
      showPickerMessage('搜索附近上游热点','没有搜索到可用热点');
    }
  }).catch(err=>{
    setNetMessage(err.message||'搜索失败','err');
    showPickerMessage('搜索附近上游热点',err.message||'搜索失败');
  }).finally(()=>{
    btn.disabled=false;
  });
}

function saveSelectedUpstream(){
  const manualSSID=(document.getElementById('upstreamSSID').value||'').trim();
  const ssid=manualSSID||document.getElementById('scanResults').value;
  const pass=document.getElementById('upstreamPass').value;

  if(!ssid){
    setNetMessage('请先选择或输入要保存的热点','err');
    return;
  }

  if(ssid.length>32){
    setNetMessage('热点名称最多 32 个字符','err');
    return;
  }

  const params=new URLSearchParams();
  params.set('ssid',ssid);
  if(pass)params.set('pass',pass);

  setNetMessage('保存中...','');

  fetch('/api/upstream/add?'+params.toString()).then(r=>{
    if(!r.ok){
      return r.text().then(t=>{throw new Error(t||'save failed');});
    }
    return r.text();
  }).then(()=>{
    document.getElementById('upstreamSSID').value='';
    document.getElementById('upstreamPass').value='';
    setNetMessage(document.getElementById('upstreamEnable').checked?'热点已保存，设备会自动切换到可连接的热点':'热点已保存，启用后会自动连接','ok');
    poll();
  }).catch(err=>{
    setNetMessage(err.message||'保存失败','err');
  });
}

function deleteSavedUpstream(ssid){
  if(!window.confirm('确定删除这个已保存热点吗？'))return;

  setNetMessage('删除中...','');
  fetch('/api/upstream/delete?ssid='+encodeURIComponent(ssid)).then(r=>{
    if(!r.ok){
      return r.text().then(t=>{throw new Error(t||'delete failed');});
    }
    return r.text();
  }).then(()=>{
    setNetMessage('已删除热点','ok');
    poll();
  }).catch(err=>{
    setNetMessage(err.message||'删除失败','err');
  });
}

document.querySelectorAll('.picker-native').forEach(select=>syncPickerButton(select.id));

function persistDnsRules(successText){
  const msg=document.getElementById('dnsMsg');
  const params=new URLSearchParams();

  params.set('dnsWhitelistEnable',document.getElementById('dnsWhitelistEnable').checked?'1':'0');
  params.set('dnsAllowlist',document.getElementById('dnsAllowlist').value.trim());
  params.set('dnsBlocklist',document.getElementById('dnsBlocklist').value.trim());

  msg.textContent='保存中...';
  msg.className='msg';

  fetch('/api/set?'+params.toString()).then(r=>{
    if(!r.ok)throw new Error('save failed');
    return r.text();
  }).then(()=>{
    dnsDirty=false;
    msg.textContent=successText||'DNS 规则已保存';
    msg.className='msg ok';
    poll();
  }).catch(()=>{
    msg.textContent='保存失败';
    msg.className='msg err';
  });
}

function saveDns(){
  persistDnsRules('DNS 规则已保存');
}

async function confirmDnsToggle(input){
  const previousDirty=dnsDirty;
  await confirmToggleAction(input,{
    label:'DNS 规则',
    title:'确认切换 DNS 规则',
    apply(){
      markDnsDirty();
      return Promise.resolve();
    },
    onCancel(){
      dnsDirty=previousDirty;
      renderBlockedDnsRequests(latestBlockedDnsRequests,latestStatusUptime);
    }
  });
}

function addBlockedDomainToAllowlist(domain){
  const normalizedDomain=normalizeDomain(domain);
  const msg=document.getElementById('dnsMsg');

  if(!normalizedDomain){
    msg.textContent='域名无效，无法加入白名单';
    msg.className='msg err';
    return;
  }

  if(isDomainAlreadyAllowed(normalizedDomain)){
    msg.textContent='这个域名已经在白名单里了';
    msg.className='msg ok';
    renderBlockedDnsRequests(latestBlockedDnsRequests,latestStatusUptime);
    return;
  }

  if(isDomainBlockedByBlacklist(normalizedDomain)){
    msg.textContent='这个域名已在黑名单里，黑名单优先，请先移除黑名单规则';
    msg.className='msg err';
    renderBlockedDnsRequests(latestBlockedDnsRequests,latestStatusUptime);
    return;
  }

  const textarea=document.getElementById('dnsAllowlist');
  const current=textarea.value.trim();
  textarea.value=current?current+'\n'+normalizedDomain:normalizedDomain;
  dnsDirty=true;
  renderBlockedDnsRequests(latestBlockedDnsRequests,latestStatusUptime);
  persistDnsRules('已加入白名单: '+normalizedDomain);
}

function clearBlockedDns(){
  const msg=document.getElementById('dnsMsg');
  msg.textContent='清空中...';
  msg.className='msg';

  fetch('/api/dns/blocked/clear').then(r=>{
    if(!r.ok)throw new Error('clear failed');
    return r.text();
  }).then(()=>{
    msg.textContent='拦截记录已清空';
    msg.className='msg ok';
    poll();
  }).catch(()=>{
    msg.textContent='清空失败';
    msg.className='msg err';
  });
}

function setOtaMessage(id,text,type){
  const msg=document.getElementById(id);
  if(!msg)return;
  msg.textContent=text;
  msg.className='msg'+(type?' '+type:'');
}

function setGitHubOtaMessage(text,type){
  setOtaMessage('otaGitHubMsg',text,type);
  setOtaMessage('versionGitHubMsg',text,type);
}

function setUploadOtaMessage(text,type){
  setOtaMessage('otaUploadMsg',text,type);
}

function formatBytes(bytes){
  if(typeof bytes!=='number'||!Number.isFinite(bytes)||bytes<=0)return '--';
  const units=['B','KB','MB','GB'];
  let value=bytes;
  let index=0;
  while(value>=1024&&index<units.length-1){
    value/=1024;
    index++;
  }
  const digits=value>=100||index===0?0:(value>=10?1:2);
  const text=value.toFixed(digits).replace(/\.0+$|(\.\d*[1-9])0+$/,'$1');
  return text+' '+units[index];
}

function setMirroredWideStatusText(primaryId,secondaryId,text,className){
  setWideStatusText(primaryId,text,className);
  setWideStatusText(secondaryId,text,className);
}

function setMirroredProgress(primaryWrapId,primaryBarId,secondaryWrapId,secondaryBarId,visible,percent){
  const wrapIds=[primaryWrapId,secondaryWrapId];
  const barIds=[primaryBarId,secondaryBarId];
  wrapIds.forEach((wrapId,index)=>{
    const wrap=document.getElementById(wrapId);
    const bar=document.getElementById(barIds[index]);
    if(!wrap||!bar)return;
    wrap.style.display=visible?'block':'none';
    bar.style.width=(visible?String(percent):'0')+'%';
  });
}

function syncGitHubOtaMetrics(totalBytes,writtenBytes){
  const totalValid=typeof totalBytes==='number'&&Number.isFinite(totalBytes)&&totalBytes>0;
  const writtenValid=typeof writtenBytes==='number'&&Number.isFinite(writtenBytes)&&writtenBytes>=0;
  const effectiveTotal=totalValid?totalBytes:(githubLatestAssetSizeBytes>0?githubLatestAssetSizeBytes:0);
  const sizeText=effectiveTotal>0?formatBytes(effectiveTotal):'未检查';
  setMirroredWideStatusText('sGitHubAssetSize','versionModalSize',sizeText,effectiveTotal>0?'status-ok':'status-no');

  let progressText='未开始';
  let progressClass='status-no';
  let progressPercent=0;
  let showProgress=false;

  if(totalValid){
    const safeWritten=Math.min(Math.max(writtenBytes||0,0),totalBytes);
    progressPercent=Math.min(100,Math.round(safeWritten/totalBytes*100));
    progressText=formatBytes(safeWritten)+' / '+formatBytes(totalBytes)+' · '+String(progressPercent)+'%';
    progressClass=safeWritten>=totalBytes&&totalBytes>0?'status-ok':'status-warn';
    showProgress=true;
  }else if(githubOtaDownloading){
    progressText='连接中，等待设备返回包大小';
    progressClass='status-warn';
  }

  setMirroredWideStatusText('sGitHubProgressText','versionModalProgress',progressText,progressClass);
  setMirroredProgress('githubOtaProgWrap','githubOtaProgBar','versionGitHubProgWrap','versionGitHubProgBar',showProgress,progressPercent);
}

function getPreferredGitHubOtaUrl(){
  return githubLatestAssetUrl||githubLatestDownloadUrl||'';
}

function resetGitHubOtaState(clearBusy){
  githubOtaDownloading=false;
  githubOtaPendingVerify=false;
  githubOtaExpectedVersion='';
  githubOtaVerifyDeadline=0;
  if(clearBusy){
    setOtaBusy(false);
  }else{
    syncGitHubOtaButtons();
  }
}

function syncGitHubOtaVerification(){
  if(!githubOtaPendingVerify)return;
  if(githubOtaExpectedVersion&&currentFirmwareVersion===githubOtaExpectedVersion){
    githubUpdateAvailable=false;
    githubOtaPendingVerify=false;
    githubOtaExpectedVersion='';
    githubOtaVerifyDeadline=0;
    setWideStatusText('sGitHubLatestVersion',currentFirmwareVersion+' · 已最新','status-ok');
    setGitHubOtaMessage('设备已更新到 '+currentFirmwareVersion,'ok');
    setOtaBusy(false);
    return;
  }
  if(githubOtaVerifyDeadline>0&&Date.now()>githubOtaVerifyDeadline){
    const expectedVersion=githubOtaExpectedVersion||githubLatestVersion||'--';
    githubOtaPendingVerify=false;
    githubOtaExpectedVersion='';
    githubOtaVerifyDeadline=0;
    setGitHubOtaMessage('设备已重连，但当前仍是 '+(currentFirmwareVersion||'--')+'，目标版本 '+expectedVersion,'err');
    setOtaBusy(false);
  }
}

function syncOnlineOTAState(d){
  const inProgress=!!d.otaOnlineInProgress;
  const state=String(d.otaOnlineState||'idle');
  const message=d.otaOnlineMessage||'';
  const totalBytes=Number.isFinite(Number(d.otaOnlineTotalBytes))?Number(d.otaOnlineTotalBytes):0;
  const writtenBytes=Number.isFinite(Number(d.otaOnlineWrittenBytes))?Number(d.otaOnlineWrittenBytes):0;

  syncGitHubOtaMetrics(totalBytes,writtenBytes);

  if(inProgress){
    if(!githubOtaDownloading){
      githubOtaDownloading=true;
      setOtaBusy(true);
    }
    setGitHubOtaMessage(message||'正在下载并写入固件，请勿断电...','');
    return;
  }

  if(githubOtaDownloading&&githubOtaExpectedVersion&&currentFirmwareVersion===githubOtaExpectedVersion){
    githubOtaDownloading=false;
    githubUpdateAvailable=false;
    setGitHubOtaMessage('设备已更新到 '+currentFirmwareVersion,'ok');
    setOtaBusy(false);
    return;
  }

  if(state==='success'){
    if(githubOtaDownloading){
      githubOtaDownloading=false;
      githubOtaPendingVerify=true;
      githubOtaVerifyDeadline=Date.now()+90000;
      syncGitHubOtaButtons();
    }
    if(message)setGitHubOtaMessage(message,'ok');
    return;
  }

  if(state==='error'){
    if(githubOtaDownloading||githubOtaPendingVerify){
      resetGitHubOtaState(true);
    }
    if(message)setGitHubOtaMessage(message,'err');
    return;
  }

  if(githubOtaDownloading&&githubOtaVerifyDeadline>0&&Date.now()>githubOtaVerifyDeadline){
    resetGitHubOtaState(true);
    setGitHubOtaMessage('在线更新超时，请重试','err');
  }
}

function syncGitHubOtaButtons(){
  const githubBtn=document.getElementById('githubOtaBtn');
  const versionActionBtn=document.getElementById('versionActionBtn');

  if(githubBtn){
    if(githubOtaCheckBusy){
      githubBtn.textContent='正在检查更新';
      githubBtn.disabled=true;
    }else if(githubOtaDownloading){
      githubBtn.textContent='正在下载新版本';
      githubBtn.disabled=true;
    }else if(githubOtaPendingVerify){
      githubBtn.textContent='等待设备重启';
      githubBtn.disabled=true;
    }else{
      githubBtn.textContent=githubUpdateAvailable&&getPreferredGitHubOtaUrl()?'一键更新':'检查更新';
      githubBtn.disabled=!!otaBusy;
    }
  }

  if(versionActionBtn){
    if(githubOtaCheckBusy){
      versionActionBtn.textContent='正在检查更新';
    }else if(githubOtaDownloading){
      versionActionBtn.textContent='正在一键更新';
    }else if(githubOtaPendingVerify){
      versionActionBtn.textContent='等待设备重启';
    }else{
      versionActionBtn.textContent=githubUpdateAvailable&&getPreferredGitHubOtaUrl()?'一键更新':'检查更新';
    }
    versionActionBtn.disabled=!!otaBusy||githubOtaCheckBusy||githubOtaDownloading||githubOtaPendingVerify;
  }

  syncVersionDialog();
}

function syncOtaUploadButton(){
  const fileInput=document.getElementById('fwFile');
  const uploadBtn=document.getElementById('uploadBtn');
  if(!uploadBtn)return;
  uploadBtn.disabled=otaBusy||!(fileInput&&fileInput.files&&fileInput.files[0]);
}

function setOtaBusy(busy){
  otaBusy=!!busy;
  const fileInput=document.getElementById('fwFile');
  if(fileInput)fileInput.disabled=otaBusy;
  syncOtaUploadButton();
  syncGitHubOtaButtons();
}

function syncVersionDialog(){
  const currentSource=document.getElementById('sFirmwareVersion');
  const latestSource=document.getElementById('sGitHubLatestVersion');
  const currentTarget=document.getElementById('versionModalCurrent');
  const latestTarget=document.getElementById('versionModalLatest');

  if(currentTarget){
    currentTarget.textContent=currentSource?currentSource.textContent:(currentFirmwareVersion||'--');
    currentTarget.className=currentSource?currentSource.className:'status-ok status-text status-wide';
  }

  if(latestTarget){
    latestTarget.textContent=latestSource?latestSource.textContent:(githubLatestVersion||'未检查');
    latestTarget.className=latestSource?latestSource.className:'status-no status-text status-wide';
  }
}

function openVersionDialog(){
  syncVersionDialog();
  document.getElementById('versionModal').classList.add('open');
}

function closeVersionDialog(evt){
  const modal=document.getElementById('versionModal');
  if(evt&&evt.target&&evt.target!==modal)return;
  modal.classList.remove('open');
}

function openHardwareInfoDialog(){
  syncHardwareInfoDialog();
  document.getElementById('hardwareInfoModal').classList.add('open');
}

function closeHardwareInfoDialog(evt){
  const modal=document.getElementById('hardwareInfoModal');
  if(evt&&evt.target&&evt.target!==modal)return;
  modal.classList.remove('open');
}

function fileChosen(inp){
  document.getElementById('fileName').textContent=inp.files[0]?inp.files[0].name:'未选择文件';
  setUploadOtaMessage('','');
  syncOtaUploadButton();
}

async function checkGitHubLatest(showMessage){
  if(githubOtaCheckBusy)return null;
  githubOtaCheckBusy=true;
  syncGitHubOtaButtons();
  if(showMessage){
    setGitHubOtaMessage('正在检查 GitHub 最新版本...','');
  }

  try{
    const response=await fetch('/api/ota/github/latest');
    const text=await response.text();
    if(!response.ok)throw new Error(text||'检查失败');
    const data=JSON.parse(text);
    githubLatestVersion=data.latestVersion||'';
    githubLatestAssetUrl=data.assetUrl||'';
    githubLatestDownloadUrl=data.latestDownloadUrl||'';
    githubLatestAssetSizeBytes=Number.isFinite(Number(data.assetSizeBytes))?Number(data.assetSizeBytes):0;
    githubUpdateAvailable=!!(data.updateAvailable&&getPreferredGitHubOtaUrl());
    syncGitHubOtaMetrics(0,0);

    if(data.latestVersion){
      const statusText=data.latestVersion+(data.updateAvailable?' · 可更新':' · 已最新');
      setWideStatusText('sGitHubLatestVersion',statusText,data.updateAvailable?'status-warn':'status-ok');
    }else{
      setWideStatusText('sGitHubLatestVersion','未找到发布版本','status-no');
    }

    if(showMessage){
      setGitHubOtaMessage(data.updateAvailable?('发现 GitHub 新版本: '+data.latestVersion):'当前已经是 GitHub 最新版','ok');
    }
    return data;
  }catch(err){
    githubUpdateAvailable=false;
    setWideStatusText('sGitHubLatestVersion','检查失败','status-err');
    if(showMessage){
      setGitHubOtaMessage(err.message||'检查失败','err');
    }
    return null;
  }finally{
    githubOtaCheckBusy=false;
    syncGitHubOtaButtons();
  }
}

function doOTA(){
  let file=document.getElementById('fwFile').files[0];
  if(!file)return;
  let xhr=new XMLHttpRequest();
  let prog=document.getElementById('progWrap');
  let bar=document.getElementById('progBar');
  prog.style.display='block';
  bar.style.width='0%';
  setUploadOtaMessage('','');
  setOtaBusy(true);
  xhr.upload.addEventListener('progress',e=>{if(e.lengthComputable)bar.style.width=Math.round(e.loaded/e.total*100)+'%';});
  xhr.onload=function(){
    if(xhr.status===200&&xhr.responseText==='OK'){setUploadOtaMessage('上传成功，正在重启...','ok');}
    else{setUploadOtaMessage('上传失败: '+(xhr.responseText||xhr.statusText||xhr.status),'err');setOtaBusy(false);}
  };
  xhr.onerror=function(){setUploadOtaMessage('连接失败','err');setOtaBusy(false);};
  let form=new FormData();
  form.append('firmware',file);
  xhr.open('POST','/api/ota');
  xhr.send(form);
}

function startOnlineOTARequest(url,pendingMessage){
  document.getElementById('progWrap').style.display='none';
  githubOtaDownloading=true;
  githubOtaPendingVerify=false;
  githubOtaVerifyDeadline=Date.now()+180000;
  setOtaBusy(true);
  syncGitHubOtaButtons();
  setGitHubOtaMessage(pendingMessage||'正在在线下载并写入固件，请勿断电...','');
  syncGitHubOtaMetrics(0,0);

  return fetch('/api/ota/online?url='+encodeURIComponent(url)).then(async r=>{
    const text=await r.text();
    if(!r.ok)throw new Error(text||'在线更新失败');
    setGitHubOtaMessage('在线更新任务已开始，正在后台下载并写入固件...','');
    poll();
  }).catch(err=>{
    resetGitHubOtaState(true);
    setGitHubOtaMessage(err.message||'在线更新失败','err');
    poll();
  });
}

function startGitHubLatestOTARequest(pendingMessage){
  document.getElementById('progWrap').style.display='none';
  githubOtaDownloading=true;
  githubOtaPendingVerify=false;
  githubOtaVerifyDeadline=Date.now()+180000;
  setOtaBusy(true);
  syncGitHubOtaButtons();
  setGitHubOtaMessage(pendingMessage||'正在从 GitHub 下载并更新固件，请勿断电...','');
  syncGitHubOtaMetrics(0,0);

  return fetch('/api/githubota/start').then(async r=>{
    const text=await r.text();
    if(!r.ok)throw new Error(text||'在线更新失败');
    setGitHubOtaMessage('在线更新任务已开始，正在后台下载并写入固件...','');
    poll();
  }).catch(err=>{
    resetGitHubOtaState(true);
    setGitHubOtaMessage(err.message||'在线更新失败','err');
    poll();
  });
}

async function doGitHubOTA(){
  if(!githubUpdateAvailable){
    const data=await checkGitHubLatest(true);
    if(!data)return;
    if(!data.updateAvailable){
      setGitHubOtaMessage('当前已经是 GitHub 最新版','ok');
      return;
    }
    return;
  }

  const targetUrl=getPreferredGitHubOtaUrl();
  if(!targetUrl){
    setGitHubOtaMessage('GitHub 最新版缺少固件下载地址','err');
    return;
  }

  const confirmed=await openConfirmDialog({
    title:'确认更新 GitHub 最新版',
    message:'当前版本 '+(currentFirmwareVersion||'--')+'，GitHub 最新版本 '+(githubLatestVersion||'--')+'。确定开始更新吗？',
    confirmText:'更新'
  });
  if(!confirmed)return;

  githubOtaExpectedVersion=githubLatestVersion||'';
  startGitHubLatestOTARequest('正在从 GitHub 下载并更新固件，请勿断电...');
}

async function restartDevice(){
  const confirmed=await openConfirmDialog({
    title:'确认重启',
    message:'确定现在重启吗？当前连接会短暂断开。',
    confirmText:'重启'
  });
  if(!confirmed)return;
  const buttons=[document.getElementById('topRestartBtn')].filter(Boolean);
  buttons.forEach(btn=>{btn.disabled=true;});
  setGitHubOtaMessage('正在重启设备...','ok');
  setUploadOtaMessage('','');
  fetch('/api/restart',{method:'POST'}).catch(()=>{}).finally(()=>{
    setTimeout(()=>{buttons.forEach(btn=>{btn.disabled=false;});},5000);
  });
}

document.addEventListener('keydown',event=>{
  if(event.key==='Escape'){
    closeConfirmDialog(false);
    closeVersionDialog();
  }
});

setOtaBusy(false);
syncGitHubOtaButtons();
speedOffsetBucketIds.forEach(id=>updateSpeedOffsetInputWidth(document.getElementById(id)));
setInterval(poll,1000);
poll();
</script>
</body>
</html>
)rawliteral";
