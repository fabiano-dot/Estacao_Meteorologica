function toNum(v) {
  if (v === undefined || v === null) return null;
  if (typeof v === "string") {
    var s = v.trim().toLowerCase();
    if (!s || s === "error" || s === "nan" || s === "null") return null;
  }
  var n = Number(v);
  return isFinite(n) ? n : null;
}

function setIfValidNumber(obj, key, val) {
  if (val === null) return;
  obj[key] = val;
}

// Parse strings if needed
if (typeof msg.uplink_message === "string") {
  try { msg.uplink_message = JSON.parse(msg.uplink_message); } catch (e) {}
}

if (typeof msg.end_device_ids === "string") {
  try { msg.end_device_ids = JSON.parse(msg.end_device_ids); } catch (e) {}
}

var upl = msg.uplink_message || {};
var payload = upl.decoded_payload || null;

if (typeof payload === "string") {
  try { payload = JSON.parse(payload); } catch (e) {}
}

// Si no hay payload, no mandes telemetría
if (!payload) {
  return { msg: {}, metadata: metadata, msgType: msgType };
}

var devId = (msg.end_device_ids && msg.end_device_ids.device_id)
  ? msg.end_device_ids.device_id
  : "unknown";

var newMsg = {};

// --- Temperature / Humidity ---
var t = (payload.Temperature !== undefined) ? toNum(payload.Temperature)
      : (payload.Temperatura !== undefined) ? toNum(payload.Temperatura)
      : null;
setIfValidNumber(newMsg, "temperature", t);

var h = (payload.Humidity !== undefined) ? toNum(payload.Humidity)
      : (payload.Umidade !== undefined) ? toNum(payload.Umidade)
      : null;
setIfValidNumber(newMsg, "humidity", h);

// --- GPS (solo si válido, para no romper el widget de mapa) ---
var lat = toNum(payload.Latitude);
var lon = toNum(payload.Longitude);
if (lat !== null && lon !== null && lat >= -90 && lat <= 90 && lon >= -180 && lon <= 180) {
  newMsg.latitude = lat;
  newMsg.longitude = lon;
}

// --- Otros campos numéricos ---
setIfValidNumber(newMsg, "Vsys", toNum(payload.Vsys));
setIfValidNumber(newMsg, "pressure", toNum(payload.Pressure));
setIfValidNumber(newMsg, "altitude", toNum(payload.Altitude));
setIfValidNumber(newMsg, "luminosity", toNum(payload.Luminosity));
setIfValidNumber(newMsg, "mpu_temperature", toNum(payload.MpuTemperature));
setIfValidNumber(newMsg, "aq_timestamp_ms", toNum(payload.AqTimestamp_ms));

// --- Strings ---
if (payload.AqTimestamp_iso !== undefined && payload.AqTimestamp_iso !== null) {
  newMsg.aq_timestamp_iso = String(payload.AqTimestamp_iso);
}
if (payload.BC !== undefined && payload.BC !== null && payload.BC !== "") {
  newMsg.bc = String(payload.BC);
}

// --- RSSI / SNR (TTN v3: rx_metadata) ---
newMsg.rssi = 0;
newMsg.snr = 0;

var meta = upl.rx_metadata;
if (meta && meta.length > 0) {
  var r = Number(meta[0].rssi);
  var s = Number(meta[0].snr);
  newMsg.rssi = isFinite(r) ? r : 0;
  newMsg.snr  = isFinite(s) ? s : 0;
}

// --- LoRa 
var settings = upl.settings;
if (typeof settings === "string") {
  try { settings = JSON.parse(settings); } catch (e) { settings = null; }
}

if (settings && settings.data_rate && settings.data_rate.lora) {
  var lora = settings.data_rate.lora;

  var bw = toNum(lora.bandwidth);
  if (bw !== null) newMsg.lora_bandwidth = bw;

  var sf = toNum(lora.spreading_factor);
  if (sf !== null) newMsg.lora_sf = sf;

  if (lora.coding_rate !== undefined && lora.coding_rate !== null && String(lora.coding_rate).trim() !== "") {
    newMsg.lora_cr = String(lora.coding_rate);
  }
}

// --- FCnt ---
newMsg.fcnt = (upl.f_cnt !== undefined && upl.f_cnt !== null) ? Number(upl.f_cnt) : 0;

// --- IDs ---
newMsg.device_id = devId;

var newMetadata = Object.assign({}, metadata);
newMetadata.deviceName = devId;
newMsg.device_name = newMetadata.deviceName;

// MÉTRICAS DERIVADAS 

var T = newMsg.temperature;  
var RH = newMsg.humidity;    

// Solo calcular si tenemos temperatura Y humedad válidas
if (T !== null && T !== undefined && RH !== null && RH !== undefined) {
  
  // ───────────────────────────────────────────────────────
  // 1. TEMPERATURA DE ROCÍO (Dew Point)
  // ───────────────────────────────────────────────────────
  var a = 17.27;
  var b = 237.7;
  var alpha = ((a * T) / (b + T)) + Math.log(RH / 100.0);
  var Td = (b * alpha) / (a - alpha);
  
  newMsg.dew_point = parseFloat(Td.toFixed(2));
  
  // Dew Point Depression (diferencia con temperatura actual)
  newMsg.dew_point_depression = parseFloat((T - Td).toFixed(2));
  
  // Clasificación de riesgo de condensación
  var dpd = T - Td;
  if (dpd < 2) {
    newMsg.condensation_risk = "ALTO";
  } else if (dpd < 5) {
    newMsg.condensation_risk = "MEDIO";
  } else {
    newMsg.condensation_risk = "BAIXO";
  }
  
  // ───────────────────────────────────────────────────────
  // 2. VPD - DÉFICIT DE PRESIÓN DE VAPOR
  // ───────────────────────────────────────────────────────
  // Presión de vapor saturado (kPa)
  var SVP = 0.6108 * Math.exp((17.27 * T) / (T + 237.3));
  
  // Presión de vapor actual (kPa)
  var AVP = SVP * (RH / 100.0);
  
  // VPD (kPa)
  var VPD = SVP - AVP;
  newMsg.vpd = parseFloat(VPD.toFixed(3));
  
  // Clasificación VPD para plantas/agricultura
  if (VPD < 0.4) {
    newMsg.vpd_category = "Muito Úmido";
  } else if (VPD < 0.8) {
    newMsg.vpd_category = "Baixo";
  } else if (VPD < 1.2) {
    newMsg.vpd_category = "Ótimo";
  } else if (VPD < 1.6) {
    newMsg.vpd_category = "Alto";
  } else {
    newMsg.vpd_category = "Muito Alto";
  }
}

return {
  msg: newMsg,
  metadata: newMetadata,
  msgType: msgType
};