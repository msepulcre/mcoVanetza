#include "mco_fac.hpp"
#include "ethernet_device.hpp"
#include "benchmark_application.hpp"
#include "cam_application.hpp"
#include "hello_application.hpp"
#include "link_layer.hpp"
#include "positioning.hpp"
#include "router_context.hpp"
#include "security.hpp"
#include "time_trigger.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <iostream>
/* #include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h> */


/* using namespace log4cxx;
using namespace log4cxx::xml;
using namespace log4cxx::helpers; */

namespace asio = boost::asio;
namespace gn = vanetza::geonet;
namespace po = boost::program_options;
using namespace vanetza;

int main(int argc, const char** argv)
{
    /* DOMConfigurator::configure("/usr/local/src/Log4cxxConfig.xml"); */
    
    po::options_description options("Allowed options");
    options.add_options()
        ("help", "Print out available options.")
        ("link-layer,l", po::value<std::string>()->default_value("ethernet"), "Link layer type")
        ("interface,i", po::value<std::string>()->default_value("lo"), "Network interface to use.")
        ("mac-address", po::value<std::string>(), "Override the network interface's MAC address.")
        ("require-gnss-fix", "Suppress transmissions while GNSS position fix is missing")
        ("gn-version", po::value<unsigned>()->default_value(1), "GeoNetworking protocol version to use.")
        ("cam-interval", po::value<unsigned>()->default_value(100), "CAM sending interval in milliseconds.")
        ("cbr,c", po::value<double>()->default_value(0.8), "CBR")
        ("cbr_target,c", po::value<double>()->default_value(0.68), "CBR target")
        ("use-mco", po::value<int>()->default_value(1), "Ejecutar con mco (!= 0) o sin mco (= 0)")
        ("num_ca, n", po::value<int>()->default_value(5), "number of application CA")
        ("print-rx-cam", "Print received CAMs")
        ("print-tx-cam", "Print generated CAMs")
        ("benchmark", "Enable benchmarking")
        ("applications,a", po::value<std::vector<std::string>>()->default_value({"ca"}, "ca")->multitoken(), "Run applications [ca,hello,benchmark]")
        ("non-strict", "Set MIB parameter ItsGnSnDecapResultHandling to NON_STRICT")
    ;
    add_positioning_options(options);
    add_security_options(options);

    po::positional_options_description positional_options;
    positional_options.add("interface", 1);

    po::variables_map vm;

    try {
        po::store(
            po::command_line_parser(argc, argv)
                .options(options)
                .positional(positional_options)
                .run(),
            vm
        );
        po::notify(vm);
    } catch (po::error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << options << std::endl;
        return 1;
    }
    
    if (vm.count("help")) {
        std::cout << options << std::endl;
        return 1;
    }

    
    
    try {
        asio::io_service io_service;
        TimeTrigger trigger(io_service);

        const char* device_name = vm["interface"].as<std::string>().c_str();
        EthernetDevice device(device_name);
        vanetza::MacAddress mac_address = device.address();

        if (vm.count("mac-address")) {
            std::cout << "Using MAC address: " << vm["mac-address"].as<std::string>() << "." << std::endl;

            if (!parse_mac_address(vm["mac-address"].as<std::string>().c_str(), mac_address)) {
                std::cerr << "The specified MAC address is invalid." << std::endl;
                return 1;
            }
        }

        const std::string link_layer_name = vm["link-layer"].as<std::string>();
        auto link_layer =  create_link_layer(io_service, device, link_layer_name);
        if (!link_layer) {
            std::cerr << "No link layer '" << link_layer_name << "' found." << std::endl;
            return 1;
        }

        auto signal_handler = [&io_service](const boost::system::error_code& ec, int signal_number) {
            if (!ec) {
                std::cout << "Termination requested." << std::endl;
                io_service.stop();
            }
        };
        asio::signal_set signals(io_service, SIGINT, SIGTERM);
        signals.async_wait(signal_handler);

        // configure management information base
        // TODO: make more MIB options configurable by command line flags
        gn::MIB mib;
        mib.itsGnLocalGnAddr.mid(mac_address);
        mib.itsGnLocalGnAddr.is_manually_configured(true);
        mib.itsGnLocalAddrConfMethod = geonet::AddrConfMethod::Managed;
        mib.itsGnSecurity = false;
        if (vm.count("non-strict")) {
            mib.itsGnSnDecapResultHandling = vanetza::geonet::SecurityDecapHandling::Non_Strict;
        }
        mib.itsGnProtocolVersion = vm["gn-version"].as<unsigned>();

        if (mib.itsGnProtocolVersion != 0 && mib.itsGnProtocolVersion != 1) {
            throw std::runtime_error("Unsupported GeoNetworking version, only version 0 and 1 are supported.");
        }

        auto positioning = create_position_provider(io_service, vm, trigger.runtime());
        if (!positioning) {
            std::cerr << "Requested positioning method is not available\n";
            return 1;
        }

        auto security = create_security_entity(vm, trigger.runtime(), *positioning);
        if (security) {
            mib.itsGnSecurity = true;
        }

        int use_mco = vm["use-mco"].as<int>();

        RouterContext context(mib, trigger, *positioning, security.get(), use_mco);

        /* RouterContext context(mib, trigger, *positioning, security.get()); */

        context.require_position_fix(vm.count("require-gnss-fix") > 0);
        context.set_link_layer(link_layer.get());
        
        

        std::unique_ptr<McoFac> mco;
        
        if(use_mco != 0){

            mco = std::make_unique<McoFac>(*positioning, trigger.runtime());

            float CBR = vm["cbr"].as<double>();
            float CBR_target = vm["cbr_target"].as<double>();

            mco->CBR = CBR;
            mco->CBR_target = CBR_target;
        }

        


        std::map<std::string, std::unique_ptr<Application>> apps;
        for (const std::string& app_name : vm["applications"].as<std::vector<std::string>>()) {
            if (apps.find(app_name) != apps.end()) {
                std::cerr << "application '" << app_name << "' requested multiple times, skip\n";
                continue;
            }

            

            if (app_name == "ca") {

                int num_ca = vm["num_ca"].as<int>();

                int i;

                for(i = 0; i < num_ca ; i++){

                    std::unique_ptr<CamApplication> ca;

                    if(use_mco != 0){

                    
                        ca = std::make_unique<CamApplication>(*mco, *positioning, trigger.runtime(), use_mco);
                    

                    } else{

                        ca = std::make_unique<CamApplication>(*mco, *positioning, trigger.runtime());

                    }  
                    std::string str_num = std::to_string(i);
                    std::string iter_name = "ca" + str_num;
                
                    ca->set_interval(std::chrono::milliseconds(vm["cam-interval"].as<unsigned>()));  
                    ca->print_received_message(vm.count("print-rx-cam") > 0);
                    ca->print_generated_message(vm.count("print-tx-cam") > 0);
                    apps.emplace(iter_name, std::move(ca));
                    
                }

                if(use_mco != 0){
                    apps.emplace("mco", std::move(mco)); 
                }
                


                
            } else if (app_name == "hello") {
                std::unique_ptr<HelloApplication> hello {
                    new HelloApplication(io_service, std::chrono::milliseconds(800))
                };
                apps.emplace(app_name, std::move(hello));
            } else if (app_name == "benchmark") {
                std::unique_ptr<BenchmarkApplication> benchmark {
                    new BenchmarkApplication(io_service)
                };
                apps.emplace(app_name, std::move(benchmark));
            } else {
                std::cerr << "skip unknown application '" << app_name << "'\n";
            }
        }

        if (apps.empty()) {
            std::cerr << "Warning: No applications are configured, only GN beacons will be exchanged\n";
        }

        for (const auto& app : apps) {
            std::cout << "Enable application '" << app.first << "'...\n";
            context.enable(app.second.get());
        }

        io_service.run();
    } catch (PositioningException& e) {
        std::cerr << "Exit because of positioning error: " << e.what() << std::endl;
        return 1;
    } catch (std::exception& e) {
        std::cerr << "Exit: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
