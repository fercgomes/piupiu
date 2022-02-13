#include <stdint.h>

#define PACKET_TYPE_DATA 0
#define PACKET_TYPE_CMD 1
#define PACKET_MAX_PAYLOAD_LEN 512

namespace Message
{
typedef struct
{
    uint16_t   type;
    uint16_t   seqn;
    uint16_t   length; // payload length
    uint64_t   timestamp;
    const char payload[PACKET_MAX_PAYLOAD_LEN];
} Packet;
}; // namespace Message