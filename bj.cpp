
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <time.h>

#include "types.h"

#define BET_DEFAULT 100

void clear() { /*system("clear");*/ }

std::string get_line()
{
    std::string line;
    std::getline(std::cin, line);
    return line;
}

int sum(Card *hand)
{
    int sum = 0, aces = 0;

    for (Rank r; r = rank_of(*hand); hand++)
    {
        if (r == ACE) aces++;
        
        sum += std::min(int(r), int(TEN));
    }

    for (int i = 0; i < aces; i++)
        if (sum + 10 <= 21) sum += 10;

    return sum;
}

std::string to_string(Card *hand, bool arrow = false)
{
    if (!*hand) return "";
    
    std::stringstream ss;

    for (Card *c = hand; *c; c++)
        ss << "+---+ ";
    ss << "\n";
    for (Card *c = hand; *c; c++)
        ss << "|" << face_string(*c) << "| ";
    ss << "(" << sum(hand) << (arrow ? ") <\n" : ")\n");
    for (Card *c = hand; *c; c++)
        ss << "+---+ ";
    ss << "\n";

    return ss.str();
}

bool blackjack(Card *hand)
{
    int num_cards = 0;

    for (Card *c = hand; *c; c++)
        num_cards++;
    
    return num_cards == 2 && sum(hand) == 21;
}

Outcome result(Card *dealer_hand, Card *player_hand)
{
    int dealer = sum(dealer_hand);
    int player = sum(player_hand);

    if (player > 21) return LOSS;

    if (player == 21)
    {
        return blackjack(player_hand) ? blackjack(dealer_hand) ? PUSH : BLACKJACK
                                      : blackjack(dealer_hand) ? LOSS : dealer == 21 ? PUSH : WIN;
    }

    return dealer >  player ? dealer > 21 ? WIN : LOSS
         : dealer == player ? PUSH              : WIN;
}

void hit_loop(bool do_split, Card (*hand)[11])
{
    Card *player;
    for (player = hand[do_split ? SPLIT : PLAYER]; *player; player++);
    
    while (sum(hand[do_split ? SPLIT : PLAYER]) < 21)
    {
        clear();
        printf("%s%s%s\n", to_string(*hand[DEALER]).c_str(),
               to_string(hand[PLAYER], !do_split && *hand[SPLIT]).c_str(),
               to_string(hand[SPLIT], do_split).c_str());
            
        char opt;
        std::cout << "(S)tand, (H)it: [h]" << std::endl;

        do
        {
            std::string line = get_line();

            opt = line.empty() ? OPT_HIT : line[0];

        } while (opt != OPT_STAND && opt != OPT_HIT);

        if (opt == OPT_HIT)
            *player++ = random_card();

        else break;
    }
}

int main()
{
    srand(time(NULL));

    Card hand[3][11];

    int bet, money = 500;

    do
    {
        printf("($%d) Bet: [%d]\n", money, BET_DEFAULT);

        do
        {
            std::string line = get_line();
            
            if (line.empty())
                bet = BET_DEFAULT;

            else if (std::istringstream is(line); !(is >> bet))
                bet = 0;
            
        } while (bet < 1 || bet > money);
        
        money -= bet;

        memset(hand, 0, sizeof(hand));

        Card *dealer = hand[DEALER], *player = hand[PLAYER], *split = hand[SPLIT];
        
        *dealer++ = random_card(), *dealer++ = random_card();
        *player++ = random_card(), *player++ = random_card();

        if (sum(hand[PLAYER]) < 21)
        {
            clear();
            printf("%s%s\n", to_string(*hand[DEALER]).c_str(), to_string(hand[PLAYER]).c_str());

            char opt;
            std::cout << "(S)tand, (H)it, s(P)lit, (D)ouble: [h]" << std::endl;

            do
            {
                std::string line = get_line();

                opt = line.empty() ? OPT_HIT : line[0];
                
            } while (std::string("shpd").find(opt = std::tolower(opt)) == std::string::npos ||
                     (opt == OPT_DOUBLE || opt == OPT_SPLIT) && bet > money                 ||
                     opt == OPT_SPLIT && value_of(*(player - 2)) != value_of(*(player - 1)));

            switch (opt)
            {
                case OPT_HIT:
                    *player++ = random_card();
                    hit_loop(false, hand);
                break;
                case OPT_SPLIT:
                    money        -= bet;
                    *split++      = *(player - 1);
                    *split++      = random_card();
                    *(player - 1) = random_card();
                    hit_loop(false, hand);
                    hit_loop(true,  hand);
                break;
                case OPT_DOUBLE:
                    *player++ = random_card();
                    money -= bet;
                    bet *= 2;
                break;
            }
        }

        for (Card *h, i = PLAYER; i <= SPLIT && *(h = hand[i]); i++)
        {
            if (sum(h) <= 21 && !blackjack(h))
                for (;sum(hand[DEALER]) < 17; *dealer++ = random_card());

            switch (result(hand[DEALER], h))
            {
                case BLACKJACK:
                    money += bet + bet * 3 / 2;
                break;
                case WIN:
                    money += bet * 2;
                break;
                case PUSH:
                    money += bet;
                break;
            }
        }

        clear();
        printf("%s%s%s\n", sum(hand[PLAYER]) > 21 && (!*hand[SPLIT] || sum(hand[SPLIT]) > 21) ? to_string(*hand[DEALER]).c_str() : to_string(hand[DEALER]).c_str(),
                           to_string(hand[PLAYER]).c_str(),
                           to_string(hand[SPLIT] ).c_str());
        
    } while (money);
}
