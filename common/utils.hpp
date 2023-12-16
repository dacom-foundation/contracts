#include <eosio/eosio.hpp>
#include <eosio/transaction.hpp>

static uint128_t combine_ids(const uint64_t &x, const uint64_t &y) {
  return (uint128_t{x} << 64) | y;
};

std::string generate_random_name() {
  const char ALPHABET[] = "abcdefghijklmnopqrstuvwxyz";
  const int ALPHABET_SIZE = sizeof(ALPHABET) - 1;
  const int NAME_LENGTH = 12;

  uint64_t current_time =
      eosio::current_time_point().sec_since_epoch(); // Получаем текущее время
  std::string random_name;

  for (int i = 0; i < NAME_LENGTH; ++i) {
    current_time = (current_time * 1103515245 + 12345) %
                   0x100000000; // Линейный конгруэнтный метод
    int random_index = (current_time % ALPHABET_SIZE);
    random_name += ALPHABET[random_index];
  }

  return random_name;
}

eosio::checksum256 hashit(std::string str) {
  return eosio::sha256(const_cast<char *>(str.c_str()), str.size());
}

uint64_t hash64(const char *buf, size_t len) {
  eosio::checksum256 hash = eosio::sha256(buf, len);
  return *(reinterpret_cast<const uint64_t *>(&hash));
}

uint64_t hash64(const std::string &arg) {
  return hash64(arg.c_str(), arg.size());
}

uint64_t generate() {
    auto size = eosio::transaction_size();
    char buf[size];
    uint32_t read = eosio::read_transaction(buf, size);
    eosio::check(size == read, "read_transaction failed");

    uint64_t seed = eosio::current_time_point().sec_since_epoch();
    for (int i = 0; i < size; i++) {
        seed ^= ((uint64_t)buf[i] << (i % 8));
    }

    return seed;
}