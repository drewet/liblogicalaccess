/**
 * \file aes128key.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief AES128 Key.
 */

#include "logicalaccess/cards/aes128key.hpp"

namespace logicalaccess
{
    AES128Key::AES128Key()
        : Key()
    {
        memset(d_key, 0x00, sizeof(d_key));
        clear();
    }

    AES128Key::AES128Key(const std::string& str)
        : Key()
    {
        memset(d_key, 0x00, sizeof(d_key));
        fromString(str);
    }

    AES128Key::AES128Key(const void* buf, size_t buflen)
        : Key()
    {
        clear();

        if (buf != NULL)
        {
            if (buflen >= AES128_KEY_SIZE)
            {
                memcpy(d_key, buf, AES128_KEY_SIZE);
                d_isEmpty = false;
            }
        }
    }

    void AES128Key::serialize(boost::property_tree::ptree& parentNode)
    {
        boost::property_tree::ptree node;
        Key::serialize(node);
        parentNode.add_child(getDefaultXmlNodeName(), node);
    }

    void AES128Key::unSerialize(boost::property_tree::ptree& node)
    {
        Key::unSerialize(node);
    }

    std::string AES128Key::getDefaultXmlNodeName() const
    {
        return "AES128Key";
    }
}