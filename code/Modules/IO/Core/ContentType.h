#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ContentType
    @ingroup IO
    @brief a stream content type, aka MIME type, aka MediaType
    @see Stream
 
    ContentType describes the data type contained in a Stream. Some 
    filesystem implementations can make use of this (for instance the
    HTTPFileSystem will use this in the Content-Type request/response
    header fields).
*/
#include "Core/Types.h"
#include "Core/String/StringAtom.h"
#include "Core/String/String.h"
#include "Core/Containers/Map.h"

namespace Oryol {
    
class ContentType {
public:
    /// default constructor
    ContentType();
    /// copy constructor
    ContentType(const ContentType& rhs);
    /// move constructor
    ContentType(ContentType&& rhs);
    /// construct from raw string
    ContentType(const char* rhs);
    /// construct from StringAtom
    ContentType(const StringAtom& rhs);
    /// construct from String
    ContentType(const String& rhs);
    
    /// copy-assignment
    void operator=(const ContentType& rhs);
    /// move-assignment
    void operator=(ContentType&& rhs);
    /// assign from raw string
    void operator=(const char* rhs);
    /// assign from StringAtom
    void operator=(const StringAtom& rhs);
    /// assign from String
    void operator=(const String& rhs);
    
    /// equality
    bool operator==(const ContentType& rhs) const;
    /// inequality
    bool operator!=(const ContentType& rhs) const;
    
    /// true if content-type string is valid
    bool IsValid() const;
    /// true if nothing assigned
    bool Empty() const;
    /// get the content-type string
    const StringAtom& Get() const;
    /// get as C string
    const char* AsCStr() const;
    
    /// check if a top-level type is present
    bool HasType() const;
    /// get the top-level media-type (e.g. text, audio, image, ...)
    String Type() const;
    /// check if subtype is present
    bool HasSubType() const;
    /// get the subtype
    String SubType() const;
    /// get the type and subtype string (but not params)
    String TypeAndSubType() const;
    /// check if the media-type has parameters
    bool HasParams() const;
    /// get the parameters
    Map<String,String> Params() const;
    
private:
    /// crack the media-type string into its components
    void crack();
    /// clear the string indices
    void clearIndices();
    /// copy the string indices
    void copyIndices(const ContentType& rhs);
    
    enum {
        typeStart = 0,
        typeEnd,
        subTypeStart,
        subTypeEnd,
        paramStart,
        paramEnd,
        
        numIndices
    };
    
    StringAtom content;
    int16 indices[numIndices];
    bool valid;
};
    
} // namespace Oryol