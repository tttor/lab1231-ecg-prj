#!/usr/bin/python

def main():
    file = open("newfile.txt", "w")
    file.write("hello world in the new file\n")
    file.write("and another line\n")

file.close()

if __name__ == '__main__':
    main()
