#ifndef BOOST_REAL_HPP
#define BOOST_REAL_HPP

#include <iostream>
#include <optional>
#include <vector>
#include <regex>
#include <initializer_list>
#include <sstream>
#include <utility>
#include <memory> // shared_ptr
#include <variant>
#include <map>
#include <string>

#include <real/real_exception.hpp>
#include <real/real_explicit.hpp>
#include <real/real_algorithm.hpp>
#include <real/real_operation.hpp>
#include <real/const_precision_iterator.hpp>
#include <real/real_data.hpp>
#include <real/real_optimization_data.hpp>

namespace boost {
    namespace real {

        /**
         * Enumerator class for type of real number
         * @brief: enumerator class which will store store types of number for constructor, for 
         * to avoid errors in giving types using string.
         * @author: Vikram Singh Chundawat.
         **/
        enum class TYPE{EXPLICIT, INTEGER, RATIONAL, ALGORITHM, OPERATION};

        /**
         * @author Laouen Mayal Louan Belloli
         *
         * @brief boost::real::real is a C++ class that represent real numbers as abstract entities that
         * can be dynamically approximated as much as needed (until a set maximum precision) to be
         * able to operate with them. Numbers can be added, subtracted, multiplied and compared by
         * lower than and equality.
         *
         * @details A boost::real::real number is represented by the operations from which the number
         * is created, the entire operation is represented as a binary tree where the leaves are
         * literal numbers and the internal nodes are the operations. Also, boost::real::real allow to
         * represent irrational numbers by taking as parameter a function pointer or lambda function
         * that given un unsigned integer "n", the function returns the n-th digit of the irrational
         * number.
         *
         * A number can be one of the following three kind:
         *
         *  1. Explicit number: A number is a vector of digits sorted as in the number natural
         *  representation. To determine where the integer part ends and the fractional part starts,
         *  an integer is used as the exponent of a floating point number and determines where the
         *  integer part start and the fractional ends. Also a boolean is used to set the number as
         *  positive (True) or negative (False)
         *
         *  2. Algorithmic number: This representation is equal to the Explicit number but instead
         *  of using a vector of digits, a lambda function must be provided. The lambda function
         *  takes an unsigned integer "n" as parameter and returns the n-th digit of the number.
         *
         *  3. A number is a composition of two numbers related by an operator (+, -, *), the number
         *  creates pointers to the operands and each time the number is used, the operation is
         *  evaluated to return the result.
         *
         * Two boost::real::real numbers can be compared by the lower operator "<" and by the equal
         * operator "==" but for those cases where the class is not able to decide the value of the
         * result before reaching the maximum precision, a precision_exception is thrown.
         */
        /// @TODO: replace T with something more descriptive 
        template <typename T = int>
        class real {
        private:
            std::shared_ptr<real_data<T>> _real_p;
            optimization_data<T> optimize_vars;

            // ctor from shared_ptr to (already init) real_data. used in check_and_distribute.
            real(std::shared_ptr<real_data<T>> x) : _real_p(x){ 
                this->set_vars_and_check_if_optimize(this->find_num_vars());
            };

        public:
            /// @TODO: Move constructors to move directly from the ctors in real_explicit to the values in real_data
            /// @TODO: do we need different ctors to be more efficient? rvalue AND lvalue ref?

            /**
             * @brief *Default constructor:* Construct a boost::real::real with undefined representation
             * and behaviour.
             *
             * @note This constructor exists to allow working with other libraries as std::map or std::tuple
             */
            real() = default;

            /**
             * @brief *Copy constructor:* Creates a copy of the boost::real::real number other
             *
             * @param other - the boost::real::real instance to copy.
             */
            real(const real<T>& other)  : _real_p(other._real_p), optimize_vars(other.optimize_vars) {};


            /**
             * @brief String constructor. Returns an exact number if possible to be represented in internal base. Else division number is returned.
             *
             * @param number - a valid string representing a number.
             *
             * @throws boost::real::invalid_string_number exception if string doesn't represent a valid number
             */
            real(const std::string& number, std::string type = "explicit") {
                if(type=="explicit"){
                    auto [integer_part, decimal_part, exponent, positive] = exact_number<>::number_from_string(number);

                    if ((int)(decimal_part.length() + integer_part.length()) <= exponent) {
                        this->_real_p = std::make_shared<real_data<T>>(real_explicit<T>(integer_part, decimal_part, exponent, positive));
                        this->set_vars_and_check_if_optimize(1);
                    } else {
                        int zeroes = decimal_part.length() + integer_part.length() - exponent;
                        std::string denominator = "1";
                        for (int i = 0; i<zeroes; ++i)
                            denominator = denominator + "0";

                        // source of inefficiency. copying, casting.
                        std::string numerator = (std::string) std::string(integer_part).c_str() + (std::string) std::string(decimal_part);
                        if (!positive)
                            numerator = "-" + numerator;
                        std::shared_ptr<real_data<T>> lhs = std::make_shared<real_data<T>>(real_explicit<T>(numerator));
                        std::shared_ptr<real_data<T>> rhs = std::make_shared<real_data<T>>(real_explicit<T>(denominator));
        
                        this->_real_p  = std::make_shared<real_data<T>>(real_operation(lhs, rhs, OPERATION::DIVISION));
                        this->set_vars_and_check_if_optimize(2);
                    }
                }
                if(type=="integer"){
                    integer_number<T> a(number);
                    integer_number<T> b("1");
                    this->_real_p = std::make_shared<real_data<T>>(real_rational<T>(a,b));
                    this->set_vars_and_check_if_optimize(1);
                }
                if(type=="rational"){
                    this->_real_p = std::make_shared<real_data<T>>(real_rational<T>(number));
                    this->set_vars_and_check_if_optimize(1);
                }
            }




            real(const std::string& number, TYPE type) {
                switch(type){
                    case TYPE::EXPLICIT: {
                        auto [integer_part, decimal_part, exponent, positive] = exact_number<>::number_from_string(number);

                        if ((int)(decimal_part.length() + integer_part.length()) <= exponent) {
                            this->_real_p = std::make_shared<real_data<T>>(real_explicit<T>(integer_part, decimal_part, exponent, positive));
                            this->set_vars_and_check_if_optimize(1);
                        } else {
                            int zeroes = decimal_part.length() + integer_part.length() - exponent;
                            std::string denominator = "1";
                            for (int i = 0; i<zeroes; ++i)
                                denominator = denominator + "0";

                            // source of inefficiency. copying, casting.
                            std::string numerator = (std::string) std::string(integer_part).c_str() + (std::string) std::string(decimal_part);
                            if (!positive)
                                numerator = "-" + numerator;
                            std::shared_ptr<real_data<T>> lhs = std::make_shared<real_data<T>>(real_explicit<T>(numerator));
                            std::shared_ptr<real_data<T>> rhs = std::make_shared<real_data<T>>(real_explicit<T>(denominator));
            
                            this->_real_p  = std::make_shared<real_data<T>>(real_operation(lhs, rhs, OPERATION::DIVISION));
                            this->set_vars_and_check_if_optimize(2);
                        }
                        break;
                    }
                
                    case TYPE::INTEGER:{
                        integer_number<T> a(number);
                        integer_number<T> b("1");
                        this->_real_p = std::make_shared<real_data<T>>(real_rational<T>(a,b));
                        this->set_vars_and_check_if_optimize(1);
                        break;
                    }
                    case TYPE::RATIONAL:{
                        this->_real_p = std::make_shared<real_data<T>>(real_rational<T>(number));
                        this->set_vars_and_check_if_optimize(1);
                        break;
                    }
                    default:
                        throw constructin_real_algorithm_or_real_operation_using_string();
            }
        }


            /**
             * @brief Initializer list constructor
             *
             * @param digits - a initializer_list<T> that represents the number digits.
             */
            real(std::initializer_list<T> digits)
                    : _real_p(std::make_shared<real_data<T>>(real_explicit<T>(digits, digits.size())))
                { this->set_vars_and_check_if_optimize(1); };

            /**
             * @brief *Signed initializer list constructor:* Creates a boost::real::real
             * instance that represents the number where the positive parameter is used to set the
             * number sign and the elements of the digits parameter list are the number digits in
             * the same order.
             *
             * @param digits - an initializer_list<T> that represent the number digits.
             * @param positive - a bool that represents the number sign. If positive is set to true,
             * the number is positive, otherwise is negative.
             */
            real(std::initializer_list<T> digits, bool positive)
                    : _real_p(std::make_shared<real_data<T>>(real_explicit<T>(digits, digits.size(), positive)))
                    { this->set_vars_and_check_if_optimize(1); };

            /**
             * @brief *Initializer list constructor with exponent:* Creates a boost::real::real
             * instance that represents the number where the exponent is used to set the number
             * integer part and the elements of the digits list are the digits the number in the same order.
             * The number is set as positive.
             *
             * @param digits - an initializer_list<T> that represent the number digits.
             * @param exponent - an integer representing the number exponent.
             */
            real(std::initializer_list<T> digits, int exponent)
                    : _real_p(std::make_shared<real_data<T>>(real_explicit<T>(digits, exponent)))
                    { this->set_vars_and_check_if_optimize(1); };

            /**
             * @brief *Initializer list constructor with exponent and sign:* Creates a boost::real::real instance
             * that represents the number where the exponent is used to set the number integer part
             * and the elements of the digit list are the digits the number in the same order.
             * This constructor uses the sign to determine if the number is positive or negative.
             *
             * @param digits - an initializer_list<T> that represent the number digits.
             * @param exponent - an integer representing the number exponent.
             * @param positive - a bool that represent the number sign. If positive is set to true,
             * the number is positive, otherwise is negative.
             */
            real(std::initializer_list<T> digits, int exponent, bool positive)
                    : _real_p(std::make_shared<real_data<T>>(real_explicit<T>(digits, exponent, positive))) 
                    { this->set_vars_and_check_if_optimize(1); };

            /**
             * @brief *Lambda function constructor with exponent:* Creates a boost::real::real
             * instance that represents the number where the exponent is used to set the number
             * integer part and the lambda function digits is used to know the number digits,
             * this function returns the n-th number digit.
             *
             * @param get_nth_digit - a function pointer or lambda function that given an unsigned
             * int "n" as parameter, it returns the number n-th digit.
             * @param exponent - an integer representing the number exponent.
             */
            real(T (*get_nth_digit)(unsigned int), int exponent)
                    : _real_p(std::make_shared<real_data<T>>(real_algorithm<T>(get_nth_digit, exponent)))
                    { this->set_vars_and_check_if_optimize(1); };

            /**
             * @brief *Lambda function constructor with exponent and sign:* Creates a boost::real::real instance
             * that represents the number where the exponent is used to set the number integer part
             * and the lambda function digits is used to know the number digit, this function returns
             * the n-th number digit. This constructor uses the sign to determine if the number
             * is positive or negative.
             *
             * @param get_nth_digit - a function pointer or lambda function that given an unsigned
             * int "n" as parameter, it returns the number n-th digit.
             * @param exponent - an integer representing the number exponent.
             * @param positive - a bool that represent the number sign. If positive is set to true,
             * the number is positive, otherwise is negative.
             */
            real(T (*get_nth_digit)(unsigned int), int exponent, bool positive) 
                 : _real_p(::std::make_shared<real_data<T>>(real_algorithm<T>(get_nth_digit, exponent, positive))) 
                 { this->set_vars_and_check_if_optimize(1); };

            // ctors from the 3 underlying types
            real(real_explicit<T> x) : _real_p(std::make_shared<real_data<T>>(x)) { this->set_vars_and_check_if_optimize(1); };
            real(real_algorithm<T> x) : _real_p(std::make_shared<real_data<T>>(x)) { this->set_vars_and_check_if_optimize(1); };
            real(real_operation<T> x) : _real_p(std::make_shared<real_data<T>>(x)) { 
                this->set_vars_and_check_if_optimize(this->find_num_vars()); 
            };

            /**
             * @brief Default destructor
             */
            ~real() = default;

            const real_number<T>& get_real_number() {
                return _real_p->get_real_number();
            }

            const_precision_iterator<T> get_real_itr() const {
                return _real_p->get_precision_itr();
            }

            /**
             * @brief Returns the maximum allowed precision, if that precision is reached and an
             * operator need more precision, a precision_exception should be thrown.
             *
             * @return and integer with the maximum allowed precision.
             */
            unsigned int maximum_precision() const {
                return get_real_itr().maximum_precision();
            }

            /// set max precision for the underlying iterator
            void set_maximum_precision(unsigned int maximum_precision) {
                this->_real_p->get_precision_itr().set_maximum_precision(maximum_precision);
            }

            /************** Operators ******************/
            
            /**
             * @brief If the number is an explicit or algorithm number it returns the n-th digit
             * of the represented number. If the number is an operation it throws an invalid_representation_exception
             *
             * @param n - an unsigned int number indicating the index of the requested digit.
             * @return an integer with the value of the number n-th digit.
             *
             * @throws boost::real::invalid_representation_exception
             */
            T operator[](unsigned int n) const {
                T ret; 

                std::visit( overloaded { // perform operation on whatever is held in variant
                    [&n, &ret] (const real_explicit<T>& real)  { 
                        ret = real[n];
                    },
                    [&n, &ret] (const real_algorithm<T>& real) {
                        ret = real[n];
                    },
                    [] (const real_operation<T>& real) {
                        throw boost::real::bad_variant_access_exception();
                    },
                    [] (auto& real) {
                        throw boost::real::bad_variant_access_exception();
                    }
                }, _real_p->get_real_number());
                return ret;
            }


            // a constant used in the print tree helper function
            static const int PRINT_SPACE = 5;

            void print_tree_traversal(std::shared_ptr<real_data<T>> real_p, int space) {
                std::visit(overloaded {
                    [space] (const real_explicit<T>& real)  {
                        for (int i = PRINT_SPACE; i < space; i++)
                            std::cout << ' ';
                        std::cout << real.get_exact_number().as_string() << '\n';
                    },
                    [space] (const real_algorithm<T>& real) {
                        for (int i = PRINT_SPACE; i < space; i++)
                            std::cout << ' ';
                        std::cout << "alg\n";
                    },
                    [&space, this] (const real_operation<T>& real) {
                        print_tree_traversal(real.rhs(), space + PRINT_SPACE);
                        std::cout << '\n';

                        for (int i = PRINT_SPACE; i < space; i++)
                            std::cout << ' ';

                        switch(real.get_operation()) {
                            case OPERATION::ADDITION:
                                std::cout << "+";
                                break;
                            case OPERATION::SUBTRACTION:
                                std::cout << "-";
                                break;
                            case OPERATION::MULTIPLICATION:
                                std::cout << "*";
                                break;
                            case OPERATION::DIVISION:
                                std::cout << "/";
                                break;
                        }
                        std::cout << '\n';

                        print_tree_traversal(real.lhs(), space + PRINT_SPACE);
                    },
                    [] (auto& real) {
                        throw boost::real::bad_variant_access_exception();
                    }
                }, real_p->get_real_number());
            }

            // a helper function for observing the trees within a real
            // note the tree is displayed with a left 90 degree rotation
            // (i.e., root on the left, nodes on the next leftmost level)
            void print_tree(int space = PRINT_SPACE) {
                print_tree_traversal(this->_real_p, space);
            }

            // this is used to control the amount of recursion that goes on in the distribution. Essentially, when we do a + b, we may 
            // look at one level below them (if applicable, i.e., they're operations pointing to operands of their own). 

            // we at most need to look at two levels.
            // this is because in the case we have, say, x*a+x*b -> (a+b)*x, we may want to look at a and b's operands, if applicable, to see if
            // they may be distributed as well. 
            enum class RECURSION_LEVEL{ZERO, ONE, TWO};

            // a helper function for distributing when performing addition/subtraction 
            // the returned bool tells us whether we distributed or not, which is mostly useful when assign_and_return_void is true
            // since std::optional<real> would = std::null_opt if assign_and_return_void is false.
            std::pair<bool, std::optional<real>> check_and_distribute(const real & other, const bool assign_and_return_void, const OPERATION op, const RECURSION_LEVEL rc_lvl) {
                // The following simplifies using the distributive property, when numbers have the same pointers.
                // We could do comparison by value, but this may force more computation than is necessary for the user,
                // since it's difficult to determine whether values are the same

                std::shared_ptr<real_data<T>> a;
                std::shared_ptr<real_data<T>> b;
                std::shared_ptr<real_data<T>> x;

                static real<T> one ("1");

                if(auto op_ptr = std::get_if<real_operation<T>>(this->_real_p->get_real_ptr())) { // lhs real_operation
                    if (auto op_ptr2 = std::get_if<real_operation<T>>(other._real_p->get_real_ptr())) { // lhs, rhs real_operation
                        if ((op_ptr->get_operation() == OPERATION::MULTIPLICATION) && op_ptr2->get_operation() == OPERATION::MULTIPLICATION) {
                            if (op_ptr->lhs() == op_ptr2->lhs()) { // x * a + x * b = (a + b) * x
                                a = op_ptr->rhs();
                                b = op_ptr2->rhs();
                                x = op_ptr->lhs();

                            } else if (op_ptr->lhs() == op_ptr2->rhs()) { // x * a + b * x = (a + b) * x
                                a = op_ptr->rhs();
                                b = op_ptr2->lhs();
                                x = op_ptr->lhs();

                            } else if (op_ptr->rhs() == op_ptr2->lhs()) { // a * x + x * b = (a + b) * x
                                a = op_ptr->lhs();
                                b = op_ptr2->rhs();
                                x = op_ptr->rhs();

                            } else if (op_ptr->rhs() == op_ptr2->rhs()) { // a * x + b * x = (a + b) * x
                                a = op_ptr->lhs();
                                b = op_ptr2->lhs();
                                x = op_ptr->rhs();
                            } else {
                                return std::make_pair(false, std::nullopt);
                            }

                            real<T> a_op_b;

                            if(op == OPERATION::ADDITION) {
                                switch(rc_lvl) {
                                    case (RECURSION_LEVEL::TWO):
                                        a_op_b = real(a).add(real(b), RECURSION_LEVEL::ONE);
                                        break;
                                    case (RECURSION_LEVEL::ONE):
                                        a_op_b = real(a).add(real(b), RECURSION_LEVEL::ZERO);
                                        break;
                                    default:
                                        throw invalid_recursion_level_exception();
                                }
                            } else if (op == OPERATION::SUBTRACTION) {
                                switch(rc_lvl) {
                                    case (RECURSION_LEVEL::TWO):
                                        a_op_b = real(a).subtract(real(b), RECURSION_LEVEL::ONE);
                                        break;
                                    case (RECURSION_LEVEL::ONE):
                                        a_op_b = real(a).subtract(real(b), RECURSION_LEVEL::ZERO);
                                        break;
                                    default:
                                        throw invalid_recursion_level_exception();
                                }
                            } else {
                                throw invalid_distribution_operation_exception();
                            }

                            if(assign_and_return_void) {
                                this->_real_p = std::make_shared<real_data<T>>(real_operation<T>(a_op_b._real_p, x, OPERATION::MULTIPLICATION));
                                return std::make_pair(true, std::nullopt);
                            } else {
                                return std::make_pair(true, real(real_operation<T>(a_op_b._real_p, x, OPERATION::MULTIPLICATION)));
                            }
                        }
                    } else { // lhs is an operation, but rhs is not an operation
                        if (op_ptr->get_operation() == OPERATION::MULTIPLICATION) { 
                            if (other._real_p == op_ptr->lhs()) { // (a * x) + a -> (x + 1) * a
                                a = op_ptr->lhs();
                                x = op_ptr->rhs();

                            } else if (other._real_p == op_ptr->rhs()) {// (x * a) + a -> (x + 1) * a
                                a = op_ptr->rhs();
                                x = op_ptr->lhs();
                            } else {
                                return std::make_pair(false, std::nullopt);
                            }

                            real<T> x_op_1;

                            if(op == OPERATION::ADDITION) {
                                switch(rc_lvl) {
                                    case (RECURSION_LEVEL::TWO):
                                        x_op_1 = real(x).add(one, RECURSION_LEVEL::ONE);
                                        break;
                                    case (RECURSION_LEVEL::ONE):
                                        x_op_1 = real(x).add(one, RECURSION_LEVEL::ZERO);
                                        break;
                                    default:
                                        throw invalid_recursion_level_exception();
                                }
                            }
                            else if (op == OPERATION::SUBTRACTION) {
                                switch(rc_lvl) {
                                    case (RECURSION_LEVEL::TWO):
                                        x_op_1 = real(x).subtract(one, RECURSION_LEVEL::ONE);
                                        break;
                                    case (RECURSION_LEVEL::ONE):
                                        x_op_1 = real(x).subtract(one, RECURSION_LEVEL::ZERO);
                                        break;
                                    default:
                                        throw invalid_recursion_level_exception();
                                }
                            } else {
                                throw invalid_distribution_operation_exception();
                            }

                            if(assign_and_return_void) {
                                this->_real_p = std::make_shared<real_data<T>>(real_operation<T>(x_op_1._real_p, a, OPERATION::MULTIPLICATION));
                                return std::make_pair(true, std::nullopt);
                            } else {
                                return std::make_pair(true, real(real_operation<T>(x_op_1._real_p, a, OPERATION::MULTIPLICATION)));
                            }
                        } 
                    }
                } else if(auto op_ptr = std::get_if<real_operation<T>>(&other._real_p->get_real_number())) { // lhs is not an operation, but rhs is
                    if (op_ptr->get_operation() == OPERATION::MULTIPLICATION) {
                        if (this->_real_p == op_ptr->lhs()) { // a + (a * x) -> (x + 1) * a
                            a = this->_real_p;
                            x = op_ptr->rhs();

                        } else if (this->_real_p == op_ptr->rhs()) {// a + (x * a) -> (x + 1) * a
                            a = this->_real_p;
                            x = op_ptr->lhs();
                        } else {
                            return std::make_pair(false, std::nullopt);
                        }

                        real<T> x_op_1;

                        if(op == OPERATION::ADDITION) {
                            switch(rc_lvl) {
                                case (RECURSION_LEVEL::TWO):
                                    x_op_1 = real(x).add(one, RECURSION_LEVEL::ONE);
                                    break;
                                case (RECURSION_LEVEL::ONE):
                                    x_op_1 = real(x).add(one, RECURSION_LEVEL::ZERO);
                                    break;
                                default:
                                    throw invalid_recursion_level_exception();
                            }
                        }
                        else if (op == OPERATION::SUBTRACTION) {
                            switch(rc_lvl) {
                                case (RECURSION_LEVEL::TWO):
                                    x_op_1 = one.subtract(real(x), RECURSION_LEVEL::ONE);
                                    break;
                                case (RECURSION_LEVEL::ONE):
                                    x_op_1 = one.subtract(real(x), RECURSION_LEVEL::ZERO);
                                    break;
                                default:
                                    throw invalid_recursion_level_exception();
                            }
                        } else {
                            throw invalid_distribution_operation_exception();
                        }

                        if(assign_and_return_void) {
                            this->_real_p = std::make_shared<real_data<T>>(real_operation(x_op_1._real_p, a, OPERATION::MULTIPLICATION));
                            return std::make_pair(true, std::nullopt);
                        } else {
                            return std::make_pair(true, real(real_operation(x_op_1._real_p, a, OPERATION::MULTIPLICATION)));
                        }
                    }
                } else { // neither is an operation
                    if ((this->_real_p == other._real_p) && (op == OPERATION::ADDITION)) { // a + a = 2 * a
                        static std::shared_ptr<real_data<T>> two = std::make_shared<real_data<T>>(real_explicit<T>("2"));

                        if(assign_and_return_void) {
                            this->_real_p = std::make_shared<real_data<T>>(real_operation(two, this->_real_p, OPERATION::MULTIPLICATION));
                            return std::make_pair(true, std::nullopt);
                        } else {
                            return std::make_pair(true, real(real_operation(two, this->_real_p, OPERATION::MULTIPLICATION)));
                        }
                    } 
                } 

                // at this point, we cannot distribute.
                return std::make_pair(false, std::nullopt);
            }
            

            // helper function used in check_and_distribute to limit recursion
            real recurse_op(real& other, RECURSION_LEVEL rc_lvl, OPERATION op) {
                switch (rc_lvl) {
                    case RECURSION_LEVEL::ONE: {
                        auto [is_simplified, result] = check_and_distribute(other, false, op, RECURSION_LEVEL::ONE);

                        if (!is_simplified) {
                            real ret = (*this);
                            ret._real_p = 
                                std::make_shared<real_data<T>>(real_operation(this->_real_p, other._real_p, op));
                            return ret;
                        } else {
                            return result.value();
                        }
                        return *this;
                        break;
                    }
                    case RECURSION_LEVEL::ZERO: {
                        real ret = (*this);
                        ret._real_p = 
                            std::make_shared<real_data<T>>(real_operation(this->_real_p, other._real_p, op));
                        return ret;
                        break;
                    }
                    default:
                        throw invalid_recursion_level_exception();
                }
            }

            /// obtains the result of *this += other, and returns it.
            real add(real other, RECURSION_LEVEL rc_lvl) {
                return recurse_op(other, rc_lvl, OPERATION::ADDITION);
            }

            /// performs *this -= other, and returns *this
            real subtract(real other, RECURSION_LEVEL rc_lvl) {
                return recurse_op(other, rc_lvl, OPERATION::SUBTRACTION);
            }
            
            // helper function used to simplify the subtree involving additions and subtractions
            // count stores the Real number and its number of occurences in subtree
            std::shared_ptr<real_data<T>> simplify(std::map<std::shared_ptr<real_data<T>>, int> count) {
                
                // shared pointer to the simplified tree
                std::shared_ptr<real_data<T>> ret;

                // checks whether we found first node for our simplified tree
                bool found_first_node = false;
                // checks whether we found a node having positive number of occurences
                bool found_positive_node = false;

                // until we don't find the positive node, all nodes which are negative are considered as having postive count
                // when we find the first positive node, we subtract the current tree from this positive node 

                for (auto x: count) {  
                    // if occurence is 0, we add nothing to our tree
                    if (x.second == 0) continue;

                    if (!found_first_node) {
                        if (x.second > 0) {
                            if (x.second == 1) {
                                ret = x.first;
                            }
                            else {
                                std::shared_ptr<real_data<T>> times = std::make_shared<real_data<T>>(real_explicit<T>(std::to_string(x.second)));
                                std::shared_ptr<real_data<T>> num = x.first;
                                ret = std::make_shared<real_data<T>>(real_operation(times, num, OPERATION::MULTIPLICATION));
                            }
                            found_positive_node = true;
                        }
                        else {
                            if (x.second == -1) {
                                ret = x.first;
                            }
                            else {
                                // number of occurences taken as positive
                                std::shared_ptr<real_data<T>> times = std::make_shared<real_data<T>>(real_explicit<T>(std::to_string(-x.second)));
                                std::shared_ptr<real_data<T>> num = x.first;
                                ret = std::make_shared<real_data<T>>(real_operation(times, num, OPERATION::MULTIPLICATION));
                            }
                        }
                        found_first_node = true;
                    }
                    else {
                        // after first node, we have to join new multiplication nodes by +/-
                        if (!found_positive_node)
                        {
                            if(x.second > 0) {
                                std::shared_ptr<real_data<T>> mul_node;
                                if (x.second == 1) {
                                    mul_node = x.first;
                                }
                                else {
                                    std::shared_ptr<real_data<T>> times = std::make_shared<real_data<T>>(real_explicit<T>(std::to_string(x.second)));
                                    std::shared_ptr<real_data<T>> num = x.first;
                                    mul_node = std::make_shared<real_data<T>>(real_operation(times, num, OPERATION::MULTIPLICATION));
                                }
                                // we found our positive node, and we subtract our current constructed tree from this positive node
                                ret = std::make_shared<real_data<T>>(real_operation(mul_node, ret, OPERATION::SUBTRACTION));

                                found_positive_node = true;
                            }
                            else
                            {
                                // if no. of times it occurs is positive, we join with + operation
                                std::shared_ptr<real_data<T>> mul_node;
                                if (x.second == -1) {
                                    mul_node = x.first;
                                }
                                else {
                                    // number of occurences taken as positive
                                    std::shared_ptr<real_data<T>> times = std::make_shared<real_data<T>>(real_explicit<T>(std::to_string(-x.second)));
                                    std::shared_ptr<real_data<T>> num = x.first;
                                    mul_node = std::make_shared<real_data<T>>(real_operation(times, num, OPERATION::MULTIPLICATION));
                                }
                                ret = std::make_shared<real_data<T>>(real_operation(mul_node, ret, OPERATION::ADDITION));
                            }
                        }
                        else {
                            // after finding postive node, we can join these multiplication nodes as usual with +/-
                            if (x.second > 0) {
                                // if no. of times it occurs is positive, we join with + operation
                                std::shared_ptr<real_data<T>> mul_node;
                                if (x.second == 1) {
                                    mul_node = x.first;
                                }
                                else {
                                    std::shared_ptr<real_data<T>> times = std::make_shared<real_data<T>>(real_explicit<T>(std::to_string(x.second)));
                                    std::shared_ptr<real_data<T>> num = x.first;
                                    mul_node = std::make_shared<real_data<T>>(real_operation(times, num, OPERATION::MULTIPLICATION));
                                }
                                ret = std::make_shared<real_data<T>>(real_operation(ret, mul_node, OPERATION::ADDITION));
                            }
                            else {
                                // if no. of times it occurs is negative, we join with - operation
                                std::shared_ptr<real_data<T>> mul_node;
                                if (x.second == -1) {
                                    mul_node = x.first;
                                }
                                else {
                                    std::shared_ptr<real_data<T>> times = std::make_shared<real_data<T>>(real_explicit<T>(std::to_string(-x.second)));
                                    std::shared_ptr<real_data<T>> num = x.first;
                                    mul_node = std::make_shared<real_data<T>>(real_operation(times, num, OPERATION::MULTIPLICATION));
                                }
                                ret = std::make_shared<real_data<T>>(real_operation(ret, mul_node, OPERATION::SUBTRACTION));
                            }
                        }
                    }
                }

                if (!found_first_node) { 
                    // if first node is not found, that means the tree is zero
                    static real<T> zero("0");
                    ret = zero._real_p;
                    found_first_node = false;
                }
                else if (!found_positive_node) {
                    // if positive node is not found, we subtract the current tree from zero
                    static real<T> zero("0");
                    std::shared_ptr<real_data<T>> zero_ptr = zero._real_p;
                    ret = std::make_shared<real_data<T>>(real_operation(zero_ptr, ret, OPERATION::SUBTRACTION));
                    found_positive_node = true;
                }

                return ret;
            }

            // returns map of the real number and its occurences in the subtree
            std::map<std::shared_ptr<real_data<T>>, int> optimize_traversal(std::shared_ptr<real_data<T>> real_p, bool top) { 
                std::map<std::shared_ptr<real_data<T>>, int> count; 

                std::visit( overloaded {
                    [&real_p, top, &count, this] (const real_explicit<T>& real) { 
                        if (optimize_vars.use_prev == true && optimize_vars.prev_top.first == real_p) {
                            // if found previous top node, use its map
                            count = optimize_vars.prev_top.second;
                        }
                        else
                        {
                            count[real_p]++; 
                            if (top) {
                                this->_real_p = real_p;
                                optimize_vars.use_prev = false;
                            }
                        } 
                    },
                    [&real_p, top, &count, this] (const real_algorithm<T>& real) {
                        if (optimize_vars.use_prev == true && optimize_vars.prev_top.first == real_p) {
                            // if found previous top node, use its map
                            count = optimize_vars.prev_top.second;
                        }
                        else
                        {
                            count[real_p]++; 
                            if (top) {
                                this->_real_p = real_p;
                                optimize_vars.use_prev = false;
                            }
                        } 
                    },
                    [&real_p, top, &count, this] (const real_operation<T>& real) { 
                        if (optimize_vars.use_prev == true && optimize_vars.prev_top.first == real_p) {
                            // if found previous top node, use its map
                            count = optimize_vars.prev_top.second; 
                        }
                        else {
                            auto count_lhs = optimize_traversal(real.lhs(), false);
                            auto count_rhs = optimize_traversal(real.rhs(), false);
                            
                            if (real.get_operation() == OPERATION::ADDITION) { 
                                // maps of lhs and rhs are added
                                count = count_lhs;
                                for (auto x: count_rhs) {
                                    count[x.first] += x.second;
                                }

                                if (top) { 
                                    // we always have to call simplify() at top
                                    real_p = simplify(count);
                                    this->_real_p = real_p;

                                    // we can use this information for further optimization
                                    optimize_vars.use_prev = true;
                                    // For next optimize() call, we need the count map before its simplification
                                    optimize_vars.prev_top = make_pair(this->_real_p, count);
                                }
                            }
                            else if (real.get_operation() == OPERATION::SUBTRACTION) {
                                // maps of lhs and rhs are subtracted
                                count = count_lhs;
                                for (auto x: count_rhs) {
                                    count[x.first] -= x.second;
                                }

                                if (top) {
                                    // we always have to call simplify() at top
                                    real_p = simplify(count);
                                    this->_real_p = real_p;

                                    // we can use this information for further optimization
                                    optimize_vars.use_prev = true;
                                    // For next optimize() call, we need the count map before its simplification
                                    optimize_vars.prev_top = make_pair(this->_real_p, count);
                                }
                            }
                            else 
                            {
                                // if operation is other than addition or subtraction, we have to simplify its left and right subtree
                                std::shared_ptr<real_data<T>> lhs_p = simplify(count_lhs);
                                std::shared_ptr<real_data<T>> rhs_p = simplify(count_rhs);
                                real_p = std::make_shared<real_data<T>>(real_operation(lhs_p, rhs_p, real.get_operation()));
                                count[real_p]++;

                                if (top) {
                                    this->_real_p = real_p;
                                    // we can't use the information beneath this node for next optimize() call
                                    optimize_vars.use_prev = false;
                                }
                            }
                        }
                    },
                    [&real_p, top, &count, this] (const real_rational<T>& real) { 
                        if (optimize_vars.use_prev == true && optimize_vars.prev_top.first == real_p) {
                            // if found previous top node, use its map
                            count = optimize_vars.prev_top.second;
                        }
                        else
                        {
                            count[real_p]++; 
                            if (top) {
                                this->_real_p = real_p;
                                optimize_vars.use_prev = false;
                            }
                        } 
                    },
                    [] (auto& real) {
                        throw boost::real::bad_variant_access_exception();
                    }
                }, real_p->get_real_number());
                
                return count;
            }

            // optimizes the dag, converts it into a simplified smaller dag 
            void optimize() {
                optimize_traversal(this->_real_p, true);
                // update number of variables after optimization
                optimize_vars.num_vars = this->find_num_vars();
            }

            // sets optimize_freq 
            void set_optimize_freq(int freq) { 
                optimize_vars.optimize_freq = freq;
                this->check_if_optimize();
            }

            // checks if we have to optimize the real
            void check_if_optimize() { 
                if (optimize_vars.optimize_freq != -1 
                    && optimize_vars.num_vars >= optimize_vars.optimize_freq + optimize_vars.num_vars_at_last_optimize) {
                    this->optimize();
                    optimize_vars.num_vars_at_last_optimize = optimize_vars.num_vars;
                }
            }

            // increment number of variables and check if we have to optimize the real
            void increment_vars_and_check_if_optimize(int num_vars) {
                optimize_vars.num_vars += num_vars;
                this->check_if_optimize();
            }

            // set number of variables and check if we have to optimize the real
            void set_vars_and_check_if_optimize(int num_vars) {
                optimize_vars.num_vars = num_vars;
                this->check_if_optimize();
            }

            // helper function to recursively find the number of variables
            int find_num_vars_traversal(std::shared_ptr<real_data<T>> real_p) {
                
                int ret;

                std::visit( overloaded{
                    [&ret] (const real_explicit<T>& real) {
                        ret = 1;
                    },
                    [&ret] (const real_algorithm<T>& real) {
                        ret = 1;
                    },
                    [&ret, this] (const real_operation<T>& real) {
                        // no. of variables for real operation
                        // = no. of variables in left subtree + no. of variables in right subtree
                        int ret_lhs = find_num_vars_traversal(real.lhs());
                        int ret_rhs = find_num_vars_traversal(real.rhs());
                        ret = ret_lhs + ret_rhs;
                    },
                    [&ret] (const real_rational<T>& real) {
                        ret = 1;
                    },
                    [] (auto& real) {
                        throw boost::real::bad_variant_access_exception();
                    }
                }, real_p->get_real_number());

                return ret;
            }

            // calculates the number of variables in real tree
            int find_num_vars() {
                int ret = find_num_vars_traversal(this->_real_p);
                return ret;
            }


            /**
             *      EXPONENT METHOD
             * @brief: Calculates e^real_num
             * @params: real_num: power to which e is to be raised
             * @return: returns a new boost::real which is e^real_num
             * @author: Vikram Singh Chundawat
             **/
            static real exp(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::EXPONENT));
            }

            /**
             *      LOGARITHM METHOD
             * @brief: Calculated ln(real_num) or log(real_num) (base e)
             * @params: real_num: boost::real number whose logarithm is to be calculated.
             * @return: returns a new boost::real which is ln(real_num)
             * @author: Vikram Singh Chundawat
             **/
            static real log(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::LOGARITHM));
            }

            /**
             *      LOGARITHM METHOD
             * @brief: Calculated log(real_num) (base 10)
             * @params: real_num: boost::real number whose logarithm(base 10) is to be calculated.
             * @return: returns a new boost::real which is ln(real_num)
             * @author: Suraj Nehra
             **/
            static real log10(real<T> real_num){
                static real<T> zero("0");
                static real<T> ten("10");
                // log x (base 10) = lnx/ln10 or (log x (base e))/ (log 10 (base e))
                // Errors related to non-positive numbers will be handled by log function which we are calling in this function.
                real<T> logx = real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::LOGARITHM));
                real<T> log10 = real(real_operation<T>(ten._real_p, zero._real_p, OPERATION::LOGARITHM));
                return real(real_operation<T>(logx._real_p, log10._real_p, OPERATION::DIVISION));
            }

            /*      POWER METHOD
             *  @brief:  Calculates real_num^exponent
             *  @params: real_num: boost real number whose power is to be evaluated
             *  @params: exponent: power to which real_num is to be raised (needs to be an integer)
             *  @return: returns a new boost real whose value is real_num^exponent
             *  @author: Kishan Shukla & Vikram Singh Chundawat
             */
            static real power(real<T> real_num, real<T> power){
                // checking whether the number is integer or not
                static real<T> zero("0");
                static real<T> one("1");
                real<T> result;

                try{
                    result = real(real_operation<T>(real_num._real_p, power._real_p, OPERATION::INTEGER_POWER));
                }
                catch(const negative_integers_not_supported& e1){
                    power = real<T>("-1")*power;
                    result = real(real_operation<T>(real_num._real_p, power._real_p, OPERATION::INTEGER_POWER));
                    result = real(real_operation<T>(one._real_p, result._real_p, OPERATION::DIVISION));
                }
                catch(const non_integral_exponent_exception& e2){
                    try{
                        /**
                     * result = exp(exponent * log(real_num))
                     * Warning: the result of non-integral power of a negative number is a complex number,
                     * and we do not support complex numbers. So, that will generate error.
                     * Note: The error will be generated by logarithm function.
                     **/
                    

                    /**
                     * Now, if number is negative, then logarithm function will check out and throw error
                     **/
                    result = real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::LOGARITHM));
                    result = real(real_operation<T>(result._real_p, power._real_p, OPERATION::MULTIPLICATION));
                    result = real(real_operation<T>(result._real_p, zero._real_p, OPERATION::EXPONENT));
                    }
                    catch(const logarithm_not_defined_for_non_positive_number& e3){
                        throw non_integral_power_of_negative_number();
                    }

                }
                return result; 
            }

            /*      SQAURE ROOT METHOD
             *  @brief:  Calculates real_num^(1/2) or sqrt(real_num)
             *  @params: real_num: boost real number whose sqaure root is to be evaluated
             *  @return: returns a new boost real whose value is sqrt(real_num)
             *  @author: Divyam Singal
             */

            static real sqrt(real<T> real_num) {
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::SQRT));
            }


            /**
             *      SIN METHOD (INPUT IN RADIANS)
             * @brief: Calculated sin(real_num), real_num should represent angle in radians
             * @params: real_num: boost::real number whose sin is to be calculated.
             * @return: returns a new boost::real which is sin(real_num)
             * @author: Vikram Singh Chundawat
             **/
            static real sin(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::SIN));
            }

            /**
             *      COS METHOD (INPUT IN RADIANS)
             * @brief: Calculated cos(real_num), real_num should represent angle in radians
             * @params: real_num: boost::real number whose cos is to be calculated.
             * @return: returns a new boost::real which is cos(real_num)
             * @author: Vikram Singh Chundawat
             **/
            static real cos(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::COS));
            }


            /**
             *      TAN METHOD (INPUT IN RADIANS)
             * @brief: Calculated tan(real_num), real_num should represent angle in radians
             * @params: real_num: boost::real number whose tan is to be calculated.
             * @return: returns a new boost::real which is tan(real_num)
             * @author: Vikram Singh Chundawat
             **/
            static real tan(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::TAN));
            }

            /**
             *      COT METHOD (INPUT IN RADIANS)
             * @brief: Calculated cot(real_num), real_num should represent angle in radians
             * @params: real_num: boost::real number whose cot is to be calculated.
             * @return: returns a new boost::real which is cot(real_num)
             * @author: Vikram Singh Chundawat
             **/
            static real cot(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::COT));
            }

            /**
             *      SEC METHOD (INPUT IN RADIANS)
             * @brief: Calculated sec(real_num), real_num should represent angle in radians
             * @params: real_num: boost::real number whose sec is to be calculated.
             * @return: returns a new boost::real which is sec(real_num)
             * @author: Vikram Singh Chundawat
             **/
            static real sec(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::SEC));
            }

            /**
             *      COSEC METHOD (INPUT IN RADIANS)
             * @brief: Calculated cosec(real_num), real_num should represent angle in radians
             * @params: real_num: boost::real number whose cosec is to be calculated.
             * @return: returns a new boost::real which is cosec(real_num)
             * @author: Vikram Singh Chundawat
             **/
            static real cosec(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::COSEC));
            }

            /**
             *      ASIN METHOD 
             * @brief: Calculates asin(real_num)
             * @params: real_num: boost::real number whose asin is to be calculated.
             * @return: returns a new boost::real which is asin(real_num)
             * @author: Divyam Singal
             **/
            static real asin(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::ASIN));
            }

            /**
             *      ACOS METHOD 
             * @brief: Calculates acos(real_num)
             * @params: real_num: boost::real number whose acos is to be calculated.
             * @return: returns a new boost::real which is acos(real_num)
             * @author: Divyam Singal
             **/
            static real acos(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::ACOS));
            }

            /**
             *      ATAN METHOD 
             * @brief: Calculates atan(real_num)
             * @params: real_num: boost::real number whose atan is to be calculated.
             * @return: returns a new boost::real which is atan(real_num)
             * @author: Divyam Singal
             **/
            static real atan(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::ATAN));
            }

            /**
             *      ACOT METHOD 
             * @brief: Calculates acot(real_num)
             * @params: real_num: boost::real number whose acot is to be calculated.
             * @return: returns a new boost::real which is acot(real_num)
             * @author: Divyam Singal
             **/
            static real acot(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::ACOT));
            }

            /**
             *      ASEC METHOD 
             * @brief: Calculates asec(real_num)
             * @params: real_num: boost::real number whose asec is to be calculated.
             * @return: returns a new boost::real which is asec(real_num)
             * @author: Divyam Singal
             **/
            static real asec(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::ASEC));
            }

            /**
             *      ACOSEC METHOD 
             * @brief: Calculates acosec(real_num)
             * @params: real_num: boost::real number whose acosec is to be calculated.
             * @return: returns a new boost::real which is acosec(real_num)
             * @author: Divyam Singal
             **/
            static real acosec(real<T> real_num){
                static real<T> zero("0");
                return real(real_operation<T>(real_num._real_p, zero._real_p, OPERATION::ACOSEC));
            }

            /**
             *      ATAN2 METHOD 
             * @brief: Calculates atan2(y, x) defined as the angle in the Euclidean plane, 
             * between the positive x axis and the ray to the point (x, y) ≠ (0, 0).
             * @params: y: y-coordinate of the point
             * @params: x: x-coordinate of the point
             * @return: returns a new boost::real which is atan2(y, x)
             * @author: Divyam Singal
             **/
            static real atan2(real<T> y, real<T> x){
                return real(real_operation<T>(y._real_p, x._real_p, OPERATION::ATAN2));
            }


            /**
             * @brief Sets this real_data to that of the operation between this previous
             * real_data and other real_data.
             * 
             * If this is being pointed to by another operation, a copy of this is created.
             *
             * @param other - the right side operand boost::real::real number.
             */

            void operator += (real<T> other) {
                std::visit( overloaded{ 
                    [this] (real_rational<T> a, real_rational<T> b){
                        this->_real_p = 
                            std::make_shared<real_data<T>>(real_rational<T>(a+b));
                    },

                    [this, &other] (real_rational<T> rat, auto tmp){
                        // converting rational number to explicit or operation
                        real<T> rat_num;
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                        
                            // if rational number is of rational type, then it would be converted to a division operation between two integers
                            real _a;
                            _a._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a)); 
                            real _b;
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                            optimize_vars.num_vars += 1;
                        }
                        
                        
                        // now adding the numbers
                        auto [is_simplified, result] = check_and_distribute(other, true, OPERATION::ADDITION, RECURSION_LEVEL::TWO);
                        if(!is_simplified){
                            this->_real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(rat_num._real_p, other._real_p, OPERATION::ADDITION));
                        }
                        this->increment_vars_and_check_if_optimize(other.optimize_vars.num_vars);
                    },

                    [this, &other] (auto tmp, real_rational<T> rat){
                        real<T> rat_num;
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            optimize_vars.num_vars += 1;
                        }
                        else{
                            real _a;
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            real _b;
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation(_a._real_p, _b._real_p, OPERATION::DIVISION));
                            optimize_vars.num_vars += 2;
                        }

                        // now adding the numbers
                        auto [is_simplified, result] = check_and_distribute(other, true, OPERATION::ADDITION, RECURSION_LEVEL::TWO);
                        if(!is_simplified){
                            this->_real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(this->_real_p,rat_num._real_p, OPERATION::ADDITION));
                        }
                        this->check_if_optimize();

                    },

                    [this, &other] (auto a, auto b){
                        auto [is_simplified,result] = check_and_distribute(other, true, OPERATION::ADDITION, RECURSION_LEVEL::TWO);
                        
                        if (!is_simplified) {
                            this->_real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(this->_real_p, other._real_p, OPERATION::ADDITION));
                        }
                        this->increment_vars_and_check_if_optimize(other.optimize_vars.num_vars);
                    }
                }, _real_p->get_real_number(), other._real_p->get_real_number());
            }

            /**
             * @brief Creates a new boost::real::real_operation representing the sum of the
             * two numbers, using pointers to each operands' data.
             *
             * @param other - the right side operand boost::real::real number.
             * @return A copy of the new boost::real::real number representation.
             */
            real<T> operator + (real<T> other) {
                real<T> result;
                std::visit( overloaded{
                    [&result] (real_rational<T> a, real_rational<T> b){
                        result._real_p = 
                            std::make_shared<real_data<T>>(real_rational<T>(a+b));
                        result.optimize_vars.num_vars = 1;
                    },

                    [this, &other, &result] (real_rational<T> rat, auto tmp){
                        // converting rational number to explicit or operation type
                        real<T> rat_num; //rational number

                        // if number is of integer type, rat_num will be converted to explicit number
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                            real<T> _a, _b; // explicits numbers to represent numerator and denominator of rational number
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                        }

                        auto [is_simplified, result1] = rat_num.check_and_distribute(other, false, OPERATION::ADDITION, RECURSION_LEVEL::TWO);
                        if (is_simplified)  {
                            result = result1.value();
                        } else {
                            result = real<T>(real_operation<T>(rat_num._real_p, other._real_p, OPERATION::ADDITION));
                        }
                    },

                    [this, &other, &result] (auto tmp, real_rational<T> rat){
                        // converting rational number to explicit or operation type
                        real<T> rat_num; //rational number

                        // if number is of integer type, rat_num will be converted to explicit number
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                            real<T> _a, _b; // explicits numbers to represent numerator and denominator of rational number
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                        }

                        auto [is_simplified, result1] = rat_num.check_and_distribute(other, false, OPERATION::ADDITION, RECURSION_LEVEL::TWO);
                        if (is_simplified)  {
                            result = result1.value();
                        } else {
                            result = real<T>(real_operation<T>(this->_real_p, rat_num._real_p, OPERATION::ADDITION));
                        }
                    },

                    [this, &other, &result] (auto a, auto b){
                        auto [is_simplified, result1] = check_and_distribute(other, false, OPERATION::ADDITION, RECURSION_LEVEL::TWO);
                        if (is_simplified)  {
                            result = result1.value();
                        } else {
                            result = real<T>(real_operation<T>(this->_real_p, other._real_p, OPERATION::ADDITION));
                        }
                    }
                }, _real_p->get_real_number(), other._real_p->get_real_number());
                return result;
                
            }

            /**
             * @brief Sets this real_data to that of the operation between this previous
             * real_data and other real_data.
             *
             * @param other - the right side operand boost::real::real number.
             */
            void operator -= (real<T> other) {
                std::visit(overloaded{
                    [this] (real_rational<T> a, real_rational<T> b){
                        this->_real_p = 
                            std::make_shared<real_data<T>>(real_rational<T>(a-b));
                    },

                    [this, &other] (real_rational<T> rat, auto tmp){
                        // converting rational number to explicit or operation
                        real<T> rat_num;
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                        
                            // if rational number is of rational type, then it would be converted to a division operation between two integers
                            real _a;
                            _a._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a)); 
                            real _b;
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                            optimize_vars.num_vars += 1;
                        }
                        
                        
                        // now adding the numbers
                        auto [is_simplified, result] = check_and_distribute(other, true, OPERATION::SUBTRACTION, RECURSION_LEVEL::TWO);
                        if(!is_simplified){
                            this->_real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(rat_num._real_p, other._real_p, OPERATION::SUBTRACTION));
                        }
                        this->increment_vars_and_check_if_optimize(other.optimize_vars.num_vars);
                    },

                    [this, &other] (auto tmp, real_rational<T> rat){
                        real<T> rat_num;
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            optimize_vars.num_vars += 1;
                        }
                        else{
                            real _a;
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            real _b;
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation(_a._real_p, _b._real_p, OPERATION::DIVISION));
                            optimize_vars.num_vars += 2;
                        }

                        // now adding the numbers
                        auto [is_simplified, result] = check_and_distribute(other, true, OPERATION::SUBTRACTION, RECURSION_LEVEL::TWO);
                        if(!is_simplified){
                            this->_real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(this->_real_p,rat_num._real_p, OPERATION::SUBTRACTION));
                        }
                        this->check_if_optimize();

                    },

                    [this, &other] (auto a, auto b){
                        auto [is_simplified, result] = check_and_distribute(other, true, OPERATION::SUBTRACTION, RECURSION_LEVEL::TWO);

                        if(!is_simplified) {
                            this->_real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(this->_real_p, other._real_p, OPERATION::SUBTRACTION));
                        }
                        this->increment_vars_and_check_if_optimize(other.optimize_vars.num_vars);
                    }
                }, _real_p->get_real_number(), other._real_p->get_real_number());
                
            }

            /**
             * @brief Creates a new boost::real::real representing the subtraction
             * between *this and other
             *
             * @param other - the right side operand boost::real::real number.
             * @return A copy of the new boost::real::real number representation.
             */
            real<T> operator - (real<T> other) {
                real<T> result;
                std::visit( overloaded{
                    [&result] (real_rational<T> a, real_rational<T> b){
                        result._real_p = 
                            std::make_shared<real_data<T>>(real_rational<T>(a-b));
                        result.optimize_vars.num_vars = 1;
                    },

                    [this, &other, &result] (real_rational<T> rat, auto tmp){
                        // converting rational number to explicit or operation type
                        real<T> rat_num; //rational number

                        // if number is of integer type, rat_num will be converted to explicit number
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                            real<T> _a, _b; // explicits numbers to represent numerator and denominator of rational number
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                        }

                        auto [is_simplified, result1] = rat_num.check_and_distribute(other, false, OPERATION::SUBTRACTION, RECURSION_LEVEL::TWO);
                        if (is_simplified)  {
                            result = result1.value();
                        } else {
                            result = real<T>(real_operation<T>(rat_num._real_p, other._real_p, OPERATION::SUBTRACTION));
                        }
                    },

                    [this, &other, &result] (auto tmp, real_rational<T> rat){
                        // converting rational number to explicit or operation type
                        real<T> rat_num; //rational number

                        // if number is of integer type, rat_num will be converted to explicit number
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                            real<T> _a, _b; // explicits numbers to represent numerator and denominator of rational number
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                        }

                        auto [is_simplified, result1] = rat_num.check_and_distribute(other, false, OPERATION::SUBTRACTION, RECURSION_LEVEL::TWO);
                        if (is_simplified)  {
                            result = result1.value();
                        } else {
                            result = real<T>(real_operation<T>(this->_real_p, rat_num._real_p, OPERATION::SUBTRACTION));
                        }
                    },

                    [this, &other, &result] (auto a, auto b){
                        auto [is_simplified, result1] = check_and_distribute(other, false, OPERATION::SUBTRACTION, RECURSION_LEVEL::TWO);
                        if (is_simplified)  {
                            result = result1.value();
                        } else {
                            result = real(real_operation<T>(this->_real_p, other._real_p, OPERATION::SUBTRACTION));
                        }
                    }
                }, _real_p->get_real_number(), other._real_p->get_real_number());
                return result;
            }

            /**
             * @brief Sets this real_data to that of the operation between 
             * this previous real_data and other real_data.
             *
             * @param other - the right side operand boost::real::real number.
             */
            void operator*=(real<T> other) {
                std::visit(overloaded{
                    [this] (real_rational<T> a, real_rational<T> b){
                        this->_real_p = 
                            std::make_shared<real_data<T>>(real_rational<T>(a*b));
                    },

                    [this, &other] (real_rational<T> rat, auto tmp){
                        // converting rational number to explicit or operation
                        real<T> rat_num;
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                        
                            // if rational number is of rational type, then it would be converted to a division operation between two integers
                            real _a;
                            _a._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a)); 
                            real _b;
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                            optimize_vars.num_vars += 1;
                        }
                        
                        
                        // now adding the numbers
                        this->_real_p = 
                            std::make_shared<real_data<T>>(real_operation<T>(rat_num._real_p, other._real_p, OPERATION::MULTIPLICATION));
                        this->increment_vars_and_check_if_optimize(other.optimize_vars.num_vars);
                    },

                    [this, &other] (auto tmp, real_rational<T> rat){
                        real<T> rat_num;
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            optimize_vars.num_vars += 1;
                        }
                        else{
                            real _a;
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            real _b;
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation(_a._real_p, _b._real_p, OPERATION::DIVISION));
                            optimize_vars.num_vars += 2;
                        }

                        // now adding the numbers
                        
                        this->_real_p = 
                            std::make_shared<real_data<T>>(real_operation<T>(this->_real_p,rat_num._real_p, OPERATION::MULTIPLICATION));
                        this->check_if_optimize();

                    },


                    [this, &other] (auto a, auto b){
                        this->_real_p =
                        std::make_shared<real_data<T>>(real_operation<T>(this->_real_p, other._real_p, OPERATION::MULTIPLICATION));
                        this->increment_vars_and_check_if_optimize(other.optimize_vars.num_vars);
                    }
                }, _real_p->get_real_number(), other._real_p->get_real_number());
                
            }

            /**
             * @brief Creates a new boost::real::real representing the product
             * of *this and other
             *
             * @param other - the right side operand boost::real::real number.
             * @return A copy of the new boost::real::real number representation.
             */
            real<T> operator * (real<T> other) {
                real<T> result;
                std::visit(overloaded{
                    [&result] (real_rational<T> a, real_rational<T> b){
                        result._real_p = 
                            std::make_shared<real_data<T>>(real_rational<T>(a*b));
                        result.optimize_vars.num_vars = 1;
                    },

                    [this, &other, &result] (real_rational<T> rat, auto tmp){
                        // converting rational number to explicit or operation type
                        real<T> rat_num; //rational number

                        // if number is of integer type, rat_num will be converted to explicit number
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                            real<T> _a, _b; // explicits numbers to represent numerator and denominator of rational number
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                        }

                        
                        result = real<T>(real_operation<T>(rat_num._real_p, other._real_p, OPERATION::MULTIPLICATION));
                        
                    },

                    [this, &other, &result] (auto tmp, real_rational<T> rat){
                        // converting rational number to explicit or operation type
                        real<T> rat_num; //rational number

                        // if number is of integer type, rat_num will be converted to explicit number
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                            real<T> _a, _b; // explicits numbers to represent numerator and denominator of rational number
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                        }

                        
                        result = real<T>(real_operation<T>(this->_real_p, rat_num._real_p, OPERATION::MULTIPLICATION));
                        
                    },

                    [this, &other, &result] (auto a, auto b){
                        result = real(real_operation<T>(this->_real_p, other._real_p, OPERATION::MULTIPLICATION));
                    }
                }, _real_p->get_real_number(), other.get_real_number());
                return result;
            }

            /**
             * @brief Creates a new boost::real::real representing the product
             * of *this and other
             *
             * @param other - the right side operand boost::real::real number.
             * @return A copy of the new boost::real::real number representation.
             */
            real<T> operator / (real<T> other) {
                real<T> result;
                std::visit(overloaded{
                    [&result] (real_rational<T> a, real_rational<T> b){
                        real_rational<T> result1 = a/b;
                        result._real_p = 
                            std::make_shared<real_data<T>>(real_rational(result1));
                        result.optimize_vars.num_vars = 1;
                    },

                    [this, &other, &result] (real_rational<T> rat, auto tmp){
                        // converting rational number to explicit or operation type
                        real<T> rat_num; //rational number

                        // if number is of integer type, rat_num will be converted to explicit number
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                            real<T> _a, _b; // explicits numbers to represent numerator and denominator of rational number
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                        }

                        
                        result = real<T>(real_operation<T>(rat_num._real_p, other._real_p, OPERATION::DIVISION));
                        
                    },

                    [this, &other, &result] (auto tmp, real_rational<T> rat){
                        // converting rational number to explicit or operation type
                        real<T> rat_num; //rational number

                        // if number is of integer type, rat_num will be converted to explicit number
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                            real<T> _a, _b; // explicits numbers to represent numerator and denominator of rational number
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                        }

                        
                        result = real<T>(real_operation<T>(this->_real_p, rat_num._real_p, OPERATION::DIVISION));
                        
                    },

                    [this, &other, &result] (auto a, auto b){
                        result = real(real_operation<T>(this->_real_p, other._real_p, OPERATION::DIVISION));
                    }
                }, _real_p->get_real_number(), other._real_p->get_real_number());
                return result;
            }

            /**
             * @brief Sets this real_data to that of the operation between 
             * this previous real_data and other real_data.
             *
             * @param other - the right side operand boost::real::real number.
             */
            void operator /= (real<T> other) {
                std::visit(overloaded{
                    [this] (real_rational<T> a, real_rational<T> b){
                        this->_real_p = 
                            std::make_shared<real_data<T>>(real_rational<T>(a/b));
                    },

                    [this, &other] (real_rational<T> rat, auto tmp){
                        // converting rational number to explicit or operation
                        real<T> rat_num;
                        if(rat.b == literals::one_integer<T>){
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                        }
                        else{
                        
                            // if rational number is of rational type, then it would be converted to a division operation between two integers
                            real _a;
                            _a._real_p =
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a)); 
                            real _b;
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));
                            optimize_vars.num_vars += 1;
                        }
                        
                        
                        // now adding the numbers
                        this->_real_p = 
                            std::make_shared<real_data<T>>(real_operation<T>(rat_num._real_p, other._real_p, OPERATION::DIVISION));
                        this->increment_vars_and_check_if_optimize(other.optimize_vars.num_vars);
                    },

                    [this, &other] (auto tmp, real_rational<T> rat){
                        real<T> rat_num;
                        if(rat.b==literals::one_integer<T>){
                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            optimize_vars.num_vars += 1;
                        }
                        else{
                            real _a;
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.a));
                            real _b;
                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat.b));

                            rat_num._real_p = 
                                std::make_shared<real_data<T>>(real_operation(_a._real_p, _b._real_p, OPERATION::DIVISION));
                            optimize_vars.num_vars += 2;
                        }

                        // now adding the numbers
                        
                        this->_real_p = 
                            std::make_shared<real_data<T>>(real_operation<T>(this->_real_p,rat_num._real_p, OPERATION::DIVISION));
                        this->check_if_optimize();

                    },

                    [this, &other] (auto a, auto b){
                        this->_real_p =
                            std::make_shared<real_data<T>>(real_operation<T>(this->_real_p, other._real_p, OPERATION::DIVISION));
                        this->increment_vars_and_check_if_optimize(other.optimize_vars.num_vars);
                    }
                }, _real_p->get_real_number(), other._real_p->get_real_number());
                
            }

            /**
             * @brief Assigns *this to other
             * @param other - the boost::real::real number to copy.
             */
            void operator=(real<T> other) {
                this->_real_p = other._real_p;
                this->optimize_vars = other.optimize_vars;
            }

            /**
             * @brief Assigns *this to the real number represented by the string number
             * @param number - a valid string representing a number.
             */
            void operator=(const std::string& number) {
                this->_real_p =
                    std::make_shared<real_data<T>>(real_explicit<T>(number));
                optimize_vars.num_vars = 1;
            }

            /**
             * @brief Compares the *this boost::real::real number against the other boost::real::real number to
             * determine if the number represented by *this is lower than the number represented by other.
             * If the maximum precision is reached and the operator was not yet able to determine
             * the value of the result, a precision_exception is thrown.
             *
             * @param other - a boost::real::real number to compare against.
             * @return a bool that is true if *this < other and false in other cases.
             *
             * @throws boost::real::precision_exception
             */
            bool operator<(const real<T>& other) const {
                bool ret;
                std::visit(overloaded{
                    [&ret] (real_rational<T> a, real_rational<T> b){
                        ret = (a<b);
                    },

                    [this, &other, &ret] (real_rational<T> rat_num, auto tmp){
                        real<T> _this;
                        if(rat_num.b == literals::one_integer<T>)
                            _this._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));
                        else {
                            real _a, _b; // to represent "a" and "b" in rational number a/b
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));

                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.b));

                            _this._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));

                        }


                        // now comparing numbers
                        auto this_it = _this._real_p->get_precision_itr().cbegin();
                        auto other_it = other._real_p->get_precision_itr().cbegin();

                        if(this_it == other_it)
                        {
                            ret = false;
                            return;
                        }

                        unsigned int current_precision = std::max(_this.maximum_precision(), other.maximum_precision());
                        unsigned int p;
                        for (p = 0; p < current_precision; ++p){
                            // Get more precision
                            ++this_it;
                            ++other_it;

                            bool this_full_precision = this_it.get_interval().is_a_number();
                            bool other_full_precision = other_it.get_interval().is_a_number();
                            if (this_full_precision && other_full_precision){
                                ret = this_it.get_interval() < other_it.get_interval();
                                break;
                            }

                            if (this_it.get_interval()< other_it.get_interval()) {
                                ret = true;
                                break;
                            }

                            if (other_it.get_interval()< this_it.get_interval()) {
                                ret = false;
                                break;
                            }
                        }

                        // If the precision is reached and the number ranges still overlap, then we cannot
                        // know if they are equals or other is less than this and we throw an error.
                        if(p == current_precision)
                            throw boost::real::precision_exception();
                    },

                    [this, &ret] (auto tmp, real_rational<T> rat_num){
                        real<T> other; // representing "other" number, which is a rational number
                        if(rat_num.b == literals::one_integer<T>)   
                            other._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));
                        else {
                            real _a, _b; // to represent "a" and "b" in rational number a/b
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));

                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.b));

                            other._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));

                        }


                        // now comparing numbers
                        auto this_it = this->_real_p->get_precision_itr().cbegin();
                        auto other_it = other._real_p->get_precision_itr().cbegin();

                        if(this_it == other_it)
                        {
                            ret = false;
                            return;
                        }

                        unsigned int current_precision = std::max(this->maximum_precision(), other.maximum_precision());
                        unsigned int p;
                        for (p = 0; p < current_precision; ++p){
                            // Get more precision
                            ++this_it;
                            ++other_it;

                            bool this_full_precision = this_it.get_interval().is_a_number();
                            bool other_full_precision = other_it.get_interval().is_a_number();
                            if (this_full_precision && other_full_precision){
                                ret = this_it.get_interval() < other_it.get_interval();
                                break;
                            }

                            if (this_it.get_interval()< other_it.get_interval()) {
                                ret = true;
                                break;
                            }

                            if (other_it.get_interval()< this_it.get_interval()) {
                                ret = false;
                                break;
                            }
                        }

                        // If the precision is reached and the number ranges still overlap, then we cannot
                        // know if they are equals or other is less than this and we throw an error.
                        if(p == current_precision)
                            throw boost::real::precision_exception();
                    },

                    [this, &other, &ret] (auto a, auto b){
                        auto this_it = this->_real_p->get_precision_itr().cbegin();
                        auto other_it = other._real_p->get_precision_itr().cbegin();

                        if (this_it == other_it)
                        {
                            ret = false;
                            return;
                        }

                        unsigned int current_precision = std::max(this->maximum_precision(), other.maximum_precision());
                        unsigned int p;
                        for (p = 0; p < current_precision; ++p) {
                            // Get more precision
                            ++this_it;
                            ++other_it;

                            bool this_full_precision = this_it.get_interval().is_a_number();
                            bool other_full_precision = other_it.get_interval().is_a_number();
                            if (this_full_precision && other_full_precision) {
                                ret = this_it.get_interval() < other_it.get_interval();
                                break;
                            }

                            if (this_it.get_interval()< other_it.get_interval()) {
                                ret = true;
                                break;
                            }

                            if (other_it.get_interval()< this_it.get_interval()) {
                                ret = false;
                                break;
                            }
                        }

                        // If the precision is reached and the number ranges still overlap, then we cannot
                        // know if they are equals or other is less than this and we throw an error.
                        if(p == current_precision)
                            throw boost::real::precision_exception();
                    }
                }, _real_p->get_real_number(), other._real_p->get_real_number());
                return ret;
                
            }

            /**
             * @brief Compares the *this boost::real::real number against the other boost::real::real number to
             * determine if the number represented by *this is greater than the number represented by other.
             * If the maximum precision is reached and the operator was not yet able to determine
             * the value of the result, a precision_exception is thrown.
             *
             * @param other - a boost::real::real number to compare against.
             * @return a bool that is true if *this > other and false in other cases.
             *
             * @throws boost::real::precision_exception
             */
            bool operator>(const real<T>& other) const {
                bool ret;
                std::visit(overloaded{
                    [&ret] (real_rational<T> a, real_rational<T> b){
                        ret = (a>b);
                    },

                    [&other, &ret] (real_rational<T> rat_num, auto tmp){
                        real<T> _this; // representing "other" number, which is a rational number
                        if(rat_num.b == literals::one_integer<T>)   
                            _this._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));
                        else {
                            real _a, _b; // to represent "a" and "b" in rational number a/b
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));

                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.b));

                            _this._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));

                        }

                        auto this_it = _this._real_p->get_precision_itr().cbegin();
                        auto other_it = other._real_p->get_precision_itr().cbegin();

                        if (this_it == other_it){
                            ret = false;
                            return;
                        }

                        unsigned int current_precision = std::max(_this.maximum_precision(), other.maximum_precision());
                        unsigned int p;
                        for (p=0; p < current_precision; ++p) {
                            // Get more precision
                            ++this_it;
                            ++other_it;
                            bool this_full_precision = this_it.get_interval().is_a_number();
                            bool other_full_precision = other_it.get_interval().is_a_number();
                            if (this_full_precision && other_full_precision) {
                                ret = this_it.get_interval() > other_it.get_interval();
                                break;
                            }
                            if (this_it.get_interval()> other_it.get_interval()) {
                                ret = true;
                                break;
                            }
                            if (other_it.get_interval()> this_it.get_interval()) {
                                ret = false;
                                break;
                            }
                        }
                        // If the precision is reached and the number ranges still overlap, then we cannot
                        // know if they are equals or other is less than this and we throw an error.
                        if(p == current_precision)
                            throw boost::real::precision_exception();


                    },

                    [this, &ret] (auto tmp, real_rational<T> rat_num){

                        real<T> other; // representing "other" number, which is a rational number
                        if(rat_num.b == literals::one_integer<T>)   
                            other._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));
                        else {
                            real _a, _b; // to represent "a" and "b" in rational number a/b
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));

                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.b));

                            other._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));

                        }

                        auto this_it = this->_real_p->get_precision_itr().cbegin();
                        auto other_it = other._real_p->get_precision_itr().cbegin();

                        if (this_it == other_it){
                            ret = false;
                            return;
                        }

                        unsigned int current_precision = std::max(this->maximum_precision(), other.maximum_precision());
                        unsigned int p;
                        for (p=0; p < current_precision; ++p) {
                            // Get more precision
                            ++this_it;
                            ++other_it;
                            bool this_full_precision = this_it.get_interval().is_a_number();
                            bool other_full_precision = other_it.get_interval().is_a_number();
                            if (this_full_precision && other_full_precision) {
                                ret = this_it.get_interval() > other_it.get_interval();
                                break;
                            }
                            if (this_it.get_interval()> other_it.get_interval()) {
                                ret = true;
                                break;
                            }
                            if (other_it.get_interval()> this_it.get_interval()) {
                                ret = false;
                                break;
                            }
                        }
                        // If the precision is reached and the number ranges still overlap, then we cannot
                        // know if they are equals or other is less than this and we throw an error.
                        if(p == current_precision)
                            throw boost::real::precision_exception();
                    },

                    [this, &other, &ret] (auto a, auto b){
                        auto this_it = this->_real_p->get_precision_itr().cbegin();
                        auto other_it = other._real_p->get_precision_itr().cbegin();

                        if (this_it == other_it){
                            ret = false;
                            return;
                        }

                        unsigned int current_precision = std::max(this->maximum_precision(), other.maximum_precision());
                        unsigned int p;
                        for (p=0; p < current_precision; ++p) {
                            // Get more precision
                            ++this_it;
                            ++other_it;
                            bool this_full_precision = this_it.get_interval().is_a_number();
                            bool other_full_precision = other_it.get_interval().is_a_number();
                            if (this_full_precision && other_full_precision) {
                                ret = this_it.get_interval() > other_it.get_interval();
                                break;
                            }
                            if (this_it.get_interval()> other_it.get_interval()) {
                                ret = true;
                                break;
                            }
                            if (other_it.get_interval()> this_it.get_interval()) {
                                ret = false;
                                break;
                            }
                        }
                        // If the precision is reached and the number ranges still overlap, then we cannot
                        // know if they are equals or other is less than this and we throw an error.
                        if(p == current_precision)
                            throw boost::real::precision_exception();
                    }
                }, _real_p->get_real_number(), other._real_p->get_real_number());
                return ret;
            }

            /**
             * @brief Compares the *this boost::real::real number against the other boost::real::real number to
             * determine if the number represented by *this is the same than the number represented by other.
             * If the maximum precision is reached and the operator was not yet able to determine
             * the value of the result, a precision_exception is thrown.
             *
             * @param other - a boost::real::real number to compare against.
             * @return a bool that is true if *this < other and false in other cases.
             *
             * @throws boost::real::precision_exception
             */
            bool operator == (const real<T>& other) const {
                bool ret;
                std::visit(overloaded{
                    [&ret] (real_rational<T> a, real_rational<T> b){
                        ret = (a==b);
                    },

                    [&other, &ret] (real_rational<T> rat_num, auto tmp){
                        real<T> _this; // representing "other" number, which is a rational number
                        if(rat_num.b==literals::one_integer<T>)   
                            _this._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));
                        else {
                            real _a, _b; // to represent "a" and "b" in rational number a/b
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));

                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.b));

                            _this._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));

                        }

                        auto this_it = _this._real_p->get_precision_itr().cbegin();
                        auto other_it = other._real_p->get_precision_itr().cbegin();

                        unsigned int current_precision = std::max(_this.maximum_precision(), other.maximum_precision());
                        unsigned int p;
                        for (p = 0; p < current_precision; ++p) {
                            // Get more precision
                            ++this_it;
                            ++other_it;

                            bool this_full_precision = this_it.get_interval().is_a_number();
                            bool other_full_precision = other_it.get_interval().is_a_number();
                            if (this_full_precision && other_full_precision) {
                                ret = this_it.get_interval()== other_it.get_interval();
                                break;
                            }

                            bool this_is_lower = this_it.get_interval()< other_it.get_interval();
                            bool other_is_lower = other_it.get_interval()< this_it.get_interval();
                            if (this_is_lower || other_is_lower) {
                                ret = false;
                                break;
                            }
                        }

                        // If the precision is reached and the numbers full precision is not reached, then
                        // we cannot know if they are equals or not.
                        if(p==current_precision)
                            throw boost::real::precision_exception();

                    },

                    [this, &ret] (auto tmp, real_rational<T> rat_num){
                        real<T> other; // representing "other" number, which is a rational number
                        if(rat_num.b==literals::one_integer<T>)   
                            other._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));
                        else {
                            real _a, _b; // to represent "a" and "b" in rational number a/b
                            _a._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));

                            _b._real_p = 
                                std::make_shared<real_data<T>>(real_explicit<T>(rat_num.b));

                            other._real_p = 
                                std::make_shared<real_data<T>>(real_operation<T>(_a._real_p, _b._real_p, OPERATION::DIVISION));

                        }

                        auto this_it = this->_real_p->get_precision_itr().cbegin();
                        auto other_it = other._real_p->get_precision_itr().cbegin();

                        unsigned int current_precision = std::max(this->maximum_precision(), other.maximum_precision());
                        unsigned int p;
                        for (p = 0; p < current_precision; ++p) {
                            // Get more precision
                            ++this_it;
                            ++other_it;

                            bool this_full_precision = this_it.get_interval().is_a_number();
                            bool other_full_precision = other_it.get_interval().is_a_number();
                            if (this_full_precision && other_full_precision) {
                                ret = this_it.get_interval()== other_it.get_interval();
                                break;
                            }

                            bool this_is_lower = this_it.get_interval()< other_it.get_interval();
                            bool other_is_lower = other_it.get_interval()< this_it.get_interval();
                            if (this_is_lower || other_is_lower) {
                                ret = false;
                                break;
                            }
                        }

                        // If the precision is reached and the numbers full precision is not reached, then
                        // we cannot know if they are equals or not.
                        if(p == current_precision)
                            throw boost::real::precision_exception();

                    },

                    [this, &other, &ret] (auto a, auto b){
                        auto this_it = this->_real_p->get_precision_itr().cbegin();
                        auto other_it = other._real_p->get_precision_itr().cbegin();

                        unsigned int current_precision = std::max(this->maximum_precision(), other.maximum_precision());
                        unsigned int p;
                        for (p = 0; p < current_precision; ++p) {
                            // Get more precision
                            ++this_it;
                            ++other_it;

                            bool this_full_precision = this_it.get_interval().is_a_number();
                            bool other_full_precision = other_it.get_interval().is_a_number();
                            if (this_full_precision && other_full_precision) {
                                ret = this_it.get_interval()== other_it.get_interval();
                                break;
                            }

                            bool this_is_lower = this_it.get_interval()< other_it.get_interval();
                            bool other_is_lower = other_it.get_interval()< this_it.get_interval();
                            if (this_is_lower || other_is_lower) {
                                ret = false;
                                break;
                            }
                        }

                        // If the precision is reached and the numbers full precision is not reached, then
                        // we cannot know if they are equals or not.
                        if(p == current_precision)
                            throw boost::real::precision_exception();

                    } 

                }, _real_p->get_real_number(), other._real_p->get_real_number());

                return ret;
            }
            /********* END OPERATORS *********/

            /**
             * @brief overload of the << operator for std::ostream and boost::real::real
             *
             * @param os - The std::ostream object where to print the r number.
             * @param r - the boost::real::real number to print
             * @return a reference of the modified os object.
             */
            friend std::ostream& operator<<(std::ostream& os, real r) {
                os << r.get_real_itr().cend().get_interval();
                return os;
            }


        // to convert a real_integer type number to a real_explicit number
        void to_explicit(){
            std::visit(overloaded{
                [this] (real_rational<T> a){
                    if(a.b!=literals::one_rational<T>){
                        throw expected_real_integer_type_number();
                    }
                    this->_real_p = 
                        std::make_shared<real_data<T>>(real_explicit<T>(a.a));
                },
                [] (auto a){
                    throw expected_real_integer_type_number();
                }
            }, _real_p->get_real_number());

            return ;
        }



        // to convert a real_rational to its real_operation equivalent
        void to_operation(){
            std::visit(overloaded{
                [this] (real_rational<T> rat_num){
                    real _a, _b;
                    _a._real_p = 
                        std::make_shared<real_data<T>>(real_explicit<T>(rat_num.a));
                    _b._real_p = 
                        std::make_shared<real_data<T>>(real_explicit<T>(rat_num.b));

                    this->_real_p = 
                        std::make_shared<real_data<T>>(real_operation<T>(_a, _b, OPERATION::DIVISION));
                    optimize_vars.num_vars = 2;
                },
                [] (auto tmp){
                    throw expected_real_rational_type_number();
                }
            }, _real_p->get_real_number());
        }


        real<T> operator % (const real<T> other){
            real<T> result;
            std::visit(overloaded{
                [&result] (real_rational<T> a, real_rational<T> b){
                    if(a.b != literals::one_integer<T> || b.b != literals::one_integer<T>){
                        throw expected_real_integer_type_number();
                    }

                    result._real_p = 
                        std::make_shared<real_data<T>>(real_rational<T>(a.a % b.a));
                },
                [] (auto a, auto b){
                    throw expected_real_integer_type_number();
                }
            }, _real_p->get_real_number(), other._real_p->get_real_number());
            return result;
        }

        }; // end real class

        namespace literals{
            template<typename T>
            const real<T> one_real = real<T>("1");

            template<typename T>
            const real<T> zero_real = real<T>("0");
        }
    }
}

//User Defined Literals for Explicit Number

inline auto operator "" _r(long double x) {
    return boost::real::real<int>(std::to_string(x));
}

inline auto operator "" _r(unsigned long long x) {
    return boost::real::real<int>(std::to_string(x));
}

inline auto operator "" _r(const char* x, size_t len) {
    return boost::real::real<int>(x);
}

// User Defined Literals for Rational Number

inline auto operator ""_rational(const char* x, size_t len){
    return boost::real::real<int>(x, "rational");
}

inline auto operator ""_rational(long double x){
    return boost::real::real<int>(std::to_string(x), "rational");
}

inline auto operator ""_rational(unsigned long long x){
    return boost::real::real<int>(std::to_string(x), "rational");
}

// User Defined Literal for Integer Number

inline auto operator ""_integer(const char* x, size_t len){
    return boost::real::real<int>(x, "integer");
}

inline auto operator ""_integer(unsigned long long x){
    return boost::real::real<int>(std::to_string(x), "integer");
}

#endif //BOOST_REAL_HPP
