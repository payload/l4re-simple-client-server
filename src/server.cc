#include <l4/re/util/object_registry>
#include <l4/cxx/iostream>

using namespace L4;
using namespace L4Re;

struct SimpleServer : Server_object
{
  int dispatch(l4_umword_t, Ipc::Iostream &ios)
  {
    int n;
    ios >> n;
    cout << n << "\n";
    ios << n * 2;
    return L4_EOK;
  }
};

int main()
{
  SimpleServer simple_server;
  Util::Registry_server<> server;
  Cap<void> cap = server.registry()->register_obj(&simple_server, "my_server_side");
  server.loop();
  return 0;
}
