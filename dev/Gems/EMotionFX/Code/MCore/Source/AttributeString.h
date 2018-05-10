/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

// include the required headers
#include "StandardHeaders.h"
#include "Attribute.h"

namespace MCore
{
    /**
     * The string attribute class.
     * This attribute represents one string.
     */
    class MCORE_API AttributeString
        : public Attribute
    {
        friend class AttributeFactory;
    public:
        enum
        {
            TYPE_ID = 0x00000003
        };

        static AttributeString* Create(const AZStd::string& value);
        static AttributeString* Create(const char* value = "");

        MCORE_INLINE uint8* GetRawDataPointer()                     { return reinterpret_cast<uint8*>(mValue.data()); }
        MCORE_INLINE uint32 GetRawDataSize() const                  { return static_cast<uint32>(mValue.size()); }
        bool GetSupportsRawDataPointer() const override             { return true; }

        // adjust values
        MCORE_INLINE const char* AsChar() const                     { return mValue.c_str(); }
        MCORE_INLINE const AZStd::string& GetValue() const          { return mValue; }
        MCORE_INLINE void SetValue(const AZStd::string& value)      { mValue = value; }

        // overloaded from the attribute base class
        Attribute* Clone() const override                           { return AttributeString::Create(mValue); }
        Attribute* CreateInstance(void* destMemory) override        { return new(destMemory) AttributeString(); }
        const char* GetTypeString() const override                  { return "AttributeString"; }
        bool InitFrom(const Attribute* other) override
        {
            if (other->GetType() != TYPE_ID)
            {
                return false;
            }
            mValue = static_cast<const AttributeString*>(other)->GetValue();
            return true;
        }
        bool InitFromString(const AZStd::string& valueString) override     { mValue = valueString; return true; }
        bool ConvertToString(AZStd::string& outString) const override      { outString = mValue; return true; }
        uint32 GetClassSize() const override                        { return sizeof(AttributeString); }
        uint32 GetDefaultInterfaceType() const override             { return ATTRIBUTE_INTERFACETYPE_STRING; }

    private:
        AZStd::string   mValue;     /**< The string value. */

        AttributeString()
            : Attribute(TYPE_ID)  { }
        AttributeString(const AZStd::string& value)
            : Attribute(TYPE_ID)
            , mValue(value) { }
        AttributeString(const char* value)
            : Attribute(TYPE_ID)
            , mValue(value) { }
        ~AttributeString()                              { mValue.clear(); }

        uint32 GetDataSize() const override
        {
            return sizeof(uint32) + static_cast<uint32>(mValue.size());
        }

        // read from a stream
        bool ReadData(MCore::Stream* stream, MCore::Endian::EEndianType streamEndianType, uint8 version) override
        {
            MCORE_UNUSED(version);

            // read the number of characters
            uint32 numCharacters;
            if (stream->Read(&numCharacters, sizeof(uint32)) == 0)
            {
                return false;
            }

            // convert endian
            Endian::ConvertUnsignedInt32(&numCharacters, streamEndianType);
            if (numCharacters == 0)
            {
                mValue.clear();
                return true;
            }

            mValue.resize(numCharacters);
            if (stream->Read(mValue.data(), numCharacters) == 0)
            {
                return false;
            }

            return true;
        }

        // write to a stream
        bool WriteData(MCore::Stream* stream, MCore::Endian::EEndianType targetEndianType) const override
        {
            // write the number of characters
            if (mValue.size() > 0)
            {
                uint32 numCharacters = static_cast<uint32>(mValue.size());
                Endian::ConvertUnsignedInt32To(&numCharacters, targetEndianType);
                if (stream->Write(&numCharacters, sizeof(uint32)) == 0)
                {
                    return false;
                }

                if (stream->Write(mValue.data(), mValue.size()) == 0)
                {
                    return false;
                }
            }
            else
            {
                uint32 numCharacters = 0;
                Endian::ConvertUnsignedInt32To(&numCharacters, targetEndianType);
                if (stream->Write(&numCharacters, sizeof(uint32)) == 0)
                {
                    return false;
                }
            }

            return true;
        }
    };
}   // namespace MCore
