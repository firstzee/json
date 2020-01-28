#include <deserialize/traits.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/value_at.hpp>

#include <type_traits>

namespace deserialize::details {

using namespace boost::property_tree;
using namespace boost::fusion;

template <typename S>
void get_sequence(S&, const ptree&);

template <class S>
void get(S& s, const ptree& pt, const std::string& node_name,
        traits::enable_if_t<
            std::is_floating_point_v<S> ||
            std::is_integral_v<S> ||
            traits::is_string_v<S>
        >* = 0
) {
    s = pt.get<S>(node_name);
}

template <class S>
void get(S& s, const ptree& pt, const std::string& node_name,
        traits::enable_if_t<boost::fusion::traits::is_sequence<S>::value>* = 0) {
    get_sequence(
        s,
        pt.get_child(node_name)
    );
}

template <class S>
void get(S& s, const ptree& pt, const std::string& node_name,
        traits::enable_if_t<traits::is_vector_v<S>>* = 0) {
    const auto& node = pt.get_child(node_name);
    s.reserve(node.size());
    for (const auto& v: node) {
        typename S::value_type value;
        get(value, v.second, "");
        s.emplace_back(std::move(value));
    }
}

template <class S>
void get(S& s, const ptree& pt, const std::string& node_name,
        traits::enable_if_t<traits::is_map_v<S>>* = 0) {
    for (const auto& v: pt.get_child(node_name)) {
        typename S::mapped_type value;
        get(value, v.second, "");
        s.emplace(v.first, std::move(value));
    }
}

template <class S>
void get(S& s, const ptree& pt, const std::string& node_name,
        traits::enable_if_t<traits::is_optional_v<S>>* = 0) {
    try {
        s = typename S::value_type {};
        get(*s, pt, node_name);
    } catch (const ptree_error&) {
    }
}

template <typename S, std::size_t... Is>
void get(S& s, const ptree& pt, std::index_sequence<Is...>) {
    (get(at_c<Is>(s), pt, extension::struct_member_name<S, Is>::call()), ...);
}

template <typename S>
void get_sequence(S& s, const ptree& pt) {
    get(s, pt, std::make_index_sequence<boost::fusion::result_of::size<S>::type::value>{});
}

}
