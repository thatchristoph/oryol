#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::Message
    
    Base class for messages.
*/
#include "Core/RefCounted.h"

namespace Oryol {
namespace Messaging {

/// the type for message ids
typedef int32 MessageIdType;
static const MessageIdType InvalidMessageId = -1;

class Message : public Core::RefCounted {
    OryolClassDecl(Message);
public:
    /// constructor
    Message();
    /// destructor
    virtual ~Message();
    
    /// get the message id
    MessageIdType MessageId() const;
    /// return true if the message has been handled
    bool Handled() const;
    
    /// get the encoded size of the message
    virtual int32 EncodedSize() const;
    /// encode the message to raw memory, maxBytes must be at least EncodedSize()
    virtual uint8* Encode(uint8* dstPtr, const uint8* maxValidPtr) const;
    /// decode the message from raw memory
    virtual const uint8* Decode(const uint8* srcPtr, const uint8* maxValidPtr);

protected:
    MessageIdType msgId;
    bool handled;
};

//------------------------------------------------------------------------------
inline Message::Message() :
msgId(InvalidMessageId),
handled(false) {
    // empty
}

//------------------------------------------------------------------------------
inline MessageIdType
Message::MessageId() const {
    return this->msgId;
}

//------------------------------------------------------------------------------
inline bool
Message::Handled() const {
    return this->handled;
}

} // namespace Messaging
} // namespace Oryol