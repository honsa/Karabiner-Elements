#pragma once

#include "core_configuration/core_configuration.hpp"
#include "json_utility.hpp"
#include "manipulator/manipulator_factory.hpp"
#include "manipulator/manipulator_manager.hpp"
#include "manipulator/manipulators/basic/basic.hpp"

namespace krbn {
namespace grabber {
namespace device_grabber_details {
class fn_function_keys_manipulator_manager final {
public:
  fn_function_keys_manipulator_manager(void) {
    manipulator_manager_ = std::make_shared<manipulator::manipulator_manager>();
  }

  std::shared_ptr<manipulator::manipulator_manager> get_manipulator_manager(void) const {
    return manipulator_manager_;
  }

  void update(const core_configuration::details::profile& profile,
              const pqrs::osx::system_preferences::properties& system_preferences_properties) {
    manipulator_manager_->invalidate_manipulators();

    auto from_mandatory_modifiers = nlohmann::json::array();

    auto from_optional_modifiers = nlohmann::json::array();
    from_optional_modifiers.push_back("any");

    auto to_modifiers = nlohmann::json::array();

    if (system_preferences_properties.get_use_fkeys_as_standard_function_keys()) {
      // f1 -> f1
      // fn+f1 -> display_brightness_decrement

      from_mandatory_modifiers.push_back("fn");

      // consumer_key_code::dictation does not work with modifier_flag::fn.
      // So, we send the plain media keys.
      // (e.g., display_brightness_decrement, not fn+display_brightness_decrement)

    } else {
      // f1 -> display_brightness_decrement
      // fn+f1 -> f1

      // fn+f1 ... fn+f12 -> f1 .. f12

      for (int i = 1; i <= 12; ++i) {
        auto from_json = nlohmann::json::object({
            {"key_code", fmt::format("f{0}", i)},
            {"modifiers", nlohmann::json::object({
                              {"mandatory", nlohmann::json::array({"fn"})},
                              {"optional", nlohmann::json::array({"any"})},
                          })},
        });

        auto to_json = nlohmann::json::object({
            {"key_code", fmt::format("f{0}", i)},
            {"modifiers", nlohmann::json::array({"fn"})},
        });

        std::vector<manipulator::to_event_definition> to_event_definitions;
        to_event_definitions.emplace_back(to_json);

        try {
          auto manipulator = std::make_shared<manipulator::manipulators::basic::basic>(manipulator::manipulators::basic::from_event_definition(from_json),
                                                                                       to_event_definitions);
          manipulator_manager_->push_back_manipulator(std::shared_ptr<manipulator::manipulators::base>(manipulator));

        } catch (const pqrs::json::unmarshal_error& e) {
          logger::get_logger()->error(fmt::format("karabiner.json error: {0}", e.what()));

        } catch (const std::exception& e) {
          logger::get_logger()->error(e.what());
        }
      }
    }

    // from_modifiers+f1 -> display_brightness_decrement ...

    for (const auto& device : profile.get_devices()) {
      for (const auto& pair : device.get_fn_function_keys().get_pairs()) {
        try {
          if (auto m = make_manipulator(pair,
                                        from_mandatory_modifiers,
                                        from_optional_modifiers,
                                        to_modifiers)) {
            m->push_back_condition(manipulator::manipulator_factory::make_event_changed_if_condition(false));
            m->push_back_condition(manipulator::manipulator_factory::make_device_unless_touch_bar_condition());

            auto c = manipulator::manipulator_factory::make_device_if_condition(device);
            m->push_back_condition(c);

            manipulator_manager_->push_back_manipulator(m);
          }

        } catch (const pqrs::json::unmarshal_error& e) {
          logger::get_logger()->error(fmt::format("karabiner.json error: {0}", e.what()));

        } catch (const std::exception& e) {
          logger::get_logger()->error(e.what());
        }
      }
    }

    for (const auto& pair : profile.get_fn_function_keys().get_pairs()) {
      if (auto m = make_manipulator(pair,
                                    from_mandatory_modifiers,
                                    from_optional_modifiers,
                                    to_modifiers)) {
        m->push_back_condition(manipulator::manipulator_factory::make_event_changed_if_condition(false));
        m->push_back_condition(manipulator::manipulator_factory::make_device_unless_touch_bar_condition());

        manipulator_manager_->push_back_manipulator(m);
      }
    }

    // fn+return_or_enter -> keypad_enter ...
    {
      nlohmann::json data = nlohmann::json::array();

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"key_code", "return_or_enter"}})},
          {"to", nlohmann::json::object({{"key_code", "keypad_enter"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"key_code", "delete_or_backspace"}})},
          {"to", nlohmann::json::object({{"key_code", "delete_forward"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"key_code", "right_arrow"}})},
          {"to", nlohmann::json::object({{"key_code", "end"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"key_code", "left_arrow"}})},
          {"to", nlohmann::json::object({{"key_code", "home"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"key_code", "down_arrow"}})},
          {"to", nlohmann::json::object({{"key_code", "page_down"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"key_code", "up_arrow"}})},
          {"to", nlohmann::json::object({{"key_code", "page_up"}})},
      }));

      for (const auto& d : data) {
        auto from_json = d["from"];
        from_json["modifiers"]["mandatory"] = nlohmann::json::array({"fn"});
        from_json["modifiers"]["optional"] = nlohmann::json::array({"any"});

        auto to_json = d["to"];
        to_json["modifiers"] = nlohmann::json::array({"fn"});

        std::vector<manipulator::to_event_definition> to_event_definitions;
        to_event_definitions.emplace_back(to_json);

        try {
          auto manipulator = std::make_shared<manipulator::manipulators::basic::basic>(manipulator::manipulators::basic::from_event_definition(from_json),
                                                                                       to_event_definitions);
          manipulator_manager_->push_back_manipulator(std::shared_ptr<manipulator::manipulators::base>(manipulator));

        } catch (const pqrs::json::unmarshal_error& e) {
          logger::get_logger()->error(fmt::format("karabiner.json error: {0}", e.what()));

        } catch (const std::exception& e) {
          logger::get_logger()->error(e.what());
        }
      }
    }

    //
    // Change keys which macOS will ignore.
    //

    // Touch ID
    {
      try {
        for (const auto& from_json : {
                 nlohmann::json::object({
                     // Touch ID
                     {"consumer_key_code", "menu"},
                 }),
                 nlohmann::json::object({
                     // Lock key on Magic Keyboard without Touch ID
                     {"consumer_key_code", "al_terminal_lock_or_screensaver"},
                 }),
             }) {
          std::vector<manipulator::to_event_definition> to_event_definitions;

          to_event_definitions.emplace_back(nlohmann::json::object({
              {"consumer_key_code", "al_terminal_lock_or_screensaver"},
          }));

          auto manipulator = std::make_shared<manipulator::manipulators::basic::basic>(manipulator::manipulators::basic::from_event_definition(from_json),
                                                                                       to_event_definitions);

          manipulator_manager_->push_back_manipulator(std::shared_ptr<manipulator::manipulators::base>(manipulator));
        }

      } catch (const std::exception& e) {
        logger::get_logger()->error(e.what());
      }
    }

    // Application launch buttons
    {
      nlohmann::json data = nlohmann::json::array();

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"consumer_key_code", "al_word_processor"}})},
          {"to", nlohmann::json::object({{"shell_command", "open -a 'Pages.app'"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"consumer_key_code", "al_text_editor"}})},
          {"to", nlohmann::json::object({{"shell_command", "open -a 'TextEdit.app'"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"consumer_key_code", "al_spreadsheet"}})},
          {"to", nlohmann::json::object({{"shell_command", "open -a 'Numbers.app'"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"consumer_key_code", "al_presentation_app"}})},
          {"to", nlohmann::json::object({{"shell_command", "open -a 'Keynote.app'"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"consumer_key_code", "al_email_reader"}})},
          {"to", nlohmann::json::object({{"shell_command", "open -a 'Mail.app'"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"consumer_key_code", "al_calculator"}})},
          {"to", nlohmann::json::object({{"shell_command", "open -a 'Calculator.app'"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"consumer_key_code", "al_local_machine_browser"}})},
          {"to", nlohmann::json::object({{"shell_command", "open -a 'Finder.app'"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"consumer_key_code", "al_internet_browser"}})},
          {"to", nlohmann::json::object({{"shell_command", "open -a 'Safari.app'"}})},
      }));

      data.push_back(nlohmann::json::object({
          {"from", nlohmann::json::object({{"consumer_key_code", "al_dictionary"}})},
          {"to", nlohmann::json::object({{"shell_command", "open -a 'Dictionary.app'"}})},
      }));

      for (const auto& d : data) {
        auto from_json = d["from"];
        from_json["modifiers"]["mandatory"] = nlohmann::json::array({"any"});

        auto to_json = d["to"];

        std::vector<manipulator::to_event_definition> to_event_definitions;
        to_event_definitions.emplace_back(to_json);

        try {
          auto manipulator = std::make_shared<manipulator::manipulators::basic::basic>(manipulator::manipulators::basic::from_event_definition(from_json),
                                                                                       to_event_definitions);
          manipulator_manager_->push_back_manipulator(std::shared_ptr<manipulator::manipulators::base>(manipulator));

        } catch (const pqrs::json::unmarshal_error& e) {
          logger::get_logger()->error(fmt::format("karabiner.json error: {0}", e.what()));

        } catch (const std::exception& e) {
          logger::get_logger()->error(e.what());
        }
      }
    }
  }

private:
  std::shared_ptr<manipulator::manipulators::base> make_manipulator(const std::pair<std::string, std::string>& pair,
                                                                    const nlohmann::json& from_mandatory_modifiers,
                                                                    const nlohmann::json& from_optional_modifiers,
                                                                    const nlohmann::json& to_modifiers) const {
    try {
      auto from_json = json_utility::parse_jsonc(pair.first);
      if (from_json.empty()) {
        return nullptr;
      }
      from_json["modifiers"]["mandatory"] = from_mandatory_modifiers;
      from_json["modifiers"]["optional"] = from_optional_modifiers;

      auto to_json = json_utility::parse_jsonc(pair.second);
      if (to_json.empty()) {
        return nullptr;
      }

      std::vector<manipulator::to_event_definition> to_event_definitions;
      for (auto&& j : to_json) {
        j["modifiers"] = to_modifiers;
        to_event_definitions.emplace_back(j);
      }

      return std::make_shared<manipulator::manipulators::basic::basic>(manipulator::manipulators::basic::from_event_definition(from_json),
                                                                       to_event_definitions);
    } catch (const pqrs::json::unmarshal_error& e) {
      logger::get_logger()->error(fmt::format("karabiner.json error: {0}", e.what()));
    } catch (const std::exception& e) {
      logger::get_logger()->error(e.what());
    }

    return nullptr;
  }

  std::shared_ptr<manipulator::manipulator_manager> manipulator_manager_;
};
} // namespace device_grabber_details
} // namespace grabber
} // namespace krbn
