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

#include "maidsafe/passport/detail/public_fob.h"

#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/passport/types.h"
#include "maidsafe/passport/detail/passport_pb.h"

namespace pb = maidsafe::passport::detail::protobuf;

namespace maidsafe {

namespace passport {

namespace test {

TEST(PublicFobTest, BEH_FobGenerationAndValidation) {
  Anmid anmid;
  Ansmid ansmid;
  Antmid antmid;
  Anmaid anmaid;
  Maid maid(anmaid);
  Pmid pmid(maid);
  Anmpid anmpid;
  Mpid mpid(NonEmptyString(RandomAlphaNumericString(1 + RandomUint32() % 100)), anmpid);

  PublicAnmid public_anmid(anmid);
  PublicAnsmid public_ansmid(ansmid);
  PublicAntmid public_antmid(antmid);
  PublicAnmaid public_anmaid(anmaid);
  PublicMaid public_maid(maid);
  PublicPmid public_pmid(pmid);
  PublicAnmpid public_anmpid(anmpid);
  PublicMpid public_mpid(mpid);

  PublicAnmid public_anmid1(public_anmid);
  PublicAnsmid public_ansmid1(public_ansmid);
  PublicAntmid public_antmid1(public_antmid);
  PublicAnmaid public_anmaid1(public_anmaid);
  PublicMaid public_maid1(public_maid);
  PublicPmid public_pmid1(public_pmid);
  PublicAnmpid public_anmpid1(public_anmpid);
  PublicMpid public_mpid1(public_mpid);

  PublicAnmid public_anmid2(std::move(public_anmid1));
  PublicAnsmid public_ansmid2(std::move(public_ansmid1));
  PublicAntmid public_antmid2(std::move(public_antmid1));
  PublicAnmaid public_anmaid2(std::move(public_anmaid1));
  PublicMaid public_maid2(std::move(public_maid1));
  PublicPmid public_pmid2(std::move(public_pmid1));
  PublicAnmpid public_anmpid2(std::move(public_anmpid1));
  PublicMpid public_mpid2(std::move(public_mpid1));

  public_anmid1 = public_anmid;
  public_ansmid1 = public_ansmid;
  public_antmid1 = public_antmid;
  public_anmaid1 = public_anmaid;
  public_maid1 = public_maid;
  public_pmid1 = public_pmid;
  public_anmpid1 = public_anmpid;
  public_mpid1 = public_mpid;

  public_anmid2 = std::move(public_anmid1);
  public_ansmid2 = std::move(public_ansmid1);
  public_antmid2 = std::move(public_antmid1);
  public_anmaid2 = std::move(public_anmaid1);
  public_maid2 = std::move(public_maid1);
  public_pmid2 = std::move(public_pmid1);
  public_anmpid2 = std::move(public_anmpid1);
  public_mpid2 = std::move(public_mpid1);
}

template<typename PublicFobType>
bool CheckSerialisationAndParsing(PublicFobType public_fob) {
  auto name(public_fob.name());
  auto serialised_public_fob(public_fob.Serialise());
  PublicFobType public_fob2(name, serialised_public_fob);
  if (public_fob.name() != public_fob2.name()) {
    LOG(kError) << "Names don't match.";
    return false;
  }
  if (public_fob.validation_token() != public_fob2.validation_token()) {
    LOG(kError) << "Validation tokens don't match.";
    return false;
  }
  if (!rsa::MatchingKeys(public_fob.public_key(), public_fob2.public_key())) {
    LOG(kError) << "Public keys don't match.";
    return false;
  }
  return true;
}

TEST(PublicFobTest, BEH_FobSerialisationAndParsing) {
  Anmid anmid;
  Ansmid ansmid;
  Antmid antmid;
  Anmaid anmaid;
  Maid maid(anmaid);
  Pmid pmid(maid);
  Anmpid anmpid;
  Mpid mpid(NonEmptyString(RandomAlphaNumericString(1 + RandomUint32() % 100)), anmpid);

  PublicAnmid public_anmid(anmid);
  PublicAnsmid public_ansmid(ansmid);
  PublicAntmid public_antmid(antmid);
  PublicAnmaid public_anmaid(anmaid);
  PublicMaid public_maid(maid);
  PublicPmid public_pmid(pmid);
  PublicAnmpid public_anmpid(anmpid);
  PublicMpid public_mpid(mpid);

  CheckSerialisationAndParsing(public_anmid);
  CheckSerialisationAndParsing(public_ansmid);
  CheckSerialisationAndParsing(public_antmid);
  CheckSerialisationAndParsing(public_anmaid);
  CheckSerialisationAndParsing(public_maid);
  CheckSerialisationAndParsing(public_pmid);
  CheckSerialisationAndParsing(public_anmpid);
  CheckSerialisationAndParsing(public_mpid);
}

TEST(PublicFobTest, BEH_ConstructFromBadStrings) {
  Identity name(RandomString(64));
  NonEmptyString string(RandomAlphaNumericString(1 + RandomUint32() % 100));
  EXPECT_THROW(PublicAnmid(PublicAnmid::name_type(name), PublicAnmid::serialised_type(string)),
               std::exception);
  EXPECT_THROW(PublicAnsmid(PublicAnsmid::name_type(name), PublicAnsmid::serialised_type(string)),
               std::exception);
  EXPECT_THROW(PublicAntmid(PublicAntmid::name_type(name), PublicAntmid::serialised_type(string)),
               std::exception);
  EXPECT_THROW(PublicAnmaid(PublicAnmaid::name_type(name), PublicAnmaid::serialised_type(string)),
               std::exception);
  EXPECT_THROW(PublicMaid(PublicMaid::name_type(name), PublicMaid::serialised_type(string)),
               std::exception);
  EXPECT_THROW(PublicPmid(PublicPmid::name_type(name), PublicPmid::serialised_type(string)),
               std::exception);
  EXPECT_THROW(PublicAnmpid(PublicAnmpid::name_type(name), PublicAnmpid::serialised_type(string)),
               std::exception);
  EXPECT_THROW(PublicMpid(PublicMpid::name_type(name), PublicMpid::serialised_type(string)),
               std::exception);
}

TEST(PublicFobTest, BEH_ConstructFromUninitialisedStrings) {
  Identity uninitialised_name;
  Identity name(RandomString(64));
  NonEmptyString uninitialised_string;
  NonEmptyString string(RandomAlphaNumericString(1 + RandomUint32() % 100));
  EXPECT_THROW(PublicAnmid(PublicAnmid::name_type(name),
                           (PublicAnmid::serialised_type(uninitialised_string))),
               std::exception);
  EXPECT_THROW(PublicAnsmid(PublicAnsmid::name_type(name),
                            (PublicAnsmid::serialised_type(uninitialised_string))),
               std::exception);
  EXPECT_THROW(PublicAntmid(PublicAntmid::name_type(name),
                            (PublicAntmid::serialised_type(uninitialised_string))),
               std::exception);
  EXPECT_THROW(PublicAnmaid(PublicAnmaid::name_type(name),
                            (PublicAnmaid::serialised_type(uninitialised_string))),
               std::exception);
  EXPECT_THROW(PublicMaid(PublicMaid::name_type(name),
                          (PublicMaid::serialised_type(uninitialised_string))),
               std::exception);
  EXPECT_THROW(PublicPmid(PublicPmid::name_type(name),
                          (PublicPmid::serialised_type(uninitialised_string))),
               std::exception);
  EXPECT_THROW(PublicAnmpid(PublicAnmpid::name_type(name),
                            (PublicAnmpid::serialised_type(uninitialised_string))),
               std::exception);
  EXPECT_THROW(PublicMpid(PublicMpid::name_type(name),
                          (PublicMpid::serialised_type(uninitialised_string))),
               std::exception);

  EXPECT_THROW(PublicAnmid(PublicAnmid::name_type(uninitialised_name),
                           (PublicAnmid::serialised_type(string))),
               std::exception);
  EXPECT_THROW(PublicAnsmid(PublicAnsmid::name_type(uninitialised_name),
                            (PublicAnsmid::serialised_type(string))),
               std::exception);
  EXPECT_THROW(PublicAntmid(PublicAntmid::name_type(uninitialised_name),
                            (PublicAntmid::serialised_type(string))),
               std::exception);
  EXPECT_THROW(PublicAnmaid(PublicAnmaid::name_type(uninitialised_name),
                            (PublicAnmaid::serialised_type(string))),
               std::exception);
  EXPECT_THROW(PublicMaid(PublicMaid::name_type(uninitialised_name),
                          (PublicMaid::serialised_type(string))),
               std::exception);
  EXPECT_THROW(PublicPmid(PublicPmid::name_type(uninitialised_name),
                          (PublicPmid::serialised_type(string))),
               std::exception);
  EXPECT_THROW(PublicAnmpid(PublicAnmpid::name_type(uninitialised_name),
                            (PublicAnmpid::serialised_type(string))),
               std::exception);
  EXPECT_THROW(PublicMpid(PublicMpid::name_type(uninitialised_name),
                          (PublicMpid::serialised_type(string))),
               std::exception);
}

TEST(PublicFobTest, BEH_SerialiseUninitialisedMessage) {
  pb::PublicFob proto_public_fob;
  EXPECT_THROW(NonEmptyString(proto_public_fob.SerializeAsString()), std::exception);
}

}  // namespace test

}  // namespace passport

}  // namespace maidsafe