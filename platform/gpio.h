#ifndef GPIO_H__
#define GPIO_H__


//  #define examples:
// AVR:
//      LED on port B, bit 1, active level high,
//      key on port C, bit 6, active level low
// #define  LED    PORTB, 1, H
// #define  KEY    PINC, 6, L
//
//            usage examples:
// #include "gpio.h"
//
// on(LED);     	// turn LED on
// off(LED);     	// turn LED off
// tg(LED);     	// toggle LED
//
// if ( is_bit_active(KEY) )   // if key pressed
// {
//    ............
// }
//
// if ( !is_bit_active(KEY) )   // if key released
// {
//    ............
// }
//



#define _setL(port,bit)         do { port &= ~(1 << bit); } while(0)
#define _setH(port,bit)         do { port |= (1 << bit); } while(0)
#define _clrL(port,bit)         do { port |= (1 << bit); } while(0)
#define _clrH(port,bit)         do { port &= ~(1 << bit); } while(0)
#define _bitL(port,bit)         (!(port & (1 << bit)))
#define _bitH(port,bit)         (port & (1 << bit))
#define _cpl(port,bit,val)      do {port ^= (1 << bit); } while(0)


#define dir_out(x)              _dir_out (x)
#define dir_in(x)               _dir_in (x)
#define dir(x)                  _dir (x)

#define _set(port,bit,val)      _set##val(port,bit)
#define on(x)                   _set (x)

#define _clr(port,bit,val)      _clr##val(port,bit)
#define off(x)                  _clr (x)

#define _bit(port,bit,val)      _bit##val(port,bit)
#define is_bit_active(x)               (!! _bit (x))

#define tg(x)                  _cpl (x)

#endif  //  GPIO_H__
