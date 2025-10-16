def create_dna():
    import random
    dna = []
    for i in range(64):
        gene = random.randint(0, 255)
        dna.append(bin(gene)[2:].zfill(8))
    return dna


def mutate(dna, mutation_chance):
    import random
    mutated_dna = []
    for gene in dna:
        gene_list = list(gene)
        for index in range(8):
            if random.random() < mutation_chance:
                if gene_list[index] == '0':
                    gene_list[index] = '1'
                elif gene_list[index] == '1':
                    gene_list[index] = '0'
        mutated_dna.append(''.join(gene_list))
    return mutated_dna


def order_cross(parent1, parent2):
    import random
    index1 = random.randint(0, 63)
    index2 = random.randint(45, 60)
    child = parent2[:index1] + parent1[index1:index2] + parent2[index2:]
    return child


def reset_playing_field(playing_field_size, treasure_locations, player_location):
    playing_field = [[' ' for _ in range(playing_field_size)] for _ in range(playing_field_size)]
    for location in treasure_locations:
        playing_field[location[0]][location[1]] = 't'
    playing_field[player_location[0]][player_location[1]] = 'p'
    return playing_field


def find_player(playing_field_size, playing_field):
    for i in range(playing_field_size):
        for j in range(playing_field_size):
            if playing_field[i][j] == 'p':
                return i, j


def create_move(suffix):
    vypis = ''
    if suffix == "00":
        vypis += 'H'
    elif suffix == "01":
        vypis += 'D'
    elif suffix == "10":
        vypis += 'L'
    elif suffix == "11":
        vypis += 'P'
    return vypis


def roulette(generation):
    import random
    fitness_sum = 0.0
    for individual in generation:
        fitness_sum += individual[1][0]
    random_number = random.uniform(0.0, fitness_sum)
    index = 0
    for individual in generation:
        index += individual[1][0]
        if index >= random_number:
            return individual


def tournament(generation, size):
    import random
    contenders = []
    for i in range(size):
        contenders.append(generation[random.randint(0, len(generation) - 1)])
    winner_fitness = max_fitness(contenders)
    for individual in contenders:
        if individual[1][0] == winner_fitness:
            return individual


def solution(generation, generation_count, max_generations, k):
    if len(generation) == 0:
        return 0, 0
    maximum = max_fitness(generation)
    if maximum >= 5:
        generation_count += k*max_generations
        print("Fitness sampiona: " + str(maximum) + " v generacii: " + str(generation_count))
        for individual in generation:
            if individual[1][0] == maximum:
                print("DNA sampiona: ")
                print(individual[0])
                print("Path sampiona: ")
                print(individual[1][1])
        return 1, maximum
    else:
        print("Maximalna fitness: " + str(maximum))
        return 0, maximum


def max_fitness(generation):
    max_value = 0.0
    for individual in generation:
        if individual[1][0] > max_value:
            max_value = individual[1][0]
    return max_value


def virtual_machine(dna):
    index = 0
    counter = 0
    path = ""
    while index < len(dna) and counter < 500:
        binary_prefix = dna[index][:2]
        binary_address = dna[index][2:]
        decimal_address = int(binary_address, 2)
        if binary_prefix in ("00", "01"):
            if binary_prefix == "00":
                temp = int(dna[decimal_address], 2)
                if temp == 255:
                    dna[decimal_address] = bin(0)[2:].zfill(8)
                else:
                    dna[decimal_address] = bin(temp + 1)[2:].zfill(8)
                path += "0"
                index += 1
            elif binary_prefix == "01":
                temp = int(dna[decimal_address], 2)
                if temp == 0:
                    dna[decimal_address] = bin(255)[2:].zfill(8)
                else:
                    dna[decimal_address] = bin(temp - 1)[2:].zfill(8)
                path += "0"
                index += 1
        elif binary_prefix == "10":
            index = int(dna[index][2:], 2)
            path += "0"
        elif binary_prefix == "11":
            path += create_move(dna[decimal_address][6:])
            index += 1
        counter += 1
    return path


def game(path, playing_field_size, playing_field):
    treasures = 0.5
    instruction_counter = 0
    final_path = ""
    fitness = 0
    for pohyb in path:
        if treasures > 5:
            break
        suradnice_hraca = find_player(playing_field_size, playing_field)
        if pohyb == '0':
            instruction_counter += 1
            continue
        if pohyb == 'H':
            final_path += 'H'
            if suradnice_hraca[0] == 0:
                break
            else:
                if playing_field[suradnice_hraca[0] - 1][suradnice_hraca[1]] == 't':
                    treasures += 1
                playing_field[suradnice_hraca[0] - 1][suradnice_hraca[1]] = 'p'
                playing_field[suradnice_hraca[0]][suradnice_hraca[1]] = ' '
        elif pohyb == 'D':
            final_path += 'D'
            if suradnice_hraca[0] == playing_field_size - 1:
                break
            else:
                if playing_field[suradnice_hraca[0] + 1][suradnice_hraca[1]] == 't':
                    treasures += 1
                playing_field[suradnice_hraca[0] + 1][suradnice_hraca[1]] = 'p'
                playing_field[suradnice_hraca[0]][suradnice_hraca[1]] = ' '
        elif pohyb == 'L':
            final_path += 'L'
            if suradnice_hraca[1] == 0:
                break
            else:
                if playing_field[suradnice_hraca[0]][suradnice_hraca[1] - 1] == 't':
                    treasures += 1
                playing_field[suradnice_hraca[0]][suradnice_hraca[1] - 1] = 'p'
                playing_field[suradnice_hraca[0]][suradnice_hraca[1]] = ' '
        elif pohyb == 'P':
            final_path += 'P'
            if suradnice_hraca[1] == playing_field_size - 1:
                break
            else:
                if playing_field[suradnice_hraca[0]][suradnice_hraca[1] + 1] == 't':
                    treasures += 1
                playing_field[suradnice_hraca[0]][suradnice_hraca[1] + 1] = 'p'
                playing_field[suradnice_hraca[0]][suradnice_hraca[1]] = ' '
    instruction_counter += len(final_path)
    fitness += treasures - instruction_counter*0.001
    return [fitness, final_path]



