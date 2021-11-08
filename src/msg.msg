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

//
// TODO generated message class
//

import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.contract.ipv4.Ipv4Address;

cplusplus {{
#include <vector>
typedef std::vector<std::pair<int,int>> IntVector;
typedef std::vector<std::vector<std::pair<int,int>>> IntVectorV;
typedef struct msg_{
	int id; //sender
    std::string msg; //message  
    std::vector<std::pair<int,int>> view; //current view or view     
    std::vector<std::pair<int,int>> installView; //current view or view     
    std::vector<std::vector<std::pair<int,int>>> SEQcv; //in case of PROPOSE
    int join_or_leave; //in case of RECONFIG (1 -> join, 0 -> leave)
    int type; //type of messagge (PROPOSE, REC-CONFIRM, RECONFIG, ecc.)
    int cer; //message certificate in case of COMMIT or ACK
    std::vector<std::pair<int,int>> Vcer; //view in which certificate is collected in case of COMMIT
    int msgId;
    int install_or_update; //in case of INSTALL (1 -> install, 0 -> update)
    std::string typeS;
    int sender;
};
}}

class IntVector {
    @existingClass;
}

class IntVectorV {
    @existingClass;
}

class msg_ {
    @existingClass;
}

namespace inet;
    
class Msg extends FieldsChunk {
    
    chunkLength = B(1);
    
    int id; //sender
    
    string msg; //message
    
    IntVector view; //current view or view 
    
    IntVector installView; //current view or view 
    
    IntVectorV SEQcv; //in case of PROPOSE
    
    int join_or_leave; //in case of RECONFIG (1 -> join, 0 -> leave)
    
    int type; //type of messagge (PROPOSE, REC-CONFIRM, RECONFIG, ecc.)
    
    int cer; //message certificate in case of COMMIT or ACK
    
    IntVector Vcer; //view in which certificate is collected in case of COMMIT
    
    simtime_t sendTime; //send time
    
    simtime_t arrivalTime; //arrival time
    
    int msgId;
    
    int install_or_update; //in case of INSTALL (1 -> install, 0 -> update)
    
    string typeS;
    
    int sender;
    
    bool allowed_ack; //current view or view 
    
    msg_ allowed_ack_value;
    
    bool stored;
    
    msg_ stored_value;

}
