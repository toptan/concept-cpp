///
/// @author  Thomas Lehmann
/// @file    matcher.h
/// @brief   matcher base class and functions.
///
/// Copyright (c) 2015 Thomas Lehmann
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
/// documentation files (the "Software"), to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
/// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all copies
/// or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
/// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY,
/// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef INCLUDE_ASSERT_H_
#define INCLUDE_ASSERT_H_

#include <matcher/matcher.h>

#include <string.h>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <typeinfo>

namespace unittest {

/// @class assertion
/// @brief exception representing a failed test
class assertion : public std::exception {
    public:
        /// providing assertion message
        assertion(const std::string& message)
            : m_message(message) {}

        /// @return assertion message
        virtual const char* what() const noexcept override {
            return m_message.c_str();
        }

    private:
        /// message explaining the assertion
        const std::string m_message;
};

/// @struct assert_that_statistic
/// @brief global counter for success and failure
struct assert_that_statistic {
    /// @return read/write access to success counter
    static uint64_t& get_succeeded() noexcept {
        static uint64_t counter = 0;
        return counter;
    }

    /// @return read/write access to failed counter
    static uint64_t& get_failed() noexcept {
        static uint64_t counter = 0;
        return counter;
    }

    /// @return total number of assert calls.
    static uint64_t total() noexcept {
        return get_succeeded() + get_failed();
    }
};

template <typename T>
void assert_that(const T& expected_value, const matcher::matcher<T>& value_matcher) {
    if (!value_matcher.check(expected_value)) {
        ++assert_that_statistic::get_failed();
        std::stringstream message;
        message << expected_value
                << " does not match "
                << value_matcher.get_expression();
        throw assertion(message.str());
    }
    ++assert_that_statistic::get_succeeded();
}

template <typename T>
void assert_raise(const std::string& message,
                  std::function<void ()> function) {
    try {
        function();

        std::stringstream assertion_message;
        assertion_message << message
                          << " - expected exceptiont type: "
                          << typeid(T).name()
                          << " - no exception thrown!";
        throw assertion(assertion_message.str());
    } catch (T&) {
        ++assert_that_statistic::get_succeeded();
        // all is fine
    } catch (const assertion& e) {
        // not fine but we do not want to catch HERE. So we re-throw ...
        ++assert_that_statistic::get_failed();
        throw e;
    } catch (...) {
        ++assert_that_statistic::get_failed();
        std::stringstream assertion_message;
        assertion_message << message
                          << " - expected exceptiont type: "
                          << typeid(T).name()
                          << " - wrong exception thrown!";
        throw assertion(assertion_message.str());
    }
}


    
}  // namespace unittest

#endif  // INCLUDE_ASSERT_H_
