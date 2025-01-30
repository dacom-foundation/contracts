
using namespace eosio;
#include <eosio/eosio.hpp>

/**
 * @brief Эффект удаления кооперативного участка
 * @param coopname Имя кооператива
 * @param braname Имя кооперативного участка
 */
void soviet::deletebranch(eosio::name coopname, eosio::name braname) {
    require_auth(_branch);

    participants_index participants(_soviet, coopname.value);
    auto idx = participants.get_index<"bybranch"_n>();

    for (auto itr = idx.lower_bound(braname.value); itr != idx.end() && itr->by_braname() == braname.value; ) {
        if (itr->braname.has_value() && itr->braname.value() == braname) {
            itr = idx.erase(itr); // erase() возвращает следующий итератор
        } else {
            ++itr;
        }
    }
}
