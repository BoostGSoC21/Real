#ifndef BOOST_REAL_REAL_EXCEPTION_HPP
#define BOOST_REAL_REAL_EXCEPTION_HPP

#include <exception>

namespace boost {
    namespace real {
        struct precision_exception : public std::exception {
            const char * what () const throw () override {
                return "The boost::real number precision is too slow to compare both numbers";
            }
        };

        struct none_operation_exception : public std::exception {
            const char * what () const throw () override {
                return "The boost::real number operation cannot be NONE";
            }
        };

        struct invalid_representation_exception : public std::exception {
            const char * what () const throw () override {
                return "The boost::real number method cannot be called for real number for the current representation";
            }
        };

        struct invalid_string_number_exception : public std::exception {
            const char * what () const throw () override {
                return "The string passed to construct the boost::real number is invalid";
            }
        };

        struct bad_variant_access_exception: public std::exception {
            const char * what () const throw () override {
                return "Cannot perform this method on this real variant type";
            }
        };

        struct divide_by_zero : public std::exception {
            const char * what () const throw () override {
                return "Divison by zero is undefined";
            }
        };

        struct divergent_division_result_exception : public std::exception {
            const char * what () const throw () override {
                return "The divisor approximation interval contains 0, so the quotient is unbounded";
            }
        };

        struct pi_precision_exception : public std::exception {
            const char * what () const throw () override {
                return "pi is currently undefined for precision > 2000 digits";
            }
        };

        struct invalid_distribution_operation_exception : public std::exception {

            const char * what () const throw () override {
                return "Distribution does not work for this operation.";
            }
        };

        struct exponent_overflow_exception : public std::exception {

            const char * what () const throw () override {
                return "The current precision exceeds the bounds of the exponent type";
            }
        };

        struct octal_input_not_supported_exception : public std::exception {

            const char * what () const throw () override {
                return "The string input began with 0 - octal input is not supported at this time";
                        
            }
        };

        struct invalid_recursion_level_exception : public std::exception {

            const char * what () const throw () override {
                return "Distributing with this recursion level is undefined.";
                        
            }
        };

        struct integer_contructor_for_non_integer_type : public std::exception {
            const char * what () const throw () override {
                return "Constructor of Integer type number is called for a non Integer type number";
            }
        };
      
        struct non_integral_exponent_exception : public std::exception {
            const char * what () const throw () override {
                return "Non integral powers not supported";
            }
        };
        struct expected_real_integer_type_number : public std::exception {
            const char * what() const throw () override {
                return "Expected a integer type number but got a non-integer type number";
            }
        };

        struct expected_real_rational_type_number : public std::exception {
            const char * what() const throw () override {
                return "Expected a rational type number but got a non-rational type number";
            }
        };

        struct constructin_real_algorithm_or_real_operation_using_string : public std::exception {
            const char * what() const throw () override {
                return "Can not construct a real_algorithm or real_operation type from a string";
            }
        };
       
        struct logarithm_not_defined_for_non_positive_number : public std::exception {
            const char * what() const throw () override {
                return "Logarithm is only defined for positive number or you are trying to find non-integral power of negative number(Complex Numbers not supported)";
            }
        };

        struct max_precision_for_trigonometric_function_error : public std::exception {
            const char * what() const throw () override {
                return "Number is not in domain of this trigonometric function";
            }
        };

        struct max_precision_for_inverse_trigonometric_function_error : public std::exception {
            const char * what() const throw () override {
                return "Number is not in domain of this inverse trigonometric function";
            }
        };

        struct negative_integers_not_supported : public std::exception {
            const char * what() const throw () override {
                return "Integer Power function only supports positive integers";
            }
        };

        struct non_integral_power_of_negative_number : public std::exception {
            const char * what() const throw () override {
                return "Non-integral power of a negative number is a complex number";
            }
        };

        struct sqrt_not_defined_for_negative_number : public std::exception {
            const char * what() const throw () override {
                return "Square root function is not defined for negative numbers";
            }
        };
        

    }
}

#endif //BOOST_REAL_REAL_EXCEPTION_HPP
