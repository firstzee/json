#pragma once

#include <serialize/put.hpp>

#include <boost/property_tree/json_parser.hpp>

namespace deserialize {

template <typename S>
std::string to_json(const S& s) {
    static_assert(boost::fusion::traits::is_sequence<S>::value, "For only boost fusion adapt struct");
    boost::property_tree::ptree pt;
    details::put_sequence(s, pt);
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    return ss.str();
}

}
