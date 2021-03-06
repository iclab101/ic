

#include <ripple/protocol/digest.h>
#include <ripple/protocol/Indexes.h>
#include <boost/endian/conversion.hpp>
#include <cassert>

namespace ripple {

uint256
getLedgerHashIndex ()
{
    return sha512Half(std::uint16_t(spaceSkipList));
}

uint256
getLedgerHashIndex (std::uint32_t desiredLedgerIndex)
{
    return sha512Half(
        std::uint16_t(spaceSkipList),
        std::uint32_t(desiredLedgerIndex >> 16));
}
uint256
getLedgerAmendmentIndex ()
{
    return sha512Half(std::uint16_t(spaceAmendment));
}

uint256
getLedgerFeeIndex ()
{
    return sha512Half(std::uint16_t(spaceFee));
}

uint256
getAccountRootIndex (AccountID const& account)
{
    return sha512Half(
        std::uint16_t(spaceAccount),
        account);
}

uint256
getGeneratorIndex (AccountID const& uGeneratorID)
{
    return sha512Half(
        std::uint16_t(spaceGenerator),
        uGeneratorID);
}

uint256
getBookBase (Book const& book)
{
    assert (isConsistent (book));
    return getQualityIndex(sha512Half(
        std::uint16_t(spaceBookDir),
        book.in.currency,
        book.out.currency,
        book.in.account,
        book.out.account));
}

uint256
getOfferIndex (AccountID const& account, std::uint32_t uSequence)
{
    return sha512Half(
        std::uint16_t(spaceOffer),
        account,
        std::uint32_t(uSequence));
}

uint256
getOwnerDirIndex (AccountID const& account)
{
    return sha512Half(
        std::uint16_t(spaceOwnerDir),
        account);
}


uint256
getDirNodeIndex (uint256 const& uDirRoot, const std::uint64_t uNodeIndex)
{
    if (uNodeIndex == 0)
        return uDirRoot;

    return sha512Half(
        std::uint16_t(spaceDirNode),
        uDirRoot,
        std::uint64_t(uNodeIndex));
}

uint256
getQualityIndex (uint256 const& uBase, const std::uint64_t uNodeDir)
{
    uint256 uNode (uBase);

    ((std::uint64_t*) uNode.end ())[-1] = boost::endian::native_to_big (uNodeDir);

    return uNode;
}

uint256
getQualityNext (uint256 const& uBase)
{
    static uint256 const uNext (
        from_hex_text<uint256>("10000000000000000"));
    return uBase + uNext;
}

std::uint64_t
getQuality (uint256 const& uBase)
{
    return boost::endian::big_to_native (((std::uint64_t*) uBase.end ())[-1]);
}

uint256
getTicketIndex (AccountID const& account, std::uint32_t uSequence)
{
    return sha512Half(
        std::uint16_t(spaceTicket),
        account,
        std::uint32_t(uSequence));
}

uint256
getRippleStateIndex (AccountID const& a, AccountID const& b, Currency const& currency)
{
    if (a < b)
        return sha512Half(
            std::uint16_t(spaceRipple),
            a,
            b,
            currency);
    return sha512Half(
        std::uint16_t(spaceRipple),
        b,
        a,
        currency);
}

uint256
getRippleStateIndex (AccountID const& a, Issue const& issue)
{
    return getRippleStateIndex (a, issue.account, issue.currency);
}

uint256
getSignerListIndex (AccountID const& account)
{
    return sha512Half(
        std::uint16_t(spaceSignerList),
        account,
        std::uint32_t (0));  
}

uint256
getCheckIndex (AccountID const& account, std::uint32_t uSequence)
{
    return sha512Half(
        std::uint16_t(spaceCheck),
        account,
        std::uint32_t(uSequence));
}

uint256
getDepositPreauthIndex (AccountID const& owner, AccountID const& preauthorized)
{
    return sha512Half(
        std::uint16_t(spaceDepositPreauth),
        owner,
        preauthorized);
}


namespace keylet {

Keylet account_t::operator()(
    AccountID const& id) const
{
    return { ltACCOUNT_ROOT,
        getAccountRootIndex(id) };
}

Keylet child (uint256 const& key)
{
    return { ltCHILD, key };
}

Keylet skip_t::operator()() const
{
    return { ltLEDGER_HASHES,
        getLedgerHashIndex() };
}

Keylet skip_t::operator()(LedgerIndex ledger) const
{
    return { ltLEDGER_HASHES,
        getLedgerHashIndex(ledger) };
}

Keylet amendments_t::operator()() const
{
    return { ltAMENDMENTS,
        getLedgerAmendmentIndex() };
}

Keylet fees_t::operator()() const
{
    return { ltFEE_SETTINGS,
        getLedgerFeeIndex() };
}

Keylet book_t::operator()(Book const& b) const
{
    return { ltDIR_NODE,
        getBookBase(b) };
}

Keylet line_t::operator()(AccountID const& id0,
    AccountID const& id1, Currency const& currency) const
{
    return { ltRIPPLE_STATE,
        getRippleStateIndex(id0, id1, currency) };
}

Keylet line_t::operator()(AccountID const& id,
    Issue const& issue) const
{
    return { ltRIPPLE_STATE,
        getRippleStateIndex(id, issue) };
}

Keylet offer_t::operator()(AccountID const& id,
    std::uint32_t seq) const
{
    return { ltOFFER,
        getOfferIndex(id, seq) };
}

Keylet quality_t::operator()(Keylet const& k,
    std::uint64_t q) const
{
    assert(k.type == ltDIR_NODE);
    return { ltDIR_NODE,
        getQualityIndex(k.key, q) };
}

Keylet next_t::operator()(Keylet const& k) const
{
    assert(k.type == ltDIR_NODE);
    return { ltDIR_NODE,
        getQualityNext(k.key) };
}

Keylet ticket_t::operator()(AccountID const& id,
    std::uint32_t seq) const
{
    return { ltTICKET,
        getTicketIndex(id, seq) };
}

Keylet signers_t::operator()(AccountID const& id) const
{
    return { ltSIGNER_LIST,
        getSignerListIndex(id) };
}

Keylet check_t::operator()(AccountID const& id,
    std::uint32_t seq) const
{
    return { ltCHECK,
        getCheckIndex(id, seq) };
}

Keylet depositPreauth_t::operator()(AccountID const& owner,
    AccountID const& preauthorized) const
{
    return { ltDEPOSIT_PREAUTH,
        getDepositPreauthIndex(owner, preauthorized) };
}


Keylet unchecked (uint256 const& key)
{
    return { ltANY, key };
}

Keylet ownerDir(AccountID const& id)
{
    return { ltDIR_NODE,
        getOwnerDirIndex(id) };
}

Keylet page(uint256 const& key,
    std::uint64_t index)
{
    return { ltDIR_NODE,
        getDirNodeIndex(key, index) };
}

Keylet page(Keylet const& root,
    std::uint64_t index)
{
    assert(root.type == ltDIR_NODE);
    return page(root.key, index);
}

Keylet
escrow (AccountID const& source, std::uint32_t seq)
{
    sha512_half_hasher h;
    using beast::hash_append;
    hash_append(h, std::uint16_t(spaceEscrow));
    hash_append(h, source);
    hash_append(h, seq);
    return { ltESCROW, static_cast<uint256>(h) };
}

Keylet
payChan (AccountID const& source, AccountID const& dst, std::uint32_t seq)
{
    sha512_half_hasher h;
    using beast::hash_append;
    hash_append(h, std::uint16_t(spaceXRPUChannel));
    hash_append(h, source);
    hash_append(h, dst);
    hash_append(h, seq);
    return { ltPAYCHAN, static_cast<uint256>(h) };
}

} 

} 
























