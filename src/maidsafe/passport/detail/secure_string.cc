/***************************************************************************************************
 *  Copyright 2013 maidsafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use. The use of this code is governed by the licence file licence.txt found in the root of     *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit written *
 *  permission of the board of directors of MaidSafe.net.                                          *
 **************************************************************************************************/

#include "maidsafe/passport/detail/secure_string.h"

#include <regex>

#include "maidsafe/common/utils.h"

namespace maidsafe {
namespace passport {
namespace detail {

SecureString::String::String() {}

SecureString::String::String(const std::string& string)
  : StringBase(string.c_str(), string.length()) {}

SecureString::String::String(const char* character_ptr)
  : StringBase(character_ptr, std::char_traits<char>::length(character_ptr)) {}

SecureString::String::String(size_type size, char character)
  : StringBase(size, character) {}

SecureString::String::~String() {}

SecureString::SecureString()
  : regex_("\\w"),
    phrase_(RandomAlphaNumericString(64)),
    string_(),
    encryptor_(Encryptor(phrase_.data(), new Encoder(new Sink(string_)))) {}

SecureString::~SecureString() {}

void SecureString::Append(char character) {
  if (!IsValid(character))
    ThrowError(CommonErrors::invalid_parameter);
  encryptor_.Put(character, 1);
}

void SecureString::Finalise() {
  encryptor_.MessageEnd();
}

void SecureString::PlainText(String& plain_text) {
  Decoder decryptor(new Decryptor(phrase_.data(), new Sink(plain_text)));
  decryptor.Put(reinterpret_cast<byte*>(const_cast<char*>(string_.data())), string_.length());
  decryptor.MessageEnd();
  return;
}

SecureString::String SecureString::CipherText() {
  return string_;
}

bool SecureString::IsValid(char& character) {
  return std::regex_match(String(1, character), regex_);
}

}  // namespace detail
}  // namespace passport
}  // namespace maidsafe