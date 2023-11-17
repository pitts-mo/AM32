
/* AM32- multi-purpose brushless controller firmware for the stm32f051 */

//===========================================================================
//=============================== Changelog =================================
//===========================================================================
/*
 * 1.54 Changelog;
 * --Added firmware name to targets and firmware version to main
 * --added two more dshot to beacons 1-3 currently working
 * --added KV option to firmware, low rpm power protection is based on KV
 * --start power now controls minimum idle power as well as startup strength.
 * --change default timing to 22.5
 * --Lowered default minimum idle setting to 1.5 percent duty cycle, slider range from 1-2.
 * --Added dshot commands to save settings and reset ESC.
 *
 *1.56 Changelog.
 * -- added check to stall protection to wait until after 40 zero crosses to fix high startup throttle hiccup.
 * -- added TIMER 1 update interrupt and PWM changes are done once per pwm period
 * -- reduce commutation interval averaging length
 * -- reduce false positive filter level to 2 and eliminate threshold where filter is stopped.
 * -- disable interrupt before sounds
 * -- disable TIM1 interrupt during stepper sinusoidal mode
 * -- add 28us delay for dshot300
 * -- report 0 rpm until the first 10 successful steps.
 * -- move serial ADC telemetry calculations and desync check to 10Khz interrupt.
 *
 * 1.57
 * -- remove spurious commutations and rpm data at startup by polling for longer interval on startup
 *
 * 1.58
 * -- move signal timeout to 10khz routine and set armed timeout to one quarter second 2500 / 10000
 * 1.59
 * -- moved comp order definitions to target.h
 * -- fixed update version number if older than new version
 * -- cleanup, moved all input and output to IO.c
 * -- moved comparator functions to comparator.c
 * -- removed ALOT of useless variables
 * -- added siskin target
 * -- moved pwm changes to 10khz routine
 * -- moved basic functions to functions.c
 * -- moved peripherals setup to periherals.c
 * -- added crawler mode settings
 *
 * 1.60
 * -- added sine mode hysteresis
 * -- increased power in stall protection and lowered start rpm for crawlers
 * -- removed onehot125 from crawler mode
 * -- reduced maximum startup power from 400 to 350
 * -- change minimum duty cycle to DEAD_TIME
 * -- version and name moved to permanent spot in FLASH memory, thanks mikeller
 *
 * 1.61
 * -- moved duty cycle calculation to 10khz and added max change option.
 * -- decreased maximum interval change to 25%
 * -- reduce wait time on fast acceleration (fast_accel)
 * -- added check in interrupt for early zero cross
 *
 * 1.62
 * --moved control to 10khz loop
 * --changed condition for low rpm filter for duty cycle from || to &&
 * --introduced max deceleration and set it to 20ms to go from 100 to 0
 * --added configurable servo throttle ranges
 *
 *
 *1.63
 *-- increase time for zero cross error detection below 250us commutation interval
 *-- increase max change a low rpm x10
 *-- set low limit of throttle ramp to a lower point and increase upper range
 *-- change desync event from full restart to just lower throttle.

 *1.64
 * --added startup check for continuous high signal, reboot to enter bootloader.
 *-- added brake on stop from eeprom
 *-- added stall protection from eeprom
 *-- added motor pole divider for sinusoidal and low rpm power protection
 *-- fixed dshot commands, added confirmation beeps and removed blocking behavior
 *--
 *1.65
 *-- Added 32 millisecond telemetry output
 *-- added low voltage cutoff , divider value and cutoff voltage needs to be added to eeprom
 *-- added beep to indicate cell count if low voltage active
 *-- added current reading on pa3 , conversion factor needs to be added to eeprom
 *-- fixed servo input capture to only read positive pulse to handle higher refresh rates.
 *-- disabled oneshot 125.
 *-- extended servo range to match full output range of receivers
 *-- added RC CAR style reverse, proportional brake on first reverse , double tap to change direction
 *-- added brushed motor control mode
 *-- added settings to EEPROM version 1
 *-- add gimbal control option.
 *--
 *1.66
 *-- move idwg init to after input tune
 *-- remove reset after save command -- dshot
 *-- added wraith32 target
 *-- added average pulse check for signal detection
 *--
 *1.67
 *-- Rework file structure for multiple MCU support
 *-- Add g071 mcu
 *--
 *1.68
 *--increased allowed average pulse length to avoid double startup
 *1.69
 *--removed line re-enabling comparator after disabling.
 *1.70 fix dshot for Kiss FC
 *1.71 fix dshot for Ardupilot / Px4 FC
 *1.72 Fix telemetry output and add 1 second arming.
 *1.73 Fix false arming if no signal. Remove low rpm throttle protection below 300kv
 *1.74 Add Sine Mode range and drake brake strength adjustment
 *1.75 Disable brake on stop for PWM_ENABLE_BRIDGE
           Removed automatic brake on stop on neutral for RC car proportional brake.
           Adjust sine speed and stall protection speed to more closely match
           makefile fixes from Cruwaller
           Removed gd32 build, until firmware is functional
 *1.76 Adjust g071 PWM frequency, and startup power to be same frequency as f051.
       Reduce number of polling back emf checks for g071
 *1.77 increase PWM frequency range to 8-48khz
 *1.78 Fix bluejay tunes frequency and speed.
           Fix g071 Dead time
           Increment eeprom version
 *1.79 Add stick throttle calibration routine
           Add variable for telemetry interval
 *1.80 -Enable Comparator blanking for g071 on timer 1 channel 4
           -add hardware group F for Iflight Blitz
           -adjust parameters for pwm frequency
           -add sine mode power variable and eeprom setting
           -fix telemetry rpm during sine mode
           -fix sounds for extended pwm range
           -Add adjustable braking strength when driving
 *1.81 -Add current limiting PID loop
           -fix current sense scale
           -Increase brake power on maximum reverse ( car mode only)
           -Add HK and Blpwr targets
           -Change low kv motor throttle limit
           -add reverse speed threshold changeover based on motor kv
           -doubled filter length for motors under 900kv
*1.82  -Add speed control pid loop.
*1.83  -Add stall protection pid loop.
           -Improve sine mode transition.
           -decrease speed step re-entering sine mode
           -added fixed duty cycle and speed mode build option
           -added rpm_controlled by input signal ( to be added to config tool )
*1.84  -Change PID value to int for faster calculations
           -Enable two channel brushed motor control for dual motors
           -Add current limit max duty cycle
*1.85  -fix current limit not allowing full rpm on g071 or low pwm frequency
                -remove unused brake on stop conditional
*1.86  - create do-once in sine mode instead of setting pwm mode each time.
*1.87  - fix fixed mode max rpm limits
*1.88  - Fix stutter on sine mode re-entry due to position reset
*1.89  - Fix drive by rpm mode scaling.
           - Fix dshot px4 timings
*1.90  - Disable comp interrupts for brushed mode
           - Re-enter polling mode after prop strike or desync
           - add G071 "N" variant
           - add preliminary Extended Dshot
*1.91  - Reset average interval time on desync only after 100 zero crosses
*1.92  - Move g071 comparator blanking to TIM1 OC5
           - Increase ADC read frequency and current sense filtering
           - Add addressable LED strip for G071 targets
*1.93  - Optimization for build process
       - Add firmware file name to each target hex file
       -fix extended telemetry not activating dshot600
       -fix low voltage cuttoff timeout
*1.94  - Add selectable input types
*1.95  - reduce timeout to 0.5 seconds when armed
*1.96  - Improved erpm accuracy dshot and serial telemetry, thanks Dj-Uran
             - Fix PID loop integral.
                 - add overcurrent low voltage cuttoff to brushed mode.
*1.97    - enable input pullup
*1.98    - Dshot erpm rounding compensation.
*1.99    - Add max duty cycle change to individual targets ( will later become an settings option)
                 - Fix dshot telemetry delay f4 and e230 mcu
*2.00    - Cleanup of target structure
*2.01    - Increase 10khztimer to 20khz, increase max duty cycle change. 
*2.02	 - Increase startup power for inverted output targets.
*2.03    - Move chime from dshot direction change commands to save command.
*2.04    - Fix current protection, max duty cycle not increasing
                 - Fix double startup chime
                 - Change current averaging method for more precision
                 - Fix startup ramp speed adjustment
*2.05		 - Fix ramp tied to input frequency	
*2.06		 - Added LUT support 
						- changed input to scale before logic
				 - General code changes to improve logic flow and reduce code space		
                 
*/
#include "main.h"
#include "ADC.h"
#include "IO.h"
#include "common.h"
#include "comparator.h"
#include "dshot.h"
#include "eeprom.h"
#include "functions.h"
#include "peripherals.h"
#include "phaseouts.h"
#include "serial_telemetry.h"
#include "signal.h"
#include "sounds.h"
#include "targets.h"
#include "lookup_tables.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef USE_LED_STRIP
#include "WS2812.h"
#endif

#ifdef USE_CRSF_INPUT
#include "crsf.h"
#endif

#define VERSION_MAJOR 2
#define VERSION_MINOR 06

uint32_t pwm_frequency_conversion_factor = 0;
uint16_t blank_time;
void zcfoundroutine(void);

// firmware build options !! fixed speed and duty cycle modes are not to be used with sinusoidal startup !!

// #define FIXED_DUTY_MODE  // bypasses signal input and arming, uses a set duty cycle. For pumps, slot cars etc
// #define FIXED_DUTY_MODE_POWER 100     // 0-100 percent not used in fixed speed mode

// #define FIXED_SPEED_MODE  // bypasses input signal and runs at a fixed rpm using the speed control loop PID
// #define FIXED_SPEED_MODE_RPM  1000  // intended final rpm , ensure pole pair numbers are entered correctly in config tool.

// #define BRUSHED_MODE         // overrides all brushless config settings, enables two channels for brushed control
// #define GIMBAL_MODE     // also sinusoidal_startup needs to be on, maps input to sinusoidal angle.

//===========================================================================
//=============================  Defaults =============================
//===========================================================================

uint8_t drive_by_rpm = 0;
uint32_t MAXIMUM_RPM_SPEED_CONTROL = 10000;
uint32_t MINIMUM_RPM_SPEED_CONTROL = 1000;

// assign speed control PID values values are x10000
fastPID speedPid = { // commutation speed loop time
    .Kp = 10,
    .Ki = 0,
    .Kd = 100,
    .integral_limit = 10000,
    .output_limit = 50000
};

fastPID currentPid = { // 1khz loop time
    .Kp = 800,
    .Ki = 0,
    .Kd = 1000,
    .integral_limit = 20000,
    .output_limit = 100000
};

fastPID stallPid = { // 1khz loop time
    .Kp = 2,
    .Ki = 0,
    .Kd = 50,
    .integral_limit = 10000,
    .output_limit = 50000
};

enum inputType {
    AUTO_IN,
    DSHOT_IN,
    SERVO_IN,
    SERIAL_IN,
    EDTARM,
};

uint16_t ledcounter = 0;
uint32_t process_time = 0;
uint32_t start_process = 0;
uint16_t one_khz_loop_counter = 0;
uint16_t target_e_com_time_high;
uint16_t target_e_com_time_low;
uint8_t compute_dshot_flag = 0;
uint8_t crsf_input_channel = 1;
uint8_t crsf_output_PWM_channel = 2;
char eeprom_layout_version = 2;
char dir_reversed = 0;
char comp_pwm = 1;
char VARIABLE_PWM = 1;
char bi_direction = 0;
char stuck_rotor_protection = 1; // Turn off for Crawlers
char brake_on_stop = 0;
char stall_protection = 0;
char use_sin_start = 0;
char TLM_ON_INTERVAL = 0;
uint8_t telemetry_interval_ms = 30;
uint8_t TEMPERATURE_LIMIT = 255; // degrees 255 to disable
char advance_level = 2; // 7.5 degree increments 0 , 7.5, 15, 22.5)
uint16_t motor_kv = 2000;
char motor_poles = 14;
uint32_t e_comm_target_divisor = 8571428;
uint16_t CURRENT_LIMIT = 202;
uint8_t sine_mode_power = 5;
char drag_brake_strength = 10; // Drag Brake Power when brake on stop is enabled
uint8_t driving_brake_strength = 10;
uint8_t dead_time_override = DEAD_TIME;
char sine_mode_changeover_thottle_level = 5; // Sine Startup Range
uint16_t stall_protect_target_interval = TARGET_STALL_PROTECTION_INTERVAL;
char USE_HALL_SENSOR = 0;
uint16_t enter_sine_angle = 180;
char do_once_sinemode = 0;
//============================= Servo Settings ==============================
uint16_t servo_low_threshold = 1100; // anything below this point considered 0
uint16_t servo_high_threshold = 1900; // anything above this point considered 2000 (max)
uint16_t servo_neutral = 1500;
uint8_t servo_dead_band = 100;

//========================= Battery Cuttoff Settings ========================
char LOW_VOLTAGE_CUTOFF = 0; // Turn Low Voltage CUTOFF on or off
uint16_t low_cell_volt_cutoff = 330; // 3.3volts per cell

//=========================== END EEPROM Defaults ===========================

#ifdef USE_MAKE
typedef struct __attribute__((packed)) {
    uint8_t version_major;
    uint8_t version_minor;
    char device_name[12];
} firmware_info_s;

firmware_info_s __attribute__((section(".firmware_info"))) firmware_info = {
    version_major : VERSION_MAJOR,
    version_minor : VERSION_MINOR,
    device_name : FIRMWARE_NAME
};
#endif
const char filename[30] __attribute__((section(".file_name"))) = FILE_NAME;

char firmware_name[12] = FIRMWARE_NAME;

uint8_t EEPROM_VERSION;
// move these to targets folder or peripherals for each mcu
char RC_CAR_REVERSE = 0; // have to set bidirectional, comp_pwm off and stall protection off, no sinusoidal startup
uint16_t ADC_CCR = 30;
uint16_t current_angle = 90;
uint16_t desired_angle = 90;
char return_to_center = 0;
uint16_t target_e_com_time = 0;
int16_t Speed_pid_output;
char use_speed_control_loop = 0;
float input_override = 0;
int16_t use_current_limit_adjust = 2000;
char use_current_limit = 0;
float stall_protection_adjust = 0;

uint32_t MCU_Id = 0;
uint32_t REV_Id = 0;

uint16_t armed_timeout_count;
uint16_t reverse_speed_threshold = 1500;
uint8_t desync_happened = 0;
char maximum_throttle_change_ramp = 1;

char crawler_mode = 0; // no longer used //
uint16_t velocity_count = 0;
uint16_t velocity_count_threshold = 75;

char low_rpm_throttle_limit = 1;

uint16_t low_voltage_count = 0;
uint16_t telem_ms_count;

char VOLTAGE_DIVIDER = TARGET_VOLTAGE_DIVIDER; // 100k upper and 10k lower resistor in divider
uint16_t battery_voltage; // scale in volts * 10.  1260 is a battery voltage of 12.60
char cell_count = 0;
char brushed_direction_set = 0;

uint16_t tenkhzcounter = 0;
float consumed_current = 0;
int32_t smoothed_raw_current = 0;
int16_t actual_current = 0;

char lowkv = 0;

uint16_t min_startup_duty = 120;
uint16_t sin_mode_min_s_d = 120;
char bemf_timeout = 10;

char startup_boost = 50;
char reversing_dead_band = 1;

uint16_t low_pin_count = 0;

uint8_t max_duty_cycle_change = 2;
char fast_accel = 1;
uint16_t last_duty_cycle = 0;
uint16_t duty_cycle_setpoint = 0;
char play_tone_flag = 0;

typedef enum {
    GPIO_PIN_RESET = 0U,
    GPIO_PIN_SET
} GPIO_PinState;

uint16_t startup_max_duty_cycle = 300 + DEAD_TIME;
uint16_t minimum_duty_cycle = DEAD_TIME;
uint16_t stall_protect_minimum_duty = DEAD_TIME;
char desync_check = 0;
char low_kv_filter_level = 20;

uint16_t tim1_arr = TIM1_AUTORELOAD; // current auto reset value
uint16_t TIMER1_MAX_ARR = TIM1_AUTORELOAD; // maximum auto reset register value
uint16_t duty_cycle_maximum = TIM1_AUTORELOAD; // restricted by temperature or low rpm throttle protect
uint16_t low_rpm_level = 20; // thousand erpm used to set range for throttle resrictions
uint16_t high_rpm_level = 70; //
uint16_t throttle_max_at_low_rpm = 400;
uint16_t throttle_max_at_high_rpm = TIM1_AUTORELOAD;

uint16_t commutation_intervals[6] = { 0 };
uint32_t average_interval = 0;
uint32_t last_average_interval;
int e_com_time;

uint16_t ADC_smoothed_input = 0;
uint8_t degrees_celsius;
int16_t converted_degrees;
uint8_t temperature_offset;
uint16_t ADC_raw_temp;
uint16_t ADC_raw_volts;
uint16_t ADC_raw_current;
uint16_t ADC_raw_input;
uint8_t adc_counter = 0;
char send_telemetry = 0;
char telemetry_done = 0;
char prop_brake_active = 0;

uint8_t eepromBuffer[176] = { 0 };

char dshot_telemetry = 0;

uint8_t last_dshot_command = 0;
char old_routine = 1;
uint16_t adjusted_input = 0;

#define TEMP30_CAL_VALUE ((uint16_t*)((uint32_t)0x1FFFF7B8))
#define TEMP110_CAL_VALUE ((uint16_t*)((uint32_t)0x1FFFF7C2))

uint16_t smoothedcurrent = 0;
const uint8_t numReadings = 100; // the readings from the analog input
uint8_t readIndex = 0; // the index of the current reading
uint32_t total = 0;
uint16_t readings[100];

uint8_t bemf_timeout_happened = 0;
uint8_t changeover_step = 5;
uint8_t filter_level = 5;
uint8_t running = 0;
uint16_t advance = 0;
uint8_t advancedivisor = 6;
char rising = 1;

////Space Vector PWM ////////////////
// const int pwmSin[] ={128, 132, 136, 140, 143, 147, 151, 155, 159, 162, 166, 170, 174, 178, 181, 185, 189, 192, 196, 200, 203, 207, 211, 214, 218, 221, 225, 228, 232, 235, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 248, 249, 250, 250, 251, 252, 252, 253, 253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253, 253, 253, 252, 252, 251, 250, 250, 249, 248, 248, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 248, 249, 250, 250, 251, 252, 252, 253, 253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253, 253, 253, 252, 252, 251, 250, 250, 249, 248, 248, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 235, 232, 228, 225, 221, 218, 214, 211, 207, 203, 200, 196, 192, 189, 185, 181, 178, 174, 170, 166, 162, 159, 155, 151, 147, 143, 140, 136, 132, 128, 124, 120, 116, 113, 109, 105, 101, 97, 94, 90, 86, 82, 78, 75, 71, 67, 64, 60, 56, 53, 49, 45, 42, 38, 35, 31, 28, 24, 21, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 8, 7, 6, 6, 5, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 8, 7, 6, 6, 5, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 21, 24, 28, 31, 35, 38, 42, 45, 49, 53, 56, 60, 64, 67, 71, 75, 78, 82, 86, 90, 94, 97, 101, 105, 109, 113, 116, 120, 124};

////Sine Wave PWM ///////////////////
int16_t pwmSin[] = { 180, 183, 186, 189, 193, 196, 199, 202,
    205, 208, 211, 214, 217, 220, 224, 227,
    230, 233, 236, 239, 242, 245, 247, 250,
    253, 256, 259, 262, 265, 267, 270, 273,
    275, 278, 281, 283, 286, 288, 291, 293,
    296, 298, 300, 303, 305, 307, 309, 312,
    314, 316, 318, 320, 322, 324, 326, 327,
    329, 331, 333, 334, 336, 337, 339, 340,
    342, 343, 344, 346, 347, 348, 349, 350,
    351, 352, 353, 354, 355, 355, 356, 357,
    357, 358, 358, 359, 359, 359, 360, 360,
    360, 360, 360, 360, 360, 360, 360, 359,
    359, 359, 358, 358, 357, 357, 356, 355,
    355, 354, 353, 352, 351, 350, 349, 348,
    347, 346, 344, 343, 342, 340, 339, 337,
    336, 334, 333, 331, 329, 327, 326, 324,
    322, 320, 318, 316, 314, 312, 309, 307,
    305, 303, 300, 298, 296, 293, 291, 288,
    286, 283, 281, 278, 275, 273, 270, 267,
    265, 262, 259, 256, 253, 250, 247, 245,
    242, 239, 236, 233, 230, 227, 224, 220,
    217, 214, 211, 208, 205, 202, 199, 196,
    193, 189, 186, 183, 180, 177, 174, 171,
    167, 164, 161, 158, 155, 152, 149, 146,
    143, 140, 136, 133, 130, 127, 124, 121,
    118, 115, 113, 110, 107, 104, 101, 98,
    95, 93, 90, 87, 85, 82, 79, 77,
    74, 72, 69, 67, 64, 62, 60, 57,
    55, 53, 51, 48, 46, 44, 42, 40,
    38, 36, 34, 33, 31, 29, 27, 26,
    24, 23, 21, 20, 18, 17, 16, 14,
    13, 12, 11, 10, 9, 8, 7, 6,
    5, 5, 4, 3, 3, 2, 2, 1,
    1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 2, 2,
    3, 3, 4, 5, 5, 6, 7, 8,
    9, 10, 11, 12, 13, 14, 16, 17,
    18, 20, 21, 23, 24, 26, 27, 29,
    31, 33, 34, 36, 38, 40, 42, 44,
    46, 48, 51, 53, 55, 57, 60, 62,
    64, 67, 69, 72, 74, 77, 79, 82,
    85, 87, 90, 93, 95, 98, 101, 104,
    107, 110, 113, 115, 118, 121, 124, 127,
    130, 133, 136, 140, 143, 146, 149, 152,
    155, 158, 161, 164, 167, 171, 174, 177 };

// int sin_divider = 2;
int16_t phase_A_position;
int16_t phase_B_position;
int16_t phase_C_position;
uint16_t step_delay = 100;
char stepper_sine = 0;
char forward = 1;
uint16_t gate_drive_offset = DEAD_TIME;

uint8_t stuckcounter = 0;
uint16_t k_erpm;
uint16_t e_rpm; // electrical revolution /100 so,  123 is 12300 erpm

uint16_t adjusted_duty_cycle;

uint8_t bad_count = 0;
uint8_t bad_count_threshold = CPU_FREQUENCY_MHZ / 24;
uint8_t dshotcommand;
uint16_t armed_count_threshold = 1000;

char armed = 0;
uint16_t zero_input_count = 0;

uint16_t input = 0;
uint16_t newinput = 0;
char inputSet = 0;
char dshot = 0;
char servoPwm = 0;
uint32_t zero_crosses;

uint8_t zcfound = 0;

uint8_t bemfcounter;
uint8_t min_bemf_counts_up = TARGET_MIN_BEMF_COUNTS;
uint8_t min_bemf_counts_down = TARGET_MIN_BEMF_COUNTS;

uint16_t lastzctime;
uint16_t thiszctime;

uint16_t duty_cycle = 0;
char step = 1;
uint16_t commutation_interval = 12500;
uint16_t waitTime = 0;
uint16_t signaltimeout = 0;
uint8_t ubAnalogWatchdogStatus = RESET;

// void checkForHighSignal(){
// changeToInput();

// setInputPullDown();
// delayMicros(1000);
// for(int i = 0 ; i < 1000; i ++){
//	 if( !(getInputPinState())){  // if the pin is low for 5 checks out of 100 in  100ms or more its either no signal or signal. jump to application
//		 low_pin_count++;
//	 }
//      delayMicros(10);
// }

// setInputPullUp();
//	 if(low_pin_count > 5){
//		 return;      // its either a signal or a disconnected pin
//	 }else{
//		allOff();
//		NVIC_SystemReset();
//	 }
// }

int32_t doPidCalculations(struct fastPID * pidnow, int actual, int target)
{

    pidnow->error = actual - target;
		pidnow->integral = Clamp(pidnow->integral + pidnow->error * pidnow->Ki, -pidnow->integral_limit, pidnow->integral_limit);

    pidnow->derivative = pidnow->Kd * (pidnow->error - pidnow->last_error);
    pidnow->last_error = pidnow->error;

    pidnow->pid_output = Clamp(pidnow->error * pidnow->Kp + pidnow->integral + pidnow->derivative, -pidnow->output_limit, pidnow->output_limit);
    return pidnow->pid_output;
}

void loadEEpromSettings()
{
    read_flash_bin(eepromBuffer, EEPROM_START_ADD, 176);

    if (eepromBuffer[17] == 0x01) {
        dir_reversed = 1;
    } else {
        dir_reversed = 0;
    }
    if (eepromBuffer[18] == 0x01) {
        bi_direction = 1;
    } else {
        bi_direction = 0;
    }
    if (eepromBuffer[19] == 0x01) {
        use_sin_start = 1;
        //	 min_startup_duty = sin_mode_min_s_d;
    }
    if (eepromBuffer[20] == 0x01) {
        comp_pwm = 1;
    } else {
        comp_pwm = 0;
    }
    if (eepromBuffer[21] == 0x01) {
        VARIABLE_PWM = 1;
    } else {
        VARIABLE_PWM = 0;
    }
    if (eepromBuffer[22] == 0x01) {
        stuck_rotor_protection = 1;
    } else {
        stuck_rotor_protection = 0;
    }
    if (eepromBuffer[23] < 4) {
        advance_level = eepromBuffer[23];
    } else {
        advance_level = 2; // * 7.5 increments
    }

    if (eepromBuffer[24] < 49 && eepromBuffer[24] > 7) {
        if (eepromBuffer[24] < 49 && eepromBuffer[24] > 23) {
            TIMER1_MAX_ARR = map(eepromBuffer[24], 24, 48, TIM1_AUTORELOAD, TIM1_AUTORELOAD / 2);
        }
        if (eepromBuffer[24] < 24 && eepromBuffer[24] > 11) {
            TIMER1_MAX_ARR = map(eepromBuffer[24], 12, 24, TIM1_AUTORELOAD * 2, TIM1_AUTORELOAD);
        }
        if (eepromBuffer[24] < 12 && eepromBuffer[24] > 7) {
            TIMER1_MAX_ARR = map(eepromBuffer[24], 7, 16, TIM1_AUTORELOAD * 3, TIM1_AUTORELOAD / 2 * 3);
        }
        SET_AUTO_RELOAD_PWM(TIMER1_MAX_ARR);
        throttle_max_at_high_rpm = TIMER1_MAX_ARR;
        duty_cycle_maximum = TIMER1_MAX_ARR;
    } else {
        tim1_arr = TIM1_AUTORELOAD;
        SET_AUTO_RELOAD_PWM(tim1_arr);
    }

    if (eepromBuffer[25] < 151 && eepromBuffer[25] > 49) {
        min_startup_duty = (eepromBuffer[25] + DEAD_TIME * 2);
        minimum_duty_cycle = (eepromBuffer[25] / 2 + DEAD_TIME / 3);
        stall_protect_minimum_duty = minimum_duty_cycle + 10;
    } else {
        min_startup_duty = 150;
        minimum_duty_cycle = (min_startup_duty / 2) + 10;
    }
    motor_kv = (eepromBuffer[26] * 40) + 20;
    motor_poles = eepromBuffer[27];
		e_comm_target_divisor = 60000000 / (motor_poles / 2);
    if (eepromBuffer[28] == 0x01) {
        brake_on_stop = 1;
    } else {
        brake_on_stop = 0;
    }
    if (eepromBuffer[29] == 0x01) {
        stall_protection = 1;
    } else {
        stall_protection = 0;
    }
    setVolume(5);
    if (eepromBuffer[1] > 0) { // these commands weren't introduced until eeprom version 1.

        if (eepromBuffer[30] > 11) {
            setVolume(5);
        } else {
            setVolume(eepromBuffer[30]);
        }
        if (eepromBuffer[31] == 0x01) {
            TLM_ON_INTERVAL = 1;
        } else {
            TLM_ON_INTERVAL = 0;
        }
        servo_low_threshold = (eepromBuffer[32] * 2) + 750; // anything below this point considered 0
        servo_high_threshold = (eepromBuffer[33] * 2) + 1750;
        ; // anything above this point considered 2000 (max)
        servo_neutral = (eepromBuffer[34]) + 1374;
        servo_dead_band = eepromBuffer[35];

        if (eepromBuffer[36] == 0x01) {
            LOW_VOLTAGE_CUTOFF = 1;
        } else {
            LOW_VOLTAGE_CUTOFF = 0;
        }

        low_cell_volt_cutoff = eepromBuffer[37] + 250; // 2.5 to 3.5 volts per cell range
        if (eepromBuffer[38] == 0x01) {
            RC_CAR_REVERSE = 1;
        } else {
            RC_CAR_REVERSE = 0;
        }
        if (eepromBuffer[39] == 0x01) {
#ifdef HAS_HALL_SENSORS
            USE_HALL_SENSOR = 1;
#else
            USE_HALL_SENSOR = 0;
#endif
        } else {
            USE_HALL_SENSOR = 0;
        }
        if (eepromBuffer[40] > 4 && eepromBuffer[40] < 26) { // sine mode changeover 5-25 percent throttle
            sine_mode_changeover_thottle_level = eepromBuffer[40];
        }
        if (eepromBuffer[41] > 0 && eepromBuffer[41] < 11) { // drag brake 1-10
            drag_brake_strength = eepromBuffer[41];
        }

        if (eepromBuffer[42] > 0 && eepromBuffer[42] < 10) { // motor brake 1-9
            driving_brake_strength = eepromBuffer[42];
            dead_time_override = DEAD_TIME + (150 - (driving_brake_strength * 10));
            if (dead_time_override > 200) {
                dead_time_override = 200;
            }
            min_startup_duty = eepromBuffer[25] + dead_time_override;
            minimum_duty_cycle = eepromBuffer[25] / 2 + dead_time_override;
            throttle_max_at_low_rpm = throttle_max_at_low_rpm + dead_time_override;
            startup_max_duty_cycle = startup_max_duty_cycle + dead_time_override;
#ifdef STMICRO
            TIM1->BDTR |= dead_time_override;
#endif
        }

        if (eepromBuffer[43] >= 70 && eepromBuffer[43] <= 140) {
            TEMPERATURE_LIMIT = eepromBuffer[43];
        }

        if (eepromBuffer[44] > 0 && eepromBuffer[44] < 100) {
            CURRENT_LIMIT = eepromBuffer[44] * 2;
            use_current_limit = 1;
        }
        if (eepromBuffer[45] > 0 && eepromBuffer[45] < 11) {
            sine_mode_power = eepromBuffer[45];
        }

        if (eepromBuffer[46] >= 0 && eepromBuffer[46] < 10) {
            switch (eepromBuffer[46]) {
            case AUTO_IN:
                dshot = 0;
                servoPwm = 0;
                EDT_ARMED = 1;
                break;
            case DSHOT_IN:
                dshot = 1;
                EDT_ARMED = 1;
                break;
            case SERVO_IN:
                servoPwm = 1;
                break;
            case SERIAL_IN:
                break;
            case EDTARM:
                EDT_ARM_ENABLE = 1;
                EDT_ARMED = 0;
                dshot = 1;
                break;
            };
        } else {
            dshot = 0;
            servoPwm = 0;
            EDT_ARMED = 1;
        }

        if (motor_kv < 300) {
            low_rpm_throttle_limit = 0;
        }
        low_rpm_level = motor_kv / 100 / (32 / motor_poles);
        high_rpm_level = motor_kv / 17 / (32 / motor_poles);
    }
    reverse_speed_threshold = map(motor_kv, 300, 3000, 1000, 500);
    //   reverse_speed_threshold = 200;
    if (!comp_pwm) {
        bi_direction = 0;
    }
}

void saveEEpromSettings()
{

    eepromBuffer[1] = eeprom_layout_version;
    if (dir_reversed == 1) {
        eepromBuffer[17] = 0x01;
    } else {
        eepromBuffer[17] = 0x00;
    }
    if (bi_direction == 1) {
        eepromBuffer[18] = 0x01;
    } else {
        eepromBuffer[18] = 0x00;
    }
    if (use_sin_start == 1) {
        eepromBuffer[19] = 0x01;
    } else {
        eepromBuffer[19] = 0x00;
    }

    if (comp_pwm == 1) {
        eepromBuffer[20] = 0x01;
    } else {
        eepromBuffer[20] = 0x00;
    }
    if (VARIABLE_PWM == 1) {
        eepromBuffer[21] = 0x01;
    } else {
        eepromBuffer[21] = 0x00;
    }
    if (stuck_rotor_protection == 1) {
        eepromBuffer[22] = 0x01;
    } else {
        eepromBuffer[22] = 0x00;
    }
    eepromBuffer[23] = advance_level;
    save_flash_nolib(eepromBuffer, 176, EEPROM_START_ADD);
}

uint16_t getSmoothedCurrent()
{
    total = total - readings[readIndex];
    readings[readIndex] = ADC_raw_current;
    total = total + readings[readIndex];
    readIndex = readIndex + 1;
    if (readIndex >= numReadings) {
        readIndex = 0;
    }
    smoothedcurrent = total / numReadings;
    return smoothedcurrent;
}

void getBemfState()
{
    uint8_t current_state = 0;
#ifdef MCU_F031
    if (step == 1 || step == 4) {
        current_state = PHASE_C_EXTI_PORT->IDR & PHASE_C_EXTI_PIN;
    }
    if (step == 2 || step == 5) { //        in phase two or 5 read from phase A Pf1
        current_state = PHASE_A_EXTI_PORT->IDR & PHASE_A_EXTI_PIN;
    }
    if (step == 3 || step == 6) { // phase B pf0
        current_state = PHASE_B_EXTI_PORT->IDR & PHASE_B_EXTI_PIN;
    }
#else
    current_state = !getCompOutputLevel(); // polarity reversed
#endif
		if (rising == current_state) {
			bemfcounter++;
		} else {
			bad_count++;
			if (bad_count > bad_count_threshold) {
					bemfcounter = 0;
			}			
		}
}

void commutate()
{
		step = RotateMinMax(step, forward ? 1 : -1, 1, 6);
		desync_check = (forward && step == 1) || (!forward && step == 6);	
		rising = (step % 2) == forward;
	
    __disable_irq(); // don't let dshot interrupt
    if (!prop_brake_active) {
        comStep(step);
    }
    __enable_irq();
    changeCompInput();
		
    if (stall_protection || RC_CAR_REVERSE) {
        if (average_interval > 2000) {
            old_routine = 1;
        }
    }
		
    bemfcounter = 0;
    zcfound = 0;
    commutation_intervals[step - 1] = thiszctime; // just used to calulate average
}

void PeriodElapsedCallback()
{

    DISABLE_COM_TIMER_INT(); // disable interrupt
    commutate();
    commutation_interval = ((3 * commutation_interval) + thiszctime) >> 2;
    advance = (commutation_interval >> 3) * advance_level; // 60 divde 8 7.5 degree increments
    waitTime = (commutation_interval >> 1) - advance;
    if (!old_routine) {
        enableCompInterrupts(); // enable comp interrupt
    }
    if (zero_crosses < 10000) {
        zero_crosses++;
    }
}

void interruptRoutine()
{
    // uint8_t badzccount = 0;
    thiszctime = INTERVAL_TIMER_COUNT;
    if (average_interval > 125) {
        if ((INTERVAL_TIMER_COUNT < 125) && (duty_cycle < 600) && (zero_crosses < 500)) { // should be impossible, desync?exit anyway
            return;
        }
        if (INTERVAL_TIMER_COUNT < (commutation_interval >> 1)) {
            return;
        }
        stuckcounter++; // stuck at 100 interrupts before the main loop happens again.
        if (stuckcounter > 100) {
            maskPhaseInterrupts();
            zero_crosses = 0;
            return;
        }
    }

    if (rising) {
        for (int i = 0; i < filter_level; i++) {
#ifdef MCU_F031
            if ((current_GPIO_PORT->IDR & current_GPIO_PIN) == (uint32_t)GPIO_PIN_RESET) {
#else
            if (getCompOutputLevel()) {
#endif
                //		badzccount++;
                return;
            }
        }
    } else {
        for (int i = 0; i < filter_level; i++) {
#ifdef MCU_F031
            if ((current_GPIO_PORT->IDR & current_GPIO_PIN) != (uint32_t)GPIO_PIN_RESET) {
#else
            if (!getCompOutputLevel()) {
#endif
                //		badzccount++;
                return;
            }
        }
    }
    //			if(badzccount > bad_count_threshold){
    //				return;
    //			}

    maskPhaseInterrupts();
    __disable_irq();
    if (INTERVAL_TIMER_COUNT > thiszctime) {
        SET_INTERVAL_TIMER_COUNT(INTERVAL_TIMER_COUNT - thiszctime);
    } else {
        SET_INTERVAL_TIMER_COUNT(0);
    }
    waitTime = waitTime >> fast_accel;
    SET_AND_ENABLE_COM_INT(waitTime); // enable COM_TIMER interrupt
    __enable_irq();
}

void startMotor()
{
    if (running == 0) {
        commutate();
        commutation_interval = 10000;
        SET_INTERVAL_TIMER_COUNT(5000);
        running = 1;
    }
    enableCompInterrupts();
}
typedef enum {
	IS_NEUTRAL = 0,
	IS_REVERSE = 1,
	IS_FORWARD = 2
} inputBiDirectionalStates_e;

bool scalesSet = 0;
uint8_t previous_input_state = IS_NEUTRAL;
uint8_t current_input_state = IS_NEUTRAL;

scale_result_t scale_result_input;
scale_result_t scale_result_input_adjust;

scale_t scaleInput;
scale_t scaleInputAdjust;
scale_functions_t scaleFunctions_int;
map_t mapInput;
map_t mapInputDir;
map_t mapInputAdjust;

static int16_t InputScaleServo[6] =   {   0, 949, 950, 1050, 1051, 2000};
static int16_t InputMapServo[6] =     {2047,  47,   0,    0,   47, 2047};
static int16_t InputDirMapPWM[6] =    {   1,   1,   0,    0,    2,    2};

static int16_t InputScaleDShot[6] =   {   0,  47,  48, 1047, 1049, 2047};
static int16_t InputMapDShot[6] =     {   0,  47,  48, 2047,   48, 2047};
static int16_t InputDirMapDshot[6] =  {   0,   0,   1,    1,    2,    2};


static int16_t InputScaleSineStart[6] =   {   0,  30,  31,  99, 100, 2047};
static int16_t InputMapSineStart[6] =  		{   0,   0,  47, 160, 161, 2047};

int16_t scaled_input = 0;
int16_t scaled_input_adjust = 0;

void SetScales(void) {
	if (!scalesSet) {        
			SetScaleFunctions_int(&scaleFunctions_int);            
			scaleInput.Type = SCALE_TYPE_INT16;
			scaleInput.Length = 6;
			__scale_t_SetResult(&scaleInput, &scale_result_input);
			scaleInput.Data = InputScaleServo;
			scaleInput.SourceValue = &newinput;   
			scale_result_input.ScaledValue = &scaled_input;
			
			scaleInputAdjust.Type = SCALE_TYPE_INT16;
			scaleInputAdjust.Length = 6;
			__scale_t_SetResult(&scaleInputAdjust, &scale_result_input_adjust);
			scaleInputAdjust.Data = InputScaleSineStart;
			scaleInput.SourceValue = &adjusted_input;
			scale_result_input_adjust.ScaledValue = &scaled_input_adjust;
		
			mapInput.Adder = 0;
			mapInput.Data = InputMapServo;
			mapInput.Dimensions = 2;
			mapInput.ScaledValue = scaleInput.Result;

			mapInputDir.Adder = 0;
			mapInputDir.Data = InputDirMapPWM;
			mapInputDir.Dimensions = 2;
			mapInputDir.ScaledValue = scaleInput.Result;		
		
			mapInputAdjust.Adder = 0;
			mapInputAdjust.Data = InputMapSineStart;
			mapInputAdjust.Dimensions = 2;
			mapInput.ScaledValue = scaleInputAdjust.Result;
			
			scalesSet = true;
	}	
}

void GetMappedInput(void) {
	if (bi_direction) {
		if (dshot) {
			scaleInput.Data = InputScaleDShot;
			mapInput.Data = InputMapDShot;
			mapInputDir.Data = InputDirMapDshot;
		}
		
		if (scaleFunctions_int.Compare(scaleInput.SourceValue, scale_result_input.ScaledValue) != 0) {
			Scale(&scaleInput, &scaleFunctions_int);
			MapLookup(&mapInput, &scale_result_input, &scaleFunctions_int);
			MapLookup(&mapInputDir, &scale_result_input, &scaleFunctions_int);
		}
	}
}

void input_bi_dir(bool dir_change) {
	if (dir_change) {
			if (((commutation_interval > reverse_speed_threshold) && (duty_cycle < 200)) || stepper_sine) {
					forward = !forward;
					zero_crosses = 0;
					old_routine = 1;
					maskPhaseInterrupts();
					brushed_direction_set = 0;
			} else {
					current_input_state = IS_NEUTRAL; //translates to adjusted_input = 0
			}
	}
	
  adjusted_input = (uint16_t)scaled_input;
}
void input_RC_CAR_REV(bool dir_change) {
	if (dir_change) {				
				current_input_state = IS_NEUTRAL; //translates to adjusted_input = 0
				prop_brake_active = !return_to_center;
				if (return_to_center) {
					forward = !forward; //just flip it
					return_to_center = 0;				
				}			
		}
		if (!prop_brake_active) {
			return_to_center = 0;
			adjusted_input = (uint16_t)scaled_input;
		}
}

void setInput()
{
	GetMappedInput();
	if (bi_direction) {
		//To change direction requires one of each, it doesn't matter which order
		bool dir_change = (current_input_state + previous_input_state == (IS_NEUTRAL + IS_FORWARD)); 
		if (current_input_state > IS_NEUTRAL) {
			if (RC_CAR_REVERSE) {
				input_RC_CAR_REV(dir_change);
			} else {
				input_bi_dir(dir_change);
			}
		} 
		if (current_input_state == IS_NEUTRAL) {
			adjusted_input = 0;
			if (RC_CAR_REVERSE) {
				return_to_center = prop_brake_active; //important to have before the next line!
				prop_brake_active = 0;
			} else {
				brushed_direction_set = 0;
			}
		}		
		previous_input_state = current_input_state;
	} else  {
		adjusted_input = newinput;
	}
	
#ifndef BRUSHED_MODE
    if ((bemf_timeout_happened > bemf_timeout) && stuck_rotor_protection) {
        allOff();
        maskPhaseInterrupts();
        input = 0;
        bemf_timeout_happened = 102;
#ifdef USE_RGB_LED
        GPIOB->BRR = LL_GPIO_PIN_8; // on red
        GPIOB->BSRR = LL_GPIO_PIN_5; //
        GPIOB->BSRR = LL_GPIO_PIN_3;
#endif
    } else {
#ifdef FIXED_DUTY_MODE
        input = FIXED_DUTY_MODE_POWER * 20 + 47;
#else
        if (use_sin_start) {					
					 scaleInputAdjust.Data = InputScaleSineStart;
					 mapInputAdjust.Data = InputMapSineStart;
					 Scale(&scaleInputAdjust, &scaleFunctions_int);
					 MapLookup(&mapInputAdjust, &scale_result_input_adjust, &scaleFunctions_int);
					 input = (uint16_t)scaled_input_adjust;
				}	else {
					bool zeroInput = (use_speed_control_loop && drive_by_rpm && current_input_state == IS_NEUTRAL);
					if (zeroInput) {
								input = 0;
								speedPid.error = 0;
								input_override = 0;						
					} else if (use_speed_control_loop) {
						if (drive_by_rpm) {
              target_e_com_time = e_comm_target_divisor / map(adjusted_input, 47, 2047, MINIMUM_RPM_SPEED_CONTROL, MAXIMUM_RPM_SPEED_CONTROL);							
						}
						input = Clamp(input_override, 48, 2047);
						
					} else {
						input = adjusted_input;
					}
        }
#endif
    }
#endif
#ifndef BRUSHED_MODE
    if (!stepper_sine) {
        if (input >= 47 + (80 * use_sin_start) && armed) {
            if (running == 0) {
                allOff();
                if (!old_routine) {
                    startMotor();
                }
                running = 1;
                last_duty_cycle = min_startup_duty;
            }

            if (use_sin_start) {
                duty_cycle_setpoint = map(input, 137, 2047, minimum_duty_cycle, TIMER1_MAX_ARR);
            } else {
                duty_cycle_setpoint = map(input, 47, 2047, minimum_duty_cycle, TIMER1_MAX_ARR);
            }

            if (!RC_CAR_REVERSE) {
                prop_brake_active = 0;
            }
        }

        if (input < 47 + (80 * use_sin_start)) {
            if (play_tone_flag != 0) {
                switch (play_tone_flag) {
                case 1:
                    playDefaultTone();
                    break;
                case 2:
                    playChangedTone();
                    break;
                case 3:
                    playBeaconTune3();
                    break;
                case 4:
                    playInputTune2();
                    break;
                case 5:
                    playDefaultTone();
                    break;
                }
                play_tone_flag = 0;
            }

            if (!comp_pwm) {
            duty_cycle_setpoint = 0;
                if (!running) {
                    old_routine = 1;
                    zero_crosses = 0;
                    if (brake_on_stop) {
                        fullBrake();
                    } else {
                        if (!prop_brake_active) {
                            allOff();
                        }
                    }
                }
                if (RC_CAR_REVERSE && prop_brake_active) {
#ifndef PWM_ENABLE_BRIDGE
										duty_cycle_setpoint = ABS(1000 - newinput) + 1000;
                    //duty_cycle_setpoint = getAbsDif(1000, newinput) + 1000;
                    if (duty_cycle_setpoint >= 1999) {
                        fullBrake();
                    } else {
                        proportionalBrake();
                    }
#endif
                }
            } else {
                if (!running) {

                    old_routine = 1;
                    zero_crosses = 0;
                    bad_count = 0;
                    if (brake_on_stop) {
                        if (!use_sin_start) {
#ifndef PWM_ENABLE_BRIDGE
                            duty_cycle = (TIMER1_MAX_ARR - 19) + drag_brake_strength * 2;
                            proportionalBrake();
                            prop_brake_active = 1;
#else
                            // todo add proportional braking for pwm/enable style bridge.
#endif
                        }
                    } else {
                        allOff();
                    }
                    duty_cycle_setpoint = 0;
                }
								phase_A_position = RotateMinMax(0, ((step - 1) * 60) + enter_sine_angle, 0, 359);
								phase_B_position = RotateMinMax(phase_A_position, 119, 0, 359);
								phase_C_position = RotateMinMax(phase_A_position, 239, 0, 359);
								
                if (use_sin_start == 1) {
                    stepper_sine = 1;
                }
                duty_cycle_setpoint = 0;
            }
        }
        if (!prop_brake_active) {
					bool startup = (input >= 47 && (zero_crosses < (20 >> stall_protection)));
					// if starting up, clamp to smaller of startup_max or standard max.  
					uint16_t maxDuty = startup? MIN(startup_max_duty_cycle, duty_cycle_maximum) : duty_cycle_maximum;
					
					duty_cycle_setpoint = Clamp(duty_cycle_setpoint, min_startup_duty, maxDuty);
					

            if (stall_protection_adjust > 0 && input > 47) {

                duty_cycle_setpoint = duty_cycle_setpoint + (uint16_t)stall_protection_adjust;
            }
        }
    }
}

void tenKhzRoutine()
{ // 20khz as of 2.00 to be renamed
    duty_cycle = duty_cycle_setpoint;
    tenkhzcounter++;
    ledcounter++;
    one_khz_loop_counter++;
    if (!armed) {
        if (cell_count == 0) {
            if (inputSet) {
                if (adjusted_input == 0) {
                    armed_timeout_count++;
                    if (armed_timeout_count > LOOP_FREQUENCY_HZ) { // one second
                        if (zero_input_count > 30) {
                            armed = 1;
#ifdef USE_LED_STRIP
                            //	send_LED_RGB(0,0,0);
                            delayMicros(1000);
                            send_LED_RGB(0, 255, 0);
#endif
#ifdef USE_RGB_LED
                            GPIOB->BRR = LL_GPIO_PIN_3; // turn on green
                            GPIOB->BSRR = LL_GPIO_PIN_8; // turn on green
                            GPIOB->BSRR = LL_GPIO_PIN_5;
#endif
                            if ((cell_count == 0) && LOW_VOLTAGE_CUTOFF) {
                                cell_count = battery_voltage / 370;
                                for (int i = 0; i < cell_count; i++) {
                                    playInputTune();
                                    delayMillis(100);
                                    RELOAD_WATCHDOG_COUNTER();
                                }
                            } else {
                                playInputTune();
                            }
                            if (!servoPwm) {
                                RC_CAR_REVERSE = 0;
                            }
                        } else {
                            inputSet = 0;
                            armed_timeout_count = 0;
                        }
                    }
                } else {
                    armed_timeout_count = 0;
                }
            }
        }
    }

    if (TLM_ON_INTERVAL) {
        telem_ms_count++;
        if (telem_ms_count > telemetry_interval_ms * 20) {
            send_telemetry = 1;
            telem_ms_count = 0;
        }
    }

#ifndef BRUSHED_MODE

    if (!stepper_sine) {
        if (old_routine && running) {
            maskPhaseInterrupts();
            getBemfState();
            if (!zcfound) {
							if (bemfcounter > rising? min_bemf_counts_up : min_bemf_counts_down) {
									zcfound = 1;
									zcfoundroutine();								
							}
            }
        }

        if (one_khz_loop_counter > PID_LOOP_DIVIDER) { // 1khz PID loop
            one_khz_loop_counter = 0;
					
						if (running) {							
							if (use_current_limit) {
									use_current_limit_adjust = Clamp( use_current_limit_adjust - (doPidCalculations(&currentPid, actual_current, CURRENT_LIMIT * 100) / 10000), minimum_duty_cycle, tim1_arr);								
							}
							if (stall_protection ) { // this boosts throttle as the rpm gets lower, for crawlers and rc cars only, do not use for multirotors.
									stall_protection_adjust = Clamp(stall_protection_adjust + (doPidCalculations(&stallPid, commutation_interval, stall_protect_target_interval)) / 10000, 0, 150);
							}
							if (use_speed_control_loop) {
									input_override = Clamp(input_override + doPidCalculations(&speedPid, e_com_time, target_e_com_time) / 10000, 0, 2047);
									if (zero_crosses < 100) {
											speedPid.integral = 0;
									}
							}
						}
        }

        if (maximum_throttle_change_ramp) {
            //	max_duty_cycle_change = map(k_erpm, low_rpm_level, high_rpm_level, 1, 40);
#ifdef VOLTAGE_BASED_RAMP
            uint16_t voltage_based_max_change = map(battery_voltage, 800, 2200, 10, 1);
            if (average_interval > 200) {
                max_duty_cycle_change = voltage_based_max_change;
            } else {
                max_duty_cycle_change = voltage_based_max_change * 3;
            }
#else
            if (last_duty_cycle < 150) {
                max_duty_cycle_change = RAMP_SPEED_STARTUP;
            } else {
                if (average_interval > 500) {
                    max_duty_cycle_change = RAMP_SPEED_LOW_RPM;
                } else {
                    max_duty_cycle_change = RAMP_SPEED_HIGH_RPM;
                }
            }
#endif		
						
						uint16_t duty_cycle_delta = (duty_cycle - last_duty_cycle);
						fast_accel = (duty_cycle_delta > max_duty_cycle_change ) && ( commutation_interval > 500 );						
						duty_cycle = Clamp(duty_cycle_delta, last_duty_cycle - max_duty_cycle_change, last_duty_cycle + max_duty_cycle_change);
						
        }
        if ((armed && running) && input > 47) {
            if (VARIABLE_PWM) {
            }
             adjusted_duty_cycle = ((duty_cycle * tim1_arr) / TIMER1_MAX_ARR)+1;

        } else {

            if (prop_brake_active) {
                adjusted_duty_cycle = TIMER1_MAX_ARR - duty_cycle;
            } else {
                adjusted_duty_cycle = ((duty_cycle * tim1_arr) / TIMER1_MAX_ARR);
            }
        }

        last_duty_cycle = duty_cycle;
        SET_AUTO_RELOAD_PWM(tim1_arr);
        SET_DUTY_CYCLE_ALL(adjusted_duty_cycle);
    }
#endif // ndef brushed_mode
#if defined(FIXED_DUTY_MODE) || defined(FIXED_SPEED_MODE)
    if (getInputPinState()) {
        signaltimeout++;
        if (signaltimeout > LOOP_FREQUENCY_HZ) {
            NVIC_SystemReset();
        }
    } else {
        signaltimeout = 0;
    }
#else
    signaltimeout++;

#endif
}

void processDshot()
{
    if (compute_dshot_flag == 1) {
        computeDshotDMA();
        compute_dshot_flag = 0;
    }
    if (compute_dshot_flag == 2) {
        make_dshot_package(e_com_time);
        compute_dshot_flag = 0;
        return;
    }
    setInput();
}

void advanceincrement()
{
	phase_A_position = RotateMinMax(phase_A_position, forward ? 1 : -1, 0, 359);
	phase_B_position = RotateMinMax(phase_B_position, forward ? 1 : -1, 0, 359);
	phase_C_position = RotateMinMax(phase_C_position, forward ? 1 : -1, 0, 359);
#ifdef GIMBAL_MODE
    setPWMCompare1(((2 * pwmSin[phase_A_position]) + gate_drive_offset) * TIMER1_MAX_ARR / 2000);
    setPWMCompare2(((2 * pwmSin[phase_B_position]) + gate_drive_offset) * TIMER1_MAX_ARR / 2000);
    setPWMCompare3(((2 * pwmSin[phase_C_position]) + gate_drive_offset) * TIMER1_MAX_ARR / 2000);
#else
    setPWMCompare1((((2 * pwmSin[phase_A_position] / SINE_DIVIDER) + gate_drive_offset) * TIMER1_MAX_ARR / 2000) * sine_mode_power / 10);
    setPWMCompare2((((2 * pwmSin[phase_B_position] / SINE_DIVIDER) + gate_drive_offset) * TIMER1_MAX_ARR / 2000) * sine_mode_power / 10);
    setPWMCompare3((((2 * pwmSin[phase_C_position] / SINE_DIVIDER) + gate_drive_offset) * TIMER1_MAX_ARR / 2000) * sine_mode_power / 10);
#endif
}

void zcfoundroutine()
{ // only used in polling mode, blocking routine.
    thiszctime = INTERVAL_TIMER_COUNT;
    SET_INTERVAL_TIMER_COUNT(0);
    commutation_interval = (thiszctime + (3 * commutation_interval)) / 4;
    advance = commutation_interval / advancedivisor;
    waitTime = commutation_interval / 2 - advance;
    while ((INTERVAL_TIMER_COUNT) < (waitTime)) {
        if (zero_crosses < 10) {
            break;
        }
    }
#ifdef MCU_GDE23
    TIMER_CAR(COM_TIMER) = waitTime;
#endif
#ifdef MCU_F051
		COM_TIMER->ARR = waitTime;
#endif
    commutate();
    bemfcounter = 0;
    bad_count = 0;

    zero_crosses++;
    if (stall_protection || RC_CAR_REVERSE) {
        if (zero_crosses >= 20 && commutation_interval <= 2000) {
            old_routine = 0;
            enableCompInterrupts(); // enable interrupt
        }
    } else {
        if (zero_crosses > 30) {
            old_routine = 0;
            enableCompInterrupts(); // enable interrupt
        }
    }
}
#ifdef BRUSHED_MODE
void runBrushedLoop()
{

    uint16_t brushed_duty_cycle = 0;

    if (brushed_direction_set == 0 && adjusted_input > 48) {
        if (forward) {
            allOff();
            delayMicros(10);
            twoChannelForward();
        } else {
            allOff();
            delayMicros(10);
            twoChannelReverse();
        }
        brushed_direction_set = 1;
    }

    brushed_duty_cycle = map(adjusted_input, 48, 2047, 0, TIMER1_MAX_ARR - 50);

    if (degrees_celsius > TEMPERATURE_LIMIT) {
        duty_cycle_maximum = map(degrees_celsius, TEMPERATURE_LIMIT, TEMPERATURE_LIMIT + 20, TIMER1_MAX_ARR / 2, 1);
    } else {
        duty_cycle_maximum = TIMER1_MAX_ARR - 50;
    }
    if (brushed_duty_cycle > duty_cycle_maximum) {
        brushed_duty_cycle = duty_cycle_maximum;
    }

    if (use_current_limit) {
        use_current_limit_adjust -= (int16_t)(doPidCalculations(&currentPid, actual_current, CURRENT_LIMIT * 100) / 10000);
        if (use_current_limit_adjust < minimum_duty_cycle) {
            use_current_limit_adjust = minimum_duty_cycle;
        }

        if (brushed_duty_cycle > use_current_limit_adjust) {
            brushed_duty_cycle = use_current_limit_adjust;
        }
    }
    if ((brushed_duty_cycle > 0) && armed) {
        SET_DUTY_CYCLE_ALL(brushed_duty_cycle);
        //	  	TIM1->CCR1 = brushed_duty_cycle;
        //		TIM1->CCR2 = brushed_duty_cycle;
        //		TIM1->CCR3 = brushed_duty_cycle;

    } else {
        SET_DUTY_CYCLE_ALL(0);
        //		TIM1->CCR1 = 0;												//
        //		TIM1->CCR2 = 0;
        //		TIM1->CCR3 = 0;
        brushed_direction_set = 0;
    }
}
#endif

int main(void)
{

    initAfterJump();

    initCorePeripherals();

    enableCorePeripherals();

    loadEEpromSettings();
		SetScales();
    EEPROM_VERSION = *(uint8_t*)(0x08000FFC);
#ifdef USE_MAKE
    if (firmware_info.version_major != eepromBuffer[3] || firmware_info.version_minor != eepromBuffer[4]) {
        eepromBuffer[3] = firmware_info.version_major;
        eepromBuffer[4] = firmware_info.version_minor;
        for (int i = 0; i < 12; i++) {
            eepromBuffer[5 + i] = firmware_info.device_name[i];
        }
        saveEEpromSettings();
    }
#else
    if (VERSION_MAJOR != eepromBuffer[3] || VERSION_MINOR != eepromBuffer[4]) {
        eepromBuffer[3] = VERSION_MAJOR;
        eepromBuffer[4] = VERSION_MINOR;
        for (int i = 0; i < 12; i++) {
            eepromBuffer[5 + i] = (uint8_t)FIRMWARE_NAME[i];
        }
        saveEEpromSettings();
    }
#endif

    if (use_sin_start) {
        min_startup_duty = sin_mode_min_s_d;
    }
    if (dir_reversed == 1) {
        forward = 0;
    } else {
        forward = 1;
    }
    tim1_arr = TIMER1_MAX_ARR;
    startup_max_duty_cycle = startup_max_duty_cycle * TIMER1_MAX_ARR / 2000 + dead_time_override; // adjust for pwm frequency
    throttle_max_at_low_rpm = throttle_max_at_low_rpm * TIMER1_MAX_ARR / 2000; // adjust to new pwm frequency
    throttle_max_at_high_rpm = TIMER1_MAX_ARR; // adjust to new pwm frequency
    if (!comp_pwm) {
        use_sin_start = 0; // sine start requires complementary pwm.
    }

    if (RC_CAR_REVERSE) { // overrides a whole lot of things!
        throttle_max_at_low_rpm = 1000;
        bi_direction = 1;
        use_sin_start = 0;
        low_rpm_throttle_limit = 1;
        VARIABLE_PWM = 0;
        // stall_protection = 1;
        comp_pwm = 0;
        stuck_rotor_protection = 0;
        minimum_duty_cycle = minimum_duty_cycle + 50;
        stall_protect_minimum_duty = stall_protect_minimum_duty + 50;
        min_startup_duty = min_startup_duty + 50;
    }

#ifdef MCU_F031
    GPIOF->BSRR = LL_GPIO_PIN_6; // uncomment to take bridge out of standby mode and set oc level
    GPIOF->BRR = LL_GPIO_PIN_7; // out of standby mode
    GPIOA->BRR = LL_GPIO_PIN_11;
#endif

#ifdef USE_LED_STRIP
    send_LED_RGB(125, 0, 0);
#endif

#ifdef USE_CRSF_INPUT
    inputSet = 1;
    playStartupTune();
    MX_IWDG_Init();
    LL_IWDG_ReloadCounter(IWDG);
#else
#if defined(FIXED_DUTY_MODE) || defined(FIXED_SPEED_MODE)
    MX_IWDG_Init();
    RELOAD_WATCHDOG_COUNTER();
    inputSet = 1;
    armed = 1;
    adjusted_input = 48;
    newinput = 48;
#ifdef FIXED_SPEED_MODE
    use_speed_control_loop = 1;
    use_sin_start = 0;
    target_e_com_time = e_comm_target_divisor / FIXED_SPEED_MODE_RPM;
    input = 48;
#endif

#else
#ifdef BRUSHED_MODE
    // bi_direction = 1;
    commutation_interval = 5000;
    use_sin_start = 0;
    maskPhaseInterrupts();
    playBrushedStartupTune();
#else
    playStartupTune();
#endif
    zero_input_count = 0;
    MX_IWDG_Init();
    RELOAD_WATCHDOG_COUNTER();
#ifdef GIMBAL_MODE
    bi_direction = 1;
    use_sin_start = 1;
#endif

#ifdef USE_ADC_INPUT
    armed_count_threshold = 5000;
    inputSet = 1;

#else
    // checkForHighSignal();     // will reboot if signal line is high for 10ms

    receiveDshotDma();
    if (drive_by_rpm) {
        use_speed_control_loop = 1;
    }
#endif

#endif // end fixed duty mode ifdef
#endif // end crsf input

#ifdef MCU_F051
    MCU_Id = DBGMCU->IDCODE &= 0xFFF;
    REV_Id = DBGMCU->IDCODE >> 16;

    if (REV_Id >= 4096) {
        temperature_offset = 0;
    } else {
        temperature_offset = 230;
    }

#endif
#ifdef MCU_G071
setInputPullDown();
#endif
    while (1) {
#ifdef MCU_F031
        if (input_ready) {
            processDshot();
            input_ready = 0;
        }
#endif

        RELOAD_WATCHDOG_COUNTER();
        e_com_time = ((commutation_intervals[0] + commutation_intervals[1] + commutation_intervals[2] + commutation_intervals[3] + commutation_intervals[4] + commutation_intervals[5]) + 4) >> 1; // COMMUTATION INTERVAL IS 0.5US INCREMENTS
        if (VARIABLE_PWM) {
					tim1_arr = map(commutation_interval, 96, 200, TIMER1_MAX_ARR / 2, TIMER1_MAX_ARR);
//      	pwm_frequency_conversion_factor = (tim1_arr << 10) / TIMER1_MAX_ARR; // multply by 1024
			  }
        if (signaltimeout > (LOOP_FREQUENCY_HZ >> 1)) { // half second timeout when armed;
            if (armed) {
                allOff();
                armed = 0;
                input = 0;
                inputSet = 0;
                zero_input_count = 0;
                SET_DUTY_CYCLE_ALL(0);
                resetInputCaptureTimer();
                for (int i = 0; i < 64; i++) {
                    dma_buffer[i] = 0;
                }
                NVIC_SystemReset();
            }
            if (signaltimeout > LOOP_FREQUENCY_HZ << 1) { // 2 second when not armed
                allOff();
                armed = 0;
                input = 0;
                inputSet = 0;
                zero_input_count = 0;
                SET_DUTY_CYCLE_ALL(0);
                resetInputCaptureTimer();
                for (int i = 0; i < 64; i++) {
                    dma_buffer[i] = 0;
                }
                NVIC_SystemReset();
            }
        }
#ifdef USE_CUSTOM_LED
        if ((input >= 47) && (input < 1947)) {
            if (ledcounter > (2000 >> forward)) {
                GPIOB->BSRR = LL_GPIO_PIN_3;
            } else {
                GPIOB->BRR = LL_GPIO_PIN_3;
            }
            if (ledcounter > (4000 >> forward)) {
                ledcounter = 0;
            }
        }
        if (input > 1947) {
            GPIOB->BSRR = LL_GPIO_PIN_3;
        }
        if (input < 47) {
            GPIOB->BRR = LL_GPIO_PIN_3;
        }
#endif

        if (tenkhzcounter > LOOP_FREQUENCY_HZ) { // 1s sample interval 10000
            consumed_current = (float)actual_current / 360 + consumed_current;
            switch (dshot_extended_telemetry) {

            case 1:
                send_extended_dshot = 0b0010 << 8 | degrees_celsius;
                dshot_extended_telemetry = 2;
                break;
            case 2:
                send_extended_dshot = 0b0110 << 8 | (uint8_t)actual_current / 50;
                dshot_extended_telemetry = 3;
                break;
            case 3:
                send_extended_dshot = 0b0100 << 8 | (uint8_t)(battery_voltage / 25);
                dshot_extended_telemetry = 1;
                break;
            }
            tenkhzcounter = 0;
        }

#ifndef BRUSHED_MODE

        if ((zero_crosses > 1000) || (adjusted_input == 0)) {
            bemf_timeout_happened = 0;
        }
        if (zero_crosses > 100 && adjusted_input < 200) {
            bemf_timeout_happened = 0;
        }
        if (use_sin_start && adjusted_input < 160) {
            bemf_timeout_happened = 0;
        }

        if (crawler_mode) {
            if (adjusted_input < 400) {
                bemf_timeout_happened = 0;
            }
        } else {
            if (adjusted_input < 150) { // startup duty cycle should be low enough to not burn motor
                bemf_timeout = 100;
            } else {
                bemf_timeout = 10;
            }
        }
#endif

        average_interval = e_com_time / 3;
        if (desync_check && zero_crosses > 10) {
            //if ((getAbsDif(last_average_interval, average_interval) > average_interval >> 1) && (average_interval < 2000)) { // throttle resitricted before zc 20.
            if ((ABS(last_average_interval - average_interval) > average_interval >> 1) && (average_interval < 2000)) { // throttle resitricted before zc 20.
                zero_crosses = 0;
                desync_happened++;
                if ((!bi_direction && (input > 47)) || commutation_interval > 1000) {
                    running = 0;
                }
                old_routine = 1;
                if (zero_crosses > 100) {
                    average_interval = 5000;
                }
                last_duty_cycle = min_startup_duty / 2;
            }
            desync_check = 0;
            //	}
            last_average_interval = average_interval;
        }

#ifndef MCU_F031
        if (dshot_telemetry && (commutation_interval > DSHOT_PRIORITY_THRESHOLD)) {
            NVIC_SetPriority(IC_DMA_IRQ_NAME, 0);
            NVIC_SetPriority(COM_TIMER_IRQ, 1);
            NVIC_SetPriority(COMPARATOR_IRQ, 1);
        } else {
            NVIC_SetPriority(IC_DMA_IRQ_NAME, 1);
            NVIC_SetPriority(COM_TIMER_IRQ, 0);
            NVIC_SetPriority(COMPARATOR_IRQ, 0);
        }
#endif
        if (send_telemetry) {
#ifdef USE_SERIAL_TELEMETRY
            makeTelemPackage(degrees_celsius,
                battery_voltage,
                actual_current,
                (uint16_t)consumed_current,
                e_rpm);
            send_telem_DMA();
            send_telemetry = 0;
#endif
        }
        adc_counter++;
        if (adc_counter > 200) { // for adc and telemetry
#if defined(MCU_F051) || defined(MCU_G071) || defined(MCU_F031)
            ADC_DMA_Callback();
            ADC_CCR = TIM1->CCR3 * 2 / 3 + 1; // sample current at quarter pwm on
            if (ADC_CCR > tim1_arr) {
                ADC_CCR = tim1_arr;
            }
            TIM1->CCR4 = ADC_CCR;

            LL_ADC_REG_StartConversion(ADC1);
            converted_degrees = __LL_ADC_CALC_TEMPERATURE(3300, ADC_raw_temp, LL_ADC_RESOLUTION_12B);
#endif
#ifdef MCU_GDE23
            ADC_DMA_Callback();
            converted_degrees = (1.43 - ADC_raw_temp * 3.3 / 4096) * 1000 / 4.3 + 25;
            adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
#endif
#ifdef ARTERY
            ADC_DMA_Callback();
            adc_ordinary_software_trigger_enable(ADC1, TRUE);
            //  converted_degrees = (4000 - ADC_raw_temp) / 20;
            converted_degrees = getConvertedDegrees(ADC_raw_temp);
#endif
            degrees_celsius = converted_degrees;
            battery_voltage = ((7 * battery_voltage) + ((ADC_raw_volts * 3300 / 4095 * VOLTAGE_DIVIDER) / 100)) >> 3;
            smoothed_raw_current = getSmoothedCurrent();
            //        smoothed_raw_current = ((63*smoothed_raw_current + (ADC_raw_current) )>>6);
            actual_current = ((smoothed_raw_current * 3300 / 41) - (CURRENT_OFFSET * 100)) / (MILLIVOLT_PER_AMP);
            if (actual_current < 0) {
                actual_current = 0;
            }
            if (LOW_VOLTAGE_CUTOFF) {
                if (battery_voltage < (cell_count * low_cell_volt_cutoff)) {
                    low_voltage_count++;
                    if (low_voltage_count > (20000 - (stepper_sine * 900))) {
                        input = 0;
                        allOff();
                        maskPhaseInterrupts();
                        running = 0;
                        zero_input_count = 0;
                        armed = 0;
                    }
                } else {
                    low_voltage_count = 0;
                }
            }
            adc_counter = 0;
#ifdef USE_ADC_INPUT
            if (ADC_raw_input < 10) {
                zero_input_count++;
            } else {
                zero_input_count = 0;
            }
#endif
        }
#ifdef USE_ADC_INPUT
        signaltimeout = 0;
        ADC_smoothed_input = (((10 * ADC_smoothed_input) + ADC_raw_input) / 11);
        newinput = ADC_smoothed_input / 2;
        if (newinput > 2000) {
            newinput = 2000;
        }
#endif
        stuckcounter = 0;
        if (stepper_sine == 0) {

            e_rpm = running * (600000 / e_com_time); // in tens of rpm
            k_erpm = e_rpm / 10; // ecom time is time for one electrical revolution in microseconds

            if (low_rpm_throttle_limit) { // some hardware doesn't need this, its on by default to keep hardware / motors protected but can slow down the response in the very low end a little.
                duty_cycle_maximum = map(k_erpm, low_rpm_level, high_rpm_level, throttle_max_at_low_rpm, throttle_max_at_high_rpm); // for more performance lower the high_rpm_level, set to a consvervative number in source.
            }

            if (degrees_celsius > TEMPERATURE_LIMIT) {
                duty_cycle_maximum = map(degrees_celsius, TEMPERATURE_LIMIT - 10, TEMPERATURE_LIMIT + 10, throttle_max_at_high_rpm / 2, 1);
            }
            if (zero_crosses < 100 || commutation_interval > 500) {
#ifdef MCU_G071
                TIM1->CCR5 = 50; // comparator blanking
                filter_level = 4;
#else
                filter_level = 12;
#endif
            } else {
#ifdef MCU_G071
                TIM1->CCR5 = 50;
#else
                filter_level = map(average_interval, 100, 500, 3, 12);
#endif
            }
            if (commutation_interval < 100) {
                filter_level = 2;
            }

            if (motor_kv < 500) {

                filter_level = filter_level * 2;
            }

            /**************** old routine*********************/
            // if (old_routine && running){
            //	maskPhaseInterrupts();
            //	 		 getBemfState();
            //	 	  if (!zcfound){
            //	 		  if (rising){
            //	 		 if (bemfcounter > min_bemf_counts_up){
            //	 			 zcfound = 1;
            //	 			 zcfoundroutine();
            //	 		}
            //	 		  }else{
            //	 			  if (bemfcounter > min_bemf_counts_down){
            //  			  			 zcfound = 1;
            //	 		  			 zcfoundroutine();
            //	 			  		}
            //	 		  }
            //	 	  }
            // }
            if (INTERVAL_TIMER_COUNT > 45000 && running == 1) {
                bemf_timeout_happened++;

                maskPhaseInterrupts();
                old_routine = 1;
                if (input < 48) {
                    running = 0;
                    commutation_interval = 5000;
                }
                zero_crosses = 0;
                zcfoundroutine();
            }
        } else { // stepper sine

#ifdef GIMBAL_MODE
            step_delay = 300;
            maskPhaseInterrupts();
            allpwm();
            if (newinput > 1000) {
                desired_angle = map(newinput, 1000, 2000, 180, 360);
            } else {
                desired_angle = map(newinput, 0, 1000, 0, 180);
            }
            if (current_angle > desired_angle) {
                forward = 1;
                advanceincrement();
                delayMicros(step_delay);
                current_angle--;
            }
            if (current_angle < desired_angle) {
                forward = 0;
                advanceincrement();
                delayMicros(step_delay);
                current_angle++;
            }
#else

            if (input > 48 && armed) {

                if (input > 48 && input < 137) { // sine wave stepper

                    if (do_once_sinemode) {
                        // disable commutation interrupt in case set
                        DISABLE_COM_TIMER_INT();
                        maskPhaseInterrupts();
                        SET_DUTY_CYCLE_ALL(0);
                        allpwm();
                        do_once_sinemode = 0;
                    }
                    advanceincrement();
                    step_delay = map(input, 48, 120, 7000 / motor_poles, 810 / motor_poles);
                    delayMicros(step_delay);
                    e_rpm = 600 / step_delay; // in hundreds so 33 e_rpm is 3300 actual erpm

                } else {
                    do_once_sinemode = 1;
                    advanceincrement();
                    if (input > 200) {
                        phase_A_position = 0;
                        step_delay = 80;
                    }

                    delayMicros(step_delay);
                    if (phase_A_position == 0) {
                        stepper_sine = 0;
                        running = 1;
                        old_routine = 1;
                        commutation_interval = 9000;
                        average_interval = 9000;
                        last_average_interval = average_interval;
                        SET_INTERVAL_TIMER_COUNT(9000);
                        zero_crosses = 20;
                        prop_brake_active = 0;
                        step = changeover_step;
                        // comStep(step);// rising bemf on a same as position 0.
                        if (stall_protection) {
                            last_duty_cycle = stall_protect_minimum_duty;
                        }
                        commutate();
                        generatePwmTimerEvent();
                    }
                }

            } else {
                do_once_sinemode = 1;
                if (brake_on_stop) {
#ifndef PWM_ENABLE_BRIDGE
                    duty_cycle = (TIMER1_MAX_ARR - 19) + drag_brake_strength * 2;
                    adjusted_duty_cycle = TIMER1_MAX_ARR - ((duty_cycle * tim1_arr) / TIMER1_MAX_ARR) + 1;
                    proportionalBrake();
                    SET_DUTY_CYCLE_ALL(adjusted_duty_cycle);
                    prop_brake_active = 1;
#else
                    // todo add braking for PWM /enable style bridges.
#endif
                } else {
                    SET_DUTY_CYCLE_ALL(0);
                    allOff();
                }
                e_rpm = 0;
            }

#endif // gimbal mode
        } // stepper/sine mode end
#endif // end of brushless mode

#ifdef BRUSHED_MODE
        runBrushedLoop();
#endif
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
