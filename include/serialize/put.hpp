#include <common/traits.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/value_at.hpp>

#include <type_traits>

namespace serialize::details {

using namespace boost::property_tree;
using namespace boost::fusion;
using namespace json::traits;

template <class S>
void put(const S&, ptree&, const std::string&,
        enable_if_t<
            std::is_floating_point_v<S> ||
            std::is_integral_v<S> ||
            is_string_v<S>
        >* = 0
);

template <class S>
void put(const S&, ptree&, const std::string&,
        enable_if_t<traits::is_sequence<S>::value>* = 0);

template <class S>
void put(const S&, ptree&, const std::string&,
        enable_if_t<is_vector_v<S>>* = 0);

template <class S>
void put(const S&, ptree&, const std::string&,
        enable_if_t<is_map_v<S>>* = 0);

template <class S>
void put(const S&, ptree&, const std::string&,
        enable_if_t<is_optional_v<S>>* = 0);

template <typename S>
void put_sequence(const S&, ptree&);

template <class S>
void put(const S& s, ptree& pt, const std::string& node_name,
        enable_if_t<
            std::is_floating_point_v<S> ||
            std::is_integral_v<S> ||
            is_string_v<S>
        >*
) {
    pt.put(node_name, s);
}

template <class S>
void put(const S& s, ptree& pt, const std::string& node_name,
        enable_if_t<traits::is_sequence<S>::value>*) {
    ptree child;
    put_sequence(
        s,
        child
    );
    pt.add_child(node_name, child);
}

template <class S>
void put(const S& s, ptree& pt, const std::string& node_name,
        enable_if_t<is_vector_v<S>>*) {
    ptree array;
    for (const auto& v: s) {
        ptree el;
        put(v, el, "");
        array.push_back(std::make_pair("", el));
    }
    pt.add_child(node_name, array);
}

template <class S>
void put(const S& s, ptree& pt, const std::string& node_name,
        enable_if_t<is_map_v<S>>*) {
    ptree map;
    for (const auto& v: s) {
        put(v.second, map, v.first);
    }
    pt.add_child(node_name, map);
}

template <class S>
void put(const S& s, ptree& pt, const std::string& node_name,
        enable_if_t<is_optional_v<S>>*) {
    if (s) {
       put(*s, pt, node_name);
    }
}

template <typename S, std::size_t... Is>
void put(const S& s, ptree& pt, std::index_sequence<Is...>) {
    (put(at_c<Is>(s), pt, extension::struct_member_name<S, Is>::call()), ...);
}

template <typename S>
void put_sequence(const S& s, ptree& pt) {
    put(s, pt, std::make_index_sequence<result_of::size<S>::type::value>{});
}

}
