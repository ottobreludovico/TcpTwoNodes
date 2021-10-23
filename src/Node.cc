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
        switch (msg->getKind()) {
            case MSGKIND_JOIN:
                //mando msg
                if(join_complete==false && rec_cond==false){
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
                        bp->setTypeS("RECONFIG");
                        auto briefPacket = new Packet();
                        Msg &tmp = *bp;
                        const auto& payload = makeShared<Msg>(tmp);
                        briefPacket->insertAtBack(payload);
                        EV << "\n\n\n\n [Client"<< selfId <<"]RECONFIG inviato a: " << simTime() << "\n\n\n";
                        sendP(briefPacket, destId);
                    }
                }

                //schedulo il reinvio
                if(join_complete==false && rec_cond==false){ //TODO o quorum
                    join(3);
                }
                break;

            case MSGKIND_LEAVE:
                break;

            case MSGKIND_BROADCAST:
                if(isInstalled(current_view)==true){
                    for (int destId = 0; destId < nodesNbr; destId++) {
                        if(destId==selfId) continue;
                        if(contain(destId,current_view)==false) continue;
                        Msg * bp = new Msg();
                        bp->setId(selfId);
                        bp->setMsg("Message");
                        bp->setSendTime(simTime());
                        bp->setView(current_view);
                        bp->setType(PREPARE);
                        bp->setTypeS("PREPARE");
                        bp->setMsgId(msgId);
                        if(first_time==true){
                            msg_to_send.push_back(bp);
                            first_time=false;
                        }
                        auto briefPacket = new Packet();
                        Msg &tmp = *bp;
                        const auto& payload = makeShared<Msg>(tmp);
                        briefPacket->insertAtBack(payload);
                        EV << "\n\n\n\n [Client"<< selfId <<"]PREPARE inviato a: " << simTime() << "\n\n\n";
                        sendP(briefPacket, destId);
                    }
                    msgId++;
                }
                break;
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
            if(x->getType()==RECONFIG){
                x->setArrivalTime(simTime());
                receivedMsg.push_back(x);
                EV << "\n\n\n\n [Client"<< selfId <<"]RECONFIG ricevuto a: " << simTime() << "\n\n\n";
                if(x->getView()==current_view){
                    bool condition = false;
                    if(x->getJoin_or_leave()==1 || (x->getJoin_or_leave()==0 && checkPropose(x->getId(),current_view))){

                        if(contain(x->getId(),RECV)==false){
                            RECV.push_back(std::make_pair(x->getId(),x->getJoin_or_leave()));
                            condition=true;
                        }

                        Msg * bp = new Msg();
                        bp->setId(selfId);
                        bp->setMsg("Join reply");
                        bp->setSendTime(simTime());
                        bp->setView(current_view);
                        bp->setType(REC_CONFIRM);
                        bp->setTypeS("REC_CONFIRM");
                        auto briefPacket = new Packet();
                        Msg &tmp = *bp;
                        const auto& payload = makeShared<Msg>(tmp);
                        briefPacket->insertAtBack(payload);
                        EV << "\n\n\n\n [Client"<< selfId <<"]REC-CONFIRM inviato a: " << simTime() << "\n\n\n";
                        sendP(briefPacket, x->getId());
                    }
                    if(condition==true){
                        uponRECV();
                    }
                }
            }else if(x->getType()==REC_CONFIRM){
                //TODO fare array e salvarli solo una volta
                x->setArrivalTime(simTime());
                receivedMsg.push_back(x);
                EV << "\n\n\n\n [Client"<< selfId <<"]REC-CONFIRM ricevuto a: " << simTime() << "\n\n\n";
                if(req_join_or_leave[x->getId()]==0){
                    req_join_or_leave[x->getId()]=1;
                    count_req_join_or_leave++;
                    if(count_req_join_or_leave>=quorumSize){
                        rec_cond=true; //non devo più inviare richieste
                    }
                }
            }else if(x->getType()==PROPOSE){ //todo WARNING: Cannot send, not enough data for a full segment -> Nagle alg
                //TODO controllare se ho gia inviato lo stesso messaggio
                if(isReceivedP(x->getSEQcv(),x->getId())==false){
                    x->setArrivalTime(simTime());
                    receivedMsg.push_back(x);
                    EV << "\n\n\n\n [Client"<< selfId <<"]PROPOSE ricevuto a: " << simTime() << "\n\n\n";
                    updatePropose(x->getSEQcv(),x->getId());
                    if(checkPropose()==true){
                        LCSEQv = returnPropose();
                        for (int destId = 0; destId < nodesNbr; destId++) {
                          if(destId==selfId) continue;
                          if(contain(destId,x->getView())==false) continue;
                          Msg * bp = new Msg();
                          bp->setId(selfId);
                          bp->setMsg("Converged");
                          bp->setSendTime(simTime());
                          bp->setView(x->getView());
                          bp->setType(CONVERGED);
                          bp->setTypeS("CONVERGED");
                          bp->setSEQcv(x->getSEQcv());
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
                        //TODO conflitto IMPORTANTE
                        if(conflictS(x->getSEQcv(),SEQv)){
                            vector<pair<int,int>> w = mostRecent(x->getSEQcv());
                            vector<pair<int,int>> w1 = mostRecent(SEQv);

                            vector<vector<pair<int,int>>> ww={};
                            ww.push_back(w);

                            vector<vector<pair<int,int>>> ww1={};
                            ww1.push_back(w1);
                            SEQv=LCSEQv;
                            SEQv=unionS(SEQv,ww);
                            SEQv=unionS(SEQv,ww1);
                        }
                        else{
                            vector<vector<pair<int,int>>> q = x->getSEQcv();
                            SEQv=unionS(SEQv,q);
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
                           bp->setTypeS("PROPOSE");
                           bp->setSEQcv(x->getSEQcv());
                           auto briefPacket = new Packet();
                           Msg &tmp = *bp;
                           const auto& payload = makeShared<Msg>(tmp);
                           briefPacket->insertAtBack(payload);
                           EV << "\n\n\n\n [Client"<< selfId <<"]PROPOSE messaggio inviato a: " << simTime() << "\n\n\n";
                           sendP(briefPacket, destId);
                        }
                     }
                }
            }else if(x->getType()==CONVERGED){
                //TODO controllare se ho gia inviato lo stesso messaggio
                if(isReceivedC(x->getSEQcv(),x->getId())==false){
                    x->setArrivalTime(simTime());
                    receivedMsg.push_back(x);
                    EV << "\n\n\n\n [Client"<< selfId <<"]CONVERGED ricevuto a: " << simTime() << "\n\n\n";
                    updateConverge(x->getSEQcv(),x->getId());
                    if(checkConverge()==true){
                        vector<vector<pair<int,int>>> k = returnPropose();
                        vector<pair<int,int>> w = leastRecent(k);
                        for (int destId = 0; destId < nodesNbr; destId++) {
                          if(destId==selfId) continue;
                          if(contain(destId,w)==true || contain(destId,current_view)==true){
                            Msg * bp = new Msg();
                            bp->setId(selfId);
                            bp->setMsg("Install");
                            bp->setSendTime(simTime());
                            bp->setInstallView(w);
                            bp->setType(INSTALL);
                            bp->setTypeS("INSTALL");
                            bp->setSEQcv(x->getSEQcv());
                            bp->setView(x->getView());
                            auto briefPacket = new Packet();
                            Msg &tmp = *bp;
                            const auto& payload = makeShared<Msg>(tmp);
                            briefPacket->insertAtBack(payload);
                            EV << "\n\n\n\n [Client"<< selfId <<"]INSTALL inviato a: " << simTime() << "\n\n\n";
                            sendP(briefPacket, destId);
                          }
                        }
                    }
                }
            }else if(x->getType()==ACK){
                x->setArrivalTime(simTime());
                receivedMsg.push_back(x);

                if(acksMsg(x, x->getId(), x->getView())==true){ //TODO verify signature
                  //  EV << "DEBUG1";
                    addAcksMsg(x, x->getId(), x->getView()); //TODO wait for return!
                    //TODO add sigma

                }

                EV << "ludo";
                for(auto it:acks){
                    for(auto it2:it.second){
                        for(auto it3:it2.second){
                            EV<<it3;
                        }
                    }
                }
                if(checkMsg()==true){
                    Msg * m = returnMsg();
                    if(quorumMsg(m)==false){
                        v_cer = m->getView();
                          if(isInstalled(current_view)==true){
                              //TODO add cer
                              for (int destId = 0; destId < nodesNbr; destId++) {
                                if(destId==selfId) continue;
                                if(contain(destId,current_view)==true){
                                  Msg * bp = new Msg();
                                  bp->setId(selfId);
                                  bp->setMsg(m->getMsg());
                                  bp->setMsgId(m->getMsgId());
                                  bp->setSendTime(simTime());
                                  bp->setType(COMMIT);
                                  bp->setTypeS("COMMIT");
                                  bp->setView(current_view);
                                  auto briefPacket = new Packet();
                                  Msg &tmp = *bp;
                                  const auto& payload = makeShared<Msg>(tmp);
                                  briefPacket->insertAtBack(payload);
                                  EV << "\n\n\n\n [Client"<< selfId <<"]COMMIT inviato a: " << simTime() << "\n\n\n";
                                  sendP(briefPacket, destId);
                                }
                              }
                          }
                    }
                }
            }else if(x->getType()==PREPARE){
                x->setArrivalTime(simTime());
                receivedMsg.push_back(x);
                msg_to_send.push_back(x);
                if(allowed_ack.empty() || isAllowed(x)==true ){
                    if(isAllowed(x)==false){
                        allowed_ack.push_back(x);
                    }
                    //TODO update_if_bot
                    //TODO sign
                    Msg * bp = new Msg();
                    bp->setId(selfId);
                    bp->setMsg("ack");
                    bp->setSendTime(simTime());
                    bp->setType(ACK);
                    bp->setTypeS("ACK");
                    bp->setMsgId(x->getMsgId());
                    bp->setView(current_view);
                    auto briefPacket = new Packet();
                    Msg &tmp = *bp;
                    const auto& payload = makeShared<Msg>(tmp);
                    briefPacket->insertAtBack(payload);
                    EV << "\n\n\n\n [Client"<< selfId <<"]ACK inviato a: " << simTime() << "\n\n\n";
                    sendP(briefPacket, x->getId());
                }
            }else if(x->getType()==COMMIT){
                x->setArrivalTime(simTime());
                receivedMsg.push_back(x);
                if(equalVec(x->getView(),current_view)){
                    //TODO verify CER
                    if(stored==false){
                        stored=true;
                        stored_value=x;
                        //TODO update if bot
                        for (int destId = 0; destId < nodesNbr; destId++) {
                           if(destId==selfId) continue;
                           if(contain(destId,current_view)==false) continue;
                           Msg * bp = new Msg();
                           bp->setId(selfId);
                           bp->setMsg(x->getMsg());
                           bp->setSendTime(simTime());
                           bp->setView(current_view);
                           bp->setType(COMMIT);
                           bp->setTypeS("COMMIT");
                           bp->setVcer(v_cer);
                           bp->setMsgId(x->getMsgId());
                           auto briefPacket = new Packet();
                           Msg &tmp = *bp;
                           const auto& payload = makeShared<Msg>(tmp);
                           briefPacket->insertAtBack(payload);
                           EV << "\n\n\n\n [Client"<< selfId <<"]COMMIT messaggio inviato a: " << simTime() << "\n\n\n";
                           sendP(briefPacket, destId);
                        }
                    }
                    Msg * bp = new Msg();
                    bp->setId(selfId);
                    bp->setMsg(x->getMsg());
                    bp->setSendTime(simTime());
                    bp->setView(current_view);
                    bp->setType(DELIVER);
                    bp->setTypeS("DELIVER");
                    bp->setMsgId(x->getMsgId());
                    auto briefPacket = new Packet();
                    Msg &tmp = *bp;
                    const auto& payload = makeShared<Msg>(tmp);
                    briefPacket->insertAtBack(payload);
                    EV << "\n\n\n\n [Client"<< selfId <<"]DELIVER messaggio inviato a: " << simTime() << "\n\n\n";
                    sendP(briefPacket, x->getId());
                }
            }else if(x->getType()==INSTALL){
                x->setArrivalTime(simTime());
                receivedMsg.push_back(x);
                EV << "\n\n\n\n\n";
                EV << "Client" << selfId << "\n";
                for(auto it : x->getInstallView()){
                    EV << it.first << "\n";
                }
                if(isReceivedI(x->getView())==false){
                    //TODO FORMAT
                    if(isMember(selfId,x->getView())){
                        if(contains(x->getInstallView(),current_view)==true){
                            stop_processing=true;
                            //TODO mandare state update
                        }
                    }if(contains(x->getInstallView(),current_view)==true){
                        //TODO state update
                        if(isMember(selfId,x->getInstallView())){

                            current_view=x->getInstallView();
                            if(isMember(selfId,x->getView())){
                                join_complete=true;
                            }
                            vector<vector<pair<int,int>>> seq=x->getSEQcv();
                            vector<vector<pair<int,int>>> seq_={};
                            int c=0;
                            for(auto it : seq){
                                if(contains(it,current_view)==true){
                                    seq_.push_back(it);
                                }
                            }
                            if(seq_.empty()==false){
                                //TODO cambio SEQ ?
                                SEQv = seq_;
                                for (int destId = 0; destId < nodesNbr; destId++) {
                                   if(destId==selfId) continue;
                                   if(contain(destId,current_view)==false) continue;
                                   Msg * bp = new Msg();
                                   bp->setId(selfId);
                                   bp->setMsg("Propose");
                                   bp->setSendTime(simTime());
                                   bp->setView(current_view);
                                   bp->setType(PROPOSE);
                                   bp->setTypeS("PROPOSE");
                                   bp->setSEQcv(SEQv);
                                   auto briefPacket = new Packet();
                                   Msg &tmp = *bp;
                                   const auto& payload = makeShared<Msg>(tmp);
                                   briefPacket->insertAtBack(payload);
                                   EV << "\n\n\n\n [Client"<< selfId <<"]PROPOSE messaggio inviato a: " << simTime() << "\n\n\n";
                                   sendP(briefPacket, destId);
                                }
                            }else{
                                installedView.push_back(current_view);
                                stop_processing = false;
                                //TODO new_view()
                            }
                        }else{
                            //TODO LEAVE
                        }
                    }
                }
            }else if(x->getType()==DELIVER){
                x->setArrivalTime(simTime());
                receivedMsg.push_back(x);
                addDeliverMsg(x,x->getId(),x->getView());
                if(checkDeliverMsg()==true && first_time_deliver==false){
                    first_time_deliver=true;
                    if(msg_to_send.empty()==false){
                        Msg * m = returnDeliverMsg();
                        delivered=true;
                        for(auto it:msg_to_send){
                            EV<< it->getMsgId() << " da Client" << it->getId();
                        }
                        EV << "\n\n\n\n [Client"<< selfId <<"]DELIVERED di " << m->getMsgId()  << "da parte di" << m->getId() << " FINISHED a: " << simTime() << "\n\n\n";
                        can_leave=true;
                    }
                }
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

vector<pair<int,int>> Node::leastRecent(vector<vector<pair<int,int>>> seq)
{
    int n=sizeof(seq[0]);
    int j=0;
    int k=0;
    for(vector<pair<int,int>> i : seq){
        if(sizeof(i)<n){
            n=sizeof(i);
            j=k;
        }
        else if(sizeof(i)==n){
            for(pair<int,int> p1 : i){
                for(pair<int,int> p2 : seq[j]){
                    if(p2.first==p1.first){
                        if(p2.second==p1.second) continue;
                        else if(p2.second==0){
                            j=k;
                        }
                    }
                }
            }
        }
        k++;
    }
    return seq[j];
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
        else if(sizeof(i)==n){
            for(pair<int,int> p1 : i){
                for(pair<int,int> p2 : seq[j]){
                    if(p2.first==p1.first){
                        if(p2.second==p1.second) continue;
                        else if(p1.second==0){
                            j=k;
                        }
                    }
                }
            }
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
       EV << i << " : " << receivedMsg.at(i)->getTypeS() <<" ricevuto da Client"<< receivedMsg.at(i)->getId() << " inviato a " << receivedMsg.at(i)->getSendTime()<<" e arrivato a " << receivedMsg.at(i)->getArrivalTime()<< "----> seq";
       for(auto it:receivedMsg.at(i)->getSEQcv()){
           EV<<"[";
           for(auto it2:it){
               EV<<it2.first<<",";
           }
           EV<<"]";
       }
       EV<<"\n";
    }
    EV_INFO << modulePath << ": opened " << numSessions << " sessions\n";
    EV_INFO << modulePath << ": sent " << bytesSent << " bytes in " << packetsSent << " packets\n";
    EV_INFO << modulePath << ": received " << bytesRcvd << " bytes in " << packetsRcvd << " packets\n";
    for(auto it : current_view){
        EV << it.first << " ";
    }
    EV << "\n";
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

bool Node::checkDeliverMsg() {
    if(msg_to_send.empty()==false){
        for(auto it = deliver.begin(); it!=deliver.end(); ++it){
            for(auto it2:it->second){
                int c=0;
                int i=0;
                for(auto it3:it2.second){
                    if(contain(i,it->first)==true){ //TODO and CER
                        c++;
                    }
                }
                if(c>=quorumSize){
                    return true;
                }
            }
         }
    }
    return false;
}

Msg * Node::returnDeliverMsg() {
    int j=0;
    if(msg_to_send.empty()==false){
        for(auto it = deliver.begin(); it!=deliver.end(); ++it){
            for(auto it2:it->second){
                int c=0;
                int i=0;
                for(auto it3:it2.second){
                    if(contain(i,it->first)==true){ //TODO and CER
                        c+=it3;
                    }
                    i++;
                }
                if(c>=quorumSize){
                    j=it2.first->getMsgId();
                    break;
                    //return it2.first;
                }
            }
         }
         for(auto it:msg_to_send){
             if(it->getMsgId()==j){
                 return it;
             }
         }
    }
}

bool Node::checkMsg() {
    if(msg_to_send.empty()==false){
        for(auto it = acks.begin(); it!=acks.end(); ++it){
            for(auto it2:it->second){
                int c=0;
                int i=0;
                for(auto it3:it2.second){
                    EV << "Client" << i << " -> " << it3 << "\n";
                    if(contain(i,it->first)==true){ //TODO and CER
                        c+=it3;
                    }
                    i++;
                }
                EV << "DEBUG " << c;
                if(c>=quorumSize){
                    return true;
                }
            }
         }
    }
    return false;
}

Msg * Node::returnMsg() {
    int j=0;
    if(msg_to_send.empty()==false){
        for(auto it = acks.begin(); it!=acks.end(); ++it){
            for(auto it2:it->second){
                int c=0;
                int i=0;
                for(auto it3:it2.second){
                    if(contain(i,it->first)==true){ //TODO and CER
                        c+=it3;
                    }
                    i++;
                }
                if(c>=quorumSize){
                    j=it2.first->getMsgId();
                    break;
                    //return it2.first;
                }
            }
         }
         for(auto it:msg_to_send){
             if(it->getMsgId()==j){
                 return it;
             }
         }
    }
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

bool Node::isAllowed(Msg * m ) {
    for(Msg * i : allowed_ack){
       if(equalMsg(i,m)){
           return true;
       }
    }
    return false;
}

bool Node::acksMsg(Msg* m, int id, vector<pair<int,int>> v){
    for(auto it = acks.begin(); it!=acks.end(); ++it){
        if(equalVec(it->first,v)){
            for(auto it2:it->second){
                if(equalMsg(it2.first,m)==true && it2.second[id]==1){
                    return false;
                }
            }
        }
    }
    return true;
}

void Node::addAcksMsg(Msg* m, int id, vector<pair<int,int>> v){
    int i=0;
    for(auto it = acks.begin(); it!=acks.end(); ++it){
        if(equalVec(it->first,v)){
            int j=0;
            for(auto it2:it->second){
                if(equalMsg(it2.first,m)==true){
                    //it2.second[id]=1;
                    acks[i].second[j].second[id]=1;
                    EV << "\n";
                    EV << "DEB1 \n";
                    for(auto i:it2.second){
                        EV << i;
                    }
                    EV << "\n";
                    return;
                }
                j++;
            }
        }
        i++;
    }
    vector<int> vec(nodesNbr,0);
    vec[id]=1;
    vector<pair<Msg*,vector<int>>> vec2 = {};
    vec2.push_back(std::make_pair(m,vec));
    acks.push_back(std::make_pair(v,vec2));
    EV << "\n";
    EV << "DEB2 \n";
    for(auto i:vec){
        EV << i;
    }
    EV << "\n";
    return;
}

void Node::addDeliverMsg(Msg* m, int id, vector<pair<int,int>> v){
    int i=0;
    for(auto it = deliver.begin(); it!=deliver.end(); ++it){
        if(equalVec(it->first,v)){
            int j=0;
            for(auto it2:it->second){
                if(equalMsg(it2.first,m)==true){
                    //it2.second[id]=1;
                    deliver[i].second[j].second[id]=1;
                    return;
                }
                j++;
            }
        }
        i++;
    }
    vector<int> vec(nodesNbr,0);
    vec[id]=1;
    vector<pair<Msg*,vector<int>>> vec2 = {};
    vec2.push_back(std::make_pair(m,vec));
    deliver.push_back(std::make_pair(v,vec2));
    return;
}

void Node::updatePropose(vector<vector<pair<int,int>>> s1,int id) { //TODO verificare chi invia propose
    int count=0;
    for(auto it = propose.begin(); it!=propose.end(); ++it) {
        if(equalSeq(it->first,s1)){
            if(it->second[id]==0){
                it->second[id]=1;
                count=1;
                return;
            }else{
                return;
            }
        }
    }
    if(count==0){
        vector<int> v(nodesNbr,0);
        v[id]=1;
        propose.push_back(std::make_pair(s1,v));
    }
    return;
}

void Node::updateConverge(vector<vector<pair<int,int>>> s1, int id) {
    int count=0;
    for(auto it = converge.begin(); it!=converge.end(); ++it) {
        if(equalSeq(it->first,s1)){
            if(it->second[id]==0){
                it->second[id]=1;
                count=1;
                return;
            }else{
                return;
            }
        }
    }
    if(count==0){
        vector<int> v(nodesNbr,0);
        v[id]=1;
        converge.push_back(std::make_pair(s1,v));
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
    int c=0;
    for (auto it = propose.begin(); it!=propose.end(); ++it) {
        for(int i:it->second){
            c+=i;
        }
    }
    if(c>=quorumSize){
        return true;
    }
    return false;
}

bool Node::isReceivedC(vector<vector<pair<int,int>>> v, int id) {
    for(auto it = converge.begin(); it!=converge.end(); ++it) {
        if(equalSeq(it->first,v)){
            if(it->second[id]==0){
                it->second[id]=1;
                return false;
            }else{
                return true;
            }
        }
    }
}

bool Node::isReceivedP(vector<vector<pair<int,int>>> v,int id) {
    for(auto it = propose.begin(); it!=propose.end(); ++it) {
        if(equalSeq(it->first,v)){
            if(it->second[id]==0){
                it->second[id]=1;
                return false;
            }else{
                return true;
            }
        }
    }
}

bool Node::isReceivedI(vector<pair<int,int>> v) {
    for(auto it : installReceived) {
        if(equalVec(it,v)){
            return true;
        }
    }
    installReceived.push_back(v);
    return false;
}


vector<vector<pair<int,int>>> Node::returnPropose() {
    int c=0;
    for (auto it = propose.begin(); it!=propose.end(); ++it) {
        for(int i:it->second){
           c+=i;
        }
        if(c>=quorumSize){
            return it->first;
        }
    }
}

bool Node::checkConverge() {
    int c=0;
    for (auto it = converge.begin(); it!=converge.end(); ++it) {
        for(int i:it->second){
            c+=i;
        }
    }
    if(c>=quorumSize){
        return true;
    }
    return false;
}

vector<vector<pair<int,int>>> Node::returnConverge() {
    int c=0;
    for (auto it = converge.begin(); it!=converge.end(); ++it) {
        for(int i:it->second){
           c+=i;
        }
        if(c>=quorumSize){
            return it->first;
        }
    }
}

void Node::join(int x){
    timerEvent = new cMessage();
    timerEvent->setKind(MSGKIND_JOIN);
    scheduleAt(simTime()+x,timerEvent);
}

void Node::broadcast(int x){
    timerEvent = new cMessage();
    timerEvent->setKind(MSGKIND_BROADCAST);
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

int Node::sizeCV(vector<pair<int,int>> v1){
    int c=0;
    for(pair<int,int> i:v1){
        c++;
    }
    return c;
}

bool Node::isInstalled(vector<pair<int,int>> v1){
    for (auto it : installedView) {
        if(equalVec(it,v1)){
            return true;
        }
    }
    return false;
}

bool Node::conflictS(vector<vector<pair<int,int>>> s1, vector<vector<pair<int,int>>> s2){
    for(vector<pair<int,int>> i:s1){
        for(vector<pair<int,int>> j:s2){
            if(conflictV(i,j)==false){
               continue;
            }else{
               return true;
            }
        }
    }
    return false;
}

vector<vector<pair<int,int>>> Node::unionS(vector<vector<pair<int,int>>> s1, vector<vector<pair<int,int>>> s2){
    vector<vector<pair<int,int>>> s=s1;
    for(vector<pair<int,int>> i:s1){
        int c=0;
        for(vector<pair<int,int>> j:s){
            if(equalVec(i,j)){
                c++;
            }
        }
        if(c==0){
            s.push_back(i);
        }
    }
    return s;
}

bool Node::contains(vector<pair<int,int>> v1, vector<pair<int,int>> v2){
    if(sizeCV(v1)<=sizeCV(v2)){
       return false;
    }
    int count=0;
    for(pair<int,int> i:v2){
        for(pair<int,int> j:v1){
            if(i.first==j.first){
                if(i.second==j.second){
                    break;
                }
                else{
                    count++;
                }
            }
        }
    }
    if(count>0){
        return false;
    }else{
        return true;
    }
}

bool Node::conflictV(vector<pair<int,int>> v1, vector<pair<int,int>> v2){
    int count=0;
    int count2=0;
    for(pair<int,int> i:v1){
        for(pair<int,int> j:v2){
            if(i.first==j.first){
                if(i.second==j.second){
                    break;
                }
                else{
                    count++;
                }
            }
        }
    }
    for(pair<int,int> i:v2){
            for(pair<int,int> j:v1){
                if(i.first==j.first){
                    if(i.second==j.second){
                        break;
                    }
                    else{
                        count++;
                    }
                }
            }
        }
    if(count*count2>0){
        return true;
    }else{
        return false;
    }
}

bool Node::quorumMsg(Msg *m){
    for(auto it:quorum_msg){
        if(equalMsg(it,m)){
            return true;
        }
    }
    quorum_msg.push_back(m);
    return false;
}

bool Node::isMember(int id, vector<pair<int,int>> v){
    for(pair<int,int> i:v){
        if(i.first==id) return true;
    }
    return false;
}

bool Node::equalMsg(Msg* m1, Msg* m2){
    if(m1->getMsgId()!=m2->getMsgId() && m1->getId()==m2->getId()) return false;
    else return true;
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
               bp->setTypeS("PROPOSE");
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

void Node::handleStartOperation(LifecycleOperation *operation) {
    EV << "\n\n\n HandleStartTime: " << startTime << "\n\n\n";

    //numMsgToSend=rand()%10+1;

    nodesNbr = par("nPar");
    f = par("fPar");
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

    current_view = {{0,1}, {1,1}, {2,1}}; //fixed current view

    installedView={current_view};

    int tmp = sizeCV(current_view) + f + 1;
    if (tmp % 2 == 1){
        tmp++;
    }
    quorumSize = tmp/2;
    EV << "sc: " << sizeCV(current_view);
    EV << "quorum: " << quorumSize;

    req_join_or_leave = {0,0,0,0,0};


    RECV={};
    SEQv={};
    propose={};
    converge={};
    proposeReceived={};
    convergeReceived={};
    installReceived={};
    FORMATv={};
    LCSEQv={};
    allowed_ack={};
    acks={};
    msg_to_send={}; //TODO IMPORTANT -> Error RNG gen 14
    msgId=0;
    deliver={};
    quorum_msg={};

    int m=par("join");
    if(m!=-1){
        rec_cond=false;
        join_complete=false;
        join(m);
    }

    int m2=par("broadcast");
    if(m2!=-1){
        broadcast(m2);
    }

}

} // namespace inet
