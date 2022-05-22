#pragma once

#include "constants.hpp"
#include "core_configuration/core_configuration.hpp"
#include "json_utility.hpp"
#include "manipulator/manipulator_factory.hpp"
#include <pqrs/string.hpp>

namespace krbn {
class complex_modifications_assets_file final {
public:
  complex_modifications_assets_file(const std::filesystem::path& file_path) : file_path_(file_path) {
    std::ifstream stream(file_path);
    if (!stream) {
      throw std::runtime_error(fmt::format("failed to open {0}", file_path.string()));
    } else {
      auto json = json_utility::parse_jsonc(stream);

      pqrs::json::requires_object(json, "json");

      for (const auto& [key, value] : json.items()) {
        if (key == "title") {
          pqrs::json::requires_string(value, "`" + key + "`");

          title_ = value.get<std::string>();

        } else if (key == "maintainers") {
          // `maintainers` is used in <https://ke-complex-modifications.pqrs.org/>.
          pqrs::json::requires_array(value, "`" + key + "`");

        } else if (key == "rules") {
          pqrs::json::requires_array(value, "`" + key + "`");

          core_configuration::details::complex_modifications_parameters parameters;
          for (const auto& j : value) {
            try {
              rules_.emplace_back(j, parameters);
            } catch (const pqrs::json::unmarshal_error& e) {
              throw pqrs::json::unmarshal_error(fmt::format("`{0}` entry error: {1}", key, e.what()));
            }
          }

        } else {
          // Ignore unknown keys
        }
      }
    }
  }

  const std::filesystem::path& get_file_path(void) const {
    return file_path_;
  }

  const std::string& get_title(void) const {
    return title_;
  }

  const std::vector<core_configuration::details::complex_modifications_rule>& get_rules(void) const {
    return rules_;
  }

  void push_back_rule_to_core_configuration_profile(core_configuration::details::profile& profile,
                                                    size_t index) {
    if (index < rules_.size()) {
      profile.push_back_complex_modifications_rule(rules_[index]);
    }
  }

  std::optional<std::filesystem::file_time_type> last_write_time(void) const {
    std::error_code error_code;
    auto result = std::filesystem::last_write_time(file_path_, error_code);

    if (error_code) {
      logger::get_logger()->error("Failed to get last_write_time of {0}: {1}.",
                                  file_path_.string(),
                                  error_code.message());
      return std::nullopt;
    }

    return result;
  }

  void unlink_file(void) const {
    unlink(file_path_.c_str());
  }

  bool user_file(void) const {
    return pqrs::string::starts_with(file_path_.string(),
                                     constants::get_user_complex_modifications_assets_directory().string());
  }

  std::vector<std::string> lint(void) const {
    std::vector<std::string> error_messages;

    for (const auto& rule : rules_) {
      for (const auto& manipulator : rule.get_manipulators()) {
        try {
          manipulator::manipulator_factory::make_manipulator(manipulator.get_json(),
                                                             manipulator.get_parameters());
          for (const auto& c : manipulator.get_conditions()) {
            manipulator::manipulator_factory::make_condition(c.get_json());
          }

        } catch (const std::exception& e) {
          error_messages.push_back(fmt::format("`{0}` error: {1}",
                                               rule.get_description(),
                                               e.what()));
        }
      }
    }

    return error_messages;
  }

private:
  std::filesystem::path file_path_;
  std::string title_;
  std::vector<core_configuration::details::complex_modifications_rule> rules_;
};
} // namespace krbn
