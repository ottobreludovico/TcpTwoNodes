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
#include "messageL_m.h"
//#include "StateUpdateMessage_m.h"
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


/*
 * The initialize() and finish() methods are declared as part of cComponent,
 * and provide the user the opportunity of running code at the beginning and
 * at successful termination of the simulation.The reason initialize() exists
 * is that usually you cannot put simulation-related code into the simple
 * module constructor, because the simulation model is still being setup when
 * the constructor runs, and many required objects are not yet available.
 *
 * In contrast, initialize() is called just before the simulation starts executing,
 * when everything else has been set up already.
 *
 *
 * Durante l'inizializzazione dello strato di applicazione apro un socket TCP nella
 * porta localPortL in cui mi metto in ascolto.
 */


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
        socketL.setOutputGate(gate("socketOut"));
        socketL.bind(localAddress[0] ? L3AddressResolver().resolve(localAddress) : L3Address(), localPortL);
        socketL.setCallback(this);
        socketL.listen();
    }
}

void Node::handleTimer(cMessage *msg) {
    EV << msg;
}


/*
 * The idea is that at each event (message arrival) we simply call a user-defined function.
 * This function, handleMessage(cMessage *msg) is a virtual member function of cSimpleModule
 * which does nothing by default -- the user has to redefine it in subclasses and add the
 * message processing code.The handleMessage() function will be called for every message
 * that arrives at the module. The function should process the message and return
 * immediately after that. The simulation time is potentially different in each call.
 * No simulation time elapses within a call to handleMessage().
 *
 *
 *
 *
 */

void Node::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()){
        switch (msg->getKind()) {

            case MSGKIND_TIMER:{
                if(sizeCV(RECV)!=0 && isInstalled(current_view)==true){
                    uponRECV();
                }
                timer(timerDelay);
                break;}

            case MSGKIND_JOIN:{
                //mando msg
                view_discovery();
                EV<<"join"<<selfId;
                for(auto it:current_view){
                    EV<< it.first << " ";
                }
                if(join_complete==false && rec_cond==false){
                    for (int destId = 0; destId < nodesNbr; destId++) {
                        if(destId==selfId) continue;
                        if(contain(destId,init_view)==false) continue;
                        Msg * bp = new Msg();
                        bp->setId(selfId);
                        bp->setMsg("Join");
                        simtime_t t=simTime()+fRand(0.0, 1.0);
                        bp->setSendTime(t);
                        bp->setJoin_or_leave(1);
                        bp->setView(current_view);
                        bp->setType(RECONFIG);
                        bp->setTypeS("RECONFIG");
                        auto briefPacket = new Packet();
                        Msg &tmp = *bp;
                        const auto& payload = makeShared<Msg>(tmp);
                        briefPacket->insertAtBack(payload);
                        //EV << "\n\n\n\n [Client"<< selfId <<"]RECONFIG inviato a: " << simTime() << "\n\n\n";
                        //sendP(briefPacket, destId);
                        MessageL * timer = new MessageL();
                        timer->setDestId(destId);
                        timer->setKind(MSGKIND_SEND);
                        timer->setM(bp);
                        //scheduleAt(simTime(),timer);
                        scheduleAt(t,timer);
                    }
                }

                //schedulo il reinvio
                if(join_complete==false){ //TODO o quorum
                    join(3);
                }
                break;}

            case MSGKIND_LEAVE:{
                break;}

            case MSGKIND_BROADCAST:{
                EV << "InstalledView";
                for(auto it:installedView){
                    for(auto it2 : it.first){
                        EV << it2.first << " ";
                    }
                    EV << "---->" << it.second << "\n";
                }
                if(isInstalled(current_view)==true){
                    for (int destId = 0; destId < nodesNbr; destId++) {
                        if(destId==selfId) continue;
                        if(contain(destId,current_view)==false) continue;
                        Msg * bp = new Msg();
                        bp->setId(selfId);
                        bp->setMsg("Message");
                        simtime_t t=simTime()+fRand(0.0, 1.0);
                        bp->setSendTime(t);
                        bp->setView(current_view);
                        bp->setType(PREPARE);
                        bp->setTypeS("PREPARE");
                        bp->setSender(selfId);
                        bp->setMsgId(msgId);
                        if(first_time==true){
                            msg_prepare.push_back(bp);
                            first_time=false;
                        }
                        auto briefPacket = new Packet();
                        Msg &tmp = *bp;
                        const auto& payload = makeShared<Msg>(tmp);
                        briefPacket->insertAtBack(payload);
                        //EV << "\n\n\n\n [Client"<< selfId <<"]PREPARE inviato a: " << simTime() << "\n\n\n";
                        //sendP(briefPacket, destId);
                        MessageL * timer = new MessageL();
                        timer->setDestId(destId);
                        timer->setKind(MSGKIND_SEND);
                        timer->setM(bp);
                        //scheduleAt(simTime(),timer);
                        scheduleAt(t,timer);
                    }
                    msgId++;
                    first_time=true;
                }else{
                    broadcast(3);
                }
                break;}

         case MSGKIND_SEND:{
             auto briefPacket = new Packet();
             Msg &tmp = *((MessageL*)msg)->getM();
             tmp.setDestId(((MessageL*)msg)->getDestId());
             inviati.push_back(&tmp);
             const auto& payload = makeShared<Msg>(tmp);
             briefPacket->insertAtBack(payload);
             EV << "\n\n\n\n [Client"<< selfId <<"]"<< tmp.getTypeS() <<" inviato a: " << simTime() << "\n\n\n";
             sendP(briefPacket, ((MessageL*)msg)->getDestId());
             break;}

         default:{
             EV<<"default";
             break;
         }

        }
   }else{
       TcpSocket *socket = check_and_cast_nullable<TcpSocket*>(socketMap.findSocketFor(msg));
       if (socket)
           socket->processMessage(msg);
       else if (socketL.belongsToSocket(msg))
           socketL.processMessage(msg);
       else {
           // throw cRuntimeError("Unknown incoming message: '%s'", msg->getName());
           EV_ERROR << "message " << msg->getFullName() << "(" << msg->getClassName() << ") arrived for unknown socket \n";
           delete msg;
       }
    }
}

void Node::sendP(Packet * pk, int destId){
    EV << "\n\n\n Client" << selfId << "\n";
    for(int i=0;i<nodesNbr;i++){
        EV << "Client" << i << " : "  << socketV[i].first << "\n";
    }
    EV << "\n\n\n";
    if(socketV[destId].first==0){
        localPort=par("localPort");
        TcpSocket * socket=new TcpSocket();
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
           socketMap.addSocket(socket);
           socketV[destId].first=1;
           socketV[destId].second=socket;
           EV << "\n\n\n Client" << selfId << "\n";
           /*for(int i=0;i<nodesNbr;i++){
               EV << "Client" << i << " : "  << socketV[i].second << "\n";
           }*/
           EV << "\n\n\n";
           numSessions++;
           emit(connectSignal, 1L);

           socket->send(pk);

           bytesSent += pk->getByteLength();
           packetsSent++;

        }
    }else{
        socketV[destId].second->send(pk);
    }

}

void Node::connect()
{
    // we need a new connId if this is not the first connection

}

void Node::close()
{
    //EV_INFO << "issuing CLOSE command\n";
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

void Node::socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo)
{
    // new TCP connection -- create new socket object and server process
    TcpSocket *newSocket = new TcpSocket(availableInfo);
    newSocket->setOutputGate(gate("socketOut"));
    newSocket->setCallback(this);
    socketMap.addSocket(newSocket);
    socket->accept(availableInfo->getNewSocketId());
}

void Node::socketEstablished(TcpSocket * s)
{
    // *redefine* to perform or schedule first sending
    EV_INFO << "connected\n";
}


void Node::socketDataArrived(TcpSocket * s, Packet *msg, bool)
{
    // *redefine* to perform or schedule next sending
    packetsRcvd++;
    bytesRcvd += msg->getByteLength();
    emit(packetReceivedSignal, msg);
    if(msg->getByteLength()==2){
        StateUpdateMessage* x2 = (StateUpdateMessage *)msg->peekAtBack().get();
        if(x2->getType()==STATE_UPDATE){
            //x->setArrivalTime(simTime());
            states_update.push_back(x2);
            states_update_rec.push_back(x2);
            state_update_f(x2);
        }
        delete msg;
    }else{
        Msg* x = (Msg *)msg->peekAtBack().get();
        if(x->getId()!=selfId ){
            //sendBack(x->getId());
            if (std::count(receivedMsg.begin(), receivedMsg.end(), x)) {

            }
            else {
                if(x->getType()==RECONFIG && stop_processing==false){
                    //x->setArrivalTime(simTime());
                    receivedMsg.push_back(x);
                    reconfig_f(x);
                }else if(x->getType()==REC_CONFIRM){
                    //x->setArrivalTime(simTime());
                    receivedMsg.push_back(x);
                    rec_confirm_f(x);
                }else if(x->getType()==PROPOSE){
                    //x->setArrivalTime(simTime());
                    propose_f(x);
                }else if(x->getType()==CONVERGED){
                    //x->setArrivalTime(simTime());
                    converge_f(x);
                }else if(x->getType()==ACK){
                    //x->setArrivalTime(simTime());
                    //ack_f(x);
                }else if(x->getType()==PREPARE && stop_processing==false){
                    //x->setArrivalTime(simTime());
                    //prepare_f(x);
                }else if(x->getType()==COMMIT && stop_processing==false){
                    //x->setArrivalTime(simTime());
                    //commit_f(x);
                }else if(x->getType()==INSTALL){
                    //x->setArrivalTime(simTime());
                    receivedMsg.push_back(x);
                    install_f(x);
                }else if(x->getType()==DELIVER){
                    //x->setArrivalTime(simTime());
                    //deliver_f(x);
                }else if(x->getType()==CHANGE_VIEW){
                    //x->setArrivalTime(simTime());
                    receivedMsg.push_back(x);
                    if(contain(selfId,x->getView())==false){
                        rec_cond=false;
                    }
                    bool s=true;
                    for(auto it:init_view_){
                        if(equalVec(it,x->getView())==true){
                            s=false;
                            break;
                        }
                    }
                    if(s==true){
                        if(contains(init_view,x->getView())==false){
                            init_view=x->getView();
                            int tmp = sizeCV(init_view) + f + 1;
                            if (tmp % 2 == 1){
                                tmp++;
                            }
                            quorumSize = tmp/2;
                        }
                        init_view_.push_back(x->getView());
                    }
                }

            }
        }
        delete msg;
    }
}

void Node::socketPeerClosed(TcpSocket *socket_)
{
    /*ASSERT(socket_ == &socketL);
    // close the connection (if not already closed)
    if (socketL.getState() == TcpSocket::PEER_CLOSED) {
        EV_INFO << "remote TCP closed, closing here as well\n";
        close();
    }*/
}

vector<pair<int,int>> Node::leastRecent(vector<vector<pair<int,int>>> seq)
{
    if(sizeS(seq)==0){
        return {};
    }
    int n=sizeCV(seq[0]);
    int j=0;
    int k=0;
    for(vector<pair<int,int>> i : seq){
        if(sizeCV(i)>n){
            n=sizeCV(i);
            j=k;
        }
        else if(sizeCV(i)==n){
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

vector<pair<int,int>> Node::mostRecent(vector<vector<pair<int,int>>> seq)
{
    int n=0;
    int j=0;
    int k=0;
    for(vector<pair<int,int>> i : seq){
        if(sizeCV(i)<n){
            n=sizeCV(i);
            j=k;
        }
        else if(sizeCV(i)==n){
            for(pair<int,int> p1 : i){
                for(pair<int,int> p2 : seq[j]){
                    if(p2.first==p1.first){
                        if(p2.second==p1.second) continue;
                        else if(p1.second==1){
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
    EV << "-----------------------------CLIENT"<<selfId<<"------------------------------\n";
    if(p==false){
        EV_INFO << "Time finish: " << simTime() << "\n\n";
        EV_INFO << "Messaggi inviati: "<< inviati.size() <<"\n";
        for(auto view : init_view_){
            EV<<"\n[ ";
            for(auto elem: view){
                EV<< elem.first << " ,";
            }
            EV<<"]:\n ";

            for (int i = 0; i < inviati.size(); i++) {
                if(equalVec(view,inviati.at(i)->getView())==true){
                    if(strcmp(inviati.at(i)->getTypeS(),"PROPOSE")==0 || strcmp(inviati.at(i)->getTypeS(),"CONVERGED")==0 ||  strcmp(inviati.at(i)->getTypeS(),"INSTALL")==0){
                      EV << i << "("<<inviati.at(i)->getSender() << ","<< inviati.at(i)->getMsgId()<< ") "<< " : " << inviati.at(i)->getTypeS() <<" inviato a Client"<< inviati.at(i)->getDestId() << " inviato a " << inviati.at(i)->getSendTime()<< "----> seq";
                      for(auto it:inviati.at(i)->getSEQcv()){
                          EV<<"[";
                          for(auto it2:it){
                              EV<<it2.first<<",";
                          }
                          EV<<"]";
                      }
                      EV<<"\n";
                   }else if(strcmp(inviati.at(i)->getTypeS(),"NEW VIEW")==0){
                       EV_WARN << i << " : " << inviati.at(i)->getTypeS() << " a "<< inviati.at(i)->getSendTime() << " ----> seq";

                       EV_WARN<<"[";
                       for(auto it2:inviati.at(i)->getView()){
                           EV_WARN<<it2.first<<",";
                       }
                       EV_WARN<<"]";

                       EV_WARN<<"\n";
                   }else if(strcmp(inviati.at(i)->getTypeS(),"NEW MESSAGE DELIVERED")==0){
                       EV_WARN << i << "("<<inviati.at(i)->getSender() << ","<< inviati.at(i)->getMsgId()<< ") "<< " : " << inviati.at(i)->getTypeS() << " a " << inviati.at(i)->getSendTime() << " ----> seq";

                       EV_WARN<<"[";
                       for(auto it2:inviati.at(i)->getView()){
                           EV_WARN<<it2.first<<",";
                       }
                       EV_WARN<<"]";

                       EV_WARN<<"\n";
                   }else{
                       EV << i << "("<< inviati.at(i)->getSender() << ","<< inviati.at(i)->getMsgId()<< ") "<< " : " << inviati.at(i)->getTypeS() <<" inviato a Client"<< inviati.at(i)->getDestId() << " inviato a " << inviati.at(i)->getSendTime() << "\n";
                   }
                }
            }
        }
        EV << "\n";
        EV_INFO << "Messaggi ricevuti: "<< receivedMsg.size() <<"\n";
        for(auto view : init_view_){
            EV<<"\n[ ";
                for(auto elem: view){
                    EV<< elem.first << " ,";
                }
                EV<<"]:\n";

                for (int i = 0; i < receivedMsg.size(); i++) {
                    if(equalVec(view,receivedMsg.at(i)->getView())==true){
                       if(strcmp(receivedMsg.at(i)->getTypeS(),"PROPOSE")==0 || strcmp(receivedMsg.at(i)->getTypeS(),"CONVERGED")==0){
                           EV << i << "("<< receivedMsg.at(i)->getSender() << ","<< receivedMsg.at(i)->getMsgId()<< ") "<< " : " << receivedMsg.at(i)->getTypeS() <<" ricevuto da Client"<< receivedMsg.at(i)->getId() << " inviato a " << receivedMsg.at(i)->getSendTime()<<" e arrivato a " << receivedMsg.at(i)->getArrivalTime()<< "----> seq";
                                  for(auto it:receivedMsg.at(i)->getSEQcv()){
                                      EV<<"[";
                                      for(auto it2:it){
                                          EV<<it2.first<<",";
                                      }
                                      EV<<"]";
                                  }
                                  EV<<"\n";
                       }else if(strcmp(receivedMsg.at(i)->getTypeS(),"INSTALL")==0){
                           EV << i << "("<< receivedMsg.at(i)->getSender() << ","<< receivedMsg.at(i)->getMsgId()<< ") "<< " : " << receivedMsg.at(i)->getTypeS() <<" ricevuto da Client"<< receivedMsg.at(i)->getId() << " inviato a " << receivedMsg.at(i)->getSendTime()<<" e arrivato a " << receivedMsg.at(i)->getArrivalTime()<< "----> seq";
                           EV<<"[";
                           for(auto it:receivedMsg.at(i)->getInstallView()){
                              EV<<it.first<<",";
                           }
                           EV<<"]\n";
                       }else if(strcmp(receivedMsg.at(i)->getTypeS(),"NEW VIEW")==0){
                            EV_WARN << i << " : " << receivedMsg.at(i)->getTypeS() << " a "<< receivedMsg.at(i)->getSendTime() << " ----> seq";

                            EV_WARN<<"[";
                            for(auto it2:receivedMsg.at(i)->getView()){
                                EV_WARN<<it2.first<<",";
                            }
                            EV_WARN<<"]";

                            EV_WARN<<"\n";
                       }else if(strcmp(receivedMsg.at(i)->getTypeS(),"NEW MESSAGE DELIVERED")==0){
                           EV_WARN << i << "("<<receivedMsg.at(i)->getSender() << ","<< receivedMsg.at(i)->getMsgId()<< ") "<< " : " << receivedMsg.at(i)->getTypeS() << " a " << receivedMsg.at(i)->getSendTime() << " ----> seq";

                           EV_WARN<<"[";
                           for(auto it2:receivedMsg.at(i)->getView()){
                               EV_WARN<<it2.first<<",";
                           }
                           EV_WARN<<"]";

                           EV_WARN<<"\n";
                       }else{
                           EV << i << "("<< receivedMsg.at(i)->getSender() << ","<< receivedMsg.at(i)->getMsgId()<< ") "<< " : " << receivedMsg.at(i)->getTypeS() <<" ricevuto da Client"<< receivedMsg.at(i)->getId() << " inviato a " << receivedMsg.at(i)->getSendTime()<<" e arrivato a " << receivedMsg.at(i)->getArrivalTime() << "\n";
                       }
                    }
                }
        }

        EV << "\n";
        EV<< "State-Updates ricevuti: \n";
        for(auto view : init_view_){
                EV<<"\n[ ";
                for(auto elem: view){
                    EV<< elem.first << " ,";
                }
                EV<<"]:\n";
                for (int i = 0; i < states_update_rec.size(); i++) {
                    if(equalVec(view,states_update_rec.at(i)->getView())==true){
                        EV << i << " : " << states_update_rec.at(i)->getTypeS() <<" ricevuto da Client"<< states_update_rec.at(i)->getId() << " inviato a " << states_update_rec.at(i)->getSendTime()<<" e arrivato a " << states_update_rec.at(i)->getArrivalTime() << "\n";
                    }
                }
        }
        EV << "\n";
        EV<< "Messaggi deliverati: \n";
        for(auto view : init_view_){
            EV<<"\n[ ";
                for(auto elem: view){
                    EV<< elem.first << " ,";
                }
                EV<<"]:\n";
                for(auto it:deliverati){
                    if(equalVec(view,it->getView())==true){
                        EV<< it->getMsgId() << " da Client" << it->getSender() << "\n";
                    }
                }
        }
        EV << "\n";
        EV<< "Stato: \n";
        if(state.ack==true){
            EV << "ACK: TRUE \n";
            //EV << "ACK_VALUE: " << (state.ack_value)->getTypeS() << "\n";
        }else{
            EV << "ACK: FALSE \n";
        }
        if(state.conflicting==true){
            EV << "CONFLICTING: TRUE \n";
            //EV << "CONF_VALUE_1: " << (state.conflicting_value_1)->getTypeS() << "\n";
            //EV << "CONF_VALUE_2: " << (state.conflicting_value_2)->getTypeS() << "\n";
        }else{
            EV << "CONFLICTING: FALSE \n";
        }
        if(state.stored==true){
            EV << "STORED: TRUE \n";
            //EV << "STORED_VALUE: " << (state.stored_value)->getTypeS() << "\n";
        }else{
            EV << "STORED: FALSE \n";
        }
        EV << "\n";
        EV_INFO << modulePath << ": opened " << numSessions << " sessions\n\n";
        EV_INFO << modulePath << ": sent " << bytesSent << " bytes in " << packetsSent << " packets\n\n";
        EV_INFO << modulePath << ": received " << bytesRcvd << " bytes in " << packetsRcvd << " packets\n\n";

        EV << "Current view: ";
        for(auto it : current_view){
            EV << it.first << " ";
        }
        EV << "\n-----------------------------------------------------------------------\n";
    }else{
        EV_INFO << "Time finish: " << simTime() << "\n\n";
        EV_INFO << "Messaggi inviati: "<< inviati.size() <<"\n";
        for(auto view : installedView){
            EV<<"\n[ ";
            for(auto elem: view.first){
                EV<< elem.first << " ,";
            }
            EV<<"]:\n ";

            for (int i = 0; i < inviati.size(); i++) {
                if(equalVec(view.first,inviati.at(i)->getView())==true){
                    if(strcmp(inviati.at(i)->getTypeS(),"PROPOSE")==0 || strcmp(inviati.at(i)->getTypeS(),"CONVERGED")==0 ||  strcmp(inviati.at(i)->getTypeS(),"INSTALL")==0){
                      EV << i << "("<<inviati.at(i)->getSender() << ","<< inviati.at(i)->getMsgId()<< ") "<< " : " << inviati.at(i)->getTypeS() <<" inviato a Client"<< inviati.at(i)->getDestId() << " inviato a " << inviati.at(i)->getSendTime()<< "----> seq";
                      for(auto it:inviati.at(i)->getSEQcv()){
                          EV<<"[";
                          for(auto it2:it){
                              EV<<it2.first<<",";
                          }
                          EV<<"]";
                      }
                      EV<<"\n";
                   }else if(strcmp(inviati.at(i)->getTypeS(),"NEW VIEW")==0){
                       EV_WARN << i << " : " << inviati.at(i)->getTypeS() << " a "<< inviati.at(i)->getSendTime() << " ----> seq";

                       EV_WARN<<"[";
                       for(auto it2:inviati.at(i)->getView()){
                           EV_WARN<<it2.first<<",";
                       }
                       EV_WARN<<"]";

                       EV_WARN<<"\n";
                   }else if(strcmp(inviati.at(i)->getTypeS(),"NEW MESSAGE DELIVERED")==0){
                       EV_WARN << i << "("<<inviati.at(i)->getSender() << ","<< inviati.at(i)->getMsgId()<< ") "<< " : " << inviati.at(i)->getTypeS() << " a " << inviati.at(i)->getSendTime() << " ----> seq";

                       EV_WARN<<"[";
                       for(auto it2:inviati.at(i)->getView()){
                           EV_WARN<<it2.first<<",";
                       }
                       EV_WARN<<"]";

                       EV_WARN<<"\n";
                   }else{
                       EV << i << "("<< inviati.at(i)->getSender() << ","<< inviati.at(i)->getMsgId()<< ") "<< " : " << inviati.at(i)->getTypeS() <<" inviato a Client"<< inviati.at(i)->getDestId() << " inviato a " << inviati.at(i)->getSendTime() << "\n";
                   }
                }
            }
        }
        EV << "\n";
        EV_INFO << "Messaggi ricevuti: "<< receivedMsg.size() <<"\n";
        for(auto view : installedView){
            EV<<"\n[ ";
                for(auto elem: view.first){
                    EV<< elem.first << " ,";
                }
                EV<<"]:\n";

                for (int i = 0; i < receivedMsg.size(); i++) {
                    if(equalVec(view.first,receivedMsg.at(i)->getView())==true){
                       if(strcmp(receivedMsg.at(i)->getTypeS(),"PROPOSE")==0 || strcmp(receivedMsg.at(i)->getTypeS(),"CONVERGED")==0){
                           EV << i << "("<< receivedMsg.at(i)->getSender() << ","<< receivedMsg.at(i)->getMsgId()<< ") "<< " : " << receivedMsg.at(i)->getTypeS() <<" ricevuto da Client"<< receivedMsg.at(i)->getId() << " inviato a " << receivedMsg.at(i)->getSendTime()<<" e arrivato a " << receivedMsg.at(i)->getArrivalTime()<< "----> seq";
                                  for(auto it:receivedMsg.at(i)->getSEQcv()){
                                      EV<<"[";
                                      for(auto it2:it){
                                          EV<<it2.first<<",";
                                      }
                                      EV<<"]";
                                  }
                                  EV<<"\n";
                       }else if(strcmp(receivedMsg.at(i)->getTypeS(),"INSTALL")==0){
                           EV << i << "("<< receivedMsg.at(i)->getSender() << ","<< receivedMsg.at(i)->getMsgId()<< ") "<< " : " << receivedMsg.at(i)->getTypeS() <<" ricevuto da Client"<< receivedMsg.at(i)->getId() << " inviato a " << receivedMsg.at(i)->getSendTime()<<" e arrivato a " << receivedMsg.at(i)->getArrivalTime()<< "----> seq";
                           EV<<"[";
                           for(auto it:receivedMsg.at(i)->getInstallView()){
                              EV<<it.first<<",";
                           }
                           EV<<"]\n";
                       }else if(strcmp(receivedMsg.at(i)->getTypeS(),"NEW VIEW")==0){
                            EV_WARN << i << " : " << receivedMsg.at(i)->getTypeS() << " a "<< receivedMsg.at(i)->getSendTime() << " ----> seq";

                            EV_WARN<<"[";
                            for(auto it2:receivedMsg.at(i)->getView()){
                                EV_WARN<<it2.first<<",";
                            }
                            EV_WARN<<"]";

                            EV_WARN<<"\n";
                       }else if(strcmp(receivedMsg.at(i)->getTypeS(),"NEW MESSAGE DELIVERED")==0){
                           EV_WARN << i << "("<<receivedMsg.at(i)->getSender() << ","<< receivedMsg.at(i)->getMsgId()<< ") "<< " : " << receivedMsg.at(i)->getTypeS() << " a " << receivedMsg.at(i)->getSendTime() << " ----> seq";

                           EV_WARN<<"[";
                           for(auto it2:receivedMsg.at(i)->getView()){
                               EV_WARN<<it2.first<<",";
                           }
                           EV_WARN<<"]";

                           EV_WARN<<"\n";
                       }else{
                           EV << i << "("<< receivedMsg.at(i)->getSender() << ","<< receivedMsg.at(i)->getMsgId()<< ") "<< " : " << receivedMsg.at(i)->getTypeS() <<" ricevuto da Client"<< receivedMsg.at(i)->getId() << " inviato a " << receivedMsg.at(i)->getSendTime()<<" e arrivato a " << receivedMsg.at(i)->getArrivalTime() << "\n";
                       }
                    }
                }
        }

        EV << "\n";
        EV<< "State-Updates ricevuti: \n";
        for(auto view : installedView){
                EV<<"\n[ ";
                for(auto elem: view.first){
                    EV<< elem.first << " ,";
                }
                EV<<"]:\n";
                for (int i = 0; i < states_update_rec.size(); i++) {
                    if(equalVec(view.first,states_update_rec.at(i)->getView())==true){
                        EV << i << " : " << states_update_rec.at(i)->getTypeS() <<" ricevuto da Client"<< states_update_rec.at(i)->getId() << " inviato a " << states_update_rec.at(i)->getSendTime()<<" e arrivato a " << states_update_rec.at(i)->getArrivalTime() << "\n";
                    }
                }
        }
        EV << "\n";
        EV<< "Messaggi deliverati: \n";
        for(auto view : installedView){
            EV<<"\n[ ";
                for(auto elem: view.first){
                    EV<< elem.first << " ,";
                }
                EV<<"]:\n";
                for(auto it:deliverati){
                    if(equalVec(view.first,it->getView())==true){
                        EV<< it->getMsgId() << " da Client" << it->getSender() << "\n";
                    }
                }
        }
        EV << "\n";
        EV<< "Stato: \n";
        if(state.ack==true){
            EV << "ACK: TRUE \n";
            //EV << "ACK_VALUE: " << (state.ack_value)->getTypeS() << "\n";
        }else{
            EV << "ACK: FALSE \n";
        }
        if(state.conflicting==true){
            EV << "CONFLICTING: TRUE \n";
            //EV << "CONF_VALUE_1: " << (state.conflicting_value_1)->getTypeS() << "\n";
            //EV << "CONF_VALUE_2: " << (state.conflicting_value_2)->getTypeS() << "\n";
        }else{
            EV << "CONFLICTING: FALSE \n";
        }
        if(state.stored==true){
            EV << "STORED: TRUE \n";
            //EV << "STORED_VALUE: " << (state.stored_value)->getTypeS() << "\n";
        }else{
            EV << "STORED: FALSE \n";
        }
        EV << "\n";
        EV_INFO << modulePath << ": opened " << numSessions << " sessions\n\n";
        EV_INFO << modulePath << ": sent " << bytesSent << " bytes in " << packetsSent << " packets\n\n";
        EV_INFO << modulePath << ": received " << bytesRcvd << " bytes in " << packetsRcvd << " packets\n\n";

        EV << "Current view: ";
        for(auto it : current_view){
            EV << it.first << " ";
        }
        EV << "\n-----------------------------------------------------------------------\n";
    }

}

void Node::handleStopOperation(LifecycleOperation *operation) {
    socketL.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void Node::handleCrashOperation(LifecycleOperation *operation) {
    socketL.destroy();
}


bool Node::contain(int x, vector<pair<int,int>> cv) {
    for(auto i : cv){
        if(i.first==x){
            return true;
        }
    }
    return false;
}

bool Node::containVS(vector<pair<int,int>> cv,vector<vector<pair<int,int>>> seq) {
    for(auto i : seq){
        if(equalVec(i,cv)){
            return true;
        }
    }
    return false;
}

bool Node::checkPropose(int x, int join_or_leave, vector<pair<int,int>> cv) {
    for(auto i : cv){
        if(i.first==x){
            if(i.second==1 && join_or_leave==0){
                return true;
            }else if(join_or_leave==1){
                return false;
            }
        }
    }
    if(join_or_leave==1){
        return true;
    }else{
        return false;
    }
}

bool Node::checkDeliverMsg() {
    bool cond=false;
    for(auto it = deliver.begin(); it!=deliver.end(); ++it){
        for(auto it2:it->second){
            int c=0;
            int i=0;
            for(auto it3:it2.second){
                if(contain(i,it->first)==true){ //TODO and CER
                    c++;
                }
            }
            if(c>=quorumSize && firstTimeDeliver(it2.first)==true && isDelivered(it2.first)==false){
                msg_to_send.push_back(it2.first);
                cond=true;
            }
        }
    }
    return cond;
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
                if(c>=quorumSize && firstTimeDeliver(it2.first)==false){
                    j=it2.first->getMsgId();
                    break;
                    //return it2.first;
                }
            }
         }
         int k=0;
         Msg * temp;
         for(auto it:msg_to_send){
             if(it->getMsgId()==j){
                 temp=it;
                 msg_to_send.erase(msg_to_send.begin() + k);
                 return temp;
             }
             k++;
         }
    }
}

bool Node::checkAckMsg() {
    bool cond=false;
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
            if(c>=quorumSize && firstTimeAck(it2.first)==true && isDelivered(it2.first)==false){
                msg_to_ack.push_back(it2.first);
                cond=true;
            }
        }
    }
    return cond;
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

/*
bool Node::isAllowed(Msg * m ) {
    for(Msg * i : allowed_ack){
       if(equalMsg(i,m)){
           return true;
       }
    }
    return false;
}*/

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

void Node::updatePropose(vector<vector<pair<int,int>>> s1,int id) {
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
    return false;
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

void Node::ftdInsert(Msg* m){
    int i=0;
    for (auto it : first_time_deliver) {
        if(equalVec(it.first,m->getView())){
            for(auto it2:it.second){
                if(equalMsg(it2,m)==true){
                    return;
                }
            }
            first_time_deliver[i].second.push_back(m);
            return;
        }
        i++;
    }
    vector<Msg*> v={};
    v.push_back(m);
    first_time_deliver.push_back(make_pair(m->getView(),v));
    return;
}

void Node::ftaInsert(Msg* m){
    int i=0;
    for (auto it : first_time_ack) {
        if(equalVec(it.first,m->getView())){
            for(auto it2:it.second){
                if(equalMsg(it2,m)==true){
                    return;
                }
            }
            first_time_ack[i].second.push_back(m);
            return;
        }
        i++;
    }
    vector<Msg*> v={};
    v.push_back(m);
    first_time_ack.push_back(make_pair(m->getView(),v));
    return;
}

bool Node::firstTimeDeliver(Msg * m) {
    for(auto it : first_time_deliver) {
            if(equalVec(it.first,m->getView())){
                for(auto it2:it.second){
                    if(equalMsg(m,it2)==true){
                       return false;
                   }
                }
            }
        }
        return true;
}

bool Node::firstTimeAck(Msg * m) {
    for(auto it : first_time_ack) {
        if(equalVec(it.first,m->getView())){
            for(auto it2:it.second){
                if(equalMsg(m,it2)==true){
                   return false;
               }
            }
        }
    }
    return true;
}


bool Node::isDelivered(Msg * m) {
    for(auto it : deliverati) {
        if(equalMsg(m,it)){
            return true;
        }
    }
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
    return {};
}

bool Node::checkConverge() {
    for (auto it = converge.begin(); it!=converge.end(); ++it) {
        int c=0;
        for(int i:it->second){
            c+=i;
        }
        if(c>=quorumSize){
            return true;
        }
    }
    return false;
}

double Node::fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}


vector<vector<pair<int,int>>> Node::returnConverge() {
    for (auto it = converge.begin(); it!=converge.end(); ++it) {
        int c=0;
        for(int i:it->second){
           c+=i;
        }
        if(c>=quorumSize){
            return it->first;
        }
    }
}

void Node::view_discovery() {
   current_view=init_view;
   int tmp = sizeCV(current_view) + f + 1;
   if (tmp % 2 == 1){
       tmp++;
   }
   quorumSize = tmp/2;
   //installedView={{current_view,1}};
}

bool Node::isAllowed(Msg * m) {
    for(auto it: allowed_ack_value){
        if(equalMsg(m,it)){
            return true;
        }
    }
    return false;
}

bool Node::isStored(Msg * m) {
    for(auto it: stored_value){
        if(equalMsg(m,it)){
            return true;
        }
    }
    return false;
}

bool Node::isAllowedS(Msg * m) {
    for(auto it: state.ack_value){
        if(equalMsg(m,it)){
            return true;
        }
    }
    return false;
}

bool Node::isStoredS(Msg * m) {
    for(auto it: state.stored_value){
        if(equalMsg(m,it)){
            return true;
        }
    }
    return false;
}

void Node::join(double x){
    timerEvent = new cMessage();
    timerEvent->setKind(MSGKIND_JOIN);
    scheduleAt(simTime()+x,timerEvent);
}

void Node::broadcast(double x){
    timerEvent = new cMessage();
    timerEvent->setKind(MSGKIND_BROADCAST);
    scheduleAt(simTime()+x,timerEvent);
}

void Node::timer(double x){
    timerEvent = new cMessage();
    timerEvent->setKind(MSGKIND_TIMER);
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

msg_ Node::castMsgToMsg_(Msg* m){
    msg_ r;
    r.SEQcv=m->getSEQcv();
    r.Vcer=m->getVcer();
    r.cer=m->getCer();
    r.id=m->getId();
    r.installView=m->getInstallView();
    r.install_or_update=m->getInstall_or_update();
    r.join_or_leave=m->getJoin_or_leave();
    r.msg=m->getMsg();
    r.msgId=m->getMsgId();
    r.sender=m->getSender();
    r.type=m->getType();
    r.typeS=m->getTypeS();
    r.view=m->getView();
    return r;
}

int Node::sizeCV(vector<pair<int,int>> v1){
    int c=0;
    for(pair<int,int> i:v1){
        c++;
    }
    return c;
}

int Node::sizeS(vector<vector<pair<int,int>>> s){
    int c=0;
    for(vector<pair<int,int>> i:s){
        c++;
    }
    return c;
}

bool Node::isInstalled(vector<pair<int,int>> v1){
    for (auto it : installedView) {
        if(equalVec(it.first,v1)){
            if(it.second==1){
                return true;
            }
        }
    }
    return false;
}

void Node::changeCV(vector<pair<int,int>> v1){
    current_view=v1;
    return;
}

void Node::deInstall(vector<pair<int,int>> v1){
    for (auto it : installedView) {
        if(equalVec(it.first,v1)){
            it.second=0;
            return;
        }
    }
    installedView.push_back(make_pair(v1,0));
    return;
}

void Node::install(vector<pair<int,int>> v1){
    int id=0;
    bool cond=true;
    for (auto it : installedView) {
        if(equalVec(it.first,v1)){
            cond=false;
            break;
        }
        id++;
    }
    if(cond==false){
        installedView[id].second=1;
        return;
    }else{
        installedView.push_back(make_pair(v1,1));
        return;
    }
}

bool Node::conflictS(vector<vector<pair<int,int>>> s1, vector<vector<pair<int,int>>> s2){
    for(vector<pair<int,int>> i:s1){
        for(vector<pair<int,int>> j:s2){
            if(conflictV(i,j)==true){
               return true;
            }
        }
    }
    return false;
}

vector<vector<pair<int,int>>> Node::unionS(vector<vector<pair<int,int>>> s1, vector<vector<pair<int,int>>> s2){
    vector<vector<pair<int,int>>> s={};
    for(vector<pair<int,int>> i:s1){
        s.push_back(i);
    }
    for(vector<pair<int,int>> i:s2){
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

bool Node::contains(vector<pair<int,int>> v1, vector<pair<int,int>> v2){ //v1 contains v2 -> v2 is subset of v1
    EV<<"CONTAINS\n";
    EV<<"v1:";
    for(auto it:v1){
        EV<<it.first<<" ";
    }
    EV<<"\nv2:";
    for(auto it2:v2){
        EV<<it2.first<<" ";
    }
    if(sizeCV(v1)<=sizeCV(v2)){
       EV<<"\nFalse";
       return false;
    }
    int count=0;
    for(pair<int,int> i:v2){
        int n=0;
        for(pair<int,int> j:v1){
            if(i.first==j.first){
                if(i.second==j.second){
                    break;
                }
                else{
                    count++;
                }
            }
            n++;
        }
        if(n==sizeCV(v1)){
            EV<<"\nFalse";
            return false;
        }
    }
    if(count>0){
        EV<<"\nFalse";
        return false;
    }else{
        EV<<"\nTrue";
        return true;
    }
}
/*
bool Node::conflictV(vector<pair<int,int>> v1, vector<pair<int,int>> v2){
    int count=0;
    int count2=0;
    bool cond=true;
    bool cond2=true;
    // v1 non è sottoinsieme di v2
    for(pair<int,int> i:v1){
        for(pair<int,int> j:v2){
            if(i.first==j.first){
                if(i.second==j.second){
                    cond=false;
                    break;
                }
                else{
                    if(i.second==1){
                        count++;
                        cond=false;
                        break;
                    }
                }
            }
        }
        if(cond==true){
            count++;
        }
    }
    // v2 non è sottoinsieme di v1
    for(pair<int,int> i:v2){
            for(pair<int,int> j:v1){
                if(i.first==j.first){
                    if(i.second==j.second){
                        cond2=false;
                        break;
                    }
                    else{
                        if(j.second==1){
                            count2++;
                            cond2=false;
                            break;
                        }
                    }
                }
            }
            if(cond2==true){
                count2++;
            }
        }
    if(count*count2>0){
        return true;
    }else{
        return false;
    }
}*/

bool Node::conflictV(vector<pair<int,int>> v1, vector<pair<int,int>> v2){
    int count=0;
    int count2=0;
    // v1 non è sottoinsieme di v2
    for(pair<int,int> i:v1){
        bool cond=true;
        for(pair<int,int> j:v2){
            if(i.first==j.first){
                cond=false;
                break;
            }
        }
        if(cond==true){
            count++;
        }
    }
    // v2 non è sottoinsieme di v1
    for(pair<int,int> i:v2){
        bool cond2=true;
        for(pair<int,int> j:v1){
            if(i.first==j.first){
                cond2=false;
                break;
            }
        }
        if(cond2==true){
            count2++;
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
    if(m1->getMsgId()==m2->getMsgId() && m1->getSender()==m2->getSender()) return true;
    else return false;
}



void Node::uponRECV(){
   if(SEQv.empty()==true){
       vector<pair<int,int>> seq = merge(current_view,RECV);
       sort(seq.begin(), seq.end(), less<pair<int,int> >());
       SEQv.push_back(seq);

       for(auto it:msg4view){
            if(it->getId()==selfId){
                if(it->getType()==PROPOSE){
                    if(equalSeq(it->getSEQcv(),SEQv)==true){
                        return;
                    }
                }
            }
        }
       bool ft=true;
       for (int destId = 0; destId < nodesNbr; destId++) {
           if(destId==selfId) continue;
           if(contain(destId,current_view)==false) continue;
           Msg * bp = new Msg();
           bp->setId(selfId);
           bp->setMsg("Propose");
           simtime_t t=simTime()+fRand(0.0, 1.0);
           bp->setSendTime(t);
           bp->setView(current_view);
           bp->setType(PROPOSE);
           bp->setTypeS("PROPOSE");
           bp->setSEQcv(SEQv);

           if(ft==true){
               msg4view.push_back(bp);
               ft=false;
           }

           auto briefPacket = new Packet();
           Msg &tmp = *bp;
           const auto& payload = makeShared<Msg>(tmp);
           briefPacket->insertAtBack(payload);
           //EV << "\n\n\n\n [Client"<< selfId <<"]PROPOSE messaggio inviato a: " << simTime() << "\n\n\n";
           //sendP(briefPacket, destId);
           MessageL * timer = new MessageL();
           timer->setDestId(destId);
           timer->setKind(MSGKIND_SEND);
           timer->setM(bp);
           //scheduleAt(simTime(),timer);
           scheduleAt(t,timer);
       }
   }
}

void Node::reconfig_f(Msg * x){
    EV << "\n\n\n\n [Client"<< selfId <<"]RECONFIG ricevuto a: " << simTime() << "\n\n\n";
    if(x->getView()==current_view){
        //bool condition = false;
        if(checkPropose(x->getId(),x->getJoin_or_leave(),current_view)){

            receivedMsg.push_back(x);
            if(contain(x->getId(),RECV)==false){
                RECV.push_back(std::make_pair(x->getId(),x->getJoin_or_leave()));
                sort(RECV.begin(), RECV.end(), less<pair<int,int> >());
                //condition=true;
            }

            Msg * bp = new Msg();
            bp->setId(selfId);
            bp->setMsg("Join reply");
            simtime_t t=simTime()+fRand(0.0, 1.0);
            bp->setSendTime(t);
            bp->setView(current_view);
            bp->setType(REC_CONFIRM);
            bp->setTypeS("REC_CONFIRM");
            auto briefPacket = new Packet();
            Msg &tmp = *bp;
            const auto& payload = makeShared<Msg>(tmp);
            briefPacket->insertAtBack(payload);
            //EV << "\n\n\n\n [Client"<< selfId <<"]REC-CONFIRM inviato a: " << simTime() << "\n\n\n";
            //sendP(briefPacket, x->getId());
            MessageL * timer = new MessageL();
            timer->setDestId(x->getId());
            timer->setKind(MSGKIND_SEND);
            timer->setM(bp);
            //scheduleAt(simTime(),timer);
            scheduleAt(t,timer);
        }
        /*if(condition==true){
            uponRECV();
        }*/
    }
}

void Node::rec_confirm_f(Msg * x){
    EV << "\n\n\n\n [Client"<< selfId <<"]REC-CONFIRM ricevuto a: " << simTime() << "\n\n\n";
    if(req_join_or_leave[x->getId()]==0){
        req_join_or_leave[x->getId()]=1;
        count_req_join_or_leave++;
        if(count_req_join_or_leave>=quorumSize){
            rec_cond=true;
        }
    }
}

void Node::propose_f(Msg * x){
    if(contain(x->getId(),x->getView())==true){
        if(FORMATv.empty()==true || isContainedIn(x->getSEQcv(),FORMATv)){
            //TODO check if SEQ is valid or not
            receivedMsg.push_back(x);
            if(conflictS(x->getSEQcv(),SEQv)==true){
                vector<pair<int,int>> w = mostRecent(x->getSEQcv());
                vector<pair<int,int>> w1 = mostRecent(SEQv);

                vector<pair<int,int>> w3 = merge(w,w1);
                for(auto it:w3){
                    EV<<it.first;
                }
                vector<vector<pair<int,int>>> ww1={};
                ww1.push_back(w3);
                SEQv=LCSEQv;
                vector<vector<pair<int,int>>> temp=unionS(SEQv,ww1);
                SEQv=temp;

            }
            else{
                vector<vector<pair<int,int>>> q = x->getSEQcv();
                SEQv=unionS(SEQv,q);
                /*EV<<"merge"<<selfId<<"\n";
                for(auto it:SEQv){
                    for(auto it2:it){
                        EV<<it2.first;
                    }
                    EV<<"\n";
                }*/
            }

            bool cond=true;
            for(auto it:msg4view){
                if(it->getId()==selfId){
                    if(it->getType()==PROPOSE){
                        if(equalSeq(it->getSEQcv(),SEQv)==true){
                            cond=false;
                            break;
                        }
                    }
                }
            }
            bool ft=true;
            if(cond==true){
                for (int destId = 0; destId < nodesNbr; destId++) {
                   if(destId==selfId) continue;
                   if(contain(destId,x->getView())==false) continue;
                   Msg * bp = new Msg();
                   bp->setId(selfId);
                   bp->setMsg("Propose");
                   simtime_t t=simTime()+fRand(0.0, 1.0);
                   bp->setSendTime(t);
                   bp->setView(current_view);
                   bp->setType(PROPOSE);
                   bp->setTypeS("PROPOSE");
                   bp->setSEQcv(SEQv);
                   auto briefPacket = new Packet();

                   if(ft==true){
                       msg4view.push_back(bp);
                       ft=false;
                   }

                   Msg &tmp = *bp;
                   const auto& payload = makeShared<Msg>(tmp);
                   briefPacket->insertAtBack(payload);
                   //EV << "\n\n\n\n [Client"<< selfId <<"]PROPOSE messaggio inviato a: " << simTime() << "\n\n\n";
                   //sendP(briefPacket, destId);
                   MessageL * timer = new MessageL();
                   timer->setDestId(destId);
                   timer->setKind(MSGKIND_SEND);
                   timer->setM(bp);
                   //scheduleAt(simTime(),timer);
                   scheduleAt(t,timer);
                }
            }

         }
        //if(isReceivedP(x->getSEQcv(),x->getId())==false){
        EV << "\n\n\n\n [Client"<< selfId <<"]PROPOSE ricevuto a: " << simTime() << "\n\n\n";
        updatePropose(x->getSEQcv(),x->getId());
        if(checkPropose()==true){
            LCSEQv = returnPropose();

            /*for(auto it:msg4view){
                if(it->getId()==selfId){
                    if(it->getType()==CONVERGED){
                        if(equalSeq(it->getSEQcv(),x->getSEQcv())==true){
                            return;
                        }
                    }
                }
            }
            bool ft=true;*/
            for (int destId = 0; destId < nodesNbr; destId++) {
              if(destId==selfId) continue;
              if(contain(destId,x->getView())==false) continue;
              Msg * bp = new Msg();
              bp->setId(selfId);
              bp->setMsg("Converged");
              simtime_t t=simTime()+fRand(0.0, 1.0);
              bp->setSendTime(t);
              bp->setView(x->getView());
              bp->setType(CONVERGED);
              bp->setTypeS("CONVERGED");
              bp->setSEQcv(x->getSEQcv());
              auto briefPacket = new Packet();
              Msg &tmp = *bp;

              /*if(ft==true){
                  msg4view.push_back(bp);
                  ft=false;
              }*/

              const auto& payload = makeShared<Msg>(tmp);
              briefPacket->insertAtBack(payload);
              //EV << "\n\n\n\n [Client"<< selfId <<"]CONVERGED inviato a: " << simTime() << "\n\n\n";
              //sendP(briefPacket, destId);
              MessageL * timer = new MessageL();
              timer->setDestId(destId);
              timer->setKind(MSGKIND_SEND);
              timer->setM(bp);
              //scheduleAt(simTime(),timer);
              scheduleAt(t,timer);
            }
        }
        //}
    }
}

void Node::converge_f(Msg * x){
    if(contain(x->getId(),x->getView())==true){
        if(isReceivedC(x->getSEQcv(),x->getId())==false){

            receivedMsg.push_back(x);
            EV << "\n\n\n\n [Client"<< selfId <<"]CONVERGED ricevuto a: " << simTime() << "\n\n\n";
            updateConverge(x->getSEQcv(),x->getId());
            if(checkConverge()==true){
                vector<vector<pair<int,int>>> k = returnConverge();
                vector<pair<int,int>> w = leastRecent(k);
                sort(w.begin(), w.end(), less<pair<int,int> >());

                for(auto it:msg4view){
                    if(it->getId()==selfId){
                        if(it->getType()==INSTALL){
                            if(equalVec(it->getInstallView(),w)==true && equalSeq(it->getSEQcv(),x->getSEQcv())==true){
                                return;
                            }
                        }
                    }
                }
                bool ft=true;
                for (int destId = 0; destId < nodesNbr; destId++) {
                  if(destId==selfId) continue;
                  if(contain(destId,w)==true || contain(destId,current_view)==true){
                    Msg * bp = new Msg();
                    bp->setId(selfId);
                    bp->setMsg("Install");
                    simtime_t t=simTime()+fRand(0.0, 1.0);
                    bp->setSendTime(t);
                    bp->setInstallView(w);
                    bp->setType(INSTALL);
                    bp->setTypeS("INSTALL");
                    bp->setSEQcv(x->getSEQcv());
                    bp->setView(x->getView());
                    auto briefPacket = new Packet();
                    Msg &tmp = *bp;

                    if(ft==true){
                        msg4view.push_back(bp);
                        ft=false;
                    }

                    const auto& payload = makeShared<Msg>(tmp);
                    briefPacket->insertAtBack(payload);
                    //EV << "\n\n\n\n [Client"<< selfId <<"]INSTALL inviato a: " << simTime() << "\n\n\n";
                    //sendP(briefPacket, destId);
                    MessageL * timer = new MessageL();
                    timer->setDestId(destId);
                    timer->setKind(MSGKIND_SEND);
                    timer->setM(bp);
                    //scheduleAt(simTime(),timer);
                    scheduleAt(t,timer);
                  }
                }
            }
        }
    }
}

void Node::prepare_f(Msg * x){
    if(equalVec(x->getView(),current_view)==true && isMember(x->getId(),current_view)==true){
        //msg_to_send.push_back(x); //TODO: se gia non c e
        receivedMsg.push_back(x);
        if(allowed_ack==false || isAllowed(x)==false ){
            allowed_ack=true;
            if(isAllowed(x)==false){
                allowed_ack_value.push_back(x);
            }
            if(state.ack==false || isAllowedS(x)==false){
                state.ack=true;
                if(isAllowedS(x)==false){
                    state.ack_value.push_back(x);
                }
            }
            //TODO sign
            Msg * bp = new Msg();
            bp->setId(selfId);
            bp->setMsg("ack");
            simtime_t t=simTime()+fRand(0.0, 1.0);
            bp->setSendTime(t);
            bp->setType(ACK);
            bp->setTypeS("ACK");
            bp->setMsgId(x->getMsgId());
            bp->setView(current_view);
            bp->setSender(x->getSender());
            auto briefPacket = new Packet();
            Msg &tmp = *bp;
            const auto& payload = makeShared<Msg>(tmp);
            briefPacket->insertAtBack(payload);
            //EV << "\n\n\n\n [Client"<< selfId <<"]ACK inviato a: " << simTime() << "\n\n\n";
            //sendP(briefPacket, x->getId());
            MessageL * timer = new MessageL();
            timer->setDestId(x->getId());
            timer->setKind(MSGKIND_SEND);
            timer->setM(bp);
            //scheduleAt(simTime(),timer);
            scheduleAt(t,timer);
        }
    }
}

void Node::install_f(Msg * x){
    EV<<"InstallRec"<<selfId;
    for(auto it:x->getSEQcv()){
        EV<<"[";
        for(auto it2:it){
            EV<<it2.first<<",";
        }
        EV<<"]";
    }

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
            vector<Msg*> temp={};
            for(auto it:state.stored_value){
                bool c=false;
                for(auto it2:deliverati){
                    if(equalMsg(it,it2)==true){
                        c=true;
                        break;
                    }
                }
                if(c==false){
                    for(auto it2:msg_to_send){
                        if(equalMsg(it,it2)==true){
                            c=true;
                            break;
                        }
                    }
                }
                if(c==false){
                    temp.push_back(it);
                }
            }
            if(state.stored_value.empty()==true){
                state.stored=false;
            }
            state.stored_value=temp;
            for (int destId = 0; destId < nodesNbr; destId++) {
               if(destId==selfId) continue;
               if(contain(destId,x->getView())==true || contain(destId,x->getInstallView())==true){
                      StateUpdateMessage * bp = new StateUpdateMessage();
                      bp->setSendTime(simTime());
                      bp->setId(selfId);
                      bp->setInstallView(x->getInstallView());
                      bp->setType(STATE_UPDATE);
                      bp->setTypeS("STATE_UPDATE");
                      bp->setAck(state.ack);
                      bp->setAck_value(state.ack_value);
                      bp->setConflicting(state.conflicting);
                      bp->setConflicting_value(state.conflicting_value);
                      bp->setStored(state.stored);

                      bp->setStored_value(state.stored_value);
                      bp->setView(x->getView());
                      bp->setSEQcv(x->getSEQcv());
                      auto briefPacket = new Packet();
                      StateUpdateMessage &tmp = *bp;
                      const auto& payload = makeShared<StateUpdateMessage>(tmp);
                      briefPacket->insertAtBack(payload);
                      sendP(briefPacket, destId);
               }
            }
        }
    }
}

void Node::state_update_f(StateUpdateMessage * x){
    if(contains(x->getInstallView(),current_view)==true){
        //TODO state update
        //states_update.push_back(x);
        StateUpdate * s = new StateUpdate();
        s->ack=x->getAck();
        s->ack_value=(vector<Msg*>)x->getAck_value();
        s->conflicting=x->getConflicting();
        s->conflicting_value=(vector<Msg*>)x->getConflicting_value();
        s->stored=x->getStored();
        s->stored_value=(vector<Msg*>)x->getStored_value();

        bool cond=false;
        int i=-1;
        int j=0;
        for(auto it : statesReceived){
            if(equalVec(it.first,x->getInstallView())==true){
                statesReceived[j].second.push_back(s);
                cond=true;
                if(statesReceived[j].second.size()>=quorumSize && containVS(x->getInstallView(),first_time_state_update)==false ){
                    i=j;
                    first_time_state_update.push_back(x->getInstallView());
                    break;
                }
            }
            j++;
        }
        if(cond==false){
            vector<StateUpdate*> v={};
            v.push_back(s);
            statesReceived.push_back(make_pair(x->getInstallView(),v));
        }
        if(i==-1){
            return;
        }
        for(auto elem : states_update){
            if(equalVec(elem->getInstallView(),statesReceived[i].first)==true){
                for(auto e : elem->getRECV()){
                    bool c=true;
                    for(auto e2 : req){
                        if(e==e2){
                            c=false;
                            break;
                        }
                    }
                    if(c==true){
                        req.push_back(e);
                    }
                }
            }
        }

        for(auto elem : req){
            bool c=true;
            for(auto e : RECV){
                if(elem==e){
                    c=false;
                    break;
                }
            }
            if(c==true){
                for(auto elem :x->getInstallView()){

                }
                RECV.push_back(elem);
            }
        }

        vector<pair<int,int>> tmp={};
        for(auto e : RECV){
            bool c=true;
            for(auto elem :x->getInstallView()){
                 if(e==elem){
                     c=false;
                     break;
                 }
            }
            if(c==true){
                tmp.push_back(e);
            }
        }

        RECV=tmp;
        EV<<"\n\n\nDEBUG RECV : \n";
        for(auto i : RECV){
            EV<<i.first<<" ";
        }
        deInstall(x->getInstallView());

        stateTransfer();

        EV << "\n\n\n\n\n STATES UPDATE\n";
        int c=0;
        for(auto e:states){
            EV << c << "\n";
            EV << "ACK: " << e->ack << "\n";
            //if(e->ack==true){
            //    EV << "ACK_VALUE: " << (e->ack_value)->getTypeS() << "\n";
            //}
            EV << "CONFLICTING: " << e->conflicting << "\n";
            //if(e->conflicting==true){
            //    EV << "CONF_VALUE_1: " << (e->conflicting_value_1)->getTypeS() << "\n";
            //    EV << "CONF_VALUE_2: " << (e->conflicting_value_2)->getTypeS() << "\n";
            //}
            EV << "STORED: " << e->stored << "\n";
            //if(e->stored==true){
            //    EV << "STORED_VALUE: " << (e->stored_value)->getTypeS() << "\n";
            //}
            EV << "\n\n";
            c++;
        }
        EV << "\n\n\n\n\n";

        if(isMember(selfId,x->getInstallView())==true){
            EV << "IN1" << selfId;
            current_view=x->getInstallView();
            bool s=true;
            for(auto it:init_view_){
                if(equalVec(it,x->getInstallView())==true){
                    s=false;
                    break;
                }
            }
            if(s==true){
                if(contains(init_view,x->getInstallView())==false){
                    init_view=x->getInstallView();
                    int tmp = sizeCV(init_view) + f + 1;
                    if (tmp % 2 == 1){
                        tmp++;
                    }
                    quorumSize = tmp/2;
                }
                init_view_.push_back(x->getInstallView());
            }
            for(auto it : x->getInstallView()){
               EV << it.first << " ";
            }
            EV<<"\n";
            for(auto it2 : current_view){
               EV << it2.first << " ";
            }
            msg4view={};
            //changeCV(x->getInstallView());
            if(isMember(selfId,x->getView())==false){
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

            if(sizeS(seq_)>0){
                EV << "IN2" << selfId;
                SEQv = seq_;
                bool cond=true;
                for(auto it:msg4view){
                    if(it->getId()==selfId){
                        if(it->getType()==PROPOSE){
                            if(equalSeq(it->getSEQcv(),SEQv)==true){
                                cond=false;
                                break;
                            }
                        }
                    }
                }

                if(cond==true){
                    EV << "IN3" << selfId;
                    bool ft=true;
                    for (int destId = 0; destId < nodesNbr; destId++) {
                       if(destId==selfId) continue;
                       if(contain(destId,current_view)==false) continue;
                       Msg * bp = new Msg();
                       bp->setId(selfId);
                       bp->setMsg("Propose");
                       simtime_t t=simTime()+fRand(0.0, 1.0);
                       bp->setSendTime(t);
                       bp->setView(current_view);
                       bp->setType(PROPOSE);
                       bp->setTypeS("PROPOSE");
                       bp->setSEQcv(SEQv);
                       auto briefPacket = new Packet();
                       Msg &tmp = *bp;

                       if(ft==true){
                           msg4view.push_back(bp);
                           ft=false;
                       }

                       const auto& payload = makeShared<Msg>(tmp);
                       briefPacket->insertAtBack(payload);
                       //EV << "\n\n\n\n [Client"<< selfId <<"]PROPOSE messaggio inviato a: " << simTime() << "\n\n\n";
                       //sendP(briefPacket, destId);
                       MessageL * timer = new MessageL();
                       timer->setDestId(destId);
                       timer->setKind(MSGKIND_SEND);
                       timer->setM(bp);
                       //scheduleAt(simTime(),timer);
                       scheduleAt(t,timer);
                    }
                }
            }else{
                EV << "IN4" << selfId;
                install(current_view);
                msg4view={};
                //init_view=current_view;
                //init_view_.push_back(current_view);
                for(auto it : current_view){
                    EV << it.first << " ";
                }
                for (int destId = 0; destId < nodesNbr; destId++) {
                   if(destId==selfId) continue;
                   if(contain(destId,current_view)==true) continue;
                   Msg * bp = new Msg();
                   bp->setId(selfId);
                   bp->setMsg("ChangeView");
                   simtime_t t=simTime();
                   bp->setSendTime(t);
                   bp->setView(current_view);
                   bp->setType(CHANGE_VIEW);
                   bp->setTypeS("CHANGE_VIEW");
                   auto briefPacket = new Packet();
                   Msg &tmp = *bp;
                   const auto& payload = makeShared<Msg>(tmp);
                   briefPacket->insertAtBack(payload);
                   //EV << "\n\n\n\n [Client"<< selfId <<"]PROPOSE messaggio inviato a: " << simTime() << "\n\n\n";
                   //sendP(briefPacket, destId);
                   MessageL * timer = new MessageL();
                   timer->setDestId(destId);
                   timer->setKind(MSGKIND_SEND);
                   timer->setM(bp);
                   //scheduleAt(simTime(),timer);
                   scheduleAt(t,timer);
                }
                stop_processing = false;
                //TODO new_view()
                Msg * prova=new Msg();
                prova->setTypeS("NEW VIEW");
                prova->setView(current_view);
                prova->setSendTime(simTime());
                Msg * prova2=new Msg();
                prova2->setTypeS("NEW VIEW");
                prova2->setView(current_view);
                prova2->setSendTime(simTime());
                receivedMsg.push_back(prova);
                inviati.push_back(prova2);
                EV << "IN5" << selfId;
                newView();
            }
        }else if(isMember(selfId,x->getInstallView())==false){
            EV << "IN6" << selfId;
            //TODO LEAVE
        }
    }
}

void Node::newView(){
    /*
    if(){ //ho mandato un messaggio ma non ho ancora il certificato

    }
    if(){ //ho mandato un messaggio e ho il certificato e non posso uscire

    }
    if(){ //non ho mandato il messaggio e ho uno stored e non posso uscire

    }*/
/*
    for(auto it:allowed_ack_value){
        if(it->getSender()==selfId && isDelivered(it)==false){
            for (int destId = 0; destId < nodesNbr; destId++) {
               if(destId==selfId) continue;
               if(contain(destId,current_view)==false) continue;
               Msg * bp = it;
               bp->setId(selfId);
               simtime_t t=simTime()+fRand(0.0, 1.0);
               bp->setSendTime(t);
               bp->setView(current_view);
               auto briefPacket = new Packet();
               Msg &tmp = *bp;
               const auto& payload = makeShared<Msg>(tmp);
               briefPacket->insertAtBack(payload);
               //EV << "\n\n\n\n [Client"<< selfId <<"]COMMIT messaggio inviato a: " << simTime() << "\n\n\n";
               //sendP(briefPacket, destId);
               MessageL * timer = new MessageL();
               timer->setDestId(destId);
               timer->setKind(MSGKIND_SEND);
               timer->setM(bp);
               //scheduleAt(simTime(),timer);
               scheduleAt(t,timer);
            }
        }
    }
    for(auto it:stored_value){
        if(it->getSender()==selfId && can_leave==false && isDelivered(it)==false){
            for (int destId = 0; destId < nodesNbr; destId++) {
               if(destId==selfId) continue;
               if(contain(destId,current_view)==false) continue;
               Msg * bp = it;
               bp->setId(selfId);
               simtime_t t=simTime()+fRand(0.0, 1.0);
               bp->setSendTime(t);
               bp->setView(current_view);
               auto briefPacket = new Packet();
               Msg &tmp = *bp;
               const auto& payload = makeShared<Msg>(tmp);
               briefPacket->insertAtBack(payload);
               //EV << "\n\n\n\n [Client"<< selfId <<"]COMMIT messaggio inviato a: " << simTime() << "\n\n\n";
               //sendP(briefPacket, destId);
               MessageL * timer = new MessageL();
               timer->setDestId(destId);
               timer->setKind(MSGKIND_SEND);
               timer->setM(bp);
               //scheduleAt(simTime(),timer);
               scheduleAt(t,timer);
            }
        }else if(can_leave==false && isDelivered(it)==false){
            for (int destId = 0; destId < nodesNbr; destId++) {
               if(destId==selfId) continue;
               if(contain(destId,current_view)==false) continue;
               Msg * bp = it;
               bp->setId(selfId);
               simtime_t t=simTime()+fRand(0.0, 1.0);
               bp->setSendTime(t);
               bp->setView(current_view);
               auto briefPacket = new Packet();
               Msg &tmp = *bp;
               const auto& payload = makeShared<Msg>(tmp);
               briefPacket->insertAtBack(payload);
               //EV << "\n\n\n\n [Client"<< selfId <<"]COMMIT messaggio inviato a: " << simTime() << "\n\n\n";
               //sendP(briefPacket, destId);
               MessageL * timer = new MessageL();
               timer->setDestId(destId);
               timer->setKind(MSGKIND_SEND);
               timer->setM(bp);
               //scheduleAt(simTime(),timer);
               scheduleAt(t,timer);
            }
        }
    }*/
    bool c=true;
    for(auto it:msg_prepare){
        for(auto it2:certificati){
            if(equalMsg(it,it2.first)==true){
                c=false;
                if(can_leave==false && isDelivered(it)==false){
                    EV << "[" << it->getSender() << ", "<<it->getMsgId()<<"],";
                    for (int destId = 0; destId < nodesNbr; destId++) {
                        if(destId==selfId) continue;
                        if(contain(destId,current_view)==true){
                          Msg * bp = new Msg();
                          bp->setId(selfId);
                          bp->setMsg(it->getMsg());
                          bp->setMsgId(it->getMsgId());
                          bp->setVcer(it2.second);
                          simtime_t t=simTime()+fRand(0.0, 1.0);
                          bp->setSendTime(t);
                          bp->setType(COMMIT);
                          bp->setTypeS("COMMIT");
                          bp->setView(current_view);
                          bp->setSender(selfId);
                          auto briefPacket = new Packet();
                          Msg &tmp = *bp;
                          const auto& payload = makeShared<Msg>(tmp);
                          briefPacket->insertAtBack(payload);
                          //EV << "\n\n\n\n [Client"<< selfId <<"]COMMIT inviato a: " << simTime() << "\n\n\n";
                          //sendP(briefPacket, destId);
                          MessageL * timer = new MessageL();
                          timer->setDestId(destId);
                          timer->setKind(MSGKIND_SEND);
                          timer->setM(bp);
                          //scheduleAt(simTime(),timer);
                          scheduleAt(t,timer);
                        }
                    }
                }
            }
        }
        if(c==true){
            EV << "[" << it->getSender() << ", "<<it->getMsgId()<<"],";
            for (int destId = 0; destId < nodesNbr; destId++) {
                if(destId==selfId) continue;
                if(contain(destId,current_view)==false) continue;
                Msg * bp = it;
                simtime_t t=simTime()+fRand(0.0, 1.0);
                bp->setSendTime(t);
                bp->setView(current_view);
                auto briefPacket = new Packet();
                Msg &tmp = *bp;
                const auto& payload = makeShared<Msg>(tmp);
                briefPacket->insertAtBack(payload);
                //EV << "\n\n\n\n [Client"<< selfId <<"]PREPARE inviato a: " << simTime() << "\n\n\n";
                //sendP(briefPacket, destId);
                MessageL * timer = new MessageL();
                timer->setDestId(destId);
                timer->setKind(MSGKIND_SEND);
                timer->setM(bp);
                //scheduleAt(simTime(),timer);
                scheduleAt(t,timer);
            }
        }
    }

    for(auto it:stored_value){
        EV << "[" << it->getSender() << ", "<<it->getMsgId()<<"],";
        if(it->getSender()!=selfId && can_leave==false && isDelivered(it)==false){
            for (int destId = 0; destId < nodesNbr; destId++) {
               if(destId==selfId) continue;
               if(contain(destId,current_view)==false) continue;
               Msg * bp = it->dup();
               bp->setId(selfId);
               simtime_t t=simTime()+fRand(0.0, 1.0);
               bp->setSendTime(t);
               bp->setView(current_view);
               auto briefPacket = new Packet();
               Msg &tmp = *bp;
               const auto& payload = makeShared<Msg>(tmp);
               briefPacket->insertAtBack(payload);
               //EV << "\n\n\n\n [Client"<< selfId <<"]COMMIT messaggio inviato a: " << simTime() << "\n\n\n";
               //sendP(briefPacket, destId);
               MessageL * timer = new MessageL();
               timer->setDestId(destId);
               timer->setKind(MSGKIND_SEND);
               timer->setM(bp);
               //scheduleAt(simTime(),timer);
               scheduleAt(t,timer);
            }
        }
    }
    //states={};
    //states_update={};

}


void Node::stateTransfer(){
    vector<Msg*> acknowledged={};
    vector<Msg*> stored_={};
    vector<Msg*> conflicting_={};
    for(auto it:states){
        if(it->ack==true){
            for(auto it3:it->ack_value){
                bool c=true;
                for(auto it2:acknowledged){
                    if(it3->getMsgId()==it2->getMsgId() && it3->getSender()==it2->getSender()){
                        c=false;
                        break;
                    }
                }
                if(c==true){
                    acknowledged.push_back(it3);
                }
            }
        }
        if(it->stored==true){
            for(auto it3:it->stored_value){
                bool c2=true;
                for(auto it2:stored_){
                    if(it3->getMsgId()==it2->getMsgId() && it3->getSender()==it2->getSender()){
                        c2=false;
                        break;
                    }
                }
                if(c2==true){
                    stored_.push_back(it3);
                }
            }
        }
        if(it->conflicting==true){
            for(auto it3:it->conflicting_value){
                bool c3=true;
                for(auto it2:conflicting_){
                    if((it3->getMsgId()==it2->getMsgId() && it3->getSender()==it2->getSender())){
                        c3=false;
                        break;
                    }
                }
                if(c3==true){
                    conflicting_.push_back(it3);
                }
            }

        }

    }

    EV << "Acknowledged" << selfId << "\n";
    for(auto it : acknowledged){
        EV << "[" << it->getSender() << ", "<<it->getMsgId()<<"],";
    }
    EV << "\n Stored" << selfId << "\n";
    for(auto it : stored_){
        EV << "[" << it->getSender() << ", "<<it->getMsgId()<<"],";
    }
    EV << "\n Conflicting" << selfId << "\n";
    for(auto it : conflicting_){
        EV << "[" << it->getSender() << ", "<<it->getMsgId()<<"],";
    }
    EV<<"\n";

    if(conflicting_.empty()==true && acknowledged.size()==1 && (allowed_ack==false || (allowed_ack==true && isAllowed(acknowledged[0])==true))){
        allowed_ack=true;
        if(isAllowed(acknowledged[0])==false){
            allowed_ack_value.push_back(acknowledged[0]);
        }
        if(state.ack==false || isAllowedS(acknowledged[0])==false){
            state.ack=true;
            state.ack_value.push_back(acknowledged[0]);
        }
    }
    else if(conflicting_.empty()==true && acknowledged.size()>1){
        allowed_ack=false;
        allowed_ack_value={};
        if(state.conflicting==false){
            state.conflicting=true;
            for(auto it:acknowledged){
                state.conflicting_value.push_back(it);
            }
        }
        state.ack=false;
        state.ack_value={};
    }
    else if(conflicting_.empty()==false){
        allowed_ack=false;
        allowed_ack_value={};
        if(state.conflicting==false){
            state.conflicting=true;
            for(auto it:conflicting_){
                state.conflicting_value.push_back(it);
            }
        }
        state.ack=false;
        state.ack_value={};
    }
    if(stored_.empty()==false){
        stored=true;
        for(auto it:stored_){
            if(isStored(it)==false){
                stored_value.push_back(it);
            }
            if(state.stored==false || isStoredS(it)==false){
                state.stored=true;
                state.stored_value.push_back(it);
            }
        }
    }
    for(auto it:stored_value){
        EV << it->getSender() << " , " << it->getMsgId() << "\n";
    }
}

void Node::insertInMsg(Msg * x){
    bool c=true;
    for(auto it:msg_to_send){
        if(equalMsg(it,x)==true){
            c=false;
            break;
        }
    }
    if(c==true){
        msg_to_send.push_back(x);
    }
}

void Node::commit_f(Msg * x){
    if(equalVec(x->getView(),current_view)){
        receivedMsg.push_back(x);
    //TODO verify CER
        //msg_to_send.push_back(x);
        //insertInMsg()
        if(stored==false || isStored(x)==false){
            stored=true;
            if(isStored(x)==false){
                stored_value.push_back(x);
            }
            //TODO update if bot
            if(state.stored==false || isStoredS(x)==false){
                state.stored=true;
                if(isStoredS(x)==false){
                    state.stored_value.push_back(x);
                }
            }
            for (int destId = 0; destId < nodesNbr; destId++) {
               if(destId==selfId) continue;
               if(contain(destId,current_view)==false) continue;
               Msg * bp = new Msg();
               bp->setId(selfId);
               bp->setMsg(x->getMsg());
               simtime_t t=simTime()+fRand(0.0, 1.0);
               bp->setSendTime(t);
               bp->setView(current_view);
               bp->setType(COMMIT);
               bp->setTypeS("COMMIT");
               bp->setVcer(v_cer);
               bp->setSender(x->getSender());
               bp->setMsgId(x->getMsgId());
               auto briefPacket = new Packet();
               Msg &tmp = *bp;
               const auto& payload = makeShared<Msg>(tmp);
               briefPacket->insertAtBack(payload);
               //EV << "\n\n\n\n [Client"<< selfId <<"]COMMIT messaggio inviato a: " << simTime() << "\n\n\n";
               //sendP(briefPacket, destId);
               MessageL * timer = new MessageL();
               timer->setDestId(destId);
               timer->setKind(MSGKIND_SEND);
               timer->setM(bp);
               //scheduleAt(simTime(),timer);
               scheduleAt(t,timer);
            }
        }
        Msg * bp = new Msg();
        bp->setId(selfId);
        bp->setMsg(x->getMsg());
        simtime_t t=simTime()+fRand(0.0, 1.0);
        bp->setSendTime(t);
        bp->setView(current_view);
        bp->setType(DELIVER);
        bp->setTypeS("DELIVER");
        bp->setSender(x->getSender());
        bp->setMsgId(x->getMsgId());
        auto briefPacket = new Packet();
        Msg &tmp = *bp;
        const auto& payload = makeShared<Msg>(tmp);
        briefPacket->insertAtBack(payload);
        //EV << "\n\n\n\n [Client"<< selfId <<"]DELIVER messaggio inviato a: " << simTime() << "\n\n\n";
        //sendP(briefPacket, x->getId());
        MessageL * timer = new MessageL();
        timer->setDestId(x->getId());
        timer->setKind(MSGKIND_SEND);
        timer->setM(bp);
        //scheduleAt(simTime(),timer);
        scheduleAt(t,timer);
    }
}

void Node::ack_f(Msg * x){
    if(contain(x->getId(),x->getView())==true){
        if(acksMsg(x, x->getId(), x->getView())==true){ //TODO verify signature
              //  EV << "DEBUG1";
            receivedMsg.push_back(x);
            addAcksMsg(x, x->getId(), x->getView()); //TODO wait for return!
            //TODO add sigma

        }
        for(auto it:acks){
            for(auto it2:it.second){
                for(auto it3:it2.second){
                    EV<<it3;
                }
            }
        }
        msg_to_ack={};
        if(checkAckMsg()==true){
            //Msg * m = returnMsg();
            for(auto it:msg_to_ack){
                Msg * m = it;
                ftaInsert(m);
                if(quorumMsg(m)==false){
                    v_cer = m->getView();
                      if(isInstalled(current_view)==true){
                          //TODO add cer
                          certificati.push_back(make_pair(m,v_cer));
                          for (int destId = 0; destId < nodesNbr; destId++) {
                            if(destId==selfId) continue;
                            if(contain(destId,current_view)==true){
                              Msg * bp = new Msg();
                              bp->setId(selfId);
                              bp->setMsg(m->getMsg());
                              bp->setMsgId(m->getMsgId());
                              simtime_t t=simTime()+fRand(0.0, 1.0);
                              bp->setSendTime(t);
                              bp->setType(COMMIT);
                              bp->setTypeS("COMMIT");
                              bp->setView(current_view);
                              bp->setSender(x->getSender());
                              auto briefPacket = new Packet();
                              Msg &tmp = *bp;
                              const auto& payload = makeShared<Msg>(tmp);
                              briefPacket->insertAtBack(payload);
                              //EV << "\n\n\n\n [Client"<< selfId <<"]COMMIT inviato a: " << simTime() << "\n\n\n";
                              //sendP(briefPacket, destId);
                              MessageL * timer = new MessageL();
                              timer->setDestId(destId);
                              timer->setKind(MSGKIND_SEND);
                              timer->setM(bp);
                              //scheduleAt(simTime(),timer);
                              scheduleAt(t,timer);
                            }
                          }
                      }
                 }
            }

        }
    }
}

void Node::deliver_f(Msg * x){
    if(contain(x->getId(),x->getView())==true){
        receivedMsg.push_back(x);
        addDeliverMsg(x,x->getId(),x->getView());
        msg_to_send={};
        if(checkDeliverMsg()==true){ //TODO mettere in un timer?
            for(auto it:msg_to_send){
                Msg * m = it;
                //first_time_deliver.push_back(m);
                ftdInsert(m);
                //delivered=true;
                EV << "\n\n\n\n [Client"<< selfId <<"]DELIVERED di " << m->getMsgId()  << "da parte di" << m->getSender() << " FINISHED a: " << simTime() << "\n\n\n";
                //can_leave=true;
                deliverati.push_back(m);
                removeMsgFromStored(m);
                removeMsgFromAck(m);
                removeMsgFromStoredS(m);
                removeMsgFromAckS(m);
                Msg * prova=new Msg();
                prova->setTypeS("NEW MESSAGE DELIVERED");
                prova->setView(current_view);
                prova->setSendTime(simTime());
                prova->setSender(m->getSender());
                prova->setMsgId(m->getMsgId());
                Msg * prova2=new Msg();
                prova2->setTypeS("NEW MESSAGE DELIVERED");
                prova2->setView(current_view);
                prova2->setSendTime(simTime());
                prova2->setSender(m->getSender());
                prova2->setMsgId(m->getMsgId());
                receivedMsg.push_back(prova);
                inviati.push_back(prova2);

            }
            can_leave=true;
        }
    }
}

void Node::removeMsgFromStored(Msg * m){
    vector<Msg*> temp={};
    for(auto it:stored_value){
        if(equalMsg(it,m)==true){
            continue;
        }else{
            temp.push_back(it);
        }
    }
    stored_value=temp;
    if(stored_value.empty()==true){
        stored=false;
    }
    return;
}

void Node::removeMsgFromAck(Msg * m){
    vector<Msg*> temp={};
    for(auto it:allowed_ack_value){
        if(equalMsg(it,m)==true){
            continue;
        }else{
            temp.push_back(it);
        }
    }
    allowed_ack_value=temp;
    if(allowed_ack_value.empty()==true){
        allowed_ack=false;
    }
    return;
}

void Node::removeMsgFromStoredS(Msg * m){
    for(auto it: state.stored_value){
        EV << "[" << it->getSender() << ", "<<it->getMsgId()<<"],";
    }
    vector<Msg*> temp={};
    for(auto it:state.stored_value){
        if(equalMsg(it,m)){
            continue;
        }else{
            temp.push_back(it);
        }
    }
    state.stored_value=temp;
    if(state.stored_value.empty()==true){
        state.stored=false;
    }
    for(auto it: state.stored_value){
        EV << "[" << it->getSender() << ", "<<it->getMsgId()<<"],";
    }
    return;
}

void Node::removeMsgFromAckS(Msg * m){
    vector<Msg*> temp={};
    for(auto it:state.ack_value){
        if(equalMsg(it,m)){
            continue;
        }else{
            temp.push_back(it);
        }
    }
    state.ack_value=temp;
    if(state.ack_value.empty()==true){
        state.ack=false;
    }
    return;
}

void Node::handleStartOperation(LifecycleOperation *operation) {
    EV << "\n\n\n HandleStartTime: " << startTime << "\n\n\n";

    //numMsgToSend=rand()%10+1;

    nodesNbr = par("nPar");
    f = par("fPar");
    for (int nodeId = 0; nodeId < nodesNbr; nodeId++) {
       socketV[nodeId].first=0;
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

    if(selfId==0 || selfId==1 || selfId==2){
        current_view = {{0,1}, {1,1}, {2,1}}; //fixed current view
        installedView={{current_view,1}};
        p=true;
    }else{
        init_view={{0,1}, {1,1}, {2,1}};
        init_view_={{{0,1}, {1,1}, {2,1}}};
        current_view={}; //fixed current view
        installedView={};
        p=false;
    }

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
    //allowed_ack={};
    acks={};
    msgId=0;
    deliver={};
    quorum_msg={};
    first_time_deliver={};
    states_update={};
    states={};
    certificati={};
    msg_prepare={};
    states_update_rec={};
    msg4view={};
    inviati={};

    state={false,{},false,{},false,{}};

    double m=par("join");
    if(m!=-1){
        rec_cond=false;
        join_complete=false;
        join(m);
    }

    double m2=par("broadcast");
    if(m2!=-1){
        broadcast(m2);
    }

    double m3=par("timer");
    timerDelay=m3;
    timer(timerDelay);


}

} // namespace inet
