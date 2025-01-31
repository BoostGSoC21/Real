#ifndef BOOST_REAL_IRRATIONAL_HELPERS_HPP
#define BOOST_REAL_IRRATIONAL_HELPERS_HPP

#include <vector>
#include <real/real.hpp>
#include <real/convenience.hpp>
#include <math.h>
#include <limits>

namespace boost {
    namespace real {
        namespace irrational {
            // used for extra precision. should be replaced with something more definitive in the future.
            inline const int PLACEHOLDER = 10; 

            /**
             * @brief The function returns the n-th digit of the binary champernowne number.
             *
             * @param n - The number digit index.
             * @return The value of the champernowne number n-th digit
             */
            template <typename T = int>
            T champernowne_binary_get_nth_digit(unsigned int n) {
                // We write it as division of champernowne sequence digits and exponent
                // eg. 0.11011 in base 2 can be written as division of 11011 and 100000
                // And convert them to the correct base of (std::numeric_limits<T>::max() / 4) * 2
                // Using append_digits method for base conversion 
                
                bool nth_digit_found = false;

                exact_number<T> prev_champernowne;
                exact_number<T> champernowne;
                exact_number<T> champernowne_seq;
                exact_number<T> two_pow;
                exact_number<T> error;
                const exact_number<T> max_error(std::vector<T> {1}, -(n + 1), true);
                
                std::vector<int> binary = {1}; 
                append_digits(champernowne_seq.digits, (std::numeric_limits<T>::max() / 4) * 2, binary, 2);

                std::vector<int> powers;
                powers.push_back(0);
                append_digits(two_pow.digits, (std::numeric_limits<T>::max() / 4) * 2, 1, 2);
                append_digits(two_pow.digits, (std::numeric_limits<T>::max() / 4) * 2, powers, 2);
                
                champernowne = champernowne_seq;
                champernowne.divide_vector(two_pow, n + 2, false);
                prev_champernowne = champernowne; 

                do {
                    if (std::all_of(binary.begin(), binary.end(), [](int d) -> bool { return d == 1; })) {

                        for (int i = (int)binary.size() - 1; i >= 0; i--) {
                            binary[i] = 0;
                        }
                        binary.insert(binary.begin(), 1);
                        powers.push_back(0);

                    } else {

                        for (int i = (int)binary.size() - 1; i >= 0; i--) {
                            if (binary[i] == 0) {
                                for (int j = (int)binary.size() - 1; j > i; j--) {
                                    binary[j] = 0;
                                }
                                binary[i] = 1;
                                break;
                            }
                        }
                    }
                    
                    append_digits(champernowne_seq.digits, (std::numeric_limits<T>::max() / 4) * 2, binary, 2);
                    append_digits(two_pow.digits, (std::numeric_limits<T>::max() / 4) * 2, powers, 2);
                    
                    champernowne = champernowne_seq;
                    champernowne.divide_vector(two_pow, n + 2, false);
                    
                    error = champernowne - prev_champernowne;
                    error.positive = true;

                    if (error < max_error) {
                        nth_digit_found = true;
                    }
                    prev_champernowne = champernowne;

                } while (!nth_digit_found);

                return champernowne[n];
            }
            

            // Returns the n-th binary digit of champernowne binary number
            int champernowne_binary_get_nth_binary_digit(unsigned int n) {
                if (n == 0) {
                    return 1;
                }

                // binary digits is the number of digits of the binary number of which n-th digit is a part of
                unsigned int binary_digits = 1, pw = 2;
                while((binary_digits - 1) * pw < n) {
                    binary_digits++; 
                    pw *= 2;
                }
                binary_digits--;

                unsigned int digits_left = n - (binary_digits - 1) * (pw / 2); 
                // binary number to which n-th digit belongs 
                unsigned int binary_num = (pw / 2) + (digits_left - 1) / (binary_digits + 1); 

                // position of n-th digit in binary number
                unsigned int rev_pos = (digits_left) % (binary_digits + 1);
                if (rev_pos == 0) {
                    rev_pos = binary_digits + 1;
                }
                unsigned int pos = binary_digits + 1 - rev_pos; 

                int digit = (binary_num & (1 << pos)) ? 1 : 0;
                return digit;
            }


            /// @TODO: figure out how to avoid unnecessary recalculation by saving
            // some information from the previous iteration.

            /**
             * @brief Returns the exact number version of Pi
             * 
             * @param max_error_exponent - Absolute Error in the result should be < 1*base^(-max_error_exponent)
             * @return The exact number value of Pi
             */
            template <typename T = int>
            exact_number<T> pi(size_t max_error_exponent) {
                // Chudnovsky Algorithm
                // pi = C * ( sum_from_k=0_to_k=x (Mk * Lk / Xk) )^(-1) 
                // increasing x you get more precise pi

                static const boost::real::real_explicit<T> real_k("6");
                static const boost::real::real_explicit<T> real_m("1");
                static const boost::real::real_explicit<T> real_l("13591409");
                static const boost::real::real_explicit<T> real_l0("545140134");
                static const boost::real::real_explicit<T> real_x("1");
                static const boost::real::real_explicit<T> real_x0("-262537412640768000");
                static const boost::real::real_explicit<T> real_s("13591409");

                // real_c is constant C in the above formula
                // its actual value is C = 426880 * sqrt(10005)
                // following approximation for C can be removed 
                // once the square root function is implemented
                static const boost::real::real<T> real_c("42698670.6663333958177128891606596082733208840025090828008380071788526051574575942163017999114556686013457371674940804113922927361812667281931368821705825634600667987664834607957359835523339854848545832762473774912507545850325782197456759912124003920153233212768354462964858373556973060121234587580491432166");

                exact_number<T> K = real_k.get_exact_number();
                exact_number<T> L = real_l.get_exact_number();
                exact_number<T> M = real_m.get_exact_number();
                exact_number<T> X = real_x.get_exact_number();
                exact_number<T> S = real_s.get_exact_number();

                static exact_number<T> L0 = real_l0.get_exact_number();
                static exact_number<T> X0 = real_x0.get_exact_number();
                static exact_number<T> _16(std::vector<T> {16}, 1, true);
                static exact_number<T> _12(std::vector<T> {12}, 1, true);
                static exact_number<T> one("1");

                static boost::real::const_precision_iterator<T> real_c_itr = real_c.get_real_itr();
                real_c_itr.set_maximum_precision(max_error_exponent);
                const exact_number<T> C = real_c_itr.cend().get_interval().lower_bound;


                bool nth_digit_found = false;
                bool first_iteration_over = false;

                exact_number<T> iteration_number = one;
                exact_number<T> prev_pi;
                exact_number<T> pi;
                exact_number<T> error;
                const exact_number<T> max_error(std::vector<T> {1}, -(max_error_exponent), true);

                do {  
                    exact_number<T> temp = K * K * K - _16 * K;
                    temp.divide_vector(iteration_number * iteration_number * iteration_number, max_error_exponent, true);
                    M *= temp;
                    X *= X0;
                    L += L0;
                    K += _12;

                    temp = M * L;
                    temp.divide_vector(X, max_error_exponent, false);
                    S += temp;

                    temp = C;
                    temp.divide_vector(S, max_error_exponent, false);

                    pi = temp;
                    if (!first_iteration_over) {
                        prev_pi = pi;
                        first_iteration_over = true;
                        iteration_number += one;
                    } else {
                        error = pi - prev_pi;
                        error.positive = true;

                        if (error < max_error) {
                            nth_digit_found = true;
                        }
                        iteration_number += one;
                        prev_pi = pi;
                    }

                } while (!nth_digit_found);

                return pi;
            }

            /**
             * @brief Returns the n-th digit of pi
             * 
             * @param n - The number digit index.
             * @return n-th digit of pi
             */
            template <typename T = int>
            T pi_nth_digit(unsigned int n) {
                return pi<T>(n + 1)[n];
            }


            /**
             * @brief Returns the n-th digit of golden ratio, using Newton's method
             * @param n - IThe number digit index
             * @return n-th digit of golden ratio
             * @author Divyam Singal
             **/
            template <typename T = int>
            T golden_ratio_nth_digit(unsigned int n) {
                // initial approximation of phi
                static boost::real::real<T> real_phi("1.6180339");
                static boost::real::const_precision_iterator<T> real_phi_itr = real_phi.get_real_itr();
                real_phi_itr.set_maximum_precision(n + 1);

                exact_number<T> phi = real_phi_itr.cend().get_interval().lower_bound;
                const exact_number<T> max_error(std::vector<T> {1}, -(n + 1), true);
                exact_number<T> error;

                exact_number<T> prev_phi = phi;
                bool nth_digit_found = false;

                do {
                    // Newton's iteration
                    // phi = (phi ^ 2 + 2 * phi)/(phi ^ 2 + 1)
                    exact_number<T> phi_squared = prev_phi * prev_phi;
                    phi = phi_squared + prev_phi * literals::two_exact<T>;
                    phi.divide_vector(phi_squared + literals::one_exact<T>, n + 1, true);

                    error = phi - prev_phi;
                    error.positive = true;

                    if (error < max_error) {
                        nth_digit_found = true;
                    }
                    prev_phi = phi;
                } while(!nth_digit_found);

                return phi[n];
            }
        }
    }
}

#endif //BOOST_REAL_IRRATIONAL_HELPERS_HPP