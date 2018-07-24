// Calculate CRC32
// (Ethernet CRC)


/*
3.2.9 Frame Check Sequence (FCS) field

A cyclic redundancy check (CRC) is used by the transmit and receive algorithms to
generate a CRC value for the FCS field. The FCS field contains a 4-octet (32-bit)
CRC value. This value is computed as a function of the contents of the protected
fields of the MAC frame: the Destination Address, Source Address, Length/ Type
field, MAC Client Data, and Pad (that is, all fields except FCS). The encoding is
defined by the following generating polynomial.

  G(x) = x32 + x26 + x23 + x22 + x16 + x12 + x11
             + x10 + x8 + x7 + x5 + x4 + x2 + x + 1

Mathematically, the CRC value corresponding to a given MAC frame is defined by
the following procedure:

a) The first 32 bits of the frame are complemented.
b) The n bits of the protected fields are then considered to be the coefficients
   of a polynomial M(x) of degree n – 1. (The first bit of the Destination Address
   field corresponds to the x(n–1) term and the last bit of the MAC Client Data
   field (or Pad field if present) corresponds to the x0 term.)
c) M(x) is multiplied by x32 and divided by G(x), producing a remainder R(x) of
   degree ≤ 31.
d) The coefficients of R(x) are considered to be a 32-bit sequence.
e) The bit sequence is complemented and the result is the CRC.

The 32 bits of the CRC value are placed in the FCS field so that the x31 term is
the left-most bit of the first octet, and the x0 term is the right most bit of the
last octet. (The bits of the CRC are thus transmitted in the order x31, x30,...,
x1, x0.) See Hammond, et al. [B37].
*/


#include <vector>
#include <bitset>
#include <iostream>

uint32_t reverseBits(uint32_t x)
{
    x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
    x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
    x = ((x >> 4) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu) << 4);
    x = ((x >> 8) & 0x00ff00ffu) | ((x & 0x00ff00ffu) << 8);
    x = ((x >> 16) & 0xffffu) | ((x & 0xffffu) << 16);
    return x;
}

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
//#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
#define POLY 0xedb88320

uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len)
{
int k;

crc = ~crc;
while (len--) {
	crc ^= *buf++;
	for (k = 0; k < 8; k++)
		crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
}
return ~crc;
}

int main(void)
{
	// From fpga4fun example (https://www.fpga4fun.com/10BASE-T2.html)
	// Checksum should be B3 31 88 1B
	std::vector<uint8_t> data =  {
		0x00, 0x10, 0xA4, 0x7B, 0xEA, 0x80,
		0x00, 0x12, 0x34, 0x56, 0x78, 0x90,
		0x08, 0x00,
		0x45, 0x00, 0x00, 0x2E, 0xB3, 0xFE, 0x00, 0x00, 0x80,
		0x11,
		0x05, 0x40,
		0xC0, 0xA8, 0x00, 0x2C,
		0xC0, 0xA8, 0x00, 0x04,
		0x04, 0x00,
		0x04, 0x00,
		0x00, 0x1A,
		0x2D, 0xE8,
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
//		0x00, 0x00, 0x00, 0x00
};

	data =  {1};

	std::vector<unsigned int> taps = {1,2,4,5,7,8,10,11,12,16,22,23,26};
	std::bitset<32> crc;
	//crc.reset(); //Start with all zeros
	crc.set(); //Start with all ones

	// Pad with four bytes of zeros
	//for(auto i=0; i<4; i++)
	//{
//		//data.push_back(0);
//		data.insert(data.begin(), 0);
//	}
	//Complement first four bytes
	//for(auto i=0; i<4; i++)
	//	data[i] = ~data[i];

	for(auto byte : data)
	{
		std::bitset<8*sizeof(byte)> bits(byte);
		for(unsigned long i=0; i<bits.size(); i++)
		//Ethernet transmits MSB first
		//for(int i=bits.size()-1; i>=0; i--)
		{
			std::cout << "Updating CRC. Old: " << crc;

			// Set val equal to our bit xor the top bit
			bool val = crc[crc.size()-1];
			val ^= bits[i];
			// Shift and set LSB to val. This implements x^0
			crc <<= 1;
			crc[0] = val;
			// For each tap, xor the data with val
			for(auto tap : taps)
			{
				crc[tap] = crc[tap] ^ val;
				//Reverse taps
				//crc[31-tap] = crc[31-tap] ^ val;
			}
			std::cout << ". New: " << crc <<std::endl;
		}
	}

	std::cout << "!CRC:" << std::hex << crc.to_ulong() << std::endl;

	//Complement resulting crc
	crc = ~crc;

	std::cout << " CRC:" << std::hex << crc.to_ulong() << std::endl;

	std::cout << "rCRC:" << std::hex << reverseBits(crc.to_ulong()) << std::endl;


	uint8_t foo = 1;
	std::cout << "  Ex:" << std::hex << crc32c(0,&foo,1) << std::endl;

	return 0;
}
