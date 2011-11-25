/*
* ============================================================================
*
* Copyright [2010] maidsafe.net limited
*
* Description:  Class for manipulating database of system packets
* Version:      1.0
* Created:      14/10/2010 11:43:59
* Revision:     none
* Company:      maidsafe.net limited
*
* The following source code is property of maidsafe.net limited and is not
* meant for external use.  The use of this code is governed by the license
* file LICENSE.TXT found in the root of this directory and also on
* www.maidsafe.net.
*
* You are not free to copy, amend or otherwise use this source code without
* the explicit written permission of the board of directors of maidsafe.net.
*
* ============================================================================
*/

#ifndef MAIDSAFE_PASSPORT_SYSTEM_PACKET_HANDLER_H_
#define MAIDSAFE_PASSPORT_SYSTEM_PACKET_HANDLER_H_

#include <memory>
#include <map>
#include <string>
#include <vector>

#include "boost/thread/mutex.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"

#include "maidsafe/common/utils.h"

#include "maidsafe/passport/system_packets.h"
#include "maidsafe/passport/version.h"

#if MAIDSAFE_PASSPORT_VERSION != 109
#  error This API is not compatible with the installed library.\
    Please update the maidsafe-passport library.
#endif


namespace maidsafe {

namespace passport {

namespace test {
class SystemPacketHandlerTest;
class SystemPacketHandlerTest_FUNC_SigningAndIdentityPackets_Test;
}  // namespace test

class SystemPacketHandler {
 public:
  SystemPacketHandler();
  ~SystemPacketHandler();

  // Identity & signature packets
  bool AddPendingPacket(PacketPtr packet);
  int ConfirmPacket(PacketPtr packet);
  bool RevertPacket(const PacketType &packet_type);
  PacketPtr GetPacket(const PacketType &packet_type, bool confirmed) const;
  PacketPtr GetPacket(const std::string &packet_id, bool confirmed) const;
  int DeletePacket(const PacketType &packet_type);
  bool Confirmed(const PacketType &packet_type) const;

  // Selectable identities
  int AddPendingSelectableIdentity(const std::string &chosen_identity,
                                   SignaturePacketPtr identity,
                                   SignaturePacketPtr signer);
  int ConfirmSelectableIdentity(const std::string &chosen_identity);
  int DeleteSelectableIdentity(const std::string &chosen_identity);
  void SelectableIdentitiesList(std::vector<std::string> *selectables) const;

  // Whole keyring
  void SerialiseKeyChain(std::string *key_chain,
                         std::string *selectables) const;
  int ParseKeyChain(const std::string &serialised_keychain,
                    const std::string &serialised_selectables);
  void ClearKeyChain();

  void Clear();
  friend class test::SystemPacketHandlerTest;
  friend class
      test::SystemPacketHandlerTest_FUNC_SigningAndIdentityPackets_Test;

 private:
  SystemPacketHandler &operator=(const SystemPacketHandler&);
  SystemPacketHandler(const SystemPacketHandler&);

  struct PacketInfo {
    PacketInfo() : pending(), stored() {}
    explicit PacketInfo(PacketPtr pend)
        : pending(),
          stored() {
      if (pend) {
        // keep a copy of the contents
        if (pend->packet_type() == kTmid || pend->packet_type() == kStmid) {
          pending = std::shared_ptr<TmidPacket>(new TmidPacket(
              *std::static_pointer_cast<TmidPacket>(pend)));
        } else if (pend->packet_type() == kMid ||
                   pend->packet_type() == kSmid) {
          pending = std::shared_ptr<MidPacket>(new MidPacket(
              *std::static_pointer_cast<MidPacket>(pend)));
        } else if (IsSignature(pend->packet_type(), false)) {
          pending = std::shared_ptr<pki::SignaturePacket>(
              new pki::SignaturePacket(
                  *std::static_pointer_cast<pki::SignaturePacket>(pend)));
        }
      }
    }
    PacketPtr pending, stored;
    friend class boost::serialization::access;

   private:
#ifdef __MSVC__
#  pragma warning(disable: 4127)
#endif
    template<typename Archive>
    void serialize(Archive &archive, const unsigned int /*version*/) {  // NOLINT (Fraser)
      pki::SignaturePacket sig_packet;
      if (Archive::is_saving::value) {
        BOOST_ASSERT(IsSignature(stored->packet_type(), false));
        sig_packet = *std::static_pointer_cast<pki::SignaturePacket>(stored);
      }
      archive & sig_packet;
      if (Archive::is_loading::value) {
        stored.reset(new pki::SignaturePacket(sig_packet));
        BOOST_ASSERT(IsSignature(stored->packet_type(), false));
      }
#ifdef __MSVC__
#  pragma warning(default: 4127)
#endif
    }
  };

 public:
  typedef std::map<PacketType, PacketInfo> SystemPacketMap;
  typedef std::map<std::string,
                   std::pair<PacketInfo, PacketInfo>>
          SelectableIdentitiesMap;
  typedef std::map<std::string,
                   std::pair<pki::SignaturePacket, pki::SignaturePacket>>
          SelectableIdentitiesSerialiser;

 private:
  bool IsConfirmed(SystemPacketMap::const_iterator it) const;

  SystemPacketMap packets_;
  SelectableIdentitiesMap selectable_ids_;
  mutable boost::mutex mutex_, selectable_ids_mutex_;
};

}  // namespace passport

}  // namespace maidsafe

#endif  // MAIDSAFE_PASSPORT_SYSTEM_PACKET_HANDLER_H_
