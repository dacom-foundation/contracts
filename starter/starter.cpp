#include "starter.hpp"
#include <ctime>
#include <eosio/transaction.hpp>
using namespace eosio;


void starter::hello(std::string message) {
  require_auth(_me);

  print("hello: ", message);
};

