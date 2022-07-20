#include "common.h"
#include "mini_uart.h"
#include "printf.h"
#include "timer.h"
#include "irq.h"
#include "mailbox.h"
#include "video.h"

// temp
#include "peripherals/base.h"

#define PM_MAGIC        (0x5A000000)

#define ASB_RPIVID_BASE (PBASE + 0xC11000)
#define ASB_RPIVID_M (ASB_RPIVID_BASE + 0x8)
#define ASB_RPIVID_S (ASB_RPIVID_BASE + 0xC)
#define ASB_RPIVID_MAGIC (0x5A000000)

void putc(void *p, char c) {
    if (c == '\n') {
        uart_send('\r');
    }
    uart_send(c);
}

u32 get_el();
void delay(int);


void kernel_main(void) {
    uart_init();
    init_printf(0, putc);
    printf("Raspberry Pi Bare Metal OS initializing...\n");

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();
    timer_init();

#if RPI_VERSION == 3
    printf("\tBoard: Raspberry Pi 3\n");
#elif RPI_VERSION == 4
    printf("\tBoard: Raspberry Pi 4\n");
#endif

    printf("\n\nException Level: %d\n", get_el());

    // We need to turn on the V3D
    uint32_t reg = 0xFE10010C;
	*(u32*)reg = PM_MAGIC | *(u32*)reg | 0x40;
	timer_sleep(20);
    *(u32*)ASB_RPIVID_S = ASB_RPIVID_MAGIC | (*(u32*)ASB_RPIVID_S & ~1);
	timer_sleep(20);
	*(u32*)ASB_RPIVID_M = ASB_RPIVID_MAGIC | (*(u32*)ASB_RPIVID_M & ~1);
	

    printf("Sleeping 200 ms...\n");
    timer_sleep(200);
    printf("Sleeping 200 ms..\n");
    timer_sleep(200);
    printf("Done!\n");

    printf("MAILBOX:\n");
    printf("SETTING ARM CLOCK RATE...");
    printf("ARM SET TO %d\n", mailbox_set_clock_rate(CT_ARM, 500000000));

    reg32* ident0 = (reg32*)(PBASE + 0x00C04000);
    reg32* ident1 = (reg32*)(PBASE + 0x00C04004);

    u32 major = (*ident0 >> 24) & 0xFF;
    u32 minor = (*ident1 >> 0) & 0xF;

    printf("V3D version major = %d\n", major);
    printf("V3D version minor = %d\n", minor);
    printf("V3D version = %d\n", major * 10 + minor);

    timer_sleep(200);

    printf("CORE CLOCK: %d\n", mailbox_clock_rate(CT_CORE));
    printf("EMMC CLOCK: %d\n", mailbox_clock_rate(CT_EMMC));
    printf("UART CLOCK: %d\n", mailbox_clock_rate(CT_UART));
    printf("ARM CLOCK: %d\n", mailbox_clock_rate(CT_ARM));

    // u32 max_temp = 0;
    // mailbox_generic_command(RPI_FIRMWARE_GET_MAX_TEMPERATURE, 0, &max_temp);

    // Set tile mode configuration
    


    //Do video...

    // printf("Resolution 1824x984\n");
    // video_set_resolution(1824, 984, 32);

    // printf("Resolution 1024x768\n");
    // video_set_resolution(1024, 768, 32);

    // printf("Resolution 1900x1200\n");
    // video_set_resolution(1900, 1200, 32);

    // printf("Resolution 480x320\n");
    // video_set_resolution(480, 320, 32);

    // while (1) {
    //     // uart_send(uart_recv());
    //     u32 cur_temp = 0;

    //     mailbox_generic_command(RPI_FIRMWARE_GET_TEMPERATURE, 0, &cur_temp);

    //     printf("Cur temp %d MAX: %d\n", cur_temp / 1000, max_temp / 1000);

    //     timer_sleep(1000);
    // }

    
}