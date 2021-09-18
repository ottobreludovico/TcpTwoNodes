//
// Generated file, do not edit! Created by nedtool 5.6 from packets.msg.
//

#ifndef __INET_PACKETS_M_H
#define __INET_PACKETS_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif


namespace inet {

class BriefPacket;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk

#include "inet/networklayer/contract/ipv4/Ipv4Address_m.h" // import inet.networklayer.contract.ipv4.Ipv4Address


namespace inet {

/**
 * Class generated from <tt>packets.msg:7</tt> by nedtool.
 * <pre>
 * class BriefPacket extends FieldsChunk
 * {
 *     // Determines the size of the packet on the links 
 *     // !! To be manually set after the message has been created, depending on the fields that are used !!
 *     chunkLength = B(1);
 * 
 *     int msgType; // For Bracha's algorithm (SEND, ECHO or READY)	
 * 
 *     // Identifiers of a broadcast message
 *     int broadcasterId; // Source of the message
 *     int msgId; // Msg ID determined by the source
 * 
 *     int echoOrReadySender;
 * 
 *     int path[];  // For Dolev's algorithm
 * 
 *     int linkSenderId; // Always used to determine who sent a message (in theory should be done by the authenticated link)
 * 
 *     int ackedEchoForREADY_ECHO; // Only for the Ready2 messages
 * 
 *     bool includeData;
 * 
 *     int dataSize;
 *     char data[];
 * 
 *     int debugId;
 * }
 * 
 * //class SendPacket extends FieldsChunk {
 * //    chunkLength = B(1);
 * //    int linkSenderId; // Used to determine who sent a msg, for ease (in theory should be done by the authenticated link)
 * //    
 * //	int msgType; // The msg type (SEND). Could be removed and only taken into account for the chunkLength to relieve the simulator a little bit. 
 * //	
 * ////	int broadcasterId; // Id of the broadcasting process (not needed with Send messages)
 * //	int msgId; // Local counter 
 * //	
 * ////	int localPayloadId;  // One ID per (broadcasterId, msgId) broadcast // Used only for network consumption
 * ////	int payloadSize; // Number of bytes the payload contains // Used only for network consumption
 * ////	byte payload[]; // Voluntarily commented: chunkLength manually set in the code
 * //}
 * </pre>
 */
class BriefPacket : public ::inet::FieldsChunk
{
  protected:
    int msgType = 0;
    int broadcasterId = 0;
    int msgId = 0;
    int echoOrReadySender = 0;
    int *path = nullptr;
    size_t path_arraysize = 0;
    int linkSenderId = 0;
    int ackedEchoForREADY_ECHO = 0;
    bool includeData = false;
    int dataSize = 0;
    char *data = nullptr;
    size_t data_arraysize = 0;
    int debugId = 0;

  private:
    void copy(const BriefPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const BriefPacket&);

  public:
    BriefPacket();
    BriefPacket(const BriefPacket& other);
    virtual ~BriefPacket();
    BriefPacket& operator=(const BriefPacket& other);
    virtual BriefPacket *dup() const override {return new BriefPacket(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getMsgType() const;
    virtual void setMsgType(int msgType);
    virtual int getBroadcasterId() const;
    virtual void setBroadcasterId(int broadcasterId);
    virtual int getMsgId() const;
    virtual void setMsgId(int msgId);
    virtual int getEchoOrReadySender() const;
    virtual void setEchoOrReadySender(int echoOrReadySender);
    virtual void setPathArraySize(size_t size);
    virtual size_t getPathArraySize() const;
    virtual int getPath(size_t k) const;
    virtual void setPath(size_t k, int path);
    virtual void insertPath(int path);
    virtual void insertPath(size_t k, int path);
    virtual void erasePath(size_t k);
    virtual int getLinkSenderId() const;
    virtual void setLinkSenderId(int linkSenderId);
    virtual int getAckedEchoForREADY_ECHO() const;
    virtual void setAckedEchoForREADY_ECHO(int ackedEchoForREADY_ECHO);
    virtual bool getIncludeData() const;
    virtual void setIncludeData(bool includeData);
    virtual int getDataSize() const;
    virtual void setDataSize(int dataSize);
    virtual void setDataArraySize(size_t size);
    virtual size_t getDataArraySize() const;
    virtual char getData(size_t k) const;
    virtual void setData(size_t k, char data);
    virtual void insertData(char data);
    virtual void insertData(size_t k, char data);
    virtual void eraseData(size_t k);
    virtual int getDebugId() const;
    virtual void setDebugId(int debugId);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const BriefPacket& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, BriefPacket& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_PACKETS_M_H

