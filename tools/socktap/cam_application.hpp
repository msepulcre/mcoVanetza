#ifndef CAM_APPLICATION_HPP_EUIC2VFR
#define CAM_APPLICATION_HPP_EUIC2VFR

#include "mco_fac.hpp" 
#include "application.hpp"
#include <vanetza/common/clock.hpp>
#include <vanetza/common/position_provider.hpp>
#include <vanetza/common/runtime.hpp>


class CamApplication : public Application
{
public:

    CamApplication(McoFac &mco, vanetza::PositionProvider& positioning, vanetza::Runtime& rt, int use_mco);

    CamApplication(McoFac &mco, vanetza::PositionProvider& positioning, vanetza::Runtime& rt); 

    PortType port() override;
    void indicate(const DataIndication&, UpPacketPtr) override;
    void set_interval(vanetza::Clock::duration) override;
    void print_received_message(bool flag);
    void print_generated_message(bool flag);

    int use_mco_;
    int PORT_number;

private:
    void schedule_timer();
    void on_timer(vanetza::Clock::time_point);

    McoFac& mco_; 
    vanetza::PositionProvider& positioning_;
    vanetza::Runtime& runtime_;
    vanetza::Clock::duration cam_interval_;
    bool print_rx_msg_ = false;
    bool print_tx_msg_ = false;
};

#endif /* CAM_APPLICATION_HPP_EUIC2VFR */
