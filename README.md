# L4Re simple client server

Example code to demonstrate simple client server code in L4Re.

## Configure, Build and Run

```sh
export L4DIR=~/src/l4
export O=~/build/l4re
make
```

## Notes

### Introduction
* we want to understand more deeply a simple setup
* we write a simple server and a client, which are just sending numbers between each other and print them
* while writing it, we focus on the purpose of each method and object and its role in L4Re

### overview
* the simple-client sends a number to the simple-server, which prints the received number and sends the number multiplied by two back to the client, which prints the received number also

* to do this we have to go through different steps
* the number has to be put into the [UTCB]
* we need to say, that this number should be send to the server
* the server should be ready to receive the number
* the number should than be send by the client
* the number should be processed by the server
* the doubled number should be send back and processed by the client

* we will first go through each line in the ```main``` function of the client, which
* puts the number into the utcb
* sends it to the correct server
* receives the answer
* and prints it

* afterwards we look at the ```dispatch``` method, which
* prints the number
* and answers with the double of the number

* finally we see the ```main``` function of the server, which
* prepares the server to be ready for receiving numbers

### ned.lua
```lua
require("L4")
local ld = L4.default_loader
local channel = ld:new_channel()
ld:start({
            caps = {
               my_server_side = channel:svr()
            },
         }, "rom/simple-server")
ld:start({
            caps = {
               my_client_side = channel
            },
         }, "rom/simple-client")
```

* for this example we ```start``` two tasks, the simple-server and simple-client task
* they are connected via ```channel```, an IPC Gate, with each other
* ```my_server_side``` is the name of the capability selector in the simple-server task, which gets full access to the IPC Gate
* the full access was granted cause of the ```svr``` method
* the capability selector in the other task is named ```my_client_side``` and gets the least priviliges (TODO what is least?)
* these capability selectors are named to retrieve them later from the ```caps``` table

### client.cc

```  Cap<void> server = Env::env()->get_cap<void>("my_client_side");  ```
* we get the capability selector [Cap], which we declared for the simple-client in its caps table in the ```ned.lua``` file
* therefor we use the ```Env::env()``` object, the [Initial Environment] of this task given by Ned
* with this object, we can access the caps table with ```get_cap```
* the returned [Cap] will be used to refer to the server, the receiver of this message

```  Ipc::Iostream ios(l4_utcb());  ```
* the ```Ipc::Iostream ios``` handles the values in the UTCB and calls the kernel for sending these values and receiving other
* it is basically a wrapper around the kernel calls and aroung the placement of values in the UTCB
* in the constructor of ```ios``` we provide the UTCB in which the ```Iostream``` should write and read the values
* with ```l4_utcb()``` we get current UTCB of the running thread
* 

```  ios << n;  ```
* ```ios``` places the number in the UTCB

```  ios.call(server.cap());  ```
* ```call``` will send and than receive data.
* first it sends the data stored in the UTCB to the thread were the server capability comes from
* second it blocks the current thread and makes a closed wait for the thread behind the server capability
* ```cap()``` just returns the capability selector from the ```Cap```
* this method unblocks the thread if an IPC arrives

```  ios >> n;  ```
* after the answer from server has arrived, we read some number out of it

```  cout << n << "\n";  ```
* and finally we print this number

### server.cc

  struct SimpleServer : Server_object
* we define our SimpleServer as a Server_object and need to implement its very simple interface

  int dispatch(l4_umword_t, Ipc::Iostream &ios)
* dispatch is the only method needed to implement the interface of Server_object
* we discard the first argument (TODO what is the first parameter)
* the second argument is more interesting
* it is an already set up Ipc::Iostream, which points to the UTCB containing the earliear

    int n;
    ios >> n;
    cout << n << "\n";
    ios << n * 2;
    return L4_EOK;


###

  SimpleServer simple_server;
* we create an instance of or own SimpleServer, which has a method which reads a number, doubles it and sends the doubled number back

  Util::Registry_server<> server;
  Cap<void> cap = server.registry()->register_obj(&simple_server, "my_server_side");
  server.loop();
  return 0;


### server.cc
### Conclusion
