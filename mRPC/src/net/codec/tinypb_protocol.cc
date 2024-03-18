#include "utils.h"
#include "tinypb_protocol.h"

MRPC_NAMESPACE_BEGIN

char TinyPBProtocol::PB_START = 0x02;
char TinyPBProtocol::PB_END = 0x03;

MRPC_NAMESPACE_END