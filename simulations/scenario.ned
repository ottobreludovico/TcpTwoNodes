package tcptwonodes.simulations;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;

network scenario
{
    parameters:
        @display("bgb=851,455");
    types:
        channel fiberline extends ThruputMeteringChannel
        {
            delay = 1us;
            datarate = 512Mbps;
            thruputDisplayFormat = "u";
        }
        channel ethernetline extends ThruputMeteringChannel
        {
            delay = 0.5ms;
            datarate = 1Mbps;
            thruputDisplayFormat = "u";
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s");
        }
        client0: StandardHost {
            parameters:
                @display("i=device/laptop;p=245.8125,130.3875"); //;p=250,100,col,100");
        }
        client1: StandardHost {
            parameters:
                @display("i=device/laptop;p=424,69"); //;p=250,100,col,100");
        }
        client2: StandardHost {
            parameters:
                @display("i=device/laptop;p=312.075,291.76874"); //;p=250,100,col,100");
        }
        client3: StandardHost {
            parameters:
                @display("i=device/laptop;p=554.6813,291.76874"); //;p=250,100,col,100");
        }
        client4: StandardHost {
            parameters:
                @display("i=device/laptop;p=619.875,130.3875"); //;p=250,100,col,100");
        }

    connections:
        client0.pppg++ <--> ethernetline <--> client1.pppg++;
        client0.pppg++ <--> ethernetline <--> client2.pppg++;
        client0.pppg++ <--> ethernetline <--> client3.pppg++;
        client0.pppg++ <--> ethernetline <--> client4.pppg++;

        client1.pppg++ <--> ethernetline <--> client2.pppg++;
        client1.pppg++ <--> ethernetline <--> client3.pppg++;
        client1.pppg++ <--> ethernetline <--> client4.pppg++;

        client2.pppg++ <--> ethernetline <--> client3.pppg++;
        client2.pppg++ <--> ethernetline <--> client4.pppg++;

        client3.pppg++ <--> ethernetline <--> client4.pppg++;
}


