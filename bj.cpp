
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

std::string to_string(Card *hand)
{
    std::stringstream ss;

    for (Card *c = hand; *c; c++)
        ss << "+---+ ";
    ss << "\n";
    for (Card *c = hand; *c; c++)
        ss << "|" << to_string(*c) << "| ";
    ss << "(" << sum(hand) << ")\n";
    for (Card *c = hand; *c; c++)
        ss << "+---+ ";

    return ss.str();
}

bool blackjack(Card *hand)
{
    int num_cards = 0;

    for (Card *c = hand; *c; c++)
        num_cards++;
    
    return num_cards == 2 && sum(hand) == 21;
}

void refresh(Card *dealer, Card *player, bool hide = false)
{
    system("clear");
    
    if (hide || sum(player) > 21)
    {
        std::cout << "+---+\n|" << to_string(*dealer) << "|\n+---+\n";
    }
    else
    {
        std::cout << to_string(dealer) << "\n";
    }

    std::cout << to_string(player) << "\n" << std::endl;
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

void action(Card *dealer, Card *player)
{
    for (Card *p = player + 2; sum(player) < 21;)
    {
        refresh(dealer, player, true);

        char opt;
        std::cout << "(S)tand, (H)it: [h]" << std::endl;

        do
        {
            std::string line = get_line();

            if (line.empty()) opt = HIT;

            else if (std::istringstream is(line); !(is >> opt))
                opt = NONE;
            
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

        Card *d = hand[DEALER], *p = hand[PLAYER], *s = split;
        
        *d++ = random_card(), *d++ = random_card();
        *p++ = random_card(), *p++ = random_card();

        for (Option opt; sum(hand[PLAYER]) < 21;)
        {
            refresh(hand[DEALER], hand[PLAYER], true);

            std::cout << "(S)tand, (H)it, (D)ouble: [h]" << std::endl;
            std::string in = get_line();

            switch (opt = in.size() ? in[0] : HIT)
            {
                case HIT:
                    *p++ = random_card();
                break;
                case DOUBLE:
                    *p++ = random_card();
                    money -= bet;
                    bet *= 2;
                break;
            }

            if (opt == DOUBLE || opt == STAND) break;
        }

        /*refresh(hand[DEALER], hand[PLAYER], true);
        
        if (rank_of(hand[PLAYER][0]) == rank_of(hand[PLAYER][1]) && money >= bet)
        {
            std::cout << "Split: y/[n]" << std::endl;
            
            do
            {
                std::string in;
                std::getline(std::cin, in);
                std::istringstream is(in);
                is >> in;
            } while (!(in == "y"));
        }
        
        std::cout << "(S)tand, (H)it, s(P)lit, (D)ouble down: [h]" << std::endl;
        std::getline(std::cin, in);

        Option opt = in.size() ? std::tolower(in[0]) : HIT;

        if (opt == SPLIT && rank_of(player[0][0]) == rank_of(player[0][1]))
        {
            *s++     = *(p - 1);
            *s++     = random_card();
            *(p - 1) = random_card();

            action(player[0]);
            action(player[1]);
        }
        
        action(player[0], opt);*/

        if (sum(hand[PLAYER]) <= 21 && !blackjack(hand[PLAYER]))
            for (;sum(hand[DEALER]) < 17; *d++ = random_card());

        switch (result(hand[DEALER], hand[PLAYER]))
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
        
        refresh(hand[DEALER], hand[PLAYER]);
        
    } while (money);
}
