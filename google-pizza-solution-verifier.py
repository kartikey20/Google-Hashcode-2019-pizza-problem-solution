#!/usr/bin/env python3

"""google-pizza-solution-verifier
Usage:
    google-pizza-solution-verifier.py <pizza_data_file> <solution_to_verify_file>
    google-pizza-solution-verifier.py (-h | --help)
Options:
    -h, --help  Show this screen.
"""
from docopt import docopt
import re


class PizzaException(Exception):
    pass


class PizzaCell:
    def __init__(self, ingredient):
        self.ingredient = ingredient
        self.owner = None


class PizzaSlice:
    def __init__(self, id, r1, c1, r2, c2):
        self.id = id
        self.r1 = r1
        self.c1 = c1
        self.r2 = r2
        self.c2 = c2

    def size(self):
        return (abs(self.r1 - self.r2) + 1) * (abs(self.c1 - self.c2) + 1)


class PizzaTask:
    def __init__(self, min_each, max_total, pizza_mesh):
        self._min_each = min_each
        self._max_total = max_total
        self._pizza_mesh = pizza_mesh

    def verify_solution(self, slices):
        points = 0

        for pizza_slice in slices:
            mushrooms, tomatoes = self._mark_slice_on_mesh(pizza_slice)

            if mushrooms < self._min_each and tomatoes < self._min_each:
                raise PizzaException("Slice {} has not enough ingredient of each type.".format(pizza_slice.id))

            if mushrooms + tomatoes > self._max_total:
                raise PizzaException("Slice {} is too big.".format(pizza_slice.id))

            points += pizza_slice.size()

        self._clear_owners_of_all_cells()
        return points

    def _mark_slice_on_mesh(self, pizza_slice):
        mushrooms = 0
        tomatoes = 0

        for row in range(min(pizza_slice.r1, pizza_slice.r2), max(pizza_slice.r1, pizza_slice.r2) + 1):
            for col in range(min(pizza_slice.c1, pizza_slice.c2), max(pizza_slice.c1, pizza_slice.c2) + 1):
                cell = self._pizza_mesh[row][col]
                if cell.owner is not None:
                    raise PizzaException("Slice {} overlaps with slice {} at row {}, column {}."
                                         .format(pizza_slice.id, cell.owner.id, row, col))

                cell.owner = pizza_slice
                if cell.ingredient == 'M':
                    mushrooms += 1
                elif cell.ingredient == 'T':
                    tomatoes += 1
                else:
                    raise PizzaException("Cell at row {}, column {} has unrecognized ingredient '{}'."
                                        .format(row, col, cell.ingredient))
        return mushrooms, tomatoes

    def _clear_owners_of_all_cells(self):
        for row in self._pizza_mesh:
            for cell in row:
                cell.owner = None


def parse_pizza_data_file(file_name):
    with open(file_name, 'r') as task_file:
        rows, cols, min_each, max_total = [int(i) for i in task_file.readline().split()]
        pizza = []

        for line in task_file:
            row = line.rstrip('\n')

            if len(row) != cols:
                raise PizzaException("Row {} has {} columns instead of {}.".format(len(pizza), len(row), cols))

            if re.search("[^MT]", row):
                raise PizzaException("Row {} has invalid cell.".format(len(pizza)))

            pizza.append([PizzaCell(i) for i in row])

        if len(pizza) != rows:
            raise PizzaException("Pizza has {} rows instead of {}.".format(len(pizza), cols))
        
        return PizzaTask(min_each, max_total, pizza)


def parse_solution_file(file_name):
    with open(file_name, 'r') as f:
        slices_number = int(f.readline())
        pizza_slices = []

        line_counter = 0
        for line in f:
            r1, c1, r2, c2 = [int(i) for i in line.split()]
            pizza_slices.append(PizzaSlice(line_counter, r1, c1, r2, c2))
            line_counter += 1

        if len(pizza_slices) != slices_number:
            raise PizzaException("Declared {} slices but file has {} lines.".format(slices_number, len(pizza_slices)))

        return pizza_slices


arguments = docopt(__doc__)
pizza_task = parse_pizza_data_file(arguments['<pizza_data_file>'])
slices = parse_solution_file(arguments['<solution_to_verify_file>'])
result = pizza_task.verify_solution(slices)
print(result)
