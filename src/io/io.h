#ifndef IO_H
#define IO_H

/**Reaad One byte From the Port `port` */
unsigned char insb(unsigned short port);
/**Read One Word From the Port `port` */
unsigned short insw(unsigned short port);
/**Write One byte `val` to the Port `port` */
void outb(unsigned short port , unsigned char val);
/**Write One Word `val` to the Port `port` */
void outw(unsigned short port , unsigned short val);










#endif
