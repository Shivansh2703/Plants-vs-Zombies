/* Memory */
#define DDR_BASE              0x00000000
#define DDR_END               0x3FFFFFFF
#define A9_ONCHIP_BASE        0xFFFF0000
#define A9_ONCHIP_END         0xFFFFFFFF
#define SDRAM_BASE            0xC0000000
#define SDRAM_END             0xC3FFFFFF
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_ONCHIP_END       0xC803FFFF
#define FPGA_CHAR_BASE        0xC9000000
#define FPGA_CHAR_END         0xC9001FFF

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define JP1_BASE              0xFF200060
#define JP2_BASE              0xFF200070
#define PS2_BASE              0xFF200100
#define PS2_DUAL_BASE         0xFF200108
#define JTAG_UART_BASE        0xFF201000
#define JTAG_UART_2_BASE      0xFF201008
#define IrDA_BASE             0xFF201020
#define TIMER_BASE            0xFF202000
#define TIMER_2_BASE          0xFF202020
#define AV_CONFIG_BASE        0xFF203000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define AUDIO_BASE            0xFF203040
#define VIDEO_IN_BASE         0xFF203060
#define ADC_BASE              0xFF204000

/* ARM A9 MPCORE devices */
#define   PERIPH_BASE         0xFFFEC000    // base address of peripheral devices
#define   MPCORE_PRIV_TIMER   0xFFFEC600    // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF      0xFFFEC100    // PERIPH_BASE + 0x100
#define ICCICR                0x00          // offset to CPU interface control reg
#define ICCPMR                0x04          // offset to interrupt priority mask reg
#define ICCIAR                0x0C          // offset to interrupt acknowledge reg
#define ICCEOIR               0x10          // offset to end of interrupt reg

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

#define INT_DISABLE            0b11000000
#define IRQ_MODE                0b10010
#define SVC_MODE                0b10011
#define INT_ENABLE             0b11000000 

/* ARM A9 MPCORE devices */
#define   PERIPH_BASE         0xFFFEC000    // base address of peripheral devices
#define   MPCORE_PRIV_TIMER   0xFFFEC600    // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST       0xFFFED000    // PERIPH_BASE + 0x1000
#define ICDDCR                0x00          // offset to distributor control reg
#define ICDISER               0x100         // offset to interrupt set-enable regs
#define ICDICER               0x180         // offset to interrupt clear-enable regs
#define ICDIPTR               0x800         // offset to interrupt processor targets regs
#define ICDICFR               0xC00         // offset to interrupt configuration regs

void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_HPS_timer(void);
void config_HPS_GPIO1(void);
void config_interval_timer(void);
void config_KEYs(void);
void enable_A9_interrupts(void);
void config_mouse(void);

int main(void)
{
    set_A9_IRQ_stack(); // initialize the stack pointer for IRQ mode
    config_GIC(); // configure the general interrupt controller
    config_interval_timer(); // configure Altera interval timer to generate
    // interrupts
    config_mouse();
    config_KEYs(); // configure pushbutton KEYs to generate interrupts
    enable_A9_interrupts(); // enable interrupt
}

void config_interval_timer()
{
    volatile int * interval_timer_ptr =
    (int *)TIMER_BASE; // interal timer base address
    /* set the interval timer period for scrolling the HEX displays */
    int counter = 5000000; // 1/(100 MHz) x 5x10^6 = 50 msec
    *(interval_timer_ptr + 0x2) = (counter & 0xFFFF);
    *(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;
    /* start interval timer, enable its interrupts */
    *(interval_timer_ptr + 1) = 0x7; // STOP = 0, START = 1, CONT = 1, ITO = 1
}
    /* setup the KEY interrupts in the FPGA */
void config_KEYs()
{
    volatile int * KEY_ptr = (int *)KEY_BASE; // pushbutton KEY address
    *(KEY_ptr + 2) = 0x3; // enable interrupts for KEY[1]
}

void config_mouse(){
    volatile int* PS2_BASE_ptr = (int *) PS2_BASE;
    //enable interrupts - set the RE bit
    *(PS2_BASE_ptr + 1) = 0x1;
}

//set up stack pointers
void set_A9_IRQ_stack(void)
{
    int stack, mode;
    stack = A9_ONCHIP_END - 7; // top of A9 onchip memory, aligned to 8 bytes
    /* change processor to IRQ mode with interrupts disabled */
    mode = INT_DISABLE | IRQ_MODE;
    asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
    /* set banked stack pointer */
    asm("mov sp, %[ps]" : : [ps] "r"(stack));
    /* go back to SVC mode before executing subroutine return! */
    mode = INT_DISABLE | SVC_MODE;
    asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
}

//allow processor interrupts
void enable_A9_interrupts(void)
{
    int status = SVC_MODE | INT_ENABLE;
    asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}

void config_GIC(void)
{
    int address; // used to calculate register addresses
    /* configure the HPS timer interrupt */
    *((int *)0xFFFED8C4) = 0x01000000;
    *((int *)0xFFFED118) = 0x00000080;
    /* configure the FPGA interval timer and KEYs interrupts */
    *((int *)0xFFFED848) = 0x00000101;
    *((int *)0xFFFED108) = 0x00000300;
    // Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all
    // priorities
    address = MPCORE_GIC_CPUIF + ICCPMR;
    *((int *)address) = 0xFFFF;
    // Set CPU Interface Control Register (ICCICR). Enable signaling of
    // interrupts
    address = MPCORE_GIC_CPUIF + ICCICR;
    *((int *)address) = 0xFFFF;  //enable
    // Configure the Distributor Control Register (ICDDCR) to send pending
    // interrupts to CPUs
    address = MPCORE_GIC_DIST + ICDDCR;
    *((int *)address) = 0xFFFF; //enable
}




