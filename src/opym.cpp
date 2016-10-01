#include <boost/python.hpp>

#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>

namespace py = boost::python;
using namespace Opm;

BOOST_PYTHON_MODULE(opym) {

    py::class_< EclipseState >( "EclipseState", py::no_init )
        ;
}
