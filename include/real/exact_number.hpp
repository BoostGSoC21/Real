#ifndef BOOST_REAL_EXACT_NUMBER
#define BOOST_REAL_EXACT_NUMBER

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <type_traits>
#include <limits>
#include <iterator>
#include <cctype>

namespace boost {
    namespace real {

        int KARATSUBA_BASE_CASE_THRESHOLD = 100;

        template <typename T = int>
        struct exact_number {
            using exponent_t = int;

            // TODO: replace all redundant declarations of base with this
            // static const T BASE = ;

            std::vector<T> digits = {};
            exponent_t exponent = 0;
            bool positive = true;

            static bool aligned_vectors_is_lower(const std::vector<T> &lhs, const std::vector<T> &rhs, bool equal = false) {

                // Check if lhs is lower than rhs
                auto lhs_it = lhs.cbegin();
                auto rhs_it = rhs.cbegin();
                while (rhs_it != rhs.end() && lhs_it != lhs.end() && *lhs_it == *rhs_it) {
                    ++lhs_it;
                    ++rhs_it;
                }

                if (rhs_it != rhs.cend() && lhs_it != lhs.cend()) {
                    return *lhs_it < *rhs_it;
                }

                if (equal && rhs_it == rhs.cend() && lhs_it == lhs.cend())
                    return false;

                bool lhs_all_zero = std::all_of(lhs_it, lhs.cend(), [](int i){ return i == 0; });
                bool rhs_all_zero = std::all_of(rhs_it, rhs.cend(), [](int i){ return i == 0; });

                return lhs_all_zero && !rhs_all_zero;
            }

            /// adds other to *this. disregards sign -- that's taken care of in the operators.
            void add_vector(exact_number &other, T base = (std::numeric_limits<T>::max() /4)*2 - 1){
                int carry = 0;
                std::vector<T> temp;
                int fractional_length = std::max((int)this->digits.size() - this->exponent, (int)other.digits.size() - other.exponent);
                int integral_length = std::max(this->exponent, other.exponent);

                // we walk the numbers from the lowest to the highest digit
                for (int i = fractional_length - 1; i >= -integral_length; i--) {

                    T lhs_digit = 0;
                    if (0 <= this->exponent + i && this->exponent + i < (int)this->digits.size()) {
                        lhs_digit = this->digits[this->exponent + i];
                    }

                    T rhs_digit = 0;
                    if (0 <= other.exponent + i && other.exponent + i < (int)other.digits.size()) {
                        rhs_digit = other.digits[other.exponent + i];
                    }
                    
                    T digit;
                    int orig_carry = carry;
                    carry = 0;
                    if ((base - lhs_digit) < rhs_digit) {
                        T min = std::min(lhs_digit, rhs_digit);
                        T max = std::max(lhs_digit, rhs_digit);
                        if (min <= base/2) {
                            T remaining = base/2 - min;
                            digit = (max - (base - 1)/2) - remaining - 2;
                        } else {
                            digit = (min - base/2) + (max - (base - 1)/2) - 2;
                        }
                        carry = 1;
                    }
                    else {
                        carry = 0;
                        digit = rhs_digit + lhs_digit;
                    }
                    if (digit < base || orig_carry == 0) {
                        digit += orig_carry;
                    }
                    else {
                        carry = 1;
                        digit = 0;
                    }
                    temp.insert(temp.begin(), digit);
                }
                if (carry == 1) {
                    temp.insert(temp.begin(), 1);
                    integral_length++;
                }
                this->digits = temp;
                this->exponent = integral_length;
                this->normalize();
            }

            /// subtracts other from *this, disregards sign -- that's taken care of in the operators
            void subtract_vector(exact_number &other, T base = (std::numeric_limits<T>::max() /4)*2 - 1) {
                std::vector<T> result;
                int fractional_length = std::max((int)this->digits.size() - this->exponent, (int)other.digits.size() - other.exponent);
                int integral_length = std::max(this->exponent, other.exponent);
                int borrow = 0;
                // we walk the numbers from the lowest to the highest digit
                for (int i = fractional_length - 1; i >= -integral_length; i--) {

                    T digit = 0;

                    T lhs_digit = 0;
                    if (0 <= this->exponent + i && this->exponent + i < (int)this->digits.size()) {
                        lhs_digit = this->digits[this->exponent + i];
                    }

                    T rhs_digit = 0;
                    if (0 <= other.exponent + i && other.exponent + i < (int) other.digits.size()) {
                        rhs_digit = other.digits[other.exponent + i];
                    }

                    if (lhs_digit < borrow) {
                        digit = (base - rhs_digit) + 1 - borrow;
                    } else {
                        lhs_digit -= borrow;
                        borrow = 0;
                        
                        if (lhs_digit < rhs_digit) {
                        ++borrow;
                        digit = (base - (rhs_digit -1)) + lhs_digit;
                        } else {
                            digit = lhs_digit - rhs_digit;
                        }

                    }                    
                    result.insert(result.begin(), digit);
                }
                this->digits = result;
                this->exponent = integral_length;
                this->normalize();
            }

            //Returns (a*b)%mod
            static T mul_mod(T a, T b, T mod) 
            { 
                T res = 0; // Initialize result 
                a = a % mod; 
                while (b > 0) 
                { 
                    // If b is odd, add 'a' to result 
                    if (b % 2 == 1) 
                        res = (res + a) % mod; 
            
                    // Multiply 'a' with 2 
                    a = (a * 2) % mod; 
            
                    // Divide b by 2 
                    b /= 2; 
                } 
            
                return res % mod; 
            }

            //Returns (a*b)/mod
            static T mult_div(T a, T b, T c) {
                T rem = 0;
                T res = (a / c) * b;
                a = a % c;
                // invariant: a_orig * b_orig = (res * c + rem) + a * b
                // a < c, rem < c.
                while (b != 0) {
                    if (b & 1) {
                        rem += a;
                        if (rem >= c) {
                            rem -= c;
                            res++;
                        }
                    }
                    b /= 2;
                    a *= 2;
                    if (a >= c) {
                        a -= c;
                        res += b;
                    }
                }
                return res;
            } 

            /// multiplies *this by other
            void multiply_vector(exact_number &other, T base = (std::numeric_limits<T>::max() / 4) * 2) {
                karatsuba_multiplication(other, base);
            }

            /// multiplies *this by other
            void standard_multiplication(exact_number &other, T base = (std::numeric_limits<T>::max() /4)*2) {
                // will keep the result number in vector in reverse order
                // Digits: .123 | Exponent: -3 | .000123 <--- Number size is the Digits size less the exponent
                // Digits: .123 | Exponent: 2  | 12.3
                std::vector<T> temp;
                size_t new_size = this->digits.size() + other.digits.size();
                if (this->exponent < 0) new_size -= this->exponent; // <--- Less the exponent
                if (other.exponent < 0) new_size -= other.exponent; // <--- Less the exponent

                temp.assign(new_size, 0);

                // Below two indexes are used to find positions
                // in result.
                auto i_n1 = (int) temp.size() - 1;
                // Go from right to left in lhs
                for (int i = (int)this->digits.size()-1; i >= 0; i--) {
                    T carry = 0;

                    // To shift position to left after every
                    // multiplication of a digit in rhs
                    int i_n2 = 0;

                    // Go from right to left in rhs
                    for (int j = (int)other.digits.size()-1; j>=0; j--) {

                        // Multiply current digit of second number with current digit of first number
                        // and add result to previously stored result at current position.
                        T rem = mul_mod(this->digits[i], other.digits[j], base);
                        T rem_s;
                        T q = mult_div(this->digits[i], other.digits[j], base);
                        if ( temp[i_n1 - i_n2] >= base - carry ) {
                            rem_s = carry - (base - temp[i_n1 - i_n2]);
                            ++q;
                        }
                        else
                            rem_s = temp[i_n1 - i_n2] + carry;
                        if ( rem >= base - rem_s ) {
                            rem -= (base - rem_s);
                            ++q;
                        }
                        else
                            rem += rem_s;

                        // Carry for next iteration
                        carry = q;

                        // Store result
                        temp[i_n1 - i_n2] = rem;

                        i_n2++;
                    }

                    // store carry in next cell
                    if (carry > 0) {
                        temp[i_n1 - i_n2] += carry;
                    }

                    // To shift position to left after every
                    // multiplication of a digit in lhs.
                    i_n1--;
                }

                int fractional_part = ((int)this->digits.size() - this->exponent) + ((int)other.digits.size() - other.exponent);
                int result_exponent = (int)temp.size() - fractional_part;
                
                digits = temp;
                exponent = result_exponent;
                this->positive = this->positive == other.positive;
                this->normalize();
            }

            /**
             *  @brief: KARATSUBA MULTIPLICATION: multiplies (*this) with other using karatsuba multiplication algorithm
             *  @param: other: an exact_number to be multiplied with (*this)
             *  @param: base: base of the numbers being multiplied
             *  @author: Kishan Shukla
             */

            void karatsuba_multiplication (
                    exact_number<T> &other, 
                    const T base = (std::numeric_limits<T>::max() / 4) * 2
            ) {

                // this --- a, other --- b
                const int a_size = this->digits.size();
                const int b_size = other.digits.size();
                const int a_exponent = this->exponent;
                const int b_exponent = other.exponent;
                const bool a_sign = this->positive;
                const bool b_sign = other.positive;

                const int max_length = std::max(a_size, b_size);

                if (max_length <= KARATSUBA_BASE_CASE_THRESHOLD || std::abs(a_size - b_size) > std::min(a_size, b_size)) {
                    this->standard_multiplication(other, base);
                    return;
                }

                // appending zeroes in front to make sizes of a & b equal
                int a_pref_zeroes = 0, b_pref_zeroes = 0;
                if (a_size < max_length) {
                    a_pref_zeroes = max_length - a_size;
                } else if (b_size < max_length) {
                    b_pref_zeroes = max_length - b_size;
                } 

                const int left_half_length = max_length / 2;
                const int right_half_length = max_length - left_half_length;

                /*
                        Variable Explanation
                    a is vector representation of "this", b is vector representation of "other"
                    a = exact_al * base^(right_half_length) + exact_ar
                    b = exact_bl * base^(right_half_length) + exact_br
                */
                exact_number<T> exact_al;
                exact_number<T> exact_ar;
                exact_number<T> exact_bl;
                exact_number<T> exact_br;

                if (a_pref_zeroes > 0) {
                    if (a_pref_zeroes >= left_half_length) {
                        exact_al = exact_number(std::vector<T> (), true);
                        exact_ar = exact_number(this->digits, true);
                    } else {
                        exact_al = exact_number(std::vector<T> (this->digits.begin(), this->digits.begin() + left_half_length - a_pref_zeroes), true);
                        exact_ar = exact_number(std::vector<T> (this->digits.begin() + left_half_length - a_pref_zeroes, this->digits.end()), true);
                    }
                    exact_bl = exact_number(std::vector<T> (other.digits.begin(), other.digits.begin() + left_half_length), true);
                    exact_br = exact_number(std::vector<T> (other.digits.begin() + left_half_length, other.digits.end()), true);
                } else if (b_pref_zeroes > 0) {
                    if (b_pref_zeroes >= left_half_length) {
                        exact_bl = exact_number(std::vector<T> (), true);
                        exact_br = exact_number(other.digits, true);
                    } else {
                        exact_bl = exact_number(std::vector<T> (other.digits.begin(), other.digits.begin() + left_half_length - b_pref_zeroes), true);
                        exact_br = exact_number(std::vector<T> (other.digits.begin() + left_half_length - b_pref_zeroes, other.digits.end()), true);
                    }
                    exact_al = exact_number(std::vector<T> (this->digits.begin(), this->digits.begin() + left_half_length), true);
                    exact_ar = exact_number(std::vector<T> (this->digits.begin() + left_half_length, this->digits.end()), true);
                } else {
                    exact_al = exact_number(std::vector<T> (this->digits.begin(), this->digits.begin() + left_half_length), true);
                    exact_ar = exact_number(std::vector<T> (this->digits.begin() + left_half_length, this->digits.end()), true);
                    exact_bl = exact_number(std::vector<T> (other.digits.begin(), other.digits.begin() + left_half_length), true);
                    exact_br = exact_number(std::vector<T> (other.digits.begin() + left_half_length, other.digits.end()), true);
                }

                exact_al.normalize();
                exact_bl.normalize();
                exact_ar.normalize();
                exact_br.normalize();

                exact_number<T> sum_al_ar;
                exact_number<T> sum_bl_br;

                /*
                    Variable explanation
                    sum_al_ar = al - ar
                    sum_bl_br = br - bl
                */
                int sign = 1;

                if(exact_al > exact_ar) {
                    sum_al_ar = exact_al;
                    sum_al_ar.subtract_vector(exact_ar, base - 1);
                }
                else {
                    sum_al_ar = exact_ar;
                    sum_al_ar.subtract_vector(exact_al, base - 1);
                    sign *= -1;
                }

                if(exact_br > exact_bl) {
                    sum_bl_br = exact_br;
                    sum_bl_br.subtract_vector(exact_bl, base - 1);
                }
                else {
                    sum_bl_br = exact_bl;
                    sum_bl_br.subtract_vector(exact_br, base - 1);
                    sign *= -1;
                }

                exact_al.karatsuba_multiplication(exact_bl, base);
                sum_al_ar.karatsuba_multiplication(sum_bl_br, base);
                exact_ar.karatsuba_multiplication(exact_br, base);
                /*
                    Variable explanation
                    exact_al = al * bl
                    sum_al_ar = (al - ar) * (br - bl)
                    exact_ar = ar * br
                */

                exact_number<T> temp(exact_al);
                temp.add_vector(exact_ar, base - 1);

                if(sign == 1) {
                    sum_al_ar.add_vector(temp, base - 1);
                }
                else {
                    temp.subtract_vector(sum_al_ar, base - 1);
                    sum_al_ar = temp;
                }
                /*
                    temp = al * bl + ar * br
                    sum_al_ar = al * br + ar * bl
                */

                // multiply exact_al by base^(2 * right_half_length)
                exact_al.exponent += 2 * right_half_length;

                // multiply sum_al_ar by base^(right_half_length)
                sum_al_ar.exponent += right_half_length;

                exact_al.add_vector(sum_al_ar, base - 1);
                exact_al.add_vector(exact_ar, base - 1);

                *this = exact_al;
                this->exponent += -(a_size + b_size) + (a_exponent + b_exponent);
                this->positive = (a_sign == b_sign);
                this->normalize();

            }

            //Performs long division on dividend by divisor and returns result in quotient
            std::vector<T> long_divide_vectors(
                    const std::vector<T>& dividend,
                    const std::vector<T>& divisor,
                    std::vector<T>& quotient,
                    T base = (std::numeric_limits<T>::max() / 4) * 2
            ) {
                /*
                 *   Currently knuths algorithm has been implemented for long division, the
                 *   algorithm can be refined furthur as to make significant optimisations
                 *   in time complexity. Later on more algorithms like Ziegler burnikel algorithm
                 *   can be implemented and proper benchmarking would give better results.
                 */

                std::vector<T> zero = {0}, remainder;
                knuth_division(dividend, divisor, quotient, remainder, base);
                if (quotient == zero) {
                    quotient.clear();
                }
                if (remainder == zero) {
                    remainder.clear();
                }
                return remainder;
            }

            /** 
             *   @brief:  "KNUTH DIVISION" computes quotient and remainder when dividend is divided by divisor using 
             *            knuth's algorithm (Not exactly knuth's Algorithm due to design constraints).
             *            Valid only for integers.
             *   @param: dividend  - vector of any size to be divided
             *   @param: divisor   - vector of any size which divides
             *   @param: quotient  - Empty vector in which quotient is returned
             *   @param: remainder - Empty vector in which remainder is returned
             *   @param: Base      - Base of integer vectors dividend and divisor provided
             *   @author: Kishan Shukla
             *   @ref:    The Art of Computer Programming, Vol 2, 4.33 Algorithm D
             */

            static void knuth_division(
                    const std::vector<T>& dividend, 
                    const std::vector<T>& divisor,
                    std::vector<T>& quotient,
                    std::vector<T>& remainder,
                    T base = (std::numeric_limits<T>::max() / 4) * 2){

                exact_number<T> tmp;
                std::vector<T> aligned_dividend = dividend;
                std::vector<T> aligned_divisor = divisor;
                size_t idx = 0;
                while (idx < aligned_dividend.size() && aligned_dividend[idx] == 0) {
                    idx++;
                }
                aligned_dividend.erase(aligned_dividend.begin(), aligned_dividend.begin() + idx);

                if (aligned_dividend.empty()) {
                    quotient.clear();
                    remainder.clear();
                    return;
                }
                if ((aligned_dividend.size() == aligned_divisor.size() && 
                        tmp.aligned_vectors_is_lower(aligned_dividend, aligned_divisor)) || 
                            aligned_dividend.size() < aligned_divisor.size()) {
                    quotient.clear();
                    remainder = aligned_dividend;
                    return;
                }

                if ((int)divisor.size() == 1) {
                    division_by_single_digit(dividend, divisor, quotient, remainder, base);
                    return;
                }

                exact_number<T> exact_divisor, exact_dividend;

                exact_dividend.digits = dividend;
                exact_dividend.exponent = dividend.size();
                exact_dividend.normalize();

                exact_divisor.digits = divisor;
                exact_divisor.exponent = divisor.size();
                exact_divisor.normalize();

                if (exact_divisor.digits[0] == 0) {
                    throw divide_by_zero();
                }

                exact_number<T> zero("0");
                static exact_number<T> one_exact("1");
                static exact_number<T> two_exact("2");

                int normalization_factor = 0;
                while (exact_divisor.digits[0] < base / 2) {
                    exact_divisor.multiply_vector(two_exact, base);
                    exact_dividend.multiply_vector(two_exact, base);
                    normalization_factor++;
                }

                //   To make the most significant bit of divisor >= (base/2) (which is required
                //   for the correctness of the algorithm), we multiply the numerator and the
                //   denominator by a factor of d as it won't affect our answer

                while (exact_divisor.exponent - (int)exact_divisor.digits.size() > 0) {
                    exact_divisor.digits.push_back(0);
                }
                while (exact_dividend.exponent - (int)exact_dividend.digits.size() > 0) {
                    exact_dividend.digits.push_back(0);
                }

                int n = exact_divisor.digits.size();
                int m = exact_dividend.digits.size();

                if (m < n) {
                    // divisor = remainder and quotient = 0
                    quotient.push_back(0);
                    remainder = exact_divisor.digits;
                } else if (m == n){
                    if (exact_dividend < exact_divisor) {
                        remainder = exact_dividend.digits;
                        quotient.push_back(0);
                    }
                    /*
                     *  Following block of code is bit inefficient. Complexity for following 
                     *  is log(base). It calculates quotient digit when dividend and divisor are
                     *  of same size. Doing a bit research may yield some method with constant
                     *  complexity.
                     */
                    else {

                        T left = 1, right = base - 1, mid = (right - left) / 2 + left;

                        // Binary search to obtain single quotient digit. loop runs less
                        // than log(base) number of times
                        exact_number<T> temp_dividend, temp_quotient;
                        while (left <= right) {

                            mid = (right - left) / 2 + left;
                            temp_quotient.digits = {mid};
                            temp_quotient.exponent = 1;
                            temp_dividend = exact_dividend;
                            temp_quotient.multiply_vector(exact_divisor, base);

                            if (temp_quotient > temp_dividend) {
                                right = mid - 1;
                            } else if (temp_quotient == temp_dividend) {
                                temp_dividend = zero;
                                break;
                            } else {
                                temp_dividend.subtract_vector(temp_quotient, base - 1);
                                if (temp_dividend < exact_divisor) {
                                    break;
                                } else if (temp_dividend == exact_divisor) {
                                    mid++;
                                    temp_dividend = zero;
                                    break;
                                } else {
                                    left = mid + 1;
                                }
                            }
                        }
                        quotient.push_back(mid);
                        temp_dividend.normalize();
                        while (temp_dividend.exponent - (int)temp_dividend.digits.size() > 0) {
                            temp_dividend.digits.push_back(0);
                        }
                        remainder = temp_dividend.digits;
                    }
                }
                else {
                    exact_number<T> exact_base(std::vector<T> {1, 0}, 2, true);

                    std::vector<T> dividend_part(exact_dividend.digits.begin(), exact_dividend.digits.begin() + n);
                    exact_number<T> temp_dividend(dividend_part, n, true);
                    
                    exact_number<T> first_digit, second_digit, temp_quotient, temp;
                    std::vector<T> quotient_digit, temp_remainder;

                    for (long int j = n; j < m; ++j) {
                        temp_dividend.digits.push_back(exact_dividend.digits[j]);
                        temp_dividend.exponent++;
                        if (temp_dividend == zero) {
                            temp_dividend.clear();
                            temp_dividend.exponent = 0;
                        }
                        while (temp_dividend < exact_divisor) {
                            if (j == m - 1) {
                                break;
                            }
                            j++;
                            temp_dividend.digits.push_back(exact_dividend.digits[j]);
                            temp_dividend.exponent++;
                            quotient.push_back(0);
                        }

                        if (temp_dividend < exact_divisor) {
                            quotient.push_back(0);
                            temp_dividend.normalize();
                            remainder = temp_dividend.digits;
                            while (temp_dividend.exponent - (long int)temp_dividend.digits.size() > 0) {
                                remainder.push_back(0);
                                temp_dividend.exponent--;
                            }
                            break;
                        }

                        /*
                         *  Following block of code is bit inefficient. Complexity for following 
                         *  is log(base). It calculates quotient digit when dividend and divisor are
                         *  of same size. Doing a bit research may yield some method with constant
                         *  complexity.
                         */
                        if ((int)temp_dividend.digits.size() == n) {

                            T left = 1, right = base - 1, mid = (right - left) / 2 + left;

                            // Binary search to obtain single quotient digit. loop runs less
                            // than log(base) number of times

                            exact_number<T> temp_dividend2;
                            while (left <= right) {

                                mid = (right - left) / 2 + left;
                                temp_quotient.digits = {mid};
                                temp_quotient.exponent = 1;
                                temp_dividend2 = temp_dividend;
                                temp_quotient.multiply_vector(exact_divisor, base);

                                if (temp_quotient > temp_dividend2) {
                                    right = mid - 1;
                                } else if (temp_quotient == temp_dividend2) {
                                    temp_dividend2 = zero;
                                    break;
                                } else {
                                    temp_dividend2.subtract_vector(temp_quotient, base - 1);
                                    if (temp_dividend2 < exact_divisor) {
                                        break;
                                    } else if (temp_dividend2 == exact_divisor){
                                        mid++;
                                        temp_dividend2 = zero;
                                        break;
                                    } else {
                                        left = mid + 1;
                                    }
                                }
                            }
                            quotient.push_back(mid);
                            temp_dividend = temp_dividend2;
                            temp_dividend.normalize();
                            if (temp_dividend == zero) {
                                temp_dividend.digits.clear();
                                continue;
                            }
                            while (temp_dividend.exponent - (int)temp_dividend.digits.size() > 0) {
                                temp_dividend.digits.push_back(0);
                            }
                            if (j == m - 1) {
                                remainder = temp_dividend.digits;
                                break;
                            } else {
                                continue;
                            }
                        }
                        temp_dividend.exponent = n + 1;

                        first_digit.digits = {temp_dividend.digits[0]};
                        second_digit.digits = {temp_dividend.digits[1]};
                        first_digit.exponent = 1;
                        second_digit.exponent = 1;
                        first_digit.multiply_vector(exact_base, base);
                        first_digit.add_vector(second_digit, base);
                        while (first_digit.exponent > (int)first_digit.digits.size()) {
                            first_digit.digits.push_back(0);
                        }

                        quotient_digit.clear(); temp_remainder.clear();
                        first_digit.division_by_single_digit(first_digit.digits,
                            std::vector<T> {exact_divisor[0]}, quotient_digit, temp_remainder, base);

                        temp_quotient.digits = quotient_digit;
                        temp_quotient.exponent = quotient_digit.size();
                        temp = temp_quotient;
                        temp.multiply_vector(exact_divisor, base);
                        while (temp > temp_dividend) {
                            temp_quotient.subtract_vector(one_exact, base - 1);
                            temp = temp_quotient;
                            temp.multiply_vector(exact_divisor, base);
                        }
                        while (temp_quotient.exponent - (int)temp_quotient.digits.size() > 0) {
                            temp_quotient.digits.push_back(0);
                        }
                        quotient.insert(quotient.end(), temp_quotient.digits.begin(), temp_quotient.digits.end());

                        temp_dividend.subtract_vector(temp, base - 1);
                        temp_dividend.normalize();
                        while (temp_dividend.exponent - (int)temp_dividend.digits.size() > 0) {
                            temp_dividend.digits.push_back(0);
                        }
                        if (j == m - 1) {
                            remainder = temp_dividend.digits;
                        }
                        if (temp_dividend == zero) {
                            temp_dividend.digits.clear();
                        }
                    }
                }
                if (normalization_factor >= 1) {
                    T factor = 1 << normalization_factor;
                    std::vector<T> temp = remainder, tempr;
                    remainder.clear();
                    division_by_single_digit(temp, std::vector<T> {factor}, remainder, tempr, base);
                }
            }

            /** 
             *  @brief: divides a vector by single digit divisor using modified (optimized) long division 
             *  @param: dividend: a vector to be divided by divisor, can be of any size
             *  @param: divisor: a vector of size 1
             *  @param: quotient: an empty vector in which quotient is returned
             *  @param: remainder: an empty vector in which remainder is returned
             *  @author: Kishan Shukla
             */

            static void division_by_single_digit(
                    const std::vector<T> & dividend,
                    const std::vector<T> & divisor,
                    std::vector<T> & quotient,
                    std::vector<T> & remainder,
                    T base = (std::numeric_limits<T>::max() / 4) * 2){

                // division by zero exception
                if (divisor[0] == 0) {
                    throw divide_by_zero();
                }

                // if Divisor is one then => quotient = dividend, remainder=0
                if (divisor[0] == 1) {
                    for(auto x : dividend) {
                        quotient.push_back(x);
                    }
                    remainder.push_back(0);
                    return;
                }

                int dividend_size = dividend.size();

                // if dividend size is one, then the operation is simply integer division
                if (dividend_size == 1) {
                    T a = dividend[0];
                    T b = divisor[0];
                    quotient.push_back(a / b);
                    remainder.push_back(a % b);
                    return;
                }

                exact_number<T> exact_remainder, exact_divisor;
                exact_remainder.digits.push_back(dividend[0]);
                exact_remainder.exponent = 1;
                exact_divisor.digits = divisor;
                exact_divisor.exponent = 1;

                auto next_digit = 1;

                if (dividend[0] < divisor[0]) {
                    exact_remainder.digits.push_back(dividend[next_digit]);
                    exact_remainder.exponent = 2;
                    next_digit++;
                }

                exact_number<T> temp_quotient, temp_remainder, zero;
                zero.digits = {0};

                // looping over the digits of dividend vector
                while (next_digit <= dividend_size) {

                    T left = 1, right = base - 1, mid = (right - left) / 2 + left;

                    // Binary search to obtain single quotient digit. loop runs less
                    // than log(base) number of times

                    while (left <= right) {
                        mid = (right - left) / 2 + left;
                        temp_quotient.digits = {mid};
                        temp_quotient.exponent = 1;
                        temp_remainder = exact_remainder;
                        temp_quotient.multiply_vector(exact_divisor, base);

                        if (temp_quotient > temp_remainder) {
                            right = mid - 1;
                        } else if (temp_quotient == temp_remainder) {
                            temp_remainder = zero;
                            break;
                        } else {
                            temp_remainder.subtract_vector(temp_quotient, base - 1);
                            if (temp_remainder < exact_divisor) {
                                break;
                            } else if (temp_remainder == exact_divisor) {
                                mid++;
                                temp_remainder = zero;
                                break;
                            } else {
                                left = mid + 1;
                            }
                        }
                    }
                    quotient.push_back(mid);

                    exact_remainder = temp_remainder;
                    if (exact_remainder == zero) {
                        if (next_digit < dividend_size) {
                            exact_remainder.digits.clear();
                            while (dividend[next_digit] == 0 && next_digit < dividend_size) {
                                quotient.push_back(0); next_digit++;
                            }
                            if (next_digit == dividend_size) {
                                break;
                            }
                            exact_remainder.digits.push_back(dividend[next_digit]);
                            exact_remainder.exponent = 1;
                            next_digit++;
                        } else {
                            break;
                        }
                        if (exact_remainder < exact_divisor) {
                            quotient.push_back(0);
                            if (next_digit < dividend_size) {
                                exact_remainder.digits.push_back(dividend[next_digit]);
                                exact_remainder.exponent = 2;
                                next_digit++;
                            } else {
                                break;
                            }
                        }
                    } else if (next_digit < dividend_size) {
                        exact_remainder.digits.push_back(dividend[next_digit]);
                        exact_remainder.exponent = 2;
                        next_digit++;
                    } else {
                        break;
                    }
                }
                remainder = exact_remainder.digits;
            }

            /**
             *  @brief:  divides (*this) by divisor
             *  @param:  divisor: number which divides (*this)
             *  @param:  max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
             *  @param:  upper: if true: error lies in [0, +epsilon]
             *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
             *  @author: Kishan Shukla
             */
            void divide_vector(const exact_number<T> divisor, unsigned int max_error_exponent, bool upper) {
                newton_raphson_division(divisor, max_error_exponent, upper);
            }

            /**         
             *  @brief: an approximate division method, used in the initial guess of reciprocal in Newton Raphson
             *  @param: divisor: an exact_number which divides
             *  @param: max_error_exponent: maximum error in the answer should be <= 1 * base^(-max_error_exponent)
             *  @author: Kishan Shukla
             */

            void binary_search_division(
                const exact_number<T> divisor,
                unsigned int max_error_exponent){

                /* Exceptions */
                if (max_error_exponent > (unsigned int) std::abs(std::numeric_limits<exponent_t>::min())) {
                    throw exponent_overflow_exception();
                }

                static const exact_number<T> zero = exact_number<T> ();
                static exact_number<T> one_exact("1");
                static exact_number<T> two_exact("2");
                if (divisor == zero) {
                    throw divide_by_zero();
                }

                /* Special Cases */
                if ((*this) == zero) {
                    return;
                }

                if (divisor == one_exact) {
                    return;
                }

                bool positive = (this->positive == divisor.positive);

                static const exact_number<T> neg_one(std::vector<T> {1}, 1, false);
                if (divisor == neg_one) {
                    this->positive = positive;
                    return;
                }

                static const T base = (std::numeric_limits<T>::max() / 4) * 2 - 1;
                exact_number<T> half;
                half.digits = {base / 2 + 1};

                exact_number<T> numerator, denominator;
                numerator = (*this).abs();
                denominator = divisor.abs();

                int exponent_diff = this->exponent - denominator.exponent;
                numerator.exponent = 1;      
                denominator.exponent = 1;   /* exponent is kept 1 to ensure denominator > 1 */
                
                exact_number<T> left, right;     /* answer will lie in the interval [left, right] */

                /* if numerator > denominator ==> numerator / denominator > 1 
                 * and numerator / denominator < numerator (denominator > 1)
                 * ==> 1 < numerator / denominator < numerator
                 * if denominator > numerator ==> numerator / denominator < 1
                 * and numerator / denominator > 0 (both numerator & denominator are +tive)
                 * ==> 0 < numerator / denominator < 1
                 */
                if (numerator > denominator) {
                    left = one_exact;
                    right = numerator;
                } else {
                    left = zero;
                    right = one_exact;
                }

                exact_number<T> length = (right - left) * half;   /* length is half the length of [left, right] */
                (*this) = length + left;

                exact_number<T> residual = (*this) * denominator - numerator;
                if (residual == zero) {
                    this->exponent += exponent_diff;
                    this->positive = positive;
                    return;
                }

                exact_number<T> maximum_error(std::vector<T> {1}, -max_error_exponent, true);
                exact_number<T> neg_maximum_error(std::vector<T> {1}, -max_error_exponent, false);

                exact_number<T> max_residual_error, neg_max_residual_error;
                max_residual_error = maximum_error*denominator;
                neg_max_residual_error = max_residual_error;
                neg_max_residual_error.positive = false;

                /* At each iteration of the following loop, the interval [left, right] reduces to
                 * half its previous length. Since (left <= numerator/denominator <= right), we get 
                 * more precise intervals at each iteration. 
                 */
                while ((residual.abs() >= max_residual_error) && (length.exponent >= maximum_error.exponent)) {

                    if (residual < neg_maximum_error) {
                        left = (*this);
                    }

                    length = length * half;
                    length.normalize();
                    while (length.digits.size() > max_error_exponent + 1) {
                        length.digits.pop_back();
                    }

                    (*this) = left + length;
                    while (this->digits.size() > max_error_exponent + 1) {
                        this->digits.pop_back();
                    }

                    residual = (*this) * denominator - numerator;
                    residual.normalize();
                }

                this->normalize();
                while (this->digits.size() > max_error_exponent + 1) {
                    this->digits.pop_back();
                }

                residual = (*this) * denominator - numerator;
                residual.normalize();

                if (residual < zero) {
                    this->round_up(base);
                }

                if (residual > zero) {
                    this->round_down(base);
                }

                this->positive = positive;
                this->exponent += exponent_diff;
                this->normalize();
            }

            /**
             * @brief:   calculates (*this)/divisor
             * @param:   divisor: an exact_number which divides (*this)
             * @param:   max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
             * @param:   upper: if upper is  
             *              1. true: returns result with [0, +epsilon] error
             *              2. false: return result with [-epsilon, 0] error, here epsilon = 1*base^(-max_error_exponent)
             * @author:  Kishan Shukla
             */

            void newton_raphson_division(
                const exact_number<T> divisor,
                unsigned int max_error_exponent,
                bool upper){

                /* Exceptions start */
                if (max_error_exponent > (unsigned int) std::abs(std::numeric_limits<exponent_t>::min())) {
                    throw exponent_overflow_exception();
                }

                static const exact_number<T> zero = exact_number<T> ();
                static exact_number<T> one_exact("1");
                static exact_number<T> two_exact("2");
                if (divisor == zero) {
                    throw divide_by_zero();
                }
                /* Exceptions end */

                /* special cases start */
                if ((*this) == zero) {
                    return;
                }

                if (divisor == one_exact) {
                    return;
                }

                bool positive = (this->positive == divisor.positive); /* sign of result */

                static const exact_number<T> neg_one(std::vector<T> {1}, 1, false); /* neg_one => negative one */
                if (divisor == neg_one) {
                    this->positive = positive;
                    return;
                }

                /* special cases end */

                /* preprocessing required for the newton raphson algorithm to converge 
                 * divisor should satisfy condition: 0.5 <= divisor <= 1 (in decimal system) */
                exact_number<T> numerator, denominator;
                numerator = (*this).abs();
                denominator = divisor.abs();

                int exponent_diff = numerator.exponent - denominator.exponent;
                numerator.exponent = 0;
                denominator.exponent = 0;

                const T base = (std::numeric_limits<T>::max() / 4) * 2;

                while (denominator.digits[0] < base / 2) {
                    denominator = denominator * two_exact;
                    numerator = numerator * two_exact;
                }
                /* preprocessing end */

                /* Reciprocal computation starts */
                /* initializing the reciprocal guess */
                /* initial guess for the reciprocal is (48 - 32*divisor)/17 */
                exact_number<T> reciprocal, more_precise_reciprocal;
                exact_number<T> _32(std::vector<T> {32}, 1, true);
                exact_number<T> _48(std::vector<T> {48}, 1, true);
                exact_number<T> _17(std::vector<T> {17}, 1, true);

                reciprocal = _48 - _32*denominator;
                reciprocal.binary_search_division(_17, max_error_exponent); /* approximate division method */
                /* Initial guess end*/

                --max_error_exponent; 
                exact_number<T> error, residual, answer, more_precise_answer;
                answer = reciprocal * numerator;

                exact_number<T> max_error(std::vector<T> {1}, -max_error_exponent, true);

                /* newton raphson iteration starts */
                do {
                    /* improving guess */
                    reciprocal = reciprocal * ( two_exact - reciprocal*denominator);
                    reciprocal.normalize();

                    /* truncate insignificant digits from the reciprocal */
                    while ((int) reciprocal.digits.size() - reciprocal.exponent - numerator.exponent >  max_error_exponent + 1) {
                        reciprocal.digits.pop_back();
                    }

                    more_precise_answer = reciprocal * numerator;
                    more_precise_answer.normalize();

                    /* truncate insignificant digits from the reciprocal */
                    while ((int) more_precise_answer.digits.size() - more_precise_answer.exponent > max_error_exponent + 1) {
                        more_precise_answer.digits.pop_back();
                    }

                    /* if there is no improvement then exit */
                    if (more_precise_answer == answer) {
                        break;
                    }

                    /* Compute the improvement in result */
                    error = more_precise_answer - answer;
                    error = error.abs();

                    answer = more_precise_answer;
                    
                } while (error > max_error);
                /* newton raphson iteration ends*/

                (*this) = answer;
                
                residual = (*this) * denominator - numerator;
                residual.normalize();

                if (upper) {/* residual shoud be positive or = zero */

                    if (residual < zero) {/* if residual is negative, we make it positive or = zero */
                        (*this) += max_error;
                    }

                    if (residual > zero) {/* if residual is positive, we check if we can make it zero */
                        exact_number<T> tmp_lower = (*this) - max_error;
                        residual = tmp_lower * denominator - numerator;
                        residual.normalize();
                        if (residual == zero) {
                            (*this) = tmp_lower;
                        }
                    }

                } else {/* residual shoud be negative = zero */

                    if (residual > zero) {/* if residual is positive, we make it negative or = zero */
                        (*this) -= max_error;
                    }

                    if (residual < zero) {/* if residual is negative, we check if we can make it zero */
                        exact_number<T> tmp_upper = (*this) + max_error;
                        residual = tmp_upper * denominator - numerator;
                        residual.normalize();

                        if (residual == zero) {
                            (*this) = tmp_upper;
                        }
                    }
                }

                this->exponent += exponent_diff;
                this->positive = positive;
                this->normalize();

            }

            /**
             *  @brief:  calculates exact_number^exact_number, (only integral powers)
             *  @param: number: an exact_number whose integral power is to be evaluated
             *  @param: exponent: an exact_number which is integer power to be evaluated
             *  @return: returns an exact_number = number^exponent
             *  @author: Kishan Shukla
             */


            static exact_number<T> binary_exponentiation(const exact_number<T>& number, const exact_number<T>& exponent) {
                exact_number<T> result, zero = exact_number<T> (), number_copy;
                result.digits = {1};
                result.exponent = 1;
                number_copy = number;

                /* exponent_vector is vector representation of exponent */
                std::vector<T> exponent_vector, remainder, quotient;
                exponent_vector = exponent.digits;

                while((int) exponent_vector.size() < exponent.exponent){
                    exponent_vector.push_back(0);
                }

                while(true){
                    division_by_single_digit(exponent_vector, std::vector<T> {2}, quotient, remainder);
                    if ( !(remainder.empty() || ((int)remainder.size() == 1 && remainder[0] == 0)) ) {
                        result = result * number_copy;
                    }

                    number_copy = number_copy * number_copy;

                    exponent_vector = quotient;
                    if(((int)exponent_vector.size() == 1 && exponent_vector[0] == 0) || exponent_vector.empty()){
                        break;
                    }
                    quotient.clear();
                    remainder.clear();
                }

                return result;
            }

            void round_up_abs(T base) {
                int index = digits.size() - 1;
                bool keep_carrying = true;

                while((index > 0) && keep_carrying) { // bring the carry back
                    if(this->digits[index] != base) {
                        ++this->digits[index];
                        keep_carrying = false;
                    } else { // digits[index] == 9, we keep carrying
                        this->digits[index] = 0;
                    }
                    --index;
                }

                if ((index == 0) && keep_carrying) { // i.e., .999 should become 1.000
                    if(this->digits[index] == base) {
                        this->digits[index] = 0;
                        this->push_front(1);
                        ++this->exponent;
                    } else {
                        ++this->digits[index];
                    }
                }
            }

            void round_up(T base) {
                if (positive)
                    this->round_up_abs(base);
                else
                    this->round_down_abs(base);
            }

            void round_down(T base) {
                if (positive)
                    this->round_down_abs(base);
                else
                    this->round_up_abs(base);
            }

            void round_down_abs(T base) {
                int index = digits.size() - 1;
                bool keep_carrying = true;

                while((index > 0) && keep_carrying) { // bring the carry back
                    if(this->digits[index] != 0) {
                        --this->digits[index];
                        keep_carrying = false;
                    } else // digits[index] == 0, we keep carrying
                        this->digits[index] = base;
                    --index;
                }
                // we should be ok at this point because the first number in digits should != 0
                if (index == 0 && keep_carrying)
                    --this->digits[index];
            }

            /**
             * @brief *default constructor*: It constructs a representation of the number zero.
             */
            exact_number<T>() = default;

            /// ctor from vector of digits, integer exponent, and optional bool positive
            exact_number<T>(std::vector<T> vec, int exp, bool pos = true) : digits(vec), exponent(exp), positive(pos) {};

            exact_number<T>(std::vector<T> vec, bool pos = true) : digits(vec), exponent(vec.size()), positive(pos) {};

            /// ctor from any integral type
            /// @TODO: use whichever base.
            // template<typename I, typename std::enable_if_t<std::is_integral<I>::value>>
            // exact_number(I x) {
                // if (x < 0)
                    // positive = false;
                // else
                    // positive = true;
                
                // exponent = 0;
                // while (((x % BASE) != 0) || (x != 0)) {
                    // exponent++;
                    // push_front(std::abs(x%BASE));
                    // x /= BASE;
                // }
            // }
        
            // returns {integer_part, decimal_part, exponent, is_positive}
            constexpr static std::tuple<std::string_view, std::string_view, exponent_t, bool> number_from_string(std::string_view number) {
                std::string_view integer_part;
                std::string_view decimal_part;

                int exponent = 0;
                bool exp_positive = true;
                bool positive = true;

                bool has_exponent = false;
                bool has_decimal = false;
                bool has_sign = false;

                size_t index = 0;

                size_t integer_count = 0;

                size_t decimal_start_index = 0; // pos of first number past '.'
                size_t decimal_count = 0;

                size_t integer_rhs_zeros = 0;
                size_t integer_lhs_zeros = 0;
                size_t decimal_lhs_zeros = 0;
                size_t decimal_rhs_zeros = 0;

                if (number[index] == '-') {
                    positive = false;
                    has_sign = true;
                    index++;
                } else if (number[index] == '+') {
                    // is already positive
                    has_sign = true;
                    index++;
                }

                // first digit must be nonzero.

                for (; index < number.size(); index++) {
                    // we should not have any characters in the input except 'e' and '.', and
                    // '.' can only come before e, and these characters can only occur once

                    // handle '.' and 'e'
                    if (!has_exponent) {
                        if(number[index] == 'e') {
                            has_exponent = true;

                            if (number[index+1] == '-') {
                                exp_positive = false;
                                index++;
                            } else if (number[index+1] == '+') {
                                index++;
                            }
                            continue;

                        } else if (!has_decimal) {
                            if(number[index] == '.') {
                                has_decimal = true;
                                decimal_start_index = index + 1;
                                continue;
                            }
                        } 
                    }

                    // handle other characters, and numbers
                    if (!std::isdigit(number[index])) {
                        // if the first number was 0, it may have been octal
                        if(number[has_sign] == '0') {
                            throw octal_input_not_supported_exception();
                        } else {
                            throw invalid_string_number_exception();
                        }
                    } else { // number[index] is a digit
                        if (has_exponent) {
                            exponent *= 10;
                            exponent += number[index] - '0';
                            if (exponent < 0) {
                                throw exponent_overflow_exception();
                            }
                            continue;

                        } else if (has_decimal) {
                            if (number[index] == '0') {
                                if (decimal_count == 0) {
                                    decimal_lhs_zeros++;
                                    continue;
                                } else {
                                    decimal_rhs_zeros++;
                                    continue;
                                }
                            } else {
                                decimal_count++;
                                decimal_count += decimal_rhs_zeros; 
                                decimal_rhs_zeros = 0;
                                continue;
                            }
                        } else { // we're on the integral part
                            if (number[index] == '0') {
                                if (integer_count == 0) {
                                    integer_lhs_zeros++;
                                    continue;
                                } else {
                                    integer_rhs_zeros++;
                                    continue;
                                }
                            } else {
                                integer_count++;
                                integer_count += integer_rhs_zeros;
                                integer_rhs_zeros = 0;
                                continue;
                            }
                        }
                    }
                }

                if (!exp_positive) {
                    exponent *= -1;
                }

                exponent += integer_count;

                if (integer_count == 0) {
                    decimal_start_index += decimal_lhs_zeros;
                    exponent -= decimal_lhs_zeros;
                } else {
                    exponent += integer_rhs_zeros;
                    integer_count += integer_rhs_zeros;
                    if(decimal_count)
                        decimal_count += decimal_lhs_zeros;
                }

                integer_part = number.substr(has_sign + integer_lhs_zeros, integer_count);
                decimal_part = number.substr(decimal_start_index, decimal_count);
                
                // normalizing decimal_part string
                size_t idx = decimal_part.size();
                while(decimal_part[idx-1] == '0')
                    idx--;
                decimal_part = decimal_part.substr(0, idx);

                // if decimal_part is empty then normalize integer_part
                if(decimal_part.empty()){
                    idx = integer_part.size();
                    while(integer_part[idx-1] == '0')
                        idx--;
                    integer_part = integer_part.substr(0, idx);
                }

                return {integer_part, decimal_part, exponent, positive};
            }

            /**
             * Takes a number of the form sign AeB, or A, where A is the desired number in base 10 and 
             * B is the integral exponent, and sign is + or -, and constructs an exact_number
             */
            constexpr explicit exact_number (const std::string& number) {
                auto [integer_part, decimal_part, exponent, positive] = number_from_string((std::string_view)number);

                if (integer_part.empty() && decimal_part.empty()) {
                    digits = {0};
                    exponent = 0;
                    return;
                }
                for (const auto& c : integer_part ) {
                    digits.push_back(c - '0');
                }
                for (const auto& c : decimal_part ) {
                    digits.push_back(c - '0');
                }
                this->exponent=exponent;
                this->digits = digits;
                this->positive=positive;
            }
            

            /**
             * @brief *Copy constructor:* It constructs a new boost::real::exact_number that is a copy of the
             * other boost::real::exact_number.
             *
             * @param other - The boost::real::exact_number to copy.
             */
            exact_number<T>(const exact_number<T> &other) = default;

            /// TODO: move ctor

            /**
             * @brief Default asignment operator.
             *
             * @param other - The boost::real::exact_number to copy.
             */
            exact_number<T> &operator=(const exact_number<T>& other) = default;

            /**
             * @brief *Lower comparator operator:* It compares the *this boost::real::exact_number with the other
             * boost::real::exact_number to determine if *this is lower than other.
             *
             * @param other - The right side operand boost::real::exact_number to compare with *this.
             * @return a bool that is true if and only if *this is lower than other.
             */
            bool operator<(const exact_number& other) const {
                std::vector<T> zero = {0};
                if (this->digits == zero || this->digits.empty()) {
                    return !(other.digits == zero || !other.positive || other.digits.empty());
                } else {
                    if ((other.digits == zero || other.digits.empty()))
                        return !this->positive;
                }
                if (this->positive != other.positive) {
                    return !this->positive;
                }

                if (this->positive) {
                    if (this->exponent == other.exponent) {
                        return aligned_vectors_is_lower(this->digits, other.digits);
                    }

                    return this->exponent < other.exponent;
                }

                if (this->exponent == other.exponent) {
                    return aligned_vectors_is_lower(other.digits, this->digits);
                }

                return other.exponent < this->exponent;
            }

            /**
             * @brief *Greater comparator operator:* It compares the *this boost::real::exact_number with the other
             * boost::real::exact_number to determine if *this is greater than other.
             *
             * @param other - The right side operand boost::real::exact_number to compare with *this.
             * @return a bool that is true if and only if *this is greater than other.
             */
            bool operator>(const exact_number& other) const {
                std::vector<T> zero = {0};
                if (this->digits == zero || this->digits.empty()) {
                    return !(other.digits == zero || other.positive || other.digits.empty());
                } else {
                    if ((other.digits == zero) && this->positive)
                        return true;
                } 
                if (this->positive != other.positive) {
                    return this->positive;
                }

                if (this->positive) {
                    if (this->exponent == other.exponent) {
                        return aligned_vectors_is_lower(other.digits, this->digits);
                    }

                    return this->exponent > other.exponent;
                }

                if (this->exponent == other.exponent) {
                    return aligned_vectors_is_lower(this->digits, other.digits);
                }

                return other.exponent > this->exponent;
            }

            bool operator>=(const exact_number& other) const {
                return (!((*this) < other));
            }

            bool operator<=(const exact_number& other) const {
                return (!((*this) > other));
            }

            /**
             * @brief *Equality comparator operator:* It compares the *this boost::real::exact_number with the other
             * boost::real::exact_number to determine if *this and other are equals or not.
             *
             * @param other - The right side operand boost::real::exact_number to compare with *this.
             * @return a bool that is true if and only if *this is equal to other.
             */
            bool operator==(const exact_number<T>& other) const {
                return !(*this < other || other < *this);
            }

            bool operator!=(const exact_number<T>& other) const {
                return !(*this == other);
            }

            exact_number<T> abs() const {
                exact_number<T> result = (*this);
                result.positive = true;
                return result;
            }

            exact_number<T> operator+(exact_number<T> other) {
                exact_number<T> result;

                if (this->positive == other.positive) {
                        result = *this;
                        result.add_vector(other);
                } else if (other.abs() < this->abs()) {
                        result = *this;
                        result.subtract_vector(other);
                        result.positive = this->positive;
                } else {
                    result = other;
                    result.subtract_vector(*this);
                    result.positive = !this->positive;
                }
                return result;
            }

            void operator+=(exact_number<T> &other) {
                *this = *this + other;
            }

            //Add exact numbers assuming base 10
            exact_number<T> base10_add (exact_number<T> other) {
                exact_number<T> result;

                if (positive == other.positive) {
                        result = *this;
                        result.add_vector(other, 9);
                        result.positive = this->positive;
                } else if (other.abs() < (*this).abs()) {
                        result = *this;
                        result.subtract_vector(other, 9);
                        result.positive = this->positive;
                } else {
                    result = other;
                    result.subtract_vector((*this), 9);
                    result.positive = !this->positive;
                }
                return result;
            }

            exact_number<T> operator-(exact_number<T> other) {
                exact_number<T> result;

                if (this->positive != other.positive) {
                    result = *this;
                    result.add_vector(other);
                    result.positive = this->positive;
                } else {
                    if (other.abs() < this->abs()) {
                        result = *this;
                        result.subtract_vector(other);
                        result.positive = this->positive;
                    } else {
                        result = other;
                        result.subtract_vector(*this);
                        result.positive = !this->positive;
                    }
                }

                return result;
            }

            void operator-=(exact_number<T> &other) {
                *this = *this - other;
            }

            //Subtract exact numbers assuming base 10
            exact_number<T> base10_subtract(exact_number<T> other) {
                exact_number<T> result;

                if (this->positive != other.positive) {
                    result = *this;
                    result.add_vector(other, 9);
                    result.positive = this->positive;
                } else {
                    if (other.abs() < (*this).abs()) {
                        result = *this;
                        result.subtract_vector(other, 9);
                        result.positive = this->positive;
                    } else {
                        result = other;
                        result.subtract_vector(*this, 9);
                        result.positive = !this->positive;
                    }
                }

                return result;
            }

            exact_number<T> operator*(exact_number<T> other) {
                exact_number<T> result = *this;
                result.multiply_vector(other);
                result.positive = (this->positive == other.positive);
                return result;
            }

            void operator*=(exact_number<T> &other) {
                *this = *this * other;
            }

            //Multiply exact numbers assuming base 10
            exact_number<T> base10_mult(exact_number<T> other) {
                exact_number<T> result = *this;
                result.multiply_vector(other, 10);
                result.positive = (this->positive == other.positive);
                return result;
            }

            /**
             * @brief Generates a string representation of the boost::real::exact_number.
             *
             * @return a string that represents the state of the boost::real::exact_number
             */
            std::string as_string() const {
                std::string result = "";   
                exact_number<T> tmp;         

                // If the number is too large, scientific notation is used to print it.
                /* @TODO add back later
                if ((this->exponent < -10) || (this->exponent > (int)this->digits.size() + 10)) {
                    result += "0|.";
                    for (const auto& d: this->digits) {
                        result += std::to_string(d);
                        result += "|";
                    }
                    result += "e" + std::to_string(this->exponent);
                    return result;
                }
                */

                if (this->exponent <= 0) {
                    result += ".";

                    for (int i = this->exponent; i < (int) this->digits.size(); ++i) {
                        if (i < 0) {
                            result += "0";
                            result += " ";
                        } else {
                            result += std::to_string(this->digits[i]);
                            result += " ";
                        }
                    }
                } else {
                    int digit_amount = std::max(this->exponent, (int) this->digits.size());

                    for (int i = 0; i < digit_amount; ++i) {

                        if (i == this->exponent) {
                            result += ".";
                        }

                        if (i < (int) this->digits.size()) {
                            result += std::to_string(this->digits[i]);
                            result += " ";
                        } else {
                            result += "0";
                            result += " ";
                        }
                    }

                    if (result.back() == '.') {
                        result.pop_back();
                    }
                }

                //Use "return result" here for actual internal representation output for debugging

                //Form new string below in base 10.
                std::vector<T> new_result = {0};
                std::size_t dot_pos = result.find('.');
                std::string integer_part;
                std::string decimal_part;
                if (dot_pos == std::string::npos) {
                    integer_part = result;
                    decimal_part = "";
                } else {
                    integer_part = result.substr(0, dot_pos);
                    decimal_part = result.substr(dot_pos + 1);
                }
                std::stringstream ss1(integer_part);
                std::istream_iterator<std::string> begin1(ss1);
                std::istream_iterator<std::string> end1;
                std::vector<std::string> integer(begin1, end1);
                std::stringstream ss2(decimal_part);
                std::istream_iterator<std::string> begin2(ss2);
                std::istream_iterator<std::string> end2;
                std::vector<std::string> decimal(begin2, end2);
                std::reverse (decimal.begin(), decimal.end()); 

                //integer and decimal are string vectors with the "digits" in diff base
                T b = (std::numeric_limits<T>::max() /4)*2;
                std::vector<T> base;
                while (b!=0) {
                    base.push_back(b%10);
                    b /=10;
                }
                std::reverse(base.begin(), base.end());
                while(!integer.empty()) {
                    std::vector<T> temp;
                    std::string num = integer.back();
                    integer.pop_back();
                    for (auto j : num) {
                        temp.push_back(j - '0');
                    }
                    tmp = (exact_number<T>(new_result).base10_add(exact_number<T>(temp)));
                    new_result = tmp.digits;
                    while (tmp.exponent - (int)tmp.digits.size() > 0) {
                        new_result.push_back(0);
                        tmp.exponent--;
                    }
                    for (size_t i = 0; i < integer.size(); ++i) {
                        std::vector<T> temp;
                        std::string tempstr = integer[i];
                        for (size_t j = 0; j < tempstr.length(); ++j) {
                            temp.push_back(tempstr[j] - '0'); 
                        }
                        tmp = (exact_number<T>(temp).base10_mult(exact_number<T>(base)));
                        temp = tmp.digits;
                        while (tmp.exponent - (int)tmp.digits.size() > 0) {
                            temp.push_back(0);
                            tmp.exponent--;
                        }
                        size_t idx = 0;
                        while(idx < temp.size() && temp[idx]==0) 
                            ++idx;
                        temp.erase(temp.begin(), temp.begin() + idx);
                        std::stringstream ss;
                        std::copy( temp.begin(), temp.end(), std::ostream_iterator<T>(ss, ""));
                        std::string str = ss.str();
                        integer[i] = str;
                    }
                }

                std::stringstream ss;
                std::copy( new_result.begin(), new_result.end(), std::ostream_iterator<T>(ss, ""));
                std::string res_decimal = ss.str();
                std::vector<T> new_base = base;
                std::vector<std::vector<T>> powers = {base};
                for (size_t i = 0; i<decimal.size(); ++i) {
                    tmp = (exact_number<T>(new_base).base10_mult(exact_number<T>(base)));
                    new_base = tmp.digits;
                    while (tmp.exponent - (int)tmp.digits.size() > 0) {
                        new_base.push_back(0);
                        tmp.exponent--;
                    }
                    size_t idx = 0;
                    while(idx < new_base.size() && new_base[idx]==0) 
                        ++idx;
                    new_base.erase(new_base.begin(), new_base.begin() + idx);
                    powers.push_back(new_base);
                }
                size_t precision = powers.back().size() + 1;
                std::string zeroes = "";
                for (size_t i = 0; i<precision; ++i)
                    zeroes = zeroes + "0";
                std::vector<T> fraction = {0};
                auto pwr = powers.cbegin();
                while(!decimal.empty()) {
                    std::string tempstr = decimal.back();
                    decimal.pop_back();
                    tempstr = tempstr + zeroes;
                    std::vector<T> temp;
                    for (size_t j = 0; j<tempstr.length(); ++j) {
                        temp.push_back(tempstr[j] - '0'); 
                    }
                    std::vector<T> k = *pwr++;
                    std::vector<T> q;
                    tmp.long_divide_vectors(temp, k, q, 10);
                    tmp = (exact_number<T>(fraction).base10_add(exact_number<T>(q)));
                    fraction = tmp.digits;
                    while (tmp.exponent - (int)tmp.digits.size() > 0) {
                        fraction.push_back(0);
                        tmp.exponent--;
                    }
                }
                std::stringstream sslast;
                while (!fraction.empty() && fraction.back() == 0) {
                    fraction.pop_back();
                    --precision;
                }
                std::copy( fraction.begin(), fraction.end(), std::ostream_iterator<T>(sslast, ""));
                std::string fractionstr = sslast.str();
                while (fractionstr.length() < precision)
                    fractionstr = "0" + fractionstr;
                
                if (fraction.empty())
                    return (positive ? "" : "-") + res_decimal;
                else
                    return (positive ? "" : "-") + res_decimal + "." + fractionstr;
            }

            /**
             * @brief Swaps the boost::real::exact_number value with the value of the other boost::real::exact_number.
             * This operation is a more preformant form of swapping to boost::real::boundaries.
             *
             * @param other - The boost::real::exact_number to swap with *this.
             */
            void swap(exact_number<T> &other) {
                this->digits.swap(other.digits);
                std::swap(this->exponent, other.exponent);
                std::swap(this->positive, other.positive);
            }

            /**
             * @brief add the digit parameter as a new digit of the boost::real::exact_number. The digit
             * is added in the right side of the number.
             *
             * @param digit - The new digit to add.
             */
            void push_back(T digit) {
                this->digits.push_back(digit);
            }

            /**
             * @brief add the digit parameter as a new digit of the boost::real::exact_number. The digit
             * is added in the left side of the number.
             *
             * @param digit - The new digit to add.
             */
            void push_front(T digit) {
                this->digits.insert(this->digits.cbegin(), digit);
            }

            /**
             * @brief Removes extra zeros at the sides to convert the number representation into a
             * normalized representation.
             */
            void normalize() {
                int zeroes_on_left = 0;
                for(int i = 0; i < (int)this->digits.size() - 1 && this->digits[i] == 0; i++) {
                    zeroes_on_left++;
                }
                this->exponent -= zeroes_on_left;
                this->digits.erase(this->digits.begin(), this->digits.begin() + zeroes_on_left);

                while (this->digits.size() > 1 && this->digits.back() == 0) {
                    this->digits.pop_back();
                }

                // Zero could have many representation, and the normalized is the next one.
                if (this->digits.size() == 1 && this->digits.front() == 0) {
                    this->exponent = 0;
                    this->positive = true;
                }
            }

            /**
             * @brief Removes extra zeros at the left side to convert the number representation
             * into a semi normalized representation.
             */
            void normalize_left() {
                int zeroes_on_left = 0;
                for(int i = 0; i < (int)this->digits.size() - 1 && this->digits[i] == 0; i++) {
                    zeroes_on_left++;
                }
                this->exponent -= zeroes_on_left;
                this->digits.erase(this->digits.begin(), this->digits.begin() + zeroes_on_left);
            }

            /**
             * @brief ir clears the number digits.
             */
            void clear() {
                this->digits.clear();
            }

            /**
             * @brief Returns the n-th digit of the boost::real::exact_number.
             *
             * @param n - an int number indicating the index of the requested digit.
             * @return an integer with the value of the number n-th digit.
             */
            T &operator[](int n) {
                return this->digits[n];
            }

            /**
             * @brief It returns the number of digits of the boost::real::exact_number
             *
             * @return an unsigned long representing the number of digits of the boost::real::exact_number
             */
            unsigned long size() {
                return this->digits.size();
            }

            /// returns an exact_number that has the precision given
            exact_number<T> up_to(size_t precision, bool upper) {
                T base = (std::numeric_limits<T>::max() /4)*2 - 1;
                if (precision >= digits.size())
                    return *this;

                exact_number<T> ret = *this;
                ret.digits = std::vector<T>(digits.begin(), digits.begin() + precision);

                bool round = (precision < digits.size());
                if (round) {
                    if(upper) {
                        ret.round_up(base);
                    } else {
                        ret.round_down(base);
                    }
                }

                return ret;
            }

            bool is_integral() { 
                   return digits.size() <= (size_t) exponent;
            }

        };


        namespace literals{

            template<typename T>
            const exact_number<T> minus_one_exact = exact_number<T>("-1");

            template<typename T = int>
            const exact_number<T> zero_exact = exact_number<T>("0");

            template<typename T>
            const exact_number<T> one_exact = exact_number<T>("1");

            template<typename T>
            const exact_number<T> two_exact = exact_number<T>("2");

            template<typename T>
            const exact_number<T> four_exact = exact_number<T>("4");

            template<typename T>
            const exact_number<T> eight_exact = exact_number<T>("8");

        }

        template<> inline int exact_number<int>::mul_mod (int a, int b, int c)
        {
            return ((long long)a * (long long)b )%c;
        }

        template<> inline int exact_number<int>::mult_div(int a, int b, int c)
        {
            return ((long long)a * (long long)b)/c;
        }
    }
}

#endif // BOOST_REAL_EXACT_NUMBER
