#include <cpu/port.h>

#include "controller.h"

/**
 * Control functions and data definitions for the 8259A interrupt controller.
 *
 * The 8259A interrupt controller is connected through cpu port like the PS/2 keyboard.
 *
 * Two chips are connected in master-slave manner and each chip can handle 8 types of interrupts so
 * the number of interrupt requests we can handle is 15.
 *
 * For detailed description, check out the 8259A data sheet published by the Intel.
 */

/**
 * Start the initialization sequence.
 *
 * When the interrupt controller receives this command through `pic_master0` or `pic_slave0` port,
 * the corresponding pic starts the initialization sequence.
 *
 * The controller then expects following ICW2, ICW3, and ICW4 commands to initialize it's internal
 * state.
 *
 * If it receives other value or ICW1 again, the initialization sequence halts and the controller
 * will be reset just like before the sequence.
 *
 * Each bit of this command has following meanings:
 * [0]: Use ICW4 command. We need to set this bit to enable 8086 mode using ICW4 command.
 * [1]: Use single PIC. Clear this bit because PIC for the pc use cascade(master-slave) strcuture.
 * [2]: This bit has to be 0.
 * [3]: Use level trigger way. Clear this bit because interrupts of PC use edge trigger way.
 * [4]: This bit has to be set.
 * [5, 7]: These bits have to be cleared.
 */
#define ICW1        (0x11)
/**
 * Set the base interrupt vector value of master pic 32.
 *
 * Note that this command should be sent through `pic_master1`.
 *
 * Each bit of this command has following meanings:
 * [0, 2]: These bits have to be cleared.
 * [3, 7]: Base offset of the interrupt vector.
 */
#define ICW2_MASTER (0x20)
/**
 * Set the base interrupt vector value of slave pic 40.
 *
 * Note that this command should be sent through `pic_slave1`.
 *
 * The bits are same as bits in `ICW2_MASTER`.
 *
 * @see ICW2_MASTER
 */
#define ICW2_SLAVE  (0x28)
/**
 * Set the pin number that connects master PIC to slave PIC.
 *
 * Each bit represents connection to the slave PIC.
 *
 * We need to set bit 2 because the slave PIC is connected through pin 2 in PC.
 */
#define ICW3_MASTER (0x04)
/**
 * Set the pin number that connects slave PIC to master PIC.
 *
 * Each bit of this command has following meanings:
 * [0, 2]: Pin number that connects this PIC to master PIC. We need to set this value 2.
 * [3, 7]: These bits have to be cleared.
 *
 * @see ICW3_MASTER
 */
#define ICW3_SLAVE  (0x02)
/**
 * Finish the initialization sequence.
 *
 * Each bit in this command has following meanings:
 * [0]: Set 8086 mode. If this bit is cleared, the PIC runs in MCS-80/85 mode.
 * [1]: Send "End of interrupt" message to the PIC automatically.
 * [2]: Select master PIC or slave PIC. This bit has meaning only when in the buffer mode.
 * [3]: Set buffer mode. Clear this bit because we don't use buffer in the PC.
 * [4]: Use "Special Fully Nested Mode". Clear this bit because we don't use this mode in the PC.
 * [5, 7]: These bits have to be cleared.
 */
#define ICW4        (0x01)
/**
 * Mask out specific interrupts.
 *
 * Each bit in this command represents whether to ignore interrupt requests on the corresponding
 * pin. If we clear bit 0 then interrupt request on the pin 0 will be ignored.
 */
#define OCW1        (0x00)
/**
 * Notify the end of interrupt.
 *
 * Each bit in this command has following meanings:
 * [0, 2]: Select specific pin.
 * [3, 4]: These bits have to be cleared.
 * [5]:
 * [6]: End specific level. Notify end of the interrupt specified using bits [0, 2].
 * [7]:
 *
 * When the bit 6 is cleared, the PIC ends interrupt has the highest priority.
 */
#define OCW2        (0x20)

static inline void initialize_master_programmable_interrupt_controller(void)
{
    port_write(pic_master0, ICW1);
    port_write(pic_master1, ICW2_MASTER);
    port_write(pic_master1, ICW3_MASTER);
    port_write(pic_master1, ICW4);
}

static inline void initialize_slave_programmable_interrupt_controller(void)
{
    port_write(pic_slave0, ICW1);
    port_write(pic_slave1, ICW2_SLAVE);
    port_write(pic_slave1, ICW3_SLAVE);
    port_write(pic_slave1, ICW4);
}

void interrupt_controller_initialize(void)
{
    initialize_master_programmable_interrupt_controller();
    initialize_slave_programmable_interrupt_controller();
    interrupt_controller_set_mask(0);
}

void interrupt_controller_set_mask(uint16_t mask)
{
    port_write(pic_master1, (uint8_t)mask);
    port_write(pic_slave1, (uint8_t)(mask >> 8));
}

void interrupt_controller_notify_end(uint8_t interrupt_request_number)
{
    port_write(pic_master0, OCW2);

    /*
     * You need to notify to the slave if the interrupt number is greater or equal to 8.
     *
     * Consider the master-slave structure.
     */
    if (interrupt_request_number >= 8) {
        port_write(pic_slave0, OCW2);
    }
}
