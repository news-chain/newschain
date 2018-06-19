//
// Created by boy on 18-6-11.
//
#pragma once



#define NEWS_SYMBOL

#define NEWS_GENESIS_TIME       (fc::time_point_sec(0))
#define NEWS_ADDRESS_PREFIX     ("NT")



#define NEWS_MAX_BLOCK_SIZE     (1024 * 1024 * 3L)
#define NEWS_MIN_BLOCK_SIZE     (0L)

#define NEWS_BLOCK_INTERVAL     3
#define IRREVERSIBLE_BLOCK_NUM  (20)            //about 1min


#define NEWS_VERSION            (news::base::version(0, 0, 1))
#define NEWS_CHAIN_ID           (fc::sha256::hash(std::string("'test")))
