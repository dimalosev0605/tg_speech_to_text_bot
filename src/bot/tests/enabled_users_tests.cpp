#include "enabled_users.h"

#define BOOST_TEST_MODULE enabled_users_tests
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(not_use_enabled_users) {
    enabled_users eu("not_use_enabled_users.json");
    std::vector<std::string> names = {"lol", "42", "aaa", "bbb", "valera_valakas"};
    bool flag = true;
    for(const auto& name : names) {
        flag = flag && eu.contains(name);
    }
    BOOST_REQUIRE(flag == true);
}

BOOST_AUTO_TEST_CASE(use_enabled_users_check_only_enabled) {
    enabled_users eu("use_enabled_users.json");
    std::vector<std::string> names = {"valera_Valakas", "Jim_keller"};
    bool flag = true;
    for(const auto& name : names) {
        flag = flag && eu.contains(name);
    }
    BOOST_REQUIRE(flag == true);
}

BOOST_AUTO_TEST_CASE(use_enabled_users_check_only_disabled) {
    enabled_users eu("use_enabled_users.json");
    std::vector<std::string> names = {"lol", "aaa", "42"};
    bool flag = true;
    for(const auto& name : names) {
        flag = flag && !eu.contains(name);
    }
    BOOST_REQUIRE(flag == true);
}

BOOST_AUTO_TEST_CASE(use_enabled_users_check_enabled_and_disabled) {
    enabled_users eu("use_enabled_users.json");

    std::vector<std::string> disabled_names = {"lol", "aaa", "42"};
    bool disabled_names_flag = true;
    for(const auto& name : disabled_names) {
        disabled_names_flag = disabled_names_flag && !eu.contains(name);
    }

    std::vector<std::string> enabled_names = {"valera_valakas", "Jim_keller"};
    bool enabled_names_flag = true;
    for(const auto& name : enabled_names) {
        enabled_names_flag = enabled_names_flag && eu.contains(name);
    }

    BOOST_REQUIRE(disabled_names_flag && enabled_names_flag);
}

