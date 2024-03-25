simple Node
{
    parameters:
        @display("i=block/routing");
    gates:
        inout gate[];  // declare two way connections 
}

network Network
{
    @display("bgb=500,500");
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100ms;
        }
    submodules:
        server0: Node {
            @display("p=50,70");
        }
        server1: Node {
            @display("p=50,120");
        }
        server2: Node {
            @display("p=50,170");
        }
        server3: Node {
            @display("p=50,220");
        }
        server4: Node {
            @display("p=50,270");
        }
        server5: Node {
            @display("p=50,320");
        }
        server6: Node {
            @display("p=50,370");
        }
        server7: Node {
            @display("p=50,420");
        }
        server8: Node {
            @display("p=50,470");
        }
        server9: Node {
            @display("p=50,520");
        }
        client0: Node {
            @display("p=318.528,30.240002");
        }
        client1: Node {
            @display("p=328.608,108.864006");
        }
        client2: Node {
            @display("p=392.112,219.744");
        }
        client3: Node {
            @display("p=308.448,319.536");
        }
        client4: Node {
            @display("p=239.904,450.57602");
        }
    connections:
        client0.gate++ <--> Channel <--> client1.gate++;
        client0.gate++ <--> Channel <--> client2.gate++;
        client0.gate++ <--> Channel <--> client3.gate++;
        client0.gate++ <--> Channel <--> client4.gate++;
        client0.gate++ <--> Channel <--> server0.gate++;
        client0.gate++ <--> Channel <--> server1.gate++;
        client0.gate++ <--> Channel <--> server2.gate++;
        client0.gate++ <--> Channel <--> server3.gate++;
        client0.gate++ <--> Channel <--> server4.gate++;
        client0.gate++ <--> Channel <--> server5.gate++;
        client0.gate++ <--> Channel <--> server6.gate++;
        client0.gate++ <--> Channel <--> server7.gate++;
        client0.gate++ <--> Channel <--> server8.gate++;
        client0.gate++ <--> Channel <--> server9.gate++;
        client1.gate++ <--> Channel <--> client2.gate++;
        client1.gate++ <--> Channel <--> client3.gate++;
        client1.gate++ <--> Channel <--> client4.gate++;
        client1.gate++ <--> Channel <--> server0.gate++;
        client1.gate++ <--> Channel <--> server1.gate++;
        client1.gate++ <--> Channel <--> server2.gate++;
        client1.gate++ <--> Channel <--> server3.gate++;
        client1.gate++ <--> Channel <--> server4.gate++;
        client1.gate++ <--> Channel <--> server5.gate++;
        client1.gate++ <--> Channel <--> server6.gate++;
        client1.gate++ <--> Channel <--> server7.gate++;
        client1.gate++ <--> Channel <--> server8.gate++;
        client1.gate++ <--> Channel <--> server9.gate++;
        client2.gate++ <--> Channel <--> client3.gate++;
        client2.gate++ <--> Channel <--> client4.gate++;
        client2.gate++ <--> Channel <--> server0.gate++;
        client2.gate++ <--> Channel <--> server1.gate++;
        client2.gate++ <--> Channel <--> server2.gate++;
        client2.gate++ <--> Channel <--> server3.gate++;
        client2.gate++ <--> Channel <--> server4.gate++;
        client2.gate++ <--> Channel <--> server5.gate++;
        client2.gate++ <--> Channel <--> server6.gate++;
        client2.gate++ <--> Channel <--> server7.gate++;
        client2.gate++ <--> Channel <--> server8.gate++;
        client2.gate++ <--> Channel <--> server9.gate++;
        client3.gate++ <--> Channel <--> client4.gate++;
        client3.gate++ <--> Channel <--> server0.gate++;
        client3.gate++ <--> Channel <--> server1.gate++;
        client3.gate++ <--> Channel <--> server2.gate++;
        client3.gate++ <--> Channel <--> server3.gate++;
        client3.gate++ <--> Channel <--> server4.gate++;
        client3.gate++ <--> Channel <--> server5.gate++;
        client3.gate++ <--> Channel <--> server6.gate++;
        client3.gate++ <--> Channel <--> server7.gate++;
        client3.gate++ <--> Channel <--> server8.gate++;
        client3.gate++ <--> Channel <--> server9.gate++;
        client4.gate++ <--> Channel <--> server0.gate++;
        client4.gate++ <--> Channel <--> server1.gate++;
        client4.gate++ <--> Channel <--> server2.gate++;
        client4.gate++ <--> Channel <--> server3.gate++;
        client4.gate++ <--> Channel <--> server4.gate++;
        client4.gate++ <--> Channel <--> server5.gate++;
        client4.gate++ <--> Channel <--> server6.gate++;
        client4.gate++ <--> Channel <--> server7.gate++;
        client4.gate++ <--> Channel <--> server8.gate++;
        client4.gate++ <--> Channel <--> server9.gate++;
}
