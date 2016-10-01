#include <algorithm>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>

namespace py = boost::python;
namespace conv = py::converter;
using namespace Opm;

namespace {

EclipseState (*parse_from_string)(const std::string&, const ParseContext&) = &Parser::parse;
EclipseState (*parse_from_file)  (const std::string&, const ParseContext&) = &Parser::parseData;

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

py::class_< EclipseState >( "EclipseState", py::no_init )
    ;
}
