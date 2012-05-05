#include <l4/re/util/object_registry>
#include <l4/cxx/iostream>

using namespace L4;
using namespace L4Re;

struct SimpleServer : Server_object
{
  int dispatch(l4_umword_t, Ipc::Iostream &ios)
  {
    unsigned long size;
    const char *buf = NULL;
    ios >> L4::Ipc::Buf_in<const char>(buf, size);
    cout << buf;
    return 0;
  }
};

int main()
{
  Util::Registry_server<> server; 
  SimpleServer simple_server;
  Cap<void> cap = server.registry()->register_obj(&simple_server, "my_server_side");
  server.loop();
  return 0;
}
