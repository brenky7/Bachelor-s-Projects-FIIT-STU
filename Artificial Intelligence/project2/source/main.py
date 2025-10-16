import lib
import random
import matplotlib.pyplot as plt

playing_field_size = None
player_location = None
treasure_locations = None
with open('game.txt', 'r') as file:
    for line in file:
        name, value = line.strip().split(' = ')
        value = eval(value)
        if name == 'grid_size':
            playing_field_size = value
        elif name == 'player_position':
            player_location = value
        elif name == 'treasure_locations':
            treasure_locations = value

mutation_chance = 0.001
generation_size = 50
new_blood = random.randint(int(generation_size/4), int(2*(generation_size/3)))
elite = random.randint(3, 10)
max_generations = random.randint(6, 10)*100
tournament_size = random.randint(2, int(generation_size/4))
choosing_method = random.choice(["turnaj", "ruleta"])
generation = []
new_generation = []
playing_field = lib.reset_playing_field(playing_field_size, treasure_locations, player_location)

switch = input("Ak chcete manualne nastavit parametre zadajte 'm', ak chcete automaticky test, zadajte hocico ine ")
if switch == 'm':
    generation_size = int(input("Zadajte pocet ludi v jednej generacii "))
    new_blood = int(input("Zadajte pocet novej krvi v kazdej novej generacii "))
    mutation_chance = float(input("Zadajte pravdepodobnost mutacie "))
    max_generations = int(input("Zadajte maximalny pocet generacii  "))
    choosing_method = input("Zadajte metodu vyberania rodicov: ruleta / turnaj  ")
    elite = int(input("Zadajte pocet elitnych jedincov "))
    if choosing_method != "ruleta" and choosing_method != "turnaj" and choosing_method != "1" and choosing_method != "2":
        print("Zly vstup")
        exit(0)
    if choosing_method == "turnaj" or choosing_method == "2":
        tournament_size = int(input("Zadajte velkost turnaja "))

program_counter = 0
opakovania = 0
path = ""
switch2 = ''
y = []
x = []
while True:
    if len(generation) == 0:
        for i in range(generation_size):
            dna = lib.create_dna()
            path = lib.virtual_machine(dna)
            generation.append((dna, lib.game(path, playing_field_size, playing_field)))
            playing_field = lib.reset_playing_field(playing_field_size, treasure_locations, player_location)
        program_counter += 1
        continue
    output = lib.solution(generation, program_counter, int(max_generations), opakovania)
    y.append(output[1])
    if output[0] == 1:
        break
    if program_counter > int(max_generations):
        switch2 = input("Nenasiel sa sampion. Chcete pokracovat? y/n ")
        if switch2 == 'n':
            exit(0)
        elif switch2 == 'y':
            opakovania += 1
            program_counter = 0
            continue
        else:
            print("Zly vstup")
            exit(0)
    generation = sorted(generation, key=lambda u: u[1][0], reverse=True)
    elite_individuals = []
    if elite > 0:
        for p in range(elite):
            elite_individuals.append(generation[p])
    parent1 = ()
    parent2 = ()
    if choosing_method == "ruleta" or choosing_method == "1":
        parent1 = lib.roulette(generation)
        parent2 = lib.roulette(generation)
        while parent2 == parent1:
            parent2 = lib.roulette(generation)
    elif choosing_method == "turnaj" or choosing_method == "2":
        parent1 = lib.tournament(generation, tournament_size)
        parent2 = lib.tournament(generation, tournament_size)
        while parent2 == parent1:
            parent2 = lib.tournament(generation, tournament_size)
    while len(new_generation) < generation_size - new_blood - elite:
        new_generation.append(lib.order_cross(parent1[0], parent2[0]))
    for k in range(new_blood):
        dna = lib.create_dna()
        new_generation.append(dna)
    for m in range(len(new_generation)):
        lib.mutate(new_generation[m], mutation_chance)
        path = lib.virtual_machine(new_generation[m])
        new_generation[m] = (new_generation[m], lib.game(path, playing_field_size, playing_field))
        playing_field = lib.reset_playing_field(playing_field_size, treasure_locations, player_location)
    for n in range(elite):
        new_generation.append(elite_individuals[n])
    generation = new_generation
    new_generation = []
    program_counter += 1

for i in range(len(y)):
    x.append(i)
plt.plot(x, y)
plt.xlabel('Generation number')
plt.ylabel('Max Fitness')
plt.show()
if switch != 'm':
    print("Velkost generacie: " + str(generation_size))
    print("Pocet novej krvi: " + str(new_blood))
    print("Pocet elitnych jedincov: " + str(elite))
    print("Pravdepodobnost mutacie: " + str(mutation_chance))
    print("Maximalny pocet generacii: " + str(max_generations))
    print("Metoda vyberania rodicov: " + choosing_method)
    if choosing_method == "turnaj" or choosing_method == "2":
        print("Velkost turnaja: " + str(tournament_size))


