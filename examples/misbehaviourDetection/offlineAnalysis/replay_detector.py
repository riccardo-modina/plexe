# Data replay detector used to help the aiMDS and the ruleMDS to detect replay attacks that happens too fast for the other components to catch them.
# It detects replay attacks by checking if a message with the same posx, posy, spdx, spdy, acl, hed has already been recived from the same vehicle.
# If so, it is a replay attack

class DataReplayDetector:

    def __init__(self, window_time=5.0):
        self.window_time = window_time
        # The overflow bucket uses the payload's hash as a key
        # { receiver_veh: { (posx, posy, spdx, spdy, acl, hed): { 'sender': sender, 'rcvTime': rcvTime } } }
        self.overflow_buckets = {}

    def evaluate(self, msg, rxVehID):
       
        # msg: contains the fields: 'posx', 'posy', 'spdx', 'spdy', 'acl', 'hed', 'sender', 'rcvTime'
        # rxVehID: reciver vehicle ID
        
        # return true if a replay attack is detected

        if rxVehID not in self.overflow_buckets:
            self.overflow_buckets[rxVehID] = {}
        
        overflow_bucket = self.overflow_buckets[rxVehID]
        current_time = msg['rcvTime']
        
        # Clean the overflow bucket of this receiver from elements older than window_time
        self.overflow_buckets[rxVehID] = {k: v for k, v in overflow_bucket.items() if (current_time - v['rcvTime']) <= self.window_time}
        overflow_bucket = self.overflow_buckets[rxVehID]
        
        # Implicit Hash that will be used as key:
        # Python's dict uses this tuple as a hash key internally (via hash(tuple)),
        # so no explicit hash() call is needed
        key = (msg['posx'], msg['posy'], msg['spdx'], msg['spdy'], msg['acl'], msg['hed'])
        
        if key in overflow_bucket:
            return True
        else:
            overflow_bucket[key] = {
                'sender': msg['sender'],
                'rcvTime': msg['rcvTime']
            }
        return False
