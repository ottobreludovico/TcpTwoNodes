//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package tcptwonodes;

//
// TODO auto-generated module
//

import inet.applications.contract.IApp;

//
// Client for adaptive video streaming over TCP. This client requests a server
// 1s of video with different quality level depending on the buffer ocuppation.
// May be used as a rough model of MPEG-DASH.
//
// The model communicates with the server in sessions. During a session,
// the client opens a single TCP connection to the server, sends several
// requests (always waiting for the complete reply to arrive before
// sending a new request), and closes the connection.
//
// The server app should be ~TCPGenericSrvApp; the model sends ~GenericAppMsg
// messages.
//
// <b>Configuring App</b>
//
// See the parameter list to get an overview of the supported options
//
// The module parameter dataTransferMode should be set the transfer mode in TCP layer.
// You should set dataTransferMode to "object"
//
//   -# use "object", which transmits
//      cMessage objects (and subclasses) over a TCP connection. The same
//      message object sequence that was sent by the client to the
//      sender-side TCP entity will be reproduced on the receiver side.
//      If a client sends a cMessage with length = 1 megabyte, the
//      receiver-side client will receive the same message object (or a clone)
//      after the TCP entities have completed simulating the transmission
//      of 1 megabyte over the connection. This is a different behaviour
//      from TCPVirtualDataSendQueue/RcvQueue.
//      This mode is not implemented in ~TCP_NSC yet.
//
//
// @see ~TCPGenericSrvApp, ~GenericAppMsg, ~TelnetApp
//
simple Node like IApp
{
    parameters:
        // General parameters
        @class(inet::Node);
        //double startTimePar @unit("s") = default(0s);
        string 			localAddress = default(""); // may be left empty ("")
        int 			localPort = default(-1); // port number to listen on
        int 			localPortL = default(-1); 
        string 			connectAddress = default("");  // server address (may be symbolic)
        int 			connectPort = default(1000); // port number to connect to
        double 			join = default(-1); // time to call a join req
        double 			leave = default(-1); // time to call a leave req
        double 			broadcast = default(-1); // time to call a broadcast req
        double 			timer = default(3); 
        int 			numMsg = default(0);
        double			sendFrequency = default(5.0);
        double			startSend = default(0.0);
        int nPar; 
        int nNPar;
        int fPar;
        bool debug = default(false);
        @display("i=block/app");
        //@signal[packetSent](type=inet::Packet);
        //@signal[packetReceived](type=inet::Packet);
        //@signal[connect](type=long);  // 1 for open, -1 for close
       
    gates:
        input socketIn @labels(TCPCommand/up);
        output socketOut @labels(TCPCommand/down);
}