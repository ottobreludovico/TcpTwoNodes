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
#include "messageL_m.h"
//#include "StateUpdateMessage_m.h"
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

struct StateUpdate{
    bool ack;
    vector<Msg *> ack_value;

    bool conflicting;
    vector<Msg*> conflicting_value;

    bool stored;
    vector<Msg *> stored_value;
};

class Node : public ApplicationBase, public TcpSocket::ICallback
{
  public:
    static map<L3Address, int> addrToId;
    cMessage **processLinkTimers = nullptr; // An array of timers, one per possible destination (instantiated only for the neighbors)
    vector< pair<Packet *, SimTime> > *pendingMsgsPerNeighbors = nullptr;
    vector<pair<vector<vector<pair<int,int>>>,vector<int>>> propose;
    vector<pair<vector<vector<pair<int,int>>>,vector<int>>> proposeReceived;
    vector<pair<vector<vector<pair<int,int>>>,vector<int>>> converge;
    vector<pair<vector<vector<pair<int,int>>>,vector<int>>> convergeReceived;
    vector<pair<vector<pair<int,int>>,vector<StateUpdate*>>> statesReceived;

    vector<vector<pair<int,int>>> installReceived;

    vector<pair<vector<pair<int,int>>,int>> installedView;
    vector<Msg*> mio;

  protected:
    TcpSocket socketL;
    SocketMap socketMap;
    pair<int,TcpSocket *> socketV[5];

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
    int f;
    bool p;
    int localPort;
    int roundId;
    cMessage *timerEvent = nullptr;
    int selfId;
    int nodesNbr;
    vector<L3Address> nodesAddr;
    simtime_t startTime;

    vector<Msg*> receivedMsg;

    int numMsgToSend;


    //DBRB
    int status;
    int quorumSize;
    vector<pair<int,int>> current_view;
    vector<pair<int,int>> init_view;
    vector<vector<pair<int,int>>> init_view_;
    vector<pair<int,int>> RECV;
    vector< vector<pair<int,int>> > SEQv;
    vector< vector<pair<int,int>> > LCSEQv;
    vector< vector<pair<int,int>> > FORMATv;

    int cer;
    vector<pair<int,int>> v_cer;
    bool allowed_ack=false;
    vector<Msg *> allowed_ack_value;
    bool stored = false;
    vector<Msg *> stored_value;
    bool can_leave = false;
    vector<pair<Msg *,bool>> delivered;
    vector<pair<vector<pair<int,int>>,vector<Msg*>>> first_time_deliver;
    vector<pair<vector<pair<int,int>>,vector<Msg*>>> first_time_ack;

    vector<pair<vector<pair<int,int>>,vector<pair<Msg*,vector<int>>>>> acks;
    vector<pair<vector<pair<int,int>>,vector<pair<Msg*,vector<int>>>>> deliver;
    int ** sigma; //bool?
    vector<Msg*> msg_to_send;
    vector<Msg*> msg_to_ack;
    vector<Msg*> quorum_msg;
    vector<Msg*> msg4view;

    bool join_complete;
    bool leave_complete;
    bool rec_cond;

    vector<int> req_rec;

    vector<int> req_join_or_leave;
    int count_req_join_or_leave;

    bool stop_processing=false;

    int msgId;

    bool first_time=true;

    vector<Msg*> deliverati;

    vector<Msg*> inviati;

    vector<Msg*> prepare_to_send;

    vector<Msg*> commit_to_send;

    vector<StateUpdateMessage*> states_update;

    vector<pair<int,int>> req;

    vector<StateUpdate*> states;

    vector<vector<pair<int,int>>> first_time_state_update;

    StateUpdate state;

    double timerDelay;

    vector<pair<Msg *,vector<pair<int,int>>>> certificati;

    vector<Msg* > msg_prepare;

    vector<StateUpdateMessage*> states_update_rec;

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

    virtual void handleTimer(cMessage *msg);

    /* TcpSocket::ICallback callback methods */
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
    virtual void socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo) override;
    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketPeerClosed(TcpSocket *socket) override;
    virtual void socketClosed(TcpSocket *socket) override;
    virtual void socketFailure(TcpSocket *socket, int code) override;
    virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) override { }
    virtual void socketDeleted(TcpSocket *socket) override {}

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    //DBRB
    virtual void join(double x);
    virtual bool contain(int x, vector<pair<int,int>> cv);
    virtual vector<pair<int,int>> mostRecent(vector<vector<pair<int,int>>> seq);
    virtual vector<pair<int,int>> leastRecent(vector<vector<pair<int,int>>> seq);
    virtual bool isContainedIn(vector<vector<pair<int,int>>> s1, vector<vector<pair<int,int>>> s2 );

    virtual void updatePropose(vector<vector<pair<int,int>>> s1,int i);
    virtual void updateConverge(vector<vector<pair<int,int>>> s1,int i);

    virtual bool checkPropose(int x, int join_or_leave, vector<pair<int,int>> cv);

    virtual bool checkPropose() ;
    virtual vector<vector<pair<int,int>>>returnPropose() ;

    virtual bool checkConverge() ;
    virtual vector<vector<pair<int,int>>>returnConverge() ;

    virtual bool conflictS(vector<vector<pair<int,int>>> s1, vector<vector<pair<int,int>>> s2);
    virtual bool conflictV(vector<pair<int,int>> v1, vector<pair<int,int>> v2);
    virtual bool isReceivedC(vector<vector<pair<int,int>>> v,int id);

    virtual bool isReceivedP(vector<vector<pair<int,int>>> v,int id);

    virtual vector<vector<pair<int,int>>> unionS(vector<vector<pair<int,int>>> s1, vector<vector<pair<int,int>>> s2);

    virtual vector<pair<int,int>> merge(vector<pair<int,int>> v1, vector<pair<int,int>> v2);
    virtual int sizeCV(vector<pair<int,int>> v1);
    virtual void uponRECV();
    virtual bool equalVec(vector<pair<int,int>>s1, vector<pair<int,int>> s2);
    virtual bool equalSeq(vector<vector<pair<int,int>>>s1, vector<vector<pair<int,int>>> s2);

    virtual bool isMember(int id, vector<pair<int,int>> v);

    virtual bool isReceivedI(vector<pair<int,int>> v);
    virtual bool isInstalled(vector<pair<int,int>> v1);
    virtual void deInstall(vector<pair<int,int>> v1);
    virtual void install(vector<pair<int,int>> v1);
    virtual bool contains(vector<pair<int,int>> v1, vector<pair<int,int>> v2);
    //virtual bool isAllowed(Msg * m );
    virtual bool acksMsg(Msg* m, int id, vector<pair<int,int>> v);
    virtual void addAcksMsg(Msg* m, int id, vector<pair<int,int>> v);
    virtual bool equalMsg(Msg* m1, Msg* m2);
    virtual Msg * returnMsg();
    virtual bool checkAckMsg();
    virtual bool firstTimeDeliver(Msg* m);

    virtual void addDeliverMsg(Msg* m, int id, vector<pair<int,int>> v);
    virtual Msg * returnDeliverMsg();
    virtual bool checkDeliverMsg();

    virtual void broadcast(double x);
    virtual bool quorumMsg(Msg *m);

    virtual void reconfig_f(Msg * x);
    virtual void rec_confirm_f(Msg * x);
    virtual void propose_f(Msg * x);
    virtual void converge_f(Msg * x);
    virtual void install_f(Msg * x);
    virtual void state_update_f(StateUpdateMessage * x);
    virtual void commit_f(Msg * x);
    virtual void ack_f(Msg * x);
    virtual void deliver_f(Msg * x);
    virtual void prepare_f(Msg * x);

    virtual void newView();
    virtual void stateTransfer();
    virtual msg_ castMsgToMsg_(Msg* m);

    virtual void timer(double x);
    virtual double fRand(double fMin, double fMax);

    virtual bool isAllowed(Msg * m);

    virtual bool isStored(Msg * m);

    virtual bool isAllowedS(Msg * m);

    virtual bool isStoredS(Msg * m);

    virtual bool isDelivered(Msg * m);

    virtual void view_discovery();

    virtual void insertInMsg(Msg * x);

    virtual bool firstTimeAck(Msg * m);

    virtual void ftdInsert(Msg* m);

    virtual void ftaInsert(Msg* m);

    virtual void removeMsgFromStored(Msg * m);

    virtual void removeMsgFromAck(Msg * m);

    virtual void removeMsgFromStoredS(Msg * m);

    virtual void removeMsgFromAckS(Msg * m);

    virtual void changeCV(vector<pair<int,int>> v1);

    virtual int sizeS(vector<vector<pair<int,int>>> s);

    virtual bool containVS(vector<pair<int,int>> cv,vector<vector<pair<int,int>>> seq);

};

} // namespace inet

#endif
