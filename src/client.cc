#include <l4/re/env>
#include <l4/cxx/ipc_stream>
#include <cstring>
#include <l4/cxx/iostream>

using namespace L4;
using namespace L4Re;
using namespace std;

int main()
{
  const char *str = "Whou, a message from outer space!\n";
  Ipc::Iostream ios(l4_utcb());
  ios << Ipc::Buf_cp_out<const char>(str, strlen(str));
  Cap<void> server = Env::env()->get_cap<void>("my_client_side");
  ios.call(server.cap());
  return 0;
}
