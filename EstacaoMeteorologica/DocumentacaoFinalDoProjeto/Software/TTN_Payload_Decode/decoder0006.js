// ********** Payload Decoder: Build: 0006 18/02/2026 **********

/* **************************************************************************** 
*                                                                             *
*               ----- Descrição do Payload -----                              *
*                                                                             *
* - O Primeiro byte, byte de controle BC, descreve os bytes seguintes.        *
*   Cada um dos bits do BC representa a presença ou não de um valor no        *
*   payload:                                                                  *
*     bit 0 ==> Battery,  1 byte (Deprecated)                                 *
*     bit 1 ==> BME280,   5 bytes                                             *
*     bit 2 ==> GPS,     10 bytes                                             *
*     bit 3 ==> BH1750,   2 bytes                                             *
*     bit 4 ==> Reserved                                                      *
*     bit 5 ==> Reserved                                                      *
*     bit 6 ==> VSys,     1 byte                                              *
*     bit 7 ==> MPU Temp, 1 byte                                              *
*                                                                             *
* - Tamanho e descrição de cada valor:                                        *
*   - Bit 0 – Battery (Deprecated)                                            *
*     - Utiliza um byte                                                       *
*     - Corresponde a carga da bateria em %, passos de 0,5%                   *
*     - Valor real = byte * 0.5                                               *
*     - Quando o byte for 0xFF ==> ERRO                                       *
*                                                                             *
*   - Bit 1 – BME280                                                          *
*     - Utiliza 5 bytes, sendo:                                               *
*       - Byte  0                  ==> Umidade, passos de 0.5%                *
*         Quando o byte for 0xFF   ==> ERRO                                   *
*       - Bytes 1 e 2              ==> Temperatura, passos de 0,01 °C         *
*         Quando a word for 0x7FFF ==> ERRO                                   *
*       - Bytes 3 e 4              ==> Pressão, passos de 0,02 hPa            *
*         Quando a word for 0xFFFF ==> ERRO                                   *
*                                                                             *
*   - Bit 2 – GPS                                                             *
*     - Utiliza 10 bytes, sendo:                                              *
*       - Bytes 0 a 3              ==> Latitude,  passos de 1 / (2^23) graus  *
*         Quando a long word for 0x7FFFFFFF ==> ERRO                          *
*       - Bytes 4 a 7              ==> Longitude, passos de 1 / (2^23) graus  *
*         Quando a long word for 0x7FFFFFFF ==> ERRO                          *
*       - Bytes 8 a 9              ==> Altitude,  passos de 0,1 metro         *
*         Quando a word for 0x7FFF ==> ERRO                                   *
*                                                                             *
*   - Bit 3 – Lux Meter                                                       *
*     - Utiliza 2 bytes, sendo:                                               *
*       - Bytes 0 e 1              ==> luminosidade, passos de 4 lux          *
*         Quando a word for 0xFFFF ==> ERRO                                   *
*                                                                             *
*   - Bit 4 – Reserved                                                        *
*                                                                             *
*   - Bit 5 – Reserved                                                        *
*                                                                             *
*   - Bit 6 – Vsys                                                            *
*     - Utiliza um byte                                                       *
*     - Corresponde a tensão VSys, passos de 20 mV                            *
*     - Valor real(em mV) = byte * 20                                         *
*     - Quando o byte for 0xFF ==> ERRO                                       *
*                                                                             *
*   - Bit 7 – CPU Temp                                                        *
*     - Utiliza um byte                                                       *
*     - Corresponde a temperatura da MPU, passos de 1 Celsius                 *
*     - Valor real = byte tratado como signed                                 *
*     - Quando o byte for 0x7F ==> ERRO                                       *
*                                                                             *
***************************************************************************** */


// ***** Rotinas Auxiliares *****
// Converte um valor uint8 para int8
function toInt8(value) {
  return value & 0x80 ? value - 0x100 : value;
}

// Lê 2 bytes do array do payload, a partir do índice fornecido por idx, e converte em um uint16
function readUInt16(bytes, idx) {
  return (bytes[idx] << 8) | bytes[idx + 1];
}

// Converte um valor uint16 para int16
function toInt16(value) {
  return value & 0x8000 ? value - 0x10000 : value;
}

// Lê 4 bytes do array do payload, a partir do índice fornecido por idx, e converte em um int32
function readInt32(bytes, idx) {
  let value =
    (bytes[idx] << 24) |
    (bytes[idx + 1] << 16) |
    (bytes[idx + 2] << 8) |
    (bytes[idx + 3]);

  // força signed 32 bits
  return value | 0;
}


// ***** Rotina Principal de decodificação *****
// Recebe um array com o payload, input, e retorna uma variável, data, com cada um dos parâmetros convertidos
function decodeUplink(input) {
  const bytes = input.bytes;
  let index = 0;
  let data = {};

  // =====================
  // Lê o Byte de Controle
  //
  // Cada um dos bits representa a presença ou não de um valor no payload
  // bit 0 ==> Battery,  1 byte (Deprecated)
  // bit 1 ==> BME280,   5 bytes
  // bit 2 ==> GPS,     10 bytes
  // bit 3 ==> BH1750,   2 bytes
  // bit 4 ==> Reserved
  // bit 5 ==> Reserved
  // bit 6 ==> VSys,     1 byte
  // bit 7 ==> MPU Temp, 1 byte   
  // =====================
  const bc = bytes[index++] & 0xFF;
  data.BC = "0x" + bc.toString(16).padStart(2, "0").toUpperCase();

  // =====================
  // Bit 0 – Battery (Deprecated)
  // - Utiliza um byte
  // - Corresponde a carga da bateria em %, passos de 0,5%
  // - Valor real = byte * 0.5 
  // - Quando o byte for 0xFF ==> ERRO 
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
  // - Utiliza 5 bytes, sendo:
  //   - Byte  0                  ==> Umidade, passos de 0.5%
  //     Quando o byte for 0xFF   ==> ERRO 
  //   - Bytes 1 e 2              ==> Temperatura, passos de 0,01 °C
  //     Quando a word for 0x7FFF ==> ERRO 
  //   - Bytes 3 e 4              ==> Pressão, passos de 0,02 hPa
  //     Quando a word for 0xFFFF ==> ERRO 
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
    //if (presRaw === -1) { // 0xFFFF
    if (presRaw === 0xFFFF) { // 0xFFFF
      data.Pressure = "ERROR";
    } else {
      data.Pressure = (60000 - toInt16(presRaw)) * 0.02; // hPa
    }
  }

  // =====================
  // Bit 2 – GPS
  // - Utiliza 10 bytes, sendo:
  //   - Bytes 0 a 3                       ==> Latitude,  passos de 1 / (2^23) graus
  //     Quando a long word for 0x7FFFFFFF ==> ERRO
  //   - Bytes 4 a 7                       ==> Longitude, passos de 1 / (2^23) graus
  //     Quando a long word for 0x7FFFFFFF ==> ERRO 
  //   - Bytes 8 a 9                       ==> Altitude,  passos de 0,1 metro
  //     Quando a word for 0x7FFF          ==> ERRO 
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
  // - Utiliza 2 bytes, sendo:
  //   - Bytes 0 e 1              ==> luminosidade, passos de 4 lux
  //     Quando a word for 0xFFFF ==> ERRO 
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
  // - Utiliza um byte
  // - Corresponde a tensão VSys, passos de 20 mV
  // - Valor real(em mV) = byte * 20
  // - Quando o byte for 0xFF ==> ERRO 
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
  // - Utiliza um byte
  // - Corresponde a temperatura da MPU, passos de 1 Celsius
  // - Valor real = byte tratado como signed 
  // - Quando o byte for 0x7F ==> ERRO 
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



// ===================== TESTES =====================
const input = {
  bytes: [
    0xCF,                   // BC (Battery + BME + GPS + Lux + Vsys + CPUTemp)
    0xAF,                   // Battery = 175      → 87,5%
    0x6C,                   // Umidade = 108      → 54,0%
    0x09, 0xEF,             // Temperatura = 2543 → 25,43 °C
    0x24, 0x79,             // Pressão     = 9337 → 1013,26 hPa
    0x00, 0x80, 0x00, 0x00, // Latitude
    0x00, 0x40, 0x00, 0x00, // Longitude
    0x1D, 0x08,             // Altitude  = 7432   → 743,2 m
    0xD4, 0X31,             // Luximetro = 54321
    0xC8,                   // Vsys      = 200    → 4000 mV
    0xE9                    // CPU Temp  = 233    → -23 °C
  ]
};

const input2 = {
  bytes: [
    0xC7,                   // BC (Battery + BME + GPS + Vsys + CPUTemp)
    0x99,                   // Battery     = 153                 →   76,5%
    0x45,                   // Umidade     =  69                 →   34,5%
    0x09, 0xEF,             // Temperatura = 2543                →   25,43 °C
    0x24, 0x7A,             // Pressão     = 9338                → 1013,24 hPa
    0xF4, 0x97, 0x6A, 0x7B, // Latitude    = -191403397          →  -22,817063
    0xE8, 0x77, 0x12, 0x09, // Longitude   = -394849782 E877120A →  -47,069762
    0x10, 0xE1,             // Altitude    = 7432                →  743,2 m
    0xFE,                   // Vsys        = 254                 →  5080 mV
    0x7E                    // CPU Temp    =  126                →  126 °C
  ]
};


const input3 = {
  bytes: [
    0xC7,                   // BC (Battery + BME + GPS + Vsys + CPUTemp)
    0x99,                   // Battery     = 153                 →   76,5%
    0x45,                   // Umidade     =  69                 →   34,5%
    0x09, 0xEF,             // Temperatura = 2543                →   25,43 °C
    0x24, 0x7A,             // Pressão     = 9338                → 1013,24 hPa
    0xF4, 0x97, 0x6A, 0x7B, // Latitude    = -191403397          →  -22,817063
    0xE8, 0x77, 0x12, 0x09, // Longitude   = -394849782 E877120A →  -47,069762
    0x10, 0xE1,             // Altitude    = 7432                →  743,2 m
    0x32,                   // Vsys        =   50                →  1000 mV
    0x80                    // CPU Temp    =  128                →  -127 °C
  ]
};


const input4 = {
  bytes: [
    0xCE,                   // BC (BME + GPS + Vsys + Lux + CPUTemp)
    0xFF,                   // Umidade     = erro
    0x7F, 0xFF,             // Temperatura = erro
    0xFF, 0xFF,             // Pressão     = erro
    0x7F, 0xFF, 0xFF, 0xFF, // Latitude    = erro
    0x7F, 0xFF, 0xFF, 0xFF, // Longitude   = erro
    0x7F, 0xFF,             // Altitude    = erro
    0xFF, 0xFF,             // Luximetro   = erro
    0xFF,                   // Vsys        = erro
    0x7F                    // CPU Temp    = erro
  ]
};

const hex = input.bytes
  .map(b => b.toString(16).padStart(2, "0"))
  .join(" ");
console.log("Input (hex):", hex);
const result = decodeUplink(input);
console.log(JSON.stringify(result, null, 2));


const hex2 = input2.bytes
  .map(b => b.toString(16).padStart(2, "0"))
  .join(" ");
console.log("Input2 (hex):", hex2);
const result2 = decodeUplink(input2);
console.log(JSON.stringify(result2, null, 2));

const hex3 = input3.bytes
  .map(b => b.toString(16).padStart(2, "0"))
  .join(" ");
console.log("Input3 (hex):", hex3);
const result3 = decodeUplink(input3);
console.log(JSON.stringify(result3, null, 2));

const hex4 = input4.bytes
  .map(b => b.toString(16).padStart(2, "0"))
  .join(" ");
console.log("Input4 (hex):", hex4);
const result4 = decodeUplink(input4);
console.log(JSON.stringify(result4, null, 2));
