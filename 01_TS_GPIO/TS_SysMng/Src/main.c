/***********************************************************************************
 * File                     :main.c
 *
 * Title                    :
 *
 * Author                   :Tarik SEMRADE
 *
 * Description              :This example  toggles led red and blue for 500ms each.
 *                           500ms = 500 * 1e-3 = 500000 * 1e-6 = 1000 * 500 for us_delay macro.
 *                           External oscillator 10Mhz.
 *                           CPU frequency 200MHz.
 *
 * Version                  : 0.1
 *
 * Copyright (c) 2020 Tarik SEMRADE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *********************************************************************************/

/**********************************************************************************
 *  Included Files
 *
 *********************************************************************************/

#include "F28x_Project.h"
#include "gpio.h"
#include "device.h"
#include "main.h"

/**********************************************************************************
 *  Defines
 *
 *********************************************************************************/
#define BLEU_LED        DEVICE_GPIO_PIN_LED1
#define RED_LED         DEVICE_GPIO_PIN_LED2
#define GPIO_CONF_SW    1U

/**********************************************************************************
 * \function:       main
 * \brief           main `0` numbers
 * \param[in]       void
 * \return          void
 **********************************************************************************/
void
main (void)
{

    /* Set up system flash and turn peripheral clocks */
    InitSysCtrl();

    /* GPIO Init */
    InitGpio();

    /* Globally disable maskable CPU interrupts */
    DINT;

    /* Clear and disable all PIE interrupts */
    InitPieCtrl();

    /* Individually disable maskable CPU interrupts */
    IER = 0x0000;

    /* Clear all CPU interrupt flags */
    IFR = 0x0000;

    /* Populate the PIE interrupt vector table with */
    InitPieVectTable();

    /* Init Led function */
    GpioLedInit();

    /* Infinite led loop */
    while (1)
    {

#if (1U == GPIO_CONF_SW)

        /* Toggle bleu led and wait 500ms */
        GPIO_togglePin(BLEU_LED);
        DELAY_US(500*1000);

        /* Toggle red led and wait 500ms */
        GPIO_togglePin(RED_LED);
        DELAY_US(500*1000);
#else
        /* Toggle Bleu led and wait 500ms */
        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
        DELAY_US(500*1000);

        /* Toggle Red led and wait 500ms */
        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
        DELAY_US(500*1000);

#endif

    }

}
/**********************************************************************************
 * \function:       GpioLedInit
 * \brief           `0` Param
 * \param[in]       void
 * \return          void
 **********************************************************************************/
void
GpioLedInit (void)
{

    /*
     *  Port A consists of GPIO0-GPIO31
        Port B consists of GPIO32-GPIO63
        Port C consists of GPIO64-GPIO95
        Port D consists of GPIO96-GPIO127
        Port E consists of GPIO128-GPIO159
        Port F consists of GPIO160-GPIO168
     */

#if (GPIO_CONF_SW == 1U)


    /* GPIO bleuLed configuration using TI function */
    GPIO_SetupPinMux(BLEU_LED,GPIO_MUX_CPU1,0);
    GPIO_SetupPinOptions(BLEU_LED, GPIO_OUTPUT, GPIO_ASYNC);

    /* GPIO RedLed configuration using TI function */
    GPIO_SetupPinMux(RED_LED,GPIO_MUX_CPU1,0);
    GPIO_SetupPinOptions(RED_LED, GPIO_OUTPUT, GPIO_ASYNC);


#else

    /* GPIO Register configuration */
    EALLOW;                                   /* Enable EALLOW protected register access                    */
    /* Group A pins */
    GpioCtrlRegs.GPACTRL.all    = 0x00000000; /* QUALPRD = PLLSYSCLK for all group A GPIO                   */
    GpioCtrlRegs.GPAQSEL1.all   = 0x00000000; /* Synchronous qualification for all group A GPIO 0-15        */
    GpioCtrlRegs.GPAQSEL2.all   = 0x00000000; /* Synchronous qualification for all group A GPIO 16-31       */
    GpioCtrlRegs.GPADIR.all     = 0x00000000; /* All GPIO are inputs                                        */
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;       /* GPIO31 should be set to output to write blue led           */
    GpioCtrlRegs.GPAPUD.all     = 0x00000000; /* All pullups enabled                                        */
    GpioCtrlRegs.GPAINV.all     = 0x00000000; /* No inputs inverted                                         */
    GpioCtrlRegs.GPAODR.all     = 0x00000000; /* All outputs normal mode (no open-drain outputs)            */
    GpioCtrlRegs.GPACSEL1.all   = 0x00000000; /* GPIO 0-7   \.                                              */
    GpioCtrlRegs.GPACSEL2.all   = 0x00000000; /* GPIO 8-15   |. GPIODAT/SET/CLEAR/TOGGLE reg. master:       */
    GpioCtrlRegs.GPACSEL3.all   = 0x00000000; /* GPIO 16-23  |. 0=CPU1, 1=CPU1.CLA1, 2=CPU2, 3=CPU2.CLA1    */
    GpioCtrlRegs.GPACSEL4.all   = 0x00000000; /* GPIO 24-31 /.                                              */

    /********************************************************************************************************/
    /* GPIO 31 for red led                GPyGMUXn|GPyMUXn n(1,2) and    X1(0..15),X2(16..31)               */
    GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0;     /* 0|0=GPIO  0|1=CANTXA       0|2=EM1WEn       0|3=rsvd       */
    GpioCtrlRegs.GPAMUX2.bit.GPIO31  = 0;     /* 1|0=GPIO  1|1=OUTPUTXBAR8  1|2=EQEP3I       1|3=SD2_C4     */
                                              /* 2|0=GPIO  2|1=rsvd         2|2=rsvd         2|3=rsvd       */
                                              /* 3|0=GPIO  3|1=rsvd         3|2=rsvd         3|3=rsvd       */

    /* Group B pins */
    GpioCtrlRegs.GPBCTRL.all    = 0x00000000; /* QUALPRD = PLLSYSCLK for all group B GPIO                   */
    GpioCtrlRegs.GPBQSEL1.all   = 0x00000000; /* Synchronous qualification for all group B GPIO 32-47       */
    GpioCtrlRegs.GPBQSEL2.all   = 0x00000000; /* Synchronous qualification for all group B GPIO 48-63       */
    GpioCtrlRegs.GPBDIR.all     = 0x00000000; /* All group B GPIO are inputs                                */
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;       /* GPIO34 should be output to write red Led                   */
    GpioCtrlRegs.GPBPUD.all     = 0x00000000; /* All group B pullups enabled                                */
    GpioCtrlRegs.GPBINV.all     = 0x00000000; /* No inputs inverted                                         */
    GpioCtrlRegs.GPBODR.all     = 0x00000000; /* All outputs normal mode (no open-drain outputs)            */
    GpioCtrlRegs.GPBCSEL1.all   = 0x00000000; /* GPIO 32-39 \.                                              */
    GpioCtrlRegs.GPBCSEL2.all   = 0x00000000; /* GPIO 40-47  |. GPIODAT/SET/CLEAR/TOGGLE reg. master:       */
    GpioCtrlRegs.GPBCSEL3.all   = 0x00000000; /* GPIO 48-55  |. 0=CPU1, 1=CPU1.CLA1, 2=CPU2, 3=CPU2.CLA1    */
    GpioCtrlRegs.GPBCSEL4.all   = 0x00000000; /* GPIO 56-63 /.                                              */

    /********************************************************************************************************/
    /* GPIO 34 for red led                GPyGMUXn|GPyMUXn n(1,2) and    X1(0..15),X2(16..31)               */
    GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;     /* 0|0=GPIO  0|1=OUTPUTXBAR1  0|2=EM1CS2n      0|3=rsvd       */
    GpioCtrlRegs.GPBMUX1.bit.GPIO34  = 0;     /* 1|0=GPIO  1|1=rsvd         1|2=SDAB         1|3=rsvd       */
                                              /* 2|0=GPIO  2|1=rsvd         2|2=rsvd         2|3=rsvd       */
                                              /* 3|0=GPIO  3|1=rsvd         3|2=rsvd         3|3=rsvd       */

    EDIS;                                     /* Disable EALLOW protected register access                   */

#endif
}
