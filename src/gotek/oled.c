/*
 * oled.c
 * 
 * SSD1306 OLED controller driving 128x32 OLED bitmap display.
 * 
 * Written & released by Keir Fraser <keir.xen@gmail.com>
 * 
 * This is free and unencumbered software released into the public domain.
 * See the file COPYING for more details, or visit <http://unlicense.org>.
 */

#define i2c i2c2

#define DMA1_CH4_IRQ 14
void IRQ_14(void) __attribute__((alias("IRQ_dma1_ch4_tc")));

/* ASCII 0x20-0x7f inclusive */
const static uint32_t font[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0xfc380000, 0x000038fc, 0x0d000000, 0x0000000d, 
    0x001e0e00, 0x000e1e00, 0x00000000, 0x00000000, 
    0x20f8f820, 0x0020f8f8, 0x020f0f02, 0x00020f0f, 
    0x47447c38, 0x0098cc47, 0x38080c06, 0x00070f38, 
    0x80003030, 0x003060c0, 0x0103060c, 0x000c0c00, 
    0xe47cd880, 0x0040d8bc, 0x08080f07, 0x00080f07, 
    0x0e1e1000, 0x00000000, 0x00000000, 0x00000000, 
    0xf8f00000, 0x0000040c, 0x07030000, 0x0000080c, 
    0x0c040000, 0x0000f0f8, 0x0c080000, 0x00000307, 
    0xc0e0a080, 0x80a0e0c0, 0x01030200, 0x00020301, 
    0xe0808000, 0x008080e0, 0x03000000, 0x00000003, 
    0x00000000, 0x00000000, 0x1e100000, 0x0000000e, 
    0x80808080, 0x00808080, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x0c000000, 0x0000000c, 
    0x80000000, 0x003060c0, 0x0103060c, 0x00000000, 
    0xc40cf8f0, 0x00f0f80c, 0x080c0703, 0x0003070c, 
    0xfc181000, 0x000000fc, 0x0f080800, 0x0008080f, 
    0xc4840c08, 0x00183c64, 0x08090f0e, 0x000c0c08, 
    0x44440c08, 0x00b8fc44, 0x08080c04, 0x00070f08, 
    0x98b0e0c0, 0x0080fcfc, 0x08000000, 0x00080f0f, 
    0x44447c7c, 0x0084c444, 0x08080c04, 0x00070f08, 
    0x444cf8f0, 0x0080c044, 0x08080f07, 0x00070f08, 
    0x84040c0c, 0x003c7cc4, 0x0f0f0000, 0x00000000, 
    0x4444fcb8, 0x00b8fc44, 0x08080f07, 0x00070f08, 
    0x44447c38, 0x00f8fc44, 0x08080800, 0x0003070c, 
    0x30000000, 0x00000030, 0x06000000, 0x00000006, 
    0x30000000, 0x00000030, 0x0e080000, 0x00000006, 
    0x60c08000, 0x00081830, 0x03010000, 0x00080c06, 
    0x20202000, 0x00202020, 0x01010100, 0x00010101, 
    0x30180800, 0x0080c060, 0x060c0800, 0x00000103, 
    0xc4041c18, 0x00183ce4, 0x0d000000, 0x0000000d, 
    0xc808f8f0, 0x00f0f8c8, 0x0b080f07, 0x00010b0b, 
    0x8c98f0e0, 0x00e0f098, 0x00000f0f, 0x000f0f00, 
    0x44fcfc04, 0x00b8fc44, 0x080f0f08, 0x00070f08, 
    0x040cf8f0, 0x00180c04, 0x080c0703, 0x00060c08, 
    0x04fcfc04, 0x00f0f80c, 0x080f0f08, 0x0003070c, 
    0x44fcfc04, 0x001c0ce4, 0x080f0f08, 0x000e0c08, 
    0x44fcfc04, 0x001c0ce4, 0x080f0f08, 0x00000000, 
    0x840cf8f0, 0x00988c84, 0x080c0703, 0x000f0708, 
    0x4040fcfc, 0x00fcfc40, 0x00000f0f, 0x000f0f00, 
    0xfc040000, 0x000004fc, 0x0f080000, 0x0000080f, 
    0x04000000, 0x0004fcfc, 0x08080f07, 0x0000070f, 
    0xc0fcfc04, 0x001c3ce0, 0x000f0f08, 0x000e0f01, 
    0x04fcfc04, 0x00000000, 0x080f0f08, 0x000e0c08, 
    0x7038fcfc, 0x00fcfc38, 0x00000f0f, 0x000f0f00, 
    0x7038fcfc, 0x00fcfce0, 0x00000f0f, 0x000f0f00, 
    0x0404fcf8, 0x00f8fc04, 0x08080f07, 0x00070f08, 
    0x44fcfc04, 0x00387c44, 0x080f0f08, 0x00000000, 
    0x0404fcf8, 0x00f8fc04, 0x0e080f07, 0x00273f3c, 
    0x44fcfc04, 0x0038fcc4, 0x000f0f08, 0x000f0f00, 
    0x44643c18, 0x00189cc4, 0x08080e06, 0x00070f08, 
    0xfc0c1c00, 0x001c0cfc, 0x0f080000, 0x0000080f, 
    0x0000fcfc, 0x00fcfc00, 0x08080f07, 0x00070f08, 
    0x0000fcfc, 0x00fcfc00, 0x0c060301, 0x00010306, 
    0xc000fcfc, 0x00fcfc00, 0x030e0f07, 0x00070f0e, 
    0xe0f03c0c, 0x000c3cf0, 0x01030f0c, 0x000c0f03, 
    0xc07c3c00, 0x003c7cc0, 0x0f080000, 0x0000080f, 
    0xc4840c1c, 0x001c3c64, 0x08090f0e, 0x000e0c08, 
    0xfcfc0000, 0x00000404, 0x0f0f0000, 0x00000808, 
    0xc0e07038, 0x00000080, 0x01000000, 0x000e0703, 
    0x04040000, 0x0000fcfc, 0x08080000, 0x00000f0f, 
    0x03060c08, 0x00080c06, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x20202020, 0x20202020, 
    0x06020000, 0x0000080c, 0x00000000, 0x00000000, 
    0xa0a0a000, 0x0000c0e0, 0x08080f07, 0x00080f07, 
    0x20fcfc04, 0x0080c060, 0x080f0f00, 0x00070f08, 
    0x2020e0c0, 0x00406020, 0x08080f07, 0x00040c08, 
    0x2460c080, 0x0000fcfc, 0x08080f07, 0x00080f07, 
    0xa0a0e0c0, 0x00c0e0a0, 0x08080f07, 0x00040c08, 
    0xfcf84000, 0x00180c44, 0x0f0f0800, 0x00000008, 
    0x2020e0c0, 0x0020e0c0, 0x48486f27, 0x00003f7f, 
    0x40fcfc04, 0x00c0e020, 0x000f0f08, 0x000f0f00, 
    0xec200000, 0x000000ec, 0x0f080000, 0x0000080f, 
    0x00000000, 0x00ecec20, 0x40703000, 0x003f7f40, 
    0x80fcfc04, 0x002060c0, 0x010f0f08, 0x000c0e03, 
    0xfc040000, 0x000000fc, 0x0f080000, 0x0000080f, 
    0xc060e0e0, 0x00c0e060, 0x07000f0f, 0x000f0f00, 
    0x20c0e020, 0x00c0e020, 0x000f0f00, 0x000f0f00, 
    0x2020e0c0, 0x00c0e020, 0x08080f07, 0x00070f08, 
    0x20c0e020, 0x00c0e020, 0x487f7f40, 0x00070f08, 
    0x2020e0c0, 0x0020e0c0, 0x48080f07, 0x00407f7f, 
    0x60c0e020, 0x00c0e020, 0x080f0f08, 0x00000000, 
    0x20a0e040, 0x00406020, 0x09090c04, 0x00040e0b, 
    0xfcf82020, 0x00002020, 0x0f070000, 0x00040c08, 
    0x0000e0e0, 0x0000e0e0, 0x08080f07, 0x00080f07, 
    0x0000e0e0, 0x00e0e000, 0x080c0703, 0x0003070c, 
    0x8000e0e0, 0x00e0e000, 0x070c0f07, 0x00070f0c, 
    0x80c06020, 0x002060c0, 0x03070c08, 0x00080c07, 
    0x0000e0e0, 0x00e0e000, 0x48484f47, 0x001f3f68, 
    0xa0206060, 0x002060e0, 0x090b0e0c, 0x000c0c08, 
    0xf8404000, 0x000404bc, 0x07000000, 0x0008080f, 
    0xfc000000, 0x000000fc, 0x0f000000, 0x0000000f, 
    0xbc040400, 0x004040f8, 0x0f080800, 0x00000007, 
    0x06020604, 0x00020604, 0x00000000, 0x00000000, 
    0x3060c080, 0x0080c060, 0x04040707, 0x00070704, 
};

static volatile uint8_t dma_count;
static uint32_t buffer[512/4];
static char text[2][16];

static void kick_tx(unsigned int sz)
{
    dma1->ch4.cmar = (uint32_t)(unsigned long)buffer;
    dma1->ch4.cndtr = sz;
    dma1->ch4.ccr = (DMA_CCR_MSIZE_8BIT |
                     DMA_CCR_PSIZE_16BIT |
                     DMA_CCR_MINC |
                     DMA_CCR_DIR_M2P |
                     DMA_CCR_TCIE |
                     DMA_CCR_EN);
}

static void IRQ_dma1_ch4_tc(void)
{
    const uint32_t *p;
    uint32_t *q;
    unsigned int i, j, c;

    /* Clear the DMA controller. */
    dma1->ch4.ccr = 0;
    dma1->ifcr = DMA_IFCR_CGIF(4);

    /* Snapshot text buffer into the bitmap buffer. */
    q = buffer;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 16; j++) {
            if ((c = text[i][j] - 0x20) > 0x5f)
                c = '?' - 0x20;
            p = &font[c * 4];
            *q++ = *p++;
            *q++ = *p++;
            q[32-2] = *p++;
            q[32-1] = *p++;
        }
        q += 32;
    }

    /* Transmit the bitmap buffer over I2C using DMA. */
    kick_tx(sizeof(buffer));

    dma_count++;
}

static bool_t i2c_wait(uint8_t s)
{
    stk_time_t t = stk_now();
    while ((i2c->sr1 & s) != s) {
        if (i2c->sr1 & I2C_SR1_ERRORS) {
            i2c->sr1 &= ~I2C_SR1_ERRORS;
            return FALSE;
        }
        if (stk_diff(t, stk_now()) > stk_ms(10)) {
            /* I2C bus seems to be locked up. */
            return FALSE;
        }
    }
    return TRUE;
}

void oled_clear(void)
{
    memset(text, ' ', sizeof(text));
}

void oled_write(int col, int row, int min, const char *str)
{
    char c, *p = &text[row][col];
    while ((c = *str++) && (col++ < 16)) {
        *p++ = c;
        min--;
    }
    while ((min-- > 0) && (col++ < 16))
        *p++ = ' ';
}

void oled_sync(void)
{
    uint8_t c = dma_count;
    /* Two IRQs: 1st: text[] -> buffer[]; 2nd: buffer[] -> I2C. */
    while ((uint8_t)(c - dma_count) < 2)
        cpu_relax();
}

bool_t oled_init(uint8_t i2c_addr)
{
    static const uint8_t init_cmds[] = {
        0xae,       /* display off */
        0xd5, 0x80, /* default clock */
        0xa8, 31,   /* multiplex ratio (lcd height - 1) */
        0xd3, 0x00, /* display offset = 0 */
        0x40,       /* display start line = 0 */
        0x8d, 0x14, /* enable charge pump */
        0x20, 0x00, /* horizontal addressing mode */
        0xa1,       /* segment mapping (reverse) */
        0xc8,       /* com scan direction (decrement) */
        0xda, 0x02, /* com pins configuration */
        0x81, 0x8f, /* display contrast */
        0xd9, 0xf1, /* pre-charge period */
        0xdb, 0x20, /* vcomh detect (default) */
        0xa4,       /* output follows ram contents */
        0xa6,       /* normal display output (inverse=off) */
        0x2e,       /* deactivate scroll */
        0xaf,       /* display on */
        0x21, 0, 127, /* column address range: 0-127 */
        0x22, 0, 3    /* page address range: 0-3 */
    };

    uint8_t *p = (uint8_t *)buffer;
    int i;

    /* Initialise DMA1 channel 4 and its completion interrupt. */
    dma1->ch4.cpar = (uint32_t)(unsigned long)&i2c->dr;
    dma1->ifcr = DMA_IFCR_CGIF(4);
    IRQx_set_prio(DMA1_CH4_IRQ, CONSOLE_IRQ_PRI);
    IRQx_enable(DMA1_CH4_IRQ);

    /* Disable I2C (currently in Standard Mode). */
    i2c->cr1 = 0;

    /* Fast Mode (400kHz). */
    i2c->cr2 = I2C_CR2_FREQ(36);
    i2c->ccr = I2C_CCR_FS | I2C_CCR_CCR(30);
    i2c->trise = 12;
    i2c->cr1 = I2C_CR1_PE;

    /* Send the START sequence. */
    i2c->cr1 |= I2C_CR1_START;
    i2c->cr2 |= I2C_CR2_DMAEN;
    if (!i2c_wait(I2C_SR1_SB)) return FALSE;
    i2c->dr = i2c_addr << 1;
    if (!i2c_wait(I2C_SR1_ADDR)) return FALSE;
    (void)i2c->sr2;

    /* Initialisation sequence for SSD1306. */
    for (i = 0; i < sizeof(init_cmds); i++) {
        *p++ = 0x80; /* Co=1, Command */
        *p++ = init_cmds[i];
    }

    /* All subsequent bytes are data bytes, forever more. */
    *p++ = 0x40;

    /* Text buffer initially empty. */
    memset(text, ' ', sizeof(text));

    /* Send the initialisation command sequence by DMA. */
    kick_tx(p - (uint8_t *)buffer);

    return TRUE;
}

/*
 * Local variables:
 * mode: C
 * c-file-style: "Linux"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */