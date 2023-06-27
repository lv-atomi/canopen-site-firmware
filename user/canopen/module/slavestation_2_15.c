#includes "slavestation_2_15.h"

/*
 * Slaveboard model 1, can station 2, 15:
 * -GPinput x8
 *     + 6 for limit switch of XYZ system: X-/+, Y-/+, Z-/+
 *     + 2 for limit switch of cutter: P1/ P2
 * -Cutter motor driver
 *     + Hbridge: speed control
 *     + Brushless: speed sense, speed control
 *
 */

/*
 * Motor
 * PB8 -> Speed sense   TMR4_CH3 / TMR10_CH1
 * PB7 -> Direction
 * PB6 -> Speed Control TMR4_CH1
 *
 * XYZ三轴的左和右限位
 * PA3 -> CN4,
 * PB0 -> CN5,
 * PB1 -> CN6,
 * PB10 -> CN7,
 * PB11 -> CN8,
 * PB12 -> CN9,
 * 切割限位，分别是左和右
 * PA2 -> CN3,
 * PA1 -> CN2,
 */

void motor_init(void) {
  gpio_init_type gpio_init_struct;
  tmr_baseinit_type tmr_base_init_structure;
  tmr_ocinit_type tmr_oc_init_structure;
  tmr_icinit_type tmr_ic_init_structure;

  // Enable clocks
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR10_PERIPH_CLOCK, TRUE);

  // Init structure for GPIO
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_mode = GPIO_MODE_AF_PP;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_pins = GPIO_PINS_6 | GPIO_PINS_8; // PB6 and PB8
  gpio_init(GPIOB, &gpio_init_struct);

  // Init structure for DIR (PB7)
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_7; // PB7
  gpio_init(GPIOB, &gpio_init_struct);

  // Init structure for PWM output
  tmr_base_default_para_init(&tmr_base_init_structure);
  tmr_base_init_structure.period = 20000;
  tmr_base_init_structure.prescaler = (SystemCoreClock / 20000000) - 1;
  tmr_base_init_structure.repetition_counter = 0;

  tmr_oc_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OC_MODE_PWM_1;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = ENABLE;
  tmr_oc_init_structure.pulse = 10000; // 50% Duty cycle

  // Init TMR4 for PWM
  tmr_base_init(TMR4, &tmr_base_init_structure);
  tmr_oc_config(TMR4, TMR_CHANNEL_1,
                &tmr_oc_init_structure); // Channel 1 for Speed Control

  // Init TMR10 for RPM sensing
  tmr_base_init(TMR10, &tmr_base_init_structure);
  tmr_oc_config(TMR10, TMR_CHANNEL_1,
                &tmr_oc_init_structure); // Channel 1 for RPM sensing

  // Enable timer counter for TMR4 and TMR10
  tmr_counter_enable(TMR4, TRUE);
  tmr_counter_enable(TMR10, TRUE);
}

void limit_switches_init(void)
{
  gpio_init_type gpio_init_struct;

  // Enable clocks
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  // Init structure for GPIO
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;

  // Initialize PA1, PA2, and PA3
  gpio_init_struct.gpio_pins = GPIO_PINS_1 | GPIO_PINS_2 | GPIO_PINS_3;
  gpio_init(GPIOA, &gpio_init_struct);

  // Initialize PB0, PB1, PB10, PB11, PB12
  gpio_init_struct.gpio_pins =
      GPIO_PINS_0 | GPIO_PINS_1 | GPIO_PINS_10 | GPIO_PINS_11 | GPIO_PINS_12;
  gpio_init(GPIOB, &gpio_init_struct);
}

void init_slavestation_2_15(){
  init_motor();
  init_limit_switch();
}

OD_extension_t OD_6100_extension;  // Trigger Input X8
OD_extension_t OD_6101_extension;  // Motor

// Read function for Trigger Input X8
static ODR_t my_OD_read_6100(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead) {
    printf("read 6100, subidx:%d\n", stream->subIndex);

    if (stream->subIndex == 0) {
        CO_setUint8(buf, 8);  // Max subindex
        *countRead = sizeof(uint8_t);
    } else if (stream->subIndex >= 1 && stream->subIndex <= 8) {
        bool_t value = // Read your actual value here.
        CO_setUint8(buf, value);
        *countRead = sizeof(bool_t);
    } else {
        return ODR_OUT_OF_RANGE;  // Invalid subindex
    }

    return ODR_OK;
}

// Read function for Motor
static ODR_t my_OD_read_6101(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead) {
    printf("read 6101, subidx:%d\n", stream->subIndex);
    
    switch (stream->subIndex) {
        case 0:
            CO_setUint8(buf, 4);  // Max subindex
            *countRead = sizeof(uint8_t);
            break;
        case 1:
        {
            bool_t value = // Read your actual value here.
            CO_setUint8(buf, value);
            *countRead = sizeof(bool_t);
            break;
        }
        case 2:
        case 3:
        {
            int32_t value = // Read your actual value here.
            CO_setInt32(buf, value);
            *countRead = sizeof(int32_t);
            break;
        }
        case 4:
        {
            int32_t value = // Read your actual value here.
            CO_setInt32(buf, value);
            *countRead = sizeof(int32_t);
            break;
        }
        default:
            return ODR_OUT_OF_RANGE;  // Invalid subindex
    }

    return ODR_OK;
}

// Write function for Motor
static ODR_t my_OD_write_6101(OD_stream_t *stream, const void *buf, OD_size_t count, OD_size_t *countWritten) {
    printf("write 6101, subidx:%d\n", stream->subIndex);

    switch (stream->subIndex) {
        case 0:
            return ODR_NO_WRITEABLE;  // Subindex 0 is read-only
        case 1:
        {
            bool_t value = CO_getUint8(buf);
            // Write your actual value here.
            break;
        }
        case 2:
        case 3:
        {
            int32_t value = CO_getInt32(buf);
            // Write your actual value here.
            break;
        }
        case 4:
            return ODR_NO_WRITEABLE;  // Subindex 4 is read-only
        default:
            return ODR_OUT_OF_RANGE;  // Invalid subindex
    }

    return ODR_OK;
}

CO_ReturnError_t io_module_trigger_input_motor_init() {
    OD_entry_t *param_6100 = OD_ENTRY_H6100_triggerInputX8;
    OD_entry_t *param_6101 = OD_ENTRY_H6101_motor;

    OD_6100_extension.object = param_6100;
    OD_6100_extension.read = my_OD_read_6100;
    OD_6100_extension.write = my_OD_write_6100;

    OD_6101_extension.object = param_6101;
    OD_6101_extension.read = my_OD_read_6101;
    OD_6101_extension.write = my_OD_write_6101;

    if (OD_extension_init(param_6100, &OD_6100_extension) != CO_ERROR_NO) {
        log_printf("ERROR, unable to extend OD object 6100\n");
        return CO_ERROR_OD;
    }
  
    if (OD_extension_init(param_6101, &OD_6101_extension) != CO_ERROR_NO) {
        log_printf("ERROR, unable to extend OD object 6101\n");
        return CO_ERROR_OD;
    }
  
    return CO_ERROR_NO;
}
