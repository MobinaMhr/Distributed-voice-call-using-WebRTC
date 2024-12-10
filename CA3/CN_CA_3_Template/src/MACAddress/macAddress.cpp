#include "macAddress.h"

MacAddress::MacAddress(uint64_t address) : m_address(address){
    if (!validate())//TODO: add loging;
        throw std::invalid_argument("Invalid MAC address");
}
