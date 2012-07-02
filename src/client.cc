#include <l4/re/env>
#include <l4/cxx/ipc_stream>
#include <iostream>

using namespace L4;
using namespace L4Re;

int main()
{
  int n = 21;
  Cap<void> server = Env::env()->get_cap<void>("my_client_side");
  if (!server.is_valid())
  {
      std::cerr << "no valid cap to 'my_client_side'" << std::endl;
      return 1;
  }
  Ipc::Iostream ios(l4_utcb());
  ios << n;
  long err = l4_error(ios.call(server.cap()));
  if (err)
  {
      std::cerr << "calling server: " << err << std::endl;
      return 1;
  }
  ios >> n;
  std::cout << n << std::endl;
  return 0;
}
