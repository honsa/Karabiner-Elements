#include "dispatcher_utility.hpp"
#include "observer/main/agent.hpp"
#include "observer/main/daemon.hpp"
#include "run_loop_thread_utility.hpp"
#include <pqrs/osx/process_info.hpp>

int main(int argc, const char* argv[]) {
  //
  // Initialize
  //

  auto scoped_dispatcher_manager = krbn::dispatcher_utility::initialize_dispatchers();
  auto scoped_run_loop_thread_manager = krbn::run_loop_thread_utility::initialize_shared_run_loop_thread();

  signal(SIGUSR1, SIG_IGN);
  signal(SIGUSR2, SIG_IGN);

  pqrs::osx::process_info::enable_sudden_termination();

  //
  // Check euid
  // (observer is launched from LaunchDaemons (root) and LaunchAgents (user).)
  //

  bool root = (geteuid() == 0);

  if (root) {
    return krbn::observer::main::daemon();
  } else {
    return krbn::observer::main::agent();
  }
}
