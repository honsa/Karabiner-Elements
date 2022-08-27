#include "device_properties.hpp"
#include <boost/ut.hpp>

void run_device_properties_test(void) {
  using namespace boost::ut;
  using namespace boost::ut::literals;

  "device_properties"_test = [] {
    auto device_properties = krbn::device_properties(krbn::device_id(0),
                                                     nullptr);
    expect(device_properties.get_device_id() != std::nullopt);
    expect(*(device_properties.get_device_id()) == krbn::device_id(0));
  };
}
