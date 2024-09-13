#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "DHT.h"

// Definicije za pin in naprave
#define DHT_PIN GPIO_NUM_4         // DHT11 na GPIO 4
#define PUMP_PIN GPIO_NUM_22       // Črpalka na GPIO 22
#define RELAY_PIN GPIO_NUM_21      // Rele na GPIO 21
#define MOISTURE_SENSOR_PIN ADC1_CHANNEL_6  // GPIO34 je ADC kanal 6

// Inicializacija DHT11 senzorja
DHT dht;

// Funkcija za inicializacijo GPIO
void init_gpio() {
    gpio_pad_select_gpio(PUMP_PIN);
    gpio_set_direction(PUMP_PIN, GPIO_MODE_OUTPUT);
    
    gpio_pad_select_gpio(RELAY_PIN);
    gpio_set_direction(RELAY_PIN, GPIO_MODE_OUTPUT);
}

// Funkcija za inicializacijo ADC za senzor vlage
void init_adc() {
    adc1_config_width(ADC_WIDTH_BIT_12);   // 12-bitna ločljivost
    adc1_config_channel_atten(MOISTURE_SENSOR_PIN, ADC_ATTEN_DB_0);
}

// Funkcija za branje vlage (analogni vhod)
int read_moisture_sensor() {
    return adc1_get_raw(MOISTURE_SENSOR_PIN);  // Vrednost od 0 do 4095
}

// Glavna zanka
void app_main() {
    init_gpio();  // Inicializacija GPIO
    init_adc();   // Inicializacija ADC
    dht_init(DHT_PIN, DHT11);  // Inicializacija DHT senzorja

    while (1) {
        // Branje temperature z DHT11
        float temperature = dht_read_temperature(DHT_PIN);

        // Branje vrednosti senzorja vlage
        int moisture_value = read_moisture_sensor();ž

        // Če je vlaga prenizka, vklopi črpalko
        if (moisture_value < 3300) {
            gpio_set_level(PUMP_PIN, 1);  // Vklopi črpalko
            printf("Črpalka vklopljena\n");
            vTaskDelay(15000 / portTICK_PERIOD_MS);  // Počakaj 15 sekund
            gpio_set_level(PUMP_PIN, 0);  // Izklopi črpalko
            printf("Črpalka izklopljena\n");
        }

        // Če je temperatura previsoka, izklopi rele
        if (temperature > 45.0) {
            gpio_set_level(RELAY_PIN, 1);  // Izklopi rele
            printf("Rele vklopljen\n");
        } else {
            gpio_set_level(RELAY_PIN, 0);  // Vklopi rele (normalno stanje)
            printf("Rele izklopljen\n");
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);  // Počakaj 5 sekund
    }
}
