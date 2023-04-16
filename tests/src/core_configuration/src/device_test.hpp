#include "core_configuration/core_configuration.hpp"
#include <boost/ut.hpp>

void run_device_test(void) {
  using namespace boost::ut;
  using namespace boost::ut::literals;

  "device.identifiers"_test = [] {
    // empty json
    {
      auto json = nlohmann::json::object();
      auto identifiers = json.get<krbn::device_identifiers>();
      expect(identifiers.get_vendor_id() == pqrs::hid::vendor_id::value_t(0));
      expect(identifiers.get_product_id() == pqrs::hid::product_id::value_t(0));
      expect(identifiers.get_is_keyboard() == false);
      expect(identifiers.get_is_pointing_device() == false);
    }

    // load values from json
    {
      nlohmann::json json({
          {"vendor_id", 1234},
          {"product_id", 5678},
          {"is_keyboard", true},
          {"is_pointing_device", true},
      });
      auto identifiers = json.get<krbn::device_identifiers>();
      expect(identifiers.get_vendor_id() == pqrs::hid::vendor_id::value_t(1234));
      expect(identifiers.get_product_id() == pqrs::hid::product_id::value_t(5678));
      expect(identifiers.get_is_keyboard() == true);
      expect(identifiers.get_is_pointing_device() == true);
    }

    // construct with vendor_id, product_id, ...
    {
      krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(1234), pqrs::hid::product_id::value_t(5678), true, false);
      expect(identifiers.get_vendor_id() == pqrs::hid::vendor_id::value_t(1234));
      expect(identifiers.get_product_id() == pqrs::hid::product_id::value_t(5678));
      expect(identifiers.get_is_keyboard() == true);
      expect(identifiers.get_is_pointing_device() == false);
    }
    {
      krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(4321), pqrs::hid::product_id::value_t(8765), false, true);
      expect(identifiers.get_vendor_id() == pqrs::hid::vendor_id::value_t(4321));
      expect(identifiers.get_product_id() == pqrs::hid::product_id::value_t(8765));
      expect(identifiers.get_is_keyboard() == false);
      expect(identifiers.get_is_pointing_device() == true);
    }
  };

  "device.identifiers.to_json"_test = [] {
    {
      auto json = nlohmann::json::object();
      auto identifiers = json.get<krbn::device_identifiers>();
      nlohmann::json expected({
          {"vendor_id", 0},
          {"product_id", 0},
          {"is_keyboard", false},
          {"is_pointing_device", false},
      });
      expect(nlohmann::json(identifiers) == expected);
    }
    {
      nlohmann::json json({
          {"is_pointing_device", true},
          {"dummy", {{"keep_me", true}}},
      });
      auto identifiers = json.get<krbn::device_identifiers>();
      nlohmann::json expected({
          {"dummy", {{"keep_me", true}}},
          {"vendor_id", 0},
          {"product_id", 0},
          {"is_keyboard", false},
          {"is_pointing_device", true},
      });
      expect(nlohmann::json(identifiers) == expected);
    }
  };

  "device"_test = [] {
    // empty json
    {
      auto json = nlohmann::json::object();
      krbn::core_configuration::details::device device(json);
      expect(device.get_identifiers().get_vendor_id() == pqrs::hid::vendor_id::value_t(0));
      expect(device.get_identifiers().get_product_id() == pqrs::hid::product_id::value_t(0));
      expect(device.get_identifiers().get_is_keyboard() == false);
      expect(device.get_identifiers().get_is_pointing_device() == false);
      expect(device.get_ignore() == false);
      expect(device.get_manipulate_caps_lock_led() == false);
      expect(device.get_treat_as_built_in_keyboard() == false);
      expect(device.get_disable_built_in_keyboard_if_exists() == false);
    }

    // load values from json
    {
      nlohmann::json json({
          {"identifiers", {
                              {"vendor_id", 1234},
                              {"product_id", 5678},
                              {"is_keyboard", true},
                              {"is_pointing_device", true},
                          }},
          {"disable_built_in_keyboard_if_exists", true},
          {"ignore", true},
          {"manipulate_caps_lock_led", true},
          {"treat_as_built_in_keyboard", false},
      });
      krbn::core_configuration::details::device device(json);
      expect(device.get_identifiers().get_vendor_id() == pqrs::hid::vendor_id::value_t(1234));
      expect(device.get_identifiers().get_product_id() == pqrs::hid::product_id::value_t(5678));
      expect(device.get_identifiers().get_is_keyboard() == true);
      expect(device.get_identifiers().get_is_pointing_device() == true);
      expect(device.get_ignore() == true);
      expect(device.get_manipulate_caps_lock_led() == true);
      expect(device.get_treat_as_built_in_keyboard() == false);
      expect(device.get_disable_built_in_keyboard_if_exists() == true);
    }

    // Special default value for specific devices
    {
      // ignore_ == true if is_pointing_device

      nlohmann::json json({
          {"identifiers", {
                              {"vendor_id", 1234},
                              {"product_id", 5678},
                              {"is_keyboard", false},
                              {"is_pointing_device", true},
                          }},
      });
      {
        krbn::core_configuration::details::device device(json);
        expect(device.get_ignore() == true);
      }
      {
        json["ignore"] = false;
        krbn::core_configuration::details::device device(json);
        expect(device.get_ignore() == false);
      }
    }
    {
      // ignore_ == true for specific devices

      nlohmann::json json({
          {"identifiers", {
                              {"vendor_id", 0x1050},
                              {"product_id", 0x407},
                              {"is_keyboard", true},
                              {"is_pointing_device", false},
                          }},
      });
      {
        krbn::core_configuration::details::device device(json);
        expect(device.get_ignore() == true);
      }
      {
        json["ignore"] = false;
        krbn::core_configuration::details::device device(json);
        expect(device.get_ignore() == false);
      }
    }
    {
      // manipulate_caps_lock_led_ == true for specific devices

      nlohmann::json json({
          {"identifiers", {
                              {"vendor_id", 0x5ac},
                              {"product_id", 0x262},
                              {"is_keyboard", true},
                              {"is_pointing_device", false},
                          }},
      });
      {
        krbn::core_configuration::details::device device(json);
        expect(device.get_manipulate_caps_lock_led() == true);
      }
      {
        json["manipulate_caps_lock_led"] = false;
        krbn::core_configuration::details::device device(json);
        expect(device.get_manipulate_caps_lock_led() == false);
      }
    }
    // Coordinate between settings
    {
      nlohmann::json json({
          {"identifiers", {
                              {"vendor_id", 1234},
                              {"product_id", 5678},
                              {"is_keyboard", false},
                              {"is_pointing_device", true},
                          }},
          {"treat_as_built_in_keyboard", true},
          {"disable_built_in_keyboard_if_exists", true},
      });
      {
        // disable_built_in_keyboard_if_exists will be false when treat_as_built_in_keyboard is true.

        krbn::core_configuration::details::device device(json);
        expect(device.get_treat_as_built_in_keyboard() == true);
        expect(device.get_disable_built_in_keyboard_if_exists() == false);

        // disable_built_in_keyboard_if_exists will be false even set true.

        device.set_disable_built_in_keyboard_if_exists(true); // ignored
        expect(device.get_disable_built_in_keyboard_if_exists() == false);

        // If treat_as_built_in_keyboard is false, disable_built_in_keyboard_if_exists can be true.

        device.set_treat_as_built_in_keyboard(false);
        expect(device.get_treat_as_built_in_keyboard() == false);
        expect(device.get_disable_built_in_keyboard_if_exists() == false);

        device.set_disable_built_in_keyboard_if_exists(true);
        expect(device.get_disable_built_in_keyboard_if_exists() == true);

        // disable_built_in_keyboard_if_exists will be false if treat_as_built_in_keyboard is set true

        device.set_treat_as_built_in_keyboard(true);
        expect(device.get_treat_as_built_in_keyboard() == true);
        expect(device.get_disable_built_in_keyboard_if_exists() == false);
      }
    }
  };

  "device.to_json"_test = [] {
    {
      auto json = nlohmann::json::object();
      krbn::core_configuration::details::device device(json);
      nlohmann::json expected({
          {"disable_built_in_keyboard_if_exists", false},
          {"identifiers", {
                              {
                                  "vendor_id",
                                  0,
                              },
                              {
                                  "product_id",
                                  0,
                              },
                              {
                                  "is_keyboard",
                                  false,
                              },
                              {
                                  "is_pointing_device",
                                  false,
                              },
                          }},
          {"ignore", false},
          {"fn_function_keys", nlohmann::json::array()},
          {"manipulate_caps_lock_led", false},
          {"simple_modifications", nlohmann::json::array()},
          {"treat_as_built_in_keyboard", false},
      });
      expect(device.to_json() == expected);

      nlohmann::json actual = device.to_json();
      expect(actual == expected);
    }
    {
      nlohmann::json json({
          {"dummy", {{"keep_me", true}}},
          {"identifiers", {
                              {
                                  "is_keyboard",
                                  true,
                              },
                              {
                                  "dummy",
                                  {{"keep_me", true}},
                              },
                          }},
          {"ignore", true},
          {"manipulate_caps_lock_led", true},
          {"treat_as_built_in_keyboard", true},
      });
      krbn::core_configuration::details::device device(json);
      nlohmann::json expected({
          {"disable_built_in_keyboard_if_exists", false},
          {"dummy", {{"keep_me", true}}},
          {"fn_function_keys", nlohmann::json::array()},
          {"identifiers", {
                              {
                                  "dummy",
                                  {{"keep_me", true}},
                              },
                              {
                                  "vendor_id",
                                  0,
                              },
                              {
                                  "product_id",
                                  0,
                              },
                              {
                                  "is_keyboard",
                                  true,
                              },
                              {
                                  "is_pointing_device",
                                  false,
                              },
                          }},
          {"ignore", true},
          {"manipulate_caps_lock_led", true},
          {"simple_modifications", nlohmann::json::array()},
          {"treat_as_built_in_keyboard", true},
      });
      expect(device.to_json() == expected);
    }
  };
}
