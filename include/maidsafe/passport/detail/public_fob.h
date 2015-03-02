/*  Copyright 2012 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#ifndef MAIDSAFE_PASSPORT_DETAIL_PUBLIC_FOB_H_
#define MAIDSAFE_PASSPORT_DETAIL_PUBLIC_FOB_H_

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "boost/optional/optional.hpp"
#include "cereal/access.hpp"
#include "cereal/types/base_class.hpp"

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"
#include "maidsafe/common/data_types/data.h"
// We must include all archives which this polymorphic type will be used with *before* the
// CEREAL_REGISTER_TYPE call below.
#include "maidsafe/common/serialisation/binary_archive.h"
#include "maidsafe/common/serialisation/serialisation.h"

#include "maidsafe/passport/detail/config.h"
#include "maidsafe/passport/detail/fob.h"

namespace maidsafe {

namespace passport {

namespace detail {

template <typename TagType>
class PublicFob : public Data {
 public:
  using Name = maidsafe::detail::Name<PublicFob>;
  using Tag = TagType;
  using Signer = Fob<typename SignerFob<TagType>::Tag>;
  using ValidationToken = typename Fob<Tag>::ValidationToken;

  explicit PublicFob(const Fob<Tag>& fob)
      : name_(fob.name()),
        public_key_(fob.public_key()),
        validation_token_(fob.validation_token()) {}

  PublicFob() = default;

  PublicFob(const PublicFob& other) = default;

  PublicFob(PublicFob&& other)
      : name_(std::move(other.name_)),
        public_key_(std::move(other.public_key_)),
        validation_token_(std::move(other.validation_token_)) {}

  PublicFob& operator=(const PublicFob&) = default;

  PublicFob& operator=(PublicFob&& other) {
    name_ = std::move(other.name_);
    public_key_ = std::move(other.public_key_);
    validation_token_ = std::move(other.validation_token_);
    return *this;
  }

  virtual ~PublicFob() final = default;

  virtual std::uint32_t TagValue() const final {
    if (!IsInitialised())
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::uninitialised));
    return static_cast<std::uint32_t>(Tag::kValue);
  }

  virtual boost::optional<std::unique_ptr<Data>> Merge(
      const std::vector<std::unique_ptr<Data>>& /*data_collection*/) const final {
    if (!IsInitialised())
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::uninitialised));
    return boost::none;
  }

  bool IsInitialised() const { return name_->IsInitialised(); }

  Name name() const {
    if (!IsInitialised())
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::uninitialised));
    return name_;
  }

  asymm::PublicKey public_key() const {
    if (!IsInitialised())
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::uninitialised));
    return public_key_;
  }

  ValidationToken validation_token() const {
    if (!IsInitialised())
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::uninitialised));
    return validation_token_;
  }

  template <typename Archive>
  Archive& save(Archive& archive) const {
    if (!IsInitialised())
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::uninitialised));
    return archive(cereal::base_class<Data>(this), name_, asymm::EncodeKey(public_key_).string(),
                   validation_token_);
  }

  template <typename Archive>
  Archive& load(Archive& archive) {
    std::string temp_raw_public_key;
    try {
      archive(cereal::base_class<Data>(this), name_, temp_raw_public_key, validation_token_);
      public_key_ = asymm::DecodeKey(asymm::EncodedPublicKey(temp_raw_public_key));
    }
    catch (const std::exception&) {
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
    }
    ValidateToken(temp_raw_public_key);
    return archive;
  }

 private:
  virtual const Identity& Id() const final {
    if (!IsInitialised())
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::uninitialised));
    return name_.value;
  }

  // For self-signed keys
  template <typename T = TagType>
  void ValidateToken(
      const std::string& encoded_public_key,
      typename std::enable_if<std::is_same<Fob<T>, Signer>::value>::type* =
          0) const {
    // Check the validation token is valid
    if (!asymm::CheckSignature(asymm::PlainText(encoded_public_key + ConvertToString(Tag::kValue)),
                               validation_token_, public_key_)) {
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
    }
    // Check the name is the hash of the public key + validation token
    if (crypto::Hash<crypto::SHA512>(encoded_public_key + validation_token_.string()) !=
        name_.value) {
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
    }
  }

  // For non-self-signed keys
  template <typename T = TagType>
  void ValidateToken(
      const std::string& encoded_public_key,
      typename std::enable_if<!std::is_same<Fob<T>, Signer>::value>::type* =
          0) const {
    // Check the validation token is valid
    if (!asymm::CheckSignature(asymm::PlainText(validation_token_.signature_of_public_key.string() +
                                                encoded_public_key + ConvertToString(Tag::kValue)),
                               validation_token_.self_signature, public_key_)) {
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
    }
    // Check the name is the hash of the public key + validation token
    if (crypto::Hash<crypto::SHA512>(encoded_public_key + ConvertToString(validation_token_)) !=
        name_.value)
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
  }

  Name name_;
  asymm::PublicKey public_key_;
  ValidationToken validation_token_;
};

}  // namespace detail

}  // namespace passport

}  // namespace maidsafe

#endif  // MAIDSAFE_PASSPORT_DETAIL_PUBLIC_FOB_H_
