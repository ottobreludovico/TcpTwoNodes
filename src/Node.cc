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

#include "Node.h"
#include "base.h"
#include "packets_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"

#include <math.h> // ceil
#include <string>
#include <map>

#include <bits/stdc++.h>

#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;


namespace inet {

simsignal_t Node::connectSignal = registerSignal("connect");

Define_Module(Node);
map<L3Address, int> Node::addrToId;

void Node::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    roundId = 0;

    if (stage == INITSTAGE_LOCAL) {
        numSessions = numBroken = packetsSent = packetsRcvd = bytesSent = bytesRcvd = 0;

        WATCH(numSessions);
        WATCH(numBroken);
        WATCH(packetsSent);
        WATCH(packetsRcvd);
        WATCH(bytesSent);
        WATCH(bytesRcvd);
    }else if (stage == INITSTAGE_APPLICATION_LAYER) {
        const char *localAddress = par("localAddress");
        int localPortL = par("localPortL");
        socketL.setOutputGate(gate("socketOut"));
        socketL.bind(localAddress[0] ? L3AddressResolver().resolve(localAddress) : L3Address(), localPortL);
        socketL.listen();
    }
}

void Node::handleTimer(cMessage *msg) {
    EV << msg;
}


void Node::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()){
        EV << "self message\n";
        EV << "selfId: " << selfId << "\n";
        if(roundId==0){
            if(selfId == 0){
               BriefPacket * bp = new BriefPacket();
               bp->setLinkSenderId(selfId);
               bp->setBroadcasterId(selfId);
               bp->setMsgId(0);
               bp->setMsgType(ECHO);

               vector<int> nids; // Send to neighbors
               for (int j = 0; j < nodesNbr; j++) {
                   nids.push_back(j);
               }
               sendTo(bp, nids);
               roundId++;
            }
        }else { // Event that signals that a message is ready to be sent on a channel
           int destId;
           std::istringstream iss (msg->getName());
           iss >> destId;

           EV<< "destId: " << destId << "\n";

           //            sort(pendingMsgsPerNeighbors[destId].begin(), pendingMsgsPerNeighbors[destId].end(), sortbysec);
           Packet *pk = pendingMsgsPerNeighbors[destId].front().first;

           // --- PROBLEMA ---
           pendingMsgsPerNeighbors[destId].erase(pendingMsgsPerNeighbors[destId].begin());
           // --- PROBLEMA ---

           if (pendingMsgsPerNeighbors[destId].size() > 0) {
               scheduleAt(pendingMsgsPerNeighbors[destId][0].second, msg);
           }

           //cout << selfId << " sending message with debugid = " << ((BriefPacket *) bp->peekAtBack().get())->getDebugId() << endl;
           socket.send(pk);
       }
       EV<<"fuori if\n";

    }
    else
        socket.processMessage(msg);
}

void Node::sendTo(BriefPacket * bp, vector<int> ids){
    bytesSent += 8*B(bp->getChunkLength()).get();
    packetsSent++;
    EV << "\n\n\n\n\n INVIO \n\n\n\n\n\n";
    for (int destId : ids) {
        if (selfId == destId) continue;
        bp->setIncludeData(false);
        if (bp->getMsgType() == ECHO) {
            // One could reduce a bit the consumption by removing the echoOrReadySender in the Send message (using another message type)
            if (bp->getEchoOrReadySender() == bp->getBroadcasterId()) { // SEND message
                bp->setChunkLength(B( ceil( (MSG_TYPE_SIZE+(PROCESS_ID_SIZE+MSG_ID_SIZE)+PATH_LENGTH_SIZE + PAYLOAD_SIZE)/8 ))); //+PROCESS_ID_SIZE*bp->getPathArraySize()));
                bp->setIncludeData(true);
            } else {
                bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE+MSG_ID_SIZE)+PROCESS_ID_SIZE + PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize()+PAYLOAD_SIZE)/8.0)));
                bp->setIncludeData(true);

            }
        } else if (bp->getMsgType() == READY) { // To resume from here
            bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE+MSG_ID_SIZE)+PROCESS_ID_SIZE + PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize()+PAYLOAD_SIZE)/8.0)));
            bp->setIncludeData(true);

        } else if (bp->getMsgType() == READY_ECHO) {
            bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE+MSG_ID_SIZE)+PROCESS_ID_SIZE+PROCESS_ID_SIZE+PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize()+PAYLOAD_SIZE)/8.0)));
            bp->setIncludeData(true);

        } else if (bp->getMsgType() == ECHO_ECHO) {
            bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE+MSG_ID_SIZE)+PROCESS_ID_SIZE+PROCESS_ID_SIZE+PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize()+PAYLOAD_SIZE)/8.0)));
            bp->setIncludeData(true);

        } else if (bp->getMsgType() == PAYLOAD_ACK) {
            bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+LOCAL_ID_SIZE)/8.0) ));
        } else {
            cout << "sendTo: msgType not supported!" << endl;
            return;
        }


        Packet *briefPacket;
        if (bp->getMsgType() == ECHO) {
            briefPacket = new Packet("ECHO");
        } else if (bp->getMsgType() == READY) {
            briefPacket = new Packet("READY");
        } else if (bp->getMsgType() == READY_ECHO) {
            briefPacket = new Packet("READY_ECHO");
        } else if (bp->getMsgType() == ECHO_ECHO) {
            briefPacket = new Packet("ECHO_ECHO");
        } else if (bp->getMsgType() == PAYLOAD_ACK) {
            briefPacket = new Packet("PAYLOAD_ACK");
        }


        BriefPacket &tmp = *bp;
        const auto& payload = makeShared<BriefPacket>(tmp);
        briefPacket->insertAtBack(payload);
        SimTime emissionTime = simTime();

        vector<pair<Packet *, SimTime>>::iterator iter = pendingMsgsPerNeighbors[destId].begin();
        while (iter != pendingMsgsPerNeighbors[destId].end() && iter->second < emissionTime) {
            iter++;
        }
        pendingMsgsPerNeighbors[destId].insert(iter, make_pair(briefPacket, emissionTime));
        EV << "pending1: " << pendingMsgsPerNeighbors[destId].front().first << "\n";
        SimTime emissionFirst = pendingMsgsPerNeighbors[destId].front().second;

        if (!processLinkTimers[destId]->isScheduled()) {
            scheduleAt(emissionFirst, processLinkTimers[destId]);
        } else {
            cancelEvent(processLinkTimers[destId]);
            scheduleAt(emissionFirst, processLinkTimers[destId]);
        }
    }
}

void Node::connect()
{
    // we need a new connId if this is not the first connection
    socket.renewSocket();

    const char *localAddress = par("localAddress");
    int localPort = par("localPort");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);

    int timeToLive = par("timeToLive");
    if (timeToLive != -1)
        socket.setTimeToLive(timeToLive);

    int dscp = par("dscp");
    if (dscp != -1)
        socket.setDscp(dscp);

    int tos = par("tos");
    if (tos != -1)
        socket.setTos(tos);

    // connect
    const char *connectAddress = par("connectAddress");
    int connectPort = par("connectPort");

    L3Address destination;
    L3AddressResolver().tryResolve(connectAddress, destination);
    if (destination.isUnspecified()) {
        EV_ERROR << "Connecting to " << connectAddress << " port=" << connectPort << ": cannot resolve destination address\n";
    }
    else {
        EV_INFO << "Connecting to " << connectAddress << "(" << destination << ") port=" << connectPort << endl;

        socket.connect(destination, connectPort);

        numSessions++;
        emit(connectSignal, 1L);
    }
}

void Node::close()
{
    EV_INFO << "issuing CLOSE command\n";
    socket.close();
    emit(connectSignal, -1L);
}

void Node::sendPacket(Packet *msg)
{
    int numBytes = msg->getByteLength();
    emit(packetSentSignal, msg);
    socket.send(msg);

    packetsSent++;
    bytesSent += numBytes;
}

void Node::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();
    getDisplayString().setTagArg("t", 0, TcpSocket::stateName(socket.getState()));
}

void Node::socketEstablished(TcpSocket *)
{
    // *redefine* to perform or schedule first sending
    EV_INFO << "connected\n";
}

void Node::socketDataArrived(TcpSocket *, Packet *msg, bool)
{
    // *redefine* to perform or schedule next sending
    packetsRcvd++;
    bytesRcvd += msg->getByteLength();
    emit(packetReceivedSignal, msg);
    delete msg;
}

void Node::socketPeerClosed(TcpSocket *socket_)
{
    ASSERT(socket_ == &socket);
    // close the connection (if not already closed)
    if (socket.getState() == TcpSocket::PEER_CLOSED) {
        EV_INFO << "remote TCP closed, closing here as well\n";
        close();
    }
}

void Node::socketClosed(TcpSocket *)
{
    // *redefine* to start another session etc.
    EV_INFO << "connection closed\n";
}

void Node::socketFailure(TcpSocket *, int code)
{
    // subclasses may override this function, and add code try to reconnect after a delay.
    EV_WARN << "connection broken\n";
    numBroken++;
}

void Node::finish()
{
    std::string modulePath = getFullPath();

    EV_INFO << modulePath << ": opened " << numSessions << " sessions\n";
    EV_INFO << modulePath << ": sent " << bytesSent << " bytes in " << packetsSent << " packets\n";
    EV_INFO << modulePath << ": received " << bytesRcvd << " bytes in " << packetsRcvd << " packets\n";
}

void Node::handleStopOperation(LifecycleOperation *operation) {
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void Node::handleCrashOperation(LifecycleOperation *operation) {
    socket.destroy();    //TODO  in real operating systems, program crash detected by OS and OS closes sockets of crashed programs.
}

void Node::handleStartOperation(LifecycleOperation *operation) {
    simtime_t startTimePar = par("startTimePar");
    simtime_t startTime = std::max(startTimePar, simTime());

    localPort=par("localPort");
    socket.setOutputGate(gate("socketOut"));
    socket.bind(localPort);
    socket.setCallback(this);

    const char *connectAddress = par("connectAddress");
    int connectPort = par("connectPort");

    L3Address destination;
    L3AddressResolver().tryResolve(connectAddress, destination);
    if (destination.isUnspecified()) {
        EV_ERROR << "Connecting to " << connectAddress << " port=" << connectPort << ": cannot resolve destination address\n";
    }
    else {
        EV_INFO << "Connecting to " << connectAddress << "(" << destination << ") port=" << connectPort << endl;

        socket.connect(destination, connectPort);

        numSessions++;
        emit(connectSignal, 1L);

        nodesNbr=2;
            if (this->addrToId.size() == 0) {
                for (int nodeId = 0; nodeId < nodesNbr; nodeId++) {
                    const char * address = ("client"+to_string(nodeId)).c_str();
                    L3Address addr = L3AddressResolver().resolve(address);
                    this->addrToId[addr] = nodeId;
                }
            }

            L3Address selfAddr = L3AddressResolver().addressOf(getParentModule(),L3AddressResolver().ADDR_IPv4);
            selfId = addrToId[selfAddr];

            roundEvent = new cMessage;
            scheduleAt(startTime, roundEvent);

            pendingMsgsPerNeighbors = new vector<pair<Packet *, SimTime>>[nodesNbr];
            processLinkTimers = new cMessage*[nodesNbr];
            for (int i = 0; i < nodesNbr; i++) {
               processLinkTimers[i] = new cMessage(std::to_string(i).c_str());
               //std::cout << selfId << " ** creating channel timer (id=" << channelTimers[i]->getId() << ") for " << i << std::endl;
            }

     }


}


} // namespace inet
