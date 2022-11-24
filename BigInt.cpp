#include "BigInt.h"
#include <bitset>
#include <sstream>
#include <string>
#include <stdexcept>
#include <regex>
#include <algorithm>
using namespace std;
using llu = long long unsigned int;
static const string NumberExp = R"(^\s*([+-])*(\d+))";
BigInt::BigInt(std::string number)
{
    regex number_regex(NumberExp);
	smatch 	matches;
	if(!regex_match(number, matches, number_regex)){
		throw invalid_argument("invalid number");
        
	}
    string signs = matches[1];
    string digs =  matches[2];
    BigInt base(1);
    BigInt result(0);
    reverse(begin(digs), end(digs));
    for(char d : digs)
    {
        result = result + base * BigInt(d -'0');
        base = base * BigInt(10);
    }
    m_sign = (count(begin(signs), end(signs), '-') % 2)? -1 : 1 ;
    m_digits = result.m_digits;
    normalize();
}

BigInt::BigInt(long long int num)
{
    if(num == 0)
        m_sign = 0;
    else if(num < 0)
    {
        num  *= -1;
        m_sign = -1;
    }
    else
    {
        m_sign = 1;
    }
    while(num)
    {
        m_digits.push_back(num & MAX_VALUE);
        num >>= DIGIT_SIZE;
    }
    normalize();
}
BigInt::BigInt(int sign, std::vector<digit> digits)
{
    m_digits = digits;
    set_sign(sign);
    normalize();
}
BigInt::BigInt() : BigInt(0) 
{
}
//setters and getters

bool BigInt::sign() const
{
    return m_sign;
}
int BigInt::size() const
{
    return m_digits.size();
}
void BigInt::set_sign(int sign)
{
    if(sign < 0)
        m_sign = -1;
    else if (sign > 0)
        m_sign = 1;
    else
    {
        m_sign = 0;
        m_digits.clear();
    }
}
std::vector<digit> BigInt::get_digits() const
{
    return m_digits;
}
void BigInt::set_digits(std::vector<digit> digs)
{
    if(digs.size() == 0)
    {
        m_digits.clear();
        m_sign = 0;
    }
    else
    {
        m_digits = move(digs);
    }
    normalize();
}
string BigInt::str(bool in_binary) const
{
    stringstream s;
    
    if(m_sign == 0)
        if(!in_binary)
            return "0";
        else
            return bitset<DIGIT_SIZE>(0).to_string();

    else if(m_sign < 0)
        s << '-';


    if(in_binary)
    {
        for(int i = size() - 1; i >= 0; i--)
        {
            s << bitset<DIGIT_SIZE>(m_digits[i]) <<  " ";
        }
    }
    else
    {
        string numstr;
        BigInt num(*this);
        while(num.m_sign != 0)
        {
            llu rem;
            num = num.divide_by(10, &rem);
            numstr.push_back(rem + '0');
        }
        reverse(numstr.begin(), numstr.end());
        s << numstr;
    }

    return s.str();
} 

//logical methods

BigInt BigInt::add(const BigInt& number) const
{
    //if one of this was zero
    if(m_sign == 0)
        return number;
    else if(number.m_sign == 0)
        return *this;

    BigInt num1(*this);
    BigInt num2(number);

    num1.m_digits.push_back(0); num1.m_digits.push_back(0);
    num2.m_digits.push_back(0); num2.m_digits.push_back(0);

    while(num1.size() < num2.size())
        num1.m_digits.push_back(0);
    while(num2.size() < num1.size())
        num2.m_digits.push_back(0);

    if(num1.m_sign < 0)
        num1 = get_complement(num1);
    if(num2.m_sign < 0)
        num2 = get_complement(num2);

    llu carry = 0;
    for(int i = 0; i < num1.size(); i++)
    {
        llu sum = (llu)num1.m_digits[i] + (llu)num2.m_digits[i] + carry;
        num1.m_digits[i] =  (digit)sum;
        
        carry = sum >> sizeof(digit) * 8;


    }
    if(num1.m_digits.back() == MAX_VALUE)
    {
        num1.m_sign = -1;
        num1 = get_complement(num1);
    }
    else
        num1.m_sign = 1;

    num1.normalize();

    return num1;



}
BigInt BigInt::subtract(const BigInt& number) const
{
    return add(BigInt(number.sign() * -1, number.get_digits()));
}

BigInt BigInt::multiply(const BigInt& number) const
{
    BigInt result;
    result.m_digits.resize(size() + number.size());
    for(int i = 0; i < size(); i++)
    {
        llu carry = 0;
        llu product = m_digits[i];
        int res_index = i;
        int num_index = 0;

        while(num_index < number.size())
        {
            carry += result.m_digits[res_index] + number.m_digits[num_index] * product;
            result.m_digits[res_index] = (digit)carry;
            carry = carry >> DIGIT_SIZE;
            res_index++;
            num_index++;
        }
        if(carry)
        {
            result.m_digits[res_index] += (digit) (carry);
        }
    } 
    result.m_sign = m_sign * number.m_sign;
    result.normalize();
    return result;
}

BigInt BigInt::divide(const BigInt& divisor, BigInt & remainder) const
{
    if(m_sign == 0)
        return BigInt(0, {});
    if(divisor.m_sign == 0)
        throw invalid_argument("cannot divide by zero");

    vector<digit> N(m_digits);
    // reverse(N.begin(), N.end());

    vector<digit> Q(N.size());

    BigInt R(0);
    for(int i = DIGIT_SIZE * size() - 1; i >= 0; i--)
    {
        R = R << 1;


        if((N[i / DIGIT_SIZE] & (1 << (i % DIGIT_SIZE))) != 0)
        {
            if(R.m_sign == 0)
                R = BigInt(1);
            else
                R.m_digits[0] |= 1;
        }
        // cout << "R: " << R.str(1) << endl;
        // cout << "N: " << BigInt(1, N).str(1) << endl;
        if(R >= divisor)
        {
            R = R - divisor;
            Q[i / DIGIT_SIZE] |= (1 << (i % DIGIT_SIZE));
        }
        // cout << "Q: " <<  BigInt(1, Q).str(1) << endl << endl;


    }

    remainder = R;


    return BigInt(m_sign * divisor.m_sign, Q); 

}

BigInt BigInt::divide_by(digit divisor, llu * rem) const
{
    if(divisor == 0)
        throw invalid_argument("cannot divide by zero");
    vector<digit> quotient;
    BigInt number(*this);
    
    llu dividend = 0;
    for(int i = 0; i < number.size(); i++)
    {

        dividend = (dividend & (~(llu)MAX_VALUE)) | (llu)number.m_digits[number.size() - i - 1];
        quotient.push_back(dividend / (llu)divisor);
        dividend = (dividend & (~ (((llu)MAX_VALUE) << DIGIT_SIZE))) | (llu)(dividend % divisor) << DIGIT_SIZE;
        
    }
    dividend >>= DIGIT_SIZE;
    if(rem)
        *rem = dividend;
    reverse(begin(quotient), end(quotient));
    BigInt result(m_sign, quotient);
    result.normalize();
    return result;
}

int BigInt::compare(const BigInt& number) const
{
    if(sign() < number.sign())
        return -1;
    else if(sign() > number.sign())
        return 1;

    if(size() > number.size())
        return m_sign;
    if(number.size() > size())
        return -1 * m_sign;

    for(int i = number.size() - 1; i >= 0; i--)
    {
        if(m_digits[i] > number.m_digits[i])
            return m_sign;
        else if(m_digits[i] < number.m_digits[i])
            return -1 * m_sign;
    }
    return 0;

}
//operators overloading stuff
BigInt BigInt::operator+(const BigInt& number) const
{
    return add(number);
}

BigInt BigInt::operator-(const BigInt& number) const
{
    return subtract(number);
}

BigInt BigInt::operator*(const BigInt& number) const
{
    return multiply(number);
}

BigInt BigInt::operator/(const BigInt& number) const
{
    BigInt rem(0);
    return divide(number,rem);
}
BigInt BigInt::operator%(const BigInt& number) const
{
    BigInt rem(0);
    divide(number, rem);
    return rem;
}
BigInt BigInt::operator<<(int n) const
{
    return left_bitshift(n);
}
BigInt BigInt::operator>>(int n) const
{
    return right_bitshift(n);
}
bool BigInt::operator==(const BigInt& number) const
{
    if(compare(number) == 0)
        return true;
    return false;

}
bool BigInt::operator>(const BigInt& number) const
{
    if(compare(number) > 0)
        return true;
    return false;
}
bool BigInt::operator>=(const BigInt& number) const
{
    if(compare(number) >= 0)
        return true;
    return false;
}
bool BigInt::operator<(const BigInt& number) const
{
    if(compare(number) < 0)
        return true;
    return false;
}
bool BigInt::operator<=(const BigInt& number) const
{
    if(compare(number) <= 0)
        return true;
    return false;
}

ostream & operator<<( ostream &output,  const BigInt & number)
{
    output << number.str(false);
    return output;
}


//helper functions

void BigInt::normalize()
{
    while(m_digits.size() > 0 && m_digits.back() == 0)
        m_digits.pop_back();
    if(m_digits.size() == 0)
        m_sign = 0;
}

BigInt BigInt::get_complement(const BigInt& number) const
{
    if(number.m_sign == 0)
        return number;
    BigInt result(number);
    //flips bits
    for(digit& d : result.m_digits)
        d = ~d;

    int i = 1;
    //add one
    llu sum = (llu)result.m_digits[0] + 1;
    result.m_digits[0] = (digit)sum;
    llu carry  = sum >> DIGIT_SIZE;

    while(carry && i < result.size())
    {
        sum = carry + result.m_digits[i];
        result.m_digits[i] = sum & MAX_VALUE;
        carry = sum >> DIGIT_SIZE;
        i++;
    }

    if(carry)
        result.m_digits.push_back(1);

    return result;
    
}

BigInt BigInt::left_digit_shift(int n) const
{
    BigInt result(*this);
    reverse(result.m_digits.begin(), result.m_digits.end());
    while(n--)
        result.m_digits.push_back(0);
    reverse(result.m_digits.begin(), result.m_digits.end());
    return result;
}

BigInt BigInt::right_digit_shift(int n) const
{
    BigInt result(*this);
    reverse(result.m_digits.begin(), result.m_digits.end());
    while(n--)
        result.m_digits.pop_back();
    reverse(result.m_digits.begin(), result.m_digits.end());
    return result;
}
BigInt BigInt::left_bitshift(int n) const
{
    BigInt num(*this);
    
    num.m_digits.push_back(0);
    reverse(num.m_digits.begin(), num.m_digits.end());
    while(n >= DIGIT_SIZE)
    {
        num.m_digits.push_back(0);
        n -= DIGIT_SIZE;
    }
    for(int i = 0; i < num.size() - 1; i++)
    {
        num.m_digits[i] <<= n;
        digit mask = ((digit)MAX_VALUE << (DIGIT_SIZE - n));
        mask >>= DIGIT_SIZE - n;
        num.m_digits[i] |= (mask & (num.m_digits[i+1] >>( DIGIT_SIZE - n)));
    }
    num.m_digits[num.size() -1] <<= n;

    reverse(num.m_digits.begin(), num.m_digits.end());
    num.normalize();
    return num;
}

BigInt BigInt::right_bitshift(int n) const
{
    BigInt num(*this);

    if(n >= num.size() * DIGIT_SIZE)
        return BigInt(0,{});
        
    reverse(begin(num.m_digits), end(num.m_digits));
    while(n >= DIGIT_SIZE)
    {
       num.m_digits.pop_back();
       n -= DIGIT_SIZE;
    }

    for(int i = num.size() -1; i >= 1;  i--)
    {
        num.m_digits[i] >>= n;
        digit mask = ((digit)MAX_VALUE << (DIGIT_SIZE - n));
        mask >>= DIGIT_SIZE - n;

        digit value = mask & num.m_digits[i - 1];
        num.m_digits[i] |= (value << (DIGIT_SIZE -n));
    }
    num.m_digits[0] >>= n;

    reverse(begin(num.m_digits), end(num.m_digits));
    num.normalize();
    return num;
}