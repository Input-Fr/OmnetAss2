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
        serverMalicious0 : Node {
            @display("p=50,70");
        }
        server1 : Node {
            @display("p=50,120");
        }
        server2 : Node {
            @display("p=50,170");
        }
        server3 : Node {
            @display("p=50,220");
        }
        server4 : Node {
            @display("p=50,270");
        }
        server5 : Node {
            @display("p=50,320");
        }
        server6 : Node {
            @display("p=50,370");
        }
        serverMalicious7 : Node {
            @display("p=50,420");
        }
        server8 : Node {
            @display("p=50,470");
        }
        server9 : Node {
            @display("p=50,520");
        }
        client0 : Node {
            @display("p=250,70");
        }
        client1 : Node {
            @display("p=250,120");
        }
    connections:
        client0.gate++ <--> Channel <--> client1.gate++;
        client0.gate++ <--> Channel <--> serverMalicious0.gate++;
        client0.gate++ <--> Channel <--> server1.gate++;
        client0.gate++ <--> Channel <--> server2.gate++;
        client0.gate++ <--> Channel <--> server3.gate++;
        client0.gate++ <--> Channel <--> server4.gate++;
        client0.gate++ <--> Channel <--> server5.gate++;
        client0.gate++ <--> Channel <--> server6.gate++;
        client0.gate++ <--> Channel <--> serverMalicious7.gate++;
        client0.gate++ <--> Channel <--> server8.gate++;
        client0.gate++ <--> Channel <--> server9.gate++;
        client1.gate++ <--> Channel <--> serverMalicious0.gate++;
        client1.gate++ <--> Channel <--> server1.gate++;
        client1.gate++ <--> Channel <--> server2.gate++;
        client1.gate++ <--> Channel <--> server3.gate++;
        client1.gate++ <--> Channel <--> server4.gate++;
        client1.gate++ <--> Channel <--> server5.gate++;
        client1.gate++ <--> Channel <--> server6.gate++;
        client1.gate++ <--> Channel <--> serverMalicious7.gate++;
        client1.gate++ <--> Channel <--> server8.gate++;
        client1.gate++ <--> Channel <--> server9.gate++;
}
