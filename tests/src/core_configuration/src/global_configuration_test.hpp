#include "core_configuration/core_configuration.hpp"
#include <boost/ut.hpp>

void run_global_configuration_test(void) {
  using namespace boost::ut;
  using namespace boost::ut::literals;

  "global_configuration"_test = [] {
    // empty json
    {
      nlohmann::json json;
      krbn::core_configuration::details::global_configuration global_configuration(json);
      expect(global_configuration.get_check_for_updates_on_startup() == true);
      expect(global_configuration.get_show_in_menu_bar() == true);
      expect(global_configuration.get_show_profile_name_in_menu_bar() == false);
      expect(global_configuration.get_unsafe_ui() == false);
    }

    // load values from json
    {
      nlohmann::json json{
          {"check_for_updates_on_startup", false},
          {"show_in_menu_bar", false},
          {"show_profile_name_in_menu_bar", true},
          {"unsafe_ui", true},
      };
      krbn::core_configuration::details::global_configuration global_configuration(json);
      expect(global_configuration.get_check_for_updates_on_startup() == false);
      expect(global_configuration.get_show_in_menu_bar() == false);
      expect(global_configuration.get_show_profile_name_in_menu_bar() == true);
      expect(global_configuration.get_unsafe_ui() == true);
    }

    // invalid values in json
    {
      nlohmann::json json{
          {"check_for_updates_on_startup", nlohmann::json::array()},
          {"show_in_menu_bar", 0},
          {"show_profile_name_in_menu_bar", nlohmann::json::object()},
          {"unsafe_ui", false},
      };
      krbn::core_configuration::details::global_configuration global_configuration(json);
      expect(global_configuration.get_check_for_updates_on_startup() == true);
      expect(global_configuration.get_show_in_menu_bar() == true);
      expect(global_configuration.get_show_profile_name_in_menu_bar() == false);
      expect(global_configuration.get_unsafe_ui() == false);
    }
  };
}
