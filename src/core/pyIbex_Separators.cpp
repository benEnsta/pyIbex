//============================================================================
//                                  I B E X
// File        : pyIbex_Separator.cpp
// Author      : Benoit Desrochers
// Copyright   : Ecole des Mines de Nantes (France)
// License     : See the LICENSE file
// Created     : Mar 02, 2015
//============================================================================


#include "ibex_IntervalVector.h"
#include "ibex_Sep.h"
#include "ibex_SepUnion.h"
#include "ibex_SepInter.h"
#include "ibex_SepNot.h"
#include "ibex_SepCtcPair.h"
#include "ibex_SepFwdBwd.h"
#include "ibex_SepBoundaryCtc.h"
#include "ibex_SepInverse.h"
#include "ibex_SepTransform.h"
#include "ibex_SepQInter.h"

#include "ibex_SepPolygon.h"

#include "ibex_SepPolarXY.h"
#include "ibex_SepPolarXYT.h"


#include "ibex_SepProj.h"

#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include "pyIbex_to_python_converter.h"

#include "ibex_SepCtcPairProj.h"

using namespace boost;
using namespace boost::python;
using namespace ibex;
namespace py = boost::python;


typedef boost::shared_ptr<ibex::SepUnion> SepUnionPtr;
typedef boost::shared_ptr<ibex::SepInter> SepInterPtr;


struct SepWrap : Sep, wrapper<Sep> {
    void separate(IntervalVector& xin, IntervalVector& xout){
        this->get_override("separate")(xin, xout);
    }
};
typedef boost::shared_ptr<Sep> SepPtr;

SepUnionPtr __or(Sep& c1, Sep& c2){
    Array<Sep> lst = Array<Sep>(c1,c2);
    return SepUnionPtr(new SepUnion(lst));
}

SepInterPtr __and(Sep& c1, Sep& c2){
    Array<Sep> lst = Array<Sep>(c1,c2);
    return SepInterPtr(new SepInter(lst));
}

template<typename SepType>
boost::shared_ptr<SepType> ctcFromList(const py::list & lst)
{
    // construct with a list here
    ibex::Array<Sep> list(len(lst));
    for(uint i = 0; i < len(lst); i++){
        extract<Sep> get_Sep(lst[i]);
        if (get_Sep.check()){
            Sep* C = extract<Sep*>(lst[i]);
            list.set_ref(i, *C);
        } else {
            std::cout << "Extraction Error \n";
            return boost::shared_ptr<SepType>();
        }
    }
    return boost::shared_ptr<SepType>(new SepType(list));
}


boost::shared_ptr<SepPolygon> initFromList(const py::list& lst){
    // check if the list contains two sub-list [[ x1, x2, ..., xn], [ y1, y2, ..., yn]]   

    boost::python::ssize_t n = boost::python::len(lst);
    if(n != 2){
        std::cout << "Invalide input argmment: expected [[ x1, x2, ..., xn], [ y1, y2, ..., yn]]\n";
        throw;
    }
    std::vector<double> lx = to_std_vector<double>(lst[0]);
    std::vector<double> ly = to_std_vector<double>(lst[1]);
    n = lx.size();
    std::vector<double> ax(n), ay(n),bx(n),by(n);
    for(int i = 0; i < n; i++){
        ax[i] = lx[i];
        ay[i] = ly[i];
        bx[i] = lx[ (i+1)%n ];
        by[i] = ly[ (i+1)%n ];
    }
    return boost::shared_ptr<SepPolygon>(new SepPolygon(ax, ay, bx, by));
}

//boost::shared_ptr<SepProj> sepProjFromList(Sep& sep, const py::list& lst, double prec){
//    return boost::shared_ptr<SepProj>(new SepProj(sep, to_std_vector<bool>(lst),prec));
//}


#define GEN_CLASS_WITH_SAVED_ARGUMENTS(class_name,new_class_name,  arg1, arg2, arg3)           \
    class new_class_name: public class_name {                          \
    public:                                                                     \
        new_class_name(boost::shared_ptr<arg1> ptr_arg1,                \
                             boost::shared_ptr<arg2> ptr_arg2,                \
                             boost::shared_ptr<arg3> ptr_arg3):               \
        class_name(*ptr_arg1, *ptr_arg2, *ptr_arg3),                          \
        ptr_arg1(ptr_arg1), ptr_arg2(ptr_arg2), ptr_arg3(ptr_arg3){}           \
                                                                                \
    private:                                                                    \
        boost::shared_ptr<arg1> ptr_arg1;                                       \
        boost::shared_ptr<arg2> ptr_arg2;                                       \
        boost::shared_ptr<arg3> ptr_arg3;                                       \
    };\




namespace ibex {

GEN_CLASS_WITH_SAVED_ARGUMENTS(SepTransform,SepTransforWrapper, Sep, Function, Function)

//class SepTransforWrapper : public SepTransform {
//public:
//    SepTransforWrapper(SepPtr sep, boost::shared_ptr<Function> f1, boost::shared_ptr<Function> f2):
//        SepTransform(*sep, *f1, *f2),
//        sep(sep), f1(f1), f2(f2){}

//private:
//    SepPtr sep;
//    boost::shared_ptr<Function> f1;
//    boost::shared_ptr<Function> f2;

//};




}
void export_Separators(){

    typedef void (Sep::*separate) (IntervalVector&, IntervalVector&);
    class_<SepWrap, boost::noncopyable>("Sep", no_init)
        .def("separate", pure_virtual( separate(&Sep::separate)))
        .def( "__or__", &__or)
    	.def( "__and__", &__and)
    ;

    class_<SepUnion, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepUnion> >("SepUnion", no_init)
    	.def("__init__", make_constructor(ctcFromList<SepUnion>), "SepUnion from list of separators\n Usage : SepUnion([s1, s2, ...])")
    	.def("separate", &SepUnion::separate)
		;

    class_<SepInter, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepInter> >
        ("SepInter", no_init)
    	.def("__init__", make_constructor(ctcFromList<SepInter>), "SepInter from list of separators\n Usage : SepInter([s1, s2, ...])")
    	.def("separate", &SepInter::separate)
    	;

    class_<SepQInterProjF, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepQInterProjF> >
        ("SepQInterProjF", no_init)
        .def("__init__", make_constructor(ctcFromList<SepQInterProjF>), "SepQInterProjF from list of separators\n Usage : SepQInterProjF([s1, s2, ...])")
        .def("separate", &SepQInterProjF::separate)
        .add_property("q", &SepQInterProjF::getq, &SepQInterProjF::setq)
        ;

    class_<SepCtcPair, bases<Sep>,  boost::noncopyable, boost::shared_ptr<ibex::SepCtcPair> >
            ("SepCtcPair", init< Ctc&, Ctc& >())
            .def("separate", &SepCtcPair::separate)
            ;


    class_<SepFwdBwd, bases<Sep>,  boost::noncopyable, boost::shared_ptr<ibex::SepFwdBwd> >
            ("SepFwdBwd", init< Function&, CmpOp >())
            .def(init<Function&, Interval& >())
            .def(init<Function&, IntervalVector& > ())
//            .def(init<Function&, IntervalMatrix& > ())
            .def("separate", &SepFwdBwd::separate)
            ;

    class_<SepNot, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepNot> >("SepNot", no_init)
        .def(init<Sep&>())
    	.def("separate", &SepNot::separate);

    class_<SepInverse, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepInverse> >("SepInverse", no_init)
        .def(init<Sep&, Function& >())
        .def("separate", &SepInverse::separate);

    // class_<SepTransform, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepTransform> >("SepTransform", no_init)
    //     .def(init<Sep&, Function& , Function& >())
    //     .def("separate", &SepTransform::separate);

    class_<SepTransforWrapper, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepTransforWrapper> >("SepTransform", no_init)
        .def(init<SepPtr, boost::shared_ptr<Function> , boost::shared_ptr<Function> >())
        .def("separate", &SepTransforWrapper::separate);
        

    class_<SepPolygon, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepPolygon> >("SepPolygon", no_init)
        .def("__init__", make_constructor(initFromList))
        .def("separate", &SepPolygon::separate);


    class_<SepPolarXY, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepPolarXY> >("SepPolarXY", no_init)
        .def(init<Interval, Interval>())
        .def("separate", &SepPolarXY::separate);

    class_<SepPolarXYT, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepPolarXYT> >("SepPolarXYT", no_init)
        .def(init<Interval, Interval, double , double>())
        .def("separate", &SepPolarXYT::separate);

    class_<SepProj, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepProj> >("SepProj", no_init)
        .def(init<Sep&, const IntervalVector&, double, int>())
        .def("separate", &SepProj::separate);

    class_<SepCtcPairProj, bases<Sep>, boost::noncopyable, boost::shared_ptr<ibex::SepCtcPairProj> >("SepCtcPairProj", no_init)
        .def(init<Ctc&, Ctc&, const IntervalVector&, double>())
        .def("separate", &SepCtcPairProj::separate);

}
