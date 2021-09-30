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

#ifndef __TCPTWONODES_NODE_H_
#define __TCPTWONODES_NODE_H_

#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"

#include "msg_m.h"
#include <vector>
#include <set>
#include <map>

#include "base.h"

#include <random>
#include <bitset>

#include <chrono>
#include "inet/common/socket/SocketMap.h"

using namespace std;
using namespace std::chrono;

/**
 * TODO - Generated class
 */
namespace inet {

/**
 * Base class for clients app for TCP-based request-reply protocols or apps.
 * Handles a single session (and TCP connection) at a time.
 *
 * It needs the following NED parameters: localAddress, localPort, connectAddress, connectPort.
 */
class Node : public ApplicationBase, public TcpSocket::ICallback
{
  public:
    static map<L3Address, int> addrToId;
    cMessage **processLinkTimers = nullptr; // An array of timers, one per possible destination (instantiated only for the neighbors)
    vector< pair<Packet *, SimTime> > *pendingMsgsPerNeighbors = nullptr;

  protected:
    TcpSocket socketL;
    SocketMap socketMap;
    TcpSocket* socketV[5];

    // statistics
    int numSessions;
    int numBroken;
    int packetsSent;
    int packetsRcvd;
    int bytesSent;
    int bytesRcvd;

    //statistics:
    static simsignal_t connectSignal;

    //ludo
    int localPort;
    int roundId;
    cMessage *roundEvent = nullptr;
    int selfId;
    int nodesNbr;
    vector<L3Address> nodesAddr;
    simtime_t startTime;

    vector<Msg*> receivedMsg;





  public:
      Node() { }
      virtual ~Node() { }

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    /* Utility functions */
    virtual void connect();
    virtual void close();
    virtual void sendPacket(Packet *pkt);
    virtual void sendP(Packet * pk, int destId);
    virtual void sendBack(int destId);

    virtual void handleTimer(cMessage *msg);

    /* TcpSocket::ICallback callback methods */
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
    virtual void socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo) override { socket->accept(availableInfo->getNewSocketId()); }
    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketPeerClosed(TcpSocket *socket) override;
    virtual void socketClosed(TcpSocket *socket) override;
    virtual void socketFailure(TcpSocket *socket, int code) override;
    virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) override { }
    virtual void socketDeleted(TcpSocket *socket) override {}

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void sendTo(Msg * bp, vector<int> ids);
};

} // namespace inet

#endif
