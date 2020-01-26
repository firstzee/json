#include "traits.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/value_at.hpp>

#include <optional>
#include <type_traits>

namespace deserialize::details {

using namespace boost::property_tree;
using namespace boost::fusion;

template <typename S>
void get_sequence(S&, const ptree&);

template <class S, class T = void>
struct Get {
    static void get(S&, const ptree&, const std::string&) {
        static_assert(std::is_integral_v<S>, "Not implemented yet");
    }
};

template <class S>
struct Get<S, typename std::enable_if<
        std::is_floating_point_v<S> ||
        std::is_integral_v<S> ||
        traits::is_string_v<S>
>::type> {
    static void get(S& s, const ptree& pt, const std::string& node_name) {
        s = pt.get<S>(node_name);
    }
};

template <class S>
struct Get<S, typename std::enable_if<
        boost::fusion::traits::is_sequence<S>::value
>::type> {
    static void get(S& s, const ptree& pt, const std::string& node_name) {
        get_sequence(
            s,
            pt.get_child(node_name)
        );
    }
};

template <class S>
struct Get<S, typename std::enable_if<
        traits::is_vector_v<S>
>::type> {
    static void get(S& s, const ptree& pt, const std::string& node_name) {
        auto& node = pt.get_child(node_name);
        s.reserve(node.size());
        for(const auto& v: node) {
            typename S::value_type value;
            Get<typename S::value_type>::get(value, v.second, "");
            s.emplace_back(std::move(value));
        }
    }
};

template <class S>
struct Get<S, typename std::enable_if<
        traits::is_map_v<S>
>::type> {
    static void get(S& s, const ptree& pt, const std::string& node_name) {
        for(const auto& v: pt.get_child(node_name)) {
            typename S::mapped_type value;
            Get<typename S::mapped_type>::get(value, v.second, "");
            s.emplace(v.first, std::move(value));
        }
    }
};

template <class S>
struct Get<std::optional<S>> {
    static void get(std::optional<S>& s, const ptree& pt, const std::string& node_name) {
        try {
            s = S {};
            Get<S>::get(*s, pt, node_name);
        } catch(const ptree_error&) {
        }
    }
};

template <typename S, std::size_t I>
void get(S& s, const ptree& pt) {
    Get<typename result_of::value_at<S, boost::mpl::int_<I>>::type>::get(
        at_c<I>(s), pt, extension::struct_member_name<S, I>::call());
}

template <typename S, std::size_t... Is>
void get(S& s, const ptree& pt, std::index_sequence<Is...>) {
    (get<S, Is>(s, pt), ...);
}

template <typename S>
void get_sequence(S& s, const ptree& pt) {
    get(s, pt, std::make_index_sequence<boost::fusion::result_of::size<S>::type::value>{});
}

}
