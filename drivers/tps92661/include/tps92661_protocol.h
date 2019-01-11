/*
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

 /**
  * @ingroup     drivers_tps92661
  *
  * @{
  *
  * @file
  * @brief       TPS92661 protocol definitions
  *
  * @author      Brooks Talley <brooks@tangentry.com>
  */

#ifndef TPS92661_PROTOCOL_H
#define TPS92661_PROTOCOL_H

#define TPS92661_REG__LED1ONL				(0x00)
#define TPS92661_REG__LED2ONL				(0x01)
#define TPS92661_REG__LED3ONL				(0x02)
#define TPS92661_REG__LED4ONL				(0x03)
#define TPS92661_REG__LED1_4ONH				(0x04)
#define TPS92661_REG__LED5ONL				(0x05)
#define TPS92661_REG__LED6ONL				(0x06)
#define TPS92661_REG__LED7ONL				(0x07)
#define TPS92661_REG__LED8ONL				(0x08)
#define TPS92661_REG__LED5_8ONH				(0x09)
#define TPS92661_REG__LED9ONL				(0x0a)
#define TPS92661_REG__LED10ONL				(0x0b)
#define TPS92661_REG__LED11ONL				(0x0c)
#define TPS92661_REG__LED12ONL				(0x0d)
#define TPS92661_REG__LED9_12ONH			(0x0e)

#define TPS92661_REG__LED1OFFL				(0x20)
#define TPS92661_REG__LED2OFFL				(0x21)
#define TPS92661_REG__LED3OFFL				(0x22)
#define TPS92661_REG__LED4OFFL				(0x23)
#define TPS92661_REG__LED1_4OFFH			(0x24)
#define TPS92661_REG__LED5OFFL				(0x25)
#define TPS92661_REG__LED6OFFL				(0x26)
#define TPS92661_REG__LED7OFFL				(0x27)
#define TPS92661_REG__LED8OFFL				(0x28)
#define TPS92661_REG__LED5_8OFFH			(0x29)
#define TPS92661_REG__LED9OFFL				(0x2a)
#define TPS92661_REG__LED10OFFL				(0x2b)
#define TPS92661_REG__LED11OFFL				(0x2c)
#define TPS92661_REG__LED12OFFL				(0x2d)
#define TPS92661_REG__LED9_12OFFH			(0x2e)

#define TPS92661_REG__ENONL					(0xb0)
#define TPS92661_REG__ENONH					(0xb1)
#define TPS92661_REG__ENOFFL				(0xb2)
#define TPS92661_REG__ENOFFH				(0xb3)

#define TPS92661_REG__PCKDIV				(0xc0)
#define TPS92661_REG__SYSCFG				(0xc1)
#define TPS92661_REG__DEFLEDL				(0xc2)
#define TPS92661_REG__DEFLEDH				(0xc3)
#define TPS92661_REG__TCNTL					(0xc4)
#define TPS92661_REG__TCNTH					(0xc5)

#define TPS92661_REG__FAULTL				(0xe0)
#define TPS92661_REG__FAULTH				(0xe1)
#define TPS92661_REG__CERRCNT				(0xe2)


#endif /* TPS92661_PROTOCOL_H */
/** @} */