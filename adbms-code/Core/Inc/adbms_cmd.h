#include <stdint.h>

/* configuration registers commands */
static uint16_t WRCFGA        = 0x0001;
static uint16_t WRCFGB      = 0x0024;
static uint16_t RDCFGA       = 0x0002;
static uint16_t RDCFGB        = 0x0026;

/* Read cell voltage result registers commands */
static uint16_t RDCVA         = 0x0004;
static uint16_t RDCVB         = 0x0006;
static uint16_t RDCVC         = 0x0008;
static uint16_t RDCVD         = 0x000A;
static uint16_t RDCVE         = 0x0009;
static uint16_t RDCVF         = 0x000B;

/* Read average cell voltage result registers commands commands */
static uint16_t RDACA         = 0x0044;
static uint16_t RDACB         = 0x0046;
static uint16_t RDACC         = 0x0048;
static uint16_t RDACD         = 0x004A;
static uint16_t RDACE         = 0x0049;
static uint16_t RDACF         = 0x004B;

/* Read s voltage result registers commands */
static uint16_t RDSVA         = 0x0003;
static uint16_t RDSVB         = 0x0005;
static uint16_t RDSVC         = 0x0007;
static uint16_t RDSVD         = 0x000D;
static uint16_t RDSVE         = 0x000E;
static uint16_t RDSVF         = 0x000F;

/* Read filtered cell voltage result registers*/
static uint16_t RDFCA         = 0x0012;
static uint16_t RDFCB         = 0x0013;
static uint16_t RDFCC         = 0x0014;
static uint16_t RDFCD         = 0x0015;
static uint16_t RDFCE         = 0x0016;
static uint16_t RDFCF         = 0x0017;

/* Read aux results */
static uint16_t RDAUXA        = 0x0019;
static uint16_t RDAUXB        = 0x001A;
static uint16_t RDAUXC        = 0x001B;
static uint16_t RDAUXD        = 0x001F;

/* Read redundant aux results */
static uint16_t RDRAXA        = 0x001C;
static uint16_t RDRAXB        = 0x001D;
static uint16_t RDRAXC        = 0x001E;
static uint16_t RDRAXD        = 0x0025;

/* Read status registers */
static uint16_t RDSTATA       = 0x0030;
static uint16_t RDSTATB       = 0x0031;
static uint16_t RDSTATC       = 0x0032;
static uint16_t RDSTATD       = 0x0033;
static uint16_t RDSTATE       = 0x0034;

/* Pwm registers commands */
static uint16_t WRPWM1        = 0x0020;
static uint16_t RDPWM1        = 0x0022;
static uint16_t WRPWM2        = 0x0021;
static uint16_t RDPWM2        = 0x0023;

/* Clear commands */
static uint16_t CLRCELL       = 0x0711;
static uint16_t CLRAUX        = 0x0712;
static uint16_t CLRSPIN       = 0x0716;
static uint16_t CLRFLAG       = 0x0717;
static uint16_t CLRFC         = 0x0714;
static uint16_t CLOVUV        = 0x0715;

/* Poll adc command */
static uint16_t PLADC         = 0x0718;
static uint16_t PLAUT         = 0x0719;
static uint16_t PLCADC        = 0x071C;
static uint16_t PLSADC        = 0x071D;
static uint16_t PLAUX1        = 0x071E;
static uint16_t PLAUX2        = 0x071F;

/* GPIOs Comm commands */
static uint16_t WRCOMM        = 0x0721;
static uint16_t RDCOMM        = 0x0722;
static uint16_t STCOMM        = 0x0723;

/* Mute and Unmute commands */
static uint16_t MUTE 	       = 0x0028;
static uint16_t UNMUTE        = 0x0029;

/* Read command counter command */
static uint16_t RSTCC         = 0x002E;

/* Snap commands */
static uint16_t SNAP          = 0x002D;
static uint16_t UNSNAP        = 0x002F;
static uint16_t SRST          = 0x0027;

/* Read Serial ID command */
static uint16_t RDSID         = 0x002C;