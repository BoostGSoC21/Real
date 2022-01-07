#ifndef BOOST_REAL_MATH_HPP
#define BOOST_REAL_MATH_HPP

#include <tuple>
#include "real/exact_number.hpp"
#include "real/real_exception.hpp"
#include "real/irrational_helpers.hpp"

namespace boost{
	namespace real{
		/**
		 *  EXPONENT FUNCTION USING TAYLOR EXPANSION
		 * @brief: calculates exponent of a exact_number using taylor expansion
		 * @param: num: the exact number. whose exponent is to be found
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Vikram Singh Chundawat
		 **/
		template<typename T>
		exact_number<T> exponent_taylor(exact_number<T> num, size_t max_error_exponent, bool upper){
			exact_number<T> result("1");
			exact_number<T> term_number("1");
			exact_number<T> factorial("1");
			exact_number<T> cur_term("0");
			exact_number<T> max_error(std::vector<T> {1}, -max_error_exponent, true);
			exact_number<T> x_pow("1");
			do{
				result += cur_term;
				factorial *= term_number;
				term_number = term_number + literals::one_exact<T>;
				x_pow *= num;
				cur_term = x_pow;
				cur_term.divide_vector(factorial, max_error_exponent, upper);

                result.up_to(max_error_exponent, upper);
                factorial.up_to(max_error_exponent, upper);
                x_pow.up_to(max_error_exponent, upper);
			}while(cur_term.abs() > max_error);
			result = result.up_to(max_error_exponent, upper);
			return result;
		}

        /**
		 *  EXPONENT FUNCTION USING BINARY EXPONENTIATION AND TAYLOR EXPANSION
		 * @brief: calculates exponent of a exact_number using binary exponentiation
         *  When num is sufficiently small, taylor expansion is used
		 * @param: num: the exact number. whose exponent is to be found
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
        exact_number<T> exponent(exact_number<T> num, size_t max_error_exponent, bool upper) { 
            // if -1 <= num <= 1, i.e. num is sufficiently small, we use taylor series
            if (num >= literals::minus_one_exact<T> && num <= literals::one_exact<T>) {
                return exponent_taylor(num, max_error_exponent, upper);
            }

            exact_number<T> half_num = num;
            half_num.divide_vector(literals::two_exact<T>, max_error_exponent, upper);

            // (e ^ num) is calculated as (e ^ num/2) * (e ^ num/2)
            exact_number<T> result;
            result = exponent(half_num, max_error_exponent, upper);
            result *= result;
            result = result.up_to(max_error_exponent, upper);
            return result;
        }

		/**
		 *  LOGARITHM(BASE e) FUNCTION USING TAYLOR EXPANSION
		 * @brief: calculates log(base e) of a exact_number using taylor expansion
		 * @param: x: the exact number. whose logarithm (ln(x)) is to be found
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Vikram Singh Chundawat
		 **/
		template<typename T>
		exact_number<T> logarithm_taylor(exact_number<T> x, size_t max_error_exponent, bool upper){
			static const exact_number<T> two("2");
			exact_number<T> result("0");
			exact_number<T> term_number("1");
			unsigned int term_number_int = 1;
			exact_number<T> cur_term("0");
			exact_number<T> x_pow ("1");
			exact_number<T> max_error(std::vector<T> {1}, -max_error_exponent, true);
			
			if(x > literals::zero_exact<T> && x < two){
				do{
					if(term_number_int %2 == 1)
						result -= cur_term;
					else 
						result += cur_term;	
					x_pow = x_pow * (x - literals::one_exact<T>);
					cur_term = x_pow;
                    if (term_number_int % 2 == 1)
					    cur_term.divide_vector(term_number, max_error_exponent, upper);
                    else 
                        cur_term.divide_vector(term_number, max_error_exponent, !upper);  // as we are subtracting next, !upper is passed
					++term_number_int;
					term_number = term_number + literals::one_exact<T>;

                    result.up_to(max_error_exponent, upper);
                    x_pow.up_to(max_error_exponent, upper);
				}while(cur_term.abs() > max_error);
				return result;
			}

			do{
				result += cur_term;
				x_pow = x_pow * (x - literals::one_exact<T>);
				x_pow.divide_vector(x, max_error_exponent, upper);
				cur_term = x_pow ;
				cur_term.divide_vector(term_number, max_error_exponent, upper);
				++term_number_int;
				term_number = term_number + literals::one_exact<T>;

                result.up_to(max_error_exponent, upper);
                x_pow.up_to(max_error_exponent, upper);
			}while(cur_term.abs() > max_error);
			result = result.up_to(max_error_exponent, upper);
			return result;
		}

        // storing log(2) value, which is computed once for recursive procedure
        template<typename T>
        exact_number<T> log_two;
        
        template<typename T>
		exact_number<T> logarithm_recurse(exact_number<T> x, size_t max_error_exponent, bool upper) {
            // if x <= 4, i.e. x is sufficiently small, we use taylor series
            if (x <= literals::four_exact<T>) {
                return logarithm_taylor(x, max_error_exponent, upper);
            }

            exact_number<T> half_x = x;
            half_x.divide_vector(literals::two_exact<T>, max_error_exponent, upper);

            // log(x) is calculated as log(x / 2) + log(2)
            exact_number<T> result = logarithm_recurse(half_x, max_error_exponent, upper);
            result += log_two<T>;
            result = result.up_to(max_error_exponent, upper);
            return result;
        }

        /**
		 *  LOGARITHM(BASE e) FUNCTION USING RECURSION AND TAYLOR EXPANSION
		 * @brief: calculates log(base e) of a exact_number using recursion
         * When x is sufficiently small, taylor expansion is used
		 * @param: x: the exact number. whose logarithm (ln(x)) is to be found
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
		exact_number<T> logarithm(exact_number<T> x, size_t max_error_exponent, bool upper) {
            // log is only defined for numbers greater than 0
            if(x == literals::zero_exact<T> || x.positive == false){
				throw logarithm_not_defined_for_non_positive_number();
			}
            
            // finding log(2) only once for recursive procedure
            log_two<T> = logarithm_taylor(literals::two_exact<T>, max_error_exponent, upper);
            
            exact_number<T> result;
            if (x >= literals::one_exact<T>) {
                result = logarithm_recurse(x, max_error_exponent, upper);
            }
            else {
                // converting x to > 1
                exact_number<T> rev_x = literals::one_exact<T>;
                exact_number<T> minus_one = literals::minus_one_exact<T>;
                // log(x) = -log(1 / x)
                rev_x.divide_vector(x, max_error_exponent, upper);
                result = minus_one * logarithm_recurse(rev_x, max_error_exponent, !upper);
            }
            result = result.up_to(max_error_exponent, upper);
            return result;
        }

        /**
		 *  SQUARE ROOT FUNCTION USING NEWTON'S METHOD
		 * @brief: calculates square root of a exact_number using Newton's Method
		 * @param: x: the exact number whose square root is to be found
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
		exact_number<T> square_root(exact_number<T> x, size_t max_error_exponent, bool upper) {
            // sqrt is defined for non negative numbers
            if (x.positive == false) {
                throw sqrt_not_defined_for_negative_number();
            }
            if (x == literals::zero_exact<T>) {
                return literals::zero_exact<T>;
            }
            
            // initial guess
            exact_number<T> result(x.digits, (x.exponent + 1)/2, true);

            exact_number<T> error;
            exact_number<T> max_error(std::vector<T> {1}, -max_error_exponent, true);
            static exact_number<T> two("2");

            // initial error
            error = x - (result * result);
            error = error.abs();
            error.divide_vector(result, max_error_exponent, upper);

            while (error > max_error) {
                // result = (result + x / result) / 2, Newton's method
                exact_number<T> reverse = x;
                reverse.divide_vector(result, max_error_exponent, upper);
                result = result + reverse;
                result.divide_vector(two, max_error_exponent, upper); 

                exact_number<T> temp;
                temp = error * error;
                temp.divide_vector((two * (error + literals::one_exact<T>)), max_error_exponent, upper);
                error = temp;
            }
            result = result.up_to(max_error_exponent, upper);
            return result;
        }

        template<typename T> 
        struct max_precise_pi
        {
            // most precise pi calculated
            exact_number<T> curr_pi;

            // maximum error exponent of pi calculated
            size_t max_error_exponent_pi = 0;

            // returns pi with the given max_error_exponent
            exact_number<T> get(size_t max_error_exponent, bool upper)
            {
                if (max_error_exponent > max_error_exponent_pi) {
                    // if we need pi at more precision than current precision, we recalculate pi
                    curr_pi = boost::real::irrational::pi<T>(max_error_exponent);
                    max_error_exponent_pi = max_error_exponent;
                    return curr_pi;
                }
                else {
                    // otherwise we use the up_to method on the previous most precise pi
                    return curr_pi.up_to(max_error_exponent, upper);
                }
            }
        };

        template<typename T>
        max_precise_pi<T> pi;

		/**
		 *  SINE FUNCTION USING TAYLOR EXPANSION
		 * @brief: calculates sin(x) of a exact_number using taylor expansion
		 * @param: x: the exact_number, representing angle in radian
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Vikram Singh Chundawat
		 **/
		template<typename T>
		exact_number<T> sine_taylor(exact_number<T> x, size_t max_error_exponent, bool upper){
			exact_number<T> result("0");
			exact_number<T> term_number("0");
			unsigned int term_number_int = 0;
			exact_number<T> cur_term(x);
			exact_number<T> x_pow(x);
			exact_number<T> factorial("1");
			exact_number<T> tmp;
			exact_number<T> x_square = x*x;
			exact_number<T> max_error(std::vector<T> {1}, -max_error_exponent, true);
			static exact_number<T> two("2");
			
			do{
				if(term_number_int % 2 == 0){ // if this term is even
					result += cur_term;
				}
				else 
					result -= cur_term; // if this term is odd
				++term_number_int;
				term_number = term_number + literals::one_exact<T>;
				x_pow *= x_square; // increasing power by two powers of original x
				factorial = factorial * ( two * term_number) * ( (two * term_number) + literals::one_exact<T>); // increasing the values of factorial by two
				cur_term  = x_pow;
                if (term_number_int % 2 == 0) {
				    cur_term.divide_vector(factorial, max_error_exponent, upper);
                }
                else {
                    // as we are subtracting next, !upper is passed
                    cur_term.divide_vector(factorial, max_error_exponent, upper);
                }
                
                result = result.up_to(max_error_exponent, upper);
                factorial = factorial.up_to(max_error_exponent, upper);
                x_pow = x_pow.up_to(max_error_exponent, upper);
			}while(cur_term.abs() > max_error);
			result = result.up_to(max_error_exponent, upper);
			return result;
		}

        /**
		 *  SINE FUNCTION USING ARGUMENT REDUCTION AND TAYLOR EXPANSION
		 * @brief: calculates sin(x) of a exact_number using argument reduction
         *  After reduction, taylor expansion is used
		 * @param: x: the exact_number, representing angle in radian
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
		exact_number<T> sine(exact_number<T> x, size_t max_error_exponent, bool upper) {
            // modifying x to lie between 0 and 2PI, using periodic nature of sine
            exact_number<T> k = x;
            exact_number<T> red_x = x;
            static exact_number<T> two("2");

            // floor(x / 2PI) times 2PI is not needed
            k.divide_vector(two, max_error_exponent, upper);
            k.divide_vector(pi<T>.get(max_error_exponent, upper), max_error_exponent, upper);
            k.floor();

            k = k * two * pi<T>.get(max_error_exponent, !upper); 
            red_x -= k;

            return sine_taylor(red_x, max_error_exponent, upper);
        }

		/**
		 *  COSINE FUNCTION USING TAYLOR EXPANSION
		 * @brief: calculates cos(x) of a exact_number using taylor expansion
		 * @param: x: the exact_number, representing angle in radian
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Vikram Singh Chundawat
		 **/
		template<typename T>
		exact_number<T> cosine_taylor(exact_number<T> x, size_t max_error_exponent, bool upper){
			exact_number<T> result("1");
			exact_number<T> cur_term("0");
			exact_number<T> square_x = x*x;
			exact_number<T> cur_power("1");
			exact_number<T> factorial("1");
			static exact_number<T> two("2");
			exact_number<T> term_number("0");
			exact_number<T> max_error(std::vector<T> {1}, -max_error_exponent, true);
			int term_number_int = 0;
			do{
				if(term_number_int % 2 == 0)
					result += cur_term;
				else 
					result -= cur_term;
				
				factorial *=((two * term_number) + literals::one_exact<T>)*(two * (term_number + literals::one_exact<T>));
				cur_power *= square_x;
				cur_term = cur_power;
                ++ term_number_int;
                if (term_number_int % 2 == 0) {
				    cur_term.divide_vector(factorial, max_error_exponent, upper);
                }
                else {
                    // as we are subtracting next, !upper is passed
                    cur_term.divide_vector(factorial, max_error_exponent, !upper);
                }
				term_number = term_number + literals::one_exact<T>;

				result.up_to(max_error_exponent, upper);
                factorial.up_to(max_error_exponent, upper);
                cur_power.up_to(max_error_exponent, upper);
			}while(cur_term.abs() > max_error);
			result = result.up_to(max_error_exponent, upper);
			return result;
		}

        /**
		 *  COSINE FUNCTION USING ARGUMENT REDUCTION AND TAYLOR EXPANSION
		 * @brief: calculates cos(x) of a exact_number using argument reduction
         * After reduction, taylor expansion is used
		 * @param: x: the exact_number, representing angle in radian
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
		exact_number<T> cosine(exact_number<T> x, size_t max_error_exponent, bool upper) {
            // modifying x to lie between 0 and 2PI, using periodic nature of cosine
            exact_number<T> k = x;
            exact_number<T> red_x = x;
            static exact_number<T> two("2");

            // floor(x / 2PI) times 2PI is not needed
            k.divide_vector(two, max_error_exponent, upper);
            k.divide_vector(pi<T>.get(max_error_exponent, upper), max_error_exponent, upper);
            k.floor();

            k = k * two * pi<T>.get(max_error_exponent, !upper); 
            red_x -= k;

            return cosine_taylor(red_x, max_error_exponent, upper);
        }

		 
		 /**
		 *  SINE AND COSINE FUNCTION USING TAYLOR EXPANSION
		 * @brief: calculates cos(x) and sin(x) of a exact_number using taylor expansion
		 * @param: x: the exact_number, representing angle in radian
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @return: a tuple containing sin(x) and cos(x)
		 * @author: Vikram Singh Chundawat
		 **/
		template<typename T>
		std::tuple<exact_number<T>, exact_number<T> > sin_cos_taylor(exact_number<T> x, size_t max_error_exponent, bool upper){
			exact_number<T> sin_result("0");
			exact_number<T> cos_result("0");
			exact_number<T> cur_sin_term = x;
			exact_number<T> cur_cos_term("1");
			exact_number<T> cur_power = x;
			exact_number<T> factorial("1");
			static exact_number<T> two("2");
			exact_number<T> factorial_number("1");
			unsigned int term_number_int = 0;
			exact_number<T> max_error(std::vector<T> {1}, -max_error_exponent, true);
			do{

				if(term_number_int % 2 == 0){
					sin_result += cur_sin_term;
					cos_result += cur_cos_term;
				}
				else{
					sin_result -= cur_sin_term;
					cos_result -= cur_cos_term;
				}
				++term_number_int;
				factorial_number = factorial_number + literals::one_exact<T>;
				factorial *= factorial_number;
				cur_power *= x;
				cur_cos_term = cur_power;
                if (term_number_int % 2 == 0) {
				    cur_cos_term.divide_vector(factorial, max_error_exponent, upper);
                }
                else {
                    // as we are subtracting next, !upper is passed
                    cur_cos_term.divide_vector(factorial, max_error_exponent, !upper);
                }

				factorial_number = factorial_number + literals::one_exact<T>;
				factorial *= factorial_number;
				cur_power *= x;
				cur_sin_term = cur_power;
				if (term_number_int % 2 == 0) {
				    cur_sin_term.divide_vector(factorial, max_error_exponent, upper);
                }
                else {
                    // as we are subtracting next, !upper is passed
                    cur_sin_term.divide_vector(factorial, max_error_exponent, !upper);
                }
			}while( (cur_cos_term.abs() > max_error) || (cur_sin_term.abs() > max_error) );

			return std::make_tuple(sin_result, cos_result);
		}

        /**
		 *  SINE AND COSINE FUNCTION USING ARGUMENT REDUCTION AND TAYLOR EXPANSION
		 * @brief: calculates cos(x) and sin(x) of a exact_number using argument reduction
         * After reduction, taylor expansion is used
		 * @param: x: the exact_number, representing angle in radian
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @return: a tuple containing sin(x) and cos(x)
		 * @author: Divyam Singal
		 **/
        template<typename T>
		std::tuple<exact_number<T>, exact_number<T> > sin_cos(exact_number<T> x, size_t max_error_exponent, bool upper){
            // modifying x to lie between 0 and 2PI, using periodic nature of sin & cosine
            exact_number<T> k = x;
            exact_number<T> red_x = x;
            static exact_number<T> two("2");

            // floor(x / 2PI) times 2PI is not needed
            k.divide_vector(two, max_error_exponent, upper);
            k.divide_vector(pi<T>.get(max_error_exponent, upper), max_error_exponent, upper);
            k.floor();

            k = k * two * pi<T>.get(max_error_exponent, !upper); 
            red_x -= k;

            return sin_cos_taylor(red_x, max_error_exponent, upper);
        }

		/**
		 *  TANGENT FUNCTION USING TAYLOR EXPANSION
		 * @brief: calculates tan(x) of a exact_number using taylor expansion
		 * @param: x: the exact_number, representing angle in radian
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Vikram Singh Chundawat
		 **/
		template<typename T>
		inline exact_number<T> tangent(exact_number<T> x, size_t max_error_exponent, bool upper){
			auto [result, cos] = sin_cos(x, max_error_exponent, upper);
			result.divide_vector(cos, max_error_exponent, upper);
			result = result.up_to(max_error_exponent, upper);
			return result; 
		}

		/**
		 *  COTANGENT FUNCTION USING TAYLOR EXPANSION
		 * @brief: calculates cot(x) of a exact_number using taylor expansion
		 * @param: x: the exact_number, representing angle in radian
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Vikram Singh Chundawat
		 **/
		template<typename T>
		inline exact_number<T> cotangent(exact_number<T> x, size_t max_error_exponent, bool upper){
			auto [sin, result] = sin_cos(x, max_error_exponent, upper);
			result.divide_vector(sin, max_error_exponent, upper);
			result = result.up_to(max_error_exponent, upper);
			return result; 
		}

		/**
		 *  SECANT FUNCTION USING TAYLOR EXPANSION
		 * @brief: calculates sec(x) of a exact_number using taylor expansion
		 * @param: x: the exact_number, representing angle in radian
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Vikram Singh Chundawat
		 **/
		template<typename T>
		inline exact_number<T> secant(exact_number<T> x, size_t max_error_exponent, bool upper){
			exact_number<T> result("1");
			exact_number<T> cos = cosine(x, max_error_exponent, upper);
			result.divide_vector(cos, max_error_exponent, upper);
			result = result.up_to(max_error_exponent, upper);
			return result;
		}

		/**
		 *  COSECANT FUNCTION USING TAYLOR EXPANSION
		 * @brief: calculates cosec(x) of a exact_number using taylor expansion
		 * @param: x: the exact_number, representing angle in radian
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Vikram Singh Chundawat
		 **/
		template<typename T>
		inline exact_number<T> cosecant(exact_number<T> x, size_t max_error_exponent, bool upper){
			exact_number<T> result("1");
			exact_number<T> sin = sine(x, max_error_exponent, upper);
			result.divide_vector(sin, max_error_exponent, upper);
			result = result.up_to(max_error_exponent, upper);
			return result;
		}


        /**
		 *  INVERSE TANGENT FUNCTION USING TAYLOR EXPANSION
		 * @brief: calculates tan_inverse(x) of a exact_number using taylor expansion
		 * @param: x: the exact_number, x ∈ R
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
        inline exact_number<T> tan_inverse_taylor(exact_number<T> x, size_t max_error_exponent, bool upper) {
            exact_number<T> result("0");
			exact_number<T> denominator("1");
			unsigned int term_number_int = 0;
			exact_number<T> cur_term(x);
			exact_number<T> x_pow(x);
			exact_number<T> x_square = x*x;
			exact_number<T> max_error(std::vector<T> {1}, -max_error_exponent, true);
			static exact_number<T> two("2");
			
			do{
				if(term_number_int % 2 == 0){ // if this term is even
					result += cur_term;
				}
				else 
					result -= cur_term; // if this term is odd
				++term_number_int;
				x_pow *= x_square; // increasing power by two powers of original x
				denominator += two;
				cur_term  = x_pow;
                if (term_number_int % 2 == 0) {
				    cur_term.divide_vector(denominator, max_error_exponent, upper);
                }
                else {
                    // as we are subtracting next, !upper is passed
                    cur_term.divide_vector(denominator, max_error_exponent, !upper);
                }
                
                result = result.up_to(max_error_exponent, upper);
                x_pow = x_pow.up_to(max_error_exponent, upper);
			}while(cur_term.abs() > max_error);
			result = result.up_to(max_error_exponent, upper);
			return result;
        }

        /**
		 *  INVERSE TANGENT FUNCTION USING RECURSION AND TAYLOR EXPANSION
		 * @brief: calculates tan_inverse(x) of a exact_number using recursion
         * When x is sufficiently small, taylor expansion is used
		 * @param: x: the exact_number, x ∈ R
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
        inline exact_number<T> tan_inverse(exact_number<T> x, size_t max_error_exponent, bool upper) {
            static exact_number<T> two("2");
            exact_number<T> bar("1");
            bar.divide_vector(two, max_error_exponent, upper); 
            exact_number<T> neg_bar = bar;
            neg_bar.positive = false;  
            if (x > neg_bar && x < bar) {
                // If -0.5 < x < 0.5, i.e. x is sufficiently small, we use taylor series
                return tan_inverse_taylor(x, max_error_exponent, upper);
            }
            
            // atan(x) = 2 * atan(x / (1 + sqrt(1 + x^2)))
            exact_number<T> red_x = x;
            red_x.divide_vector(square_root((x * x) + literals::one_exact<T>, max_error_exponent, upper) + literals::one_exact<T>, max_error_exponent, upper);
            exact_number<T> result = tan_inverse(red_x, max_error_exponent, upper);
            result = result + result;
            result.up_to(max_error_exponent, upper);
            return result;
        }

        /**
		 *  INVERSE COTANGENT FUNCTION USING INVERSE TANGENT FUNCTION
		 * @brief: calculates cot_inverse(x) of a exact_number using tan_inverse
		 * @param: x: the exact_number, x ∈ R
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
        inline exact_number<T> cot_inverse(exact_number<T> x, size_t max_error_exponent, bool upper) {
            if (x >= literals::one_exact<T>) {
                // (1 / x) <= 1
                // acot(x) = atan(1 / x), i.e. more the x, faster is calculating atan(1 / x)
                exact_number<T> reciprocal("1");
                reciprocal.divide_vector(x, max_error_exponent, upper);
                return tan_inverse(reciprocal, max_error_exponent, upper);
            }
            else if (x <= literals::minus_one_exact<T>) {
                // (1 / x) <= -1
                // acot(x) = pi + atan(1 / x), i.e. more negative the x, faster is calculating atan(1 / x)
                exact_number<T> reciprocal("1");
                reciprocal.divide_vector(x, max_error_exponent, upper);
                return tan_inverse(reciprocal, max_error_exponent, upper) + pi<T>.get(max_error_exponent, upper);
            }
            else { 
                // -1 < x < 1
                // here it is better to calculate atan(x) rather than atan(1 / x)
                static exact_number<T> two("2");
                // acot(x) = pi/2 - atan(x)
                exact_number<T> result = pi<T>.get(max_error_exponent, upper);
                result.divide_vector(two, max_error_exponent, upper);
                result = result - tan_inverse(x, max_error_exponent, !upper);
                result = result.up_to(max_error_exponent, upper);
                return result;
            }
        }

        /**
		 *  INVERSE SINE FUNCTION USING INVERSE TANGENT FUNCTION
		 * @brief: calculates sin_inverse(x) of a exact_number using tan_inverse
		 * @param: x: the exact_number, x ∈ [-1, 1]
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
        inline exact_number<T> sin_inverse(exact_number<T> x, size_t max_error_exponent, bool upper) {
            // asin(x) is defined for x between -1 and 1 inclusive
            if (x < literals::minus_one_exact<T> || x > literals::one_exact<T>) {
                throw max_precision_for_inverse_trigonometric_function_error();
            }
            
            // special cases when x = -1 or 1, to avoid division by 0
            static exact_number<T> two("2");
            if (x == literals::one_exact<T>) {
                exact_number<T> result = pi<T>.get(max_error_exponent, upper);
                result.divide_vector(two, max_error_exponent, upper);
                result = result.up_to(max_error_exponent, upper);
                return result;
            }
            else if (x == literals::minus_one_exact<T>) {
                exact_number<T> result = pi<T>.get(max_error_exponent, upper);
                result.positive = true;
                result.divide_vector(two, max_error_exponent, upper);
                result = result.up_to(max_error_exponent, upper);
                return result;
            }

            // asin(x) = atan(x / sqrt(1 - x * x))
            static exact_number<T> one("1");
            exact_number<T> x_tan = x;
            x_tan.divide_vector(square_root(one - x * x, max_error_exponent, !upper), max_error_exponent, upper);
            return tan_inverse(x_tan, max_error_exponent, upper);
        }

        /**
		 *  INVERSE COSINE FUNCTION USING INVERSE SINE FUNCTION
		 * @brief: calculates cos_inverse(x) of a exact_number using sin_inverse
		 * @param: x: the exact_number, x ∈ [-1, 1]
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
        inline exact_number<T> cos_inverse(exact_number<T> x, size_t max_error_exponent, bool upper) {
            // acos(x) is defined for x between -1 and 1 inclusive
            if (x < literals::minus_one_exact<T> || x > literals::one_exact<T>) {
                throw max_precision_for_inverse_trigonometric_function_error();
            }
            
            static exact_number<T> two("2");
            // acos(x) = pi/2 - asin(x)
            exact_number<T> result = pi<T>.get(max_error_exponent, upper);
            result.divide_vector(two, max_error_exponent, upper);
            result = result - sin_inverse(x, max_error_exponent, !upper);
            result = result.up_to(max_error_exponent, upper);
            return result;
        }

        /**
		 *  INVERSE SECANT FUNCTION USING INVERSE COSINE FUNCTION
		 * @brief: calculates sec_inverse(x) of a exact_number using cos_inverse
		 * @param: x: the exact_number, x ∈ (-∞, -1] U [1, ∞)
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
        inline exact_number<T> sec_inverse(exact_number<T> x, size_t max_error_exponent, bool upper) {
            // asec(x) is defined for x <= -1 or x >= 1
            if (x > literals::minus_one_exact<T> && x < literals::one_exact<T>) {
                throw max_precision_for_inverse_trigonometric_function_error();
            }
            
            // asec(x) = acos(1 / x)
            exact_number<T> reciprocal("1");
            reciprocal.divide_vector(x, max_error_exponent, upper);
            return cos_inverse(reciprocal, max_error_exponent, upper);
        }

        /**
		 *  INVERSE COSECANT FUNCTION USING INVERSE SINE FUNCTION
		 * @brief: calculates cosec_inverse(x) of a exact_number using sin_inverse
		 * @param: x: the exact_number, x ∈ (-∞, -1] U [1, ∞)
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
        inline exact_number<T> cosec_inverse(exact_number<T> x, size_t max_error_exponent, bool upper) {
            // acosec(x) is defined for x <= -1 or x >= 1
            if (x > literals::minus_one_exact<T> && x < literals::one_exact<T>) {
                throw max_precision_for_inverse_trigonometric_function_error();
            }
            
            // asec(x) = asin(1 / x)
            exact_number<T> reciprocal("1");
            reciprocal.divide_vector(x, max_error_exponent, upper);
            return sin_inverse(reciprocal, max_error_exponent, upper);
        }

        /**
		 *  TWO ARGUMENT INVERSE TANGENT FUNCTION USING INVERSE TANGENT FUNCTION
		 * @brief: calculates tan2_inverse(y, x) of a exact_number using tan_inverse
		 * @param: y: the y-coordinate as exact_number
         * @param: x: the x-coordinate as exact_number
		 * @param: max_error_exponent: Absolute Error in the result should be < 1*base^(-max_error_exponent)
		 * @param:  upper: if true: error lies in [0, +epsilon]
		 *                  else: error lies in [-epsilon, 0], here epsilon = 1*base^(-max_error_exponent)
		 * @author: Divyam Singal
		 **/
        template<typename T>
        inline exact_number<T> tan2_inverse(exact_number<T> y, exact_number<T> x, size_t max_error_exponent, bool upper) {
            static exact_number<T> two("2");
            exact_number<T> x_tan;

            if (x > literals::zero_exact<T>) {
                // x > 0
                // atan2(y, x) = atan(y / x)
                x_tan = y;
                x_tan.divide_vector(x, max_error_exponent, upper);
                return tan_inverse(x_tan, max_error_exponent, upper);
            }
            else if (x < literals::zero_exact<T> && y >= literals::zero_exact<T>) {
                // x < 0, y >= 0
                // atan2(y, x) = atan(y / x) + PI
                x_tan = y;
                x_tan.divide_vector(x, max_error_exponent, upper);
                exact_number<T> result = pi<T>.get(max_error_exponent, upper);
                result = result + tan_inverse(x_tan, max_error_exponent, upper);
                result.up_to(max_error_exponent, upper);
                return result;
            }
            else if (x < literals::zero_exact<T> && y < literals::zero_exact<T>) {
                // x < 0, y < 0
                // atan2(y, x) = atan(y / x) - PI
                x_tan = y;
                x_tan.divide_vector(x, max_error_exponent, upper);
                exact_number<T> result = pi<T>.get(max_error_exponent, upper);
                result.positive = false;
                result = result + tan_inverse(x_tan, max_error_exponent, upper);
                result.up_to(max_error_exponent, upper);
                return result;
            }
            else if (x == literals::zero_exact<T> && y > literals::zero_exact<T>) {
                // x = 0, y > 0
                // atan2(y, x) = PI / 2
                exact_number<T> result = pi<T>.get(max_error_exponent, upper);
                result.divide_vector(two, max_error_exponent, upper);
                result.up_to(max_error_exponent, upper);
                return result;
            }
            else if (x == literals::zero_exact<T> && y < literals::zero_exact<T>) {
                // x = 0, y < 0
                // atan2(y, x) = -PI / 2
                exact_number<T> result = pi<T>.get(max_error_exponent, upper);
                result.positive = false;
                result.divide_vector(two, max_error_exponent, upper);
                result.up_to(max_error_exponent, upper);
                return result;
            }
            else {
                // undefined for x = 0 and y = 0
                throw max_precision_for_inverse_trigonometric_function_error();
            }
        }

	}
}

#endif//BOOST_REAL_MATH_HPP