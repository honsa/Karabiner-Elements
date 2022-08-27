#include "connected_devices/connected_devices.hpp"
#include "json_utility.hpp"
#include <boost/ut.hpp>

void run_connected_devices_test(void) {
  using namespace boost::ut;
  using namespace boost::ut::literals;

  "connected_devices"_test = [] {
    {
      krbn::connected_devices::connected_devices connected_devices;
      expect(connected_devices.to_json() == nlohmann::json::array());
    }
    {
      krbn::connected_devices::connected_devices connected_devices;

      {
        krbn::connected_devices::details::descriptions descriptions("manufacturer1",
                                                                    "product1",
                                                                    "USB");
        krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(1234),
                                             pqrs::hid::product_id::value_t(5678),
                                             true,
                                             false);
        krbn::connected_devices::details::device device(descriptions,
                                                        identifiers,
                                                        true,
                                                        false,
                                                        false);
        connected_devices.push_back_device(device);
      }
      {
        krbn::connected_devices::details::descriptions descriptions("manufacturer1 (ignored)",
                                                                    "product1 (ignored)",
                                                                    "USB");
        krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(1234),
                                             pqrs::hid::product_id::value_t(5678),
                                             true,
                                             false);
        krbn::connected_devices::details::device device(descriptions,
                                                        identifiers,
                                                        true,
                                                        false,
                                                        false);
        connected_devices.push_back_device(device);
      }
      {
        krbn::connected_devices::details::descriptions descriptions("manufacturer3",
                                                                    "product3",
                                                                    "USB");
        krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(2345),
                                             pqrs::hid::product_id::value_t(6789),
                                             false,
                                             true);
        krbn::connected_devices::details::device device(descriptions,
                                                        identifiers,
                                                        false,
                                                        false,
                                                        false);
        connected_devices.push_back_device(device);
      }
      {
        krbn::connected_devices::details::descriptions descriptions("manufacturer2",
                                                                    "product2",
                                                                    "USB");
        krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(1234),
                                             pqrs::hid::product_id::value_t(5679),
                                             false,
                                             true);
        krbn::connected_devices::details::device device(descriptions,
                                                        identifiers,
                                                        false,
                                                        true,
                                                        false);
        connected_devices.push_back_device(device);
      }
      // product4 is a combined device.
      {
        krbn::connected_devices::details::descriptions descriptions("manufacturer1",
                                                                    "product4",
                                                                    "Bluetooth");
        krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(123),
                                             pqrs::hid::product_id::value_t(678),
                                             false,
                                             true);
        krbn::connected_devices::details::device device(descriptions,
                                                        identifiers,
                                                        false,
                                                        true,
                                                        false);
        connected_devices.push_back_device(device);
      }
      {
        krbn::connected_devices::details::descriptions descriptions("manufacturer1",
                                                                    "product4",
                                                                    "Bluetooth");
        krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(123),
                                             pqrs::hid::product_id::value_t(678),
                                             true,
                                             false);
        krbn::connected_devices::details::device device(descriptions,
                                                        identifiers,
                                                        true,
                                                        false,
                                                        false);
        connected_devices.push_back_device(device);
      }
      {
        krbn::connected_devices::details::descriptions descriptions("manufacturer1",
                                                                    "product5",
                                                                    "Bluetooth");
        krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(1234),
                                             pqrs::hid::product_id::value_t(678),
                                             true,
                                             false);
        krbn::connected_devices::details::device device(descriptions,
                                                        identifiers,
                                                        false,
                                                        false,
                                                        true);
        connected_devices.push_back_device(device);
      }

      expect(connected_devices.is_loaded() == false);
      expect(connected_devices.get_devices().size() == 6);
      expect(connected_devices.get_devices()[0].get_identifiers().get_vendor_id() == pqrs::hid::vendor_id::value_t(1234));
      expect(connected_devices.get_devices()[0].get_identifiers().get_product_id() == pqrs::hid::product_id::value_t(5678));
      expect(connected_devices.get_devices()[1].get_identifiers().get_vendor_id() == pqrs::hid::vendor_id::value_t(1234));
      expect(connected_devices.get_devices()[1].get_identifiers().get_product_id() == pqrs::hid::product_id::value_t(5679));
      expect(connected_devices.get_devices()[2].get_identifiers().get_vendor_id() == pqrs::hid::vendor_id::value_t(2345));
      expect(connected_devices.get_devices()[2].get_identifiers().get_product_id() == pqrs::hid::product_id::value_t(6789));

      std::ifstream ifs("json/connected_devices.json");

      expect(connected_devices.to_json() == krbn::json_utility::parse_jsonc(ifs));
    }

    {
      krbn::connected_devices::connected_devices connected_devices("json/connected_devices.json");

      expect(connected_devices.is_loaded() == true);
      expect(connected_devices.get_devices().size() == 6);
      expect(connected_devices.get_devices()[0].get_is_built_in_keyboard() == true);
      expect(connected_devices.get_devices()[0].get_is_built_in_trackpad() == false);
      expect(connected_devices.get_devices()[1].get_is_built_in_keyboard() == false);
      expect(connected_devices.get_devices()[1].get_is_built_in_trackpad() == true);
    }

    {
      krbn::connected_devices::connected_devices connected_devices("json/not_found.json");

      expect(connected_devices.is_loaded() == false);
      expect(connected_devices.get_devices().size() == 0);
    }

    {
      krbn::connected_devices::connected_devices connected_devices("json/broken.json");

      expect(connected_devices.is_loaded() == false);
      expect(connected_devices.get_devices().size() == 0);
    }
  };

  "connected_devices ill-formed name"_test = [] {
    const char ill_formed_name[] = {
        't',
        'e',
        's',
        't',
        ' ',
        static_cast<char>(0x80),
        '!',
        0,
    };

    krbn::connected_devices::connected_devices connected_devices;

    {
      krbn::connected_devices::details::descriptions descriptions(ill_formed_name,
                                                                  ill_formed_name,
                                                                  "USB");
      krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(1234),
                                           pqrs::hid::product_id::value_t(5678),
                                           true,
                                           false);
      krbn::connected_devices::details::device device(descriptions,
                                                      identifiers,
                                                      true,
                                                      false,
                                                      false);
      connected_devices.push_back_device(device);

      // Check throw with `dump`
      try {
        connected_devices.to_json().dump();
        expect(false);
      } catch (nlohmann::detail::type_error& ex) {
        expect(std::string_view("[json.exception.type_error.316] invalid UTF-8 byte at index 5: 0x80") == ex.what());
      } catch (...) {
        expect(false);
      }

      std::ifstream ifs("json/ill_formed_name_expected.json");
      auto expected = krbn::json_utility::parse_jsonc(ifs);

      expect(krbn::json_utility::dump(connected_devices.to_json()) == krbn::json_utility::dump(expected));
    }
  };
}
