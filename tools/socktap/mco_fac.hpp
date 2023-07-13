#ifndef MCO_FAC_HPP_PSIGPUTG
#define MCO_FAC_HPP_PSIGPUTG

#include <vanetza/btp/data_interface.hpp>
#include <vanetza/btp/data_indication.hpp>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/btp/port_dispatcher.hpp>
#include <vanetza/geonet/data_confirm.hpp>
#include <vanetza/geonet/router.hpp>

class McoFac : public vanetza::btp::IndicationInterface
{
public:
    using DataConfirm = vanetza::geonet::DataConfirm;
    using DataIndication = vanetza::btp::DataIndication;
    using DataRequest = vanetza::btp::DataRequestGeoNetParams;
    using DownPacketPtr = vanetza::geonet::Router::DownPacketPtr;
    using PortType = vanetza::btp::port_type;
    using PromiscuousHook = vanetza::btp::PortDispatcher::PromiscuousHook;
    using UpPacketPtr = vanetza::geonet::Router::UpPacketPtr;

    McoFac() = default;
    McoFac(const McoFac&) = delete;
    McoFac& operator=(const McoFac&) = delete;
    virtual ~McoFac() = default;

    virtual PortType port() = 0;
    virtual PromiscuousHook* promiscuous_hook();

protected:
    DataConfirm request(const DataRequest&, DownPacketPtr);

private:
    friend class RouterContext;
    vanetza::geonet::GbcDataRequest request_gbc_mco(const DataRequest&);
    vanetza::geonet::ShbDataRequest request_shb_mco(const DataRequest&);
    vanetza::geonet::Router* router_;
};

#endif /* MCO_FAC_HPP_PSIGPUTG */

