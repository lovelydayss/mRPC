#ifndef MRPC_NET_PROTOCOL_ABSTRACT_PROTOCOL_H
#define MRPC_NET_PROTOCOL_ABSTRACT_PROTOCOL_H

#include "utils.h"

MRPC_NAMESPACE_BEGIN

class AbstractProtocol {
  public:
    typedef std::shared_ptr<AbstractProtocol> s_ptr;
};

MRPC_NAMESPACE_END

#endif