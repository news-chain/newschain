//
// Created by boy on 18-6-11.
//
#pragma once




#define NEWS_GENESIS_TIME       (fc::time_point_sec(0))
#define NEWS_ADDRESS_PREFIX     ("NT")
#define NEWS_SYMBOL             ((uint64_t(8) | uint64_t('N') << 8 | uint64_t('E') << 16 | uint64_t('W') << 24 | uint64_t('T') << 32))

#define NEWS_BLOCK_INTERVAL     3
#define NEWS_TRANSACTIONS_MAN_SIZE      (1024 * 32)
#define NEWS_MIN_BLOCK_SIZE             (128)
#define NEWS_MAX_BLOCK_SIZE             (NEWS_TRANSACTIONS_MAN_SIZE * NEWS_BLOCK_INTERVAL * 2000L)


#define IRREVERSIBLE_BLOCK_NUM  (21)            //about 1min        18446744073709551616


#define NEWS_VERSION            (news::base::version(0, 0, 1))
#define NEWS_CHAIN_ID           (fc::sha256::hash(std::string("'test")))


#define NEWS_MOME_MAX_LENGTH    (256)
#define NEWS_MOME_MIN_LENGTH    (0)


#define NEWS_INIT_PRIVATE_KEY       (fc::ecc::private_key::regenerate(fc::sha256::hash(std::string("init_key"))))
#define NEWS_INIT_PUBLIC_KEY        (std::string(news::base::public_key_type(NEWS_INIT_PRIVATE_KEY.get_public_key()))) //NT6LLegbAgLAy28EHrffBVuANFWcFgmqRMW13wBmTExqFE9SCkg4

#define NEWS_MAX_TIME_EXPIRATION    (60*30)  //30min

#define NEWS_SYSTEM_ACCOUNT_NAME    (1)
#define NEWS_100_PERCENT            (10000)