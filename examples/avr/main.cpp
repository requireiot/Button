#include <avr/io.h>

#include <stdpins.h>
#include <AvrTimers.h>
#include <Button.h>


#define BUTTON_1  D,2,ACTIVE_LOW
#define BUTTON_2  D,3,ACTIVE_LOW
#define LED_1     C,0,ACTIVE_HIGH
#define LED_2     C,1,ACTIVE_HIGH

AvrTimer2 timer2;
// example of a debouncer without nowledge or port/pin
Button button1;
// example of a debouncer with knowledge or port/pin
ButtonPin button2( & PIN(BUTTON_2), portBIT(BUTTON_2) );


void myISR( void* )
{
    button1.tick( IS_TRUE(BUTTON_1) );
}


int main()
{
    AS_OUTPUT( LED_1 );
    AS_OUTPUT( LED_2 );
    AS_INPUT_PU( BUTTON_1 );
    AS_INPUT_PU( BUTTON_1 );

    timer2.begin(1000);
    timer2.add_task( Button::MS_PER_TICK, myISR );
    timer2.add_task( Button::MS_PER_TICK, Button::isr, &button2 );
    timer2.start();

    for (;;) {
        SET_PA( LED_1, button1.isDown );
        SET_PA( LED_2, button2.isDown );
    }

}
