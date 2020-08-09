/*
* The MIT License (MIT)
*
* Copyright (c) 2015 Marco Russi
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/




/* ----------- Exported constants ------------- */

#define PAGE_SIZE		0x40
#define PAGE_MASK		(PAGE_SIZE-1)

/* ----------- Exported functions prototypes ------------- */

extern void eeprom_init(void);
extern bool eeprom_write_byte(uint16_t, uint8_t);
extern bool eeprom_write_page(uint16_t, uint8_t *, uint16_t);
extern bool eeprom_write_block(uint16_t, uint8_t *, uint16_t);
extern bool eeprom_read_byte(uint16_t, uint8_t *);
extern bool eeprom_read_page(uint16_t, uint8_t *, uint16_t);
extern bool eeprom_read_block(uint16_t, uint8_t *, uint16_t);




/* End of file */
