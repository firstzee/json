#pragma once

#include "get.hpp"

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace deserialize {

template<typename S>
void fromJson(S& s, const std::string& json) {
    static_assert(boost::fusion::traits::is_sequence<S>::value, "For only boost fusion adapt struct");
    std::stringstream ss {json};
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);
    details::get_sequence(s, pt, std::make_index_sequence<boost::fusion::result_of::size<S>::type::value>{});
}

}
