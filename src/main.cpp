#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <lwm2m/connect.h>

#define VERBOSE
#ifdef VERBOSE
#define _v(...) Serial.printf(__VA_ARGS__);
#else
#define _v(...)
#endif

#include "credentials.h"
#include "wifi_config.h"
char coap_uri[] = "coaps://18.179.186.202:5684";

#define shortServerId 123
LwM2MConnect context("connect_soracom_inventory_device");

void lwm2m_reboot() {
    _v("Handled heap memory outage\n");
    ESP.restart();
}

inline void setupDeviceInformation() {
    context.deviceInstance.manufacturer = "some manufacturer";
    context.deviceInstance.model_name = "the model";
    context.deviceInstance.device_type = "led";
    context.deviceInstance.firmware_ver = "1.0";
    context.deviceInstance.serial_number = "140234-645235-12353";
}

#ifdef VERBOSE
#include "lwm2m_message.h"
bool late_connected;
int last_state;
int last_error;
#endif

//** Analog Inputの追加
#include "3202_custom.h"
KnownObjects::id3202::object analogInputObject;
KnownObjects::id3202::instance analogInputInstance;

KnownObjects::id3202::ApplicationTypeType applicationType;
#define ANALOG_INPUT_INSTANCE_ID 0
#define APPLICATION_TYPE "Random values"

bool minmaxResetted = true;
void updateAnalogValue() {
    // アナログ値としてランダムな値を返す
    // 実運用ではセンサーからの出力などを用いる
    double value = (random(20000) - 10000) / 100.0f;
    _v("value is %f\n", value);
    analogInputInstance.AnalogInputCurrentValue = value;
    analogInputObject.resChanged(CTX(context),
                                 analogInputInstance.id,
                                 (uint16_t)KnownObjects::id3202::RESID::AnalogInputCurrentValue);

    if (minmaxResetted) {
        _v("Min, Max update\n");
        minmaxResetted = false;
        analogInputInstance.MinMeasuredValue = value;
        analogInputInstance.MaxMeasuredValue = value;
        analogInputObject.resChanged(CTX(context),
                                     analogInputInstance.id,
                                     (uint16_t)KnownObjects::id3202::RESID::MinMeasuredValue);
        analogInputObject.resChanged(CTX(context),
                                     analogInputInstance.id,
                                     (uint16_t)KnownObjects::id3202::RESID::MaxMeasuredValue);
    } else {
        if (value < analogInputInstance.MinMeasuredValue) {
            _v("Min update\n");
            analogInputInstance.MinMeasuredValue = value;
            analogInputObject.resChanged(CTX(context),
                                         analogInputInstance.id,
                                         (uint16_t)KnownObjects::id3202::RESID::MinMeasuredValue);

        } else if (value > analogInputInstance.MaxMeasuredValue) {
            _v("Max update\n");
            analogInputInstance.MaxMeasuredValue = value;
            analogInputObject.resChanged(CTX(context),
                                         analogInputInstance.id,
                                         (uint16_t)KnownObjects::id3202::RESID::MaxMeasuredValue);
        }
    }
}

uint16_t count = 0;

//**//

void setup() {
#ifdef M5STACK
    M5.begin(true, false, false, false);
#endif

#ifdef VERBOSE
    Serial.begin(76800);
#endif

    // Reboot on heap memory outage
    std::set_new_handler(lwm2m_reboot);

    setupDeviceInformation();

    //using Executable = std::add_pointer<void(Lwm2mObjectInstance* instance, lwm2m_context_t* context)>::type;
    analogInputObject.verifyWrite = [](KnownObjects::id3202::instance* instance, uint16_t resource_id) {
        // SORACOM Inventoryからデータを受信したときの処理を実装する
        if (instance->id == ANALOG_INPUT_INSTANCE_ID) {
            // Objectスキーマでオペレーションが"W"riteで定義されているものを実装する
            // Writeリクエストを容認したときは true, 拒否するときは false を返す。
            switch ((KnownObjects::id3202::RESID)resource_id) {
                case KnownObjects::id3202::RESID::ApplicationType:
                    _v("Not supported change application type: %s\n", (char*)instance->ApplicationType.data);
                    return false;
            }
        }

        return false;
    };
    analogInputInstance.id = ANALOG_INPUT_INSTANCE_ID;
    strcpy((char*)applicationType.data, APPLICATION_TYPE);
    analogInputInstance.ApplicationType = applicationType;
    analogInputInstance.MaxRangeValue = 100.0f;
    analogInputInstance.MinRangeValue = -100.0f;
    analogInputInstance.ResetMinandMaxMeasuredValues = [](Lwm2mObjectInstance*, lwm2m_context_t*) {
        _v("Min, max histories are resetted\n");
        minmaxResetted = true;
    };

    analogInputObject.addInstance(CTX(context), &analogInputInstance);
    analogInputObject.registerObject(CTX(context), false);

    // Wait for network to connect
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    _v("WiFi connecting ");

    int wifi_process_count = 0;
    while (WiFi.status() != WL_CONNECTED) {
        _v(wifi_process_count / 5 & 0x1 ? "." : "_");
        if (wifi_process_count++ >= 10) wifi_process_count = 0;
        delay(500);
    }
    _v(" connected\n");

    // Init lwm2m network
    if (!context.socket_count()) {
        _v("Failed to open socket\n");
        while (1) ;
    }

    _v("Add server, ");
    bool result_add_server = context.add_server(shortServerId, coap_uri, 100, false);
    _v(result_add_server ? "Success\n" : "Failed\n");

    // Enter your DTLS secured connection information
    _v("Use DTLS, ");
    bool result_use_dtls_psk = context.use_dtls_psk(shortServerId, DEVICE_ID, DEVICE_SECRET_KEY, DEVICE_SECRET_KEY_LEN);
    _v(result_use_dtls_psk ? "Success\n" : "Failed\n");

    delay(1000);

#ifdef VERBOSE
    last_state = context.context.state;
    last_error = context.context.lastStepError;
    late_connected = context.is_connected();

    _v("Server uri: %s\n", context.get_server_uri(shortServerId));
    _v("Socket count: %d\n", context.socket_count());
    _v("Connection: %s\n", late_connected ? "Established" : "Disconnected");
    _v("State: %s\n", __state[context.context.state]);
    _v("Error: %s\n", __error[context.context.lastStepError]);

    _v("\n-------- First process -------\n");
    context.process();

    uint16_t port;
    char* host;
    _v("TargetP: %p\n", context.context.serverList);
    _v("secObjInstID: 0x%8x\n", context.context.serverList->secObjInstID);
    security_instance_t* secInst = lwm2m_get_security_object(&context.context, context.context.serverList->secObjInstID);
    _v("secInst: %p\n", secInst);
    _v("Connecting to %s on %i\r\n", secInst->uri, secInst->shortID);
    _v("decodeUri: %d\n", decode_uri(coap_uri, &host, &port));

    _v("Host: %s, Port: %d\n", host, port);

    _v("Security Mode: %d\n", secInst->securityMode);

    network_t* network = (network_t*)context.context.userData;
    _v("Network Type: %d\n", network->type);

    ip4_addr_t addr;
    int aton = ip4addr_aton(host, &addr);
    _v("AtoN: %d, %lu\n", aton, addr.addr);
    connection_t* connection = internal_connection_create(network, host, port);
    _v("Connection: %p\n", connection);

    _v("\n");

    delay(1000);
#endif
}

void loop() {
#ifdef VERBOSE
    if (last_state != context.context.state) {
        _v("State: %s -> %s\n", __state[last_state], __state[context.context.state]);
        last_state = context.context.state;
    }
    if (context.context.lastStepError) {
        _v("Error: %s\n", __error[context.context.lastStepError]);
        last_error = context.context.lastStepError;
    }
    bool connected = context.is_connected();
    if (late_connected != connected) {
        late_connected = connected;
        _v("Connection change to: %s\n", late_connected ? "Established" : "Disconnected");
    }
#endif

    context.process();

    delay(100);

    // サーバーとの接続が確立したらデータを送信し、Deep Sleepに入る
    if (context.is_connected()) {
        _v("Update analog value\n");
        updateAnalogValue();

        delay(200);
        lwm2m_network_close(CTX(context));
        ESP.deepSleep(10 * 1000 * 1000);

	   delay(100);
    }
}
