#include "bp3/testutils.hpp"
#include "bp3/context.hpp"
#include "bp3/conversion/from_python.hpp"

#include <string>

/*
 *  Unit test for from_python converters.
 *
 *  We wrap the example classes using Python's built-in Capsule objects
 *  so as not to get conversion mixed up with wrapping at this stage.
 */

namespace {

class Example1 {
public:

    static bp3::py_ptr make(std::string const & value) {
        Example1 * x = new Example1(value);
        return bp3::py_ptr::steal(PyCapsule_New(x, "Example1", &destroy));
    }

    static int check1(bp3::py_ptr const & ptr, bp3::converter_data &) {
        if (PyCapsule_IsValid(ptr.get(), "Example1")) return 0;
        return -1;
    }

    static void * convert1(bp3::py_ptr const & ptr, bp3::converter_data &) {
        return PyCapsule_GetPointer(ptr.get(), "Example1");
    }

    explicit Example1(std::string const & x) : value(x) {}

    std::string value;

private:
    static void destroy(PyObject * capsule) {
        Example1 * p = reinterpret_cast<Example1*>(PyCapsule_GetPointer(capsule, "Example1"));
        delete p;
    }
};

class suite1 : public bp3::test_suite<suite1> {
public:

    suite1() : bp3::test_suite<suite1>("from_python_tests_1"), context(), py1(Example1::make("ex1")) {
        context.register_from_python(
            bp3::type_id<Example1>(), true, &Example1::check1, &Example1::convert1
        );
    }

    template <typename T>
    void test_rv() {
        bp3::conversion::from_python<T> converter(context, py1);
        BP3_TEST(converter.is_convertible());
        T ex1 = converter.convert();
        BP3_TEST(ex1.value == "ex1");
    }

    template <typename T>
    void test_ptr() {
        bp3::conversion::from_python<T> converter(context, py1);
        BP3_TEST(converter.is_convertible());
        T ex1 = converter.convert();
        BP3_TEST(ex1->value == "ex1");
    }

    bp3::context_t context;
    bp3::py_ptr py1;

};

} // namespace

int main() {
    return !suite1::run({
        &suite1::test_rv<Example1>, 
        &suite1::test_rv<Example1 const>,
        &suite1::test_rv<Example1 &>,
        &suite1::test_rv<Example1 const &>,
        &suite1::test_ptr<Example1 *>,
        &suite1::test_ptr<Example1 * const>,
        &suite1::test_ptr<Example1 const *>,
        &suite1::test_ptr<Example1 const * const>
    });
}
