#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <avr/pgmspace.h>
#include "fastReadWrite.h"
#include "fastADC.h"
#include "pinAssignments.h"
#include "channels.h"
#include "rx5808spi.h"


#define OLED_RESET 4 // not used / nicht genutzt bei diesem Display
Adafruit_SSD1306 display(OLED_RESET);

#define DRAW_DELAY 118
#define D_NUM 47

#define NAV_LEFT 2
#define NAV_MIDDLE 3
#define NAV_RIGHT 4

// rx5808 module needs >20ms to tune.
#define MIN_TUNE_TIME 25

// number of analog rssi reads to average for the current check.
// single analog read with FASTADC defined (see below) takes ~20us on 16MHz arduino
// so e.g. 10 reads will take 200 ms, which gives resolution of 5 RSSI reads per ms,
// this means that we can theoretically have 1ms timing accuracy :)
#define RSSI_READS 5 // 5 should give about 10 000 readings per second

//----- RSSI --------------------------------------
#define FILTER_ITERATIONS 5
uint16_t rssi_read_buf_a[FILTER_ITERATIONS + 1];
uint16_t rssiThreshold_a = 0;

uint16_t rssi_read_buf_b[FILTER_ITERATIONS + 1];
uint16_t rssiThreshold_b = 0;


#define RSSI_MAX 1024
#define RSSI_MIN 0
#define MAGIC_THRESHOLD_REDUCE_CONSTANT 2
#define THRESHOLD_ARRAY_SIZE  100
uint16_t rssiThresholdArray[THRESHOLD_ARRAY_SIZE];

#define MAX_RSSI_MONITOR_DELAY_CYCLES 1000 //each 100ms, if cycle takes 100us

//----- Voltage monitoring -------------------------
#define VOLTAGE_READS 3 //get average of VOLTAGE_READS readings

// analog readings less than VOLTAGE_ZERO_THRESHOLD value won't be sent.
// This way entire chorus device will send voltages only from devices that are attached to LiPo
// So if single Solo device has LiPo attached, then broadcast voltage request to
// entire Chorus device will produce a single voltage response.
#define VOLTAGE_ZERO_THRESHOLD 100

uint16_t voltage = 0;


#define MAX_BAND_IDX 5


//----- Lap timings--------------------------------
uint32_t lastMilliseconds = 0;
#define MIN_MIN_LAP_TIME 1 //seconds
#define MAX_MIN_LAP_TIME 60 //seconds
uint8_t minLapTime = 5; //seconds
#define MAX_LAPS 100
uint32_t lapTimes[MAX_LAPS];

//----- Calibration -------------------------------
uint8_t isCalibrated = 0;
uint32_t calibrationMilliseconds = 0;

// Usage of signed int calibration constant:
// calibratedMs = readMs + readMs/timeCalibrationConst
int32_t timeCalibrationConst = 0;

//----- other globals------------------------------
uint8_t allowEdgeGeneration = 0;
uint8_t channelIndex = 0;
uint8_t bandIndex = 0;
uint8_t isRaceStarted = 0;
uint8_t isSoundEnabled = 1;
uint8_t isConfigured = 0; //changes to 1 if any input changes the state of the device. it will mean that externally stored preferences should not be applied
uint8_t rssiMonitor = 0;
uint8_t newLapIndex = 0;
uint8_t shouldSkipFirstLap = 1; //start table is before the laptimer, so first lap is not a full-fledged lap (i.e. don't respect min-lap-time for the very first lap)
uint8_t isSendingData = 0;
uint8_t sendStage = 0;
uint8_t sendLapTimesIndex = 0;
uint8_t sendLastLapIndex = 0;
uint8_t shouldSendSingleItem = 0;
uint8_t lastLapsNotSent = 0;
uint16_t rssiMonitorDelayExpiration = 0;

// ----------------------------------------------------------------------------
void wait_rssi_ready() {
    delay(MIN_TUNE_TIME);
}

// ----------------------------------------------------------------------------
uint16_t readRSSI_a() {
    int rssiA = 0;

    for (uint8_t i = 0; i < RSSI_READS; i++) {
        rssiA += analogRead(rssiPinA);
    }

    rssiA = rssiA/RSSI_READS; // average of RSSI_READS readings
    return rssiA;
}

uint16_t readRSSI_b() {
    int rssiB = 0;

    for (uint8_t i = 0; i < RSSI_READS; i++) {
        rssiB += analogRead(rssiPinB);
    }

    rssiB = rssiB/RSSI_READS; // average of RSSI_READS readings
    return rssiB;
}

// ----------------------------------------------------------------------------
void incChannel() {
    if (channelIndex < 7) {
        channelIndex++;
    }
    setChannelModule(channelIndex, bandIndex);
    wait_rssi_ready();
}
// ----------------------------------------------------------------------------
void decChannel() {
    if (channelIndex > 0) {
        channelIndex--;
    }
    setChannelModule(channelIndex, bandIndex);
    wait_rssi_ready();
}
// ----------------------------------------------------------------------------
void setChannel(uint8_t channel) {
    if (channel >= 0 && channel <= 7) {
        channelIndex = channel;
        setChannelModule(channelIndex, bandIndex);
        wait_rssi_ready();
    }
}

// ----------------------------------------------------------------------------
void incBand() {
    if (bandIndex < MAX_BAND_IDX) {
        bandIndex++;
    }
    setChannelModule(channelIndex, bandIndex);
    wait_rssi_ready();
}
// ----------------------------------------------------------------------------
void decBand() {
    if (bandIndex > 0) {
        bandIndex--;
    }
    setChannelModule(channelIndex, bandIndex);
    wait_rssi_ready();
}
// ----------------------------------------------------------------------------
void setBand(uint8_t band) {
    if (band >= 0 && band <= MAX_BAND_IDX) {
        bandIndex = band;
        setChannelModule(channelIndex, bandIndex);
        wait_rssi_ready();
    }
}

void rotateBand() {
	bandIndex = (bandIndex + 1) % (MAX_BAND_IDX + 1);
	setChannelModule(channelIndex, bandIndex);
	wait_rssi_ready();
}

// ----------------------------------------------------------------------------
uint16_t getFilteredRSSI_a() {
    rssi_read_buf_a[0] = readRSSI_a();

	// TODO: validate this statement:
    /*
		several-pass filter (need several passes because of integer artithmetics)
    	it reduces possible max value by 1 with each iteration.
    	e.g. if max rssi is 300, then after 5 filter stages it won't be greater than 295
	*/
    for(uint8_t i=1; i<=FILTER_ITERATIONS; i++) {
        rssi_read_buf_a[i] = (rssi_read_buf_a[i-1] + rssi_read_buf_a[i]) >> 1;
    }

    return rssi_read_buf_a[FILTER_ITERATIONS];
}

uint16_t getFilteredRSSI_b() {
    rssi_read_buf_b[0] = readRSSI_b();
    for(uint8_t i=1; i<=FILTER_ITERATIONS; i++) {
        rssi_read_buf_b[i] = (rssi_read_buf_b[i-1] + rssi_read_buf_b[i]) >> 1;
    }
    return rssi_read_buf_b[FILTER_ITERATIONS];
}


void setup()   {

	// initialize with the I2C addr 0x3C / mit I2C-Adresse 0x3c initialisieren
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    // initialize digital pin 13 LED as an output.
    pinMode(MIDDLE_LED_PIN, OUTPUT);
    digitalHigh(MIDDLE_LED_PIN);

    // SPI pins for RX control
    setupSPIpins();

    // set the channel as soon as we can
    // faster boot up times :)
    setChannelModule(channelIndex, bandIndex);
    wait_rssi_ready();

    initFastADC();

    // Setup Done - Turn Status LED off.
    digitalLow(MIDDLE_LED_PIN);

	display.setTextColor(WHITE);

	setChannel(0);
	setBand(0);

	pinMode(NAV_LEFT, INPUT_PULLUP);
	pinMode(NAV_MIDDLE, INPUT_PULLUP);
	pinMode(NAV_RIGHT, INPUT_PULLUP);
}

unsigned long lastAnimationFrame;
unsigned long nextAnimationFrame;

void loop() {
	static char buffer[10];

	display.clearDisplay();

	memset(&buffer, 0, sizeof(buffer));
	itoa(channelIndex, &buffer[0], 10);
	display.setCursor(10,0);
	display.print(buffer);

	memset(&buffer, 0, sizeof(buffer));
	itoa(bandIndex, &buffer[0], 10);
	display.setCursor(10,10);
	display.print(buffer);

    int rssi_A = getFilteredRSSI_a();

	memset(&buffer, 0, sizeof(buffer));
	itoa(rssi_A, &buffer[0], 10);
	display.setCursor(10,20);
	display.print(buffer);

	int height_A = map(rssi_A, 0, RSSI_MAX, 0, 32);
	display.drawRect(0, 0, 5, 32 - height_A, WHITE);
	display.fillRect(0, 32 - height_A, 5, height_A, WHITE);

	int rssi_B = getFilteredRSSI_b();
	int height_B = map(rssi_B, 0, RSSI_MAX, 0, 32);
	display.drawRect(123, 0, 5, 32 - height_B, WHITE);
	display.fillRect(123, 32 - height_B, 5, height_B, WHITE);

	display.display();
	delay(100);

	if (!digitalRead(NAV_RIGHT)) {
		incChannel();
	}
	if (!digitalRead(NAV_LEFT)) {
		decChannel();
	}
	if (!digitalRead(NAV_MIDDLE)) {
		rotateBand();
	}

}
