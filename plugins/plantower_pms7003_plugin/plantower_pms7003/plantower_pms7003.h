#ifndef __plantower_pms7003_H__
#define __plantower_pms7003_H__

#include "driver.h"
#include "device.h"

// #define plantower_pms7003_POLLING_MS         1000

class plantower_pms7003 : public Device {
    public:

	  ///bool read(DATA& data);

		// constructor
		plantower_pms7003(void);///int tx_io, int rx_io);
		// override
		void init(void);
		void process(Driver *drv);
		int  prop_count(void);
		bool prop_name(int index, char *name);
		bool prop_unit(int index, char *unit);
		bool prop_attr(int index, char *attr);
		bool prop_read(int index, char *value);
		bool prop_write(int index, char *value);
		// method
		double get_pm2_5(void);
		double get_pm10(void);
		double pm2_5;
		double pm10;

		///mmm-->
		static const uint16_t SINGLE_RESPONSE_TIME = 1000;
	  static const uint16_t TOTAL_RESPONSE_TIME = 1000 * 10;
	  static const uint16_t STEADY_RESPONSE_TIME = 1000 * 30;

	  static const uint16_t BAUD_RATE = 9600;
		

	private:
		TickType_t tickcnt, polling_tickcnt;
		int tx;
		int rx;
		///-->mmm
		///-->nnn

		///void loop();

};

#endif
