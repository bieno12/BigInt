#ifndef BIGNUM
#define BIGNUM


#include <iostream>
#include <string>
#include <vector>
#include <limits>
using digit = uint8_t;

#define DIGIT_SIZE (sizeof(digit) * 8)
#define MAX_VALUE std::numeric_limits<digit>::max()
class BigInt
{
private:
	int m_sign;
    //the least significant digit is at digits[0];
	std::vector<digit> m_digits;


private:
    void normalize();
    BigInt get_complement(const BigInt&) const;
	BigInt left_digit_shift(int n) const;
	BigInt right_digit_shift(int n) const;
public:
	BigInt();
	BigInt(std::string);
	BigInt(long long int);
	BigInt(int sign, std::vector<digit> digits);

	static BigInt zero() 
	{
		return BigInt();
	}
	BigInt add(const BigInt& ) const;
	BigInt subtract(const BigInt&) const;
	BigInt multiply(const BigInt&) const;
	BigInt divide(const BigInt&, BigInt & rem) const;
	BigInt divide_by(digit d, long long unsigned int*) const;

	bool sign() const;
	int size() const;
	void set_sign(int);
	std::vector<digit> get_digits() const;
	void set_digits(std::vector<digit> digs);
	std::string str(bool in_binary) const;
	BigInt left_bitshift(int n) const;
	BigInt right_bitshift(int n) const;

	int compare(const BigInt&) const;

	BigInt operator+(const BigInt& cnumber) const;
	BigInt operator-(const BigInt& number) const;
	BigInt operator*(const BigInt& number) const;
	BigInt operator/(const BigInt& number) const;
	BigInt operator%(const BigInt& number) const;
	BigInt operator<<(int) const;
	BigInt operator>>(int) const;


	bool operator==(const BigInt& number) const;
	bool operator>(const BigInt& number) const;
	bool operator>=(const BigInt& number) const;
	bool operator<(const BigInt& number) const;
	bool operator<=(const BigInt& number) const;
	
	friend std::ostream & operator<<( std::ostream &output,  const BigInt & );

};
#endif