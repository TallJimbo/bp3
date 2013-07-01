#include "bp3/Module.hpp"
#include "bp3/FromPython.hpp"

#include <iostream>
#include <string>

/*
 *  Unit test for FromPython converters.
 *
 *  We wrap the example classes using Python's built-in Capsule objects
 *  so as not to get conversion mixed up with wrapping at this stage.
 */

namespace {

class Example1 {
public:

    static bp3::PyPtr make(std::string const & value) {
        Example1 * x = new Example1(value);
        return bp3::PyPtr::steal(PyCapsule_New(x, "Example1", &destroy));
    }

    static int check1(bp3::PyPtr const & ptr, bp3::ConverterData &) {
        if (PyCapsule_IsValid(ptr.get(), "Example1")) return 0;
        return -1;
    }

    static void * convert1(bp3::PyPtr const & ptr, bp3::ConverterData &) {
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

static std::shared_ptr<bp3::Module> mod;

template <typename T>
static PyObject * check_rv(PyObject * self, PyObject * arg) {
    bp3::PyPtr py1 = bp3::PyPtr::borrow(arg);
    assert(mod);
    bp3::FromPython<T> converter(*mod, py1);
    if (!converter.isConvertible()) {
        Py_RETURN_FALSE;
    }
    T ex1 = converter.convert();
    if (ex1.value != "ex1") {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

template <typename T>
static PyObject * check_ptr(PyObject * self, PyObject * arg) {
    bp3::PyPtr py1 = bp3::PyPtr::borrow(arg);
    assert(mod);
    bp3::FromPython<T> converter(*mod, py1);
    if (!converter.isConvertible()) {
        Py_RETURN_FALSE;
    }
    T ex1 = converter.convert();
    if (ex1->value != "ex1") {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

} // namespace

static PyMethodDef methods[] = {
    {"check_val", &check_rv<Example1>, METH_O, nullptr},
    {"check_cval", &check_rv<Example1 const>, METH_O, nullptr},
    {"check_ref", &check_rv<Example1 &>, METH_O, nullptr},
    {"check_cref", &check_rv<Example1 const &>, METH_O, nullptr},
    {"check_ptr", &check_ptr<Example1*>, METH_O, nullptr},
    {"check_cptr", &check_ptr<Example1 const *>, METH_O, nullptr},
    {"check_ptrc", &check_ptr<Example1 * const>, METH_O, nullptr},
    {"check_cptrc", &check_ptr<Example1 const * const>, METH_O, nullptr},
    {nullptr, nullptr, 0, nullptr}
};

void wrap(bp3::Module & mod_) {
    std::cerr << "checkpoint1\n";
    mod.reset(new bp3::Module(mod_));
    std::cerr << "checkpoint2\n";
    mod_.register_from_python(
        bp3::makeTypeInfo<Example1>(), true, &Example1::check1, &Example1::convert1
    );
    std::cerr << "checkpoint3\n";
    mod_.add("py1", Example1::make("ex1"));
    std::cerr << "checkpoint4\n";
}

BP3_MODULE(from_python_mod, wrap, methods)
