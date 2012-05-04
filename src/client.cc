#include <l4/re/env>
#include <l4/cxx/ipc_stream>
#include <cstring>
#include <l4/cxx/iostream>

using namespace L4;
using namespace L4Re;
using namespace std;

int main()
{
  Cap<void> server = Env::env()->get_cap<void>("my_client_side");
  if (!server.is_valid()) {
    cout << "Could not get 'my_client_side' capability!\n";
    return 1;
  }

  const char *str = "Whou, a message from outer space!\n";

  Ipc::Iostream ios(l4_utcb());
  ios << Ipc::Buf_cp_out<const char>(str, strlen(str));
  l4_msgtag_t tag = ios.call(server.cap());
  if (l4_ipc_error(tag, l4_utcb())) {
    cout << "ERROR on making a call\n";
    return 1;
  }  

  return 0;
}
