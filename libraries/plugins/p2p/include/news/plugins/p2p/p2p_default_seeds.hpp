#pragma once

#include <vector>
#include <string>

namespace news{ namespace plugins { namespace p2p {

#ifdef IS_TEST_NET
const std::vector< std::string > default_seeds;
#else
const std::vector< std::string > default_seeds;
#endif

} } } //  ::plugins::p2p
