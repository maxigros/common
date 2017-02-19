#!/usr/bin/python3
# password generator
#  usage: ./passgen.py <quantity> <size>

import sys
import random

symbols = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`_-.?"


def main():
    if len(sys.argv) in (1, 2):
        print("usage: {0} <quantity of passwords> <password size>".format(sys.argv[0]))
        sys.exit()

    try:
        s = int(sys.argv[2])
        del s
    except ValueError:
        print("size must be integer")
        sys.exit()

    size = int(sys.argv[2])

    try:
        for i in range(int(sys.argv[1])):
            password = generate_password(size)
            print("{0}".format(password))
    except ValueError:
        print("quantity must be integer")
        sys.exit()


def generate_password(size):
    password = []
    random.seed()
    symbols_list = list(symbols)
    random.shuffle(symbols_list)
    for i in range(size):
        password.append(random.choice(symbols_list))
    password_str = "".join(password)
    return password_str

main()
