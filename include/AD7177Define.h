#ifndef INCLUDE_AD7177_DEFINE
#define INCLUDE_AD7177_DEFINE

/* AD7177 Register Map */
#define AD7177_COMM_REG       0x00
#define AD7177_STATUS_REG     0x00
#define AD7177_ADCMODE_REG    0x01
#define AD7177_IFMODE_REG     0x02
#define AD7177_REGCHECK_REG   0x03
#define AD7177_DATA_REG       0x04
#define AD7177_GPIOCON_REG    0x06
#define AD7177_ID_REG         0x07
#define AD7177_CHMAP0_REG     0x10
#define AD7177_CHMAP1_REG     0x11
#define AD7177_CHMAP2_REG     0x12
#define AD7177_CHMAP3_REG     0x13
#define AD7177_CHMAP4_REG     0x14
#define AD7177_CHMAP5_REG     0x15
#define AD7177_CHMAP6_REG     0x16
#define AD7177_CHMAP7_REG     0x17
#define AD7177_CHMAP8_REG     0x18
#define AD7177_CHMAP9_REG     0x19
#define AD7177_CHMAP10_REG    0x1A
#define AD7177_CHMAP11_REG    0x1B
#define AD7177_CHMAP12_REG    0x1C
#define AD7177_CHMAP13_REG    0x1D
#define AD7177_CHMAP14_REG    0x1E
#define AD7177_CHMAP15_REG    0x1F
#define AD7177_SETUPCON0_REG  0x20
#define AD7177_SETUPCON1_REG  0x21
#define AD7177_SETUPCON2_REG  0x22
#define AD7177_SETUPCON3_REG  0x23
#define AD7177_SETUPCON4_REG  0x24
#define AD7177_SETUPCON5_REG  0x25
#define AD7177_SETUPCON6_REG  0x26
#define AD7177_SETUPCON7_REG  0x27
#define AD7177_FILTCON0_REG   0x28
#define AD7177_FILTCON1_REG   0x29
#define AD7177_FILTCON2_REG   0x2A
#define AD7177_FILTCON3_REG   0x2B
#define AD7177_FILTCON4_REG   0x2C
#define AD7177_FILTCON5_REG   0x2D
#define AD7177_FILTCON6_REG   0x2E
#define AD7177_FILTCON7_REG   0x2F
#define AD7177_OFFSET0_REG    0x30
#define AD7177_OFFSET1_REG    0x31
#define AD7177_OFFSET2_REG    0x32
#define AD7177_OFFSET3_REG    0x33
#define AD7177_OFFSET4_REG    0x34
#define AD7177_OFFSET5_REG    0x35
#define AD7177_OFFSET6_REG    0x36
#define AD7177_OFFSET7_REG    0x37
#define AD7177_GAIN0_REG      0x38
#define AD7177_GAIN1_REG      0x39
#define AD7177_GAIN2_REG      0x3A
#define AD7177_GAIN3_REG      0x3B
#define AD7177_GAIN4_REG      0x3C
#define AD7177_GAIN5_REG      0x3D
#define AD7177_GAIN6_REG      0x3E
#define AD7177_GAIN7_REG      0x3F

/* Communication Register bits */
#define AD7177_COMM_REG_WEN    (0 << 7)
#define AD7177_COMM_REG_WR     (0 << 6)
#define AD7177_COMM_REG_RD     (1 << 6)
#define AD7177_COMM_REG_RA(x)  ((x) & 0x3F)

/* Status Register bits */
#define AD7177_STATUS_REG_RDY      (1 << 7)
#define AD7177_STATUS_REG_ADC_ERR  (1 << 6)
#define AD7177_STATUS_REG_CRC_ERR  (1 << 5)
#define AD7177_STATUS_REG_REG_ERR  (1 << 4)
#define AD7177_STATUS_REG_CH(x)    ((x) & 0x0F)

/* ADC Mode Register bits */
#define AD7177_ADCMODE_REG_REF_EN     (1 << 15)
#define AD7177_ADCMODE_SING_CYC       (1 << 13)
#define AD7177_ADCMODE_REG_DELAY(x)   (((x) & 0x7) << 8)
#define AD7177_ADCMODE_REG_MODE(x)    (((x) & 0x7) << 4)
#define AD7177_ADCMODE_REG_CLKSEL(x)  (((x) & 0x3) << 2)

/* ADC Mode Register additional bits for AD7172-2 */
#define AD7177_ADCMODE_REG_HIDE_DELAY   (1 << 14)

/* Interface Mode Register bits */
#define AD7177_IFMODE_REG_ALT_SYNC      (1 << 12)
#define AD7177_IFMODE_REG_IOSTRENGTH    (1 << 11)
#define AD7177_IFMODE_REG_HIDE_DELAY    (1 << 10)
#define AD7177_IFMODE_REG_DOUT_RESET    (1 << 8)
#define AD7177_IFMODE_REG_CONT_READ     (1 << 7)
#define AD7177_IFMODE_REG_DATA_STAT     (1 << 6)
#define AD7177_IFMODE_REG_REG_CHECK     (1 << 5)
#define AD7177_IFMODE_REG_XOR_EN        (0x01 << 2)
#define AD7177_IFMODE_REG_CRC_EN        (0x02 << 2)
#define AD7177_IFMODE_REG_XOR_STAT(x)   (((x) & AD7177_IFMODE_REG_XOR_EN) == AD7177_IFMODE_REG_XOR_EN)
#define AD7177_IFMODE_REG_CRC_STAT(x)   (((x) & AD7177_IFMODE_REG_CRC_EN) == AD7177_IFMODE_REG_CRC_EN)
#define AD7177_IFMODE_REG_DATA_WL16     (1 << 0)
#define AD7177_IFMODE_REG_DATA_WL32     (1 << 1)

/* GPIO Configuration Register bits */
#define AD7177_GPIOCON_REG_MUX_IO      (1 << 12)
#define AD7177_GPIOCON_REG_SYNC_EN     (1 << 11)
#define AD7177_GPIOCON_REG_ERR_EN(x)   (((x) & 0x3) << 9)
#define AD7177_GPIOCON_REG_ERR_DAT     (1 << 8)
#define AD7177_GPIOCON_REG_IP_EN1      (1 << 5)
#define AD7177_GPIOCON_REG_IP_EN0      (1 << 4)
#define AD7177_GPIOCON_REG_OP_EN1      (1 << 3)
#define AD7177_GPIOCON_REG_OP_EN0      (1 << 2)
#define AD7177_GPIOCON_REG_DATA1       (1 << 1)
#define AD7177_GPIOCON_REG_DATA0       (1 << 0)

/* GPIO Configuration Register additional bits for AD7172-4, AD7173-8 */
#define AD7177_GPIOCON_REG_GP_DATA3    (1 << 7)
#define AD7177_GPIOCON_REG_GP_DATA2    (1 << 6)
#define AD7177_GPIOCON_REG_GP_DATA1    (1 << 1)
#define AD7177_GPIOCON_REG_GP_DATA0    (1 << 0)

/* GPIO Configuration Register additional bits for AD7173-8 */
#define AD7177_GPIOCON_REG_PDSW        (1 << 14)
#define AD7177_GPIOCON_REG_OP_EN2_3    (1 << 13)

/* Channel Map Register 0-3 bits */
#define AD7177_CHMAP_REG_CH_EN         (1 << 15)
#define AD7177_CHMAP_REG_SETUP_SEL(x)  (((x) & 0x7) << 12)
#define AD7177_CHMAP_REG_AINPOS(x)     (((x) & 0x1F) << 5)
#define AD7177_CHMAP_REG_AINNEG(x)     (((x) & 0x1F) << 0)

/* Channel Map Register additional bits for AD4111 */
#define AD4111_CHMAP_REG_INPUT(x)      (((x) & 0x3FF) << 0)

/* Setup Configuration Register 0-3 bits */
#define AD7177_SETUP_CONF_REG_BI_UNIPOLAR  (1 << 12)
#define AD7177_SETUP_CONF_REG_REF_SEL(x)   (((x) & 0x3) << 4)

/* Setup Configuration Register additional bits for AD7173-8 */
#define AD7177_SETUP_CONF_REG_REF_BUF(x)  (((x) & 0x3) << 10)
#define AD7177_SETUP_CONF_REG_AIN_BUF(x)  (((x) & 0x3) << 8)
#define AD7177_SETUP_CONF_REG_BURNOUT_EN  (1 << 7)
#define AD7177_SETUP_CONF_REG_BUFCHOPMAX  (1 << 6)

/* Setup Configuration Register additional bits for AD7172-2, AD7172-4, AD7175-2 */
#define AD7177_SETUP_CONF_REG_REFBUF_P    (1 << 11)
#define AD7177_SETUP_CONF_REG_REFBUF_N    (1 << 10)
#define AD7177_SETUP_CONF_REG_AINBUF_P    (1 << 9)
#define AD7177_SETUP_CONF_REG_AINBUF_N    (1 << 8)

/* Filter Configuration Register 0-3 bits */
#define AD7177_FILT_CONF_REG_SINC3_MAP    (1 << 15)
#define AD7177_FILT_CONF_REG_ENHFILTEN    (1 << 11)
#define AD7177_FILT_CONF_REG_ENHFILT(x)   (((x) & 0x7) << 8)
#define AD7177_FILT_CONF_REG_ORDER(x)     (((x) & 0x3) << 5)
#define AD7177_FILT_CONF_REG_ODR(x)       (((x) & 0x1F) << 0)

/* ID register mask for relevant bits */
#define AD7177_ID_REG_MASK	  0xFFF0
/* AD7177-2 ID */
#define AD7177_2_ID_REG_VALUE 0x4FD0

#endif
