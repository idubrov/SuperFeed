#include "main.hpp"
#include "encoder.hpp"
#include "5switch.hpp"
#include "keypad.hpp"
#include "util.hpp"
#include "lcd.hpp"
#include "driver.hpp"
#include "stepper.hpp"

// Initialize subsystems
util util::g_instance;
//switch5 switch5::g_instance;
encoder encoder::g_instance;
//keypad keypad::g_instance;
//driver driver::g_instance;
//lcd lcd::g_instance;
//stepper g_stepper;

void digit(int dig) {
	lcd::print('0' + dig);
}

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main()
{
	encoder::limit(10);

	while(1) {
		//int pos = encoder::position();
		int pos = 10;
//		if (encoder::pressed()) {
//			util::led4_on();
//		} else {
//			util::led4_off();
//		}
		for (int i = 0; i < pos; i++) {
			util::led3_on();
			util::delay_ms(500);
			util::led3_off();
			util::delay_ms(500);
		}
		util::delay_ms(1000);
	}
//	lcd::display(lcd::DisplayOn, lcd::CursorOff, lcd::BlinkOff);
//	while (1) {
//		lcd::position(0, 0);
//		if (encoder::pressed()) {
//			lcd::print("P ");
//			encoder::limit(10);
//		} else {
//			lcd::print("N ");
//		}
//
//		digit(switch5::position());
//		lcd::print(" ");
//		digit(GPIOC->IDR & GPIO_Pin_10 ? 1 : 0);
//		digit(GPIOC->IDR & GPIO_Pin_11 ? 1 : 0);
//		digit(GPIOC->IDR & GPIO_Pin_12 ? 1 : 0);
//
////		uint16_t position = encoder::position();
////		digit((position / 1000) % 10);
////		digit((position / 100) % 10);
////		digit((position / 10) % 10);
////		digit((position / 1) % 10);
//
//		util::delay_ms(100);
//	}
	return 0;
}

extern "C"
void __attribute__ ((section(".after_vectors")))
SysTick_Handler(void)
{
	encoder::scan();
}

