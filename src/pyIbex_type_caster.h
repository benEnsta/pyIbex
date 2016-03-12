#include "ibex_Vector.h"
#include "ibex_Array.h"
#include "ibex_Ctc.h"
#include <pybind11/pybind11.h>


NAMESPACE_BEGIN(pybind11)
NAMESPACE_BEGIN(detail)

template <> struct type_caster<ibex::Vector> {
    typedef ibex::Vector type;
    typedef type_caster<double> value_conv;
public:

    type_caster<ibex::Vector>() :  value(0) { }
    bool load(PyObject *src, bool convert) {
        if (!PyList_Check(src))
            return false;
        size_t size = (size_t) PyList_GET_SIZE(src);
        // value.reserve(size);
        // value.clear();
        value_conv conv;
        double *tmp = new double[size];
        for (size_t i=0; i<size; ++i) {
            if (!conv.load(PyList_GetItem(src, (ssize_t) i), convert))
                return false;
            tmp[i] = (double) conv;
            // value.push_back((Value) conv);
        }
        value = ibex::Vector(size, tmp);
        delete tmp;
        return true;
    }

    static handle cast(const ibex::Vector &src, return_value_policy policy, handle parent) {
        list l(src.size());
        for (size_t i = 0; i < src.size(); i++){
            object value_ = object(value_conv::cast(src[i], policy, parent), false);
            if (!value_) {
                return handle();
            }
            PyList_SET_ITEM(l.ptr(), i, value_.release().ptr()); // steals a reference
        }
        return l.release();
    }
    PYBIND11_TYPE_CASTER(type, _("list<") + value_conv::name() + _(">"));
    // PYBIND11_TYPE_CASTER(ibex::Vector, detail::descr("list<") + value_conv::name() + detail::descr(">"));
};

template <typename Value> struct type_caster<ibex::Array<Value> > {
    typedef ibex::Array<Value> type;
    typedef type_caster<Value> value_conv;
public:
    // Vector_caster() :  value(0) { }
    bool load(PyObject *src, bool convert) {
        list l(src, true);
        if (!l.check())
            return false;
        size_t size = (size_t) PyList_GET_SIZE(src);
        value.resize(size);
        value_conv conv;
        for (size_t i=0; i<size; ++i) {
            if (!conv.load(PyList_GetItem(src, (ssize_t) i), convert))
                return false;
            value.set_ref(i, *(Value*) conv);
        }
        return true;
    }

    static handle cast(const ibex::Array<Value> &src, return_value_policy policy, handle parent) {
        // PyObject *list = PyList_New(src.size());
        list l(src.size());
        // size_t index = 0;
        // for (size_t i = 0; i < src.size(); i++){
        // // for (auto const &value: src) {
        //     object value_ = object(value_conv::cast(src[i], policy, parent), false);

        //     if (!value_) {
        //         Py_DECREF(list);
        //         return nullptr;
        //     }
        //     PyList_SET_ITEM(list, i, value_.release().ptr()); // steals a reference
        // }
        return l.release();
    }
    PYBIND11_TYPE_CASTER(type, _("list<") + value_conv::name() + _(">"));
    // PYBIND11_TYPE_CASTER(ibex::Vector, detail::descr("list<") + value_conv::name() + detail::descr(">"));
};

NAMESPACE_END(detail)
NAMESPACE_END(pybind11)