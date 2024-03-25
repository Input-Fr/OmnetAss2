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
server2 : Node {
       @display("p=50,170");
   }
client0 : Node {
     @display("p=250,70");
 }
connections:
client0.gate++ <--> Channel <--> server0.gate++;
client0.gate++ <--> Channel <--> server1.gate++;
client0.gate++ <--> Channel <--> server2.gate++;
}