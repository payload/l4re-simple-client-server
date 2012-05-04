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