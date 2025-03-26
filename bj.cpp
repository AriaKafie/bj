
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <time.h>

#include "types.h"

#define BET_DEFAULT 100
#define MAX_SPLITS  11

typedef struct {
    Card cards[11];
    int bet;
} Hand;

void clear() { system("clear"); }

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

void action(Card *dealer, Hand *player, Hand *hand, int *money)
{
    if (sum(hand->cards) >= 21) return;
    
    clear();
    std::cout << to_string(*dealer);

    for (Hand *h = player; *h->cards; h++)
        std::cout << to_string(h->cards, h == hand);

    char opt;
    std::cout << "\n(S)tand, (H)it, s(P)lit, (D)ouble: [h]" << std::endl;

    do
    {
        std::string line = get_line();

        opt = line.empty() ? HIT : std::tolower(line[0]);

    } while (std::string("shpd").find(opt) == std::string::npos    ||
             (opt == DOUBLE || opt == SPLIT) && hand->bet > *money ||
             opt == SPLIT && value_of(hand->cards[0]) != value_of(hand->cards[1]));

    switch (opt)
    {
    case HIT:
    {
        Card *c;
        for (c = hand->cards; *c; *c++);
        *c++ = random_card();

        while (sum(hand->cards) < 21)
        {
            clear();
            std::cout << to_string(*dealer);

            for (Hand *h = player; *h->cards; h++)
                std::cout << to_string(h->cards, h == hand);

            char opt;
            std::cout << "\n(S)tand, (H)it: [h]" << std::endl;

            do
            {
                std::string line = get_line();

                opt = line.empty() ? HIT : std::tolower(line[0]);

            } while (opt != STAND && opt != HIT);

            if (opt == HIT)
                *c++ = random_card();

            else break;
        }
        break;
    }
    case SPLIT:
    {
        Hand *new_hand;
        for (new_hand = player; *new_hand->cards; new_hand++);
        
        new_hand->bet      = hand->bet;
        new_hand->cards[0] = hand->cards[1];
        new_hand->cards[1] = random_card();
        hand->cards[1]     = random_card();
        *money            -= new_hand->bet;

        action(dealer, player, hand, money);
        action(dealer, player, new_hand, money);
    break;
    }
    case DOUBLE:
        hand->cards[2] = random_card();
        *money -= hand->bet;
        hand->bet *= 2;
    break;
    }
}

int main()
{
    srand(time(NULL));

    Card dealer[11];
    Hand player[MAX_SPLITS];

    int money = 50000;

    do
    {
        printf("($%d) Bet: [%d]\n", money, BET_DEFAULT);

        memset(dealer, 0, sizeof(dealer));
        memset(player, 0, sizeof(player));
        
        do
        {
            std::string line = get_line();
            
            if (line.empty())
                player->bet = BET_DEFAULT;

            else if (std::istringstream is(line); !(is >> player->bet))
                player->bet = 0;
            
        } while (player->bet < 1 || player->bet > money);
        
        money -= player->bet;
        
        Card *d = dealer, *p = player->cards;
        
        *d++ = random_card(), *d++ = random_card();
        *p++ = random_card(), *p++ = random_card();

        action(dealer, player, player, &money);

        for (Hand *hand = player; *hand->cards; hand++)
        {
            if (sum(hand->cards) <= 21 && !blackjack(hand->cards))
                for (;sum(dealer) < 17; *d++ = random_card());

            switch (result(dealer, hand->cards))
            {
                case BLACKJACK:
                    money += hand->bet + hand->bet * 3 / 2;
                break;
                case WIN:
                    money += hand->bet * 2;
                break;
                case PUSH:
                    money += hand->bet;
                break;
            }
        }

        bool bust = true;
        
        for (Hand *hand = player; *hand->cards; hand++)
            if (sum(hand->cards) <= 21) bust = false;
        
        clear();
        std::cout << (bust ? to_string(*dealer) : to_string(dealer));

        for (Hand *hand = player; *hand->cards; hand++)
            std::cout << to_string(hand->cards);
        std::cout << std::endl;
        
    } while (money);
}
