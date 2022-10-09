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
            
