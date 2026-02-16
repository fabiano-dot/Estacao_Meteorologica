function toInt8(value) {
  return value & 0x80 ? value - 0x100 : value;
}


function readUInt16(bytes, idx) {
  return (bytes[idx] << 8) | bytes[idx + 1];
}

function toInt16(value) {
  return value & 0x8000 ? value - 0x10000 : value;
}

function readInt32(bytes, idx) {
  let value =
    (bytes[idx] << 24) |
    (bytes[idx + 1] << 16) |
    (bytes[idx + 2] << 8) |
    (bytes[idx + 3]);

  // força signed 32 bits
  return value | 0;
}


function decodeUplink(input) {
  const bytes = input.bytes;
  let index = 0;
  let data = {};

  // =====================
  // Byte de Controle
  // =====================
  const bc = bytes[index++] & 0xFF;
  data.BC = "0x" + bc.toString(16).padStart(2, "0").toUpperCase();

  // =====================
  // Bit 0 – Battery
  // =====================
  if (bc & 0x01) {
    const bat = bytes[index++] & 0xFF;
    if (bat === 0xFF) {
      data.Battery = "ERROR";
    } else {
      data.Battery = bat * 0.5; // %
    }
  }

  // =====================
  // Bit 1 – BME280
  // =====================
  if (bc & 0x02) {
    // Umidade
    const humidity = bytes[index++] & 0xFF;
    if (humidity === 0xFF) {
      data.Humidity = "ERROR";
    } else {
      data.Humidity = humidity * 0.5; // %
    }

    // Temperatura (signed int16)
    const tempRaw = readUInt16(bytes, index);
    index += 2;
    if (tempRaw === 0x7FFF) {
      data.Temperature = "ERROR";
    } else {
      data.Temperature = toInt16(tempRaw) / 100.0; // °C
    }

    // Pressão (signed int16, mesma lógica do Java)
    const presRaw = readUInt16(bytes, index);
    index += 2;
    if (presRaw === -1) { // 0xFFFF
      data.Pressure = "ERROR";
    } else {
      data.Pressure = (60000 - toInt16(presRaw)) * 0.02; // hPa
    }
  }

  // =====================
  // Bit 2 – GPS
  // =====================
  if (bc & 0x04) {
    // Latitude
    const latRaw = readInt32(bytes, index);
    index += 4;
    if (latRaw === 0x7FFFFFFF) {
      data.Latitude = "ERROR";
    } else {
      data.Latitude = latRaw / 8388608.0; // graus
    }

    // Longitude
    const lonRaw = readInt32(bytes, index);
    index += 4;
    if (lonRaw === 0x7FFFFFFF) {
      data.Longitude = "ERROR";
    } else {
      data.Longitude = lonRaw / 8388608.0; // graus
    }

    // Altitude
    const altRaw = readUInt16(bytes, index);
    index += 2;
    if (altRaw === 0x7FFF) {
      data.Altitude = "ERROR";
    } else {
      data.Altitude = toInt16(altRaw) * 0.1; // metros
    }
  }
  
 // =====================
  // Bit 3 – Lux Meter
  // =====================
  if (bc & 0x08) {
    const luminosity = readUInt16(bytes, index);
    index += 2;
    if (luminosity === 0xFFFF) {
      data.Luminosity = "ERROR";
    } else {
      data.Luminosity = luminosity * 4; // lux
    }
  }
  

  // =====================
  // Bit 6 – Vsys
  // =====================
  if (bc & 0x40) {
    const vsys = bytes[index++] & 0xFF;
    if (vsys === 0xFF) {
      data.Vsys = "ERROR";
    } else {
      data.Vsys = vsys * 20; // mV
    }
  }  
  
  // =====================
  // Bit 7 – CPU Temp
  // =====================
  if (bc & 0x80) {
    const mpu_temp = bytes[index++] & 0xFF;
    if (mpu_temp === 0x7F) {
      data.MpuTemperature = "ERROR";
    } else {
      data.MpuTemperature = toInt8(mpu_temp); // °C
    }
  }
  
  const agora = new Date();
  data.AqTimestamp_ms  = agora.getTime();         // Data/hora em milissegundos desde o Epoch (01/01/1970 UTC)
  data.AqTimestamp_iso = agora.toISOString();     // Data/hora em string no formato ISO 8601

  return {
    data: data
  };
}
