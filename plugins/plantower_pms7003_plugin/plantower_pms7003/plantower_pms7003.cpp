#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "kidbright32.h"
#include "plantower_pms7003.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"


#define UART_NUM				UART_NUM_1
#define UART_BAUDRATE			 9600
#define UART_LINETERM			'\n'
#define TX_BUF_SIZE				1024
#define RX_BUF_SIZE				1024
#define HPM_MAX_RESP_SIZE 		8

enum STATUS1 {
  STATUS_WAITING,
  STATUS_OK };

STATUS1 _status;
uint8_t _index = 0;
uint16_t _calculatedChecksum;
uint16_t _frameLen=0;
uint16_t _checksum=0;
uint8_t _payload[12];

struct DATA {
  // Standard Particles, CF=1
  uint16_t PM_SP_UG_1_0;
  uint16_t PM_SP_UG_2_5;
  uint16_t PM_SP_UG_10_0;

  // Atmospheric environment
  uint16_t PM_AE_UG_1_0;
  uint16_t PM_AE_UG_2_5;
  uint16_t PM_AE_UG_10_0;
};

DATA _data;

unsigned int makeWord(unsigned char h, unsigned char l)
{
    return (h << 8) | l;
}


void loop(void)
{
  int len, index;
  uint8_t old_byte=0;
  _status = STATUS_WAITING;
  //vTaskDelay(100 / portTICK_RATE_MS);
  uint8_t respBuf[1];///
  //stream.available()
  ///available() gets the number of bytes available in the stream. This is only for bytes that have already arrived.
  len = uart_read_bytes(UART_NUM, respBuf, 1, (20 / portTICK_RATE_MS));
  if (len > 0)
  {
    printf("\n%x, len%d",respBuf[0], len);
    uint8_t ch = respBuf[0];

    if((respBuf[0] == 0x4d)&&(old_byte==0x42)){
      printf("--<>--HEADER");
      ///fprintf(stdout, "STARTED!");
    }
    old_byte = respBuf[0];

    switch(_index)
    {
      case 0:
        if (ch != 0x42)
        {
          return;
        }
        _calculatedChecksum = ch;
        break;

      case 1:
        if (ch != 0x4D)
        {
          _index = 0;
          return;
        }
        _calculatedChecksum += ch;
        break;

        case 2:
          _calculatedChecksum += ch;
          _frameLen = ch << 8;
          break;

        case 3:
          _frameLen |= ch;
          // Unsupported sensor, different frame length, transmission error e.t.c.
          if (_frameLen != 2 * 9 + 2 && _frameLen != 2 * 13 + 2)
          {
            _index = 0;
            return;
          }
          _calculatedChecksum += ch;
          break;

        default:
          if ( _index == (_frameLen + 2) )
          {
            _checksum = ch << 8;
          }
          else if ( _index == (_frameLen + 2 + 1) )
          {
            _checksum |= ch;

            if (_calculatedChecksum == _checksum)
            {
              _status = STATUS_OK;

              // Standard Particles, CF=1.
              _data.PM_SP_UG_1_0 = makeWord(_payload[0], _payload[1]);///
              _data.PM_SP_UG_2_5 = makeWord(_payload[2], _payload[3]);///
              _data.PM_SP_UG_10_0 = makeWord(_payload[4], _payload[5]);///
              printf("\tPM2.5=%d",_data.PM_SP_UG_2_5);
              printf("\tPM10.0=%d",_data.PM_SP_UG_10_0);
              ///pm2_5 = _data.PM_SP_UG_2_5;
              // Atmospheric Environment.
              _data.PM_AE_UG_1_0 = makeWord(_payload[6], _payload[7]);///
              _data.PM_AE_UG_2_5 = makeWord(_payload[8], _payload[9]);///
              _data.PM_AE_UG_10_0 = makeWord(_payload[10], _payload[11]);///

            }

            _index = 0;
            return;
          }
          else
          {

            _calculatedChecksum += ch;
            uint8_t payloadIndex = _index - 4;

            // Payload is common to all sensors (first 2x6 bytes).
            if (payloadIndex < sizeof(_payload))
            {
              _payload[payloadIndex] = ch;
            }
          }

          break;

    }///end of switch(_index)
    _index++;
  }//end of if(len>0)
}

void vPMS7003Task(void *pvParameters) {
	plantower_pms7003 *pmx = (plantower_pms7003 *)pvParameters;




	vTaskDelay(500 / portTICK_RATE_MS);
	int k = 0;
	int collect = 0;
	uint8_t pm25;
	uint8_t pm10;

	while (1)
  {
    loop();
  }
	vTaskDelete(NULL);
}

plantower_pms7003::plantower_pms7003(void)///int tx_io, int rx_io){
{
	///tx = tx_io;
	///rx = rx_io;
	tx = GPIO_NUM_19;
  //rx = GPIO_NUM_23;
	 rx = GPIO_NUM_4; 
}

void plantower_pms7003::init(void) {
	pm2_5 = 0;
	pm10 = 0;

	// uart init
	uart_config_t uart_config = {
		.baud_rate = UART_BAUDRATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 0,
		.use_ref_tick = true
	};

	// uart config
	uart_param_config(UART_NUM, &uart_config);
	// set uart pins   tx=GPIO_NUM_22 rx=GPIO_NUM_23
	uart_set_pin(UART_NUM, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

	uart_set_line_inverse(UART_NUM, UART_INVERSE_TXD);


	// install uart driver
	uart_driver_install(UART_NUM, (RX_BUF_SIZE * 2), (TX_BUF_SIZE * 2), 0, NULL, 0);
	// uart task
	xTaskCreatePinnedToCore(vPMS7003Task, "pms7003 Task", IO_DEV_STACK_SIZE_MIN, this, IO_DEV_TASK_PRIORITY, NULL, KIDBRIGHT_RUNNING_CORE);

	initialized = true;
	error = false;
}

int plantower_pms7003::prop_count(void) {
	return 2;
}

bool plantower_pms7003::prop_name(int index, char *name) {
	if (index == 0) {
		snprintf(name, DEVICE_PROP_NAME_LEN_MAX, "%s", "pm2_5");
		return true;
	}
	else
	if (index == 1) {
		snprintf(name, DEVICE_PROP_NAME_LEN_MAX, "%s", "pm10");
		return true;
	}

	// not supported
	return false;
}

bool plantower_pms7003::prop_unit(int index, char *unit) {
	if (index == 0) {
		snprintf(unit, DEVICE_PROP_UNIT_LEN_MAX, "%s", "ug/m3");
		return true;
	}
	else
	if (index == 1) {
		snprintf(unit, DEVICE_PROP_UNIT_LEN_MAX, "%s", "%ug/m3");
		return true;
	}

	// not supported
	return false;
}

bool plantower_pms7003::prop_attr(int index, char *attr) {
	if ((index == 0) || (index == 1)) {
		get_attr_str(attr, PROP_ATTR_READ_FLAG | PROP_ATTR_TYPE_NUMBER_FLAG); // read only, number
		return true;
	}

	// not supported
	return false;
}

bool plantower_pms7003::prop_read(int index, char *value) {
	if (index == 0) {
		snprintf(value, DEVICE_PROP_VALUE_LEN_MAX, "%f", pm2_5);
		return true;
	}
	else
	if (index == 1) {
		snprintf(value, DEVICE_PROP_VALUE_LEN_MAX, "%f", pm10);
		return true;
	}

	// not supported
	return false;
}

bool plantower_pms7003::prop_write(int index, char *value) {
	// not supported
	return false;
}



void plantower_pms7003::process(Driver *drv) {

}

double plantower_pms7003::get_pm2_5(void) {
  pm2_5 = _data.PM_SP_UG_2_5;
	return pm2_5;
}

double plantower_pms7003::get_pm10(void) {
  pm10 = _data.PM_SP_UG_10_0;
	return pm10;
}
