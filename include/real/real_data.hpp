#ifndef BOOST_REAL_REAL_DATA_HPP
#define BOOST_REAL_REAL_DATA_HPP

#include <variant>
#include <assert.h>
#include <iostream>
#include <limits>

#include <real/const_precision_iterator.hpp>
#include <real/interval.hpp>
#include <real/real_explicit.hpp>
#include <real/real_algorithm.hpp>
#include <real/real_operation.hpp>
#include <real/real_exception.hpp>
#include <real/real_rational.hpp>
#include <real/integer_number.hpp>
#include <real/real_math.hpp>

namespace boost { 
    namespace real{

        template <typename T = int>
        class real_data {
            real_number<T> _real;
            const_precision_iterator<T> _precision_itr;

            public:
            /// @TODO: use move constructors, if possible
            
            real_data() = default;
            
            /// copy ctor - constructs real_data from other real_data
            real_data(const real_data<T> &other) : _real(other._real), _precision_itr(other._precision_itr) {};

            // construct from the three different reals 
            real_data(real_explicit<T> x) :_real(x), _precision_itr(&_real) {};
            real_data(real_algorithm<T> x) : _real(x), _precision_itr(&_real) {};
            real_data(real_operation<T> x) : _real(x), _precision_itr(&_real) {};
            real_data(real_rational<T> x) : _real(x), _precision_itr(&_real) {};
            const real_number<T>& get_real_number() const {
                return _real;
            }

            real_number<T> const * get_real_ptr() const {
                return &_real;
            }

            const_precision_iterator<T>& get_precision_itr() {
                return _precision_itr;
            }
        };

        // Now that real_data and const_precision_iterator have been defined, we may now define the following.
        // Note these are all inline to avoid linker issues.

        /* const_precision_iterator member functions */
        /// determines a real_operation's approximation interval from its operands'
        template <typename T>
        inline void const_precision_iterator<T>::update_operation_boundaries(real_operation<T> &ro) {
            switch (ro.get_operation()) {
                case OPERATION::ADDITION:
                    this->_approximation_interval.lower_bound =
                            ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) +
                            ro.get_rhs_itr().get_interval().lower_bound.up_to(_precision, false);

                    this->_approximation_interval.upper_bound =
                            ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) +
                            ro.get_rhs_itr().get_interval().upper_bound.up_to(_precision, true);
                    break;


                case OPERATION::SUBTRACTION:
                    this->_approximation_interval.lower_bound =
                            ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) -
                            ro.get_rhs_itr().get_interval().upper_bound.up_to(_precision, true);

                    this->_approximation_interval.upper_bound =
                            ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) -
                            ro.get_rhs_itr().get_interval().lower_bound.up_to(_precision, false);
                    break;

                case OPERATION::MULTIPLICATION: {
                    bool lhs_positive = ro.get_lhs_itr().get_interval().positive();
                    bool rhs_positive = ro.get_rhs_itr().get_interval().positive();
                    bool lhs_negative = ro.get_lhs_itr().get_interval().negative();
                    bool rhs_negative = ro.get_rhs_itr().get_interval().negative();

                    if (lhs_positive && rhs_positive) { // Positive - Positive
                        this->_approximation_interval.lower_bound =
                                ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) *
                                ro.get_rhs_itr().get_interval().lower_bound.up_to(_precision, false);

                        this->_approximation_interval.upper_bound =
                                ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) *
                                ro.get_rhs_itr().get_interval().upper_bound.up_to(_precision, true);

                    } else if (lhs_negative && rhs_negative) { // Negative - Negative
                        this->_approximation_interval.lower_bound =
                                ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) *
                                ro.get_rhs_itr().get_interval().upper_bound.up_to(_precision, true);

                        this->_approximation_interval.upper_bound =
                                ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) *
                                ro.get_rhs_itr().get_interval().lower_bound.up_to(_precision, false);
                    } else if (lhs_negative && rhs_positive) { // Negative - Positive
                        this->_approximation_interval.lower_bound =
                                ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) *
                                ro.get_rhs_itr().get_interval().upper_bound.up_to(_precision, true);

                        this->_approximation_interval.upper_bound =
                                ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) *
                                ro.get_rhs_itr().get_interval().lower_bound.up_to(_precision, false);

                    } else if (lhs_positive && rhs_negative) { // Positive - Negative
                        this->_approximation_interval.lower_bound =
                                ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) *
                                ro.get_rhs_itr().get_interval().lower_bound.up_to(_precision, false);

                        this->_approximation_interval.upper_bound =
                                ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) *
                                ro.get_rhs_itr().get_interval().upper_bound.up_to(_precision, true);

                    } else { // One is around zero all possible combinations are be tested

                        exact_number<T> current_boundary;

                        // Lower * Lower
                        current_boundary =
                                ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) *
                                ro.get_rhs_itr().get_interval().lower_bound.up_to(_precision, false);

                        this->_approximation_interval.lower_bound = current_boundary;
                        this->_approximation_interval.upper_bound = current_boundary;

                        // Upper * upper
                        current_boundary =
                                ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) *
                                ro.get_rhs_itr().get_interval().upper_bound.up_to(_precision, true);

                        if (current_boundary < this->_approximation_interval.lower_bound) {
                            this->_approximation_interval.lower_bound.up_to(_precision, false) = current_boundary;
                        }

                        if (this->_approximation_interval.upper_bound < current_boundary) {
                            this->_approximation_interval.upper_bound = current_boundary;
                        }

                        // Lower * upper
                        current_boundary =
                                ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) *
                                ro.get_rhs_itr().get_interval().upper_bound.up_to(_precision, true);

                        if (current_boundary < this->_approximation_interval.lower_bound.up_to(_precision, false)) {
                            this->_approximation_interval.lower_bound = current_boundary;
                        }

                        if (this->_approximation_interval.upper_bound < current_boundary) {
                            this->_approximation_interval.upper_bound = current_boundary;
                        }

                        // Upper * lower
                        current_boundary =
                                ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) *
                                ro.get_rhs_itr().get_interval().lower_bound.up_to(_precision, false);

                        if (current_boundary < this->_approximation_interval.lower_bound.up_to(_precision, false)) {
                            this->_approximation_interval.lower_bound = current_boundary;
                        }

                        if (this->_approximation_interval.upper_bound < current_boundary) {
                            this->_approximation_interval.upper_bound = current_boundary;
                        }
                    }
                    break;
                }
                case OPERATION::DIVISION: {
                    T base = (std::numeric_limits<T>::max() / 4) * 2 - 1;
                    exact_number<T> zero = exact_number<T>();
                    exact_number<T> residual;
                    exact_number<T> quotient;
                    exact_number<T> numerator;
                    exact_number<T> denominator;
                    bool deviation_upper_boundary, deviation_lower_boundary;

                    /* if the interval contains zero, iterate until it doesn't, or until maximum_precision. */
                   while (((!ro.get_rhs_itr().get_interval().positive() 
                            && !ro.get_rhs_itr().get_interval().negative() ) 
                            || ro.get_rhs_itr().get_interval().lower_bound == literals::zero_exact<T>
                            || ro.get_rhs_itr().get_interval().upper_bound == literals::zero_exact<T> ) 
                            && _precision <= this->maximum_precision())
                        ++(*this);

                    /* if the interval contains zero after iterating until max precision, throw,
                       because this causes one side of the result interval to tend towards +/-infinity */
                    if (!ro.get_rhs_itr().get_interval().positive() &&
                        !ro.get_rhs_itr().get_interval().negative())
                        throw boost::real::divergent_division_result_exception();

                    
                    /* Upper Boundary */
                    if (ro.get_lhs_itr().get_interval().positive()) {
                        if (ro.get_rhs_itr().get_interval().positive()) {
                            deviation_upper_boundary = true;
                            numerator = ro.get_lhs_itr().get_interval().upper_bound;
                            denominator = ro.get_rhs_itr().get_interval().lower_bound;
                        } else {
                            deviation_upper_boundary = false;
                            numerator = ro.get_lhs_itr().get_interval().lower_bound;
                            denominator = ro.get_rhs_itr().get_interval().lower_bound;
                        }
                    } else if (ro.get_lhs_itr().get_interval().negative()) {
                        if (ro.get_rhs_itr().get_interval().positive()) {
                            deviation_upper_boundary = false;
                            numerator = ro.get_lhs_itr().get_interval().upper_bound;
                            denominator = ro.get_rhs_itr().get_interval().upper_bound;
                        } else if (ro.get_rhs_itr().get_interval().negative()) {
                            deviation_upper_boundary = true;
                            numerator = ro.get_lhs_itr().get_interval().lower_bound;
                            denominator = ro.get_rhs_itr().get_interval().upper_bound;
                        }
                    } else {
                        if (ro.get_rhs_itr().get_interval().positive()) {
                            deviation_upper_boundary = true;
                            numerator = ro.get_lhs_itr().get_interval().upper_bound;
                            denominator = ro.get_rhs_itr().get_interval().lower_bound;
                        } else if (ro.get_rhs_itr().get_interval().negative()) {
                            deviation_upper_boundary = true;
                            numerator = ro.get_lhs_itr().get_interval().lower_bound;
                            denominator = ro.get_rhs_itr().get_interval().upper_bound;
                        }
                    }

                    quotient = numerator;
                    quotient.divide_vector(denominator, this->_precision, deviation_upper_boundary);

                    this->_approximation_interval.upper_bound = quotient;

                    /* Lower Boundary */
                    if (ro.get_lhs_itr().get_interval().positive()) {
                        if (ro.get_rhs_itr().get_interval().positive()) {
                            deviation_lower_boundary = false;
                            numerator = ro.get_lhs_itr().get_interval().lower_bound;
                            denominator = ro.get_rhs_itr().get_interval().upper_bound; 
                        } else {
                            deviation_lower_boundary = true;
                            numerator = ro.get_lhs_itr().get_interval().upper_bound;
                            denominator = ro.get_rhs_itr().get_interval().upper_bound;
                        }
                    } else if (ro.get_lhs_itr().get_interval().negative()) {
                        if (ro.get_rhs_itr().get_interval().positive()) {
                            deviation_lower_boundary = true;
                            numerator = ro.get_lhs_itr().get_interval().lower_bound;
                            denominator = ro.get_rhs_itr().get_interval().lower_bound;
                        } else if (ro.get_rhs_itr().get_interval().negative()) {
                            deviation_lower_boundary = false;
                            numerator = ro.get_lhs_itr().get_interval().upper_bound;
                            denominator = ro.get_rhs_itr().get_interval().lower_bound;
                        }
                    } else {
                        if (ro.get_rhs_itr().get_interval().positive()) {
                            deviation_lower_boundary = true;
                            numerator = ro.get_lhs_itr().get_interval().lower_bound;
                            denominator = ro.get_rhs_itr().get_interval().lower_bound;
                        } else if (ro.get_rhs_itr().get_interval().negative()) {
                            deviation_lower_boundary = true;
                            numerator = ro.get_lhs_itr().get_interval().upper_bound;
                            denominator = ro.get_rhs_itr().get_interval().upper_bound;
                        }
                    }

                    quotient = numerator;
                    quotient.divide_vector(denominator, this->_precision, deviation_lower_boundary );

                    this->_approximation_interval.lower_bound = quotient;

                    break;
                }
                case OPERATION::INTEGER_POWER: {
                    ro.get_rhs_itr().iterate_n_times(ro.get_rhs_itr().maximum_precision());

                    if (ro.get_rhs_itr().get_interval().lower_bound != ro.get_rhs_itr().get_interval().upper_bound ||
                        (int) ro.get_rhs_itr().get_interval().lower_bound.digits.size() > ro.get_rhs_itr().get_interval().lower_bound.exponent) {
                        throw non_integral_exponent_exception();
                    }

                    if(ro.get_rhs_itr().get_interval().upper_bound.positive == false){
                        throw negative_integers_not_supported();
                    }

                    exact_number<T> exponent = ro.get_rhs_itr().get_interval().upper_bound, _2, zero = exact_number<T> (), tmp;
                    _2.digits = {2};
                    _2.exponent = 1;

                    std::vector<T> exponent_vector, quotient, remainder;
                    exponent_vector = exponent.digits;
                    while ((int) exponent_vector.size() < exponent.exponent) {
                        exponent_vector.push_back(0);
                    }

                    tmp.division_by_single_digit(exponent_vector, std::vector<T> {2}, quotient, remainder);

                    bool exponent_is_even = false;

                    if (remainder.empty()) {
                        exponent_is_even = true;
                    }

                    if (ro.get_lhs_itr().get_interval().positive()) {
                        this->_approximation_interval.upper_bound = 
                                tmp.binary_exponentiation(ro.get_lhs_itr().get_interval().upper_bound, exponent);
                        this->_approximation_interval.lower_bound =
                                tmp.binary_exponentiation(ro.get_lhs_itr().get_interval().lower_bound, exponent);
                    } else if (ro.get_lhs_itr().get_interval().negative()) {
                        if (exponent_is_even) {
                            this->_approximation_interval.upper_bound =
                                    tmp.binary_exponentiation(ro.get_lhs_itr().get_interval().lower_bound, exponent);
                            this->_approximation_interval.lower_bound =
                                    tmp.binary_exponentiation(ro.get_lhs_itr().get_interval().upper_bound, exponent);
                        } else {
                            this->_approximation_interval.upper_bound =
                                    tmp.binary_exponentiation(ro.get_lhs_itr().get_interval().upper_bound, exponent);
                            this->_approximation_interval.lower_bound =
                                    tmp.binary_exponentiation(ro.get_lhs_itr().get_interval().lower_bound, exponent);
                        }
                    } else {
                        if (exponent_is_even) {
                            if (ro.get_lhs_itr().get_interval().upper_bound.abs() > ro.get_lhs_itr().get_interval().lower_bound.abs()) {
                                this->_approximation_interval.upper_bound =
                                        tmp.binary_exponentiation(ro.get_lhs_itr().get_interval().upper_bound, exponent);
                                this->_approximation_interval.lower_bound = zero;
                            } else {
                                this->_approximation_interval.upper_bound =
                                        tmp.binary_exponentiation(ro.get_lhs_itr().get_interval().lower_bound, exponent);
                                this->_approximation_interval.lower_bound = zero;
                            }
                        } else {
                            this->_approximation_interval.upper_bound =
                                    tmp.binary_exponentiation(ro.get_lhs_itr().get_interval().upper_bound, exponent);
                            this->_approximation_interval.lower_bound =
                                    tmp.binary_exponentiation(ro.get_lhs_itr().get_interval().lower_bound, exponent);
                        }
                    }

                    break;
                }

                case OPERATION::EXPONENT :{
                    this->_approximation_interval.lower_bound = 
                        exponent(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision, false);
                    this->_approximation_interval.upper_bound = 
                        exponent(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision, true);
                    break;
                }

                case OPERATION::LOGARITHM :{
                    // if upper bound of number is zero or negative, then it is sure that number is out of domain
                    if(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) == literals::zero_exact<T> || ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true).positive == false){
                        throw logarithm_not_defined_for_non_positive_number();
                    }
                    // now if we get our lower bound as negative, then we iterate for more precise input, until maximum precision is reached or we get positive lower bound
                    while(true){
                        if(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, true) == literals::zero_exact<T> || ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, true).positive == false){
                            if(_precision >= ro.get_lhs_itr().maximum_precision()){
                                        throw logarithm_not_defined_for_non_positive_number();
                            }
                            ro.get_lhs_itr().iterate_n_times(1);
                            ++_precision;
                        }
                        else break;
                    }
                    this->_approximation_interval.lower_bound = 
                        logarithm(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision, false);
                    this->_approximation_interval.upper_bound = 
                        logarithm(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision, true);
                    break;
                }

                case OPERATION::SQRT :{
                    // if upper bound of number is zero or negative, then it is sure that number is out of domain
                    if (ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true).positive == false) {
                        throw sqrt_not_defined_for_negative_number();
                    }
                    // now if we get our lower bound as negative, then we iterate for more precise input, until maximum precision is reached or we get a non-negative lower bound
                    while(true) {
                        if (ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false).positive == false) {
                            if (_precision >= ro.get_lhs_itr().maximum_precision()) {
                                throw sqrt_not_defined_for_negative_number();
                            }
                            ro.get_lhs_itr().iterate_n_times(1);
                            ++_precision;
                        }
                        else break;
                    }

                    // sqrt is an increasing function in its domain
                    this->_approximation_interval.lower_bound = 
                        square_root(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision, false);
                    this->_approximation_interval.upper_bound = 
                        square_root(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision, true);
                    break;
                }

                case OPERATION::SIN :{
                    /** 
                     * First we ensure that our input interval is greater than 2π or not. We can either check that by comparing the difference
                     * of upper and lower bound with 2π or a number greater than 2π. We will check whether the difference is greater than 8 or not.
                     * If the difference is greater than 8 then we are sure that difference is greater than 2π. We are not using π here for checking because
                     * algorithm for π is very complex and checking with that number will be very inefficient.
                     * The condition of difference between 2π and 8 will be checked in next case. Here we will only check whether our number is
                     * greater than 8 or not. If it is, then we will give hardcoded output of [-1, 1] because the result can be anything between -1 to 1.
                     **/
                    if(ro.get_lhs_itr().get_interval().upper_bound - ro.get_lhs_itr().get_interval().lower_bound >= literals::eight_exact<T>){
                        this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                        this->_approximation_interval.upper_bound = literals::one_exact<T>;
                        break;
                    }
                    /**
                     * Now we will check whether the difference is greater than 4 or not.
                     * 4 is greater than π, so we are sure that there is at least one minima/maxima.
                     * But there can exist both minima and maxima, as input can be anywhere between [4,8).
                     * So, we will check sign of derivative of sin, which is cos. If there is one minima/maxima, sign of derivative
                     * will change once, so we will different signs of derivative on upper and lower bound. If there are both minima and maxima,
                     * sign of derivative will change twice, so at the end, sign of derivative in both upper and lower bound will remain same.
                     **/
                    exact_number<T> sin_lower,sin_upper,cos_lower,cos_upper;
                    std::tie(sin_lower, cos_lower) = sin_cos(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, false);
                    std::tie(sin_upper, cos_upper) = sin_cos(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, true);
                    if(ro.get_lhs_itr().get_interval().upper_bound - ro.get_lhs_itr().get_interval().lower_bound >= literals::four_exact<T>){
                        /**
                         * If sign of derivative, which cos(x), if it is same for both upper and lower bound. Then we will return 
                         * hardcoded output [-1, 1].
                         **/
                        if(cos_upper.positive == cos_lower.positive){
                            this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                            this->_approximation_interval.upper_bound = literals::one_exact<T>;
                        }
                        /**
                         * Now, the sign was changed, then there is either one minima/maxima or three maxima minima.
                         * There can exist either one minima/maxima or three points of maxima. In case of three points of maxima, and 
                         * difference between inputs less than 8, both ends of sin will have same sign and sin(x) of mid point of upper and 
                         * lower bound will have opposite sign from end points. In that case, we will give hard coded output [-1, 1]. 
                         **/
                        else{
                            auto mid = ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) + ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false);
                            mid.divide_vector(literals::two_exact<T>, _precision, true);
                            if(sin_lower.positive == sin_upper.positive 
                                && sine(mid, _precision, true).positive != sin_upper.positive){
                                this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                                this->_approximation_interval.upper_bound = literals::one_exact<T>;
                            }
                            /**
                             * We will check sign of lower bound of derivative, if it is positive, then initially function was increasing
                             * then it would have gone up to 1 and then started decreasing. So, the output will be [min(sin(upper_bound), sin(lower_bound))), 1].
                            **/
                            else if(cos_lower.positive){
                                this->_approximation_interval.lower_bound = std::min(sin_upper, sin_lower);
                                this->_approximation_interval.upper_bound = literals::one_exact<T>;
                            }
                            /**
                             * Now if the sign of derivative is negative, then initially function was decreasing. So, the function would have got to -1 and then again 
                             * started increasing. So, the output should be [-1, max(sin_upper, sin_lower)].
                             **/
                            else{
                                this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                                this->_approximation_interval.upper_bound = std::max(sin_upper, sin_lower);
                            }
                        }
                    }
                    /**
                     * Now, the final case, the difference is less than 4.
                     * There are three possibilities, no maxima/minima, one maxima/minima or both maxima and minima.
                     * If sign of derivative changes, then one maxima/minima, if it doesn't, then no maxima/minima or both maxima and minima
                     **/
                    else{
                        /**
                         * If sign does not change, then no maxima/minima or both maxima and minima.
                         * We will check sign of derivative at mid of interval, if it is not same as that of lower and upper bounds.
                         * Then we have both maxima and minima.
                         **/
                        if(cos_upper.positive == cos_lower.positive){
                             // if sign of derivative at mid point is not same as at end points then both minima and maxima are there
                            auto mid = ro.get_lhs_itr().get_interval().upper_bound + ro.get_lhs_itr().get_interval().lower_bound;
                            mid.divide_vector(literals::two_exact<T>, _precision, true);
                            if(cosine(mid, _precision, true).positive != cos_lower.positive){
                                this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                                this->_approximation_interval.upper_bound = literals::one_exact<T>;
                            }
                            // If cos(x), which is derivative of sin(x), is positive, then function was increasing in that interval. So, the output will be [sin_lower, sin_upper].
                            else if(cos_lower.positive){
                                this->_approximation_interval.lower_bound = sin_lower;
                                this->_approximation_interval.upper_bound = sin_upper;
                            }
                            // If it is negative, then function was decreasing in that interval. SO, the output will be [sin_upper, sin_lower].
                            else{
                                this->_approximation_interval.lower_bound = sin_upper;
                                this->_approximation_interval.upper_bound = sin_lower;
                            }
                        }
                        /**
                         * If sign changes, then one maxima/minima. So, we will check sign of derivative at lower bound of input.
                         * If it is positive, then function was increasing initially. So, the output will be [min(sin_lower, sin_upper), 1].
                         * If it is negative, then function was decreasing initially. So, the output will be [-1, max(sin_upper, sin_lower)].
                         **/
                        else{
                            if(cos_lower.positive){
                                this->_approximation_interval.lower_bound = std::min(sin_lower, sin_upper);
                                this->_approximation_interval.upper_bound = literals::one_exact<T>;
                            }
                            else{
                                this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                                this->_approximation_interval.upper_bound = std::max(sin_lower, sin_upper);
                            }
                        }

                    }
                    break;
                }

                case OPERATION::COS :{
                    /** 
                     * First we ensure that our input interval is greater than 2π or not. We can either check that by comparing the difference
                     * of upper and lower bound with 2π or a number greater than 2π. We will check whether the difference is greater than 8 or not.
                     * If the difference is greater than 8 then we are sure that difference is greater than 2π. We are not using π here for checking because
                     * algorithm for π is very complex and checking with that number will be very inefficient.
                     * The condition of difference between 2π and 8 will be checked in next case. Here we will only check whether our number is
                     * greater than 8 or not. If it is, then we will give hardcoded output of [-1, 1] because the result can be anything between -1 to 1.
                     **/
                    if(ro.get_lhs_itr().get_interval().upper_bound - ro.get_lhs_itr().get_interval().lower_bound >= literals::eight_exact<T>){
                        this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                        this->_approximation_interval.upper_bound = literals::one_exact<T>;
                        break;
                    }
                    /**
                     * Now we will check whether the difference is greater than 4 or not.
                     * 4 is greater than π, so we are sure that there is at least one minima/maxima.
                     * But there can exist both minima and maxima, as input can be anywhere between [4,8).
                     * So, we will check sign of derivative of cos(x), which is -sin(x). If there is one minima/maxima, sign of derivative
                     * will change once, so we will different signs of derivative on upper and lower bound. If there are both minima and maxima,
                     * sign of derivative will change twice, so at the end, sign of derivative in both upper and lower bound will remain same.
                     **/
                    exact_number<T> sin_lower,sin_upper,cos_lower,cos_upper;
                    std::tie(sin_lower, cos_lower) = sin_cos(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, false);
                    std::tie(sin_upper, cos_upper) = sin_cos(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, true);
                    if(ro.get_lhs_itr().get_interval().upper_bound - ro.get_lhs_itr().get_interval().lower_bound >= literals::four_exact<T>){
                        /**
                         * If sign of derivative, which -sin(x), if it is same for both upper and lower bound. Then we will return 
                         * hardcoded output [-1, 1].
                         **/
                        if(sin_upper.positive == sin_lower.positive){
                            this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                            this->_approximation_interval.upper_bound = literals::one_exact<T>;
                        }
                        /**
                         * Now, the sign of derivative is changed, then there is either one minima/maxima or three maxima minima.
                         * There can exist either one minima/maxima or three points of maxima. In case of three points of maxima, 
                         * difference between inputs less than 8, both ends of cos(x) will have same sign and cos(x) of mid point of upper and 
                         * lower bound will have opposite sign from end points. In that case, we will give hard coded output [-1, 1]. 
                         **/
                        else{
                            auto mid = ro.get_lhs_itr().get_interval().upper_bound + ro.get_lhs_itr().get_interval().lower_bound;
                            mid.divide_vector(literals::two_exact<T>, _precision, true);
                            if(cos_lower.positive == cos_upper.positive 
                                && cosine(mid, _precision, true).positive != cos_upper.positive){
                                this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                                this->_approximation_interval.upper_bound = literals::one_exact<T>;
                            }
                            /**
                             * We will check sign of lower bound of sin(x), if it is negative, then initially function was increasing
                             * then it would have gone up to 1 and then started decreasing. So, the output will be [min(cos(upper_bound), cos(lower_bound))), 1].
                            **/
                            else if(!sin_lower.positive){
                                this->_approximation_interval.lower_bound = std::min(cos_upper, cos_lower);
                                this->_approximation_interval.upper_bound = literals::one_exact<T>;
                            }
                            /**
                             * Now if the sign of cos(lower_bound) is positive, then initially function was decreasing. So, the function would have got to -1 and then again 
                             * started increasing. So, the output should be [-1, max(cos_upper, cos_lower)].
                             **/
                            else{
                                this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                                this->_approximation_interval.upper_bound = std::max(cos_upper, cos_lower);
                            }
                        }
                    }
                    /**
                     * Now, the final case, the difference is less than 4.
                     * There are three possibilities, no maxima/minima or one maxima/minima or both maxima and minima.
                     **/
                    else{
                        /**
                         * If sign does not changes, then no maxima/minima or both maxima and minima.
                         * We will check sign of derivative at mid of interval, if it is not same as that of lower and upper bounds.
                         * Then we have both maxima and minima.
                         **/
                        if(sin_upper.positive == sin_lower.positive){
                            // if sign of derivative at mid point is not same as at end points then both minima and maxima are there
                            auto mid = ro.get_lhs_itr().get_interval().upper_bound + ro.get_lhs_itr().get_interval().lower_bound;
                            mid.divide_vector(literals::two_exact<T>, _precision, true);
                            if(sine(mid, _precision, true).positive != sin_lower.positive){
                                this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                                this->_approximation_interval.upper_bound = literals::one_exact<T>;
                            }
                            // If -sin(x), which is derivative of cos(x), is positive, then function was increasing in that interval. So, the output will be [cos_lower, cos_upper].
                            else if(!sin_lower.positive){
                                this->_approximation_interval.lower_bound = cos_lower;
                                this->_approximation_interval.upper_bound = cos_upper;
                            }
                            // If it is negative, then function was decreasing in that interval. SO, the output will be [cos_upper, cos_lower].
                            else{
                                this->_approximation_interval.lower_bound = cos_upper;
                                this->_approximation_interval.upper_bound = cos_lower;
                            }
                        }
                        /**
                         * If sign changes, then one maxima/minima. So, we will check sign of derivative at lower bound of input.
                         * If it is positive, then function was increasing initially. So, the output will be [min(cos_lower, cos_upper), 1].
                         * If it is negative, then function was decreasing initially. So, the output will be [-1, max(cos_upper, cos_lower)].
                         **/
                        else{
                            if(!sin_lower.positive){
                                this->_approximation_interval.lower_bound = std::min(cos_lower, cos_upper);
                                this->_approximation_interval.upper_bound = literals::one_exact<T>;
                            }
                            else{
                                this->_approximation_interval.lower_bound = literals::minus_one_exact<T>;
                                this->_approximation_interval.upper_bound = std::max(cos_lower, cos_upper);
                            }
                        }

                    }
                    break;
                }

                case OPERATION::TAN :{
                    // we will keep on iterating until we get our interval in domain of tan(x)
                    exact_number<T> sin_lower, cos_lower, sin_upper, cos_upper, sin_lower_tmp, sin_upper_tmp, cos_upper_tmp, cos_lower_tmp;           
                    while(true)
                    {
                        

                        bool iterate_again;
                        if(ro.get_lhs_itr().get_interval().upper_bound - ro.get_lhs_itr().get_interval().lower_bound >= literals::four_exact<T>){
                            iterate_again = true;
                        }
                        else{
                            std::tie(sin_lower_tmp, cos_lower_tmp) = sin_cos(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, false);
                            std::tie(sin_upper_tmp, cos_upper_tmp) = sin_cos(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, true);
                            /**
                             * Now if difference between lower and upper bounds of interval is less than 4, then there can exist 0,1 or 2 minima/maxima points.
                             * First we will check whether the sign of cos(x) from lower to upper bound is changed or not, if it is, then we have one point 
                             * of maxima/minima. Then we will iterate for better input.
                             **/
                            if(cos_upper_tmp.positive != cos_lower_tmp.positive || cos_lower_tmp == literals::zero_exact<T> || cos_upper_tmp == literals::zero_exact<T>){
                                iterate_again = true;
                            }
                            /**
                             * Now, if sign is not changed, then we will check if sign of mid point of derivative is same as end points.
                             * If it is, then no points of minima/maxima, no need to iterate further.
                             **/
                            else{
                                auto mid = ro.get_lhs_itr().get_interval().lower_bound + ro.get_lhs_itr().get_interval().upper_bound;
                                mid.divide_vector(literals::two_exact<T>, _precision, true);
                                if(cosine(mid, _precision, true).positive != cos_lower_tmp.positive){
                                    iterate_again = true;
                                }
                                else{
                                    iterate_again = false;
                                }
                            }
                        }
                        // if we have point of maxima of minima in our input interval
                        if(iterate_again){
                            // updating the boundaries of lhs
                            if(_precision >= ro.get_lhs_itr().maximum_precision()){
                                    throw max_precision_for_trigonometric_function_error();
                            }
                            ro.get_lhs_itr().iterate_n_times(1);
                            ++_precision;
                        }
                        else{
                            sin_lower = sin_lower_tmp;
                            sin_upper = sin_upper_tmp;
                            cos_lower = cos_lower_tmp;
                            cos_upper = cos_upper_tmp;
                            break;
                        }
                    }
                    sin_lower.divide_vector(cos_lower, _precision, false);
                    sin_upper.divide_vector(cos_upper, _precision, true);
                    this->_approximation_interval.lower_bound = sin_lower;
                    this->_approximation_interval.upper_bound = sin_upper;
                    break;

                }

                case OPERATION::COT :{
                    exact_number<T> sin_lower, cos_lower, sin_upper, cos_upper,sin_lower_tmp, sin_upper_tmp, cos_lower_tmp, cos_upper_tmp;                   
                    while(true)
                    {
                        bool iterate_again;
                        if(ro.get_lhs_itr().get_interval().upper_bound - ro.get_lhs_itr().get_interval().lower_bound >= literals::four_exact<T>){
                            iterate_again = true;
                        }
                        else{
                            std::tie(sin_lower_tmp, cos_lower_tmp) = sin_cos(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, false);
                            std::tie(sin_upper_tmp, cos_upper_tmp) = sin_cos(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, true);
                            /**
                             * Now if difference between lower and upper bounds of interval is less than 4, then there can exist 0,1 or 2 minima/maxima points.
                             * First we will check whether the sign of sin(x) from lower to upper bound is changed or not, if it is, then we have one point 
                             * of maxima/minima. Then we will iterate for better input.
                             **/
                            if(sin_upper_tmp.positive != sin_lower_tmp.positive || sin_lower_tmp == literals::zero_exact<T> || sin_upper_tmp == literals::zero_exact<T>){
                                iterate_again = true;
                            }
                            /**
                             * Now, if sign is not changed, then we will check if sign of mid point of derivative is same as end points.
                             * If it is, then no points of minima/maxima, no need to iterate further.
                             **/
                            else{
                                auto mid = ro.get_lhs_itr().get_interval().lower_bound + ro.get_lhs_itr().get_interval().upper_bound;
                                mid.divide_vector(literals::two_exact<T>, _precision, true);
                                if(sine(mid, _precision, true).positive != sin_lower_tmp.positive){
                                    iterate_again = true;
                                }
                                else{
                                    iterate_again = false;
                                }
                            }
                        }
                        // if we have point of maxima of minima in our input interval
                        if(iterate_again){
                            // updating the boundaries of lhs
                            if(_precision >= ro.get_lhs_itr().maximum_precision()){
                                    throw max_precision_for_trigonometric_function_error();
                            }
                            ro.get_lhs_itr().iterate_n_times(1);
                            ++_precision;
                        }
                        else{
                            sin_lower = sin_lower_tmp;
                            sin_upper = sin_upper_tmp;
                            cos_lower = cos_lower_tmp;
                            cos_upper = cos_upper_tmp;
                            break;
                        }
                    }
                    cos_lower.divide_vector(sin_lower, _precision, false);
                    cos_upper.divide_vector(sin_upper, _precision, true);
                    this->_approximation_interval.lower_bound = cos_upper;
                    this->_approximation_interval.upper_bound = cos_lower;
                    break;

                }

                case OPERATION::SEC :{
                    exact_number<T> sin_lower, cos_lower, sin_upper, cos_upper, sin_lower_tmp, sin_upper_tmp, cos_lower_tmp, cos_upper_tmp;

                    while(true){
                        

                        bool iterate_again;
                        if(ro.get_lhs_itr().get_interval().upper_bound - ro.get_lhs_itr().get_interval().lower_bound >= literals::four_exact<T>){
                            iterate_again = true;
                        }
                        else{
                            std::tie(sin_lower_tmp, cos_lower_tmp) = sin_cos(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, false);
                            std::tie(sin_upper_tmp, cos_upper_tmp) = sin_cos(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, true);
                            /**
                             * Now if difference between lower and upper bounds of interval is less than 4, then there can exist 0,1 or 2 minima/maxima points.
                             * First we will check whether the sign of cos(x) from lower to upper bound is changed or not, if it is, then we have one point 
                             * of maxima/minima. Then we will iterate for better input.
                             **/
                            if(cos_upper_tmp.positive != cos_lower_tmp.positive || cos_lower_tmp == literals::zero_exact<T> || cos_upper_tmp == literals::zero_exact<T>){
                                iterate_again = true;
                            }
                            /**
                             * Now, if sign is not changed, then we will check if sign of mid point of derivative is same as end points.
                             * If it is, then no points of minima/maxima, no need to iterate further.
                             **/
                            else{
                                auto mid = ro.get_lhs_itr().get_interval().lower_bound + ro.get_lhs_itr().get_interval().upper_bound;
                                mid.divide_vector(literals::two_exact<T>, _precision, true);
                                if(cosine(mid, _precision, true).positive != cos_lower_tmp.positive){
                                    iterate_again = true;
                                }
                                else{
                                    iterate_again = false;
                                }
                            }
                        }
                        // if we have point of maxima of minima in our input interval
                        if(iterate_again){
                            // updating the boundaries of lhs
                            if(_precision >= ro.get_lhs_itr().maximum_precision()){
                                    throw max_precision_for_trigonometric_function_error();
                            }
                            ro.get_lhs_itr().iterate_n_times(1);
                            ++_precision;
                        }
                        else{
                            sin_lower = sin_lower_tmp;
                            sin_upper = sin_upper_tmp;
                            cos_lower = cos_lower_tmp;
                            cos_upper = cos_upper_tmp;
                            break;
                        }
                    }

                    // derivative of sec(x) is sec(x)tan(x)
                    exact_number<T> derivative_lower = sin_lower;
                    derivative_lower.divide_vector(cos_lower*cos_lower, _precision, false);
                    
                    
                    exact_number<T> derivative_upper = sin_upper;
                    derivative_upper.divide_vector(cos_upper*cos_upper, _precision, true); 
                    // checking for point of minima
                    if(derivative_lower.positive != derivative_upper.positive){
                        // if minima exists and either number is positive, then lower end of resulting interval is 1
                        if(cos_upper.positive){
                            this->_approximation_interval.lower_bound = exact_number<T>("1");
                            this->_approximation_interval.upper_bound = exact_number<T>("1");
                            if(cos_upper > cos_lower){
                                this->_approximation_interval.upper_bound.divide_vector(cos_lower, _precision, true);
                            }
                            else{
                                this->_approximation_interval.upper_bound.divide_vector(cos_upper, _precision, true);
                            }
                        }
                        else{
                            this->_approximation_interval.upper_bound = exact_number<T>("-1");
                            this->_approximation_interval.lower_bound = exact_number<T>("1");
                            if(cos_upper > cos_lower){
                                this->_approximation_interval.lower_bound.divide_vector(cos_lower, _precision, true);
                            }
                            else{
                                this->_approximation_interval.lower_bound.divide_vector(cos_upper, _precision, true);
                            }

                        }
                    }
                    else{
                        this->_approximation_interval.upper_bound = exact_number<T>("1");
                        this->_approximation_interval.lower_bound = exact_number<T>("1");
                        if(cos_upper > cos_lower){
                            this->_approximation_interval.lower_bound.divide_vector(cos_upper, _precision, false);
                            this->_approximation_interval.upper_bound.divide_vector(cos_lower, _precision, true);
                        }
                        else{
                            this->_approximation_interval.lower_bound.divide_vector(cos_lower, _precision, false);
                            this->_approximation_interval.upper_bound.divide_vector(cos_upper, _precision, true);
                        }
                    }

                    break;
                }

                case OPERATION::COSEC :{
                    exact_number<T> sin_lower, cos_lower, sin_upper, cos_upper, sin_lower_tmp, sin_upper_tmp, cos_upper_tmp, cos_lower_tmp;

                    while(true){

                        bool iterate_again;
                        if(ro.get_lhs_itr().get_interval().upper_bound - ro.get_lhs_itr().get_interval().lower_bound >= literals::four_exact<T>){
                            iterate_again = true;
                        }
                        else{
                            std::tie(sin_lower_tmp, cos_lower_tmp) = sin_cos(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, false);
                            std::tie(sin_upper_tmp, cos_upper_tmp) = sin_cos(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, true);
                            /**
                             * Now if difference between lower and upper bounds of interval is less than 4, then there can exist 0,1 or 2 minima/maxima points.
                             * First we will check whether the sign of sin(x) from lower to upper bound is changed or not, if it is, then we have one point 
                             * of maxima/minima. Then we will iterate for better input.
                             **/
                            if(sin_upper_tmp.positive != sin_lower_tmp.positive || sin_upper_tmp == literals::zero_exact<T> || sin_lower_tmp == literals::zero_exact<T>){
                                iterate_again = true;
                            }
                            /**
                             * Now, if sign is not changed, then we will check if sign of mid point of derivative is same as end points.
                             * If it is, then no points of minima/maxima, no need to iterate further.
                             **/
                            else{
                                auto mid = ro.get_lhs_itr().get_interval().lower_bound + ro.get_lhs_itr().get_interval().upper_bound;
                                mid.divide_vector(literals::two_exact<T>, _precision, true);
                                if(sine(mid, _precision, true).positive != sin_lower_tmp.positive){
                                    iterate_again = true;
                                }
                                else{
                                    iterate_again = false;
                                }
                            }
                        }
                        // if we have point of maxima of minima in our input interval
                        if(iterate_again){
                            // updating the boundaries of lhs
                            if(_precision >= ro.get_lhs_itr().maximum_precision()){
                                    throw max_precision_for_trigonometric_function_error();
                            }
                            ro.get_lhs_itr().iterate_n_times(1);
                            ++_precision;
                        }
                        else{
                            sin_lower = sin_lower_tmp;
                            sin_upper = sin_upper_tmp;
                            cos_lower = cos_lower_tmp;
                            cos_upper = cos_upper_tmp;
                            break;
                        }
                    }

                    // derivative of cosec(x) is -cosec(x)cot(x)
                    exact_number<T> derivative_lower = cos_lower;
                    derivative_lower.divide_vector(sin_lower*sin_lower, _precision, false);
                    
                    
                    exact_number<T> derivative_upper = cos_upper;
                    derivative_upper.divide_vector(sin_upper*sin_upper, _precision, true); 
                    // checking for point of minima
                    if(derivative_lower.positive != derivative_upper.positive){
                        // if minima exists and either number is positive, then lower end of resulting interval is 1
                        if(sin_upper.positive){
                            this->_approximation_interval.lower_bound = exact_number<T>("1");
                            this->_approximation_interval.upper_bound = exact_number<T>("1");
                            if(sin_upper > sin_lower){
                                this->_approximation_interval.upper_bound.divide_vector(sin_lower, _precision, true);
                            }
                            else{
                                this->_approximation_interval.upper_bound.divide_vector(sin_upper, _precision, true);
                            }
                        }
                        else{
                            this->_approximation_interval.upper_bound = exact_number<T>("-1");
                            this->_approximation_interval.lower_bound = exact_number<T>("1");
                            if(sin_upper > sin_lower){
                                this->_approximation_interval.lower_bound.divide_vector(sin_lower, _precision, false);
                            }
                            else{
                                this->_approximation_interval.lower_bound.divide_vector(sin_upper, _precision, false);
                            }

                        }
                    }
                    else{
                        this->_approximation_interval.upper_bound = exact_number<T>("1");
                        this->_approximation_interval.lower_bound = exact_number<T>("1");
                        if(sin_upper > sin_lower){
                            this->_approximation_interval.lower_bound.divide_vector(sin_upper, _precision, false);
                            this->_approximation_interval.upper_bound.divide_vector(sin_lower, _precision, true);
                        }
                        else{
                            this->_approximation_interval.lower_bound.divide_vector(sin_lower, _precision, false);
                            this->_approximation_interval.upper_bound.divide_vector(sin_upper, _precision, true);
                        }
                    }

                    break;
                }

                case OPERATION::ASIN :{
                    // If upper bound < -1 or lower bound > 1, then the number is out of domain
                    if (ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) > literals::one_exact<T> 
                        || ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) < literals::minus_one_exact<T>) {
                        throw max_precision_for_inverse_trigonometric_function_error();
                    }

                    // If lower_bound < -1 or upper_bound > 1, we iterate for more precise input
                    while(true) {
                        if (ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) < literals::minus_one_exact<T> 
                            || ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) > literals::one_exact<T>) {
                            if (_precision >= ro.get_lhs_itr().maximum_precision()) {
                                throw max_precision_for_inverse_trigonometric_function_error();
                            }
                            ro.get_lhs_itr().iterate_n_times(1);
                            ++_precision;
                        }
                        else break;
                    }

                    // asin is an increasing function in its domain
                    this->_approximation_interval.lower_bound = 
                        sin_inverse(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, false);
                    this->_approximation_interval.upper_bound = 
                        sin_inverse(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, true);
                    break;
                }

                case OPERATION::ACOS :{
                    // If upper bound < -1 or lower bound > 1, then the number is out of domain
                    if (ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) > literals::one_exact<T> 
                        || ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) < literals::minus_one_exact<T>) {
                        throw max_precision_for_inverse_trigonometric_function_error();
                    }

                    // If lower_bound < -1 or upper_bound > 1, we iterate for more precise input
                    while(true) {
                        if (ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) < literals::minus_one_exact<T> 
                            || ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) > literals::one_exact<T>) {
                            if (_precision >= ro.get_lhs_itr().maximum_precision()) {
                                throw max_precision_for_inverse_trigonometric_function_error();
                            }
                            ro.get_lhs_itr().iterate_n_times(1);
                            ++_precision;
                        }
                        else break;
                    }

                    // acos is a decreasing function in its domain
                    this->_approximation_interval.lower_bound = 
                        cos_inverse(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, false);
                    this->_approximation_interval.upper_bound = 
                        cos_inverse(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, true);
                    break;
                }

                case OPERATION::ATAN :{
                    // atan is an increasing function in its domain
                    this->_approximation_interval.lower_bound = 
                        tan_inverse(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, false);
                    this->_approximation_interval.upper_bound = 
                        tan_inverse(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, true);
                    break;
                }

                case OPERATION::ACOT :{
                    // acot is a decreasing function in its domain
                    this->_approximation_interval.lower_bound = 
                        cot_inverse(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, false);
                    this->_approximation_interval.upper_bound = 
                        cot_inverse(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, true);
                    break;
                }

                case OPERATION::ASEC :{
                    // If -1 < lower_bound < upper_bound < 1, then the number is out of domain
                    if (ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) > literals::minus_one_exact<T> 
                        && ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) < literals::one_exact<T>) {
                        throw max_precision_for_inverse_trigonometric_function_error();
                    }

                    // We iterate for more precise input till lower_bound >= 1 or upper_bound <= -1
                    while(true) {
                        if (!(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) >= literals::one_exact<T> 
                            || ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) <= literals::minus_one_exact<T>)) {
                            if (_precision >= ro.get_lhs_itr().maximum_precision()) {
                                throw max_precision_for_inverse_trigonometric_function_error();
                            }
                            ro.get_lhs_itr().iterate_n_times(1);
                            ++_precision;
                        }
                        else break;
                    }

                    // asec is an increasing function in its domain
                    this->_approximation_interval.lower_bound = 
                        sec_inverse(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, false);
                    this->_approximation_interval.upper_bound = 
                        sec_inverse(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, true);
                    break;
                }

                case OPERATION::ACOSEC :{
                    // If -1 < lower_bound < upper_bound < 1, then the number is out of domain
                    if (ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) > literals::minus_one_exact<T> 
                        && ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) < literals::one_exact<T>) {
                        throw max_precision_for_inverse_trigonometric_function_error();
                    }

                    // We iterate for more precise input till lower_bound >= 1 or upper_bound <= -1
                    while(true) {
                        if (!(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) >= literals::one_exact<T> 
                            || ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) <= literals::minus_one_exact<T>)) {
                            if (_precision >= ro.get_lhs_itr().maximum_precision()) {
                                throw max_precision_for_inverse_trigonometric_function_error();
                            }
                            ro.get_lhs_itr().iterate_n_times(1);
                            ++_precision;
                        }
                        else break;
                    }

                    // acosec is a decreasing function in its domain
                    this->_approximation_interval.lower_bound = 
                        cosec_inverse(ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true), _precision+4, false);
                    this->_approximation_interval.upper_bound = 
                        cosec_inverse(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false), _precision+4, true);
                    break;
                }

                case OPERATION::ATAN2 :{
                    // If 0 lies in both lhs and rhs intervals, we iterate for more precise inputs
                    if (!(ro.get_lhs_itr().get_interval().positive() || ro.get_lhs_itr().get_interval().negative()) 
                        && !(ro.get_rhs_itr().get_interval().positive() || ro.get_rhs_itr().get_interval().negative())) {
                        
                        int flag = 0;
                        while(true) {
                            if (!(ro.get_lhs_itr().get_interval().positive() || ro.get_lhs_itr().get_interval().negative())) {
                                if (_precision >= ro.get_lhs_itr().maximum_precision()) {
                                    flag++;
                                }
                                ro.get_lhs_itr().iterate_n_times(1);
                                ++_precision;
                            }
                            else break;
                        }

                        while(true) {
                            if (!(ro.get_rhs_itr().get_interval().positive() || ro.get_rhs_itr().get_interval().negative())) {
                                if (_precision >= ro.get_rhs_itr().maximum_precision()) {
                                    flag++;
                                }
                                ro.get_rhs_itr().iterate_n_times(1);
                                ++_precision;
                            }
                            else break;
                        }

                        // After going to max precision, if still 0 lies in both lhs and rhs intervals, throw error 
                        if (flag == 2) {
                            throw max_precision_for_inverse_trigonometric_function_error();
                        }
                    }

                    if (ro.get_rhs_itr().get_interval().positive()) {
                        // if x > 0, atan2 is continuous
                        exact_number<T> numerator;
                        exact_number<T> denominator;
                        bool deviation_upper_boundary, deviation_lower_boundary;
                        
                        // Upper Boundary
                        if (!ro.get_lhs_itr().get_interval().negative()) {
                            deviation_upper_boundary = true;
                            numerator = ro.get_lhs_itr().get_interval().upper_bound;
                            denominator = ro.get_rhs_itr().get_interval().lower_bound;
                        } else {
                            deviation_upper_boundary = false;
                            numerator = ro.get_lhs_itr().get_interval().upper_bound;
                            denominator = ro.get_rhs_itr().get_interval().upper_bound;
                        }
                        this->_approximation_interval.upper_bound = tan2_inverse(numerator, denominator, _precision+4, deviation_upper_boundary);

                        // Lower Boundary
                        if (ro.get_lhs_itr().get_interval().positive()) {
                            deviation_lower_boundary = false;
                            numerator = ro.get_lhs_itr().get_interval().lower_bound;
                            denominator = ro.get_rhs_itr().get_interval().upper_bound; 
                        } else {
                            deviation_lower_boundary = true;
                            numerator = ro.get_lhs_itr().get_interval().lower_bound;
                            denominator = ro.get_rhs_itr().get_interval().lower_bound;
                        } 
                        this->_approximation_interval.lower_bound = tan2_inverse(numerator, denominator, _precision+4, deviation_lower_boundary);
                    }
                    else {
                        // x < 0, atan2 is dicontinuous along negative x-axis

                        // We iterate for more precise lhs until lhs >= 0 or lhs < 0 
                        while(true) {
                            if (!(ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) >= literals::zero_exact<T>
                                || ro.get_lhs_itr().get_interval().upper_bound.up_to(_precision, true) < literals::zero_exact<T>)) {
                                if (_precision >= ro.get_lhs_itr().maximum_precision()) {
                                    throw max_precision_for_inverse_trigonometric_function_error();
                                }
                                ro.get_lhs_itr().iterate_n_times(1);
                                ++_precision;
                            }
                            else break;
                        }

                        exact_number<T> numerator;
                        exact_number<T> denominator;
                        bool deviation_upper_boundary, deviation_lower_boundary;

                        if (ro.get_lhs_itr().get_interval().lower_bound.up_to(_precision, false) >= literals::zero_exact<T>) {
                            // if lhs >= 0

                            // Upper Boundary
                            deviation_upper_boundary = false;
                            numerator = ro.get_lhs_itr().get_interval().lower_bound;
                            denominator = ro.get_rhs_itr().get_interval().lower_bound;
                            this->_approximation_interval.upper_bound = tan2_inverse(numerator, denominator, _precision+4, deviation_upper_boundary);

                            // Lower Boundary
                            if (ro.get_rhs_itr().get_interval().negative()) {
                                deviation_lower_boundary = true;
                                numerator = ro.get_lhs_itr().get_interval().upper_bound;
                                denominator = ro.get_rhs_itr().get_interval().upper_bound;
                                this->_approximation_interval.lower_bound = tan2_inverse(numerator, denominator, _precision+4, deviation_lower_boundary);
                            } else {
                                deviation_lower_boundary = true;
                                numerator = ro.get_lhs_itr().get_interval().lower_bound;
                                denominator = ro.get_rhs_itr().get_interval().upper_bound;
                                this->_approximation_interval.lower_bound = tan2_inverse(numerator, denominator, _precision+4, deviation_lower_boundary);
                            }
                        } 
                        else {
                            // if lhs < 0

                            // Upper Boundary
                            if (ro.get_rhs_itr().get_interval().negative()) {
                                deviation_upper_boundary = true;
                                numerator = ro.get_lhs_itr().get_interval().lower_bound;
                                denominator = ro.get_rhs_itr().get_interval().upper_bound;
                                this->_approximation_interval.upper_bound = tan2_inverse(numerator, denominator, _precision+4, deviation_upper_boundary);
                            } else {
                                deviation_upper_boundary = true;
                                numerator = ro.get_lhs_itr().get_interval().upper_bound;
                                denominator = ro.get_rhs_itr().get_interval().upper_bound;
                                this->_approximation_interval.upper_bound = tan2_inverse(numerator, denominator, _precision+4, deviation_upper_boundary);
                            }

                            // Lower Boundary
                            deviation_lower_boundary = false;
                            numerator = ro.get_lhs_itr().get_interval().upper_bound;
                            denominator = ro.get_rhs_itr().get_interval().lower_bound;
                            this->_approximation_interval.lower_bound = tan2_inverse(numerator, denominator, _precision+4, deviation_lower_boundary);
                        }
                    }

                    break;
                }

                default:
                    throw boost::real::none_operation_exception();
            }
        }

        template <typename T>
        inline void const_precision_iterator<T>::operation_iterate_n_times(real_operation<T> &ro, int n) {
            /// @warning there could be issues if operands have different precisions/max precisions

            if (ro.get_lhs_itr()._precision < this->_precision + n) {
                ro.get_lhs_itr().iterate_n_times(n);
            }
            
            if (ro.get_rhs_itr()._precision < this->_precision + n) {
                ro.get_rhs_itr().iterate_n_times(n);
            }

            this->_precision += n;

            update_operation_boundaries(ro);
        }

        template <typename T>
        inline void const_precision_iterator<T>::operation_iterate(real_operation<T> &ro) {
            // only iterate if we must. If operand precision < this precision, then it must have
            // hit its maximum_precision. If operand precision == this precision, we try iterating. Otherwise,
            // it is == this->_precision + 1 (from being iterated elsewhere in the operation tree) and
            // we do not iterate again.

            if (ro.get_lhs_itr()._precision == this->_precision)
                ++(ro.get_lhs_itr());
            
            if (ro.get_rhs_itr()._precision == this->_precision)
                ++(ro.get_rhs_itr());

            (this->_precision)++;

            update_operation_boundaries(ro);
        }

        /* real_operation member functions */

        // note that we return a reference. It is necessary, for now, since iterating operands 
        // (see operation_iterate, above) REQUIRES modifying the operands' precision iterators
        template <typename T>
        inline const_precision_iterator<T>& real_operation<T>::get_lhs_itr() {
            return _lhs->get_precision_itr();
        }

        template <typename T>
        inline const_precision_iterator<T>& real_operation<T>::get_rhs_itr() {
            return _rhs->get_precision_itr();
        }
    }
}

#endif // BOOST_REAL_REAL_DATA_HPP 
