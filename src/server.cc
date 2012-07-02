#include <l4/re/util/object_registry>
#include <iostream>

using namespace L4;
using namespace L4Re;

struct SimpleServer : Server_object
{
  int dispatch(l4_umword_t, Ipc::Iostream &ios)
  {
    int n;
    ios >> n;
    std::cout << n << std::endl;
    ios << n * 2;
    return L4_EOK;
  }
};

int main()
{
  SimpleServer simple_server;
  Util::Registry_server<> server;
  Cap<void> cap = server.registry()->register_obj(&simple_server, "my_server_side");
  if (!cap.is_valid())
  {
      std::cerr << "invalid cap on register_obj 'my_server_side'" << std::endl;
      return 1;
  }
  server.loop();
  return 0;
}
