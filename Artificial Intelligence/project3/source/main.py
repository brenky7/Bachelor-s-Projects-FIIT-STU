import random
import matplotlib.pyplot as plt
import numpy as np


class KNNClassifier:

    # konstruktor
    def __init__(self, k):
        self.k = k
        self.points = {'R': [], 'G': [], 'B': [], 'P': []}

    # prida inicialne body zo zadania
    def add_initial_points(self):
        initial_points = {
            'R': [(-4500, -4400), (-4100, -3000), (-1800, -2400), (-2500, -3400), (-2000, -1400)],
            'G': [(4500, -4400), (4100, -3000), (1800, -2400), (2500, -3400), (2000, -1400)],
            'B': [(-4500, 4400), (-4100, 3000), (-1800, 2400), (-2500, 3400), (-2000, 1400)],
            'P': [(4500, 4400), (4100, 3000), (1800, 2400), (2500, 3400), (2000, 1400)]
        }

        for label, points in initial_points.items():
            self.points[label] = np.array(points)

    # prida bod do pola bodov
    def add_point(self, x, y, label):
        self.points[label] = np.append(self.points[label], np.array([(x, y)]), axis=0)

    # vrati label na zaklade najblizsich bodov
    def classify(self, x, y):
        search_radius = 150.0
        min_points_required = self.k

        while True:
            distances, indexy = self.calculate_distances(x, y, search_radius)
            valid_indexy = [index for index in indexy if index != -1]

            if len(valid_indexy) >= min_points_required:
                k_nearest = distances[:self.k]
                counts = {'R': 0, 'G': 0, 'B': 0, 'P': 0}
                for label, _ in k_nearest:
                    counts[label] += 1

                return max(counts, key=counts.get)

            search_radius *= 2.0

    # vrati najblizsie body zoradene podla ich vzdialenosti
    def calculate_distances(self, x, y, search_radius):
        distances, indexy = [], []

        for label, points in self.points.items():
            if len(points) > 0:
                dist = np.linalg.norm(points - np.array([x, y]), axis=1)
                indexy.extend(np.argwhere(dist <= search_radius).flatten())
                distances.extend(list(zip([label] * len(dist), dist)))

        # zoradi body podla vzdialenosti
        distances.sort(key=lambda x: x[1])
        sorted_indexy = [index for _, index in distances]
        return distances, sorted_indexy


# vygeneruje nahodny bod
def generate_random_point(label, existing_points):
    while True:
        chance = random.randint(1, 100)
        if chance < 100:
            if label == 'R':
                x = random.uniform(-5000, 500)
                y = random.uniform(-5000, 500)
            elif label == 'G':
                x = random.uniform(-500, 5000)
                y = random.uniform(-5000, 500)
            elif label == 'B':
                x = random.uniform(-5000, 500)
                y = random.uniform(-500, 5000)
            else:
                x = random.uniform(-500, 5000)
                y = random.uniform(-500, 5000)
        else:
            x = random.uniform(-5000, 5000)
            y = random.uniform(-5000, 5000)
        new_point = [x, y, label]
        if new_point not in existing_points:
            existing_points.append(new_point)
            return new_point



# vykresli body
def visualize(classifier, title):
    colors = {'R': 'red', 'G': 'green', 'B': 'blue', 'P': 'purple'}
    for label, points in classifier.points.items():
        xs, ys = zip(*points)
        plt.scatter(xs, ys, c=colors[label], label=label)

    plt.title(title)
    plt.savefig(f'plot_{title.replace(" ", "_")}.png')
    plt.show()


def main():
    points = []
    print("Generating points")

    # vygenerovanie jedinecnych bodov
    for i in range(10000):
        label = 'R'
        x, y, _ = generate_random_point(label, points)
        points.append([x, y, label])
        label = 'G'
        x, y, _ = generate_random_point(label, points)
        points.append([x, y, label])
        label = 'B'
        x, y, _ = generate_random_point(label, points)
        points.append([x, y, label])
        label = 'P'
        x, y, _ = generate_random_point(label, points)
        points.append([x, y, label])
    print("Points generated")

    # testovanie klasifikatora
    k_values = [1, 3, 7, 15]
    for k in k_values:

        # inicializacia klasifikatora
        print(f'Running k-NN with k={k}')
        classifier = KNNClassifier(k)
        classifier.add_initial_points()
        reclassified = 0

        # klasifikacia bodov
        for i in range(40000):
            label = classifier.classify(points[i][0], points[i][1])
            if label != points[i][2]:
                reclassified += 1
            classifier.add_point(points[i][0], points[i][1], label)

        # vizualizacia vysledkov
        title = f'k-NN Classification (k={k})'
        visualize(classifier, title)
        print(f"Reclassified {reclassified} points")
        print(f"Accuracy: {100 - reclassified / 40000 * 100}%")


if __name__ == '__main__':
    main()
