#pragma once
#include <fsciolib/fscio.hpp>
#include <fsciolib/ignore.hpp>
#include <fsciolib/transaction.hpp>

namespace fscio {

   class [[fscio::contract("fscio.msig")]] multisig : public contract {
      public:
         using contract::contract;

         [[fscio::action]]
         void propose(ignore<name> proposer, ignore<name> proposal_name,
               ignore<std::vector<permission_level>> requested, ignore<transaction> trx);
         [[fscio::action]]
         void approve( name proposer, name proposal_name, permission_level level,
                       const fscio::binary_extension<fscio::checksum256>& proposal_hash );
         [[fscio::action]]
         void unapprove( name proposer, name proposal_name, permission_level level );
         [[fscio::action]]
         void cancel( name proposer, name proposal_name, name canceler );
         [[fscio::action]]
         void exec( name proposer, name proposal_name, name executer );
         [[fscio::action]]
         void invalidate( name account );

      private:
         struct [[fscio::table]] proposal {
            name                            proposal_name;
            std::vector<char>               packed_transaction;

            uint64_t primary_key()const { return proposal_name.value; }
         };

         typedef fscio::multi_index< "proposal"_n, proposal > proposals;

         struct [[fscio::table]] old_approvals_info {
            name                            proposal_name;
            std::vector<permission_level>   requested_approvals;
            std::vector<permission_level>   provided_approvals;

            uint64_t primary_key()const { return proposal_name.value; }
         };
         typedef fscio::multi_index< "approvals"_n, old_approvals_info > old_approvals;

         struct approval {
            permission_level level;
            time_point       time;
         };

         struct [[fscio::table]] approvals_info {
            uint8_t                 version = 1;
            name                    proposal_name;
            //requested approval doesn't need to cointain time, but we want requested approval
            //to be of exact the same size ad provided approval, in this case approve/unapprove
            //doesn't change serialized data size. So, we use the same type.
            std::vector<approval>   requested_approvals;
            std::vector<approval>   provided_approvals;

            uint64_t primary_key()const { return proposal_name.value; }
         };
         typedef fscio::multi_index< "approvals2"_n, approvals_info > approvals;

         struct [[fscio::table]] invalidation {
            name         account;
            time_point   last_invalidation_time;

            uint64_t primary_key() const { return account.value; }
         };

         typedef fscio::multi_index< "invals"_n, invalidation > invalidations;
   };

} /// namespace fscio
