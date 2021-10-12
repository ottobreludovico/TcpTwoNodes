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
#include "msg_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/applications/common/SocketTag_m.h"
#include "inet/applications/tcpapp/GenericAppMsg_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"
#include <math.h> // ceil
#include <string>
#include <map>

#include <bits/stdc++.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

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
        startTime = 0;

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
        socketL = TcpSocket();
        socketL.setOutputGate(gate("socketOut"));
        socketL.bind(localAddress[0] ? L3AddressResolver().resolve(localAddress) : L3Address(), localPortL);
        socketL.setCallback(this);
        socketL.listen();
    }
}

void Node::handleTimer(cMessage *msg) {
    EV << msg;
}


void Node::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()){
       /*EV << "self message\n";
       EV << "selfId: " << selfId << "\n";
       EV << "time: " << simTime() << "\n";
       //if(roundId==0){
       const char * x = msg->getName();
       int i = stoi(x);
       if(selfId == i){
           // Event that signals that a message is ready to be sent on a channel
           int destId;
           std::istringstream iss (msg->getName());
           iss >> destId;


           vector<int> nids; // Send to neighbors
           for (int destId = 0; destId < nodesNbr; destId++) {
                if(destId==selfId) continue;
                Msg * bp = new Msg();
                bp->setId(selfId);
                bp->setMsg("RequestLudo");
                bp->setSendTime(simTime());
                auto briefPacket = new Packet();
                Msg &tmp = *bp;
                const auto& payload = makeShared<Msg>(tmp);
                briefPacket->insertAtBack(payload);
                EV << "\n\n\n\n [Client"<< selfId <<"]messaggio inviato a: " << simTime() << "\n\n\n";
                sendP(briefPacket, destId);
           }
       }*/
        switch (msg->getKind()) {
            case MSGKIND_JOIN:

                //mando msg
                for (int destId = 0; destId < nodesNbr; destId++) {
                    if(destId==selfId) continue;
                    if(contain(destId,current_view)==false) continue;
                    Msg * bp = new Msg();
                    bp->setId(selfId);
                    bp->setMsg("Join");
                    bp->setSendTime(simTime());
                    bp->setJoin_or_leave(1);
                    bp->setView(current_view);
                    bp->setType(RECONFIG);
                    auto briefPacket = new Packet();
                    Msg &tmp = *bp;
                    const auto& payload = makeShared<Msg>(tmp);
                    briefPacket->insertAtBack(payload);
                    EV << "\n\n\n\n [Client"<< selfId <<"]messaggio inviato a: " << simTime() << "\n\n\n";
                    sendP(briefPacket, destId);
                }

                //schedulo il reinvio
                if(join_complete==false && rec_cond==false){ //TODO o quorum
                    join(3);
                }
        }
   }
    else{
        //socket.processMessage(msg);
        //socketL.processMessage(msg);
       /* TcpSocket *socket = check_and_cast_nullable<TcpSocket*>(socketMap.findSocketFor(msg));
        if (socket){
            EV << "\n\n\n\n if 1 \n\n\n\n";
            socket->processMessage(msg);
            //socket->close();
            //socketMap.removeSocket(socket);
        }
        else{*/
            //EV << "\n\n\n\n if 2 \n\n\n\n";
            socketL.processMessage(msg);
        //}
    }
}

void Node::sendP(Packet * pk, int destId){
    EV << "\n\n\n Client" << selfId << "\n";
    for(int i=0;i<nodesNbr;i++){
        EV << "Client" << i << " : "  << socketV[i] << "\n";
    }
    EV << "\n\n\n";
    if(socketV[destId]==nullptr){
        localPort=par("localPort");
        TcpSocket * socket = new TcpSocket();
        socket->setOutputGate(gate("socketOut"));
        socket->bind(localPort);
        socket->setCallback(this);

        const char * address = ("client"+to_string(destId)).c_str();
        L3Address destination = L3AddressResolver().resolve(address);
        int connectPort = par("connectPort");

        if (destination.isUnspecified()) {
           EV_ERROR <<  "cannot resolve destination address\n";
        }
        else {
           EV_INFO << "Connecting to " << destination << ") port=" << connectPort << endl;

           socket->connect(destination, connectPort);
           socketV[destId]=socket;
           EV << "\n\n\n Client" << selfId << "\n";
           for(int i=0;i<nodesNbr;i++){
               EV << "Client" << i << " : "  << socketV[i] << "\n";
           }
           EV << "\n\n\n";
           numSessions++;
           emit(connectSignal, 1L);


           bytesSent += pk->getByteLength();
           packetsSent++;
           socket->send(pk);
        }
    }else{
        socketV[destId]->send(pk);
    }

}

void Node::connect()
{
    // we need a new connId if this is not the first connection

}

void Node::close()
{
    EV_INFO << "issuing CLOSE command\n";
    socketL.close();
    emit(connectSignal, -1L);
}

void Node::sendPacket(Packet *msg)
{

}

void Node::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();
    getDisplayString().setTagArg("t", 0, TcpSocket::stateName(socketL.getState()));
}

void Node::socketEstablished(TcpSocket * s)
{
    // *redefine* to perform or schedule first sending
    if(addrToId[s->getLocalAddress()]!=selfId){
        EV << "\n\n\n Client" << selfId << "\n";
            for(int i=0;i<nodesNbr;i++){
                EV << "Client" << i << " : "  << socketV[i] << "\n";
            }
            EV << "\n\n\n";
            //socketV[addrToId[s->getLocalAddress()]]= s;
    }
}

template<class T> bool vectorExist (vector<T> c, T item)
{
    return (std::find(c.begin(), c.end(), item) != c.end());
}


void Node::socketDataArrived(TcpSocket * s, Packet *msg, bool)
{
    // *redefine* to perform or schedule next sending
    packetsRcvd++;
    bytesRcvd += msg->getByteLength();
    emit(packetReceivedSignal, msg);
    Msg* x = (Msg *)msg->peekAtBack().get();
    if(x->getId()!=selfId ){
        //sendBack(x->getId());
        if (std::count(receivedMsg.begin(), receivedMsg.end(), x)) {

        }
        else {
            x->setArrivalTime(simTime());
            receivedMsg.push_back(x);

            if(x->getType()==RECONFIG){
                EV << "\n\n\n\n [Client"<< selfId <<"]RECONFIG ricevuto a: " << simTime() << "\n\n\n";
                if(x->getView()==current_view){
                    if(x->getJoin_or_leave()==1 || (x->getJoin_or_leave()==0 && checkPropose(x->getId(),current_view))){
                        RECV.push_back(std::make_pair(x->getId(),x->getJoin_or_leave()));

                        Msg * bp = new Msg();
                        bp->setId(selfId);
                        bp->setMsg("Join reply");
                        bp->setSendTime(simTime());
                        bp->setView(current_view);
                        bp->setType(REC_CONFIRM);
                        auto briefPacket = new Packet();
                        Msg &tmp = *bp;
                        const auto& payload = makeShared<Msg>(tmp);
                        briefPacket->insertAtBack(payload);
                        EV << "\n\n\n\n [Client"<< selfId <<"]REC-CONFIRM inviato a: " << simTime() << "\n\n\n";
                        sendP(briefPacket, x->getId());
                    }
                    if(contain(x->getId(),RECV)==true){
                        uponRECV();
                    }
                }
            }else if(x->getType()==REC_CONFIRM){
                //TODO fare array e salvarli solo una volta
                EV << "\n\n\n\n [Client"<< selfId <<"]REC-CONFIRM ricevuto a: " << simTime() << "\n\n\n";
                if(req_join_or_leave[x->getId()]==0){
                    req_join_or_leave[x->getId()]=1;
                    count_req_join_or_leave++;
                    if(count_req_join_or_leave==3){ //TODO quorum
                        rec_cond=true; //non devo più inviare richieste
                    }
                }
            }else if(x->getType()==PROPOSE){
                update(x->getSEQcv());
                EV << "\n\n\n\n\n\n\n\n";
                for(auto i : propose){
                    EV << "count : " << i.second << "\n";
                }
                EV << "\n\n\n\n\n\n\n\n";
                if(checkPropose()==true){
                    EV << "\n\n\n\n\n\n\n\n dajeeeee \n\n\n\n\n\n\n";
                    LCSEQv = returnPropose();
                    for (int destId = 0; destId < nodesNbr; destId++) {
                      if(destId==selfId) continue;
                      if(contain(destId,x->getView())==false) continue;
                      Msg * bp = new Msg();
                      bp->setId(selfId);
                      bp->setMsg("Propose");
                      bp->setSendTime(simTime());
                      bp->setView(current_view);
                      bp->setType(CONVERGED);
                      bp->setSEQcv(SEQv);
                      auto briefPacket = new Packet();
                      Msg &tmp = *bp;
                      const auto& payload = makeShared<Msg>(tmp);
                      briefPacket->insertAtBack(payload);
                      EV << "\n\n\n\n [Client"<< selfId <<"]CONVERGED inviato a: " << simTime() << "\n\n\n";
                      sendP(briefPacket, destId);
                    }
                }
                else if(FORMATv.empty()==true || isContainedIn(x->getSEQcv(),FORMATv)){
                    //TODO check if SEQ is valid or not
                    //TODO conflitto
                    vector<pair<int,int>> w = mostRecent(x->getSEQcv());
                    vector<pair<int,int>> w1 = mostRecent(SEQv);

                    vector<vector<pair<int,int>>> q = x->getSEQcv();
                    for(vector<pair<int,int>> v : q){
                        SEQv.push_back(v);
                    }

                    for (int destId = 0; destId < nodesNbr; destId++) {
                       if(destId==selfId) continue;
                       if(contain(destId,x->getView())==false) continue;
                       Msg * bp = new Msg();
                       bp->setId(selfId);
                       bp->setMsg("Propose");
                       bp->setSendTime(simTime());
                       bp->setView(current_view);
                       bp->setType(PROPOSE);
                       bp->setSEQcv(SEQv);
                       auto briefPacket = new Packet();
                       Msg &tmp = *bp;
                       const auto& payload = makeShared<Msg>(tmp);
                       briefPacket->insertAtBack(payload);
                       EV << "\n\n\n\n [Client"<< selfId <<"]PROPOSE messaggio inviato a: " << simTime() << "\n\n\n";
                       sendP(briefPacket, destId);
                    }
                 }

            }else if(x->getType()==CONVERGED){

            }else if(x->getType()==ACK){

            }else if(x->getType()==PREPARE){

            }else if(x->getType()==COMMIT){

            }else if(x->getType()==INSTALL){

            }else if(x->getType()==DELIVER){

            }
        }
    }
    delete msg;
}

void Node::socketPeerClosed(TcpSocket *socket_)
{
    ASSERT(socket_ == &socketL);
    // close the connection (if not already closed)
    if (socketL.getState() == TcpSocket::PEER_CLOSED) {
        EV_INFO << "remote TCP closed, closing here as well\n";
        close();
    }
}

vector<pair<int,int>> Node::mostRecent(vector<vector<pair<int,int>>> seq)
{
    int n=0;
    int j=0;
    int k=0;
    for(vector<pair<int,int>> i : seq){
        if(sizeof(i)>n){
            n=sizeof(i);
            j=k;
        }
        k++;
    }
    return seq[j];
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
    EV_INFO << "Time finish: " << simTime() << "\n";
    EV_INFO << "Messaggi ricevuti: \n";
    for (int i = 0; i < receivedMsg.size(); i++) {
       EV << i << " : " << "Messaggio ricevuto da Client"<< receivedMsg.at(i)->getId() << " inviato a " << receivedMsg.at(i)->getSendTime()<<" e arrivato a " << receivedMsg.at(i)->getArrivalTime()<<"\n";
    }
    EV_INFO << modulePath << ": opened " << numSessions << " sessions\n";
    EV_INFO << modulePath << ": sent " << bytesSent << " bytes in " << packetsSent << " packets\n";
    EV_INFO << modulePath << ": received " << bytesRcvd << " bytes in " << packetsRcvd << " packets\n";
}

void Node::handleStopOperation(LifecycleOperation *operation) {
    socketL.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void Node::handleCrashOperation(LifecycleOperation *operation) {
    socketL.destroy();    //TODO  in real operating systems, program crash detected by OS and OS closes sockets of crashed programs.
}


bool Node::contain(int x, vector<pair<int,int>> cv) {
    for(auto i : cv){
        if(i.first==x){
            return true;
        }
    }
    return false;
}

bool Node::checkPropose(int x, vector<pair<int,int>> cv) {
    for(auto i : cv){
        if(i.first==x){
            if(i.second==1){
                return true;
            }
        }
    }
    return false;
}


bool Node::isContainedIn(vector<vector<pair<int,int>>> s1, vector<vector<pair<int,int>>> s2 ) {
    for(vector<pair<int,int>> i : s1){
        for(vector<pair<int,int>> j : s2){
            if(i==j) continue;
            else return false;
        }
    }
    return true;
}

void Node::update(vector<vector<pair<int,int>>> s1) {
    int count=0;
    for(auto it = propose.begin(); it!=propose.end(); ++it) {
        if(equalSeq(it->first,s1)){
            (it->second)++;
            count=1;
            return;
        }
    }
    if(count==0){
        propose.push_back(std::make_pair(s1,1));
    }
    return;
}

bool Node::equalVec(vector<pair<int,int>>s1, vector<pair<int,int>> s2){
    int count=0;
    if(s1.size()!=s2.size()) return false;
    for(pair<int,int> v1 : s1){
        for(pair<int,int> v2 : s2){
            if(v1==v2){
                count+=1;
                break;
            }
        }
    }
    if(count==s1.size()) return true;
    else return false;
}

bool Node::equalSeq(vector<vector<pair<int,int>>>s1, vector<vector<pair<int,int>>> s2){
    //TODO stampare SEQ
    int count=0;
    if(s1.size()!=s2.size()) return false;
    for(vector<pair<int,int>> v1 : s1){
        for(vector<pair<int,int>> v2 : s2){
            if(equalVec(v1,v2)==true){
                count+=1;
                break;
            }
        }
    }
    if(count==s1.size()) return true;
    else return false;
}

bool Node::checkPropose() {
    for (auto it = propose.begin(); it!=propose.end(); ++it) {
        if(it->second>=2){ //TODO quorum
            return true;
        }
    }
    return false;
}

vector<vector<pair<int,int>>> Node::returnPropose() {
    for (auto it = propose.begin(); it!=propose.end(); ++it) {
        if(it->second>=2){
            return it->first; //TODO quorum
        }
    }
}

void Node::handleStartOperation(LifecycleOperation *operation) {
    EV << "\n\n\n HandleStartTime: " << startTime << "\n\n\n";

    numMsgToSend=rand()%10+1;

    nodesNbr=5;
    for (int nodeId = 0; nodeId < nodesNbr; nodeId++) {
       socketV[nodeId]=nullptr;
    }

    if (this->addrToId.size() == 0) {
        for (int nodeId = 0; nodeId < nodesNbr; nodeId++) {
            const char * address = ("client"+to_string(nodeId)).c_str();
            L3Address addr = L3AddressResolver().resolve(address);
            this->addrToId[addr] = nodeId;
        }
    }

    L3Address selfAddr = L3AddressResolver().addressOf(getParentModule(),L3AddressResolver().ADDR_IPv4);
    selfId = addrToId[selfAddr];

    current_view = {{0,1}, {1,1}, {2,1}};
    req_join_or_leave = {0, 0, 0, 0, 0};
    RECV={};
    SEQv={};
    propose={};
    FORMATv={};
    LCSEQv={};
    req_rec={};

    int m=par("join");
    if(m!=-1){
        rec_cond=false;
        join_complete=false;
        join(m);
    }

}

void Node::join(int x){
    timerEvent = new cMessage();
    timerEvent->setKind(MSGKIND_JOIN);
    scheduleAt(simTime()+x,timerEvent);
}

vector<pair<int,int>> Node::merge(vector<pair<int,int>> v1, vector<pair<int,int>> v2){
    vector<pair<int,int>> seq2={};
    for(pair<int,int> i:v1){
        seq2.push_back(i);
    }
    for(pair<int,int> i:v2){
        int c=0;
        for(pair<int,int> j:v1){
            if(i==j){
                c++;
                break;
            }
        }
        if(c==0){
            seq2.push_back(i);
        }
    }
    return seq2;
}

void Node::uponRECV(){
   if(RECV.empty()==false){ //TODO and installed view

       if(SEQv.empty()==true){

           vector<pair<int,int>> seq = merge(current_view,RECV);
           SEQv.push_back(seq);

           for (int destId = 0; destId < nodesNbr; destId++) {
               if(destId==selfId) continue;
               if(contain(destId,current_view)==false) continue;
               Msg * bp = new Msg();
               bp->setId(selfId);
               bp->setMsg("Propose");
               bp->setSendTime(simTime());
               bp->setView(current_view);
               bp->setType(PROPOSE);
               bp->setSEQcv(SEQv);
               auto briefPacket = new Packet();
               Msg &tmp = *bp;
               const auto& payload = makeShared<Msg>(tmp);
               briefPacket->insertAtBack(payload);
               EV << "\n\n\n\n [Client"<< selfId <<"]PROPOSE messaggio inviato a: " << simTime() << "\n\n\n";
               sendP(briefPacket, destId);
           }
       }
   }
}



} // namespace inet
