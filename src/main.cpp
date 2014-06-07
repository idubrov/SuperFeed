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
//encoder encoder::g_instance;
//keypad keypad::g_instance;
driver driver::g_instance;
//lcd lcd::g_instance;
//stepper g_stepper;

//void digit(int dig) {
//	lcd::print('0' + dig);
//}

//		// FIXME: For CN0162 setup time is 10usec at least, need to check...
//
//
//		Port->BSRR = StepPin;
//		// Wait at least 75ns (for IMS483). Each NOP should be ~40ns (1/24000000 sec)
//		__NOP();
//		__NOP();
//
//		// for CN0162, wait time should be 300ns at least
//		//__NOP(); __NOP(); __NOP(); __NOP();
//		//__NOP(); __NOP(); __NOP(); __NOP();
//
//		Port->BRR = StepPin;

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main()
{
	bool dir = true;
	while(1) {
		for (int i = 0; i < 10000; i++) {
			driver::step(Bit_SET);
			util::led4_on();
			util::delay_us(50);
			driver::step(Bit_RESET);
			util::led4_off();
			util::delay_us(50);
		}

		util::delay_ms(1);
		driver::direction(dir ? Bit_SET : Bit_RESET);
		dir = !dir;
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

