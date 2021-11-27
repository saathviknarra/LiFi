# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.


def converting_to_binray(input):
    # Use a breakpoint in the code line below to debug your script.
    encoding = [
        0b01111, 0b10010, 0b00101, 0b10101,
        0b01010, 0b11010, 0b01110, 0b11110,
        0b01001, 0b11001, 0b01101, 0b11101,
        0b01011, 0b11011, 0b00111, 0b10111
        ]
    for x in range(len(input)):
        temp = format(ord(input[x]), "b")
#        print(type(temp))
        padding = 8 - len(temp)
        temp = ("0" * padding) + temp
#        print((encoding[0]))
        firsthalf = temp[:4]
        secondhalf = temp[-4:]
        firsthalf = int(firsthalf, 2)
        secondhalf = int(secondhalf, 2)
        firsthalf = encoding[firsthalf]
        secondhalf = encoding[secondhalf]
        firsthalf = format(firsthalf, "b")
        secondhalf = format(secondhalf, "b")
        padding = 5 - len(firsthalf)
        firsthalf = ("0" * padding) + firsthalf
        padding = 5 - len(secondhalf)
        secondhalf = ("0" * padding) + secondhalf
        print(firsthalf, end=" ")
        print(secondhalf, end=" ")
#        print(temp, end =" ")

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    string = input("Enter what you want to transmit: ")
    converting_to_binray(string)

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
