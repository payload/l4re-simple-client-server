#include <l4/re/env>
#include <l4/cxx/ipc_stream>
#include <l4/cxx/iostream>

using namespace L4;
using namespace L4Re;

int main()
{
  int n = 21;
  Cap<void> server = Env::env()->get_cap<void>("my_client_side");
  Ipc::Iostream ios(l4_utcb());
  ios << n;
  ios.call(server.cap());
  ios >> n;
  cout << n << "\n";
  return 0;
}
