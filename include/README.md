```wifi_config.h
#pragma once

#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"
```

```credentials.h
#pragma once

#define DEVICE_ID "your_device_id_on_soracom_eg_d-1xxxxxxxxxxxxxxxxxxx"
#define DEVICE_SECRET_KEY_LEN 16
const char DEVICE_SECRET_KEY_BYTE_ARRAY[DEVICE_SECRET_KEY_LEN] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x10, 0x11,
    0x12, 0x13, 0x14, 0x15,};

#define DEVICE_SECRET_KEY DEVICE_SECRET_KEY_BYTE_ARRAY
```
