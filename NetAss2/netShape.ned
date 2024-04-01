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
        server0 : Node {
            @display("p=50,70");
        }
        server1 : Node {
            @display("p=50,120");
        }
        serverMalicious2 : Node {
            @display("p=50,170");
        }
        serverMalicious3 : Node {
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
        server7 : Node {
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
    connections:
        client0.gate++ <--> Channel <--> server0.gate++;
        client0.gate++ <--> Channel <--> server1.gate++;
        client0.gate++ <--> Channel <--> serverMalicious2.gate++;
        client0.gate++ <--> Channel <--> serverMalicious3.gate++;
        client0.gate++ <--> Channel <--> server4.gate++;
        client0.gate++ <--> Channel <--> server5.gate++;
        client0.gate++ <--> Channel <--> server6.gate++;
        client0.gate++ <--> Channel <--> server7.gate++;
        client0.gate++ <--> Channel <--> server8.gate++;
        client0.gate++ <--> Channel <--> server9.gate++;
}