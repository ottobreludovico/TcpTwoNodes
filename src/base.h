#ifndef BASE_H
#define BASE_H

// Message types
const int SEND = 0;
const int ECHO = 1;
const int READY = 2;
const int READY_ECHO = 3; // A Ready with an embedded Echo
const int ECHO_ECHO = 4; // An Echo with an embedded Echo
const int PAYLOAD_ACK = 5;

/*** Size of log components in bits ***/

const double MSG_TYPE_SIZE = 3.0;
const double PAYLOAD_PRESENCE_BIT = 1.0;
const double MSG_ID_BIT = 1.0;
const double LOCAL_ID_BIT = 1.0;
const double SELF_MSG_BIT = 1.0;

const double PROCESS_ID_SIZE = 10.0; // 2^10 = 1024
const double MSG_ID_SIZE = 32.0;

const double LOCAL_ID_SIZE = 32.0; // decided between two nodes

const double PATH_LENGTH_SIZE = 10.0; // At most the path contains all processes
// + PATH

const double PAYLOAD_SIZE = 8.0; // in bits, to indicate the number of bytes that the payload contains


#endif
