#include "async_file_writer.hpp"
#include <boost/ut.hpp>

int main(void) {
  using namespace boost::ut;
  using namespace boost::ut::literals;

  auto scoped_dispatcher_manager = krbn::dispatcher_utility::initialize_dispatchers();

  "async_file_writer"_test = [] {
    krbn::async_file_writer::enqueue("tmp/example", "example1", 0755, 0600);
    krbn::async_file_writer::enqueue("tmp/example", "example2", 0755, 0600);
    krbn::async_file_writer::enqueue("tmp/example", "example3", 0755, 0600);
    krbn::async_file_writer::enqueue("tmp/mode666", "mode666", 0755, 0666);
    krbn::async_file_writer::enqueue("tmp/mode644", "mode644", 0755, 0644);
    krbn::async_file_writer::enqueue("tmp/not_found/example", "example", 0755, 0600);
    krbn::async_file_writer::wait();
  };

  return 0;
}
