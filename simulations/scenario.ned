package tcptwonodes.simulations;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import ned.DatarateChannel;

network scenario
{
    parameters:
        int n;
        @display("bgb=939.4312,489.4875");
        
    types:
        channel fiberline extends ThruputMeteringChannel
        {
            delay = 1us;
            datarate = 512Mbps;
            //thruputDisplayFormat = "u";
        }
        channel ethernetline extends DatarateChannel//ThruputMeteringChannel
        {
            delay = 0.5ms;
            datarate = 1Mbps;
            //thruputDisplayFormat = "u";
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s");
        }
        client[n]: StandardHost {
            parameters:
                @display("i=device/laptop"); //;p=250,100,col,100");
        }

    connections:
        for i=0..n-1, for j=i+1..n-1 {
			client[i].pppg++ <--> ethernetline <--> client[j].pppg++;
        }
        
}


