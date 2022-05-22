#include <catch2/catch.hpp>

#include "../../share/json_helper.hpp"
#include "manipulator/types.hpp"
#include <pqrs/json.hpp>

TEST_CASE("simple_modifications.json") {
  auto json = krbn::unit_testing::json_helper::load_jsonc("../../../src/apps/PreferencesWindow/Resources/simple_modifications.json");
  auto label_set = std::unordered_set<std::string>();

  for (const auto& entry : json) {
    if (auto data = pqrs::json::find_array(entry, "data")) {
      //
      // Check `data` is valid
      //

      for (const auto& d : **data) {
        krbn::manipulator::to_event_definition e(d);
      }

      //
      // Check `from` data array size == 1
      //

      auto not_from = false;
      if (auto nf = pqrs::json::find<bool>(entry, "not_from")) {
        not_from = *nf;
      }
      if (!not_from) {
        REQUIRE((**data).size() == 1);
      }

      //
      // Check `label` uniqueness
      //

      if (auto label = pqrs::json::find<std::string>(entry, "label")) {
        std::cout << *label << std::endl;
        if (label_set.contains(*label) == true) {
          std::cout << std::endl
                    << *label << " is duplicated." << std::endl
                    << std::endl;
          REQUIRE(false);
        }

        label_set.insert(*label);
      }
    }
  }

  REQUIRE(label_set.size() > 0);
}
