/***************************************************************************************************
 *  Copyright 2012 maidsafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use. The use of this code is governed by the licence file licence.txt found in the root of     *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit written *
 *  permission of the board of directors of MaidSafe.net.                                          *
 **************************************************************************************************/

#ifndef MAIDSAFE_PASSPORT_TYPES_H_
#define MAIDSAFE_PASSPORT_TYPES_H_

#include "maidsafe/common/tagged_value.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/detail/config.h"
#include "maidsafe/passport/detail/fob.h"
#include "maidsafe/passport/detail/public_fob.h"
#include "maidsafe/passport/detail/identity_data.h"


namespace maidsafe {

namespace passport {

// The Fob class template provides the unique identity elements required for encrypted, self
// authenticated storage/retrieval and communication on the the novinet network,
// http://novinet.com/. Identities, with the exception of Mid, Tmid and their surrogate variants
// are defined in terms of a Fob.

// Maidsafe Anonymous Identification:
// The Maid is used authenticate network transactions, it is signed by the Anmaid passed during its
// construction.
typedef detail::Fob<detail::MaidTag> Maid;
// Proxy Maidsafe Identification:
// The Pmid identifies a vault on the network, see http://maidsafe.github.io/MaidSafe-Vault/ for
// more information about vaults. Requests are signed by the Maid passed during construction.
typedef detail::Fob<detail::PmidTag> Pmid;
// Maidsafe Public Identification:
// The Mpid is used for anomymous public communication independent of a users credentials.
typedef detail::Fob<detail::MpidTag> Mpid;
// Anonymous Maidsafe Public Identification:
// The Anmpid is used to sign the Mpid.
typedef detail::Fob<detail::AnmpidTag> Anmpid;
// Anonymous Maid:
// Used only to sign the Maid.
typedef detail::Fob<detail::AnmaidTag> Anmaid;

// Surrogate/Temporary Maidsafe Identification:
// Signed by the Antmid, the Tmid/Stmid are used to store session details for a client. The Tmid
// name is the cryptographic hash of the PBKDF2 key that results from encrypting the session data
// with user input keyword, pin and password.
typedef detail::TmidData Tmid, Stmid;
// Anonymous Tmid:
// The Antmid is the signer type used by both Tmid and Stmid.
typedef detail::Fob<detail::AntmidTag> Antmid;
// Maidsafe Identification:
// The Mid contains the encrypted Tmid name that determine's the location of session details saved
// by a client. The Mid name is generated using PBKDF2 from a users keyword and pin and is used to
// authenticate a user during login.
typedef detail::MidData<detail::MidTag> Mid;
// Surrogate Maidsafe Identification:
// The Smid is used to ensure the integrity of the Mid during an update.
typedef detail::MidData<detail::SmidTag> Smid;
typedef detail::Fob<detail::AnmidTag> Anmid;
typedef detail::Fob<detail::AnsmidTag> Ansmid;

// Public key types allowing peers to validate digitally signed requests made by nodes on the
// network. Typically used to authenticate requesting nodes or to check data integrity. The digital
// signatures are generated using the RSA-probabilistic signature scheme, RSA-PSS, more information
// can be found at http://www.rsa.com/rsalabs, or http://www.cryptopp.com/ for the implementation.
typedef detail::PublicFob<detail::AnmidTag> PublicAnmid;
typedef detail::PublicFob<detail::AnsmidTag> PublicAnsmid;
typedef detail::PublicFob<detail::AntmidTag> PublicAntmid;
typedef detail::PublicFob<detail::AnmaidTag> PublicAnmaid;
typedef detail::PublicFob<detail::MaidTag> PublicMaid;
typedef detail::PublicFob<detail::PmidTag> PublicPmid;

typedef detail::PublicFob<detail::AnmpidTag> PublicAnmpid;
typedef detail::PublicFob<detail::MpidTag> PublicMpid;

// Public key type traits.
template<typename T>
struct is_public_key_type : public std::false_type {};

template<>
struct is_public_key_type<PublicAnmid> : public std::true_type {};
template<>
struct is_public_key_type<PublicAnsmid> : public std::true_type {};
template<>
struct is_public_key_type<PublicAntmid> : public std::true_type {};
template<>
struct is_public_key_type<PublicAnmaid> : public std::true_type {};
template<>
struct is_public_key_type<PublicMaid> : public std::true_type {};
template<>
struct is_public_key_type<PublicPmid> : public std::true_type {};
template<>
struct is_public_key_type<PublicAnmpid> : public std::true_type {};
template<>
struct is_public_key_type<PublicMpid> : public std::true_type {};

}  // namespace passport

// Short term cacheability traits.
template<>
struct is_short_term_cacheable<passport::PublicAnmid> : public std::true_type {};
template<>
struct is_short_term_cacheable<passport::PublicAnsmid> : public std::true_type {};
template<>
struct is_short_term_cacheable<passport::PublicAntmid> : public std::true_type {};
template<>
struct is_short_term_cacheable<passport::PublicAnmaid> : public std::true_type {};
template<>
struct is_short_term_cacheable<passport::PublicMaid> : public std::true_type {};
template<>
struct is_short_term_cacheable<passport::PublicPmid> : public std::true_type {};
template<>
struct is_short_term_cacheable<passport::PublicAnmpid> : public std::true_type {};
template<>
struct is_short_term_cacheable<passport::PublicMpid> : public std::true_type {};

}  // namespace maidsafe

#endif  // MAIDSAFE_PASSPORT_TYPES_H_
