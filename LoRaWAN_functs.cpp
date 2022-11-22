#include "LoRaWAN_functs.h"

// pointer set by initLoRaWAN() to be used by lorawanJoinedHandler() to start timer that sends payloads
SoftwareTimer *timer_to_start_on_join = nullptr;

// LoRaWan parameters & callbacks used in initLoRaWAN()
lmh_param_t lora_init_params;
lmh_callback_t lora_init_callbacks;

// forward declarations
static void lorawanJoinedHandler(void);
static void lorawanJoinedFailedHandler(void);
static void lorawanRXHandler(lmh_app_data_t *app_data);

bool initLoRaWAN(uint8_t *appEUI, uint8_t *deviceEUI, uint8_t *appKey, uint8_t tx_power) {
    Serial.println("Initialising LoRaWAN...");

    // Initialize LoRa chip.
    uint32_t ret = lora_rak4630_init(); // function return code
    if (ret != 0) {
        Serial.println("lora_rak4630_init failed");
        return false;
    }

    /**
     * Set the OTAA EUIs and Keys
     * OTAA_KEY's must be defined locally in a separate file called "OTAA_keys.h".
     * See README for further instructions.
     */
    lmh_setAppEui(appEUI);
    lmh_setDevEui(deviceEUI);
    lmh_setAppKey(appKey);

    // Fill the init params and callback structs for passing to lmh_init()
    lora_init_params = { LORAWAN_ADR_OFF, LORAWAN_DEFAULT_DATARATE, LORAWAN_PUBLIC_NETWORK, LORAWAN_JOIN_TRIALS,
                         tx_power,        LORAWAN_DUTYCYCLE_OFF };
    lora_init_callbacks.BoardGetBatteryLevel = BoardGetBatteryLevel;
    lora_init_callbacks.BoardGetUniqueId = BoardGetUniqueId;
    lora_init_callbacks.BoardGetRandomSeed = BoardGetRandomSeed;
    lora_init_callbacks.lmh_RxData = lorawanRXHandler;
    lora_init_callbacks.lmh_has_joined = lorawanJoinedHandler;
    lora_init_callbacks.lmh_has_joined_failed = lorawanJoinedFailedHandler;

    // Initialize LoRaWan
    ret = lmh_init(&lora_init_callbacks, lora_init_params, true, loraClass, loraRegion);
    if (ret != 0) {
       Serial.println("lmh_init failed");
        return false;
    }

    return true;
}

bool initLoRaWAN(SoftwareTimer *timer, uint8_t *appEUI, uint8_t *deviceEUI, uint8_t *appKey, uint8_t tx_power) {
    // Save the timer so it can be started later by lorawanJoinedHandler()
    timer_to_start_on_join = timer;
    // Then init LoRaWAN
    return initLoRaWAN(appEUI, deviceEUI, appKey, tx_power);
}

// used by sendLoRaWANFrame() for logging
uint32_t count = 0;
uint32_t count_fail = 0;

void sendLoRaWANFrame(lmh_app_data_t *lora_app_data) {
    if (!isLoRaWANConnected()) {
        Serial.println("Device has not joined the network. Try again later.");
        return;
    }

    //log(LOG_LEVEL::DEBUG, "Sending payload frame now...");
    // lmh_error_status ret = lmh_send(lora_app_data, loraConfirm);
    // if (ret == LMH_SUCCESS) {
    //     count++;
    //     Serial.println("lmh_send ok count ");
    //     Serial.println(count);
    // } else {
    //     count_fail++;
    //     Serial.println("lmh_send fail count ");
    //     Serial.println(count_fail);}
}

/**
 * @brief LoRa function for handling HasJoined event.
 * Sends LoRa class change and starts app timer to send the payload periodically.
 */
void lorawanJoinedHandler(void) {
    //log(LOG_LEVEL::INFO, "Network Joined!");
    if (setLoRaWANClass()) {
        // if given a SoftwareTimer in initLoRaWAN
        if (timer_to_start_on_join != NULL) {
            timer_to_start_on_join->start();
        }
    }
    delay(1000); // This ensures the log message is printed
}

/**
 * @brief LoRa function for handling OTAA join failed.
 */
void lorawanJoinedFailedHandler(void) {
      Serial.println("OTAA join failed!");
      Serial.println( "Check your EUI's and Keys's!");
    Serial.println("Check if a Gateway is in range!");
    delay(1000); // This ensures the log messages are printed
}

/**
 * @brief Function for handling LoRaWan received data from Gateway.
 * As we're not expecting any RX the app_data is just logged for now.
 * @param app_data  Pointer to rx data
 */
void lorawanRXHandler(lmh_app_data_t *app_data) {
    Serial.println("LoRa Packet received");
    delay(1000); // This ensures the log message is printed
}
