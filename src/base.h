#ifndef BASE_H
#define BASE_H

// Message types
const int RECONFIG = 0;
const int REC_CONFIRM = 1;
const int PROPOSE = 2;
const int CONVERGED = 3;
const int ACK = 4;
const int PREPARE = 5;
const int COMMIT = 6;
const int INSTALL = 7;
const int DELIVER = 8;
const int STATE_UPDATE = 9;
const int CHANGE_VIEW = 10;

const int PARTECIPANT = 0;
const int NON_PARTECIPANT = 1;
const int WAITING = 2;

const int N=5;

const int MSGKIND_JOIN=1;
const int MSGKIND_LEAVE=2;
const int MSGKIND_BROADCAST=3;
const int MSGKIND_SEND=4;
const int MSGKIND_TIMER=5;


#endif
