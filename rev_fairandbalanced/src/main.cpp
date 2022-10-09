#include <cstdio>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include <unistd.h>
#include <sys/random.h>

/*
* 
* /mnt/c/Users/M/source/repos/Darkest
* g++ -o test2 main.cpp
* 
* 
* Seed che funziona selezionando le stanze vuote ogni volta: 

        seed = -519944808;

* Percorso per trovare le stanze vuote con questo seed:
* sneseswwnensennnswennnnesweennenwwnswwwwnewesesss
* 
* stringa totale: ffffffffe1024598sneseswwnensennnswennnnesweennenwwnswwwwnewesessssgg
*/


enum class Item {GEMS, ANTIDOTE, SHOVEL, MONEY};

enum class Corridor {EMPTY, CURIO, TRAP, CRATE};

enum class ChatterEvent {ENTER, TRAP, LOOT, COMBATSTART, CRIT, FAILURE, SUCCESS};

std::map <ChatterEvent, std::vector<std::string>> ChatterDB = {
    { ChatterEvent::ENTER, {"A devil walks these halls... only the mad or the desperate go in search of him.", "There is method in the wild corruption here. It bears a form both wretched and malevolent.", "Corruption has soaked the soil, sapping all good life from these groves - let us burn out this evil.", "They breed quickly down there in the dark, but perhaps we can slay them even faster."}},
    { ChatterEvent::TRAP, {"Watch your step.", "Ambushed by foul invention!","Curious is the trap-maker's art... his efficacy unwitnessed by his own eyes."}},
    { ChatterEvent::LOOT, {"Glittering gold, trinkets and baubles - paid for in blood.", "Packs laden with loot are often low on supplies.", "Impressive haul! If you value such things.", "Ornaments neatly ordered, lovingly admired."}},
    { ChatterEvent::COMBATSTART, {"A star - spawned horror!", "Behold the infinite malignity of the stars!"}},
    { ChatterEvent::FAILURE, {"Regroup. Reassemble. Evil is timeless, after all.", "Great adversity has a beauty - it is the fire that tempers."}},
    { ChatterEvent::SUCCESS, {"The great Ruins belong to us, and we will find whatever secrets they hold.", "Driving out corruption is an endless battle, but one that must be fought.", "It could be dismissed as a fever dream, if not for the corpses."}},
    { ChatterEvent::CRIT , {"A powerful blow!", "Ringing ears, blurred vision - the end approaches...", "Dazed, reeling, about to break...", "The ground quakes!"}} ,
};

// Client/checker build diffs
#define CLIENT 1

void win() {

    if(!getenv("FLAG")) {
        printf("Error reading flag env. If you see this in the CTF, call an admin.");
        exit(1);
    } else{
        printf("%s", getenv("FLAG"));
    }
    
    /* File based one
    int c;
    FILE* file;
    file = fopen("./flag.txt", "r");
    if (file) {
        while ((c = getc(file)) != EOF)
            putchar(c);
        fclose(file);
    }
    else {
        printf("Error reading flag file. If you see this in the CTF, call an admin.");
    }
    */
}

void delayedTyping(std::string s) {
    if (CLIENT) std::cout << "<< ";
    for (char& c : s) {
        if (CLIENT) usleep(30000);
        if (CLIENT) std::cout << c;
    }
    if (CLIENT) std::cout << ">> " << std::endl;
}

int generateChatter(ChatterEvent ce) {
    auto quoteList = ChatterDB[ce];
    int quoteIdx = rand() % quoteList.size();
    delayedTyping(quoteList[quoteIdx]);
    return quoteIdx;
}

int clamp(int n, int lower, int upper) {
    return n <= lower ? lower : n >= upper ? upper : n;
}

class Character {
private:
    std::string name;
    int maxHP;
    int HP;
    int stress;
    int AC;
    int hitRoll;
    int dmgRoll;
public:
    Character(std::string Oname, int OHP, int OAC, int OhitRoll, int OdmgRoll) : stress(0) {
        name = Oname;
        HP = maxHP = OHP;
        AC = OAC;
        hitRoll = OhitRoll;
        dmgRoll = OdmgRoll;
    }

    std::string getName() {
        return name;
    }
    int getHP() {
        return HP;
    }
    int getStress() {
        return stress;
    }
    int getAC() {
        return AC;
    }
    int getHitRoll() {
        return hitRoll;
    }
    int getDmgRoll() {
        return dmgRoll;
    }

    int adjustHP(int offset) { // This should be negative for damage
        HP = clamp(HP+offset, 0, maxHP);
        return HP;
    }

    int adjustStress(int offset) { // This should be negative for stress heal
        stress = clamp(stress + offset, 0, 100);
        return stress;
    }

};

class Dungeon {
private:
    int lightLevel;
    int lightDecay;
    std::vector<Character> party;
    std::vector<Item> inventory;
    bool gameOverFlag;
    std::string state;
public:
    Dungeon(std::vector<Character> p) : inventory(10, Item::MONEY), gameOverFlag(false) {

        long long int seed = 0; 
        if (CLIENT) {
            std::cout << "Generating skill issue..." << std::endl;
            getrandom(&seed, sizeof(seed), 0); // leggi da urandom(con non abbastanza byte da non fare bruteforce)
            //seed = -519944808; // TODO DEBUG DISABLE

            srand(seed);
            lightLevel = 100;
            lightDecay = -2;
            party = p;
            std::cout << "Dungeon balanced." << std::endl;

            std::stringstream sstream;
            sstream << std::setfill('0') << std::setw(sizeof(seed) * 2) << std::hex << seed;
            state = sstream.str();
        } else {
            // Checker setup. Read first 2*len seed and turn its hex into the run seed.
            long long unsigned int x;
            char hexbuf[2 * sizeof(seed)+1];

            std::cin.getline(hexbuf,sizeof(hexbuf));
            std::cin.clear();

            std::stringstream ss;
            ss << std::hex << hexbuf;
            ss >> x;
            seed = (long long int)x;

            std::cout << "SEED " << seed << std::endl; // TODO DEBUG
            srand(seed);
            lightLevel = 100;
            lightDecay = -2;
            party = p;
        }

        generateChatter(ChatterEvent::ENTER);
    }

    int adjustLight(int offset) {
        lightLevel = clamp(lightLevel+offset, 0, 100);
        return lightLevel;
    }

    int gameOver() {
        return gameOverFlag;
    }

    void eventTrap(){
        if (rand() % 2) {
            if (CLIENT) std::cout << "The party disarms a trap laying in wait." << std::endl;
            for (Character c : party) {
                int bonus = 1 + rand() % 4;
                if (CLIENT) std::cout << c.getName() << " is soothed for " << bonus << " stress." << std::endl;
                c.adjustStress(-bonus);
            }
        }
        else {
            if (CLIENT) std::cout << "The party fails to disarm a trap laying in wait!" << std::endl;
            for (Character c : party) {
                int bonus = 10 + rand() % 4;
                if (CLIENT) std::cout << c.getName() << " is damaged for " << bonus << " stress." << std::endl;
                c.adjustStress(bonus);
            }
            generateChatter(ChatterEvent::TRAP);
        }
    }

    void eventCurio(){
        if (CLIENT) std::cout << "A pile of books captures your attention. ";
        if (rand() % 2) {
            if (CLIENT) std::cout << "Such useful information will surely help your journey." << std::endl;
            for (Character c : party) {
                int bonus = 1 + rand() % 4;
                if (CLIENT) std::cout << c.getName() << " is soothed for " << bonus << " stress." << std::endl;
                c.adjustStress(-bonus);
            }
        }
        else {
            if (CLIENT) std::cout << "The secrets within are too much for you to bear." << std::endl;
            for (Character c : party) {
                int bonus = 10 + rand() % 4;
                if (CLIENT) std::cout << c.getName() << " is damaged for " << bonus << " stress." << std::endl;
                c.adjustStress(bonus);
            }
        }

    }

    void eventCrate(){
        if (CLIENT) std::cout << "You encounter a crate. Upon opening it, you discover ";
        auto item = static_cast<Item> (rand() % 4);
        switch (item) {
        case Item::ANTIDOTE:
            if (CLIENT) std::cout << "an antidote." << std::endl;
            break;
        case Item::SHOVEL:
            if (CLIENT) std::cout << "a shovel." << std::endl;
            break;
        case Item::MONEY:
            if (CLIENT) std::cout << "a stack of gold coins." << std::endl;
            break;
        case Item::GEMS:
        default:
            if (CLIENT) std::cout << "a pouch full of precious gems." << std::endl;
            break;
        }
        if (CLIENT) std::cout << "However, your inventory is already full." << std::endl;
        generateChatter(ChatterEvent::LOOT);
    }

    void hitLogic(Character* attacker, Character* victim){
        // Hit roll
        double hitRoll = (rand() % attacker->getHitRoll()) + 1;
        hitRoll *= (100 - attacker->getStress()) / 100;

        if (hitRoll >= victim->getAC()) {
            // Damage roll. We don't have damage reduction mechanisms so we just apply the damage (with possible critical hits.)
            int dmgRoll = (rand() % attacker->getDmgRoll()) + 1;
            if (hitRoll >= 19) {
                dmgRoll *= 2;
                if (CLIENT) std::cout << "Critical Hit!" << std::endl;
                generateChatter(ChatterEvent::CRIT);
            }
            if (CLIENT) std::cout << victim->getName() << " is hit for " << dmgRoll << " HP." << std::endl;
            victim->adjustHP(-dmgRoll);
        }
        else {
            if (CLIENT) std::cout << "The attack misses. " << std::endl;
        }
    }

    int eventCombat(){
        auto s = Character("The Shambler", 200, 8, 20, 12);

        if (CLIENT) std::cout << "The Shambler appears!" << std::endl;
        generateChatter(ChatterEvent::COMBATSTART);

        // Turn loop
        int turnctr = 1;
        while (true) {
            if (CLIENT) std::cout << std::endl << "TURN " << turnctr++ << std::endl << std::endl;

            if (CLIENT) std::cout << "The Shambler has "<< s.getHP() << " HP left." << std::endl;

            // End combat checks.
            bool wiped = true;
            for (Character c : party) {
                if (c.getHP() > 0)
                    wiped = false;
                if (CLIENT) std::cout << c.getName() << " has " << c.getHP() << " HP left." << std::endl;
            }
            if (wiped) {
                // Failure State: all characters are dead.
                return eventFailure();
            }

            if (s.getHP() <= 0) {
                // Success state: the shambler is dead with at least one party member alive.
                return eventSuccess();
            }

            // Party attacks
            for (auto &c : party) {
                if (c.getHP() > 0) {
                    if (CLIENT) std::cout << c.getName() << " attacks! " << std::endl;
                    hitLogic(&c, &s);
                }
                else {
                    if (CLIENT) std::cout << c.getName() << " is dead." << std::endl;
                }
                if (CLIENT) usleep(500000);
            }

            if (s.getHP() > 0) {
                for (auto& c : party) {
                    if (CLIENT) std::cout << s.getName() << " attacks! " << std::endl;
                    hitLogic(&s, &c);
                }
            }
            else {
                if (CLIENT) std::cout << s.getName() << " is dead." << std::endl;
            }
            if (CLIENT) usleep(1000000);
        }
    }

    int eventFailure() {
        std::cout << "Your party is dead. Better luck next time!" << std::endl;
        generateChatter(ChatterEvent::FAILURE);
        gameOverFlag = true;
        return 2;
    }

    int eventSuccess() {
        std::cout << "You have killed the Shambler. Good work!" << std::endl;
        generateChatter(ChatterEvent::SUCCESS);
        if (CLIENT) {
            std::cout << "Want to show your friends how cool you are? send them this string!" << std::endl << state << "gg" << std::endl;
        }
        else {
            std::cout << "Totally awesome bro. That's like a one in a million run." << std::endl;
            win();
        }
        gameOverFlag = true;
        return 1;
    }

    int navigate(int direction = 0) { // "Move" to different room. Generate new room event.

        adjustLight(lightDecay);


        state += "snwe"[direction]; 

        if (lightLevel > 0) {
            Corridor event = static_cast<Corridor> ((rand() + direction)% 4);
            //event = Corridor::EMPTY; // TODO DEBUG REMOVE
            switch (event) {
            case Corridor::TRAP:
                eventTrap();
                break;
            case Corridor::CURIO:
                eventCurio();
                break;
            case Corridor::CRATE:
                eventCrate();
                break;
            case Corridor::EMPTY:
            default:
                if (CLIENT) std::cout << "The room is empty." << std::endl;
                break;
            }
            return 0;
        }
        else {
            return eventCombat();
        }
    }
};

int gameloop() {
    delayedTyping("You will arrive along the old road. \n  It winds with a troubling, serpent-like suggestion through the corrupted countryside. \n  Leading only, I fear, to ever more tenebrous places. ");
    auto r = Character("Reynauld", 24, 13, 20, 8);
    auto d = Character("Dismas", 18, 11, 20, 12);
    std::vector<Character> party{ r, d };
    if (!CLIENT) {
        std::string winState;
        std::cout << "Send me your super cool winning run NOW " << std::endl;
    }
    auto game = Dungeon(party);
    

    int gamestate = 0;

    while (!game.gameOver()) {
        if (CLIENT) std::cout << "Which way do you want to proceed? [N]orth/[E]ast/[S]outh/[W]est" << std::endl;
        char c;
        std::cin >> c;
        std::cin.clear();
        std::cin.ignore(std::cin.rdbuf()->in_avail(), '\n');
        //std::cout << "DEBUG : DIRECTION " << c << std::endl;
        switch (c) {
        case 'N':
        case 'n':
            gamestate = game.navigate(1);
            break;
        case 'E':
        case 'e':
            gamestate = game.navigate(3);
            break;
        case 'W':
        case 'w':
            gamestate = game.navigate(2);
            break;
        case 'S':
        case 's':
        default:
            gamestate = game.navigate(0);
            break;
        }
    }
    return gamestate;
}

int main()
{
    if (CLIENT) std::cout.setf(std::ios::unitbuf);
    return gameloop(); // TODO DEBUG
}