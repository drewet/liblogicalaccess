#include "desfirekey.hpp"
#include "logicalaccess/logs.hpp"
#include "omnitechkeydiversification.hpp"
#include "boost/shared_ptr.hpp"
#include "logicalaccess/bufferhelper.hpp"
#include "desfirecrypto.hpp"
#include <vector>

namespace logicalaccess
{
    void OmnitechKeyDiversification::initDiversification(std::vector<unsigned char> identifier, int /*AID*/, std::shared_ptr<Key> /*key*/, unsigned char /*keyno*/, std::vector<unsigned char>& diversify)
    {
        if (identifier.size() > 0)
        {
            diversify.resize(2 * identifier.size() + 2);
            diversify[0] = 0xFF;
            memcpy(&diversify[1], &identifier[0], identifier.size());
            diversify[8] = 0x00;
            memcpy(&diversify[9], &identifier[0], identifier.size());
            for (unsigned char i = 0; i < 7; ++i)
            {
                diversify[9 + i] ^= 0xFF;
            }
        }
        else
        {
            THROW_EXCEPTION_WITH_LOG(LibLogicalAccessException, "Chip identifier is required for key diversification.");
        }
    }

    std::vector<unsigned char> OmnitechKeyDiversification::getDiversifiedKey(std::shared_ptr<Key> key, std::vector<unsigned char> diversify)
    {
        LOG(LogLevel::INFOS) << "Using key diversification Omnitech with div : " << BufferHelper::getHex(diversify);
        std::vector<unsigned char> keydiv;

        std::shared_ptr<DESFireKey> desfirekey = std::dynamic_pointer_cast<DESFireKey>(key);

        std::vector<unsigned char> iv;
        // Two time, to have ECB and not CBC mode
        std::vector<unsigned char> vkeydata;
        if (desfirekey->isEmpty())
        {
            vkeydata.resize(desfirekey->getLength(), 0x00);
        }
        else
        {
            vkeydata.insert(vkeydata.end(), desfirekey->getData(), desfirekey->getData() + desfirekey->getLength());
        }

        std::vector<unsigned char> r = DESFireCrypto::sam_CBC_send(vkeydata, iv, std::vector<unsigned char>(diversify.begin(), diversify.begin() + 8));
        std::vector<unsigned char> r2 = DESFireCrypto::sam_CBC_send(vkeydata, iv, std::vector<unsigned char>(diversify.begin() + 8, diversify.begin() + 16));

		for (unsigned char i = 0; i < 8; ++i)
		{
			r[7 - i] = static_cast<unsigned char>((r[7 - i] & 0xFE) | ((desfirekey->getKeyVersion() >> i) & 0x01));
			r2[i] = static_cast<unsigned char>(r2[i] & 0xFE);
		}

		keydiv.insert(keydiv.end(), r.begin(), r.end());
        keydiv.insert(keydiv.end(), r2.begin(), r2.end());
        return keydiv;
    }

    void OmnitechKeyDiversification::serialize(boost::property_tree::ptree& parentNode)
    {
        boost::property_tree::ptree node;
        parentNode.add_child(getDefaultXmlNodeName(), node);
    }

    void OmnitechKeyDiversification::unSerialize(boost::property_tree::ptree& /*node*/)
    {
    }
}