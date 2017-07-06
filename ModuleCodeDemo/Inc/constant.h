#ifndef CONSTANT
#define CONSTANT

#define PAGE_SIZE                             128
#define PAGE_NUMBER                           5
#define MEMORY_MAP_SIZE                       PAGE_SIZE*2
#define RW_CHUNK_SIZE                         PAGE_SIZE

#define QSFP28_IDENTIFIER                     0X11
#define IS_PAGED                              0x00<<2U

#define GN2104_TX_I2C_ADDRESS                 0x24
#define GN2104_RX_I2C_ADDRESS                 0x2C
#define GN2104_ADDRESS_SIZE                   I2C_MEMADD_SIZE_16BIT

#define IDENTIFIER_ADDRESS                    0
#define STATUS                                1
#define INTERRUPT_FLAG                        3
#define FREE_SIDE_MONITORS                    22
#define CHANNEL_MONITORS                      34
#define CONTROL                               86
#define MODULE_AND_CHANNEL_MASKES             100
#define FREE_SIDE_DEVICE_PROPERTIES           108
#define PASSWORD_CHANGE_ENTRY_AREA            119
#define PASSWORD_ENTRY_AREA                   123
#define PAGE_SELECT_BYTE                      127

#endif
