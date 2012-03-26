#include "bp3/testutils.hpp"
#include "bp3/keywords.hpp"

namespace {

template <std::size_t N>
PyObject * test_parser(
    PyObject * self, PyObject * args, PyObject * kwds
) {
    auto key_pack = bp3::kwds("a", "b", bp3::arg("c")=2, bp3::arg("d")=3);
    std::array<bp3::py_ptr,N> output;
    try {
        key_pack.parse("example()", bp3::py_ptr::borrow(args), bp3::py_ptr::borrow(kwds), output);
    } catch (bp3::builtin::TypeError & err) {
        return err.release();
    }
    int offset = N - 4;
    if (!output[2 + offset]) output[2 + offset] = bp3::py_ptr::steal(PyLong_FromLong(2));
    if (!output[3 + offset]) output[3 + offset] = bp3::py_ptr::steal(PyLong_FromLong(3));
    PyObject * r = PyTuple_New(N);
    for (Py_ssize_t i = 0, n = N; i < n; ++i) {
        PyTuple_SET_ITEM(r, i, output[i].release());
    }
    return r;
}

template PyObject * test_parser<4>(PyObject *, PyObject *, PyObject *);
template PyObject * test_parser<5>(PyObject *, PyObject *, PyObject *);

class suite : public bp3::test_suite<suite> {
public:

    suite() : bp3::test_suite<suite>("keywords_tests") {}

    void test() {
        BP3_TEST_PYTHON(
            "def do_test(func, expected, *args, **kwds):\n"
            "    t = func(*args, **kwds)\n"
            "    return t == tuple(expected)\n"
            "results[0] = do_test(keywords_tests.test_parser_4, range(4), 0, 1, 2, 3)\n"
            "results[1] = do_test(keywords_tests.test_parser_4, range(4), d=3, a=0, b=1)\n"
            "results[2] = do_test(keywords_tests.test_parser_4, range(4), 0, 1, c=2)\n"
            "results[3] = do_test(keywords_tests.test_parser_5, range(-1,4), -1, 0, 1, 2, 3)\n"
            "results[4] = do_test(keywords_tests.test_parser_5, range(-1,4), -1, d=3, a=0, b=1)\n"
            "results[5] = do_test(keywords_tests.test_parser_5, range(-1,4), -1, 0, 1, c=2, d=3)\n"
        );
    }

    static PyMethodDef methods[];

};

PyMethodDef suite::methods[] = {
    {"test_parser_4", (PyCFunction)test_parser<4>, METH_KEYWORDS | METH_VARARGS,
     "same number of keywords as signature args"},
    {"test_parser_5", (PyCFunction)test_parser<5>, METH_KEYWORDS | METH_VARARGS,
     "fewer keywords than signature args"},
    {0, 0, 0, 0}
};

} // namespace

int main() {
    return !suite::run({&suite::test});
}
