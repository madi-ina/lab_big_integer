// =============================================================
//  big_integer.cpp -- BigInteger class implementation
//
//  TASK: Implement all methods declared in big_integer.h
//  This stub file exists only so the project structure is clear.
//  Replace its contents with your implementation.
// =============================================================

#include "big_integer.h"
#include <algorithm>
#include <stdexcept>

// =======

void remove_leading_zeros(std::vector<int>& v) {
    while (v.size() > 1 && v.back() == 0) {
        v.pop_back();
    }
}

int compare_abs(const BigInteger& a, const BigInteger& b) {
    if (a.digits.size() != b.digits.size()) {
        return (a.digits.size() < b.digits.size()) ? -1 : 1;
    }

    for (int i = (int)a.digits.size() - 1; i >= 0; --i) {
        if (a.digits[i] != b.digits[i]) {
            return (a.digits[i] < b.digits[i]) ? -1 : 1;
        }
    }
    return 0;
}

// =======

BigInteger::BigInteger() : digits(1, 0), negative(false) {}

BigInteger::BigInteger(int value) : BigInteger((long long)value) {}

BigInteger::BigInteger(long long value) {
    negative = value < 0;

    long long tmp = value;
    if (tmp < 0) tmp = -tmp;

    if (tmp == 0) digits.push_back(0);

    while (tmp > 0) {
        digits.push_back(tmp % 10);
        tmp /= 10;
    }
}

BigInteger::BigInteger(const std::string& str) {
    negative = false;
    digits.clear();

    int start = 0;
    if (str[0] == '-') {
        negative = true;
        start = 1;
    }

    for (int i = (int)str.size() - 1; i >= start; --i) {
        digits.push_back(str[i] - '0');
    }

    remove_leading_zeros(digits);

    if (digits.size() == 1 && digits[0] == 0) {
        negative = false;
    }
}

// =======

std::string BigInteger::to_string() const {
    std::string res;

    if (negative) res += "-";

    for (int i = (int)digits.size() - 1; i >= 0; --i) {
        res += char('0' + digits[i]);
    }

    return res;
}

bool BigInteger::is_zero() const {
    return digits.size() == 1 && digits[0] == 0;
}

bool BigInteger::is_negative() const {
    return negative;
}

// =======

BigInteger::operator bool() const {
    return !is_zero();
}

// =======

bool BigInteger::operator==(const BigInteger& rhs) const {
    return negative == rhs.negative && digits == rhs.digits;
}

bool BigInteger::operator!=(const BigInteger& rhs) const {
    return !(*this == rhs);
}

bool BigInteger::operator<(const BigInteger& rhs) const {
    if (negative != rhs.negative) {
        return negative;
    }

    int cmp = compare_abs(*this, rhs);

    if (negative) return cmp > 0;
    return cmp < 0;
}

bool BigInteger::operator>(const BigInteger& rhs) const {
    return rhs < *this;
}

bool BigInteger::operator<=(const BigInteger& rhs) const {
    return !(*this > rhs);
}

bool BigInteger::operator>=(const BigInteger& rhs) const {
    return !(*this < rhs);
}

// ===============
BigInteger BigInteger::operator-() const {
    BigInteger res = *this;
    if (!res.is_zero()) {
        res.negative = !negative;
    }
    return res;
}

// =======================

BigInteger BigInteger::operator+(const BigInteger& rhs) const {
    if (negative == rhs.negative) {
        BigInteger res;
        res.negative = negative;

        int carry = 0;
        size_t n = std::max(digits.size(), rhs.digits.size());
        res.digits.resize(n);

        for (size_t i = 0; i < n; ++i) {
            int a = (i < digits.size()) ? digits[i] : 0;
            int b = (i < rhs.digits.size()) ? rhs.digits[i] : 0;

            int sum = a + b + carry;
            res.digits[i] = sum % 10;
            carry = sum / 10;
        }

        if (carry) res.digits.push_back(carry);

        return res;
    }

    return *this - (-rhs);
}

// =============================

BigInteger BigInteger::operator-(const BigInteger& rhs) const {
    if (negative != rhs.negative) {
        return *this + (-rhs);
    }

    if (compare_abs(*this, rhs) < 0) {
        return -(rhs - *this);
    }

    BigInteger res;
    res.negative = negative;
    res.digits.resize(digits.size());

    int borrow = 0;

    for (size_t i = 0; i < digits.size(); ++i) {
        int a = digits[i];
        int b = (i < rhs.digits.size()) ? rhs.digits[i] : 0;

        int sub = a - b - borrow;

        if (sub < 0) {
            sub += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }

        res.digits[i] = sub;
    }

    remove_leading_zeros(res.digits);

    return res;
}

// ========================
BigInteger BigInteger::operator*(const BigInteger& rhs) const {
    BigInteger res;
    res.negative = (negative != rhs.negative);

    res.digits.assign(digits.size() + rhs.digits.size(), 0);

    for (size_t i = 0; i < digits.size(); ++i) {
        int carry = 0;
        for (size_t j = 0; j < rhs.digits.size() || carry; ++j) {
            int cur = res.digits[i + j] +
                digits[i] * (j < rhs.digits.size() ? rhs.digits[j] : 0) +
                carry;

            res.digits[i + j] = cur % 10;
            carry = cur / 10;
        }
    }

    remove_leading_zeros(res.digits);

    if (res.is_zero()) res.negative = false;

    return res;
}

// ========================================

BigInteger BigInteger::operator/(const BigInteger& rhs) const {
    BigInteger zero(0);
    if (rhs == zero) {
        throw std::runtime_error("division by zero");
    }

    BigInteger a = *this;
    BigInteger b = rhs;

    a.negative = false;
    b.negative = false;

    BigInteger res;
    BigInteger cur;

    res.digits.resize(a.digits.size());

    for (int i = (int)a.digits.size() - 1; i >= 0; --i) {
        cur.digits.insert(cur.digits.begin(), a.digits[i]);
        remove_leading_zeros(cur.digits);

        int x = 0;
        while (cur >= b) {
            cur -= b;
            x++;
        }

        res.digits[i] = x;
    }

    remove_leading_zeros(res.digits);

    res.negative = (negative != rhs.negative);
    if (res.is_zero()) res.negative = false;

    return res;
}

// ==================================

BigInteger BigInteger::operator%(const BigInteger& rhs) const {
    BigInteger div = *this / rhs;
    BigInteger mul = div * rhs;
    BigInteger res = *this - mul;
    return res;
}

// =========================================

BigInteger& BigInteger::operator+=(const BigInteger& rhs) {
    *this = *this + rhs;
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& rhs) {
    *this = *this - rhs;
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& rhs) {
    *this = *this * rhs;
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& rhs) {
    *this = *this / rhs;
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& rhs) {
    *this = *this % rhs;
    return *this;
}

// =============================================

BigInteger& BigInteger::operator++() {
    *this += 1;
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger temp = *this;
    ++(*this);
    return temp;
}

BigInteger& BigInteger::operator--() {
    *this -= 1;
    return *this;
}

BigInteger BigInteger::operator--(int) {
    BigInteger temp = *this;
    --(*this);
    return temp;
}

// ===============================================

std::ostream& operator<<(std::ostream& os, const BigInteger& value) {
    os << value.to_string();
    return os;
}

std::istream& operator>>(std::istream& is, BigInteger& value) {
    std::string s;
    is >> s;
    value = BigInteger(s);
    return is;
}
