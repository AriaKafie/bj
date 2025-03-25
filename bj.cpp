
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <time.h>

#include "types.h"

constexpr int BET_DEFAULT = 100;

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
    if (!hand) return "";
    
    std::stringstream ss;

    for (Card *c = hand; *c; c++)
        ss << "+---+ ";
    ss << "\n";
    for (Card *c = hand; *c; c++)
        ss << "|" << to_string(*c) << "| ";
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

void refresh(Card *dealer, Card *player, bool hide, Card *split = nullptr)
{
    system("clear");
    
    if (hide || sum(player) > 21)
    {
        std::cout << "+---+\n|" << to_string(*dealer) << "|\n+---+\n";
    }
    else
    {
        std::cout << to_string(dealer);
    }

    std::cout << to_string(player);

    if (split)
        std::cout << to_string(split);

    std::cout << std::endl;
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

void hit_loop(bool do_split, Card (*hand)[11], Card *split = nullptr)
{
    Card *player = do_split ? split : hand[PLAYER], *p = player;
    for (;*p; p++);
    
    while (sum(player) < 21)
    {
        system("clear");
        printf("+---+\n|%s|\n+---+\n%s%s\n", to_string(*(hand[DEALER])).c_str(),
               to_string(hand[PLAYER], !do_split && split).c_str(),
               to_string(split, do_split).c_str());
            
        char opt;
        std::cout << "(S)tand, (H)it: [h]" << std::endl;

        do
        {
            std::string line = get_line();

            opt = line.empty() ? HIT : line[0];

        } while (opt != STAND && opt != HIT);

        if (opt == HIT)
            *p++ = random_card();

        else break;
    }
}

int main()
{
    srand(time(NULL));

    Card hand[2][11], split[11];

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

        memset(hand,  0, sizeof(hand));
        memset(split, 0, sizeof(split));

        Card *d = hand[DEALER], *p = hand[PLAYER], *s = nullptr;
        
        *d++ = random_card(), *d++ = random_card();
        *p++ = random_card(), *p++ = random_card();

        if (sum(hand[PLAYER]) < 21)
        {
            refresh(hand[DEALER], hand[PLAYER], true);

            char opt;
            std::cout << "(S)tand, (H)it, s(P)lit, (D)ouble: [h]" << std::endl;

            do
            {
                std::string line = get_line();

                opt = line.empty() ? HIT : line[0];
                
            } while (std::string("shpd").find(opt = std::tolower(opt)) == std::string::npos ||
                     (opt == DOUBLE || opt == SPLIT) && bet > money                         ||
                     opt == SPLIT && rank_of(*(p - 2)) != rank_of(*(p - 1)));

            switch (opt)
            {
                case HIT:
                    *p++ = random_card();
                    hit_loop(false, hand);
                break;
                case SPLIT:
                    money   -= bet;
                    s        = split;
                    *s++     = *(p - 1);
                    *s++     = random_card();
                    *(p - 1) = random_card();
                    hit_loop(false, hand, split);
                    hit_loop(true,  hand, split);
                break;
                case DOUBLE:
                    *p++ = random_card();
                    money -= bet;
                    bet *= 2;
                break;
            }
        }

        for (Card *hands[3] = { hand[PLAYER], s ? split : s, nullptr }, i = 0, *h; h = hands[i]; i++)
        {
            if (sum(h) <= 21 && !blackjack(h))
                for (;sum(hand[DEALER]) < 17; *d++ = random_card());

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
            }
        }
        
        refresh(hand[DEALER], hand[PLAYER], false, s ? split : nullptr);
        
    } while (money);
}
