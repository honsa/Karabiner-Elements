#pragma once

#include "../../share/manipulator_conditions_helper.hpp"
#include "actual_examples_helper.hpp"

void run_manipulator_conditions_test(void) {
  using namespace boost::ut;
  using namespace boost::ut::literals;

  "manipulator.manipulator_factory"_test = [] {
    {
      nlohmann::json json({
          {"type", "frontmost_application_if"},
          {
              "bundle_identifiers",
              {
                  "^com\\.apple\\.Terminal$",
                  "^com\\.googlecode\\.iterm2$",
              },
          },
      });
      auto condition = krbn::manipulator::manipulator_factory::make_condition(json);
      expect(dynamic_cast<krbn::manipulator::conditions::frontmost_application*>(condition.get()) != nullptr);
      expect(dynamic_cast<krbn::manipulator::conditions::nop*>(condition.get()) == nullptr);
    }
    {
      nlohmann::json json({
          {"type", "input_source_if"},
      });
      auto condition = krbn::manipulator::manipulator_factory::make_condition(json);
      expect(dynamic_cast<krbn::manipulator::conditions::input_source*>(condition.get()) != nullptr);
      expect(dynamic_cast<krbn::manipulator::conditions::nop*>(condition.get()) == nullptr);
    }
    {
      nlohmann::json json({
          {"type", "input_source_unless"},
      });
      auto condition = krbn::manipulator::manipulator_factory::make_condition(json);
      expect(dynamic_cast<krbn::manipulator::conditions::input_source*>(condition.get()) != nullptr);
      expect(dynamic_cast<krbn::manipulator::conditions::nop*>(condition.get()) == nullptr);
    }
    {
      nlohmann::json json;
      json["type"] = "input_source_if";
      json["input_sources"] = nlohmann::json::array();
      {
        nlohmann::json j;
        j["language"] = "^en$";
        j["input_source_id"] = "^com\\.apple\\.keylayout\\.US$";
        j["input_mode_id"] = "^com\\.apple\\.inputmethod\\.SCIM\\.ITABC$";
        json["input_sources"].push_back(j);
      }
      auto condition = krbn::manipulator::manipulator_factory::make_condition(json);
      expect(dynamic_cast<krbn::manipulator::conditions::input_source*>(condition.get()) != nullptr);
      expect(dynamic_cast<krbn::manipulator::conditions::nop*>(condition.get()) == nullptr);
    }
    {
      nlohmann::json json({
          {"type", "variable_if"},
          {"name", "variable_name"},
          {"value", 1},
      });
      auto condition = krbn::manipulator::manipulator_factory::make_condition(json);
      expect(dynamic_cast<krbn::manipulator::conditions::variable*>(condition.get()) != nullptr);
      expect(dynamic_cast<krbn::manipulator::conditions::nop*>(condition.get()) == nullptr);
    }
    {
      nlohmann::json json({
          {"type", "variable_unless"},
          {"name", "variable_name"},
          {"value", 1},
      });
      auto condition = krbn::manipulator::manipulator_factory::make_condition(json);
      expect(dynamic_cast<krbn::manipulator::conditions::variable*>(condition.get()) != nullptr);
      expect(dynamic_cast<krbn::manipulator::conditions::nop*>(condition.get()) == nullptr);
    }
    {
      nlohmann::json json;
      json["type"] = "keyboard_type_if";
      json["keyboard_types"] = nlohmann::json::array({"iso"});
      auto condition = krbn::manipulator::manipulator_factory::make_condition(json);
      expect(dynamic_cast<krbn::manipulator::conditions::keyboard_type*>(condition.get()) != nullptr);
      expect(dynamic_cast<krbn::manipulator::conditions::nop*>(condition.get()) == nullptr);
    }
    {
      nlohmann::json json;
      json["type"] = "keyboard_type_unless";
      json["keyboard_types"] = nlohmann::json::array({"iso"});
      auto condition = krbn::manipulator::manipulator_factory::make_condition(json);
      expect(dynamic_cast<krbn::manipulator::conditions::keyboard_type*>(condition.get()) != nullptr);
      expect(dynamic_cast<krbn::manipulator::conditions::nop*>(condition.get()) == nullptr);
    }
  };

  "manipulator_environment.save_to_file"_test = [] {
    krbn::manipulator::manipulator_environment manipulator_environment;
    manipulator_environment.enable_json_output("tmp/manipulator_environment.json");

    pqrs::osx::frontmost_application_monitor::application application;
    application.set_bundle_identifier("com.apple.Terminal");
    application.set_file_path("/Applications/Utilities/Terminal.app/Contents/MacOS/Terminal");
    manipulator_environment.set_frontmost_application(application);

    pqrs::osx::input_source::properties properties;
    properties.set_first_language("en");
    properties.set_input_source_id("com.apple.keylayout.US");
    manipulator_environment.set_input_source_properties(properties);

    manipulator_environment.set_variable("value1", krbn::manipulator_environment_variable(100));
    manipulator_environment.set_variable("value2", krbn::manipulator_environment_variable(200));

    pqrs::osx::system_preferences::properties system_preferences_properties;
    system_preferences_properties.set_use_fkeys_as_standard_function_keys(true);
    system_preferences_properties.set_scroll_direction_is_natural(false);
    system_preferences_properties.set_keyboard_types(
        std::map<pqrs::osx::system_preferences::keyboard_type_key,
                 pqrs::osx::iokit_keyboard_type::value_t>({
            {
                pqrs::osx::system_preferences::keyboard_type_key(krbn::hid::vendor_id::karabiner_virtual_hid_device,
                                                                 krbn::hid::product_id::karabiner_virtual_hid_keyboard,
                                                                 pqrs::hid::country_code::value_t(0)),
                pqrs::osx::iokit_keyboard_type::iso,
            },
        }));
    manipulator_environment.set_system_preferences_properties(system_preferences_properties);

    auto core_configuration = std::make_shared<krbn::core_configuration::core_configuration>("json/core_configuration.json", geteuid());
    manipulator_environment.set_core_configuration(core_configuration);

    krbn::async_file_writer::wait();

    expect(krbn::unit_testing::json_helper::compare_files("expected/manipulator_environment.json",
                                                          "tmp/manipulator_environment.json"));
  };

  "conditions.frontmost_application"_test = [] {
    actual_examples_helper helper("frontmost_application.json");
    krbn::manipulator::manipulator_environment manipulator_environment;
    krbn::event_queue::entry entry(krbn::device_id(1),
                                   krbn::event_queue::event_time_stamp(krbn::absolute_time_point(0)),
                                   krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                         pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                   krbn::event_type::key_down,
                                   krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                         pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                   krbn::event_origin::grabbed_device,
                                   krbn::event_queue::state::original);

    // bundle_identifiers matching
    {
      pqrs::osx::frontmost_application_monitor::application application;
      application.set_bundle_identifier("com.apple.Terminal");
      application.set_file_path("/not_found");
      manipulator_environment.set_frontmost_application(application);
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == true);
      // use cache
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == true);
    }

    // Test regex escape works properly
    {
      pqrs::osx::frontmost_application_monitor::application application;
      application.set_bundle_identifier("com/apple/Terminal");
      application.set_file_path("/not_found");
      manipulator_environment.set_frontmost_application(application);

      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == false);
      // use cache
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == false);
    }

    // file_path matching
    {
      pqrs::osx::frontmost_application_monitor::application application;
      application.set_bundle_identifier("com/apple/Terminal");
      application.set_file_path("/Applications/Utilities/Terminal.app/Contents/MacOS/Terminal");
      manipulator_environment.set_frontmost_application(application);

      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == true);
    }

    // frontmost_application_unless
    {
      pqrs::osx::frontmost_application_monitor::application application;
      application.set_bundle_identifier("com.googlecode.iterm2");
      application.set_file_path("/Applications/iTerm.app");
      manipulator_environment.set_frontmost_application(application);

      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == true);
    }

    {
      pqrs::osx::frontmost_application_monitor::application application;
      application.set_bundle_identifier("com.googlecode.iterm2");
      application.set_file_path("/Users/tekezo/Applications/iTerm.app");
      manipulator_environment.set_frontmost_application(application);

      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == false);
    }
  };

  "conditions.input_source"_test = [] {
    actual_examples_helper helper("input_source.json");
    krbn::manipulator::manipulator_environment manipulator_environment;
    krbn::event_queue::entry entry(krbn::device_id(1),
                                   krbn::event_queue::event_time_stamp(krbn::absolute_time_point(0)),
                                   krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                         pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                   krbn::event_type::key_down,
                                   krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                         pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                   krbn::event_origin::grabbed_device,
                                   krbn::event_queue::state::original);

    // language matching
    {
      pqrs::osx::input_source::properties properties;
      properties.set_first_language("en");
      properties.set_input_source_id("com.apple.keylayout.Australian");
      manipulator_environment.set_input_source_properties(properties);
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == true);
      // use cache
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == true);
    }

    // Test regex escape works properly
    {
      pqrs::osx::input_source::properties properties;
      properties.set_first_language("ja");
      properties.set_input_source_id("com/apple/keylayout/Australian");
      manipulator_environment.set_input_source_properties(properties);
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == false);
      // use cache
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == false);
    }

    // input_source_id matching
    {
      pqrs::osx::input_source::properties properties;
      properties.set_first_language("ja");
      properties.set_input_source_id("com.apple.keylayout.US");
      manipulator_environment.set_input_source_properties(properties);
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == true);
    }

    // input_mode_id matching
    {
      pqrs::osx::input_source::properties properties;
      properties.set_first_language("ja");
      properties.set_input_source_id("com.apple.keylayout.Australian");
      properties.set_input_mode_id("com.apple.inputmethod.Japanese.FullWidthRoman");
      manipulator_environment.set_input_source_properties(properties);
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == true);
    }

    // input_source_unless
    {
      pqrs::osx::input_source::properties properties;
      properties.set_first_language("fr");
      properties.set_input_source_id("com.apple.keylayout.US");
      manipulator_environment.set_input_source_properties(properties);
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == false);
    }
  };

  "conditions.device"_test = [] {
    krbn::unit_testing::manipulator_conditions_helper manipulator_conditions_helper;
    auto& environment = manipulator_conditions_helper.get_manipulator_environment();

    auto device_id_8888_9999 = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(8888), pqrs::hid::product_id::value_t(9999), std::nullopt, true, false);

    auto device_id_1000_2000 = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1000), pqrs::hid::product_id::value_t(2000), std::nullopt, true, false);

    auto device_id_1000_2001 = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1000), pqrs::hid::product_id::value_t(2001), std::nullopt, true, false);

    auto device_id_1001_2000 = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1001), pqrs::hid::product_id::value_t(2000), std::nullopt, true, false);

    auto device_id_1001_2001 = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1001), pqrs::hid::product_id::value_t(2001), std::nullopt, true, false);

    auto device_id_1099_9999 = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1099), pqrs::hid::product_id::value_t(9999), std::nullopt, true, false);

    auto device_id_1000_2000_tt = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1000), pqrs::hid::product_id::value_t(2000), std::nullopt, true, true);

    auto device_id_1000_2000_tf = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1000), pqrs::hid::product_id::value_t(2000), std::nullopt, true, false);

    auto device_id_1000_2000_ft = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1000), pqrs::hid::product_id::value_t(2000), std::nullopt, false, true);

    auto device_id_1000_2000_ff = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1000), pqrs::hid::product_id::value_t(2000), std::nullopt, false, false);

    auto device_id_1000_2000_3000 = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1000), pqrs::hid::product_id::value_t(2000), krbn::location_id(3000), std::nullopt, std::nullopt);

    auto device_id_1000_2000_none = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1000), pqrs::hid::product_id::value_t(2000), std::nullopt, std::nullopt, std::nullopt);

    auto device_id_1000_2000_4000 = manipulator_conditions_helper.prepare_device(
        pqrs::hid::vendor_id::value_t(1000), pqrs::hid::product_id::value_t(2000), krbn::location_id(4000), std::nullopt, std::nullopt);

    {
      actual_examples_helper helper("device_if.jsonc");

      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_8888_9999);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == false);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == true);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2001);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == false);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1001_2000);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == false);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1001_2001);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == true);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1099_9999);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == true);
      }
    }
    {
      actual_examples_helper helper("device_unless.jsonc");

      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_8888_9999);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == true);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == false);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2001);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == true);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1001_2000);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == true);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1001_2001);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == false);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1099_9999);
        expect(helper.get_condition_manager().is_fulfilled(e, environment) == false);
      }
    }

    {
      nlohmann::json json;
      json["type"] = "device_if";
      json["identifiers"] = nlohmann::json::array();
      json["identifiers"].push_back(nlohmann::json::object());
      json["identifiers"].back()["vendor_id"] = 1000;
      json["identifiers"].back()["product_id"] = 2000;
      json["identifiers"].back()["is_keyboard"] = true;
      json["identifiers"].back()["is_pointing_device"] = false;
      krbn::manipulator::conditions::device condition(json);

      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000);
        expect(condition.is_fulfilled(e, environment) == true);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2001);
        expect(condition.is_fulfilled(e, environment) == false);
      }

      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000_tf);
        expect(condition.is_fulfilled(e, environment) == true);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000_ff);
        expect(condition.is_fulfilled(e, environment) == false);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000_tt);
        expect(condition.is_fulfilled(e, environment) == false);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000_ft);
        expect(condition.is_fulfilled(e, environment) == false);
      }
    }

    {
      nlohmann::json json;
      json["type"] = "device_if";
      json["identifiers"] = nlohmann::json::array();
      json["identifiers"].push_back(nlohmann::json::object());
      json["identifiers"].back()["vendor_id"] = 1000;
      json["identifiers"].back()["product_id"] = 2000;
      json["identifiers"].back()["location_id"] = 3000;
      krbn::manipulator::conditions::device condition(json);

      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000_3000);
        expect(condition.is_fulfilled(e, environment) == true);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000_none);
        expect(condition.is_fulfilled(e, environment) == false);
      }
      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000_4000);
        expect(condition.is_fulfilled(e, environment) == false);
      }
    }

    // is_built_in_keyboard
    {
      nlohmann::json json;
      json["type"] = "device_if";
      json["identifiers"] = nlohmann::json::array();
      json["identifiers"].push_back(nlohmann::json::object());
      json["identifiers"].back()["is_built_in_keyboard"] = true;
      krbn::manipulator::conditions::device condition(json);

      {
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000);
        expect(condition.is_fulfilled(e, environment) == false);
      }
      {
        manipulator_conditions_helper.get_core_configuration()->get_selected_profile().set_device_treat_as_built_in_keyboard(
            krbn::device_identifiers(pqrs::hid::vendor_id::value_t(1000), pqrs::hid::product_id::value_t(2000), true, false),
            true);
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000);
        expect(condition.is_fulfilled(e, environment) == true);
      }
      {
        manipulator_conditions_helper.get_core_configuration()->get_selected_profile().set_device_treat_as_built_in_keyboard(
            krbn::device_identifiers(pqrs::hid::vendor_id::value_t(1000), pqrs::hid::product_id::value_t(2000), true, false),
            false);
        auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1000_2000);
        expect(condition.is_fulfilled(e, environment) == false);
      }
    }
  };

  "conditions.event_changed"_test = [] {
    krbn::manipulator::manipulator_environment manipulator_environment;
    krbn::event_queue::entry original_entry(krbn::device_id(1),
                                            krbn::event_queue::event_time_stamp(krbn::absolute_time_point(0)),
                                            krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                                  pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                            krbn::event_type::key_down,
                                            krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                                  pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                            krbn::event_origin::grabbed_device,
                                            krbn::event_queue::state::original);
    krbn::event_queue::entry manipulated_entry(krbn::device_id(1),
                                               krbn::event_queue::event_time_stamp(krbn::absolute_time_point(0)),
                                               krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                                     pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                               krbn::event_type::key_down,
                                               krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                                     pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                               krbn::event_origin::grabbed_device,
                                               krbn::event_queue::state::manipulated);

    {
      actual_examples_helper helper("event_changed_if.json");

      expect(helper.get_condition_manager().is_fulfilled(original_entry,
                                                         manipulator_environment) == true);

      expect(helper.get_condition_manager().is_fulfilled(manipulated_entry,
                                                         manipulator_environment) == false);
    }
    {
      actual_examples_helper helper("event_changed_unless.json");

      expect(helper.get_condition_manager().is_fulfilled(original_entry,
                                                         manipulator_environment) == true);

      expect(helper.get_condition_manager().is_fulfilled(manipulated_entry,
                                                         manipulator_environment) == false);
    }
  };

  "conditions.keyboard_type"_test = [] {
    krbn::manipulator::manipulator_environment manipulator_environment;
    krbn::event_queue::entry entry(krbn::device_id(1),
                                   krbn::event_queue::event_time_stamp(krbn::absolute_time_point(0)),
                                   krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                         pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                   krbn::event_type::key_down,
                                   krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                         pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                   krbn::event_origin::grabbed_device,
                                   krbn::event_queue::state::original);

    pqrs::osx::system_preferences::properties system_preferences_properties;
    system_preferences_properties.set_use_fkeys_as_standard_function_keys(true);
    system_preferences_properties.set_scroll_direction_is_natural(false);
    system_preferences_properties.set_keyboard_types(
        std::map<pqrs::osx::system_preferences::keyboard_type_key,
                 pqrs::osx::iokit_keyboard_type::value_t>({
            {
                pqrs::osx::system_preferences::keyboard_type_key(krbn::hid::vendor_id::karabiner_virtual_hid_device,
                                                                 krbn::hid::product_id::karabiner_virtual_hid_keyboard,
                                                                 pqrs::hid::country_code::value_t(0)),
                pqrs::osx::iokit_keyboard_type::ansi,
            },
            {
                pqrs::osx::system_preferences::keyboard_type_key(krbn::hid::vendor_id::karabiner_virtual_hid_device,
                                                                 krbn::hid::product_id::karabiner_virtual_hid_keyboard,
                                                                 pqrs::hid::country_code::value_t(1)),
                pqrs::osx::iokit_keyboard_type::iso,
            },
        }));
    manipulator_environment.set_system_preferences_properties(system_preferences_properties);

    auto core_configuration = std::make_shared<krbn::core_configuration::core_configuration>("json/core_configuration.json", geteuid());
    manipulator_environment.set_core_configuration(core_configuration);

    {
      actual_examples_helper helper("keyboard_type_if.json");

      // iso
      core_configuration->get_selected_profile().get_virtual_hid_keyboard().set_country_code(pqrs::hid::country_code::value_t(1));
      manipulator_environment.set_core_configuration(core_configuration);
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == true);

      // ansi
      core_configuration->get_selected_profile().get_virtual_hid_keyboard().set_country_code(pqrs::hid::country_code::value_t(0));
      manipulator_environment.set_core_configuration(core_configuration);
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == false);
    }
    {
      actual_examples_helper helper("keyboard_type_unless.json");

      // iso
      core_configuration->get_selected_profile().get_virtual_hid_keyboard().set_country_code(pqrs::hid::country_code::value_t(1));
      manipulator_environment.set_core_configuration(core_configuration);
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == false);

      // ansi
      core_configuration->get_selected_profile().get_virtual_hid_keyboard().set_country_code(pqrs::hid::country_code::value_t(0));
      manipulator_environment.set_core_configuration(core_configuration);
      expect(helper.get_condition_manager().is_fulfilled(entry,
                                                         manipulator_environment) == true);
    }
  };

  "conditions.variable"_test = [] {
    krbn::manipulator::manipulator_environment manipulator_environment;
    krbn::event_queue::entry entry(krbn::device_id(1),
                                   krbn::event_queue::event_time_stamp(krbn::absolute_time_point(0)),
                                   krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                         pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                   krbn::event_type::key_down,
                                   krbn::event_queue::event(krbn::momentary_switch_event(pqrs::hid::usage_page::keyboard_or_keypad,
                                                                                         pqrs::hid::usage::keyboard_or_keypad::keyboard_a)),
                                   krbn::event_origin::grabbed_device,
                                   krbn::event_queue::state::original);

    manipulator_environment.set_variable("variable_name", krbn::manipulator_environment_variable(123));
    manipulator_environment.set_variable("variable_bool", krbn::manipulator_environment_variable(true));
    manipulator_environment.set_variable("variable_string", krbn::manipulator_environment_variable("hello"));

    //
    // variable_if
    //

    {
      nlohmann::json json;
      json["type"] = "variable_if";
      json["name"] = "variable_name";
      json["value"] = 123;
      krbn::manipulator::conditions::variable condition(json);

      expect(condition.is_fulfilled(entry, manipulator_environment));
    }
    {
      nlohmann::json json;
      json["type"] = "variable_if";
      json["name"] = "variable_name";
      json["value"] = 1234;
      krbn::manipulator::conditions::variable condition(json);

      expect(!condition.is_fulfilled(entry, manipulator_environment));
    }
    {
      nlohmann::json json;
      json["type"] = "variable_if";
      json["name"] = "unknown";
      json["value"] = 0;
      krbn::manipulator::conditions::variable condition(json);

      expect(condition.is_fulfilled(entry, manipulator_environment));
    }
    {
      nlohmann::json json;
      json["type"] = "variable_if";
      json["name"] = "unknown";
      json["value"] = 1;
      krbn::manipulator::conditions::variable condition(json);

      expect(!condition.is_fulfilled(entry, manipulator_environment));
    }
    {
      nlohmann::json json;
      json["type"] = "variable_if";
      json["name"] = "variable_bool";
      json["value"] = 1234;
      krbn::manipulator::conditions::variable condition(json);

      expect(!condition.is_fulfilled(entry, manipulator_environment));
    }
    {
      nlohmann::json json;
      json["type"] = "variable_if";
      json["name"] = "variable_bool";
      json["value"] = true;
      krbn::manipulator::conditions::variable condition(json);

      expect(condition.is_fulfilled(entry, manipulator_environment));
    }
    {
      nlohmann::json json;
      json["type"] = "variable_if";
      json["name"] = "variable_string";
      json["value"] = true;
      krbn::manipulator::conditions::variable condition(json);

      expect(!condition.is_fulfilled(entry, manipulator_environment));
    }
    {
      nlohmann::json json;
      json["type"] = "variable_if";
      json["name"] = "variable_string";
      json["value"] = "hello";
      krbn::manipulator::conditions::variable condition(json);

      expect(condition.is_fulfilled(entry, manipulator_environment));
    }

    //
    // variable_unless
    //

    {
      nlohmann::json json;
      json["type"] = "variable_unless";
      json["name"] = "variable_name";
      json["value"] = 123;
      krbn::manipulator::conditions::variable condition(json);

      expect(!condition.is_fulfilled(entry, manipulator_environment));
    }
    {
      nlohmann::json json;
      json["type"] = "variable_unless";
      json["name"] = "variable_name";
      json["value"] = 1234;
      krbn::manipulator::conditions::variable condition(json);

      expect(condition.is_fulfilled(entry, manipulator_environment));
    }
    {
      nlohmann::json json;
      json["type"] = "variable_unless";
      json["name"] = "unknown";
      json["value"] = 0;
      krbn::manipulator::conditions::variable condition(json);

      expect(!condition.is_fulfilled(entry, manipulator_environment));
    }
    {
      nlohmann::json json;
      json["type"] = "variable_unless";
      json["name"] = "unknown";
      json["value"] = 1;
      krbn::manipulator::conditions::variable condition(json);

      expect(condition.is_fulfilled(entry, manipulator_environment));
    }
  };
}
