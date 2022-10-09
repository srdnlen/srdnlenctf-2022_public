# srdnlenctf 2022

## Fair and Balanced (1 solves)

The challenge gives a binary called _client_ which
implements a dungeon exploration game. After 50 turns,
the characters are placed in a combat encounter.
If they win, the user receives a string that they have to
submit to a remote service for the flag.

### Solution

The entirety of the game is controlled by the C standard rand() function
which is seeded from urandom. The binary is bruteforceable, but is time gated
by a number of printing functions and user interaction prompts.

The core vulnerability is the solution accepting any given seed.
Knowing the seed, the player can select a path which will keep their characters safe until the encounter,
but they will still need to beat the enemy.

There are a few approaches to this, depending on the tradeoffs between
search space and complexity of the harness:

- Patching the program to infinitely loop until it finds a solution
- Recreating the rand() rolls in a separate program (eg. in python) and seed more systematically
- A few observations on selecting your path before the encounters: finding a winning window +/-50 rolls ahead
  (selecting a beneficial room instead of empty rooms will advance the RNG by 1 extra step)

The simple solution doesn't require us to reimplement the game logic.
We apply three patches to the given client:
- Remove all calls to usleep() in generateChatter and eventCombat
- Remove call to std::cin (waiting for user input) in the main loop and edit Dungeon::navigate to always enter empty rooms
- Call main() from Dungeon::eventFailure() (or alternatively instead of it)

This binary will crash after a few seconds due to these changes, but it will iterate through a large number of seeds 
in a single execution. After some executions of `./patched_client | grep "how cool" -A 1` you should receive a winning string.
The path is incorrect due to our patches to the navigate switch, but it still gives us a seed 
for which we can win combat if we enter all empty rooms.

Now we have to correct the path to actually enter empty rooms. The search space is 4*49 = 196 attempts, 
so it's bruteforceable by hand if you want. But here is a python script which does it for you.

```python
from ctypes import CDLL

win_combat = "f3e309eebf825c73ssssssssssssssssssssssssssssssssssssssssssssssssssgg"

seed = int(win_combat[:16], 16)

libc = CDLL('libc.so.6')


libc.srand(seed)
libc.rand() # Initial generateChatter

steps = []
directions = 'snwe'
for step in range(49):
    r = libc.rand()
    # (rand() + index of direction chosen) %4= empty room (0)
    directionindex = (4-(r%4))%4
    steps.append(directions[directionindex])
print(win_combat[:16] + ''.join(steps)+'ngg')
```

Below is the more complex solution, scanning for a sliding window where you can win combat, and then generating 
a path string which allows to reach combat at the required iteration. This finds a solution within the first 500 seeds 
in less than a second.

### Exploit

```python
from ctypes import CDLL # to use the rand() funtion of the C library

libc = CDLL("libc.so.6")
UINT_32_MAX = 0xFFFFFFFF

class Character:
    def __init__(self, name, health, stress, ac_value, hit_roll, dmg_roll):
        self.name = name
        self.health = health
        self.stress = stress
        self.ac_value = ac_value
        self.hit_roll = hit_roll
        self.dmg_roll = dmg_roll

class Index:
    def __init__(self, value):
        self.value = value

def get_moves(seed, target_index, rands):
    num_moves = 49 # Last step decreases light before 
    a = (3*num_moves - (target_index-1))/2
    '''
    basically I'm solving an equation system
    a + 3*b = target_index - 1
    a + b = num_moves
    '''
    if (not a.is_integer()) or (a < 0):
        return
    a = int(a)
    b = num_moves - a

    with open('paths.txt', 'a') as f:

        finalstr = format(seed, '016X')
        
        # print(format(seed, '016X'))
        rand_index = 1
        dir_map = ['s', 'n', 'w', 'e']
        for _ in range(a):
            rand = rands[rand_index]; rand_index += 1
            # print(rand)
            for direction in range(4):
                if (rand + direction) % 4 == 0: # the room is empty
                    # print(dir_map[direction])
                    finalstr += dir_map[direction]
                    continue

        for _ in range(b):
            rand = rands[rand_index]; rand_index += 3
            # print(rand)
            for direction in range(4):
                if (rand + direction) % 4 == 3: # eventCurio
                    # print(dir_map[direction])
                    finalstr += dir_map[direction]
                    continue
        
        finalstr += 'ngg\n'
        # print(finalstr)
        f.write(finalstr)


def hitLogic(character1, character2, rands, rand_index):
    v3 = rands[rand_index.value]; rand_index.value+=1
    v14 = v3 % character1.hit_roll + 1
    v15 = ((100 - character1.stress)/100) * v14
    damage = 0
    if character2.ac_value <= v15:
        v4 = rands[rand_index.value]; rand_index.value+=1
        damage = v4 % character1.dmg_roll + 1
        if v15 >= 19:
            damage *= 2
            rand_index.value+=1 # generateChatter
    character2.health -= damage
    return

def eventCombat(rands, rand_index, pr=False):
    dungeon = [Character('R', 24, 0, 13, 20, 8), Character('D', 18, 0, 11, 20, 12)]
    shambler = Character('shambler', 200, 0, 8, 20, 12)
    rand_index = Index(rand_index) # using class to pass by reference and not by value

    rand_index.value += 1 # generateChatter
    while True:
        start_turn_idx = rand_index.value
        both_dead = True
        for character in dungeon:
            if character.health > 0:
                both_dead = False
        if both_dead:
            return False
        if shambler.health <= 0:
            return True
        
        for character in dungeon:
            if character.health > 0:
                hitLogic(character, shambler, rands, rand_index)
        
        if shambler.health > 0: # Completely unnecessary to find a flag, but removes some false negatives.    
            for character in dungeon: # It will attack dead characters too!
                hitLogic(shambler, character, rands, rand_index)
        end_turn_idx = rand_index.value
        if (pr): 
            print(rands[start_turn_idx:end_turn_idx]) # Debug: with a client patched to echo the rand() generations, check if each turn uses these "rolls"

for seed in range(UINT_32_MAX):
    libc.srand(seed) # init random generator
    rands = [libc.rand() for x in range(500)] # extracts first 500 random values for given seed
    for start_fight_idx in range(50, 152): # starts a fight for every possible starting point
        if eventCombat(rands, start_fight_idx): # returns True if successful run, else False
            get_moves(seed, start_fight_idx, rands)
            # eventCombat(rands, start_fight_idx, pr=True) # Debug: print rand() used by each combat turn
            

```
