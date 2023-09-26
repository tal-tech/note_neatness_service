#include "config.hpp"

namespace facethink {
  namespace detgemmactpn {

	  void Config::ReadIniFile(const std::string& config_file) {

		  boost::property_tree::ptree pt;
		  boost::property_tree::ini_parser::read_ini(config_file, pt);


		  boost::log::add_file_log
		  (
			  boost::log::keywords::auto_flush = true,
			  boost::log::keywords::file_name = "det_ge_mma_ctpn.log",
			  boost::log::keywords::format = "[%TimeStamp%]: %Message%"
		  );
		  boost::log::add_common_attributes();

		  int log_level = pt.get<int>("log_level", 4);
		  boost::log::core::get()->set_filter(boost::log::trivial::severity >= log_level);
	  }

	  void Config::StopFileLogging() {
		  boost::log::core::get()->remove_sink(file_sink);
		  file_sink.reset();
	  }


  }
}
