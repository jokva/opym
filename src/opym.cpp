#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/IOConfig/RestartConfig.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Well.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>

namespace py = boost::python;
namespace conv = py::converter;
using namespace Opm;

namespace {

EclipseState (*parse_from_file)(const std::string&, const ParseContext&) = &Parser::parse;
const RestartConfig& (EclipseState::*getrst)() const = &EclipseState::getRestartConfig;

struct iterable_converter {

    template< typename Container >
    iterable_converter& from_python() {
        conv::registry::push_back( &iterable_converter::convertible,
                                   &iterable_converter::construct< Container >,
                                   py::type_id< Container >() );
        return *this;
    }

    static void* convertible( PyObject* object ) {
        return PyObject_GetIter( object ) ? object : nullptr;
    }

    template< typename Container >
    static void construct( PyObject* object,
                           conv::rvalue_from_python_stage1_data* data ) {

        using storage_type = conv::rvalue_from_python_storage< Container >;
        using iterator = py::stl_input_iterator<typename Container::value_type>;


        py::handle<> handle( py::borrowed( object ) );
        void* storage = reinterpret_cast< storage_type* >( data )->storage.bytes;
        new (storage) Container( iterator( py::object( handle ) ), iterator() );
        data->convertible = storage;
    }
};

struct pair_converter {

    template< typename T, typename U >
    pair_converter& from_python() {

        conv::registry::push_back( &pair_converter::convertible,
                                   &pair_converter::construct< T, U >,
                                   py::type_id< std::pair< T, U > >() );

        return *this;
    }

    static void* convertible( PyObject* object ) {
        if( !PySequence_Check( object ) || PySequence_Size( object ) != 2 )
            return nullptr;

        return object;
    }

    template< typename T, typename U >
    static void construct( PyObject* object,
                           conv::rvalue_from_python_stage1_data* data ) {

        using pair = std::pair< T, U >;
        using storage_type = conv::rvalue_from_python_storage< pair >;

        void* storage = reinterpret_cast< storage_type* >( data )->storage.bytes;
        new (storage) pair (
                py::extract< T >( PySequence_GetItem( object, 0 ) )(),
                py::extract< U >( PySequence_GetItem( object, 1 ) )()
        );
        data->convertible = storage;
    }
};

template< typename T >
struct vector_to_pylist {
    static PyObject* convert( const std::vector< T >& vec ) {
        py::object getiter = py::iterator< std::vector< T > >();
        return py::list{ getiter( vec ) }.ptr();
    }
};

std::vector< Well > get_wells( const Schedule& sch ) {
    std::vector< Well > wells;
    for( const auto& w : sch.getWells() )
        wells.push_back( *w );

    return wells;
}

}

BOOST_PYTHON_MODULE(opym) {

py::def( "parse",        parse_from_file );
py::def( "parse_string", parse_from_string );

/* ParseContext support */
py::enum_< InputError::Action >( "InputErrorAction" )
    .value( "throw",  InputError::Action::THROW_EXCEPTION )
    .value( "warn",   InputError::Action::WARN )
    .value( "ignore", InputError::Action::IGNORE )
    ;

using ctxact = std::pair< std::string, InputError::Action >;
py::class_< std::vector< ctxact > >( "ContextCfgVector" )
    .def( py::vector_indexing_suite< std::vector< ctxact > >() )
    ;

iterable_converter()
    .from_python< std::vector< ctxact > >()
    ;

pair_converter()
    .from_python< std::string, InputError::Action >()
    ;

py::class_< ParseContext >( "ParseContext" )
    .def( py::init<>() )
    .def( py::init< std::vector< ctxact > >() )
    ;

py::implicitly_convertible< std::vector< ctxact >, ParseContext >();

/* EclipseState support */
py::class_< EclipseState >( "EclipseState", py::no_init )
    .add_property( "title", &EclipseState::getTitle )
    .add_property( "restart", py::make_function(
                                getrst,
                                py::return_internal_reference<>() ) )
    .add_property( "schedule", py::make_function(
                               &EclipseState::getSchedule,
                               py::return_internal_reference<>() ) )
    .add_property( "e3d", py::make_function(
                            &EclipseState::get3DProperties,
                            py::return_internal_reference<>() ) )
    ;

py::class_< Eclipse3DProperties >( "E3D", py::no_init )
    .def( "__contains__", &Eclipse3DProperties::supportsGridProperty )
    ;

/* pretty ghetto; should be replaced (and augmented) by a proper date conv */
py::class_< boost::gregorian::date >( "Date" )
    .def("__str__", &boost::gregorian::to_iso_extended_string )
    ;

/* RestartConfig support */
py::class_< RestartConfig >( "RestartConfig", py::no_init )
    .add_property( "first_step", &RestartConfig::getFirstRestartStep )
    .def( "write", &RestartConfig::getWriteRestartFile )
    .def( "date", &RestartConfig::getTimestepDate )
    ;

/* schedule support */
size_t (Schedule::*num_wells)() const = &Schedule::numWells;
size_t (Schedule::*num_wells_ts)(size_t) const = &Schedule::numWells;

py::class_< Schedule >( "Schedule", py::no_init )
    .def( "num_wells", num_wells )
    .def( "num_wells", num_wells_ts )
    .def( "has_well", &Schedule::hasWell )
    .def( "__contains__", &Schedule::hasWell )
    .def( "get_well", &Schedule::getWell, py::return_internal_reference<>() )
    .def( "__getitem__", &Schedule::getWell, py::return_internal_reference<>() )
    .add_property( "wells", get_wells )
    ;

py::class_< Well >( "Well", py::no_init )
    .add_property( "name", py::make_function(
                            &Well::name,
                            py::return_value_policy< py::copy_const_reference >() ) )
    .add_property( "I", &Well::getHeadI )
    .add_property( "J", &Well::getHeadJ )
    .add_property( "allow_crossflow", &Well::getAllowCrossFlow )
    ;

py::class_< std::vector< Well > >( "WellVec" )
    .def( py::vector_indexing_suite< std::vector< Well > >() )
    ;

}
