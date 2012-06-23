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
We want to understand more deeply the following simple setup. A simple server and a client, which are just sending numbers between each other and print them. While writing it, we focus on the purpose of each method and object and its role in L4Re.

### overview
Our two tasks are called simple-client and simple-server. The simple-client sends a number to the simple-server, which prints the received number and sends the number multiplied by two back to the client, which prints the received number also.

To do this we have to go through different steps:
* the number has to be put into the UTCB
* we need to say, that this number should be send to the server
* the server should be ready to receive the number
* the number should than be send by the client
* the number should be processed by the server
* the doubled number should be send back
* and the finally received number should be processed by the client

We will first go through each line in the ```main``` function of the client, which puts the number into the utcb, sends it to the correct server, receives the answer and prints it.

Afterwards we look at the ```dispatch``` method, which prints the number and answers with the double of the number.

Finally we see the ```main``` function of the server, which prepares the server to be ready for receiving numbers.

### ned.lua

First we look at the configuration file which sets up both tasks.

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

For this example we ```start``` two tasks, the simple-server and simple-client task. They are connected via ```channel```, an IPC Gate, with each other. ```my_server_side``` is the name of the capability selector in the simple-server task, which gets full access to the IPC Gate. The full access was granted cause of the ```svr``` method. The capability selector in the other task is named ```my_client_side``` and gets the least priviliges (TODO what is least?). These capability selectors are named to retrieve them later from the ```caps``` table.

### client.cc

We now want to look at the straight forward client.

```  Cap<void> server = Env::env()->get_cap<void>("my_client_side");  ```
We get the capability selector, which we declared for the simple-client in its caps table in the ```ned.lua``` file. Therefor we use the ```Env::env()``` object, the [Initial Environment](http://os.inf.tu-dresden.de/L4Re/doc/group__api__l4re__env.html) of this task given by Ned. With this object, we can access the caps table with ```get_cap```. The returned ```Cap``` will be used to refer to the server, the receiver of this message.

```  Ipc::Iostream ios(l4_utcb());  ```
The ```Ipc::Iostream ios``` handles the values in the UTCB and calls the kernel for sending these values and receiving other. It is basically a wrapper around the kernel calls and around the placement of values in the UTCB. In the constructor of ```ios``` we provide the UTCB in which the ```Iostream``` should write and read the values. With ```l4_utcb()``` we get the current UTCB of the running thread.

```  ios << n;  ```
This writes the number into the UTCB at the current location as managed by ```ios```. The current location here is the first and it is moved after writing the number.

```  ios.call(server.cap());  ```
```call``` will send and than receive data. First it sends the data stored in the UTCB to the thread were the server capability comes from. Second it blocks the current thread and makes a closed wait for the thread behind the server capability. ```cap()``` just returns the capability selector from the ```Cap```. The thread will be unblocked if an IPC arrives, in this case the answer of the simple-server containing a number.

```  ios >> n;  ```
After the answer from server has arrived, we read some number out of it.
<!-- TODO is the request number still there or how do i explain the position management in ios? -->

```  cout << n << "\n";  ```
And finally we print this number. Done.

### server.cc

The more complex server contains a very important pattern. It defines a ```Server_object``` which does the actual work of answering requests and a ```Registry_server``` which dispatches IPCs to different ```Server_object```s.

```  struct SimpleServer : Server_object  ```
We define our ```SimpleServer``` as a ```Server_object``` and need to implement its very simple interface.

```  int dispatch(l4_umword_t, Ipc::Iostream &ios)  ```
```dispatch``` is the only method needed to implement the interface of ```Server_object```. We discard the first argument, cause we don't need it. <!-- TODO what is the first parameter? --> The second argument is more interesting. It is an already set up ```Ipc::Iostream```, which points to the UTCB containing the earliear send number.

```  ios >> n;  ```
We can read this number. Particularly ```sizeof(n)``` bytes are read from the UTCB to ```n``` from the current position and the current position is forward by this number of bytes. No we can print the number and send the answer.

```
    ios << n * 2;
    return L4_EOK;
```
The answer is written to (TODO which position) and with ```L4_EOK``` we signal to the one who called ```dispatch``` that everything is alright. (TODO other return values)

In our ```main``` function we set up our server and the ```Server_object``` we implemented and let the server loop forever for accepting incoming IPCs and answering them.

```  Util::Registry_server<> server;  ```
We instantiate a ```Registry_server``` which will be the heart of our server task. It maintains a registry, where objects of type ```Server_object``` are registered and has the ```loop``` method, which accepts an incoming IPC containing the capability selector used to identify the registered ```Server_object```. Now the ```Server_object``` can be found in the registry and the incoming IPC can be forwarded as an ```Iostream``` to the ```dispatch``` method of the ```Server_object```, the method above in this file.

```  Cap<void> cap = server.registry()->register_obj(&simple_server, "my_server_side");  ```
This is the call which actually registers the our instance of ```SimpleServer``` in the ```registry()``` of the ```Registry_server```. As we already saw in the ```ned.lua``` file, there is the name ```"my_server_side"``` and we use it here to say that whenever an IPC is made to the IPC-Gate named ```"my_server_side"``` the IPC is forwarded to the ```dispatch``` method of ```simple_server```.

Now we have pretty much set up everything we need to let interaction of a client and a server happen.

```  server.loop();  ```
We can now loop forever and deal with IPCs by forwarding them to our own ```Server_object``` and send our answer back.

### Conclusion
