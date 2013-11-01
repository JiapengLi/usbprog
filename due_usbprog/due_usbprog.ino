/*   Copyright (C) 2013 JiapengLi <gapleehit@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <SPI.h>

#define S_ACK 0x06
#define S_NAK 0x15

#define S_CMD_NOP          0x00UL /* No operation                                 */
#define S_CMD_Q_IFACE      0x01UL /* Query interface version                      */
#define S_CMD_Q_CMDMAP     0x02UL /* Query supported commands bitmap              */
#define S_CMD_Q_PGMNAME    0x03UL /* Query programmer name                        */
#define S_CMD_Q_SERBUF     0x04UL /* Query Serial Buffer Size                     */
#define S_CMD_Q_BUSTYPE    0x05UL /* Query supported bustypes                     */
#define S_CMD_Q_CHIPSIZE   0x06UL /* Query supported chipsize (2^n format)        */
#define S_CMD_Q_OPBUF      0x07UL /* Query operation buffer size                  */

#define S_CMD_Q_WRNMAXLEN  0x08UL /* Query Write to opbuf: Write-N maximum length */
#define S_CMD_R_BYTE       0x09UL /* Read a single byte                           */
#define S_CMD_R_NBYTES     0x0AUL /* Read n bytes                                 */
#define S_CMD_O_INIT       0x0BUL /* Initialize operation buffer                  */
#define S_CMD_O_WRITEB     0x0CUL /* Write opbuf: Write byte with address         */
#define S_CMD_O_WRITEN     0x0DUL /* Write to opbuf: Write-N                      */
#define S_CMD_O_DELAY      0x0EUL /* Write opbuf: udelay                          */
#define S_CMD_O_EXEC       0x0FUL /* Execute operation buffer                     */

#define S_CMD_SYNCNOP      0x10UL /* Special no-operation that returns NAK+ACK    */
#define S_CMD_Q_RDNMAXLEN  0x11UL /* Query read-n maximum length                  */
#define S_CMD_S_BUSTYPE    0x12UL /* Set used bustype(s).                         */
#define S_CMD_O_SPIOP      0x13UL /* Perform SPI operation.                       */

#define S_IFACE_VERSION    0x01    /* Version of the protocol */
#define S_PGM_NAME    "usbprog" /* The program's name */
#define S_SPEED      57600    /* Serial speed */

/* 
 * we must split in 3 parts because else avr-gcc doesn't seem to
 *  be able to compute stuff like 1<<S_CMD_SYNCNOP (it returns 0)
 */
#define SUPPORTED_COMMANDS_LOW ( ( \
  (1<<S_CMD_NOP) | (1<<S_CMD_Q_IFACE) | (1<<S_CMD_Q_CMDMAP) \
  | (1<<S_CMD_Q_PGMNAME) | (1<<S_CMD_Q_SERBUF) | (1<<S_CMD_Q_BUSTYPE) \
        ) & 0xff)
#define SUPPORTED_COMMANDS_HIGH ( ( ( \
  (1<<(S_CMD_SYNCNOP - 16)) | (1<<(S_CMD_O_SPIOP - 16)) | (1<<(S_CMD_S_BUSTYPE - 16)) \
  ) & 0xff ) )

#define SUPPORTED_BUS 0x08

#define CS                A5

#define select_chip()      digitalWrite(CS, LOW);
#define unselect_chip()    digitalWrite(CS, HIGH);


void setup_spi(void)
{
  SPI.begin(52);
  SPI.setClockDivider(52, 4);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
}

inline char readwrite_spi(char c)
{
  return SPI.transfer(c);
}

#define putchar_uart(x)  SerialUSB.write(x)

char getchar_uart(void)
{
  while(!SerialUSB.available());
  return SerialUSB.read();
}

#define word_uart(x)  SerialUSB.print(x)

/* get 24bit values in little endian */
uint32_t get24_le()
{
  uint32_t val = 0;
  
  val = (uint32_t)getchar_uart() << 0;
  val |= (uint32_t)getchar_uart() << 8;
  val |= (uint32_t)getchar_uart() << 16;
  return val;
}

void handle_command(unsigned char command)
{
  int i;
  char c;
  uint32_t slen = 0; /* write len */
  uint32_t rlen = 0; /* read len */
  switch (command){
    case S_CMD_NOP:
      putchar_uart(S_ACK);
      break;
    case S_CMD_Q_IFACE:
      putchar_uart(S_ACK);
      putchar_uart(S_IFACE_VERSION);
      /* little endian multibyte value to complete to 16bit */
      putchar_uart((uint8_t)0);
      break;
    case S_CMD_Q_CMDMAP:
      putchar_uart(S_ACK);
      /* little endian */
      putchar_uart(SUPPORTED_COMMANDS_LOW);
      putchar_uart((uint8_t)0x00);
      putchar_uart(SUPPORTED_COMMANDS_HIGH);
      for (i=0;i<29;i++){
        putchar_uart((uint8_t)0x0);
      }
      break;
    case S_CMD_Q_PGMNAME:
      putchar_uart(S_ACK);
      word_uart(S_PGM_NAME);
      for (i=strlen(S_PGM_NAME);i<16;i++){
        putchar_uart((uint8_t)0);
      }
      break;
    case S_CMD_Q_SERBUF:
      putchar_uart(S_ACK);
      putchar_uart(0xFF);
      putchar_uart(0xFF);
      break;
    case S_CMD_Q_BUSTYPE:
      putchar_uart(S_ACK);
      putchar_uart(0b00001000);
      break;
    case S_CMD_Q_CHIPSIZE:
      break;
    case S_CMD_Q_OPBUF:
      break;
    case S_CMD_Q_WRNMAXLEN:
      break;
    case S_CMD_R_BYTE:
      break;
    case S_CMD_R_NBYTES:
      break;
    case S_CMD_O_INIT:
      break;
    case S_CMD_O_WRITEB:
      break;
    case S_CMD_O_WRITEN:
      break;
    case S_CMD_O_DELAY:
      break;
    case S_CMD_O_EXEC:
      break;
    case S_CMD_SYNCNOP:
      putchar_uart(S_NAK);
      putchar_uart(S_ACK);
      break;
    case S_CMD_Q_RDNMAXLEN:
      break;
    case S_CMD_S_BUSTYPE:
      switch (getchar_uart()) {
        case SUPPORTED_BUS:
          putchar_uart(S_ACK);
          break;
        default:
          putchar_uart(S_NAK);
          break;
      }
      break;
    case S_CMD_O_SPIOP:
      /* get slen */
      slen = get24_le();
      /* get rlen */
      rlen = get24_le();

      select_chip();
      /* SPI is configured in little endian */
      while (slen--){
        c = getchar_uart();
        readwrite_spi(c);
      }
      putchar_uart(S_ACK);
      /* receive TODO: handle errors */
      while (rlen--){
        putchar_uart(readwrite_spi(0x0));
      }
      unselect_chip();
      break;
    default:
      break;
  }
}


void setup(void)
{
  SerialUSB.begin(9600);
  while(!SerialUSB);
  setup_spi();
}

void loop(void)
{
  uint8_t data=SerialUSB.read();
  if(data>0){
    handle_command(data);
  }
}



