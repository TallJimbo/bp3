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

static bool test1(bp3::context_t const & context) {
    bp3::py_ptr py1 = Example1::make("ex1");
    {
        bp3::conversion::from_python<Example1> converter(context, py1);
        if (!converter.is_convertible()) return false;
        Example1 ex1 = converter.convert();
        if (ex1.value != "ex1") return false;
    }
    {
        bp3::conversion::from_python<Example1 const> converter(context, py1);
        if (!converter.is_convertible()) return false;
        Example1 ex1 = converter.convert();
        if (ex1.value != "ex1") return false;
    }
    {
        bp3::conversion::from_python<Example1 &> converter(context, py1);
        if (!converter.is_convertible()) return false;
        Example1 & ex1 = converter.convert();
        if (ex1.value != "ex1") return false;
    }
    {
        bp3::conversion::from_python<Example1 const &> converter(context, py1);
        if (!converter.is_convertible()) return false;
        Example1 const & ex1 = converter.convert();
        if (ex1.value != "ex1") return false;
    }
    {
        bp3::conversion::from_python<Example1 *> converter(context, py1);
        if (!converter.is_convertible()) return false;
        Example1 * ex1 = converter.convert();
        if (ex1->value != "ex1") return false;
    }
    {
        bp3::conversion::from_python<Example1 const *> converter(context, py1);
        if (!converter.is_convertible()) return false;
        Example1 const * ex1 = converter.convert();
        if (ex1->value != "ex1") return false;
    }
    {
        bp3::conversion::from_python<Example1 * const> converter(context, py1);
        if (!converter.is_convertible()) return false;
        Example1 * ex1 = converter.convert();
        if (ex1->value != "ex1") return false;
    }
    {
        bp3::conversion::from_python<Example1 const * const> converter(context, py1);
        if (!converter.is_convertible()) return false;
        Example1 const * ex1 = converter.convert();
        if (ex1->value != "ex1") return false;
    }
    return true;
}

static PyMethodDef methods[] = {
    {0, 0, 0, 0}
};

#if PY_MAJOR_VERSION != 2
static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "from_python_tests",
    0,
    0,
    methods
};
static PyObject * PyInit_from_python_tests(void) {
    return PyModule_Create(&module);
}

#endif

} // namespace

int main() {
    int result = true;
#if PY_MAJOR_VERSION == 2
    Py_Initialize();
    Py_InitModule("from_python_tests", methods);
#else
    PyImport_AppendInittab("from_python_tests", &PyInit_from_python_tests);
    Py_Initialize();
#endif
    bp3::context_t context;
    context.register_from_python(bp3::type_id<Example1>(), true, &Example1::check1, &Example1::convert1);
    result = result && test1(context);
    Py_Finalize();
    return !result;
}
