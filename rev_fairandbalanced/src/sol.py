from ctypes import CDLL

def get_moves(seed, rand_index):

    # All empty rooms
    if rand_index ==51:
        path = ''
        path += format(seed, '016X')
        libc2 = CDLL("libc.so.6")
        libc2.srand(seed)
        libc2.rand()
        dir_map = ['s', 'n', 'w', 'e']
        for i in range(50):
            r = libc2.rand()
            for direction in range(4):
                if (r + direction) % 4 == 0: # the room is empty
                    path += (dir_map[direction])
        path += 'gg'
        print(path)
        exit(0)

    """ There is some error with the dynamic pathfinding. 
    a = (3*50 - rand_index-1)/2
    if a != int(a) or a < 0:
        #print(a)
        return
    a = int(a)
    b = 50 - a
    print(a, b, a + b)
    with open('paths.txt', 'a') as f:

        f.write(format(seed, '016X'))
        libc2 = CDLL("libc.so.6")
        libc2.srand(seed)
        libc2.rand()
        dir_map = ['s', 'n', 'w', 'e']
        for i in range(a):
            r = libc2.rand()
            for direction in range(4):
                if (r + direction) % 4 == 0: # the room is empty
                    f.write(dir_map[direction])
                    continue
        for i in range(b):
            r = libc2.rand()
            libc2.rand() # choice
            libc2.rand() # generateChatter
            for direction in range(4):
                if (r + direction) % 4 == 3: # eventCrate
                    f.write(dir_map[direction])
                    continue
        f.write('gg\n')
    """

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

def hitLogic(character1, character2, rand_index):
    v3 = rands[rand_index.value]; rand_index.value+=1
    v14 = v3 % character1.hit_roll + 1
    v15 = ((100 - character1.stress)/100) * v14
    character2_ac_value = character2.ac_value
    damage = 0
    if character2_ac_value <= v15:
        v4 = rands[rand_index.value]; rand_index.value+=1
        damage = v4 % character1.dmg_roll + 1
        if v15 >= 19:
            damage *= 2
            rand_index.value+=1
    character2.health -= damage
    return

def eventCombat(dungeon, rand_index):
    shambler = Character('shambler', 200, 0, 8, 20, 12)
    rand_index.value += 1 # generateChatter
    while True:
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
                hitLogic(character, shambler, rand_index)
        
        for character in dungeon:
            if character.health > 0:
                hitLogic(shambler, character, rand_index)

libc = CDLL("libc.so.6")
for seed in range(0xFFFFFFFF):
    if seed % 10000 == 0:
        print(hex(seed))
    libc.srand(seed)
    rands = [libc.rand() for x in range(400)]
    
    # There is an error in the pathfinding. For now, search only all-empty-rooms paths.
    # for value in range(51, 152):
    value = 51
    rand_index = Index(value)
    if eventCombat([Character('R', 24, 0, 13, 20, 8), Character('D', 18, 0, 11, 20, 12)], rand_index):
        #print(f'rand_index: {value}\nseed: {hex(seed)}')
        get_moves(seed, value)